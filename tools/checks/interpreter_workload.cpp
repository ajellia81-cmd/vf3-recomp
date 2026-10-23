// Hand-authored synthetic PowerPC loop, no game program or ROM data.
// Run under both -Os and -O3; compare architectural state, print timing separately.
#include "../../src/ppc/ppc.h"
#include <cassert>
#include <cfenv>
#include <chrono>
#include <cstdio>
#include <vector>

int main()
{
	std::vector<uint32_t> ram(0x800000 / 4), crom(0x800000 / 4);
	constexpr unsigned start = 0x1000, data = 0x20000, iterations = 1000003;
	ram[start / 4 + 0] = (32u << 26) | (3 << 21) | (4 << 16); // lwz r3,0(r4)
	ram[start / 4 + 1] = (21u << 26) | (3 << 21) | (3 << 16) | (5 << 11) | (31 << 1); // rotlwi r3,r3,5
	ram[start / 4 + 2] = (14u << 26) | (3 << 21) | (3 << 16) | 17; // addi r3,r3,17
	ram[start / 4 + 3] = (36u << 26) | (3 << 21) | (4 << 16); // stw r3,0(r4)
	ram[start / 4 + 4] = (18u << 26) | 0x03fffff0; // b -16
	ppc::Cpu cpu;
	cpu.attach(nullptr, reinterpret_cast<uint8_t *>(ram.data()), reinterpret_cast<uint8_t *>(crom.data()));
	cpu.configure(0x00071202, 2, 66000000, 0xf0000000);
	cpu.reset(); cpu.s.npc = start; cpu.s.r[4] = data;
	ram[data / 4] = 0x12345678;
	const auto t0 = std::chrono::steady_clock::now();
	assert(cpu.execute(iterations * 5) == iterations * 5 && !cpu.s.fatal);
	const double ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
	uint32_t expected = 0x12345678;
	for (unsigned i = 0; i < iterations; ++i) expected = ((expected << 5) | (expected >> 27)) + 17;
	assert(ram[data / 4] == expected && cpu.s.r[3] == expected && cpu.s.npc == start);
	assert(cpu.s.total_cycles == iterations * 5 && cpu.s.tb == iterations * 5 / 4);
	// Preserve multiplication rounding in every supported host rounding mode.
	for (int mode : {FE_TONEAREST, FE_TOWARDZERO, FE_UPWARD, FE_DOWNWARD}) {
		assert(std::fesetround(mode) == 0);
		cpu.s.fpscr = 0;
		cpu.s.f[2].d = 1.0000000000000002; cpu.s.f[3].d = 1.0000000000000002;
		cpu.exec_op((63u << 26) | (1 << 21) | (2 << 16) | (3 << 6) | (25 << 1)); // fmul
		volatile double a = cpu.s.f[2].d, b = cpu.s.f[3].d;
		volatile double product = a * b;
		assert(cpu.s.f[1].d == product);
	}
	std::fesetround(FE_TONEAREST);
	fprintf(stderr, "synthetic interpreter: 5 million instructions %.2f ms\n", ms);
	printf("PASS: interpreter state %08x; cycles %llu; strict FP rounding\n", expected, (unsigned long long)cpu.s.total_cycles);
}
