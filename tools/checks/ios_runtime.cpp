#include "../../src/ppc/ppc.h"
#include "../../src/rec/ppc_rec.h"
#include "../../src/machine/romset.h"
#include <cassert>
#include <cstdio>
int main(){
 ppc::Cpu cpu; ppcrec::Rec rec; ppcrec::Stats stats;
 assert(!rec.build(cpu,{},&stats));assert(!rec.run(cpu));assert(!rec.enabled());
 assert(!romset::load("/no-such-user-archive/vf3.zip").ok);
 puts("PASS: executable-memory recompiler disabled; missing user archive rejected without ROM data");
}
