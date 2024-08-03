#include "stdafx.h"

#define MAX_MONSTERS 4096
static float minD;
static int minUnitId,fUseSelfRadiusSkill,bossMs;
static int dwStaticFieldLv;
static float dwStaticFieldDis;
static int dwDecrepifyLv,dwDecrepifyMs;
static int dwDimVisionLv,dwDimVisionMs;
static int dwIceBlastLv,dwIceBlastMana;
static int fcrMs,dwSkillChangeCountVerify;
static int monsterSkillMs[MAX_MONSTERS];

/*
decrepify 33 yard
frost nova 8 yard
chain lightning 24 yard
	(-12,-3)8.24 yes
	(8,-7)7.08 yes
	(8,-8)7.54 yes
	(8,-9)8.02 no
	(-14,-3)9.54 no
horizontal vertial |x|=|y| D=|x|*0.94
diagnal x=00||y==0 D=|x|/1.5
	Decrepify time 3.4+lv*0.6
	Attract 6yard mana17 time estimate 2.1+lv*0.9 	3,3.8,4.8,5.6,6.6,7.4,...
	DimVision mana9 radius=1.7+lv*0.7 time=1.2+lv*0.5
*/
void AutoSkillNewGame() {
	memset(monsterSkillMs,0,MAX_MONSTERS*sizeof(int));
	dwAutoSkillCheckMs=dwCurMs+1000;dwAutoSkillMode=0;
	dwDecrepifyLv=0;dwDimVisionLv=0;fcrMs=600;dwSkillChangeCountVerify=0;
}
void AutoSkillRunLoop() {
	//static int lastMode=-1;if (PLAYER->dwMode!=lastMode) {lastMode=PLAYER->dwMode;//LOG("mode=%x\n",lastMode);}
	if (dwSkillChangeCountVerify!=dwSkillChangeCount||!dwDecrepifyLv) {
		dwDecrepifyLv=getSkillLevel(PLAYER,87);
		dwDecrepifyMs=3400+dwDecrepifyLv*600;
		dwDimVisionLv=getSkillLevel(PLAYER,71);
		dwDimVisionMs=1200+dwDimVisionLv*600;
		dwStaticFieldLv=getSkillLevel(PLAYER,42);
		dwStaticFieldDis=dwStaticFieldLv==0?0:4+(dwStaticFieldLv-2)*2.0f/3.0f;
		dwIceBlastLv=getSkillLevel(PLAYER,45);
		dwIceBlastMana=(11+dwIceBlastLv+1)/2;
		fcrMs=getFcrFrame(dwPlayerClass,PLAYER)*40;
		dwSkillChangeCountVerify=dwSkillChangeCount;
	}
	if (!tAutoSkill.isOn
		||fPlayerInTown
		||dwCurMs<dwAutoSkillCheckMs) {
		dwAutoSkillMode=0;
		return;
	}
	int need_mana;
	switch (dwRightSkill) {
		case 87://decrepify
			need_mana=dwPlayerMana<11;
			break;
		case 71://dim vision
			need_mana=dwPlayerMana<9;
			break;
		case 42: //static field
			if (fAutoFollowMoving||fWinActive||!dwAutoStaticFieldDistance||!canUseSkillNow(PLAYER,dwRightSkill)) {
				dwAutoSkillMode=0;return;
			}
			need_mana=dwPlayerMana<9;
			break;
		default:return;
	}
	if (need_mana) {
		dwAutoSkillCheckMs=dwCurMs+dwAutoSkillCheckInterval;//Check mana again after 500ms
		return;
	}
	if (fScreenSaverActive) delayScreenSaver(60);
	if (dwAutoSkillMode==0) {
		dwAutoSkillMode=1;
		minD=10000;minUnitId=0;fUseSelfRadiusSkill=0;
	} else if (dwAutoSkillMode==1) {
		if (minUnitId||fUseSelfRadiusSkill) { //has a target
			if (fAutoFollowMoving||!canUseSkillNow(PLAYER,dwRightSkill)) { //can't use skill now
				minD=10000;minUnitId=0; //rescan
			} else {
				if (minUnitId) dwAutoSkillMode=2;
				else if (fUseSelfRadiusSkill) {
					RightSkill(NULL,PLAYER->pMonPath->wPosX,PLAYER->pMonPath->wPosY);
					dwAutoSkillMode=0;dwAutoSkillCheckMs=dwCurMs+fcrMs-50;
				}
			}
		} else {
			dwAutoSkillCheckMs=dwCurMs+dwAutoSkillCheckInterval;
		}
	} else {
		dwAutoSkillMode=0;
	}
}
void AutoSkillUnit(UnitAny *pMon) {
	if (!dwAutoSkillMode) return;
	if (pMon->dwUnitType!=UNITNO_MONSTER) return;
	if (pMon->dwMode == 0x0C) return; //already dead
	int owner = D2GetMonsterOwner(pMon->dwUnitId);
	if (owner != -1) return; //Íæ¼ÒËæ´Ó
	MonsterTxt *pMonTxt= pMon->pMonsterData->pMonsterTxt;
	int isActBoss=0,isBoss=0,notMonster=0;
	switch (pMon->dwTxtFileNo) {
		case 156: //Andariel
		case 211: //Duriel
		case 242: //Mephisto
		case 243: //Diablo
		case 544: //Baal
			isActBoss=1;isBoss=1;
			break;
		case 283:
			notMonster=1;
			break;
		default:
			if (pMonTxt->fBoss||pMon->pMonsterData->fBoss||pMon->pMonsterData->fUnique||pMon->pMonsterData->fChamp )
				isBoss=1;
	}
	if (notMonster) return;
	if (!isBoss&&!(pMonTxt->nflag1&1)) return; //npc
	/*
 TXT14 flags=09,a0,01,12 Ô{Öä¹Ö
 TXT19 flags=3b,a0,41,02 ÉòœSÄ§
 TXT22 flags=3b,a0,41,02 ºÚ°µÄ§
 TXT58 flags=19,a0,01,02 ÉòœSÎ×ŽŸ
 TXT61 flags=19,a0,01,02 ºÚ°µÎ×ŽŸ
 TXT308 flags=03,a0,41,02 ‡ŠÍÂŒÆów¹Ö
 TXT466 flags=03,80,20,02 ¿ñy±ù·âºì`
 TXT156 flags=c8,a0,00,06 °²ß_Àû –
 TXT267 flags=40,a0,00,03 ÑªøB

 TXT211 flags=c2,a0,00,06 ¶¼Èð –£¨ÎÒÄÜµô30£©
 TXT227 flags=02,80,01,02 ÇùÏx
 TXT283 flags=03,80,01,02 Ó×Ïx
 TXT149 flags=02,04,01,02 Ò»¹ÉÐ°ºÁ¦Á¿ dummy chicken
 TXT152 flags=02,85,01,02 NPC Ò»¹ÉÐ°ºÁ¦Á¿ dummy RogueTown
 TXT404 flags=02,80,04,02 ±»À§µÄì`»ê
 TXT408 flags=02,07,01,03 NPC ¹þµÂ –
	*/
	int ms=monsterSkillMs[pMon->dwUnitId%MAX_MONSTERS];
	if (ms && dwCurMs< ms+1000) return; //just used skill
	if (dwRightSkill==87) {//decrepify
		if (!isBoss&&!dwAutoDecripifyDistance) return;
		if (D2CheckUnitState(pMon, 60)) { //decrepify
			if (isActBoss) {
				if (dwCurMs<bossMs) return;
			} else {
				return;
			}
		}
	} else if (dwRightSkill==71) { //Dim vision
		if (isBoss) return;
		if (!aAutoDimVisionMonster[pMon->dwTxtFileNo]) return;
		if (D2CheckUnitState(pMon, 23)) {
			if (ms&&dwCurMs<ms+dwDimVisionMs-fcrMs-dwAutoSkillCheckInterval) return;
		}
	} else if (dwRightSkill==42) { //static field
		if (D2GetUnitStat(pMon, 41, 0) >= 100) return; //lightning immune
	}
	if (D2UnitVisionBlocked(PLAYER,pMon,2)) return;
	if (dwAutoSkillMode==1) { //find closest unit
		float d=getUnitDistance(PLAYER,pMon);
		switch (dwRightSkill) {
			case 71: //DimVision
				if (d<33&&d<minD) {minD=d;minUnitId=pMon->dwUnitId;}
				break;
			case 87://decrepify
				if (isActBoss&&d<33||isBoss&&d<=dwAutoDecripifyBossDistance||d<dwAutoDecripifyDistance) {
					if (d<minD) {minD=d;minUnitId=pMon->dwUnitId;}
				}
				break;
			case 42: //static field
				if (!fUseSelfRadiusSkill&&d<dwAutoStaticFieldDistance&&d<dwStaticFieldDis) {
					 int hp=D2GetUnitStat(pMon, STAT_HP, 0)>>8;
					 if (hp>=dwAutoStaticFieldHP) fUseSelfRadiusSkill=1;
				}
				if (dwLeftSkill==45&&d<dwIceBlasterDistance&&dwPlayerMana>=dwIceBlastMana) { //Ice Blast
					if (D2GetUnitStat(pMon, 43, 0)<100&&!D2CheckUnitState(pMon, 1)) { //not cold immune, not freeze
						if (d<minD) {minD=d;minUnitId=pMon->dwUnitId;}
					}
				}
				break;
		}
	} else if (pMon->dwUnitId==minUnitId) {
		dwAutoSkillMode=0;
		switch (dwRightSkill) {
			case 71: //DimVision
			case 87://decrepify
				if (isActBoss) bossMs=dwCurMs+dwDecrepifyMs-fcrMs-50;
				RightSkill(pMon,pMon->pMonPath->wPosX,pMon->pMonPath->wPosY);
				monsterSkillMs[pMon->dwUnitId%MAX_MONSTERS]=dwCurMs;
				dwAutoSkillCheckMs=dwCurMs+fcrMs-50;
				break;
			case 42: //static field
				if (dwLeftSkill==45) { //Ice Blast
					dwAutoSkillCheckMs=dwCurMs+fcrMs-50;
					LeftClickObject(pMon);
				}
				break;
		}
	}
}
