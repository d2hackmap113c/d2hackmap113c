#include "stdafx.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
void dbg_start() {D2ShowGameMessage(L"start", 0);}
void dbg_end() {D2ShowGameMessage(L"end", 0);}

int guessBOLevel(int debug);
int DoTest() {
	//guessBOLevel(1);
	LOG("GamePacketSendInterceptPatch_ASM: %x\n",GamePacketSendInterceptPatch_ASM);
	LOG("start: %x\n",dbg_start);
	LOG("end: %x\n",dbg_end);
	D2EditBox *focus=(*p_D2FocusedControl);
	if (focus) D2SetEditBoxText(focus,L"´ó°×ÍÃ");
/*
	char buf[256];
	fp=fopen("d:\\git\\diablo2\\t\\input.txt","r");if (!fp) return 0;
	fgets(buf,256,fp);
	fclose(fp);
	int id=strtol(buf,0,0);
	return 1;
	*/
	dbg_start();
	singlePlayerSaveGame("d:\\git\\diablo2\\t\\test.d2s");
	dbg_end();
	return 1;
}
