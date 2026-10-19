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
- Windows 10 or 11, 64-bit.
- Your own copy of the game as MAME's "vf3" set (vf3.zip, Revision D). That's all it needs.


FIRST LAUNCH
1. Unzip the VirtuaFighter3 folder anywhere (not inside Program Files).
2. Run VirtuaFighter3.exe. It isn't signed, so Windows SmartScreen may warn the first time:
   click "More info", then "Run anyway".
3. It asks for your vf3.zip. Choose it. It's copied into
   %APPDATA%\Virtua Fighter 3 Recomp\roms, so you only do this once.

Prefer to keep everything in the game's folder (portable)? Put a file named portable.txt next to
VirtuaFighter3.exe, or in the folder above it (an empty one will do: right-click > New > Text Document, and name it
portable). The game then keeps everything in its own folder and nothing in %APPDATA%: settings,
saves, screenshots, log.txt, and your vf3.zip, which it asks for once and copies into its "roms"
folder. (A "roms" folder next to VirtuaFighter3.exe with vf3.zip in it does the same.)

Keep your ROMs in a folder of your own? Add a line to settings.ini (in the data folder above, or
next to VirtuaFighter3.exe in portable mode):  roms_folder = D:\ROMs\vf3
or give it to the program: a shortcut whose Target ends in  --roms "D:\ROMs\vf3"  (or set the
environment variable VF3_ROMS). The command line wins, then VF3_ROMS, then settings.ini. The game looks
there first for vf3.zip and only reads it; log.txt says which folder it used, or where it looked when
the zip isn't there (it then looks in the usual place, and a zip you then choose is copied into your
ROM folder).


CONTROLS (change them in the menu: Esc, or View/Select on a pad; L3 + R3 or the touchpad click too)
Keyboard, player 1:  arrows move   Z guard   X punch   C kick   V escape   1 start
Keyboard, player 2:  I J K L move  B guard   N punch   M kick   , escape   2 start
Pads (Xbox layout):  d-pad/stick move   X guard   A punch   B kick   Y escape
                     LT guard+punch   RB punch+kick   LB guard+kick   RT all three   Start start
Esc menu   P pause   F3 reset   F5 render scale   F6 picture shape   F11, F or Alt+Enter fullscreen
F9 timings in the title bar   F12 screenshot   Tab (held) fast forward
In the arcade's test menu: Down moves, Start selects (or 9 and F2).


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

If your computer can't keep up, the game first shows only the arcade's own 57.5 pictures a second,
then lowers the antialiasing and the render scale; all come back once it can keep up again.


YOUR FILES
Settings, saves (rankings, the arcade settings), screenshots and your ROMs are in
%APPDATA%\Virtua Fighter 3 Recomp (or beside VirtuaFighter3.exe, with a portable.txt or a "roms" folder there). log.txt there says what the game found and did, for bug reports.


CREDITS
Built on MAME's 68000 and SCSP (as Virtua Fighter 2 Recomp has them), SDL, RmlUi, FreeType and
zlib and the MinGW-w64 runtime: see LICENSES.txt. Supermodel's documentation of the Model 3 (its source code's comments)
guided this port's own code. The port was done with Claude.
