// Assemble a fresh portable directory and verify its packaged PE dependencies.
// This module reads files; it never executes a binary or deletes an existing directory.
import fs from 'node:fs/promises';
import path from 'node:path';
import crypto from 'node:crypto';

const sha256 = data => crypto.createHash('sha256').update(data).digest('hex');
const requireTrue = (condition, message) => { if (!condition) throw new Error(message); };

export function readPE(data) {
  const bounds = (offset, size) => requireTrue(Number.isSafeInteger(offset) && offset >= 0 && offset + size <= data.length, 'Invalid PE bounds');
  const u16 = offset => { bounds(offset,2); return data.readUInt16LE(offset); };
  const u32 = offset => { bounds(offset,4); return data.readUInt32LE(offset); };
  bounds(0x3c,4);
  requireTrue(data.toString('ascii',0,2)==='MZ','Missing DOS header');
  const pe=u32(0x3c);
  requireTrue(u32(pe)===0x4550 && u16(pe+4)===0x8664,'Expected x86-64 PE');
  const optional=pe+24;
  requireTrue(u16(optional)===0x20b,'Expected PE32+');
  const sectionOffset=optional+u16(pe+20);
  const sections=[];
  for(let i=0;i<u16(pe+6);i++) {
    const s=sectionOffset+i*40;
    sections.push({address:u32(s+12),length:Math.max(u32(s+8),u32(s+16)),raw:u32(s+20)});
  }
  const rva = address => {
    const section=sections.find(s=>address>=s.address && address<s.address+s.length);
    if(section) { const offset=section.raw+address-section.address; bounds(offset,1);return offset; }
    requireTrue(address<u32(optional+60),'Unmapped PE RVA');bounds(address,1);return address;
  };
  const string = address => {
    const offset=rva(address),end=data.indexOf(0,offset);
    requireTrue(end>=offset && end-offset<32768,'Unterminated PE name');
    return data.toString('utf8',offset,end);
  };
  const imports=[];
  const addImports = (address,delayed=false) => {
    if(!address)return;
    const start=rva(address),stride=delayed?32:20;
    for(let i=0;i<4096;i++) {
      const d=start+i*stride;bounds(d,stride);
      const nameAddress=u32(d+(delayed?4:12));
      if(!nameAddress)break;
      if(delayed)requireTrue((u32(d)&1)!==0,'Unsupported VA-based delay imports');
      const module=string(nameAddress);
      const thunk=u32(d+(delayed?16:0))||u32(d+(delayed?12:16));
      const entries=rva(thunk),symbols=[];
      for(let j=0;j<100000;j++) {
        const offset=entries+j*8;bounds(offset,8);
        const value=data.readBigUInt64LE(offset);
        if(!value)break;
        symbols.push((value&(1n<<63n)) ? {ordinal:Number(value&65535n)} : {name:string(Number(value)+2)});
      }
      imports.push({module,symbols});
    }
  };
  addImports(u32(optional+112+8));
  if(u32(optional+108)>13)addImports(u32(optional+112+13*8),true);
  const names=new Set(),ordinals=new Set();
  const exportAddress=u32(optional+112);
  if(exportAddress) {
    const e=rva(exportAddress),base=u32(e+16),functions=u32(e+20),count=u32(e+24);
    const functionArray=rva(u32(e+28));
    requireTrue(functions<100000 && count<100000,'Excessive PE export count');
    for(let i=0;i<functions;i++)if(u32(functionArray+i*4))ordinals.add(base+i);
    if(count) {
      const nameArray=rva(u32(e+32));
      for(let i=0;i<count;i++)names.add(string(u32(nameArray+i*4)));
    }
  }
  const characteristics=u16(optional+70);
  return {imports,names,ordinals,aslr:!!(characteristics&0x40),dep:!!(characteristics&0x100)};
}

async function files(folder,prefix='') {
  const result=[];
  for(const entry of await fs.readdir(folder,{withFileTypes:true})) {
    const relative=prefix?prefix+'/'+entry.name:entry.name;
    requireTrue(!entry.isSymbolicLink(),'Symlinks are not accepted in portable inputs');
    if(entry.isDirectory())result.push(...await files(path.join(folder,entry.name),relative));
    else if(entry.isFile())result.push(relative);
  }
  return result;
}

export async function assembleRuntime(options) {
  const base=path.resolve(options.base),output=path.resolve(options.output);
  requireTrue(output!==base,'Use a fresh output directory');
  try { await fs.stat(output); throw new Error('Output already exists'); } catch(error) { if(error.code!=='ENOENT')throw error; }
  const userFiles=await files(path.join(base,'user'));
  requireTrue(userFiles.length===1 && userFiles[0]==='config/qt-config.ini','Base contains personal user data');
  const inputs=new Map(),roots=[];
  for(const relative of await files(base)) {
    if(!relative.toLowerCase().endsWith('.dll') || relative.startsWith('user/') || relative.startsWith('licenses/'))continue;
    const key=path.basename(relative).toLowerCase();
    requireTrue(!inputs.has(key),'Ambiguous DLL basename: '+key);
    inputs.set(key,{relative,file:path.join(base,relative)});
    if(relative.includes('/'))roots.push(key);
  }
  for(const name of ['eglantine.exe','eden-cli.exe']) {
    inputs.set(name,{relative:name,file:path.join(options.binaries,name)});roots.push(name);
  }
  for(const name of ['avcodec-63.dll','avutil-61.dll','swresample-7.dll','swscale-10.dll']) {
    inputs.set(name,{relative:name,file:path.join(options.ffmpeg,name)});roots.push(name);
  }
  inputs.set('zlib1.dll',{relative:'zlib1.dll',file:path.resolve(options.zlib)});
  const included=new Map(),queue=[...roots];
  const windows=options.windows||'C:/Windows';
  while(queue.length) {
    const key=queue.pop().toLowerCase();
    if(included.has(key))continue;
    const item=inputs.get(key);requireTrue(item,'Missing packaged input: '+key);
    const data=await fs.readFile(item.file),pe=readPE(data);
    requireTrue(pe.aslr && pe.dep,'Missing expected PE hardening marks: '+item.relative);
    included.set(key,{...item,data,pe,sha256:sha256(data)});
    for(const imported of pe.imports) {
      const dependency=imported.module.toLowerCase();
      if(inputs.has(dependency))queue.push(dependency);
      else if(!dependency.startsWith('api-ms-win-')&&!dependency.startsWith('ext-ms-win-'))
        await fs.access(path.join(windows,'System32',imported.module));
    }
  }
  const unresolved=[];
  for(const item of included.values())for(const imported of item.pe.imports) {
    const target=included.get(imported.module.toLowerCase());if(!target)continue;
    for(const symbol of imported.symbols)
      if(symbol.name ? !target.pe.names.has(symbol.name) : !target.pe.ordinals.has(symbol.ordinal))
        unresolved.push({file:item.relative,module:imported.module,symbol});
  }
  requireTrue(!unresolved.length,'Packaged export mismatch: '+JSON.stringify(unresolved));
  await fs.mkdir(output,{recursive:true});
  for(const item of included.values()) {
    const target=path.join(output,item.relative);await fs.mkdir(path.dirname(target),{recursive:true});
    await fs.writeFile(target,item.data);
  }
  await fs.cp(path.join(base,'licenses'),path.join(output,'licenses'),{recursive:true});
  await fs.mkdir(path.join(output,'user/config'),{recursive:true});
  await fs.copyFile(path.join(base,'user/config/qt-config.ini'),path.join(output,'user/config/qt-config.ini'));
  await fs.writeFile(path.join(output,'qt.conf'),'[Paths]\nPlugins=.\n');
  let launcher=await fs.readFile(path.join(base,'Iniciar.cmd'),'utf8');
  launcher=launcher.replace(/start ""/,'set "PATH=%SystemRoot%\\System32;%SystemRoot%"\nstart ""');
  await fs.writeFile(path.join(output,'Iniciar.cmd'),launcher.replace(/\r?\n/g,'\r\n'));
  for(const name of ['LICENSE.txt','CREDITS.md','SECURITY.md','SECURITY-REVIEW.md','RELEASE-STATUS.json'])
    await fs.copyFile(path.join(options.documentation,name),path.join(output,name));
  await fs.copyFile(path.join(options.documentation,'README.md'),path.join(output,'LEEME.md'));
  const receipt={files:[...included.values()].map(x=>({path:x.relative,sha256:x.sha256,aslr:x.pe.aslr,dep:x.pe.dep})),
    dlls:[...included.keys()].filter(x=>x.endsWith('.dll')).length,unresolved_packaged_symbols:unresolved,
    removed_dlls:[...inputs.keys()].filter(x=>x.endsWith('.dll')&&!included.has(x)),
    runtime_execution_validated:false,antivirus_of_this_directory:'pending',binary_release_ready:false};
  await fs.writeFile(path.join(output,'RUNTIME-RECEIPT.json'),JSON.stringify(receipt,null,2));
  return receipt;
}
