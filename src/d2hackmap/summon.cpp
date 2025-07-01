#include "stdafx.h"
#include "header.h"
#include "auto.h"
#include "multi.h"

extern ToggleVar tLogMonsterExp;
int dwTotalMonsterCount=0;
extern int fAutoEnchantCheckUnit;

#define MAX_MONSTERS 4096
int dwSkillChangeCount=0; //increase if skill level maybe changed
int dwNecMana,dwNecInfoExpireMs;
int fAutoSummonNeedCorpse;
int fAutoSummonSwitchSkill,fAutoSummonSwitchSkillTmp;
static int monsterSummonMs[MAX_MONSTERS];
static int minD2,minUnitId,revivePriority,lastSummonMs,seletonLvl,mageLvl;
static UnitAny *pMinUnit;
static int fcrMs,dwSkillChangeCountVerify;
int dwCheckSkeletonCountMs;
void AutoSummonNewGame() {
	dwTotalMonsterCount=0;
	memset(monsterSummonMs,0,MAX_MONSTERS*sizeof(int));
	lastSummonMs=dwCurMs;
	dwAutoSummonStartMs=dwCurMs+1000;
	dwSkeletonHPPercent=0;dwNecInfoExpireMs=0;
	minUnitId=0;seletonLvl=0;mageLvl=0;dwSkeletonMaxCount=0;dwSkeletonMageMaxCount=0;dwCheckSkeletonCountMs=0;
	dwSkeletonCount=0;dwSkeletonMageCount=0;
	dwReviveCount=0;dwReviveMaxCount=0;
	fcrMs=600;dwSkillChangeCountVerify=0;
	fAutoSummonNeedCorpse=0;fAutoSummonSwitchSkill=0;fAutoSummonSwitchSkillTmp=0;
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
		gameMessageW(dwGameLng?L"自动召唤级别%d,%d 数量%d,%d,%d":L"AutoSummon level=%d,%d count=%d,%d,%d",
			sLvl,mLvl,dwSkeletonMaxCount,dwSkeletonMageMaxCount,dwReviveMaxCount);
}
void checkSkeletonCount() {
	int hp=0,hpm=0,ns=0,nm=0,nr=0,enchant=0,checkEnchant=0;
	if (dwAutoSummonCheckEnchantMs) {
		if (dwAutoSummonCheckEnchantMs==1) dwAutoSummonCheckEnchantMs=dwCurMs+500;
		else if (dwCurMs>dwAutoSummonCheckEnchantMs) checkEnchant=1;
	}
	for (PetUnit *pPetUnit = *d2client_pPetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) {
		if (pPetUnit->dwOwerType!=UNITNO_PLAYER) continue;
		if (pPetUnit->dwOwnerId!=dwPlayerId) continue;
		switch (pPetUnit->dwPetType) {
			case 3:break;//Golem
			case 4:hp+=pPetUnit->dwHpPercent;ns++;
				if (checkEnchant) {
					UnitAny *pUnit=d2client_GetUnitFromId(pPetUnit->dwUnitId, UNITNO_MONSTER);
					if (pUnit&&d2common_CheckUnitState(pUnit, State_Enchant)) enchant++;
				}
				break;
			case 5:hpm+=pPetUnit->dwHpPercent;nm++;break;
			case 6:break;//revive
		}
	}
	if (checkEnchant) {dwEnchantSkeletonCount=enchant;dwAutoSummonCheckEnchantMs=0;}
	dwSkeletonCount=ns;dwSkeletonMageCount=nm;dwReviveCount=nr;
	dwSkeletonHPPercent=!dwSkeletonCount?0:hp/dwSkeletonCount;
	dwSkeletonMageHPPercent=!dwSkeletonMageCount?0:hpm/dwSkeletonMageCount;
	dwNecMana=dwPlayerMana;dwNecInfoExpireMs=dwCurMs+3300;
}
static void switchSummonSkill(int skill) {
	fAutoSummonSwitchSkillTmp=1;selectSkill(1,skill);
	dwAutoSummonStartMs=dwCurMs+dwSwitchSkillMs;
}
static int needSummon() {
	if (fAutoSummonSwitchSkill) {
		if (dwSkeletonCount<dwSkeletonMaxCount) {
			if (dwRightSkill!=70) {switchSummonSkill(70);return 0;}
		} else if (dwSkeletonMageCount<dwSkeletonMageMaxCount) {
			if (dwRightSkill!=80) {switchSummonSkill(80);return 0;}
		} else if (tAutoSummonRevive.isOn&&dwReviveCount<dwReviveMaxCount) {
			if (dwRightSkill!=95) {switchSummonSkill(95);return 0;}
		}
	}
	switch (dwRightSkill) {
		case 70:
			if (dwSkeletonCount>=dwSkeletonMaxCount) {
				if (dwSkeletonMageCount<dwSkeletonMageMaxCount) {switchSummonSkill(80);return 0;}
				if (tAutoSummonRevive.isOn&&dwReviveCount<dwReviveMaxCount) {switchSummonSkill(95);return 0;}
				dwAutoSummonStartMs=dwCurMs+1000;return 0;
			}
			return 1;
		case 80: 
			if (dwSkeletonMageCount>=dwSkeletonMageMaxCount) {
				if (dwSkeletonCount<dwSkeletonMaxCount) {switchSummonSkill(70);return 0;}
				if (tAutoSummonRevive.isOn&&dwReviveCount<dwReviveMaxCount) {switchSummonSkill(95);return 0;}
				dwAutoSummonStartMs=dwCurMs+1000;return 0;
			}
			return 1;
		case 95:
			if (dwReviveCount>=dwReviveMaxCount) {
				if (dwSkeletonCount<dwSkeletonMaxCount) {switchSummonSkill(70);return 0;}
				if (dwSkeletonMageCount<dwSkeletonMageMaxCount) {switchSummonSkill(80);return 0;}
				dwAutoSummonStartMs=dwCurMs+1000;return 0;
			}
			return 1;
	}
	return 0;
}

static void checkUnit(UnitAny *pMon) {
	if (pMon->dwUnitType!=UNITNO_MONSTER) return;
	if (pMon->dwMode && pMon->dwMode != 0x0C) return;
	if (!d2common_CheckCorpseCanUse(pMon,0)) return;
	int ms=monsterSummonMs[pMon->dwUnitId%MAX_MONSTERS];
	if (ms && dwCurMs< ms + 5000) return; //just sommoned
	if (d2common_CheckUnitState(pMon, 0x76)) return;//used corpse
	int priority=1;
	if (dwRightSkill==95) { //revive
		MonsterTxt *pMonTxt= pMon->pMonsterData->pMonsterTxt;
		if (pMonTxt->fBoss||pMon->pMonsterData->fBoss||pMon->pMonsterData->fUnique||pMon->pMonsterData->fChamp)
			return;
		if (d2common_IsUnitBlocked(PLAYER,pMon,4)) return;
		int txt=pMon->dwTxtFileNo;
		if (txt<0||txt>=1024) return;
		priority=aAutoReviveMonster[txt];
		if (priority<revivePriority) return;
	}
	int dx=PLAYER->pMonPath->wUnitX-pMon->pMonPath->wUnitX;
	int dy=PLAYER->pMonPath->wUnitY-pMon->pMonPath->wUnitY;
	int d2=(dx*dx+dy*dy)*4/9;
	if (d2<33*33&&d2<minD2) {minD2=d2;pMinUnit=pMon;minUnitId=pMon->dwUnitId;revivePriority=priority;}
}
void AutoSummonRunLoop() {
	//check skeleton count
	if (!dwAutoSummonStartMs||!dwCheckSkeletonCountMs||dwCurMs>=dwCheckSkeletonCountMs) {
		dwCheckSkeletonCountMs=dwCurMs+3000;
		if (!dwAutoSummonStartMs) dwAutoSummonStartMs=dwCurMs;
		checkSkeletonCount();
	}
	if (fPlayerInTown) return;
	//check skill level
	if (dwSkillChangeCountVerify!=dwSkillChangeCount||!seletonLvl&&!mageLvl) {
		checkSummonSkillLevel();
		dwSkillChangeCountVerify=dwSkillChangeCount;
	}
	if (!tAutoSummon.isOn
		||fAutoFollowMoving
		||!fCanUseRightSkill
		||(dwRightSkill!=70&&dwRightSkill!=80&&!(tAutoSummonRevive.isOn&&dwRightSkill==95)) 
		||dwCurMs<dwAutoSummonStartMs) {
		return;
	}
	/*check mana
	if (dwPlayerMana<dwAutoSummonMana
		||dwRightSkill==70&&dwPlayerMana<seletonLvl+5
		||dwRightSkill==80&&dwPlayerMana<mageLvl+7
		||dwRightSkill==95&&dwPlayerMana<45) { //Not enough mana
		dwAutoSummonStartMs=dwCurMs+500;//Check mana again after 500ms
		return;
	}*/
	if (PLAYER->dwMode==PlayerMode_Attacking1||PLAYER->dwMode==PlayerMode_Attacking2||PLAYER->dwMode==PlayerMode_Cast) return;
	fAutoSummonNeedCorpse=0;
	if (!needSummon()) return;
	checkSkeletonCount();
	if (!needSummon()) return;
	fAutoSummonNeedCorpse=1;
	minD2=10000;minUnitId=0;revivePriority=1;pMinUnit=NULL;
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_MONSTER*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (pUnit->dwUnitType!=UNITNO_MONSTER) break;
			checkUnit(pUnit);
		}
	}
	if (pMinUnit) {
		UnitAny *pMon=pMinUnit;
		dwAutoSummonStartMs=dwCurMs+fcrMs*40-50;
		monsterSummonMs[pMon->dwUnitId%MAX_MONSTERS]=dwCurMs;
		RightSkillUnit(pMon);
		lastSummonMs=dwCurMs;
	} else {
		if (!fWinActive) dwAutoSummonStartMs=dwCurMs+500; //run in background no corporse
		else dwAutoSummonStartMs=dwCurMs+200;
	}
}
void __fastcall addUnit(UnitAny *pUnit) {
	switch (pUnit->dwUnitType) {
		case 0:fAutoEnchantCheckUnit=1;break;
		case 1:
			if (pUnit->dwMode==0||pUnit->dwMode==12) {
				if (fAutoSummonNeedCorpse) dwAutoSummonStartMs=dwCurMs;
			} else {
				dwAutoSkillCheckMs=dwCurMs;
				dwTotalMonsterCount++;
			}
			break;
	}
}
/*
The following code add pUnit to hashtable
6FB56505 - 8B 86 C4000000        - mov eax,[esi+000000C4] <-----esi=pUnit
6FB5662B - 89 34 90              - mov [eax+edx*4],esi <--------- add unit
when any player appears, open the screen so it can be scanned.
can modify, eax,ecx,edx,
can't modify ebx,edi,esi
*/
void __declspec(naked) AddUnitPatch_ASM() {
	__asm {
		push ebx
		push edi
		push esi
		mov ecx, esi
		call addUnit
		pop esi
		pop edi
		pop ebx
		mov eax,[esi+0xC4]
		ret
	}
}
/*
ecx=pUnit
6FB56F25 - 8B 81 E4000000        - mov eax,[ecx+000000E4] <---
6FB56F2B - 89 04 96              - mov [esi+edx*4],eax 
*/
void __declspec(naked) RemoveUnitPatch1_ASM() {
	__asm {
		mov eax,dwCurMs;
		add eax, 100
		mov dwCheckSkeletonCountMs,eax
		mov eax,[ecx+0xE4]
		ret
	}
}
/*
ecx=pUnit
6FB56F30 - 8B 91 E4000000        - mov edx,[ecx+000000E4] <---
6FB56F36 - 89 90 E4000000        - mov [eax+000000E4],edx
*/
void __declspec(naked) RemoveUnitPatch2_ASM() {
	__asm {
		mov edx,dwCurMs;
		add edx, 100
		mov dwCheckSkeletonCountMs,edx
		mov edx,[ecx+0xE4]
		ret
	}
}
static void __fastcall dumpMonsterInfo(int id) {
	UnitAny *pUnit=d2client_GetUnitFromId(id,UNITNO_MONSTER);if (!pUnit) return;
	if (pUnit->dwMode==12) return;
	if (!logfp) return;
	if (351<=pUnit->dwTxtFileNo&&pUnit->dwTxtFileNo<=353) return; //hydra
	FILE *fp=logfp;
	char name[64],def='?';BOOL used=FALSE;
	MonsterTxt *pMonsterTxt = pUnit->pMonsterData->pMonsterTxt;
	MonsterData *pMonsterData=pUnit->pMonsterData; 
	WideCharToMultiByte(CP_ACP,0,pUnit->pMonsterData->wszMonName,-1,name,64,&def,&used);
	fprintf(fp,"MonsterDeath: txt%d",pUnit->dwTxtFileNo);
	fprintf(fp," id=%d",id);
	fprintf(fp," mode=%d",pUnit->dwMode);
	fprintf(fp," lv=%d",d2common_GetUnitStat(pUnit, STAT_LEVEL, 0));
	fprintf(fp," areaLv=%d",GetAreaLevel());
	fprintf(fp," cLv=%d",dwPlayerLevel);
	fprintf(fp," type=%02x",pMonsterData->bTypeFlags);
	if (pMonsterData->wUniqueNo)
		fprintf(fp," unique=%d",pMonsterData->wUniqueNo);
	fprintf(fp," flags=%02x,%02x,%02x,%02x",
		pMonsterTxt->nflag1,pMonsterTxt->nflag2,pMonsterTxt->flag3,pMonsterTxt->nflag4);
	if (pMonsterTxt->fNpc) fprintf(fp," NPC");
	fprintf(fp," %s",name);
	if (pMonsterData->fUnique) fprintf(fp," unique");
	if (pMonsterData->fChamp) fprintf(fp," champ");
	if (pMonsterData->fBoss) fprintf(fp," boss");
	if (pMonsterData->fMinion) fprintf(fp," minion");
	fprintf(fp," comp:{");
	for (int i=0;i<16;i++) {
		if (pMonsterData->nComponents[i]) fprintf(fp," %d:%d",i,pMonsterData->nComponents[i]);
	}
	fprintf(fp,"}");
	fprintf(fp," enchants:{");
	for (int i=0;i<9;i++) {
		if (pMonsterData->anEnchants[i]) fprintf(fp," %d:%d",i,pMonsterData->anEnchants[i]);
	}
	fprintf(fp,"}");
	fputc('\n',fp);
	fflush(fp);
}
static void __fastcall recvCommand19to1F(char *packet) {
	int cmd=packet[0];
	unsigned int exp;
	switch (cmd) {
		case 0x1a:exp=packet[1]&0xFF;break;
		case 0x1b:exp=*(unsigned short *)(packet+1);break;
		case 0x1c:exp=*(unsigned int *)(packet+1)-(unsigned int)d2common_GetUnitStat(PLAYER,STAT_EXP,0);;break;
		default:return;
	}
	LOG("Add exp: %d\n",exp);
}
/*
monster dead: state->8->9
d2client_AF390: 83 EC 1C           sub esp, 0x1C (28)
d2client_AF393: 8B C2                 - mov eax,edx <--- edx,esi:packet
d2client_AF395: 8B D1                 - mov edx,ecx <--- install here
d2client_AF397: 0F B7 48 06           - movzx ecx,word ptr [eax+06]
...
d2client_AF3BD: 0FB6 48 05            - movzx ecx,byte ptr [eax+05] <-state
*/
void __declspec(naked) RecvCommand_69_Patch_ASM() {
	__asm {
		mov edx,ecx
		movzx ecx, byte ptr [eax+5]
		cmp ecx, 9
		jne original
		cmp tLogMonsterExp.isOn, 0
		je checkSummon
		pushad
		mov ecx, dword ptr [eax+1]
		call dumpMonsterInfo
		popad
checkSummon:
		cmp fAutoSummonNeedCorpse,0
		je original
		mov dwAutoSummonStartMs,0
original:	
		movzx ecx, word ptr [eax+6]
		ret
	}
}
/*
6FB5C720 - 8A 41 01              - mov al,[ecx+01] <-0x00 = Unsummoned/Lost Sight || 0x01 = Summoned/Assign
6FB5C723 - 84 C0                 - test al,al
6FB5C725 - 8B 51 09              - mov edx,[ecx+09]
*/
extern int dwPetListChangeCount;
void __declspec(naked) RecvCommand_7A_Patch_ASM() {
	__asm {
		mov edx,dwCurMs;
		add edx, 100
		mov dwCheckSkeletonCountMs,edx
		mov dwAutoSummonCheckEnchantMs,1
		inc dwPetListChangeCount
//original
		mov al, byte ptr [ecx+1]
		test al,al
		ret
	}
}
/*
Update Item Oskill  21 [WORD Unknown] [DWORD Unit Id] [WORD Skill] [BYTE Base Level] [BYTE Bonus Amount] [BYTE Unknown] 
	00: 21 00 00 01 - 00 00 00 06 - 00 01 00 00 -             |!               
6FB5C911 - 8B F1                 - mov esi,ecx
6FB5C913 - 0FB6 56 01            - movzx edx,byte ptr [esi+01]
*/
void __declspec(naked) RecvCommand_21_Patch_ASM() {
	__asm {
		mov edx,dwSkillChangeCount
		add edx,1
		mov dwSkillChangeCount,edx
//original
		mov esi,ecx
		movzx edx,byte ptr [esi+1]
		ret
	}
}
//d2client_AEDF2: 8B 2D FC BB BC 6F  mov ebp, [d2client_11BBFC UnitAny *d2client_PlayerUnit]
void __declspec(naked) RecvCommand_19_to_1F_Patch_ASM() {
	__asm {
		cmp tLogMonsterExp.isOn, 0
		je original
		pushad
		call recvCommand19to1F
		popad
original:
		mov ebp, d2client_pPlayerUnit
		mov ebp, [ebp]
		ret
	}
}
