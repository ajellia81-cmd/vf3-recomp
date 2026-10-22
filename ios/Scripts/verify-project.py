#!/usr/bin/env python3
import json,pathlib,subprocess
root=pathlib.Path(__file__).resolve().parents[1]
d=json.loads(subprocess.check_output(['plutil','-convert','json','-o','-',str(root/'VF3Recomp.xcodeproj/project.pbxproj')]))
objects=d['objects'];app=next(v for v in objects.values() if v.get('isa')=='PBXNativeTarget' and v['name']=='VF3Recomp')
sources=[]
for phase in app['buildPhases']:
 p=objects[phase]
 if p['isa']=='PBXSourcesBuildPhase':
  for f in p['files']:
   ref=objects[objects[f]['fileRef']];sources.append(pathlib.PurePosixPath(ref.get('path',ref.get('name',''))).name)
for required in ['main.cpp','ppc.cpp','ppc_rec_stub.cpp','r3d_gpu.cpp','VF3Platform.mm','VF3ROMImport.mm','VF3InputBridge.mm']:
 assert sources.count(required)==1,(required,sources.count(required))
for forbidden in ['ppc_rec.cpp','vf3_map.cpp','sljitLir.c']:
 assert forbidden not in sources,forbidden
assert 'VF3_DISABLE_JIT=1' in (root/'Config/Debug.xcconfig').read_text()
print('PASS: iOS source selection excludes all executable-memory recompiler sources')
