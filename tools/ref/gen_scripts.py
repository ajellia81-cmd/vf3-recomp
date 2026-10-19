#!/usr/bin/env python3
# Writes the input scripts for the coverage runs (format: see InputScript.cpp in supermodel-cov.patch).
#   tools/ref/gen_scripts.py <out-folder> [--boot FRAMES] [--fight FRAMES] [--attract FRAMES]
# Checked against Revision D's screenshots: attract from about frame 1200, 2 coins a credit, the select screen
# a 6x2 grid with the cursor on Akira (top left), punch chooses, the first fight about 1300 frames after the
# coins. Each script still takes screenshots (shot_*.bmp in the run's out folder) to confirm.
import argparse, os

ap = argparse.ArgumentParser()
ap.add_argument('out')
ap.add_argument('--boot', type=int, default=1200, help='frames from power-on to the attract')
ap.add_argument('--fight', type=int, default=10800, help='frames of random play per fighter run')
ap.add_argument('--attract', type=int, default=36000, help='frames of attract mode')
a = ap.parse_args()
os.makedirs(a.out, exist_ok=True)

class Script:
    def __init__(self):
        self.t = 0
        self.lines = []
    def at(self, dt, cmd):
        self.t += dt
        self.lines.append(f'{self.t} {cmd}')
    def write(self, name):
        with open(os.path.join(a.out, name), 'w') as f:
            f.write('\n'.join(self.lines + [f'{self.t} quit']) + '\n')

def boot():
    s = Script()
    s.at(0, 'mark boot')
    s.at(0, 'shot boot')
    s.at(a.boot, 'mark attract')
    s.at(0, 'shot attract')
    return s

# Attract mode only: the demo fights, rankings, title
s = boot()
for _ in range(a.attract // 1200):
    s.at(1200, 'shot attract')
s.write('attract.txt')

def credit(s, player):
    s.at(10, f'press {player}.coin')
    s.at(20, f'press {player}.coin')

# One run per fighter: credit, move the select cursor to fighter n (row n // 6, column n % 6), choose, then
# random play against the CPU. Every two minutes a credit and start, to continue after a loss.
for n in range(12):
    s = boot()
    s.at(100, 'mark credit')
    credit(s, 'p1')
    s.at(60, 'press p1.start')
    s.at(180, 'shot select')
    if n >= 6:
        s.at(60, 'press p1.down 4')
    for _ in range(n % 6):
        s.at(60, 'press p1.right 4')
    s.at(60, 'shot chosen')
    s.at(10, 'press p1.punch')
    s.at(0, f'mark fighter{n:02}')
    s.at(240, 'shot fight')
    s.at(0, f'random p1 {n + 1}')
    for k in range(1, a.fight // 900 + 1):
        s.at(900, 'shot fight')
        if k % 8 == 0:
            credit(s, 'p1')
            s.at(60, 'press p1.start')
    s.write(f'fighter{n:02}.txt')

# Versus: two players, both random (the random play also picks their fighters)
s = boot()
s.at(100, 'mark credit')
credit(s, 'p1')
credit(s, 'p2')
s.at(60, 'press p1.start')
s.at(10, 'press p2.start')
s.at(180, 'shot select')
s.at(0, 'mark versus')
s.at(0, 'random p1 101')
s.at(0, 'random p2 202')
for k in range(1, a.fight // 900 + 1):
    s.at(900, 'shot versus')
    if k % 4 == 0:
        credit(s, 'p1')
        credit(s, 'p2')
        s.at(60, 'press p1.start')
        s.at(10, 'press p2.start')
s.write('versus.txt')

# Operator test menu: SERVICE moves the cursor, TEST chooses
s = boot()
s.at(300, 'press test 10')
s.at(0, 'mark testmenu')
s.at(120, 'shot test')
for k in range(40):
    s.at(40, 'press service 4')
    if k % 5 == 4:
        s.at(30, 'press test 4')
        s.at(120, 'shot test')
s.at(600, 'shot test')
s.write('testmenu.txt')
print('scripts in', a.out)
