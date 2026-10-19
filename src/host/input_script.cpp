#include "input_script.h"
#include <cctype>
#include <climits>
#include <cstdlib>
#include <fstream>
#include <sstream>

namespace {

bool *find(m3::Controls &c, const std::string &name)
{
	if (name == "test") return &c.test[0];
	if (name == "service") return &c.service[0];
	if (name.size() < 4 || name[0] != 'p' || (name[1] != '1' && name[1] != '2') || name[2] != '.')
		return nullptr;
	int p = name[1] - '1';
	std::string b = name.substr(3);
	if (b == "coin") return &c.coin[p];
	if (b == "start") return &c.start[p];
	if (b == "up") return &c.up[p];
	if (b == "down") return &c.down[p];
	if (b == "left") return &c.left[p];
	if (b == "right") return &c.right[p];
	if (b == "punch") return &c.punch[p];
	if (b == "kick") return &c.kick[p];
	if (b == "guard") return &c.guard[p];
	if (b == "escape") return &c.escape[p];
	return nullptr;
}

void set(m3::Controls &c, const std::string &name, bool down)
{
	if (bool *b = find(c, name))
		*b = down;
}

} // namespace

bool InputScript::load(const std::string &path, std::string &error)
{
	std::ifstream in(path);
	if (!in) {
		error = "can't read " + path;
		return false;
	}
	std::string line;
	uint64_t prev = 0;
	int n = 0;
	while (std::getline(in, line)) {
		n++;
		size_t hash = line.find('#');
		if (hash != std::string::npos)
			line.resize(hash);
		std::istringstream ss(line);
		std::string when, op, arg, num;
		if (!(ss >> when >> op))
			continue;
		ss >> arg >> num;
		Cmd c;
		c.frame = when[0] == '+' ? prev + strtoull(when.c_str() + 1, nullptr, 10) : strtoull(when.c_str(), nullptr, 10);
		c.op = op;
		c.arg = arg;
		c.n = (num.empty() || !isdigit((unsigned char)num[0])) ? -1 : strtol(num.c_str(), nullptr, 0);
		if (op != "press" && op != "hold" && op != "release" && op != "random" && op != "mark" && op != "shot" && op != "quit") {
			error = path + ":" + std::to_string(n) + ": unknown command " + op;
			return false;
		}
		if (c.frame < prev) {
			error = path + ":" + std::to_string(n) + ": frame goes backwards";
			return false;
		}
		prev = c.frame;
		cmds.push_back(c);
	}
	return true;
}

InputScript::Actions InputScript::step(m3::Controls &c, uint64_t frame)
{
	Actions a;
	for (size_t i = 0; i < held.size();) {
		if (frame >= held[i].second) {
			set(c, held[i].first, false);
			held.erase(held.begin() + long(i));
		} else
			i++;
	}
	// Random play: every 3-20 frames a new stick direction (or none) and buttons. The same generator and
	// draws as the reference build's, so both runs press the same things.
	for (int p = 0; p < 2; p++) {
		Random &r = random[p];
		if (!r.on)
			continue;
		auto rnd = [&r]() { r.state = r.state * 6364136223846793005ull + 1442695040888963407ull; return uint32_t(r.state >> 33); };
		if (frame >= r.until) {
			r.until = frame + 3 + rnd() % 18;
			uint32_t d = rnd() % 12;
			static const int dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 }, dy[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
			int x = d < 8 ? dx[d] : 0, y = d < 8 ? dy[d] : 0;
			r.dir[0] = y < 0; r.dir[1] = y > 0; r.dir[2] = x < 0; r.dir[3] = x > 0;
			r.btn[0] = rnd() % 100 < 35;
			r.btn[1] = rnd() % 100 < 35;
			r.btn[2] = rnd() % 100 < 25;
			r.btn[3] = rnd() % 100 < 8;
		}
		std::string pre = p ? "p2." : "p1.";
		static const char *dirs[4] = { "up", "down", "left", "right" }, *btns[4] = { "punch", "kick", "guard", "escape" };
		for (int k = 0; k < 4; k++) {
			set(c, pre + dirs[k], r.dir[k]);
			set(c, pre + btns[k], r.btn[k]);
		}
	}
	while (next < cmds.size() && cmds[next].frame <= frame) {
		const Cmd &cmd = cmds[next++];
		auto hold_until = [&](uint64_t until) {
			for (auto &h : held)
				if (h.first == cmd.arg) { h.second = until; return; }
			held.push_back({ cmd.arg, until });
		};
		if (cmd.op == "press")
			hold_until(frame + uint64_t(cmd.n > 0 ? cmd.n : 6));
		else if (cmd.op == "hold")
			hold_until(UINT64_MAX);
		else if (cmd.op == "release") {
			for (size_t i = 0; i < held.size(); i++)
				if (held[i].first == cmd.arg) { held.erase(held.begin() + long(i)); break; }
			set(c, cmd.arg, false);
		} else if (cmd.op == "random") {
			int p = cmd.arg == "p2" ? 1 : 0;
			random[p].on = cmd.n >= 0;
			random[p].state = uint64_t(cmd.n) * 2654435761ull + 1;
			random[p].until = 0;
			if (!random[p].on)
				for (const char *b : { "up", "down", "left", "right", "punch", "kick", "guard", "escape" })
					set(c, std::string(p ? "p2." : "p1.") + b, false);
		} else if (cmd.op == "mark")
			a.marks.push_back(cmd.arg);
		else if (cmd.op == "shot")
			a.shots.push_back(cmd.arg.empty() ? "shot" : cmd.arg);
		else if (cmd.op == "quit")
			a.quit = true;
	}
	for (auto &h : held)
		set(c, h.first, true);
	return a;
}
