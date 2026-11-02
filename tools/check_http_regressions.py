"""Run a loopback-only positive test and a vulnerable-version negative control."""
import hashlib
import io
import json
from pathlib import Path
import re
import subprocess
import tarfile
import urllib.request

ROOT=Path(__file__).resolve().parents[1]
WORK=ROOT/'work/http-security'
WORK.mkdir(parents=True,exist_ok=True)
manifest=json.loads((ROOT/'SOURCE-MANIFEST.json').read_text(encoding='utf-8'))
include=WORK/'include/common'
include.mkdir(parents=True,exist_ok=True)
header=subprocess.check_output(['git','-C',str(ROOT/'work/upstream'),'show',manifest['base']+':src/common/httplib.h'])
(include/'httplib.h').write_bytes(header)
versions={
    'v0.46.0':'159ed94965018f2a371d45a3bfc1961e5fb1549e501ded70a6b4532d7fe99d0579c18b5195aff6e35f96f399b426cea2650ec9fb75ef80d4c9edeccb51f2e6c9',
    'v0.51.0':'da2e205dcd1127685b7f2ff1e2d5b213f08489942e1f8d97be7f4d4bf0019be2bf48b260bc463c5861d5d537e0eaf648d28953f7a9fdae880005c7bf5319cfbc',
}
results={}
for version,expected in versions.items():
    url='https://github.com/yhirose/cpp-httplib/archive/'+version+'.tar.gz'
    with urllib.request.urlopen(url,timeout=45) as response:data=response.read()
    assert hashlib.sha512(data).hexdigest()==expected,'Source checksum mismatch: '+version
    source=WORK/version
    source.mkdir(exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(data)) as archive:archive.extractall(source,filter='data')
    library=next(source.glob('*/httplib.h')).parent
    executable=WORK/('http-test-'+version)
    subprocess.run(['g++','-std=c++20','-O1','-I'+str(WORK/'include'),'-I'+str(library),
                    str(ROOT/'tests/http_security_test.cpp'),'-lssl','-lcrypto','-lpthread','-o',str(executable)],check=True)
    result=subprocess.run([str(executable)],capture_output=True,text=True,timeout=30)
    output=result.stdout+result.stderr
    print(version,output.strip(),flush=True)
    results[version]=dict(exit_code=result.returncode,output=output)
    if version=='v0.46.0':
        assert result.returncode!=0 and re.search(r'(?:cookie|injected)=1',output),'Negative control did not demonstrate the issue'
    else:assert result.returncode==0,'Fixed version failed regression test'
(WORK/'results.json').write_text(json.dumps(results,indent=2),encoding='utf-8')
