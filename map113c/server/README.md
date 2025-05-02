This is a working copy of D2GS server 1.13c.

Pvpgn downloaded from https://github.com/pvpgn/pvpgn-server/releases/download/v1.99/pvpgn-1.99.r578-win32-ALL.rar and the plain-cdb directory is extracted to pvpgn-1.99.r578

How to setup:
1.  copy the following dll to this directory
    D2Client.dll D2CMP.dll D2Common.dll D2Game.dll D2gfx.dll D2Lang.dll 
    D2MCPClient.dll D2Net.dll D2sound.dll D2Win.dll Fog.dll Storm.dll ijl11.dll
		check them with md5.txt. If the dll is modified, it might load other dlls.
2. Setup mpq path
  Change mpq_path in config.txt to the directory with contains d2data.mpq d2sfx.mpq d2speech.mpq patch_d2.mpq d2exp.mpq
3. Startup the server in the following order:
    pvpgn-1.99.r578\d2dbsConsole.exe
    pvpgn-1.99.r578\d2csConsole.exe
    pvpgn-1.99.r578\PvPGNConsole.exe
    d2server113c.exe
4. Start the client with something like the following or simply import localhost.reg
  D:\game\diablo2\D2Loader.exe -lq -locale chi -lq -pdir D:\d2hackmap\map113c -direct -skiptobnet -w -mpqpath=d:\game\mpq -bnetip=127.0.0.1

The source directory src/server is compiled using Visual Studio 2008 on Windows XP SP3
