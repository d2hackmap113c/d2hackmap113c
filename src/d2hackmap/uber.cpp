#include "stdafx.h"
#include "header.h"

//copied from PlugY
static int UberMephistoPos[2] = {25130,5143};
static int UberDiabloPos[2] = {25139,5139};
static int UberBaalPos[2] = {25139,5135};
static bool active_UberMinions = true;
int UberMephistoNbMinions = 6;
int UberMephistoMinions[] = {
	mon_uber_mephisto_squeleton, mon_uber_mephisto_archer, mon_uber_mephisto_fire, mon_uber_mephisto_lightning, mon_uber_mephisto_cold, mon_uber_mephisto_poison};
int UberMephistoSpawnPercent = 80;
int UberMephistoSpawnRadius = 30;
int UberBaalNbMinions = 2;
int UberBaalMinions[] = {mon_uber_baal_dark_lord, mon_uber_baal_specter};
int UberBaalSpawnPercent = 30;
int UberBaalSpawnRadius = 30;
int UberDiabloNbMinions = 1;
int UberDiabloMinions[] = {mon_uber_diablo_pit_lord};
int UberDiabloSpawnPercent = 30;
int UberDiabloSpawnRadius = 30;

bool active_UberDiabloRushTweekAI = false;
bool active_UberBaalTeleportTweekAI = false;
bool active_UberBaalChillingArmorTweekAI = false;
int uberBaalChillingArmorTimer = 6000;
int uberBaalChillingArmorLastFrame = 0;
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_INT_ARRAY1, "UberMephistoPos",&UberMephistoPos,2,{0}},
  {CONFIG_VAR_TYPE_INT_ARRAY1, "UberDiabloPos",&UberDiabloPos,2,{0}},
  {CONFIG_VAR_TYPE_INT_ARRAY1, "UberBaalPos",&UberBaalPos,2,{0}},
  {CONFIG_VAR_TYPE_INT, "ActiveUberMinions",&active_UberMinions      , 4 },
};
void uber_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
static struct {
	union{
		DWORD all;
		struct{
			DWORD uber1Spawned:1;
			DWORD uber2Spawned:1;
			DWORD uber3Spawned:1;
			DWORD uber4Spawned:1;
			DWORD uber5Spawned:1;
			DWORD uber6Spawned:1;
		};
	};
	int count1;
	int count2;
	int count3;
} questState;

void resetUberQuest() {
	ZeroMemory(&questState,sizeof(questState));
	questState.count1 = 10 + (rand()%10);
	questState.count2 = 20 + (rand()%40);
	questState.count3 = 10 + (rand()%20);
	uberBaalChillingArmorLastFrame = 0;
	//LOG("Uber Quest State : %d %d %d\n",questState.count1, questState.count2, questState.count3);
}
int __fastcall openPandPortal(World* world, UnitAny* pUnit) {
	if (world->difficulty!=2) return 0;
	AreaRectData* pData = d2common_getRectData(pUnit);
	DrlgLevel* pLvl = pData->pAreaRectInfo->pDrlgLevel;
	int area=0;
	if (!hasPortalInLevel(pLvl,Level_MatronDen)) area=Level_MatronDen;
	else if (!hasPortalInLevel(pLvl,Level_ForgottenSands)) area=Level_ForgottenSands;
	else if (!hasPortalInLevel(pLvl,Level_FurnaceofPain)) area=Level_FurnaceofPain;
	if (!area) return 0;
	return portalToLevel(world,pUnit, area);
}
int __fastcall openPandFinalPortal(World* world, UnitAny* pUnit) {
	if (world->difficulty!=2) return 0;
	return portalToLevel(world,pUnit, Level_Tristram6boss);
}
int spawnSuperUnique(World* world,AreaRectData *pData,int zero1,int x,int y,int minusOne,int txt,int zero2) {
	__asm {
		push minusOne
		push y
		push x
		push pData
		push world
		mov eax, zero1
		mov edx, zero2
		mov ecx, txt
		call d2game_SpawnSuperUnique
	}
}
UnitAny *spawnMonster(int id,int mode,World* world,AreaRectData *pData,int x,int y,int one,int zero) {
	return NULL;
}
void monsterUseSkill(World* world,UnitAny *pMon,int arg,int skill,UnitAny *target,int x,int y) {
	__asm {
		push ebx
		push pMon
		mov ebx,world
		mov ecx,pMon
		mov eax,arg
		push y
		push x
		push target
		push skill
		call d2game_MonsterUseSkill
		// game sets monsters aicontrol args[0] to 0 after this call
		pop edx
		mov edx,dword ptr ds:[edx+0x14]
		mov edx,dword ptr ds:[edx+0x28]
		mov dword ptr ds:[edx+0x14],0
		pop ebx
	}
	//*(int *)((char *)pMon->pMonsterData->pAiGeneral+0x14)=0;
}

void __stdcall spawnUber(World* world, AreaRectData* pData) {
	//LOG("Uber Quest State : %d %d %d\n",questState.count1, questState.count2, questState.count3);
	//LOG("Uber Quest questState spawn : %d %d %d %d %d %d\n",questState.uber1Spawned, questState.uber2Spawned, questState.uber3Spawned, questState.uber4Spawned, questState.uber5Spawned, questState.uber6Spawned);
	int areaId=d2common_GetLevelIdFromRectData(pData);
	switch (areaId) {
	case Level_MatronDen:
		if (!questState.uber1Spawned) {
			if (questState.count1) questState.count1--;
			else {
				if (spawnSuperUnique(world, pData, 0, 0, 0, -1, Mon_UberAndariel, 0))
					questState.uber1Spawned = 1;
			}
		}
		break;
	case Level_ForgottenSands:
		if (!questState.uber2Spawned) {
			if (questState.count2) questState.count2--;
			else {
				if (spawnSuperUnique(world, pData, 0, 0, 0, -1, Mon_UberDuriel, 0))
					questState.uber2Spawned = 1;
			}
		}
		break;
	case Level_FurnaceofPain:
		if (!questState.uber3Spawned) {
			if (questState.count3) questState.count3--;
			else {
				if (spawnSuperUnique(world, pData, 0, 0, 0, -1, Mon_UberIzual, 0))
					questState.uber3Spawned = 1;
			}
		}
		break;
	case Level_Tristram6boss:
		if (isInRect(pData, UberMephistoPos[0], UberMephistoPos[1])) {
			spawnSuperUnique(world, pData, 0, UberMephistoPos[0], UberMephistoPos[1], -1, Mon_UberMephisto, 0);
		}
		if (isInRect(pData, UberDiabloPos[0], UberDiabloPos[1])) {
			spawnSuperUnique(world, pData, 0, UberDiabloPos[0], UberDiabloPos[1], -1, Mon_UberDiablo, 0);
		}
		if (isInRect(pData, UberBaalPos[0], UberBaalPos[1])) {
			spawnSuperUnique(world, pData, 0, UberBaalPos[0], UberBaalPos[1], -1, Mon_UberBaal, 0);
		}
	}
	d2game_Game235C0(world, pData);
}
int GetNbMonsters(DrlgLevel* level) {
	int nbMonsters = 0;
	for (AreaRectInfo* pInfo = level->pAreaRectInfo; pInfo; pInfo = pInfo->pNext) {
		if (!pInfo->pAreaRectData) continue;
		for (UnitAny* unit = pInfo->pAreaRectData->pUnit; unit; unit = unit->pRectNext) {
			if (unit->dwUnitType == UNITNO_MONSTER && unit->dwMode != MonsterMode_Dead && unit->dwTxtFileNo != 711) {
				nbMonsters++;
			}
		}
	}
	return nbMonsters;
}
int GetNbMonstersRadius(DrlgLevel* level, POINT position, int radius) {
	int nbMonsters = 0;
	for (AreaRectInfo* pInfo = level->pAreaRectInfo; pInfo; pInfo = pInfo->pNext) {
		AreaRectData *pData=pInfo->pAreaRectData;if (!pData) continue;
		for (UnitAny* unit = pData->pUnit; unit; unit = unit->pRectNext) {
			if (unit->dwUnitType == UNITNO_MONSTER && unit->dwMode != MonsterMode_Dead && unit->dwTxtFileNo != 711) {
				POINT p;
				d2common_getUnitPos(unit, &p);
				int dis=getDistanceM256(position.x-p.x,position.y-p.y)>>8;
				if (dis <= radius) nbMonsters++;
			}
		}
	}
	return nbMonsters;
}
int CastSummonMonster(World* ptGame, UnitAny* ptMonster, int x, int y,
	int* minions, int minionsSize, int mode) {
	int minionId = minions[rand()%minionsSize];
	//LOG("CastSummonMonster: id:%d  x:%d  y:%d  mode:%d\n", minionId, x, y, mode);
	AreaRectData* pData = d2common_getRectData(ptMonster);
	DrlgLevel* ptLevel = pData->pAreaRectInfo->pDrlgLevel;
	int monstersInLevel = GetNbMonsters(ptLevel);
	POINT position;
	d2common_getUnitPos(ptMonster, &position);
	int monstersNearby = GetNbMonstersRadius(ptLevel, position, 25);
	//LOG("CastSummonMonster nbMonsters: %d - %d\n", monstersNearby, monstersInLevel);
	if (monstersNearby > 16 || monstersInLevel > 100) return 0;
	// Spawn Monster
	//LOG("CastSummonMonster spawnMonster(%d, %d, %08X, %08X, %d, %d, %d, %d)\n", minionId, mode, ptGame, pData, x, y, 1, 0);
	UnitAny* monster = d2game_SpawnMonster(minionId, mode, ptGame, pData, x, y, 1, 0);
	//LOG("CastSummonMonster spawnMonster end\n");
	if (monster) return true;
	// Try in near rooms
	//LOG("CastSummonMonster Try in near rooms: %d - %d\n", pData->unitX, pData->unitY);
	for(int i = 0; i < pData->nearbyRectCount; i++) {
		AreaRectData* pData2 = pData->paDataNear[i];
		if (isInRect(pData2, x, y)) {
			//LOG("CastSummonMonster spawnMonster(%d, %d, %08X, %08X, %d, %d, %d, %d)\n", minionId, mode, ptGame, pData2, x, y, 1, 0);
			monster = d2game_SpawnMonster(minionId, mode, ptGame, pData2, x, y, 1, 0);
			//LOG("CastSummonMonster spawnMonster end\n");
			if (monster) return true;
		}
	}
	//LOG("CastSummonMonster end: %d - %d\n", pData->unitX, pData->unitY);
	return false;
}
double randf() {
	return (rand()&0xFFFF)/65536.0;
}
void RandInCircle(POINT* position, int radius) {
	double theta = randf() * 6.283185;
	double r = sqrt(randf()) * radius;
	position->x += (int)(r * cos(theta));
	position->y += (int)(r * sin(theta));
}
void __fastcall uberMephAI(World* ptGame, UnitAny* ptMonster, AIParam* ptAIArgs) {
	//LOG("UberMephAI called.\n");
	// Spawn Minions
	if (active_UberMinions && (rand()%100) < UberMephistoSpawnPercent 
		&& ptAIArgs->distanceToTarget < UberMephistoSpawnRadius) {
		POINT pos;
		d2common_getUnitPos(ptAIArgs->target, &pos);
		CastSummonMonster(ptGame, ptMonster, pos.x, pos.y, UberMephistoMinions, UberMephistoNbMinions, 8);
	}
	//LOG("Uber Mephisto AI activated.\n");
	d2game_MephAI(ptGame, ptMonster, ptAIArgs);
}
void __fastcall uberDiabloAI(World* ptGame, UnitAny* ptMonster, AIParam* ptAIArgs) {
	//LOG("UberDiabloAI called.\n");
	// Spawn Minions
	if (active_UberMinions && (rand()%100) < UberDiabloSpawnPercent && ptAIArgs->distanceToTarget < UberDiabloSpawnRadius) {
		POINT pos;
		d2common_getUnitPos(ptAIArgs->target, &pos);
		RandInCircle(&pos, 3);
		CastSummonMonster(ptGame, ptMonster, pos.x, pos.y, UberDiabloMinions, UberDiabloNbMinions, 1);
	}
	// Tweek Diablo AI Rush
	if (active_UberDiabloRushTweekAI && (rand()%100) < (int)ptAIArgs->distanceToTarget - 10) {
		if ((rand()%3) > 0)//67% to Rush
		{
			//LOG("Uber Diablo Run activated.\n");
			int diabloRunSkillId = 4;
			monsterUseSkill(ptGame, ptMonster, ptAIArgs->ptMonStatsBIN->skillArg[diabloRunSkillId], ptAIArgs->ptMonStatsBIN->skill[diabloRunSkillId], ptAIArgs->target, 0, 0);
		} else {
			//LOG("Uber Diablo Move activated.\n");
			POINT pos;
			d2common_getUnitPos(ptAIArgs->target, &pos);
			d2game_MonsterMove(ptGame, ptMonster, NULL, 2, pos.x, pos.y, 1, 0);
		}
		return;
	}
	//LOG("Uber Diablo AI activated.\n");
	d2game_DiabloAI(ptGame, ptMonster, ptAIArgs);
}
void __fastcall uberBaalAI(World* ptGame, UnitAny* ptMonster, AIParam* ptAIArgs) {
	//LOG("UberBaalAI called.\n");
	if (active_UberMinions && (rand()%100) < UberBaalSpawnPercent && ptAIArgs->distanceToTarget < UberBaalSpawnRadius) {
		POINT pos;
		d2common_getUnitPos(ptAIArgs->target, &pos);
		RandInCircle(&pos, 3);
		CastSummonMonster(ptGame, ptMonster, pos.x, pos.y, UberBaalMinions, UberBaalNbMinions, 1);
	}
	// Tweek Baal AI Chilling Armor
	if (active_UberBaalChillingArmorTweekAI) {
		if (uberBaalChillingArmorLastFrame == 0 || ((int)ptGame->frame - uberBaalChillingArmorLastFrame > uberBaalChillingArmorTimer)) {
			//LOG("Uber Baal Chilling Armor activated.\n");
			uberBaalChillingArmorLastFrame = ptGame->frame;
			const int baalChillingArmorSkillId = 7;
			monsterUseSkill(ptGame, ptMonster, ptAIArgs->ptMonStatsBIN->skillArg[baalChillingArmorSkillId], ptAIArgs->ptMonStatsBIN->skill[baalChillingArmorSkillId], NULL, 0, 0);
			return;
		}
	}
	// Tweek Baal AI Teleport
	if (active_UberBaalTeleportTweekAI && (rand()%100) < (int)ptAIArgs->distanceToTarget - 10) {
		if ((rand()%3) > 0)//67% to teleport
		{
			//LOG("Uber Baal Teleport activated.\n");
			const int baalTeleportSkillId = 4;
			POINT pos;
			d2common_getUnitPos(ptAIArgs->target, &pos);
			monsterUseSkill(ptGame, ptMonster, ptAIArgs->ptMonStatsBIN->skillArg[baalTeleportSkillId], ptAIArgs->ptMonStatsBIN->skill[baalTeleportSkillId], NULL/*ptAIArgs->target*/, pos.x, pos.y);
		} else {
			//LOG("Uber Baal Move activated.\n");
			POINT pos;
			d2common_getUnitPos(ptAIArgs->target, &pos);
			d2game_MonsterMove(ptGame, ptMonster, NULL, 2, pos.x, pos.y, 1, 0);
		}
		return;
	}
	//LOG("Uber Baal AI activated.\n");
	d2game_BaalAI(ptGame, ptMonster, ptAIArgs);
}

int installUberQuestPatches() {
	//uber quest
patchValue("openPandPortal",PATCH_ADDR(d2game,0xFA5F0),(DWORD)openPandPortal,4,"$(+70180)");
patchValue("openPandFinalPortal",PATCH_ADDR(d2game,0xFA5F4),(DWORD)openPandFinalPortal,4,"$(+70170)");
patchValue("uberBaalAI",PATCH_ADDR(d2game,0x10F0A0),(DWORD)uberBaalAI,4,"$(+B6C80)");
patchValue("uberMephAI",PATCH_ADDR(d2game,0x10F0B0),(DWORD)uberMephAI,4,"$(+7B4E0)");
patchValue("uberDiabloAI",PATCH_ADDR(d2game,0x10F0C0),(DWORD)uberDiabloAI,4,"$(+A39D0)");
//d2game_2CCAA: E8 31 34 FF FF     call d2game_200E0
PatchCall(d2game,0x2CCAA,spawnUber,5,"E8 31 34 FF FF"); //Conflict with PlugY
	return 1;
}	
