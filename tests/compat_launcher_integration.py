"""AI-assisted launcher integration test; executes a tiny fixture, never a game."""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

parser=argparse.ArgumentParser()
parser.add_argument('--launcher',required=True,type=Path)
parser.add_argument('--child',required=True,type=Path)
args=parser.parse_args()
assert os.name=='nt'
with tempfile.TemporaryDirectory(prefix='eglantine-revision-\u00e1 ') as folder:
    root=Path(folder)
    launcher=root/'Iniciar-compatible.exe'
    shutil.copyfile(args.launcher,launcher)
    shutil.copyfile(args.child,root/'eglantine.exe')
    env=os.environ.copy()
    env.update(QT_PLUGIN_PATH='invalid-plugin-directory',QT_QPA_PLATFORM_PLUGIN_PATH='invalid-platform-directory',
               EDEN_NX2_TEST_UNKNOWN='must-not-leak',EDEN_NX2_SMALL_VRAM_PROFILE='1',PATH='invalid-search-path')
    subprocess.run([str(launcher),'archivo con espacios \u00e1.nsp','--prueba'],cwd=root.parent,
                   env=env,check=True,timeout=15)
    assert (root/'launcher-child-ok.txt').read_text().startswith('PASS:')
    (root/'launcher-child-ok.txt').unlink()
    # Also exercise missing variables; deleting an absent variable must be harmless.
    clean_env={key:value for key,value in os.environ.items()
               if not key.upper().startswith(('QT_','EDEN_NX2_'))}
    subprocess.run([str(launcher),'archivo con espacios \u00e1.nsp','--prueba'],cwd=root.parent,
                   env=clean_env,check=True,timeout=15)
    assert (root/'launcher-child-ok.txt').read_text().startswith('PASS:')
    result=subprocess.run([str(launcher),'--check'],env=env,check=True,capture_output=True,text=True,timeout=10)
    assert 'cpu_x86_64_v2=1' in result.stdout and 'os_target_ok=1' in result.stdout
    (root/'launcher-child-ok.txt').unlink()
    env['EDEN_NX2_PIPELINE_WORKERS']='64'
    subprocess.run([str(launcher),'--perfil-4gb','archivo con espacios \u00e1.nsp','--prueba','--espera-4gb'],
                   cwd=root.parent,env=env,check=True,timeout=15)
    assert (root/'launcher-child-ok.txt').read_text().startswith('PASS:')
    result=subprocess.run([str(launcher),'--perfil-4gb','--check'],env=env,check=True,capture_output=True,text=True,timeout=10)
    assert 'profile_4gb=1' in result.stdout
print('PASS: real child process, quoted Unicode arguments, working directory, environment and preflight')
