#include "stdafx.h"
#include "header.h"

void loadWaypoints(char *packet,int len);
void saveWaypoints(char *packet,int len);

int dwTakeWaypointToLevel=0;
char waypointBits[8]={0};
const int waypointAreas[5][9]={
{1,3,4,5,6,27,29,32,35},
{40,48,42,57,43,44,52,74,46},
{75,76,77,78,79,80,81,83,101},
{103,106,107,0,0,0,0,0,0},
{109,111,112,113,115,123,117,118,129},
};
int waypointMap[140]={0};

void waypoint_initConfigVars() {
	int id=0;
	for (int act=0;act<5;act++) {
		for (int i=0;i<9;i++) {
			int lvl=waypointAreas[act][i];if (!lvl) break;
			waypointMap[lvl]=id++;
		}
	}
}
int waypointExist(int area) {return area==1||area<140&&waypointMap[area];}
int hasWaypoint(int area) {
	if (area==1) return 1;
	if (area>=140) return 0;
	int bit=waypointMap[area];if (bit==0) return 0;
	return waypointBits[bit>>3]&(1<<(bit&7));
}
void WaypointNewGame() {
	loadWaypoints(waypointBits,5);
}
int isWaypointTxt(int txt) {
	switch (txt) {
		case 119:case 156:case 157:case 237:case 238:case 288:case 323:
		case 324:case 398:case 402:case 429:case 496:case 511:
			return 1;
	}
	return 0;
}
UnitAny *findWaypoint(int maxdis) {
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_OBJECT*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (pUnit->dwUnitType!=UNITNO_OBJECT) break;
			switch (pUnit->dwTxtFileNo) {
				case 119:case 156:case 157:case 237:case 238:case 288:case 323:
				case 324:case 398:case 402:case 429:case 496:case 511:
					if (getPlayerDistanceYard(pUnit)<=maxdis)
						return pUnit;
			}
		}
	}
	return NULL;
}
void takeWaypointToLevel(UnitAny *waypoint,int level) {
	dwTakeWaypointToLevel=level;
	BYTE packet[9];packet[0]=0x13;
	*(int *)(packet+1)=waypoint->dwUnitType;
	*(int *)(packet+5)=waypoint->dwUnitId;
	SendPacket(packet,sizeof(packet));
}
int takeWaypointToLvl(int lvl,int maxdis) {
	UnitAny *waypoint=findWaypoint(maxdis);
	if (!waypoint) return 0;
	takeWaypointToLevel(waypoint,lvl);
	return 1;
}

// 00: 63 1b 00 00 - 00 02 01 ff - ff ff ff 77 - 00 00 00 00 |c          w    
// 10: 00 00 00 00 - 00          -             -             |                
static int __fastcall recvWaypointMenu(char *packet,int len) {
	int id=*(int *)(packet+1);memcpy(waypointBits,packet+7,5);
	saveWaypoints(packet,len);
	if (1<=dwTakeWaypointToLevel&&dwTakeWaypointToLevel<140) {
		if (dwTakeWaypointToLevel>1&&!hasWaypoint(dwTakeWaypointToLevel)) {
			gameMessage("No waypoint to level %d",dwTakeWaypointToLevel);return 0;
		}
		BYTE packet[9];packet[0]=0x49;
		*(int *)(packet+1)=id;
		*(int *)(packet+5)=dwTakeWaypointToLevel;
		SendPacket(packet,sizeof(packet));
		dwTakeWaypointToLevel=0;
		return 1;
	}
	return 0;
}
/*
ret=6FB5CFB5
can modify: eax,edx
ecx,edi=buf edx=len
6FB5F8B0 - 83 EC 18              - sub esp,18 { 24 }
6FB5F8B3 - 8B 01                 - mov eax,[ecx]
6FB5F8B5 - 8B 51 04              - mov edx,[ecx+04]
*/
void __declspec(naked) RecvCommand_63_Patch_ASM() {
	__asm {
		pushad
		call recvWaypointMenu
		test eax,eax
		jne blockPacket
		popad
		mov eax,[ecx]
		mov edx,[ecx+4]
		ret
blockPacket:
		popad
		add esp,0x1C
		ret
	}
}
