#include "stdafx.h"
#include "header.h"
#include "auto.h"
#include "multi.h"

#define MAX_PLAYERS 256
#define MAX_MONSTERS 4096
#define NOT_ENCHANTED 0x33333333

int getUnitOwnerId(UnitAny *pUnit);
static int playerEnchantMs[MAX_PLAYERS];
static int monsterEnchantMs[MAX_MONSTERS];
static int checkMs,needHelpId;
static int maxMs;
static UnitAny *maxUnit;
int fAutoEnchantCheckUnit=0;
static int enchantLv,dwSkillChangeCountVerify;
void AutoEnchantNewGame() {
	memset(playerEnchantMs,0,MAX_PLAYERS*sizeof(int));
	memset(monsterEnchantMs,0,MAX_MONSTERS*sizeof(int));
	checkMs=dwCurMs+3000;//进入游戏3秒后开始强化
	needHelpId=0;
	enchantLv=0;dwSkillChangeCountVerify=0;
}
static int doEnchant(UnitAny *pUnit) {
	if (!pUnit) return 0;
	if (pUnit->dwUnitType==UNITNO_PLAYER) playerEnchantMs[pUnit->dwUnitId%MAX_PLAYERS]=dwCurMs;
	else if ( pUnit->dwUnitType==UNITNO_MONSTER ) monsterEnchantMs[pUnit->dwUnitId%MAX_MONSTERS]=dwCurMs;
	checkMs=dwCurMs+dwPlayerFcrMs;
	if (fPlayerInTown&&pUnit->dwUnitType==UNITNO_PLAYER) {
		BYTE cast[9];
		if (dwRightSkill==Skill_Enchant) cast[0]=0xd;
		else if (dwLeftSkill==Skill_Enchant) cast[0]=6;
		else return 0;
		*(DWORD*)&cast[1] = pUnit->dwUnitType;
		*(DWORD*)&cast[5] = pUnit->dwUnitId;
		SendPacket(cast,sizeof(cast));		
	} else {
		if (dwRightSkill==Skill_Enchant) RightSkillUnit(pUnit);
		else if (dwLeftSkill==Skill_Enchant) ShiftLeftClickUnit(pUnit);
	}
	return 1;
}
int checkUnit(UnitAny *pUnit) {
	int enchant=0;
	int unitType=pUnit->dwUnitType;
	DWORD dwUnitId = pUnit->dwUnitId; 
	int ms,passed;
	if (pUnit->dwUnitType==UNITNO_PLAYER) {
		if (dwUnitId==dwPlayerId) {
			if (fUsingBow||fUsingCrossBow) enchant=1;
		} else if (DIFFICULTY==0) {
			enchant=1;
		} else {
			if (DIFFICULTY==1&&d2common_GetUnitStat(pUnit, STAT_LEVEL, 0)<=dwAutoEnchantPlayerLevel) {
				enchant=1;goto player_end;
			}
			if (pUnit->dwTxtFileNo!=1&&pUnit->dwTxtFileNo!=2) enchant=1;
			for (UnitAny *pItem = d2common_GetFirstItemInInv(pUnit->pInventory);pItem;pItem = d2common_GetNextItemInInv(pItem)) {
				if (pItem->dwUnitType!=UNITNO_ITEM) continue;
				if (pItem->pItemData->nLocation!=3) continue; //not on body
				if (pItem->pItemData->nItemLocation==255) { //equipped
					switch (pItem->pItemData->nBodyLocation) {
						case 4:case 5:
							ItemTxt *pItemTxt=d2common_GetItemTxt(pItem->dwTxtFileNo);
							switch (pItemTxt->nType) {
								case 27:case 85://bow
								case 35://crossbow
								case 42:case 43://Throwing
								case 44:case 87://Javelins
									enchant=1;
									goto player_end;
							}
							break;
					}
				} 
			}
		}
player_end:
		ms=playerEnchantMs[dwUnitId%MAX_PLAYERS];
		passed=(dwCurMs-ms)<<1;
		if (dwUnitId==needHelpId) {needHelpId=0;enchant=2;}
	} else if ( pUnit->dwUnitType==UNITNO_MONSTER ){
		if (0<=pUnit->dwTxtFileNo&&pUnit->dwTxtFileNo<1024) enchant=dwAutoEnchantMonster[pUnit->dwTxtFileNo];
		dwUnitId = getUnitOwnerId(pUnit);
		if (dwUnitId == (DWORD)-1) return 0; //not owned by player
		ms=monsterEnchantMs[pUnit->dwUnitId%MAX_MONSTERS];
		passed=dwCurMs-ms;
	}
	if (!enchant) return 0;
	if (dwCurMs<ms+3000) return 0; //just enchanted
	if (pUnit->dwUnitType==UNITNO_MONSTER&&!sameParty(dwUnitId)) return 0;
	if (enchant>1||!d2common_CheckUnitState(pUnit, State_Enchant)) {maxUnit=pUnit;maxMs=NOT_ENCHANTED;return 0;}
	if (passed>=dwRenewEnchantMs&&passed>maxMs) {maxMs=passed;maxUnit=pUnit;}
	return 0;
}
void AutoEnchantRunLoop() {
	if (!fAutoEnchant) return;
	if (fAutoFollowMoving) return;
	if (!fAutoEnchantCheckUnit&&dwCurMs<checkMs) return;
	if (dwPlayerMana<dwAutoEnchantMana) return;
	fAutoEnchantCheckUnit=0;
	if (dwSkillChangeCountVerify!=dwSkillChangeCount||!enchantLv) {
		enchantLv=getSkillLevel(PLAYER,Skill_Enchant);
		dwSkillChangeCountVerify=dwSkillChangeCount;
	}
	if (d2client_CheckUiStatus(UIVAR_INTERACT)) return;
	if (!fCanUseRightSkill) return;
	if (dwPlayerMana<enchantLv+24) { //Not enough mana
		checkMs=dwCurMs+500;//Check mana again after 500ms
		return;
	}
	if (PLAYER->dwMode==PlayerMode_Attacking1||PLAYER->dwMode==PlayerMode_Attacking2||PLAYER->dwMode==PlayerMode_Cast) return;
	maxMs=0;maxUnit=NULL;
	for (RosterUnit *pRU=PLAYERLIST;pRU;pRU=pRU->pNext) {
		if (dwPlayerPartyId!=pRU->wPartyId) continue;
		UnitAny *pUnit=d2client_GetUnitFromId(pRU->dwUnitId, UNITNO_PLAYER);if (!pUnit) continue;
		checkUnit(pUnit);
		if (maxMs>=NOT_ENCHANTED) {doEnchant(maxUnit);return;}
	}
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_MONSTER*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (pUnit->dwUnitType!=UNITNO_MONSTER) break;
			checkUnit(pUnit);if (maxMs>=NOT_ENCHANTED) {doEnchant(maxUnit);return;}
		}
	}
	if (maxUnit&&dwPlayerMana>=dwRenewEnchantMana) {doEnchant(maxUnit);return;}
	checkMs=dwCurMs+3000;
}
void AutoEnchantPlayerNeedHelp(int id) {
	if (!sameParty(id)) return;
	needHelpId=id;fAutoEnchantCheckUnit=1;checkMs=dwCurMs;
}
extern ToggleVar t3BBProtect;
void check3bb() {
	static int ms=0;if (dwCurMs<ms) return;ms=dwCurMs+1000;
	wchar_t wszbuf[256];int pos=0;
	pos+=wsprintfW(wszbuf+pos, L"3BB:");
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_MONSTER*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (pUnit->dwUnitType!=UNITNO_MONSTER) break;
			switch (pUnit->dwTxtFileNo) {
				case 540:case 541:case 542: {
					if (DIFFICULTY>=1&&dwCurrentLevel==Level_ArreatSummit&&t3BBProtect.isOn) {
						gameMessageW(L"3BB protect, exit game");
						QuickNextGame(1);
					}
					int dr = d2common_GetUnitStat(pUnit, STAT_DAMAGE_REDUCED,	0);
					//int mr = d2common_GetUnitStat(pUnit, STAT_MAGIC_RESIST,	0);
					int fr = d2common_GetUnitStat(pUnit, STAT_FIRE_RESIST,		0);
					int lr = d2common_GetUnitStat(pUnit, STAT_LIGHTING_RESIST,	0);
					int cr = d2common_GetUnitStat(pUnit, STAT_COLD_RESIST,		0);
					//int pr = d2common_GetUnitStat(pUnit, STAT_POSION_RESIST,	0);
					if (dr|fr|lr|cr) {
						wszbuf[pos++]=' ';
						if (dr)	pos+=wsprintfW(wszbuf+pos, L"D%d", dr);
						//if (mr)	pos+=wsprintfW(wszbuf+pos, L"M%d", mr);
						if (fr)	pos+=wsprintfW(wszbuf+pos, L"F%d", fr);
						if (lr)	pos+=wsprintfW(wszbuf+pos, L"L%d", lr);
						if (cr)	pos+=wsprintfW(wszbuf+pos, L"C%d", cr);
						//if (pr)	pos+=wsprintfW(wszbuf+pos, L"P%d", pr);
					}
					break;
				}
			}
		}
	}
	if (pos>4) partyMessageWColor(2,wszbuf);
}
void __fastcall GamePacketPlaySound(BYTE* buf) {
	int unitType=buf[1];
	int unitId=*(int *)(buf+2);
	int snd=*(short *)(buf+6);
	//19:Help 1a:followme 1b:thisisforyou 1c:thanks 1d:sorry 1e:bye 1f:die 20:retreat
	switch (snd) {
		case 16: if (LEVELNO==120) check3bb();break; 
		case 0x19: AutoEnchantPlayerNeedHelp(unitId);break;
	}
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
