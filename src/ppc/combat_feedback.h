#pragma once
#include "../host/fight_haptics.h"
#include <cstddef>
#include <vector>

// Revision D contact preview. Deliberately NOT an automatic live/demo gate.
// Explicitly armed by the user at the start of one round; never persisted.
namespace vf3_feedback {
class Contacts {
 static constexpr uint32_t fields[2]={0x1087d6,0x10a7d6};
 bool armed_=false;
 uint64_t start_=0, epoch_=0;
 int maximum_=0;
 uint64_t damage_[2]{}, contacts_[2]{};
 static unsigned hp(const uint8_t *ram,uint32_t a) {
  return (unsigned(ram[a^3])<<8)|ram[(a+1)^3];
 }
public:
 bool armed() const { return armed_; }
 void stop() { armed_=false; }
 bool arm(const std::vector<uint8_t> &ram,uint64_t frame) {
  stop();
  if(ram.size()<=fields[1]+1)return false;
  const unsigned a=hp(ram.data(),fields[0]),b=hp(ram.data(),fields[1]);
  // Arm before damage, with both full bars: no guessed fixed maximum.
  if(!a || a!=b || a>1000)return false;
  maximum_=int(a);start_=frame;++epoch_;
  damage_[0]=damage_[1]=contacts_[0]=contacts_[1]=0;
  armed_=true;return true;
 }
 void tick(uint64_t frame) {
  // Fail closed even if an unobserved transition never writes the known fields.
  if(armed_ && (frame<start_ || frame-start_>=7200))stop();
 }
 void observe(const uint8_t *ram,uint32_t a,uint64_t value,unsigned bytes,uint32_t pc) {
  if(!armed_ || !bytes || bytes>8 || uint64_t(a)+bytes>0x800000)return;
  for(unsigned p=0;p<2;++p) {
   const uint32_t h=fields[p];
   if(a>=h+2 || uint64_t(a)+bytes<=h)continue;
   const unsigned old=hp(ram,h);
   // Every other writer to actual HP fails closed: replay restore, refill,
   // ring-out/setup and any unknown path. Display smoothing never overlaps.
   if(pc!=0x104bc || a!=h || bytes!=2) { stop();return; }
   const unsigned now=unsigned(value)&0xffff;
   if(old>unsigned(maximum_) || now>old) { stop();return; }
   if(old>now) { damage_[p]+=old-now;++contacts_[p]; }
   else if(old==0) ++contacts_[p]; // verified post-KO zero-to-zero contact
  }
 }
 fight_haptics::Snapshot snapshot(const std::vector<uint8_t> &ram,uint64_t frame) {
  tick(frame);
  fight_haptics::Snapshot s;
  s.frame=frame;s.epoch=epoch_;s.maximum=maximum_;s.damage_events=true;
  s.live=armed_ && ram.size()>fields[1]+1;
  if(s.live)for(unsigned p=0;p<2;++p) {
   s.health[p]=int(hp(ram.data(),fields[p]));
   s.damage_total[p]=damage_[p];s.contact_total[p]=contacts_[p];
  }
  return s;
 }
};
} // namespace vf3_feedback
