# Personal iOS port

Based on upstream d30b722eb492fd05edabc4b6ad428eff972f7c2c.
PowerPC execution is interpreter-only. The original SDL_GPU/Metal Real3D renderer
is retained. Original 1x-6x raster choices and all Picture shape options remain;
2868 x 1320 is appended as a fixed composition output from a 4x raster, not a
fractional native 3D rasterizer. Every resolution honors the selected aspect.
Brightness supports 50-200%. Touch controls appear only without a controller.
The player imports their own complete vf3 Revision D ZIP on-device.
No game files, generated game program, JIT, signing secrets or public release.
