#include "stdafx.h"


#ifdef MODULE_QUICKBACKTOWN

static int lastSkill=-1;
DWORD skillCount(int skillid){
	if (PLAYER->pSkill){
		Skill *pSkill = PLAYER->pSkill->pFirstSkill;
		while(pSkill){
			if(pSkill->pSkillInfo->wSkillId == skillid){
				return pSkill->dwQuality;
			}
			pSkill= pSkill->pNextSkill;
		}
	}
	return 0;
}

int BackToTown() {
	UnitAny *pUnit = PLAYER;
	if( fPlayerInTown ) return 0;
	fBackToTown = TRUE;
	lastSkill=PLAYER->pSkill->pRightSkill->pSkillInfo->wSkillId;

	BYTE selectRightSkill[9] = {0x3C};
	*(DWORD*)&selectRightSkill[1] = skillCount(219)>0?0xDB:0xDC; //use scroll first
	*(DWORD*)&selectRightSkill[5] = 0xFFFFFFFF;
	SendPacket(selectRightSkill,sizeof(selectRightSkill));
	
	BYTE rightSkill[5] = {0x0C};
	*(WORD*)&rightSkill[1] = pUnit->pMonPath->wTargetX;
	*(WORD*)&rightSkill[3] = pUnit->pMonPath->wTargetY;
	SendPacket(rightSkill,sizeof(rightSkill));
	return 0;
}

void __fastcall AutoBackToTown(BYTE *aPacket) {
	//LOG("QuickBackTown packet %02x\n",aPacket[0]);
	/*if (aPacket[0]==0x60) {
		LOG("QuickBackTown state=%d area=%d id=%d\n",
			aPacket[1],aPacket[2],*(int *)(aPacket+3));
	}
	*/
	if( aPacket[0]==0x60 && aPacket[1] == 0x00 ) //00限定只能走通向城里的传送门
	{
		if ( fBackToTown ){
			fBackToTown = FALSE;
			BYTE castMove[9] = {0x13};
			*(DWORD*)&castMove[1] = 2;
			*(DWORD*)&castMove[5] = *(DWORD*)&aPacket[3]; // portal ID
			SendPacket(castMove,sizeof(castMove));
		}

		if( nTownportalAlertNums!= (DWORD)-1 ){
			DWORD srollcount = skillCount(220);
			if ( srollcount<= nTownportalAlertNums ){
				wchar_t temp[512];
				wsprintfW(temp,  L"<Hackmap>: Warning !! Only %d Townportals Left ", srollcount);
				D2ShowGameMessage(temp, 8);
			}
		}

		/*if (lastSkill != -1) {
			BYTE castTP1[9] = {0x3C};
			*(DWORD*)&castTP1[1] = lastSkill;
			*(DWORD*)&castTP1[5] = 0xFFFFFFFF;
			SendPacket(castTP1,sizeof(castTP1));
		}
		*/
	}
}


void __declspec(naked) RecvCommand_60_Patch_ASM() {
	__asm {
		//ecx = cmdbuf
		push ecx
		call AutoBackToTown;
		pop ecx
		pop edx //ret code
//org:
		sub esp, 8
		mov eax, dword ptr [ecx]
		jmp edx
	}
}

#endif
