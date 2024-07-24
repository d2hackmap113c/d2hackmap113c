#include "stdafx.h"

#define MAX_MONSTERS 4096
static int monsterSummonMs[MAX_MONSTERS];
static int minD,minUnitId,lastSummonMs,seletonN0,mageN0,seletonN,mageN;
void AutoSummonNewGame() {
	memset(monsterSummonMs,0,MAX_MONSTERS*sizeof(int));
	lastSummonMs=dwCurMs;
	dwAutoSummonStartMs=dwCurMs+1000;
	dwAutoSummonMode=0;minUnitId=0;seletonN0=0,mageN0=0;seletonN=0;mageN=0;
}
static int getSummonCount(int lvl) {return lvl<=3?lvl:lvl/3+2;}
static void checkSummonSkillLevel() {
	int sLvl=0,mLvl=0;
	for (struct Skill *p=PLAYER->pSkill->pFirstSkill;p;p=p->pNextSkill) {
		int id=p->pSkillInfo->wSkillId;
		if (id==70) sLvl=p->dwSkillLevel;
		if (id==80) mLvl=p->dwSkillLevel;
	}
	sLvl+=D2GetUnitStat(PLAYER, 107, 70); //only nec
	mLvl+=D2GetUnitStat(PLAYER, 107, 80); //only nec
	int s=D2GetUnitStat(PLAYER, 97, 70); 
	int m=D2GetUnitStat(PLAYER, 97, 80);
	if (s>3) s=3;if (m>3) m=3;
	sLvl+=s;mLvl+=m;
	int summon = D2GetUnitStat(PLAYER, 188, 2*8+2);
	int nec = D2GetUnitStat(PLAYER, 83, 2);
	int all = D2GetUnitStat(PLAYER, 127, 0);
	int add=summon+nec+all;
	sLvl+=add;mLvl+=add;
	sLvl+=dwAutoSummonSkeletonLevelAdjust;
	mLvl+=dwAutoSummonSkeletonMageLevelAdjust;
	seletonN=getSummonCount(sLvl);
	mageN=getSummonCount(mLvl);
	if (seletonN0!=seletonN||mageN0!=mageN) {
		seletonN0=seletonN;
		mageN0=mageN;
		wchar_t wszbuf[64];
		wsprintfW(wszbuf, L"AutoSummon level=%d,%d count=%d,%d\n",sLvl,mLvl,seletonN,mageN);
		D2ShowGameMessage(wszbuf, 0);
	}
}

static void summonUnit(UnitAny *pUnit) {
	int dwMana = D2GetUnitStat(PLAYER, STAT_MANA, 0)>>8;
	if (dwMana<dwAutoSummonMana) { //Not enough mana
		dwAutoSummonStartMs=dwCurMs+500;//Check mana again after 500ms
		return;
	}
	int n_seleton=0,n_mage=0;
	for (PetUnit *pPetUnit = *p_D2PetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) {
		if (pPetUnit->dwOwerType!=UNITNO_PLAYER) continue;
		if (pPetUnit->dwOwnerId!=dwPlayerId) continue;
		//if (pPetUnit->dwHpPercent==0) continue;
		switch (pPetUnit->dwPetType) {
			case 3:break;//Golem
			case 4:n_seleton++;break;
			case 5:n_mage++;break;
		}
	}
	checkSummonSkillLevel();
	if (dwRightSkill==70&&n_seleton>=seletonN) {
		dwAutoSummonMode=0;
		dwAutoSummonStartMs=dwCurMs+1000;
		if (n_mage<mageN&&hasSkill(80)) {
			selectSkill(1,80);
			dwAutoSummonStartMs=dwCurMs+dwAutoSummonSwitchSkillMs;
		}
		return;
	}
	if (dwRightSkill==80&&n_mage>=mageN) {
		dwAutoSummonMode=0;
		dwAutoSummonStartMs=dwCurMs+1000;
		if (n_seleton<seletonN&&hasSkill(70)) {
			selectSkill(1,70);
			dwAutoSummonStartMs=dwCurMs+dwAutoSummonSwitchSkillMs;
		}
		return;
	}
	dwAutoSummonMode=0;
	dwAutoSummonStartMs=dwCurMs+getFcrFrame(dwPlayerClass,PLAYER)*40-50;
	monsterSummonMs[pUnit->dwUnitId%MAX_MONSTERS]=dwCurMs;
	RightSkill(pUnit,pUnit->pMonPath->wPosX,pUnit->pMonPath->wPosY);
	lastSummonMs=dwCurMs;
}
void AutoSummonRunLoop() {
	if (!tAutoSummon.isOn
		||fPlayerInTown
		||(dwRightSkill!=70&&dwRightSkill!=80) 
		||(tAutoSummon.value32==2&&LEVELNO!=121)
		||dwCurMs<dwAutoSummonStartMs) {
		dwAutoSummonMode=0;
		return;
	}
	if (dwAutoSummonMode==0) {
		dwAutoSummonMode=1;
		//LOG("start scan %d %d\n",tAutoSummon.value32,LEVELNO);
		minD=10000;minUnitId=0;
	} else if (dwAutoSummonMode==1) {
		if (minUnitId&&canUseSkillNow(PLAYER,dwRightSkill)) {
			dwAutoSummonMode=2;
		} else {
			minD=10000;minUnitId=0; //rescan
		}
	} else {
		dwAutoSummonMode=0;
	}
}
void AutoSummonUnit(UnitAny *pUnit) {
	if (!dwAutoSummonMode) return;
	if (pUnit->dwUnitType!=UNITNO_MONSTER) return;
	if (pUnit->dwMode && pUnit->dwMode != 0x0C) return;
	if (!D2CheckCorpseCanUse(pUnit,0)) return;
	int ms=monsterSummonMs[pUnit->dwUnitId%MAX_MONSTERS];
	if (ms && dwCurMs< ms + 5000) return; //just sommoned
	if (D2CheckUnitState(pUnit, 0x76)) return;//used corpse
	if (dwAutoSummonMode==1) { //find closest unit
		int dx=PLAYER->pMonPath->wPosX-pUnit->pMonPath->wPosX;
		int dy=PLAYER->pMonPath->wPosY-pUnit->pMonPath->wPosY;
		if (dx<0) dx=-dx;if (dy<0) dy=-dy;
		int d=dx>dy?dx:dy;
		if (d<minD) {minD=d;minUnitId=pUnit->dwUnitId;}
	} else if (pUnit->dwUnitId==minUnitId) {
		summonUnit(pUnit);
	}
}
