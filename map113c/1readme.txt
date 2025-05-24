Download URL: https://github.com/d2hackmap113c/d2hackmap113c/
If d2hackmap is downloaded to d:\d2hackmap, start it with the following parameters
    D:\game\diablo2\D2Loader.exe -lq -pdir D:\d2hackmap\map113c -direct -w
	D2Loader will load D:\d2hackmap\map113c\loadhackmap.dll. loadhackmap.dll will copy and load/reload D:\d2hackmap\map113c\dll\d2hackmap.dll to D:\d2hackmap\map113c\runtime\d2hackmap?.dll

How to compile
  The project is compiled using Visual Studio 2008 on Windows XP SP3. Open src/d2hackmap.sln and press F7 to compile. There is also a Makefile which can quickly recompile single C file when headers are modified. Press Ctrl+R inside the game to reload the new dll.
  All source code are edited with gvim9.0 with TAB setting of 2
    set tabstop=2
    set shiftwidth=2

Config files
  The default config file is config.txt, it can be overridden by -d2hackmapcfg=<path>. The default config.txt will load other configs.
  config_def.txt     Most config
  itemconf_def.txt   Item config
  config_my.txt      My config for debugging
  itemconf_left.txt  My item config 
  itemconf_right.txt My item config 

Keyboard setting
    map113c\default.key is the game keyboard setting I'm using. Copy it to <game>\default.key and delete *.key files under <game>\save will enable this setting.
	I - Character
	U - Quest
	9 - Skill tree
	V - Minimap on/off
	Skill 1-8: Q,E,A,S,D,Z,X,C
	Skill 9-16: Y,H,TAB,Front left side mouse button, Middle mouse button
d2hackmap keyboard setting can be modified in the config files. The final keyboard settings will be printed to map113c\runtime\keymap.log for inspection.
	Ctrl+R - reload config, or d2hackmap.dll if it is recompiled
	Alt+ENTER - Full screen
	Control+L - Lock mouse in the current window
	5	- Back to town
	6 - Exit game
	7 - See next page of attributes
	8 - Switch between Original Effects, Infravision, Full light on
	= - Next game 
	\ - Socket protection on/off
	" - Low HP protection on/off
	Alt+C - Character
	Alt+Q - Quest
	Alt+T - Skill tree
	Alt+F - Reveal current level map
	Hold R,T,Ctrl+F,G: Move screen
	Arrow Keys: Move minimap if minimap is on, otherwize move screen
	Alt+S - Always show items on ground
	Alt+G - Show all items on ground
	J - Enable selecting players out of town
	K - Show other player's equipment
	Shift+F: Make characters in other game windows on the same computer follow, the number is controlled by MultiClientGroupSize1.
	Shift+G: Make characters in other game windows on the same computer follow, the number is controlled by MultiClientGroupSize2.
	Shift+T: Stop follow
	Shift+E: Make characters in other game windows on the same computer enter/exit portal
	Hold Alt: Send the mouse operation to other game windows.
	Mouse wheel: switch map target
	H - Auto skill if the game window is active
	Ctrl+Alt+K - Auto skill if the game window is inactive
	F - Auto use skill. If the right skill is teleport, auto teleport to the map target.

