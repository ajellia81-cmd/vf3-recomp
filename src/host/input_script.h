// Scripted inputs for headless runs, in the same format as the reference build's (tools/ref), so that both
// play exactly the same inputs on the same frames.
//
// One command per line, '#' starts a comment. The frame is absolute, or +N after the previous line's frame.
//   <frame> press <input> [frames]   hold for a number of frames (default 6), then release
//   <frame> hold <input>             hold until released
//   <frame> release <input>
//   <frame> random <p1|p2> <seed>    random play (stick and buttons) for that player; "random p1 off" stops
//   <frame> mark <name>              a phase marker (passed to the caller)
//   <frame> shot <name>              a screenshot (passed to the caller)
//   <frame> quit
// Inputs: p1.coin p1.start p1.up p1.down p1.left p1.right p1.punch p1.kick p1.guard p1.escape, the same for p2,
// and test, service.
#pragma once
#include "../machine/model3.h"
#include <string>
#include <vector>

class InputScript
{
public:
	struct Actions
	{
		bool quit = false;
		std::vector<std::string> marks, shots;
	};

	bool load(const std::string &path, std::string &error);
	// Before frame `frame` (0 for the first) runs: updates the controls
	Actions step(m3::Controls &c, uint64_t frame);

private:
	struct Cmd { uint64_t frame; std::string op, arg; long n; };
	struct Random { bool on = false; uint64_t state = 1, until = 0; bool dir[4] = {}, btn[4] = {}; };
	std::vector<Cmd> cmds;
	size_t next = 0;
	std::vector<std::pair<std::string, uint64_t>> held;  // input, release frame
	Random random[2];
};
