// Synthetic texture changes only. Compare the actual snapshot/upload row planner
// with the former copy loop, including overlaps and clipped sheet boundaries.
#include "../../src/video/texture_rows.h"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <random>

struct Upload { unsigned x, y, w, h; };
int main()
{
	std::mt19937 rng(103);
	std::vector<Upload> uploads;
	for (unsigned trial = 0; trial < 80; ++trial) {
		uploads.clear();
		for (unsigned i = 0; i < trial * 4; ++i)
			uploads.push_back({0, unsigned(rng() % 4096), 32, unsigned(rng() % 600)});
		std::array<unsigned, 2048> original{}, merged{}, counts{};
		for (const auto &u : uploads) {
			unsigned y = u.y & 2047, h = std::min(u.h, 2048 - y);
			for (unsigned j = y; j < y + h; ++j) original[j] = j + 1;
		}
		for (const auto &[y, h] : video::texture_rows(uploads)) {
			assert(h && y + h <= 2048);
			for (unsigned j = y; j < y + h; ++j) { merged[j] = j + 1; ++counts[j]; }
		}
		assert(original == merged);
		for (auto n : counts) assert(n <= 1);
	}
	uploads.assign(256, {0, 32, 32, 1024});
	const auto rows = video::texture_rows(uploads);
	assert(rows.size() == 1 && rows[0].first == 32 && rows[0].second == 1024);
	const auto all = video::texture_rows(uploads, true);
	assert(all.size() == 1 && all[0].first == 0 && all[0].second == 2048);
	puts("PASS: identical texture snapshot coverage; each row copied once; 256 overlapping 4 MiB copies become one 4 MiB copy");
}
