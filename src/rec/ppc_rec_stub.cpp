// iOS has no executable-memory recompiler. The PowerPC interpreter runs every instruction.
#include "ppc_rec.h"
#if !defined(VF3_IOS) || !defined(VF3_DISABLE_JIT)
#error "This stub is only for interpreter-only iOS builds"
#endif
namespace ppcrec {
struct Spin {};
Rec::Rec() = default;
Rec::~Rec() = default;
bool Rec::build(ppc::Cpu &, const std::vector<uint32_t> &, Stats *) { return false; }
bool Rec::run(ppc::Cpu &) { return false; }
void Rec::code_written(uint32_t) {}
void Rec::recount() {}
}
