Improvements of d2hackmap 1.13c

d2hackmap is a map plugin of the computer game "Diablo II". This project only works with Diablo II version 1.13c.

Improvements:
1. Support CONTROL,SHIFT,ALT Key combinations
2. Control multiple game windows, things like auto follow.
3. Perform certain skills automatically
4. Draw guide line on minimap or screen to a select target. So it is possible to play the game without using minimap.
5. Auto teleport to a selected target. The only way 60FCR Amazon can teleport smoothly in the game.
6. Create a snapshot of the character on local hard disk. A text file will be created for text searching. A d2s file will be created for local single player playing.
7. Drop protection of valuable equipment. Specified equipments can't be dropped to ground or sold to NPC. Character with equipments can't be deleted in the game menu.

see map113c/config.txt for details.

To use this plugin, start the game with the following command line:

D:\game\diablo2\D2Loader.exe -locale chi -lq -pdir D:\d2hackmap\map113c -direct -skiptobnet -w

Source code is compiled with Microsoft Visual Studio 2008 and tested on Windows XP SP3. The hardware setting of the developer machine is 1.5GB Ram, CPU 800MHz, screen resolution 1024*768.

files:
  map113c/* - All files needed to play
  map113c/config.txt - default config file which will load other config files
  src/d2hackmap - source code of d2hackmap
  src/loadhackmap - source code of loadhackmap, which load and reload d2hackmap.dll inside a game
  src/tools - Some useful C code which can be executed with Tiny C Compiler

About code reset
The compiled d2hackmap.dll is quite big, around 0.5MB. So this project will be periodically reset to an initial empty point to remove histories and maintain a reasonable size of the git repository. 
Information for myself, use the following commands to reset to the initial point.
	git clone git@github.com:d2hackmap113c/d2hackmap113c
	cd d2hackmap113c
	git reset --hard 836625d35e87bbc757d31cba9b2f1439543860de
	git push --force
