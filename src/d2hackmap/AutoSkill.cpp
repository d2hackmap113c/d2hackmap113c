#include "stdafx.h"

#define MAX_MONSTERS 4096
static float minD;
static int minUnitId,bossMs;
static int dwDecrepifyLv,dwDecrepifyMs;
static int dwDimVisionLv,dwDimVisionMs;
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
		fcrMs=getFcrFrame(dwPlayerClass,PLAYER)*40;
		dwSkillChangeCountVerify=dwSkillChangeCount;
	}
	if (!tAutoSkill.isOn
		||fPlayerInTown
		||(dwRightSkill!=71&&dwRightSkill!=87)
		||dwCurMs<dwAutoSkillCheckMs) {
		dwAutoSkillMode=0;
		return;
	}
	if (dwRightSkill==87&&dwPlayerMana<11||dwRightSkill==71&&dwPlayerMana<9) { //Not enough mana
		dwAutoSkillCheckMs=dwCurMs+500;//Check mana again after 500ms
		return;
	}
	if (fScreenSaverActive) delayScreenSaver(60);
	if (dwAutoSkillMode==0) {
		dwAutoSkillMode=1;
		minD=10000;minUnitId=0;
	} else if (dwAutoSkillMode==1) {
		if (!fAutoFollowMoving&&minUnitId&&canUseSkillNow(PLAYER,dwRightSkill)) {
			dwAutoSkillMode=2;
		} else {
			if (!minUnitId) {
				dwAutoSkillCheckMs=dwCurMs+500;
			} else {minD=10000;minUnitId=0;} //rescan
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
	int isActBoss=0,isBoss=0;
	switch (pMon->dwTxtFileNo) {
		case 156: //Andariel
		case 211: //Duriel
		case 242: //Mephisto
		case 243: //Diablo
		case 544: //Baal
			isActBoss=1;isBoss=1;
			break;
		default:
			if (pMonTxt->fBoss||pMon->pMonsterData->fBoss||pMon->pMonsterData->fUnique||pMon->pMonsterData->fChamp )
				isBoss=1;
	}
	if (dwRightSkill==87) {//decrepify
		if (!isBoss) return;
		if (D2CheckUnitState(pMon, 60)) { //decrepify
			if (isActBoss) {
				if (dwCurMs<bossMs) return;
			} else {
				return;
			}
		}
	} else if (dwRightSkill==71) { //Dim vision
		if (isBoss) return;
		if (anMonsterColours[pMon->dwTxtFileNo] != (BYTE)0x9B) return; //not dangrous
	}
	int ms=monsterSkillMs[pMon->dwUnitId%MAX_MONSTERS];
	if (ms && dwCurMs< ms+1000) return; //just used skill
	if (D2UnitVisionBlocked(PLAYER,pMon,2)) return;
	if (dwRightSkill==71) { //Dim vision
		if (D2CheckUnitState(pMon, 23)) {
			if (ms&&dwCurMs<ms+dwDimVisionMs-fcrMs-dwAutoSkillCheckInterval) return;
		}
	}

	if (dwAutoSkillMode==1) { //find closest unit
		int dx=PLAYER->pMonPath->wPosX-pMon->pMonPath->wPosX;
		int dy=PLAYER->pMonPath->wPosY-pMon->pMonPath->wPosY;
		float d=sqrt((float)(dx*dx+dy*dy))*2/3;
		switch (dwRightSkill) {
			case 71: //DimVision
				if (d<33&&d<minD) {minD=d;minUnitId=pMon->dwUnitId;}
				break;
			case 87://decrepify
				if (isActBoss&&d<33||d<=dwAutoSkillDistance) {
					if (d<minD) {minD=d;minUnitId=pMon->dwUnitId;}
				}
				break;
		}
	} else if (pMon->dwUnitId==minUnitId) {
		dwAutoSkillMode=0;
		dwAutoSkillCheckMs=dwCurMs+dwAutoSkillCheckInterval;
		if (isActBoss) bossMs=dwCurMs+dwDecrepifyMs-fcrMs-50;
		RightSkill(pMon,pMon->pMonPath->wPosX,pMon->pMonPath->wPosY);
		monsterSkillMs[pMon->dwUnitId%MAX_MONSTERS]=dwCurMs;
	}
}
