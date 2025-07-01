OUT=..\..\map113c\dll\d2hackmap.dll
MAP=..\..\map113c\dll\d2hackmap.map
SYMS=..\..\map113c\dll\d2hackmap.txt
OBJ=\
 ./Release/bitstream.obj\
 ./Release/config.obj\
 ./Release/DllMain.obj\
 ./Release/guess.obj\
 ./Release/install.obj\
 ./Release/mpq.obj\
 ./Release/common.obj\
 ./Release/d2callstub.obj\
 ./Release/dbg.obj\
 ./Release/loop.obj\
 ./Release/map.obj\
 ./Release/packet.obj\
 ./Release/route.obj\
 ./Release/waypoint.obj\
 ./Release/winmsg.obj\
 ./Release/env.obj\
 ./Release/GameMonitor.obj\
 ./Release/info.obj\
 ./Release/MessageLog.obj\
 ./Release/item.obj\
 ./Release/ItemExtInfo.obj\
 ./Release/dangerous.obj\
 ./Release/quest.obj\
 ./Release/unicode.obj\
 ./Release/lifebar.obj\
 ./Release/bo.obj\
 ./Release/d2s.obj\
 ./Release/DropProtection.obj\
 ./Release/enchant.obj\
 ./Release/chat.obj\
 ./Release/mcp.obj\
 ./Release/login.obj\
 ./Release/misc.obj\
 ./Release/multi.obj\
 ./Release/NpcTrade.obj\
 ./Release/party.obj\
 ./Release/quick.obj\
 ./Release/QuickSwapItem.obj\
 ./Release/resolution.obj\
 ./Release/skill.obj\
 ./Release/Snapshot.obj\
 ./Release/snap.obj\
 ./Release/summon.obj\
 ./Release/teleport.obj\
 ./Release/txt.obj\
 ./Release/panel.obj\
 ./Release/server.obj\
 ./Release/admin.obj\
 ./Release/uber.obj\

CFLAGS=/Oi /Ot /D WIN32 /D NDEBUG /D _WINDOWS /D _USRDLL /D D2HACKMAP_EXPORTS /D _WINDLL /D _MBCS /FD /EHsc /MT /Gy /Fp"Release\d2hackmap.pch" /Fd"Release\vc90.pdb" /W3 /WX /c /Zi /TP
LDFLAGS_EXE= /INCREMENTAL:NO /MANIFEST:NO /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /LTCG /DYNAMICBASE /NXCOMPAT /MACHINE:X86 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LDFLAGS_DLL= /INCREMENTAL:NO /DLL /MANIFEST:NO /DEF:d2hackmap.def /MAP:d:/d2hackmap/map113c/dll/d2hackmap.map /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /BASE:0x66000000 /DYNAMICBASE /NXCOMPAT /MACHINE:X86 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib psapi.lib

all: $(OUT) $(SYMS)
clean:
	rm $(OBJ)
h:
	cl.exe $(CFLAGS) /Yc"stdafx.h" /Fo"$@" .\\stdafx.cpp /nologo /errorReport:prompt

$(SYMS): ./Release/dumpmap.exe $(MAP)
	./Release/dumpmap.exe $(MAP) $@ d2hackmap
	
./Release/install.obj: d2ptrs.h
./Release/multi.obj: multi.h
./Release/info.obj: multi.h
./Release/GameMonitor.obj: multi.h
./Release/%.obj: %.cpp
	@echo $< $@
	@cl.exe $(CFLAGS) /Yu"stdafx.h" /Fo"$@" $< /nologo /errorReport:prompt

$(OUT): $(OBJ) ./Release/version.obj
	link.exe $(LDFLAGS_DLL) /OUT:../../map113c/dll/d2hackmap.dll $^ ./Release/stdafx.obj /NOLOGO /ERRORREPORT:PROMPT

version.cpp: ./Release/versionmake.exe $(OBJ)
	./Release/versionmake.exe $(OBJ)
./Release/versionmake.exe: ./Release/versionmake.obj
	link.exe $(LDFLAGS_EXE) /OUT:\"$@\" $^ ./Release/stdafx.obj /NOLOGO /ERRORREPORT:PROMPT
./Release/dumpmap.exe: ./Release/dumpmap.obj
	link.exe $(LDFLAGS_EXE) /OUT:$@ $^ ./Release/stdafx.obj /NOLOGO /ERRORREPORT:PROMPT

#environment
#INCLUDE=C:\Program Files\Microsoft Visual Studio 9.0\VC\include;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\include;C:\Program Files\Microsoft SDKs\Windows\v6.0A\\include;C:\Program Files\Microsoft SDKs\Windows\v6.0A\include
#LIBPATH=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\lib
#LIB=C:\Program Files\Microsoft Visual Studio 9.0\VC\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib\i386;C:\Program Files\Microsoft SDKs\Windows\v6.0A\\lib;C:\Program Files\Microsoft SDKs\Windows\v6.0A\lib;C:\Program Files\Microsoft Visual Studio 9.0\;C:\Program Files\Microsoft Visual Studio 9.0\lib
