LDFLAGS=/INCREMENTAL:NO /DLL /MANIFEST:NO /SUBSYSTEM:WINDOWS /OPT:REF /OPT:ICF /BASE:"0x68020000" /DYNAMICBASE /NXCOMPAT /MACHINE:X86 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib gdi32.lib

all: obj/d2server.dll

obj/d2server.dll: obj/server.dll mkdll.c
	tcc -run mkdll.c
obj/server.txt: obj/server.dll
	dumpbin.exe /disasm $< > $@
obj/server.dll: obj/server.asm.obj
	link.exe /MAP:obj/server.map /DEBUG /ENTRY:MyPatchInit $(LDFLAGS) $^ /OUT:$@ /NOLOGO /ERRORREPORT:PROMPT
obj/%.asm.obj: %.asm
	ml.exe /Zd /Zf /Zi /c /Fo"$@" $<
