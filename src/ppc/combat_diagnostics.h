#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <vector>

// Opt-in scalar discovery only. Candidates are NOT confirmed fighter health.
// Call menu methods only with the emulation worker joined. No RAM/ROM export.
namespace vf3_diagnostics {
inline uint16_t health_word(const uint8_t *ram, uint32_t a) {
 return uint16_t((unsigned(ram[a ^ 3]) << 8) | ram[(a+1) ^ 3]);
}
struct Candidate { uint32_t address; uint16_t value; };
class Search {
public:
 static constexpr size_t limit=262144;
 std::vector<Candidate> candidates;
 bool started=false, overflow=false;
 bool start(const std::vector<uint8_t> &ram) {
  candidates.clear(); started=false; overflow=false;
  // Exclude the immutable 0..0x9FFFF program image documented by Model3::program_image.
  for(uint32_t a=0xa0000;a+1<ram.size() && a<0x800000;a+=2) {
   unsigned v=health_word(ram.data(),a);
   if(v && v<=1000) {
    if(candidates.size()==limit) { candidates.clear(); overflow=true; return false; }
    candidates.push_back({a,uint16_t(v)});
   }
  }
  started=true; return true;
 }
 void refine(const std::vector<uint8_t> &ram, bool decreased) {
  if(!started) return;
  auto end=std::remove_if(candidates.begin(),candidates.end(),[&](Candidate &c) {
   if(c.address+1>=ram.size()) return true;
   unsigned now=health_word(ram.data(),c.address);
   bool keep=decreased ? now<c.value : now==c.value;
   c.value=uint16_t(now); return !keep;
  });
  candidates.erase(end,candidates.end());
 }
};
enum class Phase : unsigned { unknown, live, post_ko, replay, outside };
inline const char *phase_name(Phase p) {
 switch(p) { case Phase::live:return "live"; case Phase::post_ko:return "post_ko";
 case Phase::replay:return "replay"; case Phase::outside:return "outside"; default:return "unknown"; }
}
struct Row {
 uint64_t frame; uint32_t pc,address; uint16_t before,after;
 unsigned player,bytes; Phase phase;
};
class Probe {
 struct Watch { uint32_t address; unsigned player; };
 std::array<Watch,16> watches{};
 size_t watch_count=0, count=0, next=0;
 std::array<Row,4096> rows{};
 uint32_t first=0x800000,last=0;
public:
 Search searches[2];
 uint64_t frame=0;
 Phase phase=Phase::unknown;
 size_t size() const { return count; }
 size_t watched() const { return watch_count; }
 void clear() {
  for(auto &s:searches) { s.candidates.clear(); s.started=false; s.overflow=false; }
  watch_count=count=next=0; first=0x800000; last=0; phase=Phase::unknown;
 }
 void update_watches() {
  watch_count=0; first=0x800000; last=0;
  for(unsigned p=0;p<2;++p) if(searches[p].started && searches[p].candidates.size()<=8)
   for(auto c:searches[p].candidates) {
    watches[watch_count++]={c.address,p+1}; first=std::min(first,c.address);last=std::max(last,c.address+2);
   }
 }
 // Identified independently from both player captures; diagnostic use only.
 // This is not a live/replay gate and never enables automatic feedback.
 bool watch_identified_health(const std::vector<uint8_t> &ram) {
  constexpr uint32_t fields[2]={0x1087d6,0x10a7d6};
  if(ram.size()<=fields[1]+1)return false;
  clear();
  for(unsigned p=0;p<2;++p) {
   searches[p].started=true;
   searches[p].candidates.push_back({fields[p],health_word(ram.data(),fields[p])});
  }
  update_watches();return true;
 }
 void observe(const uint8_t *ram,uint32_t a,uint64_t value,unsigned bytes,uint32_t pc) {
  if(!watch_count || a>=last || uint64_t(a)+bytes<=first || uint64_t(a)+bytes>0x800000) return;
  for(size_t i=0;i<watch_count;++i) {
   const auto w=watches[i];
   if(a>=w.address+2 || uint64_t(a)+bytes<=w.address) continue;
   const uint16_t old=health_word(ram,w.address); uint16_t now=old;
   for(unsigned b=0;b<bytes;++b) if(a+b>=w.address && a+b<w.address+2) {
    unsigned shift=(w.address+1-a-b)*8;
    now=uint16_t((now&~(255u<<shift)) | (((value>>((bytes-1-b)*8))&255u)<<shift));
   }
   if(old==now && now!=0) continue;
   rows[next]={frame,pc,w.address,old,now,w.player,bytes,phase};
   next=(next+1)%rows.size(); count=std::min(count+1,rows.size());
  }
 }
 bool save(const char *path,const std::vector<uint8_t> &ram) const {
  // Do not overwrite a useful earlier export with a header-only file.
  if(!watch_count && !count)return false;
  FILE *f=std::fopen(path,"w"); if(!f) return false;
  std::fprintf(f,"# Unverified scalar health candidates; player/phase are manual labels. No RAM or ROM dump.\n");
  std::fprintf(f,"# watched=%zu; stores=%zu; labels are manual, not an automatic scene gate.\n",watch_count,count);
  std::fprintf(f,"kind,frame,pc_hex,guest_address_hex,player_label,before,after,store_bytes,phase_label\n");
  for(size_t i=0;i<watch_count;++i) {
   auto w=watches[i];if(w.address+1<ram.size())
    std::fprintf(f,"candidate,%llu,00000000,%08x,%u,%u,%u,0,%s\n",(unsigned long long)frame,w.address,w.player,health_word(ram.data(),w.address),health_word(ram.data(),w.address),phase_name(phase));
  }
  const size_t begin=(next+rows.size()-count)%rows.size();
  for(size_t i=0;i<count;++i) {
   const auto &r=rows[(begin+i)%rows.size()];
   std::fprintf(f,"store,%llu,%08x,%08x,%u,%u,%u,%u,%s\n",(unsigned long long)r.frame,r.pc,r.address,r.player,r.before,r.after,r.bytes,phase_name(r.phase));
  }
  bool ok=std::ferror(f)==0;return std::fclose(f)==0 && ok;
 }
};
} // namespace vf3_diagnostics
