#include "../../src/ppc/ppc.h"
#include <cassert>
#include <cmath>
#include <cstdio>
#include <vector>
static void put(std::vector<uint8_t>& r,unsigned a,unsigned v) {r[a^3]=uint8_t(v>>8);r[(a+1)^3]=uint8_t(v);}
int main() {
 constexpr unsigned hp[2]={0x1087d6,0x10a7d6};
 std::vector<uint8_t> ram(0x800000);
 vf3_feedback::Contacts contacts;
 ppc::Cpu cpu;cpu.attach(nullptr,ram.data(),nullptr);cpu.combat_feedback=&contacts;
 auto store=[&](unsigned a,unsigned v,unsigned pc=0x104bc,unsigned op=44) {
  cpu.s.r[4]=a;cpu.s.r[3]=v;cpu.s.pc=pc;
  cpu.exec_op((op<<26)|(3u<<21)|(4u<<16));
 };
 auto refill=[&](uint64_t frame,unsigned maximum=181) {
  contacts.tick(frame);
  store(hp[0],maximum,0x09cf0);store(hp[1],maximum,0x09cf0);
  assert(contacts.active());
 };
 // No boot feedback, no smoothing trigger and no equal-HP polling rearm.
 assert(!contacts.snapshot(ram,0).live);
 for(auto a:hp)put(ram,a,181);
 store(hp[0],181,0x3fdb4);
 assert(!contacts.snapshot(ram,1).live);
 // First known full-bar refill, then subsequent rounds, all automatic.
 for(int maximum: {120,181,200,240})for(unsigned victim=0;victim<2;++victim)
 for(unsigned selected: {1u,2u,3u})for(int gain: {0,25,75,100}) {
  contacts.reset();fight_haptics::Tracker tracker;
  for(uint64_t round=0;round<3;++round) {
   const uint64_t base=round*100;
   refill(base,unsigned(maximum));
   auto baseline=contacts.snapshot(ram,base);
   assert(baseline.live && baseline.maximum==maximum);
   tracker.advance(baseline,selected,gain);
   contacts.tick(base+1);store(hp[victim],maximum-5);
   auto weak=tracker.advance(contacts.snapshot(ram,base+1),selected,gain);
   contacts.tick(base+2);store(hp[victim],maximum-45);
   auto strong=tracker.advance(contacts.snapshot(ram,base+2),selected,gain);
   for(unsigned p=0;p<2;++p) {
    if(!(selected&(1u<<p))) {assert(!weak[p].ms && !strong[p].ms);continue;}
    assert(weak[p].ms && strong[p].ms);
    if(gain)assert(strong[p].low>weak[p].low);
   }
   contacts.tick(base+3);store(hp[victim],0);
   auto ko=tracker.advance(contacts.snapshot(ram,base+3),selected,gain);
   for(unsigned p=0;p<2;++p)if(selected&(1u<<p))assert(ko[p].knockout);
   contacts.tick(base+4);store(hp[victim],0);
   auto current=contacts.snapshot(ram,base+4);
   auto post=tracker.advance(current,selected,gain);
   const float g=gain/100.0f;
   for(unsigned p=0;p<2;++p) {
    if(!(selected&(1u<<p))) {assert(!post[p].ms);continue;}
    assert(!post[p].knockout);
    assert(std::fabs(post[p].low-(p==victim?0.90f:0.52f)*g)<0.00001f);
    assert(std::fabs(post[p].high-(p==victim?0.65f:0.90f)*g)<0.00001f);
    assert(post[p].ms==(p==victim?100u:65u));
   }
   assert(!tracker.advance(current,selected,gain)[0].ms);
   contacts.tick(base+5);store(hp[victim],4,0x3fdb4);
   auto replay=tracker.advance(contacts.snapshot(ram,base+5),selected,gain);
   assert(!contacts.active() && !replay[0].ms && !replay[1].ms);
   // Even replay's zero writes and full-looking restoration cannot re-arm.
   store(hp[victim],0,0x2a160);store(hp[victim],0,0x2a1b8);
   for(auto a:hp)store(a,maximum,0x3fdb4);
   assert(!contacts.snapshot(ram,base+6).live);
   store(hp[victim],maximum-5);assert(!contacts.active());
  }
 }
 contacts.reset();contacts.tick(0);
 store(hp[0],181,0x09cf0);assert(!contacts.active());
 contacts.tick(4);store(hp[1],181,0x09cf0);assert(!contacts.active());
 contacts.reset();contacts.tick(0);
 store(hp[0],181,0x09cf0);store(hp[1],180,0x09cf0);assert(!contacts.active());
 refill(10);store(0x10a408,100,0x8b6c);assert(contacts.active());
 for(unsigned pc: {0x3fdb4u,0x29474u,0x29478u,0x1234u}) {
  refill(10);store(hp[0],160,pc);assert(!contacts.active());
 }
 for(unsigned op: {38u,36u}) {
  refill(10);store(op==36?(hp[0]&~3u):hp[0],0,0x104bc,op);assert(!contacts.active());
 }
 refill(10);store(hp[0],182);assert(!contacts.active());
 refill(10);contacts.tick(7210);assert(!contacts.active());
 refill(7220);contacts.tick(0);assert(!contacts.active());
 refill(10);contacts.reset();assert(!contacts.active());
 std::vector<uint8_t> tiny(16);assert(!contacts.snapshot(tiny,10).live);
 puts("PASS: automatic multi-round refill/replay/refill sequences, no manual arming or label dependence");
 puts("PASS: actual interpreter normal/KO/max-post-KO contacts, both victims, selections, gains and health references");
 puts("PASS: replay/setup/display exclusion, incomplete refill, unknown/invalid writes, timeout/reset and duplicate suppression");
}
