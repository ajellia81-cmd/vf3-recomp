VIRTUA FIGHTER 3 RECOMP (alpha)
===============================

An unofficial fan project, not made, approved or supported by SEGA. Run it at your own risk.

Virtua Fighter 3 (Sega Model 3, 1996) as a native program for your computer: the game's own
PowerPC program is recompiled from your ROM to native code as it starts, and the rest of the
arcade board (the Real3D graphics, the tile generator and the sound board) runs on code written
for this port and from MAME, inside a PC host with widescreen, higher resolutions, smooth motion
up to 120 pictures a second, a menu and gamepads. No game files are included: you need your own
MAME "vf3" set.


WHAT YOU NEED
- A Mac with macOS 11 (Big Sur) or later, Apple silicon or Intel.
- Your own copy of the game as MAME's "vf3" set (vf3.zip, Revision D). That's all it needs.


FIRST LAUNCH
1. Unzip and move "Virtua Fighter 3 Recomp" to Applications (or anywhere).
2. The app isn't signed, so the first time macOS won't open it. Double-click it once, close the
   message, then go to System Settings > Privacy & Security, scroll down and click Open Anyway.
   (On macOS 14 and earlier you can instead right-click the app, choose Open, then Open again.)
3. It asks for your vf3.zip. Choose it. It's copied into
   ~/Library/Application Support/Virtua Fighter 3 Recomp/roms, so you only do this once.

Prefer to keep everything in the game's folder (portable)? Put a file named portable.txt next to
the app, in the same folder, or in the folder above it (an empty one will do). The game then keeps everything in that folder
and nothing in ~/Library/Application Support: settings, saves, screenshots, log.txt, and your
vf3.zip, which it asks for once and copies into a "roms" folder there. (An app that hasn't been
moved since it was downloaded runs from a hidden copy that can't see the file: move it with the
Finder first.)

Keep your ROMs in a folder of your own? Add a line to settings.ini (in the data folder above, or
beside the app in portable mode):  roms_folder = ~/ROMs/vf3
or start it from Terminal:  open "Virtua Fighter 3 Recomp.app" --args --roms ~/ROMs/vf3
(or set VF3_ROMS). The game looks there first for vf3.zip and only reads it; log.txt says which
folder it used, or where it looked when the zip isn't there (it then looks in the usual place).


CONTROLS (change them in the menu: Esc, or View/Select on a pad; L3 + R3 or the touchpad click too)
Keyboard, player 1:  arrows move   Z guard   X punch   C kick   V escape   1 start
Keyboard, player 2:  I J K L move  B guard   N punch   M kick   , escape   2 start
Pads (Xbox layout):  d-pad/stick move   X guard   A punch   B kick   Y escape
                     LT guard+punch   RB punch+kick   LB guard+kick   RT all three   Start start
Esc menu   P pause   F3 reset   F5 render scale   F6 picture shape   F11, F or Alt+Enter fullscreen
F9 timings in the title bar   F12 screenshot   Tab (held) fast forward
In the arcade's test menu: Down moves, Start selects (or 9 and F2).

8BitDo pads on a Mac: use Switch (S) mode.


OPTIONS
Display: render scale (up to 6x), antialiasing (2x or 3x: smoother edges for more work), the 3D
renderer, picture shape (fits the window: widescreen and ultrawide; the fights see more to the
sides, and the stages' backgrounds carry on), background filter, smooth motion (a picture for each
refresh of your screen, up to 120 a second), screen filter (scanlines or CRT), Show FPS,
fullscreen. Sound: volume, music, effects and voices.
Game: play style (PC: no boot screen, always free play / arcade: the
boot, coins and credits), reset. Arcade settings: difficulty, rounds to win, energy, demo
sound, country, and the arcade's own service menu (its other settings, sound test, bookkeeping); a
change restarts the game. Controls: each player's pad, keys and buttons.
3D renderer: Auto (the default) draws the 3D on the graphics card once it has checked, as the
game starts, that its picture is the CPU's; GPU or CPU to choose yourself. If the GPU ever draws
differently or fails, the game uses the CPU for that run (log.txt says so), and on Windows the
next start tries Vulkan if Direct3D 12 was the one that failed.

If your Mac can't keep up, the game first shows only the arcade's own 57.5 pictures a second,
then lowers the antialiasing and the render scale; all come back once it can keep up again.


YOUR FILES
Settings, saves (rankings, the arcade settings), screenshots and your ROMs are in
~/Library/Application Support/Virtua Fighter 3 Recomp (or beside the app, with a portable.txt
there). log.txt there says what the game found and did, for bug reports.


CREDITS
Built on MAME's 68000 and SCSP (as Virtua Fighter 2 Recomp has them), SDL, RmlUi, FreeType and
zlib: see LICENSES.txt. Supermodel's documentation of the Model 3 (its source code's comments)
guided this port's own code. The port was done with Claude.
