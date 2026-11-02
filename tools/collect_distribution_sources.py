"""Collect pinned upstream sources without executing package recipes."""
import concurrent.futures
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tarfile
import time
import urllib.parse
import urllib.request
import zipfile

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'work/source-distribution'
OUT.mkdir(parents=True,exist_ok=True)
manifest=json.loads((ROOT/'dependencies/source-distribution.json').read_text(encoding='utf-8'))
allowed={'repo.msys2.org','mirror.msys2.org','github.com','codeload.github.com','git.eden-emu.dev'}

def download(row):
    parsed=urllib.parse.urlparse(row['url'])
    assert parsed.scheme=='https' and parsed.hostname in allowed,row['url']
    assert Path(row['filename']).name==row['filename']
    target=OUT/'archives'/row['filename']
    target.parent.mkdir(exist_ok=True)
    def valid():
        return target.is_file() and hashlib.file_digest(target.open('rb'),row['algorithm']).hexdigest()==row['hash']
    if not valid():
        for attempt in range(3):
            try:
                request=urllib.request.Request(row['url'],headers={'User-Agent':'Eglantine-Source-Distribution/1.0'})
                with urllib.request.urlopen(request,timeout=45) as response,target.with_suffix('.part').open('wb') as output:
                    shutil.copyfileobj(response,output,1024*1024)
                assert hashlib.file_digest(target.with_suffix('.part').open('rb'),row['algorithm']).hexdigest()==row['hash'],target.name
                target.with_suffix('.part').replace(target)
                break
            except Exception:
                if attempt==2:raise
                time.sleep(1)
    assert valid(),target.name
    if row.get('signature_sha256'):
        signature=target.with_name(target.name+'.sig')
        with urllib.request.urlopen(row['url']+'.sig',timeout=45) as response:signature.write_bytes(response.read())
        assert hashlib.sha256(signature.read_bytes()).hexdigest()==row['signature_sha256']
        subprocess.run(['gpgv','--homedir',str(OUT/'gnupg'),'--keyring',str(OUT/'msys2.gpg'),str(signature),str(target)],check=True)
    print('Verified source:',target.name,flush=True)
    return dict(file=target.name,sha256=hashlib.file_digest(target.open('rb'),'sha256').hexdigest(),bytes=target.stat().st_size)

keyring=ROOT/'dependencies/msys2-distribution-keyring.asc'
assert hashlib.sha256(keyring.read_bytes()).hexdigest()==manifest['keyring_sha256']
(OUT/'gnupg').mkdir(mode=0o700,exist_ok=True)
subprocess.run(['gpg','--homedir',str(OUT/'gnupg'),'--batch','--yes','--dearmor','--output',str(OUT/'msys2.gpg'),str(keyring)],check=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:records=list(pool.map(download,manifest['archives']))

upstream=OUT/'upstream'
if not upstream.exists():
    subprocess.run(['git','init',str(upstream)],check=True)
    subprocess.run(['git','-C',str(upstream),'fetch','--depth=1','https://github.com/Djihads80/Eglantine.git',manifest['base']],check=True)
subprocess.run(['python3',str(ROOT/'tools/verify_source.py'),'--upstream',str(upstream)],check=True)
tar_path=OUT/'base.tar'
subprocess.run(['git','-C',str(upstream),'archive','--format=tar','--output='+str(tar_path),manifest['base']],check=True)
prepared=OUT/'Eglantine'
prepared.mkdir(exist_ok=True)
with tarfile.open(tar_path) as tar:tar.extractall(prepared,filter='data')
source_manifest=json.loads((ROOT/'SOURCE-MANIFEST.json').read_text(encoding='utf-8'))
for name in source_manifest['modified_paths']:
    target=(prepared/name).resolve()
    assert target.is_relative_to(prepared.resolve())
    target.parent.mkdir(parents=True,exist_ok=True)
    shutil.copy2(ROOT/'modified'/name,target)

# This generated dataset uses a Git submodule; include that exact submodule too.
timezone=OUT/'timezone-generator'
if not timezone.exists():
    subprocess.run(['git','clone','--depth=1','--branch','230326','--recurse-submodules',
                    '--shallow-submodules','https://git.eden-emu.dev/eden-emu/tzdb_to_nx.git',str(timezone)],check=True)
timezone_commit=subprocess.check_output(['git','-C',str(timezone/'externals/tz/tz'),'rev-parse','HEAD'],text=True).strip()
receipt=dict(base=manifest['base'],archives=records,timezone_submodule=timezone_commit,
             recipes_executed=False,msys_signatures_verified_with_gpgv=True)
(OUT/'SOURCE-RECEIPT.json').write_text(json.dumps(receipt,indent=2),encoding='utf-8')

destination=ROOT/'Eglantine-3060Ti-16GB-corresponding-sources.zip'
with zipfile.ZipFile(destination,'w',zipfile.ZIP_DEFLATED,compresslevel=6) as archive:
    for folder,prefix in [(prepared,'Eglantine'),(timezone,'timezone-generator'),(OUT/'archives','upstream-archives')]:
        for p in folder.rglob('*'):
            if p.is_file() and '.git' not in p.relative_to(folder).parts:
                compression=zipfile.ZIP_STORED if p.suffix in {'.gz','.xz','.zst','.zip'} else zipfile.ZIP_DEFLATED
                archive.write(p,prefix+'/'+p.relative_to(folder).as_posix(),compress_type=compression)
    tracked=subprocess.check_output(['git','-C',str(ROOT),'ls-files','-z']).decode('utf-8').split('\0')
    for name in filter(None,tracked):archive.write(ROOT/name,'release-project/'+name)
    archive.write(OUT/'SOURCE-RECEIPT.json','SOURCE-RECEIPT.json')
assert destination.stat().st_size<1_900_000_000,'Split source archive before release'
digest=hashlib.file_digest(destination.open('rb'),'sha256').hexdigest()
(ROOT/'SOURCE-SHA256SUMS.txt').write_text(digest+'  '+destination.name+'\n',encoding='utf-8')
print(destination.name,digest,flush=True)
