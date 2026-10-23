#include "../../src/ppc/ppc.h"
#include <cassert>
#include <fstream>
#include <iterator>
#include <string>
static void put(std::vector<uint8_t> &ram,unsigned a,unsigned v) {
 ram[a^3]=uint8_t(v>>8);ram[(a+1)^3]=uint8_t(v);
}
int main() {
 using namespace vf3_diagnostics;
 std::vector<uint8_t> ram(0x800000);
 put(ram,0x100,200);put(ram,0x102,150);put(ram,0x200,200);
 Probe p;assert(p.searches[0].start(ram));assert(p.searches[0].candidates.size()==3);
 put(ram,0x100,170);p.searches[0].refine(ram,true);assert(p.searches[0].candidates.size()==1);
 p.searches[0].refine(ram,false);p.update_watches();assert(p.watched()==1);
 p.frame=50;p.phase=Phase::live;
 // Observe before mutation: a full word writes both 16-bit guest halves.
 p.observe(ram.data(),0x100,0x008c0096,4,0x1234);assert(p.size()==1);assert(health_word(ram.data(),0x100)==170);
 ppc::Cpu cpu;cpu.attach(nullptr,ram.data(),nullptr);cpu.combat_probe=&p;
 cpu.s.r[4]=0x100;cpu.s.r[3]=140;cpu.s.pc=0x2000;
 cpu.exec_op((44u<<26)|(3u<<21)|(4u<<16)); // synthetic sth r3,0(r4)
 assert(p.size()==2 && health_word(ram.data(),0x100)==140);
 cpu.s.r[3]=0;cpu.exec_op((38u<<26)|(3u<<21)|(4u<<16)|1u); // stb low byte
 assert(p.size()==3 && health_word(ram.data(),0x100)==0);
 p.phase=Phase::post_ko;cpu.exec_op((36u<<26)|(3u<<21)|(4u<<16)); // zero-to-zero stw
 assert(p.size()==4);
 cpu.s.f[3].i=uint64_t(100)<<48;cpu.exec_op((54u<<26)|(3u<<21)|(4u<<16)); // stfd
 assert(p.size()==5 && health_word(ram.data(),0x100)==100);
 // Partial high-byte update, out-of-range and unrelated writes stay bounded.
 p.observe(ram.data(),0x100,0,1,0x1238);assert(p.size()==5);
 p.observe(ram.data(),0x7fffff,0,8,0);p.observe(ram.data(),0x300,0,4,0);assert(p.size()==5);
 put(ram,0x100,0);
 for(int i=0;i<5000;++i) p.observe(ram.data(),0x100,0,2,0x3000);
 assert(p.size()==4096);assert(p.save("/tmp/vf3-combat-check.csv",ram));
 std::ifstream f("/tmp/vf3-combat-check.csv");std::string csv((std::istreambuf_iterator<char>(f)),{});
 assert(csv.find("post_ko")!=std::string::npos && csv.find("00000100")!=std::string::npos);
 assert(csv.find("00000300")==std::string::npos);std::remove("/tmp/vf3-combat-check.csv");
 p.clear();assert(!p.watched() && !p.size());
 // Never silently truncate a search: report overflow and arm no watches.
 for(unsigned a=0;a<ram.size();a+=2) put(ram,a,100);
 assert(!p.searches[1].start(ram) && p.searches[1].overflow);p.update_watches();assert(!p.watched());
 puts("PASS: scalar candidate narrowing, guest byte order, actual interpreter 8/16/32/64 stores, ring bounds, CSV and overflow");
}
