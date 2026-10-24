#pragma once
#include "../host/fight_haptics.h"
#include <vector>

// Revision D scalar contact tracking, no diagnostic capture or game-data export.
// This recognizes observed round/refill and replay writers, NOT human-vs-attract state.
namespace vf3_feedback {
class Contacts {
 static constexpr uint32_t fields[2]={0x1087d6,0x10a7d6};
 bool active_=false,cold_=true;
 uint64_t frame_=0,start_=0,epoch_=0,refill_frame_=0;
 unsigned refill_mask_=0,refill_value_[2]{};
 int maximum_=0;
 uint64_t damage_[2]{},contacts_[2]{};
 static unsigned hp(const uint8_t *ram,uint32_t a) {
  return (unsigned(ram[a^3])<<8)|ram[(a+1)^3];
 }
 void begin(unsigned maximum) {
  active_=true;cold_=false;maximum_=int(maximum);start_=frame_;++epoch_;
  refill_mask_=0;
  damage_[0]=damage_[1]=contacts_[0]=contacts_[1]=0;
 }
public:
 bool active() const {return active_;}
 void stop() {active_=false;cold_=false;refill_mask_=0;}
 void reset() {stop();cold_=true;frame_=start_=0;maximum_=0;++epoch_;}
 void tick(uint64_t frame) {
  if(frame<frame_)reset();
  frame_=frame;
  if(active_ && frame_-start_>=7200)stop();
  if(refill_mask_ && frame_-refill_frame_>2)refill_mask_=0;
 }
 void observe(const uint8_t *ram,uint32_t a,uint64_t value,unsigned bytes,uint32_t pc) {
  if(!bytes || bytes>8 || a>=fields[1]+2 || uint64_t(a)+bytes<=fields[0])return;
  for(unsigned p=0;p<2;++p) {
   const uint32_t h=fields[p];
   if(a>=h+2 || uint64_t(a)+bytes<=h)continue;
   const unsigned now=unsigned(value)&0xffff;
   // The independently observed full-health round refill for both sides.
   if(pc==0x09cf0 && a==h && bytes==2 && now>0 && now<=1000) {
    active_=false;cold_=false;
    if(!refill_mask_)refill_frame_=frame_;
    refill_value_[p]=now;refill_mask_|=1u<<p;
    if(refill_mask_==3) {
     if(refill_value_[0]==refill_value_[1])begin(now);
     else refill_mask_=0;
    }
    return;
   }
   // Includes replay restoration, ring-out/setup and unverified throw writers.
   if(pc!=0x104bc || a!=h || bytes!=2) {stop();return;}
   const unsigned old=hp(ram,h);
   // Initial round may have initialized before our first contact observation.
   // Only a verified contact with BOTH unchanged/full equal bars seeds it.
   if(!active_ && cold_) {
    const unsigned other=hp(ram,fields[p^1]);
    if(old && old==other && old<=1000 && now<old)begin(old);
   }
   if(!active_)return;
   if(old>unsigned(maximum_) || now>old) {stop();return;}
   if(old>now) {damage_[p]+=old-now;++contacts_[p];}
   else if(old==0)++contacts_[p];
  }
 }
 fight_haptics::Snapshot snapshot(const std::vector<uint8_t> &ram,uint64_t frame) {
  tick(frame);
  fight_haptics::Snapshot s;
  s.frame=frame;s.epoch=epoch_;s.maximum=maximum_;s.damage_events=true;
  s.live=active_ && ram.size()>fields[1]+1;
  if(s.live)for(unsigned p=0;p<2;++p) {
   s.health[p]=int(hp(ram.data(),fields[p]));
   s.damage_total[p]=damage_[p];s.contact_total[p]=contacts_[p];
  }
  return s;
 }
};
} // namespace vf3_feedback
