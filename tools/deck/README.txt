VIRTUA FIGHTER 3 RECOMP for Linux / Steam Deck (alpha)
======================================================

An unofficial fan project, not made, approved or supported by SEGA. Run it at your own risk.
No game files are included: you need your own MAME "vf3" set (vf3.zip, Revision D).

The game's own PowerPC program is recompiled from your ROM to native code as it starts.

For the Steam Deck and other x86-64 Linux PCs: it's built against Valve's Steam Runtime, the
libraries every Steam install has. On another Linux PC, unpack the VirtuaFighter3 folder anywhere,
put your vf3.zip in its roms folder and run VirtuaFighter3.sh (or add it to Steam as below).

INSTALL ON THE STEAM DECK
1. Copy the download to the Deck (e.g. Downloads) and unpack it there: in Desktop Mode, right-click
   it in the file manager > Extract > Extract archive here. (Unpacking on Windows, or copying the
   folder through a USB stick, can lose the files' permissions, and then nothing launches.)
2. Put your vf3.zip in the VirtuaFighter3/roms folder.
3. Desktop Mode: Steam > Games > Add a Non-Steam Game to My Library > Browse, set the file type
   filter to "All files", pick the vf3 file in the VirtuaFighter3 folder, Add Selected Programs.
   Then in its Properties: Start In is the VirtuaFighter3 folder (e.g. /home/deck/Downloads/
   VirtuaFighter3/), and under Compatibility, "Force the use of a specific Steam Play
   compatibility tool" is off (it's a native Linux game: Proton stops it launching).
4. Back in Game Mode, launch it from your Library with the default Gamepad layout.

It starts fullscreen at the Deck's 16:10, at render scale 2x. If the Deck can't keep up, the game
first shows only the arcade's own 57.5 pictures a second, then lowers the scale, and lifts both
again once it can. Show FPS (Display) puts the frame rate in a corner.

YOUR OWN ROM FOLDER (Batocera, a ROMs folder an update won't touch)
To keep vf3.zip somewhere else, tell the game where it is, in any of these ways (the first one given wins):
  - on the command line: ./vf3 --roms /userdata/roms/ports/vf3   (or --roms=/userdata/roms/ports/vf3);
    in Steam: the game's Properties > Launch Options: --roms "/home/deck/ROMs/vf3"
  - the environment variable VF3_ROMS=/userdata/roms/ports/vf3
  - a line in settings.ini (in this folder):  roms_folder = /userdata/roms/ports/vf3
    (~ is your home folder; a relative path is from settings.ini's folder)
For example, a Batocera port launcher, /userdata/roms/ports/VirtuaFighter3.sh:
  #!/bin/bash
  exec /userdata/system/VirtuaFighter3/VirtuaFighter3.sh --roms /userdata/roms/ports/vf3
The game only reads that folder (log.txt says which folder it used). If vf3.zip isn't there, log.txt says
where it looked and the game uses the roms folder here as before; a set you then choose with the
file picker is copied into your ROM folder, so the next start finds it there.

CONTROLS
  D-pad / left stick   move            X  guard     A  punch     B  kick     Y  escape
  LT  guard + punch                    RB punch + kick           LB guard + kick
  RT  guard + punch + kick             Start  start
  View (tap)  the menu: display, sound, game, arcade settings, controls and quit
              (L3 + R3, or the touchpad's click, open it too)
  View + LB / RB   render scale / picture shape

Display > 3D renderer: Auto (the default) draws the 3D on the GPU (Vulkan) once it has checked, as
the game starts, that its picture is the CPU's; GPU or CPU to choose yourself.

Settings, rankings, screenshots and saves are kept in this folder, and nothing anywhere else (a
portable.txt here keeps it that way even without the roms folder). See LICENSES.txt for the
software it uses: MAME's 68000 and SCSP, SDL, RmlUi, FreeType and zlib.
