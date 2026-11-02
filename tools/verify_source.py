"""Verify public patches against their immutable upstream base; never build untrusted binaries."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser()
parser.add_argument('--upstream', type=Path, required=True)
parser.add_argument('--git', default='git')
args = parser.parse_args()
manifest = json.loads((ROOT/'SOURCE-MANIFEST.json').read_text(encoding='utf-8'))

def git(directory, *arguments, **kwargs):
    return subprocess.run([args.git,'-C',str(directory),'-c','core.autocrlf=false',
                           *arguments],check=True,**kwargs)

for name, expected in manifest['sha256'].items():
    p = (ROOT/name).resolve()
    assert p.is_relative_to(ROOT) and p.is_file(), name
    assert hashlib.sha256(p.read_bytes().replace(b'\r\n',b'\n')).hexdigest() == expected, name

scratch = ROOT/'work'
scratch.mkdir(exist_ok=True)
with tempfile.TemporaryDirectory(prefix='eglantine-source-check-',dir=scratch) as temp:
    fixture = Path(temp).resolve()
    assert fixture.is_relative_to(scratch.resolve())
    for name in manifest['modified_paths']:
        assert (name.startswith('src/') or name == 'cpmfile.json') and '..' not in Path(name).parts
        if name in manifest['new_paths']:
            continue
        content = git(args.upstream,'show',f"{manifest['base']}:{name}",capture_output=True).stdout
        p = fixture/name
        p.parent.mkdir(parents=True,exist_ok=True)
        p.write_bytes(content.replace(b'\r\n',b'\n'))
    git(fixture,'init','-q')
    git(fixture,'add','src')
    for patch in manifest['patches']:
        git(fixture,'apply','--check',str(ROOT/patch))
        git(fixture,'apply',str(ROOT/patch))
    git(fixture,'diff','--check')
    for name in manifest['modified_paths']:
        expected = (ROOT/'modified'/name).read_bytes().replace(b'\r\n',b'\n')
        assert (fixture/name).read_bytes().replace(b'\r\n',b'\n') == expected, name

for p in ROOT.rglob('*'):
    if not p.is_file() or any(part in {'.git','Eglantine','work','build'} for part in p.relative_to(ROOT).parts):
        continue
    assert p.suffix.lower() not in {'.exe','.dll','.keys','.nsp','.xci','.log'}, p
    if p.suffix.lower() in {'.md','.ini','.json','.txt','.sh','.py','.patch','.cpp','.h','.yml'}:
        text=p.read_text(encoding='utf-8-sig',errors='strict')
        assert not re.search(r'C:[/\\]Users[/\\]isacm',text,re.I), p
        assert not re.search(r'(?:ghp_|github_pat_)[A-Za-z0-9_]{20,}',text), p
        assert ('-----BEGIN '+'PRIVATE KEY-----') not in text, p
print(f"PASS: {len(manifest['modified_paths'])} source files, patches, hashes and public file checks")
