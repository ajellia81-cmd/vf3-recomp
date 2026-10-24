#include "../../src/ppc/ppc.h"
#include <cassert>
#include <cstdio>
#include <vector>
static void put(std::vector<uint8_t>& r,unsigned a,unsigned v) {r[a^3]=uint8_t(v>>8);r[(a+1)^3]=uint8_t(v);}
int main() {
 constexpr unsigned hp[2]={0x1087d6,0x10a7d6};
 std::vector<uint8_t> ram(0x800000);
 vf3_feedback::Contacts contacts;
 ppc::Cpu cpu;cpu.attach(nullptr,ram.data(),nullptr);cpu.combat_feedback=&contacts;
 auto full=[&]{for(auto a:hp)put(ram,a,181);};
 auto store=[&](unsigned a,unsigned v,unsigned pc=0x104bc,unsigned op=44) {
  cpu.s.r[4]=a;cpu.s.r[3]=v;cpu.s.pc=pc;
  cpu.exec_op((op<<26)|(3u<<21)|(4u<<16));
 };
 full();store(hp[0],160);assert(!contacts.armed()); // default off
 full();assert(contacts.arm(ram,0));
 auto s=contacts.snapshot(ram,0);assert(s.live && s.maximum==181);
 // Both victims route to each human selection; stronger damage increases response.
 for(unsigned victim=0;victim<2;++victim)for(unsigned selected=1;selected<=3;++selected) {
  float weak=0;
  for(unsigned damage: {5u,40u}) {
   full();assert(contacts.arm(ram,10));
   fight_haptics::Tracker tracker;tracker.advance(contacts.snapshot(ram,10),selected,100);
   store(hp[victim],181-damage);
   auto current=contacts.snapshot(ram,11);
   assert(current.damage_total[victim]==damage && current.contact_total[victim]==1);
   auto pulse=tracker.advance(current,selected,100);
   for(unsigned p=0;p<2;++p)assert(bool(pulse[p].ms)==bool(selected&(1u<<p)));
   float sum=pulse[0].low+pulse[1].low;
   if(damage==5)weak=sum;else assert(sum>weak);
   assert(!tracker.advance(current,selected,100)[0].ms);
   auto unchanged=tracker.advance(contacts.snapshot(ram,12),selected,100);
   assert(!unchanged[0].ms && !unchanged[1].ms);
  }
 }
 for(unsigned victim=0;victim<2;++victim) {
  full();assert(contacts.arm(ram,20));fight_haptics::Tracker tracker;
  tracker.advance(contacts.snapshot(ram,20),3,100);
  store(hp[victim],0);
  auto ko=tracker.advance(contacts.snapshot(ram,21),3,100);
  assert(ko[0].knockout && ko[1].knockout);
  store(hp[victim],0);
  auto post=tracker.advance(contacts.snapshot(ram,22),3,100);
  assert(post[0].ms && post[1].ms && !post[0].knockout && !post[1].knockout);
  assert(std::fabs(post[victim].low-0.90f)<0.00001f && post[victim].ms==100);
  assert(std::fabs(post[victim^1].high-0.90f)<0.00001f && post[victim^1].ms==65);
  // A repeated zero at another writer ends the round preview, never a pulse.
  store(hp[victim],0,0x3fdb4);
  auto replay=tracker.advance(contacts.snapshot(ram,23),3,100);
  assert(!contacts.armed() && !replay[0].ms && !replay[1].ms);
 }
 // Ignore display animation, no HP-only fallback, and manual labels cannot arm.
 full();assert(contacts.arm(ram,30));
 store(0x10a408,100,0x8b6c);assert(contacts.armed());
 fight_haptics::Tracker tracker;tracker.advance(contacts.snapshot(ram,30),3,100);
 put(ram,hp[0],160);
 auto silent=tracker.advance(contacts.snapshot(ram,31),3,100);
 assert(!silent[0].ms && !silent[1].ms);
 for(unsigned pc: {0x3fdb4u,0x09cf0u,0x29474u,0x29478u,0x2a160u,0x2a1b8u,0x1234u}) {
  full();assert(contacts.arm(ram,40));store(hp[0],160,pc);assert(!contacts.armed());
 }
 // Partial, wider, increasing and invalid stores fail closed.
 for(unsigned op: {38u,36u}) {
  full();assert(contacts.arm(ram,50));store(op==36 ? (hp[0]&~3u) : hp[0],0,0x104bc,op);assert(!contacts.armed());
 }
 full();assert(contacts.arm(ram,50));store(hp[0],182);assert(!contacts.armed());
 full();assert(contacts.arm(ram,50));contacts.tick(7250);assert(!contacts.armed());
 full();assert(contacts.arm(ram,50));contacts.tick(49);assert(!contacts.armed());
 full();assert(contacts.arm(ram,50));contacts.stop();assert(!contacts.snapshot(ram,51).live);
 std::vector<uint8_t> tiny(16);assert(!contacts.arm(tiny,0));
 full();put(ram,hp[0],180);assert(!contacts.arm(ram,0));
 full();put(ram,hp[0],0);assert(!contacts.arm(ram,0));
 full();put(ram,hp[0],1001);put(ram,hp[1],1001);assert(!contacts.arm(ram,0));
 full();assert(contacts.arm(ram,0));tracker.reset();tracker.advance(contacts.snapshot(ram,0),3,0);
 store(hp[0],150);silent=tracker.advance(contacts.snapshot(ram,1),3,0);
 assert(silent[0].low==0 && silent[1].low==0);
 puts("PASS: actual interpreter contacts, both victims/players, damage strength, KO and zero-health contacts");
 puts("PASS: default-off manual round scope, replay/refill/unknown disarm, no HP/display fallback, timeout and invalid stores");
}
