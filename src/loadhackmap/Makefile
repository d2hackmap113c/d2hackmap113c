OUT=..\..\map113c\loadhackmap.dll
MAP=..\..\map113c\loadhackmap.map
OBJ=\
 ./Release/dllmain.obj\

CFLAGS=/Oi /Ot /D WIN32 /D NDEBUG /D _WINDOWS /D _USRDLL /D D2HACKMAP_EXPORTS /D _WINDLL /D _MBCS /FD /EHsc /MT /Gy /Fp"Release\loadhackmap.pch" /Fd"Release\vc90.pdb" /W3 /WX /c /Zi /TP
LDFLAGS_EXE= /INCREMENTAL:NO /MANIFEST:NO /SUBSYSTEM:CONSOLE /OPT:REF /OPT:ICF /LTCG /DYNAMICBASE /NXCOMPAT /MACHINE:X86 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LDFLAGS_DLL= /INCREMENTAL:NO /DLL /MANIFEST:NO /DEF:loadhackmap.def /MAP:../../map113c/loadhackmap.map /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /BASE:0x66000000 /DYNAMICBASE /NXCOMPAT /MACHINE:X86 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib psapi.lib

all: $(OUT)
clean:
	rm $(OBJ)
h:
	cl.exe $(CFLAGS) /Yc"stdafx.h" /Fo"$@" .\\stdafx.cpp /nologo /errorReport:prompt

./Release/%.obj: %.cpp
	@echo $< $@
	@cl.exe $(CFLAGS) /Yu"stdafx.h" /Fo"$@" $< /nologo /errorReport:prompt

$(OUT): $(OBJ) 
	link.exe $(LDFLAGS_DLL) /OUT:../../map113c/loadhackmap.dll $^ ./Release/stdafx.obj /NOLOGO /ERRORREPORT:PROMPT

#environment
#INCLUDE=C:\Program Files\Microsoft Visual Studio 9.0\VC\include;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\include;C:\Program Files\Microsoft SDKs\Windows\v6.0A\\include;C:\Program Files\Microsoft SDKs\Windows\v6.0A\include
#LIBPATH=C:\WINDOWS\Microsoft.NET\Framework\v2.0.50727;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\lib
#LIB=C:\Program Files\Microsoft Visual Studio 9.0\VC\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib;C:\Program Files\Microsoft Visual Studio 9.0\VC\atlmfc\lib\i386;C:\Program Files\Microsoft SDKs\Windows\v6.0A\\lib;C:\Program Files\Microsoft SDKs\Windows\v6.0A\lib;C:\Program Files\Microsoft Visual Studio 9.0\;C:\Program Files\Microsoft Visual Studio 9.0\lib

