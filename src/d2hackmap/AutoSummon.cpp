#include "stdafx.h"

#define MAX_MONSTERS 4096
int dwNecMana,dwNecInfoExpireMs;
int fAutoSummonSwitchSkill,fAutoSummonSwitchSkillTmp;
static int monsterSummonMs[MAX_MONSTERS];
static int reviveMs[MAX_MONSTERS];
static int reviveId[MAX_MONSTERS];
static float minD;
static int minUnitId,revivePriority,lastSummonMs,seletonLvl,mageLvl;
static int fcrMs,dwSkillChangeCountVerify;
static int checkSeletonMs;
void AutoSummonNewGame() {
	memset(monsterSummonMs,0,MAX_MONSTERS*sizeof(int));
	memset(reviveMs,0,MAX_MONSTERS*sizeof(int));
	memset(reviveId,0,MAX_MONSTERS*sizeof(int));
	lastSummonMs=dwCurMs;
	dwAutoSummonStartMs=dwCurMs+1000;
	dwSkeletonHPPercent=0;dwNecInfoExpireMs=0;
	dwAutoSummonMode=0;minUnitId=0;seletonLvl=0;mageLvl=0;dwSkeletonMaxCount=0;dwSkeletonMageMaxCount=0;checkSeletonMs=0;
	dwSkeletonCount=0;dwSkeletonMageCount=0;
	dwReviveCount=0;dwReviveMaxCount=0;
	fcrMs=600;dwSkillChangeCountVerify=0;
	fAutoSummonSwitchSkill=0;fAutoSummonSwitchSkillTmp=0;
}
static int getSummonCount(int lvl) {return lvl<=3?lvl:lvl/3+2;}
static void checkSummonSkillLevel() {
	fcrMs=getFcrFrame(dwPlayerClass,PLAYER);
	int sLvl=getSkillLevel(PLAYER,70);
	int mLvl=getSkillLevel(PLAYER,80);
	if (EXPANSION) {sLvl+=dwAutoSummonSkeletonLevelAdjust;mLvl+=dwAutoSummonSkeletonMageLevelAdjust;}
	seletonLvl=sLvl;mageLvl=mLvl;
	int lastS=dwSkeletonMaxCount,lastM=dwSkeletonMageMaxCount,lastR=dwReviveMaxCount;
	dwSkeletonMaxCount=getSummonCount(sLvl);dwSkeletonMageMaxCount=getSummonCount(mLvl);
	dwReviveMaxCount=getSkillLevel(PLAYER,95);
	if (EXPANSION) dwReviveMaxCount+=dwAutoSummonReviveLevelAdjust;
	if (lastS!=dwSkeletonMaxCount||lastM!=dwSkeletonMageMaxCount||lastR!=dwReviveMaxCount) 
		gameMessage("AutoSummon level=%d,%d count=%d,%d,%d",
			sLvl,mLvl,dwSkeletonMaxCount,dwSkeletonMageMaxCount,dwReviveMaxCount);
}
void checkSkeletonCount() {
	int hp=0,ns=0,nm=0,nr=0,nrt=0,time=0;
	for (PetUnit *pPetUnit = *p_D2PetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) {
		if (pPetUnit->dwOwerType!=UNITNO_PLAYER) continue;
		if (pPetUnit->dwOwnerId!=dwPlayerId) continue;
		//if (pPetUnit->dwHpPercent==0) continue;
		switch (pPetUnit->dwPetType) {
			case 3:break;//Golem
			case 4:hp+=pPetUnit->dwHpPercent;ns++;break;
			case 5:hp+=pPetUnit->dwHpPercent;nm++;break;
			case 6:{
				nr++; 
				if (reviveId[pPetUnit->dwUnitId%MAX_MONSTERS]!=pPetUnit->dwUnitId) {
					reviveId[pPetUnit->dwUnitId%MAX_MONSTERS]=pPetUnit->dwUnitId;
					reviveMs[pPetUnit->dwUnitId%MAX_MONSTERS]=dwCurMs;
				}
				int ms=dwCurMs-reviveMs[pPetUnit->dwUnitId%MAX_MONSTERS];
				time+=ms;nrt++;
				break;
			}
		}
	}
	dwSkeletonCount=ns;dwSkeletonMageCount=nm;dwReviveCount=nr;
	dwSkeletonHPPercent=!dwSkeletonCount&&!dwSkeletonMageCount?0:hp/(dwSkeletonCount+dwSkeletonMageCount);
	dwReviveTimePercent=!nrt?0:time/nrt/1800;
	dwNecMana=dwPlayerMana;dwNecInfoExpireMs=dwCurMs+1000;
	if (dwMultiClientFollowId) {
		follower_send_info(0x01000000|(dwSkeletonCount<<16)|(dwSkeletonMageCount<<8)|dwReviveCount);
		follower_send_info(0x02000000|(dwSkeletonHPPercent<<16)|(dwReviveTimePercent<<8));
		follower_send_info(0x03000000|dwPlayerMana);
	}
}

static void switchSummonSkill(int skill) {
	fAutoSummonSwitchSkillTmp=1;selectSkill(1,skill);
	dwAutoSummonMode=0;dwAutoSummonStartMs=dwCurMs+dwSwitchSkillMs;
}
void AutoSummonRunLoop() {
	if (fPlayerInTown) return;
	//check skill level
	if (dwSkillChangeCountVerify!=dwSkillChangeCount||!seletonLvl&&!mageLvl) {
		checkSummonSkillLevel();
		dwSkillChangeCountVerify=dwSkillChangeCount;
	}
	//check skeleton count
	if (!dwAutoSummonStartMs||!checkSeletonMs||dwCurMs>=checkSeletonMs) {
		checkSeletonMs=dwCurMs+500;
		if (!dwAutoSummonStartMs) dwAutoSummonStartMs=dwCurMs;
		checkSkeletonCount();
	}
	if (!tAutoSummon.isOn
		||(dwRightSkill!=70&&dwRightSkill!=80&&!(tAutoSummonRevive.isOn&&dwRightSkill==95)) 
		||dwCurMs<dwAutoSummonStartMs) {
		dwAutoSummonMode=0;
		return;
	}
	if (dwAutoSummonMode>1) checkSkeletonCount();
	if (fAutoSummonSwitchSkill) {
		if (dwSkeletonCount<dwSkeletonMaxCount) {
			if (dwRightSkill!=70) {switchSummonSkill(70);return;}
		} else if (dwSkeletonMageCount<dwSkeletonMageMaxCount) {
			if (dwRightSkill!=80) {switchSummonSkill(80);return;}
		} else if (tAutoSummonRevive.isOn&&dwReviveCount<dwReviveMaxCount) {
			if (dwRightSkill!=95) {switchSummonSkill(95);return;}
		}
	}
	//check mana
	if (dwPlayerMana<dwAutoSummonMana
		||dwRightSkill==70&&dwPlayerMana<seletonLvl+5
		||dwRightSkill==80&&dwPlayerMana<mageLvl+7
		||dwRightSkill==95&&dwPlayerMana<45) { //Not enough mana
		dwAutoSummonStartMs=dwCurMs+500;//Check mana again after 500ms
		return;
	}
	if (dwRightSkill==70&&dwSkeletonCount>=dwSkeletonMaxCount) {
		if (dwSkeletonMageCount<dwSkeletonMageMaxCount) {switchSummonSkill(80);return;}
		if (tAutoSummonRevive.isOn&&dwReviveCount<dwReviveMaxCount) {switchSummonSkill(95);return;}
		dwAutoSummonStartMs=dwCurMs+1000;return;
	}
	if (dwRightSkill==80&&dwSkeletonMageCount>=dwSkeletonMageMaxCount) {
		if (dwSkeletonCount<dwSkeletonMaxCount) {switchSummonSkill(70);return;}
		if (tAutoSummonRevive.isOn&&dwReviveCount<dwReviveMaxCount) {switchSummonSkill(95);return;}
		dwAutoSummonStartMs=dwCurMs+1000;return;
	}
	if (dwRightSkill==95&&dwReviveCount>=dwReviveMaxCount) {
		if (dwSkeletonCount<dwSkeletonMaxCount) {switchSummonSkill(70);return;}
		if (dwSkeletonMageCount<dwSkeletonMageMaxCount) {switchSummonSkill(80);return;}
		dwAutoSummonStartMs=dwCurMs+1000;return;
	}
	if (fScreenSaverActive) delayScreenSaver(60);
	if (dwAutoSummonMode==0) {
		dwAutoSummonMode=1;
		//LOG("start scan %d %d\n",tAutoSummon.value32,LEVELNO);
		minD=10000;minUnitId=0;revivePriority=1;
	} else if (dwAutoSummonMode==1) {
		if (!fAutoFollowMoving&&minUnitId&&canUseSkillNow(PLAYER,dwRightSkill)) {
			dwAutoSummonMode=2;
		} else {
			if (!fWinActive&&!minUnitId) dwAutoSummonStartMs=dwCurMs+500; //run in background no corporse
			else {minD=10000;minUnitId=0;revivePriority=1;} //rescan
		}
	} else {
		dwAutoSummonMode=0;
	}
}
void AutoSummonUnit(UnitAny *pMon) {
	if (!dwAutoSummonMode) return;
	if (pMon->dwUnitType!=UNITNO_MONSTER) return;
	if (pMon->dwMode && pMon->dwMode != 0x0C) return;
	if (!D2CheckCorpseCanUse(pMon,0)) return;
	int ms=monsterSummonMs[pMon->dwUnitId%MAX_MONSTERS];
	if (ms && dwCurMs< ms + 5000) return; //just sommoned
	if (D2CheckUnitState(pMon, 0x76)) return;//used corpse
	int priority=1;
	if (dwRightSkill==95) { //revive
		MonsterTxt *pMonTxt= pMon->pMonsterData->pMonsterTxt;
		if (pMonTxt->fBoss||pMon->pMonsterData->fBoss||pMon->pMonsterData->fUnique||pMon->pMonsterData->fChamp)
			return;
		if (D2UnitVisionBlocked(PLAYER,pMon,2)) return;
		int txt=pMon->dwTxtFileNo;
		if (txt<0||txt>=1024) return;
		priority=aAutoReviveMonster[txt];
		if (priority<revivePriority) return;
	}
	if (dwAutoSummonMode==1) { //find closest unit
		int dx=PLAYER->pMonPath->wPosX-pMon->pMonPath->wPosX;
		int dy=PLAYER->pMonPath->wPosY-pMon->pMonPath->wPosY;
		float d=sqrt((float)(dx*dx+dy*dy))*2/3;
		if (d<33&&d<minD) {minD=d;minUnitId=pMon->dwUnitId;revivePriority=priority;}
	} else if (pMon->dwUnitId==minUnitId) {
		dwAutoSummonMode=0;
		dwAutoSummonStartMs=dwCurMs+fcrMs*40-50;
		monsterSummonMs[pMon->dwUnitId%MAX_MONSTERS]=dwCurMs;
		RightSkill(pMon,pMon->pMonPath->wPosX,pMon->pMonPath->wPosY);
		lastSummonMs=dwCurMs;
	}
}
