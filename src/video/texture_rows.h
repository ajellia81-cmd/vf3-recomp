#pragma once
#include <array>
#include <cstdint>
#include <utility>
#include <vector>

namespace video {
// Snapshots and GPU uploads both copy whole texture-sheet rows. Coalesce them
// before copying: many small textures at a character cut often share rows.
// Work is bounded by the sheet size, regardless of the number of overlapping areas.
template<class Uploads>
std::vector<std::pair<uint32_t, uint32_t>> texture_rows(const Uploads &uploads, bool full = false)
{
	std::array<bool, 2048> dirty{};
	if (full) return {{0, 2048}};
	if (uploads.empty()) return {};
	for (const auto &u : uploads) {
		const uint32_t first = u.y & 2047;
		const uint32_t count = u.h < 2048 - first ? u.h : 2048 - first;
		for (uint32_t y = first; y < first + count; ++y) dirty[y] = true;
	}
	std::vector<std::pair<uint32_t, uint32_t>> rows;
	for (uint32_t y = 0; y < 2048;) {
		if (!dirty[y]) { ++y; continue; }
		const uint32_t first = y;
		while (y < 2048 && dirty[y]) ++y;
		rows.push_back({first, y - first});
	}
	return rows;
}
} // namespace video
