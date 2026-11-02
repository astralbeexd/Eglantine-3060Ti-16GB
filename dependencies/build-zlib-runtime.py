"""Build the reviewed zlib snapshot with its upstream tests on Windows/MSYS2 UCRT64."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tarfile
import urllib.request

ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser()
parser.add_argument('--toolchain',type=Path,default=Path('C:/msys64/ucrt64/bin'))
parser.add_argument('--work',type=Path,default=ROOT/'work/security-zlib')
args=parser.parse_args()
args.work.mkdir(parents=True,exist_ok=True)
commit='6be4bcda3da1d28b18a081c884b141a88a7cb84d'
expected='791d3769f92102b4b83a790971d400c8440e0cbe4823276319a1bdad32551a15'
archive=args.work/'zlib-source.tar.gz'
url='https://codeload.github.com/madler/zlib/tar.gz/'+commit
if not archive.exists():
    with urllib.request.urlopen(url,timeout=60) as response:archive.write_bytes(response.read())
assert hashlib.file_digest(archive.open('rb'),'sha256').hexdigest()==expected,'Source checksum mismatch'
source=args.work/('zlib-'+commit)
if not source.exists():
    with tarfile.open(archive) as tar:tar.extractall(args.work,filter='data')
build=args.work/'build'
env=os.environ.copy()
env['PATH']=str(args.toolchain)+';'+env.get('PATH','')
cmake=str(args.toolchain/'cmake.exe')
subprocess.run([cmake,'-S',str(source),'-B',str(build),'-G','Ninja',
    '-DCMAKE_BUILD_TYPE=Release','-DCMAKE_C_COMPILER=clang',
    '-DCMAKE_SHARED_LINKER_FLAGS=-fuse-ld=lld','-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld',
    '-DZLIB_BUILD_SHARED=ON','-DZLIB_BUILD_STATIC=OFF','-DZLIB_BUILD_TESTING=ON',
    '-DCMAKE_C_FLAGS=-O2 -D_LARGEFILE64_SOURCE=1 -D_FILE_OFFSET_BITS=64'],env=env,check=True)
subprocess.run([cmake,'--build',str(build),'--parallel','2'],env=env,check=True)
subprocess.run([str(args.toolchain/'ctest.exe'),'--test-dir',str(build),'--output-on-failure'],env=env,check=True)
receipt=dict(upstream='https://github.com/madler/zlib',commit=commit,source_url=url,
    source_sha256=expected,tests='upstream CTest suite passed',
    fixed_commit='df84af25dc1942490e1d1c899a07619152a46148',
    cves=['CVE-2026-76844','CVE-2026-85091'],
    binaries={p.name:hashlib.file_digest(p.open('rb'),'sha256').hexdigest() for p in build.glob('*.dll')})
(args.work/'build-receipt.json').write_text(json.dumps(receipt,indent=2),encoding='utf-8')
print(json.dumps(receipt,indent=2))
