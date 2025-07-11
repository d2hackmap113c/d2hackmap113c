#include "stdafx.h"
#include "header.h"
#include "auto.h"
#include "multi.h"

#define VARIABLE_DECLARE
#include "auto.h"
#undef VARIABLE_DECLARE
int dwRescueBarSafeDistance;
extern int fUserOperating;
void recheckSelfItems();
int autoCubeTransform();
int autoSimpleItemStack();
int autoIdentify();
static int autoIdentifying=0;

ToggleVar tAutoForegroundSkill2={TOGGLEVAR_DOWN,0,-1, 1,"Auto Foreground Skill 2",&AutoSkillNow};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY, "AutoBackgroundSkillToggle",   &tAutoBackgroundSkill     },
	{CONFIG_VAR_TYPE_KEY, "AutoForegroundSkillToggle",   &tAutoForegroundSkillToggle     },
	{CONFIG_VAR_TYPE_KEY, "AutoForegroundSkillKey",   &tAutoForegroundSkill     },
	{CONFIG_VAR_TYPE_KEY, "AutoForegroundSkillKey2",   &tAutoForegroundSkill2     },
  {CONFIG_VAR_TYPE_INT, "UseLeftSkillFirst",	&dwUseLeftSkillFirst,     4},
  {CONFIG_VAR_TYPE_INT, "AutoSkillCheckInterval",	&dwAutoSkillCheckInterval,     4},
  {CONFIG_VAR_TYPE_INT, "AutoSkillIdleDelay",	&dwAutoSkillIdleDelay,     4},
  {CONFIG_VAR_TYPE_INT, "AutoThrowMinQuantity",	&dwAutoThrowMinQuantity,     4},
  {CONFIG_VAR_TYPE_INT, "AutoReloadThrowing",	&dwAutoReloadThrowing,     4},
  {CONFIG_VAR_TYPE_INT, "AutoStaticFieldHPPercent",	&dwAutoStaticFieldHPPercent,     4},
  {CONFIG_VAR_TYPE_INT, "RescueBarSafeDistance",	&dwRescueBarSafeDistance,     4},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "AutoSkillMonsterType", &aAutoSkillMonsterType,       1, {1024}},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "AutoSkillDistance", &aAutoSkillDistance,       2, {512}},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "AutoDimVisionMonster", &aAutoDimVisionMonster,       1, {1024}},
//--- m_AutoEnchant.h ---
	{CONFIG_VAR_TYPE_KEY, "AutoEnchantToggle",   &tAutoEnchant     },
  {CONFIG_VAR_TYPE_INT, "RenewEnchantMs",				&dwRenewEnchantMs,     4},
  {CONFIG_VAR_TYPE_INT, "AutoEnchantMana",	&dwAutoEnchantMana,     4},
  {CONFIG_VAR_TYPE_INT, "RenewEnchantMana",				&dwRenewEnchantMana,     4},
  {CONFIG_VAR_TYPE_INT, "AutoEnchantPlayerLevel",	&dwAutoEnchantPlayerLevel,     4},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "AutoEnchantMonster",dwAutoEnchantMonster,       1, {1024}},
//--- m_AutoWarCry.h ---
	{CONFIG_VAR_TYPE_KEY, "AutoWarCryToggle",   &tAutoWarCry     },
  {CONFIG_VAR_TYPE_INT, "AutoWarCryRenewPercent",	&dwAutoWarCryRenewPercent,     4},
//--- m_AutoSummon.h ---
	{CONFIG_VAR_TYPE_KEY, "AutoSummonToggle",   &tAutoSummon     },
	{CONFIG_VAR_TYPE_KEY, "AutoSummonReviveToggle",   &tAutoSummonRevive     },
	{CONFIG_VAR_TYPE_KEY, "ShowSummonInfoToggle",   &tShowSummonInfo     },
  {CONFIG_VAR_TYPE_INT, "AutoSummonMana",				&dwAutoSummonMana,     4},
  {CONFIG_VAR_TYPE_INT, "SwitchSkillMs",				&dwSwitchSkillMs,     4},
  {CONFIG_VAR_TYPE_INT, "AutoSummonSkeletonLevelAdjust",				&dwAutoSummonSkeletonLevelAdjust,     4},
  {CONFIG_VAR_TYPE_INT, "AutoSummonSkeletonMageLevelAdjust",				&dwAutoSummonSkeletonMageLevelAdjust,     4},
  {CONFIG_VAR_TYPE_INT, "AutoSummonReviveLevelAdjust",				&dwAutoSummonReviveLevelAdjust,     4},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "AutoReviveMonster",       &aAutoReviveMonster,       1, {1024}},
};
void autoskill_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void autoskill_initConfigVars() {
	memset(dwAutoEnchantMonster,    0,        sizeof(dwAutoEnchantMonster));
	dwRescueBarSafeDistance=16;
	const static int dummy[]={
		//all-1:150,152,153,154,158,159,160,180,186,196,197,198,269,270,284,
		//all-1:294,319,333,340,320,321,322,323,324,325,326,352,353,354,371,378,379,402,0
		146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 157, 
		158, 159, 175, 176, 177, 178, 179, 185, 195, 196, 197, 
		198, 199, 200, 201, 202, 203, 204, 205, 210, 227, 244, 
		245, 246, 251, 252, 253, 254, 255, 257, 264, 265, 266, 
		268, 269, 272, 283, 293, 294, 296, 297, 318, 319, 320, 
		321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 
		332, 339, 344, 351, 352, 353, 355, 358, 359, 366, 367, 
		368, 369, 377, 378, 392, 393, 401, 403, 404, 405, 406, 
		408, 410, 411, 412, 413, 414, 415, 416, 432, 433, 434, 
		435, 511, 512, 513, 514, 515, 516, 517, 518, 519, 520, 
		521, 522,523, 524, 525, 527, 534, 535, 536, 537, 538, 539, 
		543, 545, 556, 559, 567, 568, 569, 574, 0,
		};
	const static int dangerous[]={
		Mon_FrenziedIceSpawn,
		Mon_UndeadStygianDoll1,Mon_UndeadStygianDoll2,
		Mon_Gloam1,Mon_Gloam2,Mon_BurningSoul1,Mon_BurningSoul2,733,Mon_BlackSoul1,Mon_BlackSoul2,
		Mon_SoulKiller3,Mon_SoulKiller4,Mon_UndeadSoulKiller2,0,
	};
	memset(aAutoSkillMonsterType,0,sizeof(aAutoSkillMonsterType));
	for (const int *p=dummy;*p;p++) aAutoSkillMonsterType[*p]|=MONSTER_TYPE_NEUTRAL; 
	for (int i=58;i<=62;i++) aAutoSkillMonsterType[i]|=MONSTER_TYPE_REVIVE; 
	for (const int *p=dangerous;*p;p++) aAutoSkillMonsterType[*p]|=MONSTER_TYPE_DANGROUS; 
	aAutoSkillMonsterType[Mon_Andariel]|=MONSTER_TYPE_MF|MONSTER_TYPE_ACTBOSS;
	aAutoSkillMonsterType[Mon_Duriel]|=MONSTER_TYPE_MF|MONSTER_TYPE_ACTBOSS; //Duriel
	aAutoSkillMonsterType[Mon_Mephisto]|=MONSTER_TYPE_MF|MONSTER_TYPE_ACTBOSS; //Mephisto
	aAutoSkillMonsterType[Mon_Diablo]|=MONSTER_TYPE_MF|MONSTER_TYPE_ACTBOSS; //Diablo
	aAutoSkillMonsterType[Mon_BaalCrab]|=MONSTER_TYPE_MF|MONSTER_TYPE_ACTBOSS; 
	aAutoSkillMonsterType[Mon_Nihlathak]|=MONSTER_TYPE_MF;
	aAutoSkillMonsterType[Mon_TheSummoner]|=MONSTER_TYPE_MF;
	memset(aAutoSkillDistance,0,sizeof(aAutoSkillDistance));
}

enum AutoState {
	STATE_NORMAL=0,
	STATE_PICKING,STATE_STACKING,STATE_PUTING,
};
#define MAX_MONSTERS 0x1000
#define MONSTER_MASK 4096
extern int dwThrowWeaponId,dwNextStackingId,dwNextStackX,dwNextStackY,dwNextStackLoc;
int dwAutoSkillReloading;
static int stackFromId,stackToId,stackX,stackY,stackLoc;
static int reloadingMs,usingAutoSkill;
static int minD2,maxMs,fSelfRadiusSkill,bossMs;
static UnitAny *pMinUnit;
static int minD2_2;
static UnitAny *pMinUnit_2;
struct AutoSkillInfo {
	int isRight,id,lv,ms,valid;
	int onNormal,onBoss,notState,notImmune,delayState,needKeep,selfRadius,melee;
	int umindis2,umaxdis2; //user setting
	int vdis2,maxdis2; //max valid range
};
static struct AutoSkillInfo leftSkill,rightSkill,*curSkill,*nextSkill;
static int dwSkillChangeCountVerify;
static int monsterSkillMs[MAX_MONSTERS];
static int stopAttack;

static void checkUnit(UnitAny *pMon);
int isCowKing(MonsterData *pMonsterData);

/*
monster mode:
	0:Death	DT 1:Neutral	NU 2:Walk	WL 3:Get Hit	GH
	4:Attack1	A1 5:Attack2	A2 6:Block	BL 7:Cast	SC
	8:Skill1	S1 9:Skill2	S2 10:Skill3	S3 11:Skill4	S4
	12:Dead	DD 13:Knockback	GH 14:Sequence	xx 15:Run	RN
player mode:
	0:Death	DT 1:Neutral	NU 2:Walk	WL 3:Run	RN
	4:Get Hit	GH 5:Town Neutral	TN 6:Town Walk	TW 7:Attack1	A1
	8:Attack2	A2 9:Block	BL 10:Cast	SC 11:Throw	TH
	12:Kick	KK 13:Skill1	S1 14:Skill2	S2 15:Skill3	S3
	16:Skill4	S4 17:Dead	DD 18:Sequence	GH 19:Knock back	GH
skill mana time:
	Decrepify mana11 time 3.4+lv*0.6
	LifeTap mana9 time=13.6+2.4*lv
	Attract 6yard mana17 
		normal: time 8.4+lv*3.6 	12,15.6,19.2,22.8
		nightmare: time 4.2+lv*1.8 	12,15.6,19.2,22.8
		hell: time estimate 2.1+lv*0.9 	3,3.8,4.8,5.6,6.6,7.4,...
	DimVision mana9 
		normal: time=5+lv*2
		nightmare: time=2.4+lv
		hell: time=1.2+lv*0.5
	IceBlast mana(11+lv+1)/2  GlacialSpike mana(10+lv/2)
	Blizzard mana(22+lv)
	War Cry 5yard mana=9+lv time=0.8+0.2*lv
	Battle Order 12-13yard mana=7 time=30+10*lv
*/
void AutoSkillNewGame() {
	memset(monsterSkillMs,0,MAX_MONSTERS*sizeof(int));
	dwAutoSkillCheckMs=dwCurMs+1000;
	memset(&leftSkill,0,sizeof(struct AutoSkillInfo));memset(&rightSkill,0,sizeof(struct AutoSkillInfo));
	dwAutoSkillReloading=0;dwSkillChangeCountVerify=-1;usingAutoSkill=0;
}
static int isAutoSkill(Skill *pSkill) {
	int id=pSkill->pSkillInfo->wSkillId;
	if (pSkill->pSkillInfo->flags3&0x30) return 0;//passive skill or aura
	if (!aAutoSkillDistance[id][0]) return 0;
	switch (id) {
		case Skill_Enchant:return 0;
		case Skill_RaiseSkeleton:case Skill_RaiseSkeletalMage:case Skill_Revive:return 0;
		case Skill_Shout:case Skill_BattleOrders:case Skill_BattleCommand:return 0;
		case Skill_Attack:case Skill_ExplodingArrow:return fUsingCrossBow||fUsingBow;
		case Skill_Throw:return fUsingThrow;
		case Skill_FrostNova:case Skill_IceBlast:case Skill_GlacialSpike:case Skill_Blizzard:
		case Skill_FireBolt:case Skill_FireBall:case Skill_Meteor:case Skill_Hydra:
		case Skill_StaticField:case Skill_Nova:case Skill_Lightning:case Skill_ChainLightning:
		case Skill_LifeTap:case Skill_Decrepify:case Skill_DimVision:case Skill_LowerResist:
		case Skill_WarCry:case Skill_Smite:
			return 1;
		default:
			return 1;
	}
	return 0;
}
static void checkAutoSkill1(int skill,int left){
	switch (skill) {
		case Skill_Enchant:fAutoEnchant=1;fAutoEnchantUseLeftSkill=left;break;
		case Skill_Shout:case Skill_BattleOrders:case Skill_BattleCommand:
			fAutoWarCry=1;fAutoWarCryUseLeftSkill=left;break;
		case Skill_Revive:if (!tAutoSummonRevive.isOn) break;
		case Skill_RaiseSkeleton:case Skill_RaiseSkeletalMage:
			fAutoSummon=1;fAutoSummonUseLeftSkill=left;break;
	}
}
void checkAutoSkill() {
	fAutoEnchant=0;fAutoWarCry=0;fAutoSummon=0;
	checkAutoSkill1(dwLeftSkill,1);checkAutoSkill1(dwRightSkill,0);
	if (!tAutoEnchant.isOn) fAutoEnchant=0;
	if (!tAutoWarCry.isOn) fAutoWarCry=0;
	if (!tAutoSummon.isOn) fAutoSummon=0;
	dwSkillChangeCountVerify=-1;
	memset(&leftSkill,0,sizeof(struct AutoSkillInfo));memset(&rightSkill,0,sizeof(struct AutoSkillInfo));
	rightSkill.isRight=1;
	leftSkill.valid=isAutoSkill(pLeftSkill);
	rightSkill.valid=isAutoSkill(pRightSkill);
	checkAutoSkillStatus();
}
void checkAutoSkillStatus() {
	fAutoSkill=leftSkill.valid||rightSkill.valid;
	if (fAutoSkillNow&&(
		dwRightSkill==Skill_Teleport
		||dwRightSkill==Skill_Telekinesis
		||*d2client_pUiCubeOn
		||*d2client_pUiInventoryOn
		||*d2client_pUiStashOn)) fAutoSkill=1;
	if (!fAutoSkill) return;
	fAutoSkill=fWinActive&&(tAutoForegroundSkillToggle.isOn||fAutoSkillNow)&&!fUserOperating
		||!fWinActive&&tAutoBackgroundSkill.isOn;
}
static void updateSkill(struct AutoSkillInfo *info,int id) {
	float d=33;
	int varMs=0;
	int lv=getSkillLevel(PLAYER,id);
	info->id=id;info->lv=lv;info->ms=0;
	info->onBoss=1;info->onNormal=1;info->notState=0;info->notImmune=0;
	info->delayState=0;info->needKeep=0;info->selfRadius=0;info->melee=0;
	switch (id) {
		case Skill_AmplifyDamage:info->ms=5000+lv*3000;info->notState=State_AmplifyDamage;info->delayState=600;break;
		case Skill_LowerResist:info->ms=18000+lv*2000;info->notState=State_LowerRes;info->delayState=600;break;
		case Skill_LifeTap:info->ms=13600+lv*2400;info->notState=State_LifeTap;info->delayState=600;break;
		case Skill_Decrepify:info->ms=3400+lv*600;info->onNormal=0;info->notState=State_Decrepify;//info->mana=11;
			info->delayState=600;info->needKeep=1;break;
		case Skill_Weaken:info->notState=State_Weaken;info->delayState=600;break;
		case Skill_Terror:info->notState=State_Terror;info->delayState=600;break;
		case Skill_DimVision:info->ms=1200+lv*500;info->onBoss=0;info->notState=State_DimVision;varMs=1;
			info->delayState=600;info->needKeep=1;break;
		case Skill_Confuse:info->notState=State_Confuse;info->onBoss=0;info->delayState=600;break;
		case Skill_StaticField:d=lv==0?0:4+(lv-2)*2.0f/3.0f;info->notImmune=41;
			info->selfRadius=1;break;//light immnue
		case Skill_WarCry:info->ms=800+lv*200;info->onBoss=0;d=5;//info->mana=9+lv;
			info->delayState=600;info->notState=State_Stun;info->needKeep=1;info->selfRadius=1;break;
		case Skill_IceBolt:case Skill_IceBlast:d=19;info->notImmune=43;break;//cold immune
		case Skill_GlacialSpike:d=20;info->notState=State_Freeze;info->notImmune=43;break;//cold immune
		case Skill_FrostNova:d=8;info->notImmune=43;break;//cold immune
		case Skill_FrozenOrb:d=20;info->notImmune=43;break;//cold immune
		case Skill_Blizzard:d=33;info->notImmune=43;break;//cold immune
		case Skill_FireBolt:case Skill_FireBall:d=31;info->notImmune=39;break;//fire immune
		case Skill_Meteor:case Skill_Hydra:d=33;info->notImmune=39;break;//fire immune
		case Skill_Attack:case Skill_ExplodingArrow:d=30;info->notImmune=39;break;//fire immune
		case Skill_Nova:d=8;info->notImmune=41;break;//lightning immune
		case Skill_ChargedBolt:d=20;info->notImmune=41;break;//lightning immune
		case Skill_Lightning:case Skill_ChainLightning:d=24;info->notImmune=41;break;//lightning immune
		case Skill_ChargedStrike:d=5;info->melee=1;info->notImmune=41;break;//lightning immune
		case Skill_LightningFury:d=30;info->notImmune=41;break;//lightning immune
		case Skill_Teeth:d=26;info->notImmune=37;break;//magic immune
		case Skill_BoneSpear:d=29;info->notImmune=37;break;//magic immune
		case Skill_BoneSpirit:d=33;info->notImmune=37;break;//magic immune
		case Skill_PoisonNova:d=10;info->notImmune=45;break;//poision immune
		case Skill_Smite:case Skill_Sacrifice:case Skill_Zeal:
			d=20;info->melee=1;info->notImmune=36;break;//physic immune
	}
	info->vdis2=(int)(d*d*9)>>2; //max valid distance^2
	int d0=aAutoSkillDistance[id][0]; //user setting on closest monster
	int d1=aAutoSkillDistance[id][1]; //user setting of monster in turn
	info->umindis2=(int)(d0*d0*9)>>2;
	info->umaxdis2=(d1*d1*9)>>2;
	if (info->umindis2>info->vdis2) info->umindis2=info->vdis2;
	if (info->umaxdis2>info->vdis2) info->umaxdis2=info->vdis2;
	if (info->umaxdis2<info->umindis2) info->umaxdis2=info->umindis2;
	if (varMs) {
		switch (DIFFICULTY) {case 0:info->ms*=4;break;case 1:info->ms*=2;break;}
	}
}
void stackThrowing() {
	recheckSelfItems();if (!dwThrowWeaponId||!dwNextStackingId) return;
	stackToId=dwThrowWeaponId;stackFromId=dwNextStackingId;
	stackX=dwNextStackX;stackY=dwNextStackY;stackLoc=dwNextStackLoc;
	BYTE packet[5] = {0x19};
	*(DWORD*)&packet[1] = dwNextStackingId;
	SendPacket(packet,sizeof(packet));		
	dwAutoSkillReloading=STATE_PICKING;reloadingMs=dwCurMs+1000;
	dwAutoSkillCheckMs=dwCurMs+100;
}
static void processStacking() {
	if (dwCurMs>reloadingMs) {gameMessage("stacking timeout");dwAutoSkillReloading=0;return;}
	switch (dwAutoSkillReloading) {
		case STATE_PICKING: {
			if (!PLAYER->pInventory->pCursorItem) return;
			if (!stackToId) {gameMessage("no id");dwAutoSkillReloading=0;return;}
			BYTE packet[9] = {0x21};
			*(DWORD*)&packet[1]=stackFromId;*(DWORD*)&packet[5]=stackToId;
			SendPacket(packet,sizeof(packet));		
			dwAutoSkillReloading=STATE_STACKING;
			break;
		}
		case STATE_STACKING:dwAutoSkillReloading=STATE_PUTING;break;
		case STATE_PUTING: {
			BYTE packet[17] = {0x18};
			*(DWORD*)&packet[1]=stackFromId;*(DWORD*)&packet[5]=stackX;
			*(DWORD*)&packet[9]=stackY;*(DWORD*)&packet[13]=stackLoc;
			SendPacket(packet,sizeof(packet));		
			dwAutoSkillReloading=0;
			break;
		}
	}
}
static int canUse(struct AutoSkillInfo *info) {
	switch (info->id) {
		case Skill_Throw:
			if (d2common_GetUnitStat(PLAYER, 70, 0)<=dwAutoThrowMinQuantity) {
				if (dwAutoReloadThrowing&&dwThrowCount>0) stackThrowing();
				return 0; //quantity, keep some to repair
			}
			break;
		case Skill_LightningFury:
			if (d2common_GetUnitStat(PLAYER, 70, 0)<=dwAutoThrowMinQuantity) return 0;
			break;
		case Skill_Attack:case Skill_ExplodingArrow: //attack
			if (d2common_GetUnitStat(PLAYER, 70, 0)<=0) return 0; //quantity
			break;
	}
	return 1;
}
int AutoTeleport();
int AutoTeleportStart();
int AutoTeleportEnd();
int autoTelekinesis();
extern int fAutoTeleporting;
int AutoSkillNow() {
	if (!fAutoSkillNow&&dwRightSkill==Skill_Teleport) AutoTeleportStart();
	fAutoSkillNow=1;checkAutoSkillStatus();return 0;
}
extern int dwBackToTownTimeout;
void AutoSkillRunLoop() {
	if (fAutoSkillNow&&!IsKeyDown(tAutoForegroundSkill.key)&&!IsKeyDown(tAutoForegroundSkill2.key)) {
		fAutoSkillNow=0;checkAutoSkillStatus();
		if (dwRightSkill==Skill_Teleport) AutoTeleportEnd();
		if (autoIdentifying) autoIdentifying=0;
		return;
	}
	if (dwSkillChangeCountVerify!=dwSkillChangeCount) {
		if (leftSkill.valid) updateSkill(&leftSkill,dwLeftSkill);
		if (rightSkill.valid) updateSkill(&rightSkill,dwRightSkill);
		dwSkillChangeCountVerify=dwSkillChangeCount;
	}
	if (fAutoSkillNow) {
		if (*d2client_pUiStashOn) {autoSimpleItemStack();return;}
		if (*d2client_pUiCubeOn) {autoCubeTransform();return;}
		if (*d2client_pUiInventoryOn) {
			if (autoIdentify()) {autoIdentifying=1;return;}
		}
		if (dwRightSkill==Skill_Telekinesis) {
			if (autoTelekinesis()) return;
		}
		if (dwRightSkill==Skill_Teleport&&!autoIdentifying) {
			if (!AutoTeleport()) return;
		} else if (fAutoTeleporting) AutoTeleportEnd();
	}
	if (dwBackToTownTimeout) return;
	if (dwCurMs<dwAutoSkillCheckMs) return;
	if (dwAutoSkillReloading) {processStacking();dwAutoSkillCheckMs=dwCurMs+100;return;}
	if (fPlayerInTown||fAutoFollowMoving||tpMs) return;
	if (*d2client_pUiGameMenuOn) return;
	if (PLAYER->pInventory->pCursorItem) return;
	if (PLAYER->dwMode==PlayerMode_Attacking1||PLAYER->dwMode==PlayerMode_Attacking2||PLAYER->dwMode==PlayerMode_Cast) return;
	curSkill=NULL;nextSkill=NULL;
	if (dwUseLeftSkillFirst) {
		if (leftSkill.valid&&fCanUseLeftSkill&&canUse(&leftSkill)) curSkill=&leftSkill;
		if (rightSkill.valid&&fCanUseRightSkill&&canUse(&rightSkill)) {
			if (!curSkill) curSkill=&rightSkill;else nextSkill=&rightSkill;
		}
	} else {
		if (rightSkill.valid&&fCanUseRightSkill&&canUse(&rightSkill)) curSkill=&rightSkill;
		if (leftSkill.valid&&fCanUseLeftSkill&&canUse(&leftSkill)) {
			if (!curSkill) curSkill=&leftSkill;else nextSkill=&leftSkill;
		}
	}
	if (!curSkill) return;
	curSkill->maxdis2=fAutoSkillNow?curSkill->vdis2:curSkill->umaxdis2;
	//if (dwPlayerMana<curSkill->mana) {dwAutoSkillCheckMs=dwCurMs+dwAutoSkillCheckInterval;return;}
	minD2=0x7FFFFFFF;maxMs=0;pMinUnit=NULL;
	minD2_2=1000000;pMinUnit_2=NULL;
	fSelfRadiusSkill=0;stopAttack=0;
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_MONSTER*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (pUnit->dwUnitType!=UNITNO_MONSTER) break;
			checkUnit(pUnit);
			if (fSelfRadiusSkill) {pMinUnit=pUnit;goto loopend;}
			if (stopAttack) {dwAutoSkillCheckMs=dwCurMs+dwAutoSkillCheckInterval;return;}
		}
	}
loopend:
	if (!pMinUnit&&pMinUnit_2&&nextSkill) {
		curSkill=nextSkill;
		checkUnit(pMinUnit_2);
	}
	if (pMinUnit) {
		if (curSkill->isRight) {
			if (curSkill->selfRadius) RightSkillPos(PLAYER->pMonPath->wUnitX,PLAYER->pMonPath->wUnitY);
			else RightSkillUnit(pMinUnit);
		} else {
			if (curSkill->selfRadius) ShiftLeftClickPos(PLAYER->pMonPath->wUnitX+1,PLAYER->pMonPath->wUnitY+1);
			else if (curSkill->melee) LeftClickUnit(pMinUnit);
			else ShiftLeftClickUnit(pMinUnit);
		}
		switch (curSkill->id) {
			case Skill_Attack:
			case Skill_Throw:
			case Skill_ExplodingArrow:dwAutoSkillCheckMs=dwCurMs+40;break;
			default:dwAutoSkillCheckMs=dwCurMs+dwPlayerFcrMs-50;
		}
		monsterSkillMs[pMinUnit->dwUnitId&MONSTER_MASK]=dwCurMs;
		usingAutoSkill=1;
		multiclient_send_info(((MCI_AutoSkill|dwGameWindowId)<<24)|(pMinUnit->dwUnitId&0x7FFFFF)
			|(curSkill->isRight?0:0x800000));
	} else {
		if (usingAutoSkill) {
			usingAutoSkill=0;
			//multiclient_send_info((MCI_AutoSkill|dwGameWindowId)<<24);
		}
		dwAutoSkillCheckMs=dwCurMs+dwAutoSkillCheckInterval;
	}
}
int getUnitOwnerId(UnitAny *pUnit);
int getTowerValue(int isFire,int type);
static void checkUnit(UnitAny *pMon) {
	if (pMon->dwUnitType!=UNITNO_MONSTER) return;
	if (pMon->dwMode==MonsterMode_Death) return; //dying?
	if (pMon->dwMode==MonsterMode_Dead) return; //already dead
	int owner=getUnitOwnerId(pMon);
	if (owner!=-1) return; //玩家随从
	MonsterTxt *pMonTxt= pMon->pMonsterData->pMonsterTxt;
	MonsterData *pMonsterData = pMon->pMonsterData;
	int isActBoss=0,isBoss=0;
	int mtype=aAutoSkillMonsterType[pMon->dwTxtFileNo];
	if (mtype&MONSTER_TYPE_NEUTRAL) return;
	if (mtype&MONSTER_TYPE_ACTBOSS) {isActBoss=1;isBoss=1;}
	if (mtype&MONSTER_TYPE_NORMAL) {
	} else {
		if (pMonTxt->fBoss||pMon->pMonsterData->fBoss||pMon->pMonsterData->fUnique||pMon->pMonsterData->fChamp )
			isBoss=1;
		else
			if (!(pMonTxt->nflag1&1)) return; //npc
	}
	if (pMon->dwMode==MonsterMode_Sequence) {
		if (!isActBoss) return;
		if (258<=pMon->dwTxtFileNo&&pMon->dwTxtFileNo<=263) //Water Watcher,River Stalker
			return; //under water?
	}
	if (isBoss) {
		if (LEVELNO==39&&isCowKing(pMonsterData)) {//cow level 
			int d2=getUnitRawDistance2(PLAYER,pMon);
			switch (curSkill->id) {
				case Skill_StaticField://static field
					if (d2<curSkill->umindis2) stopAttack=1;return;
					break;
				default:stopAttack=1;return;
			}	
		}
		if (!curSkill->onBoss) return;
	} else {
		if (!curSkill->onNormal) return;
	}
	/*
 TXT14 flags=09,a0,01,12 詛咒怪
 TXT19 flags=3b,a0,41,02 沈淪魔
 TXT22 flags=3b,a0,41,02 黑暗魔
 TXT58 flags=19,a0,01,02 沈淪巫師
 TXT61 flags=19,a0,01,02 黑暗巫師
 TXT308 flags=03,a0,41,02 噴吐屍體怪
 TXT466 flags=03,80,20,02 狂亂冰封惡靈
 TXT156 flags=c8,a0,00,06 安達利爾
 TXT267 flags=40,a0,00,03 血鳥

 TXT211 flags=c2,a0,00,06 都瑞爾（我能掉30）
 TXT227 flags=02,80,01,02 蛆蟲
 TXT283 flags=03,80,01,02 幼蟲
 TXT149 flags=02,04,01,02 一股邪惡力量 dummy chicken
 TXT152 flags=02,85,01,02 NPC 一股邪惡力量 dummy RogueTown
 TXT404 flags=02,80,04,02 被困的靈魂
 TXT408 flags=02,07,01,03 NPC 哈德爾
	*/
	int ms=monsterSkillMs[pMon->dwUnitId&MONSTER_MASK];
	if (curSkill->delayState&&dwCurMs<ms+curSkill->delayState) return;
	if (curSkill->notState&&d2common_CheckUnitState(pMon, curSkill->notState)) {
		if (!curSkill->needKeep) return;
		int passed=dwCurMs-ms;
		if (isActBoss) {
			switch (curSkill->id) {
				case Skill_LifeTap:case Skill_Decrepify:passed<<=1;break;
			}
		}
		if (passed<curSkill->ms-dwPlayerFcrMs-dwAutoSkillCheckInterval-50) return;
	}
	if (curSkill->notImmune&&d2common_GetUnitStat(pMon, curSkill->notImmune, 0)>=100) return;
	int d2=getUnitRawDistance2(PLAYER,pMon);
	if (d2>curSkill->maxdis2) return;
	if (mtype&MONSTER_TYPE_DANGROUS) d2>>=2;
	else if (mtype&MONSTER_TYPE_REVIVE) d2>>=3;
	if (minD2<curSkill->umindis2) {
		if (d2>=minD2) return;
	} 
	if (dwRightSkill==Skill_Hydra&&dwCurrentLevel==Level_ArreatSummit) {
		if (pMon->dwTxtFileNo==371||pMon->dwTxtFileNo==372) {
			int value=getTowerValue(pMon->dwTxtFileNo==372,pMon->pMonsterData->bTypeFlags); //128,192,256,320,384,426,640,853
			value>>=4; //8-53
			if (pMonsterData->fBoss||pMonsterData->fChamp) {
				BYTE *enchants=pMonsterData->anEnchants;
				for (int i=0;i<9;i++,enchants++) {
					int enchno=*enchants ;if (!enchno) break;
					switch (enchno) {
						case 9: //FireEnchantedDesc
						case 18: //ColdEnchantedDesc
							value<<=1;
							break;
					}
				}
			}
			value=127-value;
			int fr=d2common_GetUnitStat(pMon,STAT_FIRE_RESIST,0);
			if (fr<0) fr=0;if (fr>100) fr=100;
			d2|=(fr<<23)|(value<<16);
		}
	}
	if (curSkill->id==Skill_DimVision&&!aAutoDimVisionMonster[pMon->dwTxtFileNo]) {
		d2|=0x10000;
	}
	POINT p1,p2;
	p1.x=PLAYER->pMonPath->wUnitX;p1.y=PLAYER->pMonPath->wUnitY;
	p2.x=pMon->pMonPath->wUnitX;p2.y=pMon->pMonPath->wUnitY;
	if (d2common_IsLineBlocked(PLAYER->pMonPath->pAreaRectData,&p1,&p2,4)) return;
	if (fAutoSkillNow&&pMon->dwTxtFileNo==434&&LEVELNO==111) { //rescue mission: prison door
		if (d2>=(dwRescueBarSafeDistance*dwRescueBarSafeDistance*9)>>2) {
			switch (curSkill->id) {
				case Skill_Blizzard:case Skill_Meteor:case Skill_FireWall:case Skill_Hydra:d2=0;break;
			}
		}
	}
	switch (curSkill->id) {
		case Skill_LifeTap:case Skill_Decrepify:
			if (isActBoss&&d2<33*33||d2<=curSkill->umindis2) {
				if (d2<minD2) {minD2=d2;pMinUnit=pMon;}
			}
			break;
		case Skill_StaticField: 
			if (d2<curSkill->umindis2) {
				int hp=d2common_GetUnitStat(pMon, STAT_HP, 0);
				int maxhp=d2common_GetUnitStat(pMon, STAT_MAXHP, 0);
				if (EXPANSION) {
					if (DIFFICULTY==1) {if (hp*3<=maxhp) return;}
					else if (DIFFICULTY==2) {if (hp*2<=maxhp) return;}
				}
				if (hp*100>=dwAutoStaticFieldHPPercent*maxhp) fSelfRadiusSkill=1;
				if (d2<minD2_2) {minD2_2=d2;pMinUnit_2=pMon;}
			}
			break;
		default: {
			int dd2=d2&0xFFFF;
			if (dd2<curSkill->umindis2||curSkill->id==Skill_DimVision) { 
				if (d2<minD2) {minD2=d2;pMinUnit=pMon;}
			} else if (dd2<curSkill->maxdis2&&minD2>=curSkill->umindis2) {
				int ms=monsterSkillMs[pMon->dwUnitId&MONSTER_MASK];
				if (!ms) ms=10000;else ms=dwCurMs-ms;
				if (ms>maxMs) {maxMs=ms;minD2=d2;pMinUnit=pMon;}
			}
			break;
		}
	}
}
void __fastcall autoSkillDimVision(char *packet) {
	if (!curSkill||curSkill->id!=Skill_DimVision) return;
	if (packet[1]!=1) return;
	int id=*(int *)(packet+2);
	monsterSkillMs[id&MONSTER_MASK]=dwCurMs;
}
int autoTelekinesis() {
	if (dwRightSkill!=Skill_Telekinesis) return 0;
	UnitAny *pUnit=NULL;int testBlock=0;
	switch (dwCurrentLevel) {
		case Level_TalRashaTomb1:case Level_TalRashaTomb2:case Level_TalRashaTomb3:
		case Level_TalRashaTomb4:case Level_TalRashaTomb5:case Level_TalRashaTomb6:case Level_TalRashaTomb7:{
			pUnit=findObjectByTxt(100,100,2);if (!pUnit) return 0;
			testBlock=0;
			break;
		}
		case Level_ChaosSanctuary: {
			//right_bottom:392, right_top:393, top:394,left_bottom:395,left_top:396, 
			pUnit=findObjectByTxt(392,396,0);
			if (!pUnit) return 0;
			break;
		}
	}
	if (!pUnit) return 0;
	if (testBlock) {
		POINT p1,p2;
		p1.x=PLAYER->pMonPath->wUnitX;p1.y=PLAYER->pMonPath->wUnitY;
		p2.x=pUnit->pMonPath->wUnitX;p2.y=pUnit->pMonPath->wUnitY;
		if (d2common_IsLineBlocked(PLAYER->pMonPath->pAreaRectData,&p1,&p2,4)) {
			//LOG("blocked %X\n",pUnit);
			return 0;
		}
	}
	//LOG("skill %X\n",pUnit);
	RightSkillUnit(pUnit);
	return 1;
}
