#include "stdafx.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void takeWaypointToArea(int level);
extern int fSkipPainting;
int DoTest() {
	fSkipPainting=!fSkipPainting;
	gameMessage("level=%d skill=(%d,%d) mouse=(%d,%d)",
		LEVELNO,
		dwLeftSkill,dwRightSkill,
		p_D2MousePos->y,p_D2MousePos->x);
	//LOG("GamePacketSendInterceptPatch_ASM: %x\n",GamePacketSendInterceptPatch_ASM);
	return 1;
}
