// The stages' 2D skies tied to the 3D camera. VF3 scrolls a stage's backdrop (the tile layer below the 3D) with the
// camera of a few frames before: in fights, on every stage measured, the backdrop's move each frame is the camera's
// turn 3 frames earlier (1.11 of the turn in the screen's pixels across, 1.08 up and down; R2 0.95-0.99), so the sky
// slides behind the 3D for about 50 ms after every turn (a round's opening sweep has it in step). SkyLead follows the
// camera's turn (the viewport's root node, where the game puts its camera) and gives how far ahead of the game's
// backdrop the camera's sky is: the last 3 frames' turn.
// It is applied only while the backdrop is seen to follow the camera so (eased in and out), never on a close-up, a
// replay's still or the menus.
#pragma once
#include "r3d_scene.h"
#include <algorithm>
#include <cmath>

namespace video {

struct SkyLead
{
	static constexpr int DELAY = 3, WINDOW = 8, N = DELAY + WINDOW;
	static constexpr float RX = 1.11f, RY = 1.08f;
	// the last N frames, newest first: the camera's turn (in the backdrop's pixels) and the backdrop's move
	float tx[N] = {}, ty[N] = {}, mx[N] = {}, my[N] = {};
	bool mk[N] = {};    // the backdrop's move known
	int known = 0;      // frames in a row with the turn known
	int missed = 0;     // frames in a row with it unknown, or the backdrop not following
	float trust = 0;    // how much of the lead is applied (0 to 1)
	// The newest frame's lead (the camera's sky past the game's backdrop, in the backdrop's pixels and
	// TileGen::scrolled's sense), as trusted
	float lead_x = 0, lead_y = 0;
	// Where the sky is drawn: moved by the camera's own turn each frame (smooth, to the fraction of a pixel; the game's
	// backdrop moves by whole pixels, unevenly around the camera's move: at 4x, steps of 4 pixels), and drawn a little
	// of the way (PULL a frame) towards the game's backdrop with the lead, so it never drifts from it. game_*: the
	// game's backdrop (its moves added up), sky_*: the sky; off_*: the sky past the game's backdrop, as drawn (the
	// newest frame's and the one before's)
	static constexpr float PULL = 0.15f;
	double game_x = 0, game_y = 0, sky_x = 0, sky_y = 0;
	float off_x = 0, off_y = 0, off_x0 = 0, off_y0 = 0;

	void reset()
	{
		*this = SkyLead();
	}

	// The camera's turn from a's viewport to b's, in the backdrop's pixels (false if it isn't known)
	static bool turn(const Scene &a, const Scene &b, float &x, float &y)
	{
		if (a.viewports.size() != b.viewports.size()) return false;
		for (size_t v = 0; v < b.viewports.size(); v++) {
			const Viewport &va = a.viewports[v], &vb = b.viewports[v];
			if (!vb.full_screen || vb.models.size() < 8) continue;
			if (vb.root_matrix < 0 || va.root_matrix != vb.root_matrix) return false;
			Mat4 inv = Mat4::identity();   // a's rotation, transposed
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++) inv.m[j * 4 + i] = va.root.m[i * 4 + j];
			const Mat4 d = vb.root * inv;
			const float fx = 496.0f / (vb.right - vb.left), fy = 384.0f / (vb.top - vb.bottom);
			x = RX * fx * std::atan2(d.m[8], d.m[0]);
			y = RY * fy * std::atan2(-d.m[9], d.m[10]);
			return std::isfinite(x) && std::isfinite(y) && std::fabs(x) < 60 && std::fabs(y) < 60;
		}
		return false;
	}

	// Each frame: the scenes before and now, and the backdrop's move (TileGen::scrolled; moved false if unknown)
	void frame(const Scene &before, const Scene &now, bool moved, float dx, float dy)
	{
		float x, y;
		if (!turn(before, now, x, y)) {
			// a frame without the camera's turn: once is forgiven (its turn taken as the frame before's), twice
			// (a cut, a scene without a stage) starts again
			if (++missed > 1 || known == 0) { reset(); return; }
			x = tx[0];
			y = ty[0];
		}
		for (int i = N - 1; i > 0; i--) { tx[i] = tx[i - 1]; ty[i] = ty[i - 1]; mx[i] = mx[i - 1]; my[i] = my[i - 1]; mk[i] = mk[i - 1]; }
		tx[0] = x; ty[0] = y; mx[0] = dx; my[0] = dy; mk[0] = moved;
		known++;
		if (known >= N) {
			// Does the backdrop follow the camera DELAY frames late, rather than in step (a round's opening sweep)?
			// Over the last WINDOW frames, while the turn changes (a steady turn fits both): DELAY frames late
			// fitting clearly better eases the lead in (over 10 frames); in step fitting clearly better, or the
			// backdrop not following, for 2 frames in a row (one is forgiven) eases it out (over 4)
			float late = 0, in_step = 0, change = 0, bias_x = 0, bias_y = 0;
			int n = 0;
			for (int i = 0; i < WINDOW; i++) {
				if (mk[i]) {
					late += std::fabs(mx[i] - tx[i + DELAY]) + std::fabs(my[i] - ty[i + DELAY]);
					in_step += std::fabs(mx[i] - tx[i]) + std::fabs(my[i] - ty[i]);
					bias_x += mx[i] - tx[i + DELAY];
					bias_y += my[i] - ty[i + DELAY];
					n++;
				}
				change += std::fabs(tx[i + 1] - tx[i]) + std::fabs(ty[i + 1] - ty[i]);
			}
			// not following: on average over the window (a hit's shake, the backdrop jolted a few pixels each way,
			// averages out), or the move unknown
			const bool follows = moved && n > WINDOW / 2 && std::fabs(bias_x) < 2.0f * n && std::fabs(bias_y) < 2.0f * n;
			const bool judged = change > 4 && late + in_step > 2;
			if (judged && follows && late < 0.6f * in_step) { trust = std::min(1.0f, trust + 0.1f); missed = 0; }
			else if ((judged && in_step < 0.6f * late) || !follows) {
				if (++missed > 1) trust = std::max(0.0f, trust - 0.25f);
			}
			else missed = 0;
		}
		float full_x = 0, full_y = 0;
		for (int i = 0; i < DELAY; i++) { full_x += tx[i]; full_y += ty[i]; }
		lead_x = trust * full_x;
		lead_y = trust * full_y;
		off_x0 = off_x;
		off_y0 = off_y;
		if (moved) { game_x += dx; game_y += dy; }
		const double want_x = game_x + full_x, want_y = game_y + full_y;
		if (trust == 0 || std::fabs(sky_x + x - want_x) > 40 || std::fabs(sky_y + y - want_y) > 40) {
			sky_x = want_x;   // (not following, or too far: starts again where the game's backdrop with the lead is)
			sky_y = want_y;
		} else {
			sky_x += x + PULL * (want_x - (sky_x + x));
			sky_y += y + PULL * (want_y - (sky_y + y));
		}
		off_x = float(trust * (sky_x - game_x));
		off_y = float(trust * (sky_y - game_y));
	}

	// How far past the game's backdrop to draw the sky, t of the way from the frame before to the newest (1: the
	// newest): between the two frames' (the game's backdrop is drawn between its own two places)
	float at_x(float t) const { return off_x0 + t * (off_x - off_x0); }
	float at_y(float t) const { return off_y0 + t * (off_y - off_y0); }
};

} // namespace video
