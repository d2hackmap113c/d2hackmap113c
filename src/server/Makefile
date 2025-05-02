OUT=..\..\map113c\server\d2server113c.exe
OBJ=\
	./Release/bnethash.obj\
	./Release/callback.obj\
	./Release/charlist.obj\
	./Release/config.obj\
	./Release/d2gamelist.obj\
	./Release/d2ge.obj\
	./Release/debug.obj\
	./Release/eventlog.obj\
	./Release/handle_s2s.obj\
	./Release/hexdump.obj\
	./Release/main.obj\
	./Release/net.obj\
	./Release/patch.obj\
	./Release/timer.obj\
	./Release/utils.obj\
	./Release/vars.obj\
	./Release/versioncheck.obj\

CFLAGS=/O2 /Oi /D WIN32 /D NDEBUG /D _CONSOLE /D _MBCS /FD /EHsc /MT /Gy /Fp"Release\d2server.pch" /Fd"Release\vc90.pdb" /W3 /WX /c /Zi /TP
LDFLAGS_EXE= /INCREMENTAL:NO /MANIFEST:NO /MAP:d:/d2hackmap/map113c/server/d2server113c.map /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /DYNAMICBASE /NXCOMPAT /MACHINE:X86 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib

all: $(OUT)
clean:
	rm $(OBJ)
h:
	cl.exe $(CFLAGS) /Yc"stdafx.h" /Fo"$@" .\\stdafx.cpp /nologo /errorReport:prompt

./Release/%.obj: %.cpp
	@echo $< $@
	@cl.exe $(CFLAGS) /Yu"stdafx.h" /Fo"$@" $< /nologo /errorReport:prompt

$(OUT): $(OBJ) 
	link.exe $(LDFLAGS_EXE) /OUT:../../map113c/server/d2server.exe $^ ./Release/stdafx.obj /NOLOGO /ERRORREPORT:PROMPT

#environment
#INCLUDE=C:\Program Files\Microsoft Visual Studio 9.0\VC\include;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\include;C:\Program Files\Microsoft SDKs\Windows\v6.0A\\include;C:\Program Files\Microsoft SDKs\Windows\v6.0A\include
#LIBPATH=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\lib
#LIB=C:\Program Files\Microsoft Visual Studio 9.0\VC\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib\i386;C:\Program Files\Microsoft SDKs\Windows\v6.0A\\lib;C:\Program Files\Microsoft SDKs\Windows\v6.0A\lib;C:\Program Files\Microsoft Visual Studio 9.0\;C:\Program Files\Microsoft Visual Studio 9.0\lib
