#include "stdafx.h"

#ifdef MODULE_AUTOENCHANT

#define MAX_PLAYERS 256
#define MAX_MONSTERS 4096
static int playerEnchantMs[MAX_PLAYERS];
static int monsterEnchantMs[MAX_MONSTERS];
static int startEnchantMs,checkMs,needHelpId;
static int enchantEnable=0;
int needOpenScreen=0;
void AutoEnchantNewGame() {
	memset(playerEnchantMs,0,MAX_PLAYERS*sizeof(int));
	memset(monsterEnchantMs,0,MAX_MONSTERS*sizeof(int));
	startEnchantMs=dwCurMs+3000;//进入游戏3秒后开始强化
	checkMs=dwCurMs+3000;
	enchantEnable=0;dwAutoEnchantGroupId=0;needHelpId=0;
}
int getFcrMs(UnitAny *pUnit) {
	int fcrMs;
	int fcr = D2GetUnitStat(pUnit, STAT_FCR, 0);
	if (fcr>=200) fcrMs=280;
	else if (fcr>=105) fcrMs=320;
	else if (fcr>=63) fcrMs=360;
	else if (fcr>=37) fcrMs=400;
	else if (fcr>=20) fcrMs=440;
	else if (fcr>=9) fcrMs=480;
	else fcrMs=520;
	fcrMs+=40;
	return fcrMs;
}
static int needEnchant(int enchantMs,int id,UnitAny *pUnit,int def) {
	if (pUnit->dwUnitType==UNITNO_PLAYER&&id==needHelpId) {needHelpId=0;return 1;}
	if (dwCurMs<enchantMs+3000) return 0; //刚刚强化
	if (pUnit&&!D2CheckUnitState(pUnit, 16)) return 1; //未强化
	if (dwCurMs>=enchantMs+(int)dwRenewEnchantSeconds*1000) return 1; //需要更新
	return def;
}

static int doEnchant(UnitAny *pUnit) {
	DWORD dwUnitId = pUnit->dwUnitId; 
	int unitType=pUnit->dwUnitType;
	int pos,*ptime;
	if ( pUnit->dwUnitType==UNITNO_PLAYER ){
		if (pUnit->dwUnitId == dwPlayerId) return 0;//不给自己强化
		pos=pUnit->dwUnitId%MAX_PLAYERS;
		ptime=&playerEnchantMs[pos];
	} else if ( pUnit->dwUnitType==UNITNO_MONSTER ){
		dwUnitId = D2GetMonsterOwner(pUnit->dwUnitId);
		if (dwUnitId == (DWORD)-1) return 0; //非玩家随从
		pos=pUnit->dwUnitId%MAX_MONSTERS;
		ptime=&monsterEnchantMs[pos];
	} else {
		return 0;
	}
	if (!needEnchant(*ptime,pUnit->dwUnitId,pUnit,0)) return 0;
	if (!sameParty(dwPlayerId, dwUnitId )) return 0;
	int sLvl=getSkillLevel(PLAYER,52);
	if (dwPlayerMana<sLvl+24
		||dwPlayerMana<dwEnchantMana) { //Not enough mana
		startEnchantMs=dwCurMs+500;//Check mana again after 500ms
		delayScreenSaver(dwAutoEnchantScreenSaverDelayMs);
		dwAutoEnchantGroupId=0;
		return 0;
	}
	*ptime=dwCurMs;
	startEnchantMs=dwCurMs+getFcrMs(PLAYER);
	delayScreenSaver(dwAutoEnchantScreenSaverDelayMs);
	dwAutoEnchantGroupId=0;
	if (dwRightSkill !=52) { //Right skill is enchant
		LOG("FATAL: Auto enchant error\r\n");
		return 1;
	}
	if (fPlayerInTown&&pUnit->dwUnitType==UNITNO_PLAYER) {
		BYTE cast[9] = {0x0D};
		*(DWORD*)&cast[1] = pUnit->dwUnitType;
		*(DWORD*)&cast[5] = pUnit->dwUnitId;
		SendPacket(cast,sizeof(cast));		
	} else {
		RightSkill(pUnit,pUnit->pMonPath->wPosX,pUnit->pMonPath->wPosY);
	}
	return 1;
}
void AutoEnchantPlayerNeedHelp(int id) {
	if (!sameParty(dwPlayerId, id )) return;
	delayScreenSaver(dwAutoEnchantScreenSaverDelayMs);
	needHelpId=id;
}
void AutoEnchantRunLoop() {
	enchantEnable=tAutoEnchant.isOn&&dwRightSkill==52; //Right skill is enchant
	if (!enchantEnable) {
		if (dwAutoEnchantGroupId) dwAutoEnchantGroupId=0;
		return;
	}
	if (dwCurMs>=checkMs) {
		if (fScreenSaverActive&&enchantEnable) {
			//if any player appears around, open screen if it is closed
			if (needOpenScreen) {
				delayScreenSaver(dwAutoEnchantScreenSaverDelayMs);
				needOpenScreen=0;
			}
		}
		checkMs=dwCurMs+1000;
	}
	if (dwAutoEnchantGroupId<=0) {
		if (dwCurMs>=startEnchantMs&&!fScreenSaverActive) dwAutoEnchantGroupId=1; //开始强化玩家
	} else if (dwAutoEnchantGroupId==dwAutoEnchantGroup) { //已完成所有组的强化
		startEnchantMs=dwCurMs+1000;//check again after 1000ms
		dwAutoEnchantGroupId=0;
	} else {
		dwAutoEnchantGroupId++; //强化下一组
	}
}
void AutoEnchantUnit(UnitAny *pUnit) {
	if (!dwAutoEnchantGroupId) return;
	if (pUnit->dwUnitType==UNITNO_PLAYER&&pUnit->dwUnitId==needHelpId) {
		doEnchant(pUnit);needHelpId=0;return;
	}
	int grp=0;
	if (pUnit->dwUnitType==UNITNO_PLAYER) {
		if (DIFFICULTY==2&&(pUnit->dwTxtFileNo==1||pUnit->dwTxtFileNo==2)) //Don't enchant nec and sor in hell difficulty
			grp=100;
		else
			grp=1;
	} else if (0<=pUnit->dwTxtFileNo&&pUnit->dwTxtFileNo<1024) grp=dwAutoEnchantMonsterGroup[pUnit->dwTxtFileNo];
	if (grp&&grp<=(int)dwAutoEnchantGroupId) doEnchant(pUnit);
	return ;
}
/*
The following code add/remove pUnit to/from hashtable when any unit come close or leave
6FB56505 - 8B 86 C4000000        - mov eax,[esi+000000C4] <-----new unit esi
need to save esi,ebx,edi
6FB5662B - 89 34 90              - mov [eax+edx*4],esi <--------- add unit
6FB56F2B - 89 04 96              - mov [esi+edx*4],eax <---------remove unit
when any player appears, open the screen so it can be scanned.
*/
void __declspec(naked) AddUnitPatch_ASM() {
	__asm {
		mov ecx,esi
		cmp enchantEnable,0
		je end
		cmp fScreenSaverActive,0
		je end
		mov eax,[esi]
		cmp eax,0
		jne end
		mov needOpenScreen,1
end:
		mov eax,[esi+0xC4]
		ret
	}
}
void __fastcall GamePacketPlaySound(BYTE* buf) {
	int unitType=buf[1];
	int unitId=*(int *)(buf+2);
	int snd=*(short *)(buf+6);
	//19:Help 1a:followme 1b:thisisforyou 1c:thanks 1d:sorry 1e:bye 1f:die 20:retreat
	if (snd==0x19) AutoEnchantPlayerNeedHelp(unitId);
	/*if (unitType==1) {
		wchar_t wszbuf[256];
		wsprintfW(wszbuf, L"sound %d:%d %d",unitType,unitId,snd);
		D2ShowGameMessage(wszbuf, 0); 
	}*/
}
void __declspec(naked) RecvCommand_2C_Patch_ASM() {
	__asm {
		pushad
		call GamePacketPlaySound;
		popad
		//original code
		mov dl, byte ptr [ecx+1]
		xor eax,eax
		ret
	}
}

#endif
