#include "stdafx.h"
#include "header.h"

void usePotion(int mana);
extern int dwCheckRelationMs,hasHostilePlayer;
extern int fState100HP,fState106ManaMs;
extern int dwHPotionCount,dwMPotionCount;
extern int dwBackToTownTimeout;
int BackToTown();
int dwCheckMercMs;
static int manaMs=0,healingMs=0,golemMs=0,hireMs=0;
static int *pMercHpPercent=NULL;
static int *pGolemHpPercent=NULL;
static int fIronGolemIsRuneword,fIronGolemTownProtection;
static int ceHp,feHp,cowKingHp;
static int ceDisRaw256,feDisRaw256,cowKingDis;
static int lastChickenLifeThreshold=-1;

int dwAutoPotionCheckMs=						1000;
int dwIronGolemLifeAlertPercent=						0;
int dwIronGolemLifeTownPercent=						0;
int dwIronGolemLifeExitPercent=						0;
int dwIronGolemTownLifePercent=			0;
int dwHirePotionMinCLevel=						0;
int dwHirePotionLifePercent=						0;
int dwHirePotionColumn=						0;
int dwHirePotionDelayMs=						3000;
int dwHealingPotionLifePercent=						0;
int dwHealingPotionDelayMs=						0;
int dwManaPotionMax=						0;
int dwManaPotionValue=						0;
int dwManaPotionNTValue=						0;
int dwManaPotionDelayMs=						3000;
int dwChickenLife=						0;
int dwChickenLifeCondition=						0;
int dwChickenHostileLife=				0;
int dwChickenHostileNearbyLife=			0;
int dwChickenLifePercent=0;
int dwHCChickenLifePercent=0;
int dwChickenHostileLifePercent=		0;
int dwChickenHostileNearbyLifePercent=	0;
int nChickenLifeAction=1;
ToggleVar tHealingPotion={TOGGLEVAR_ONOFF,	0,	-1,	1,	"Auto Healing Potion"};
ToggleVar tManaPotion={TOGGLEVAR_ONOFF,	0,	-1,	1,	"Auto Mana Potion"};
ToggleVar tChickenLife={TOGGLEVAR_ONOFF,	0,	-1,	1,	"Chicken life"};
ToggleVar tChickenHostile={TOGGLEVAR_ONOFF,	0,	-1,	1,	"Chicken hostile"};
ToggleVar tChickenHostileNearby={TOGGLEVAR_ONOFF,	0,	-1,	1,	"Chicken hostile nearby"};
ToggleVar tChickenDangerousMonster={		TOGGLEVAR_ONOFF,	0,	-1,	1, "Chicken Dangerous Monster"};
ToggleVar tAutoPotion={TOGGLEVAR_ONOFF,	0,	-1,	1,	"Auto Potion"};
ToggleVar tHardCoreAllBackTownIfProtect={TOGGLEVAR_ONOFF,0,-1,1,"HardCoreAllBackTownIfProtect"};
ToggleVar tHardCoreLeaveTownDurability={TOGGLEVAR_ONOFF,0,-1,1,"HardCoreLeaveTownDurability"};
int dwHardCoreLeaveTownDurabilityPercent=10;
int dwChickenLifeMinClevel=0;
int dwChickenLifeForcedClevel=90;
int dwNoQuitIfDeadClevel=90;
int dwChickenLifeEnterGame=1;
BOOL fLifeProtectOn=FALSE;
BOOL fDangerousMonsterActive=FALSE;
char anDangerousMonster[1000][2]={0};
int nDangerousMonsterAction=2;
int dwHardCoreFEHpProtect=0;
static ToggleVar tHardCoreResistProtect={TOGGLEVAR_ONOFF,0,-1,1,"Hard Core Resist Protect"};
static BYTE hardCoreLeaveTownResist[3][5]={0};
static ConfigVar aConfigVars[]={
//--- m_CheckDangerous.h ---
  {CONFIG_VAR_TYPE_KEY, "HardCoreLeaveTownDurabilityToggle",&tHardCoreLeaveTownDurability,4},
  {CONFIG_VAR_TYPE_INT, "HardCoreLeaveTownDurabilityPercent",&dwHardCoreLeaveTownDurabilityPercent,4},
  {CONFIG_VAR_TYPE_INT, "HardCoreFEHpProtect",&dwHardCoreFEHpProtect,4},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "HardCoreLeaveTownResist",&hardCoreLeaveTownResist,5,{3}},
  {CONFIG_VAR_TYPE_KEY, "HardCoreAllBackTownIfProtect",&tHardCoreAllBackTownIfProtect,4},
  {CONFIG_VAR_TYPE_KEY, "HardCoreResistProtectToggle",&tHardCoreResistProtect,4},
  {CONFIG_VAR_TYPE_INT, "IronGolemLifeAlertPercent",&dwIronGolemLifeAlertPercent,     4},
  {CONFIG_VAR_TYPE_INT, "IronGolemLifeTownPercent",&dwIronGolemLifeTownPercent,     4},
  {CONFIG_VAR_TYPE_INT, "IronGolemLifeExitPercent",&dwIronGolemLifeExitPercent,     4},
  {CONFIG_VAR_TYPE_INT, "HirePotionMinCLevel",	&dwHirePotionMinCLevel,     4},
  {CONFIG_VAR_TYPE_INT, "HirePotionLifePercent",	&dwHirePotionLifePercent,     4},
  {CONFIG_VAR_TYPE_INT, "HirePotionColumn",				&dwHirePotionColumn,     4},
  {CONFIG_VAR_TYPE_INT, "HirePotionDelayMs",			&dwHirePotionDelayMs,     4},
  {CONFIG_VAR_TYPE_INT, "HealingPotionLifePercent",				&dwHealingPotionLifePercent,     4},
  {CONFIG_VAR_TYPE_INT, "HealingPotionDelayMs",				&dwHealingPotionDelayMs,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionMax",				&dwManaPotionMax,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionValue",				&dwManaPotionValue,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionNTValue",				&dwManaPotionNTValue,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionDelayMs",			&dwManaPotionDelayMs,     4},
  {CONFIG_VAR_TYPE_KEY, "HealingPotionToggle",				&tHealingPotion,     4},
  {CONFIG_VAR_TYPE_KEY, "ManaPotionToggle",				&tManaPotion,     4},
  {CONFIG_VAR_TYPE_KEY, "ChickenLifeToggle",          &tChickenLife     },
  {CONFIG_VAR_TYPE_KEY, "ChickenHostileToggle",		&tChickenHostile    },
  {CONFIG_VAR_TYPE_KEY, "ChickenHostileNearbyToggle", &tChickenHostileNearby  },
  {CONFIG_VAR_TYPE_INT, "ChickenLifeCondition",				&dwChickenLifeCondition,     4},
  {CONFIG_VAR_TYPE_INT, "ChickenLife",				&dwChickenLife,     4},
  {CONFIG_VAR_TYPE_INT, "ChickenHostileLife",			&dwChickenHostileLife,  4},
  {CONFIG_VAR_TYPE_INT, "ChickenHostileNearbyLife",   &dwChickenHostileNearbyLife,4},
  {CONFIG_VAR_TYPE_INT, "ChickenLifePercent",			&dwChickenLifePercent,  4},
  {CONFIG_VAR_TYPE_INT, "HCChickenLifePercent",&dwHCChickenLifePercent,4},
  {CONFIG_VAR_TYPE_INT, "ChickenHostileLifePercent",  &dwChickenHostileLifePercent,4},
  {CONFIG_VAR_TYPE_INT, "ChickenHostileNearbyLifePercent", &dwChickenHostileNearbyLifePercent,4},
  {CONFIG_VAR_TYPE_INT, "ChickenLifeAction",          &nChickenLifeAction,  4},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "DangerousMonster",           &anDangerousMonster,  2,{1000}},
  {CONFIG_VAR_TYPE_KEY, "DangerousMonsterChickenToggle", &tChickenDangerousMonster},
  {CONFIG_VAR_TYPE_KEY, "AutoPotionToggle", &tAutoPotion},
  {CONFIG_VAR_TYPE_INT, "DangerousMonsterAction",     &nDangerousMonsterAction, 4},
  {CONFIG_VAR_TYPE_INT, "ChickenLifeMinClevel",				&dwChickenLifeMinClevel,     4},
  {CONFIG_VAR_TYPE_INT, "ChickenLifeForcedClevel",				&dwChickenLifeForcedClevel,     4},
  {CONFIG_VAR_TYPE_INT, "NoQuitIfDeadClevel",				&dwNoQuitIfDeadClevel,     4},
  {CONFIG_VAR_TYPE_INT, "AutoProtectionNewGame",				&dwChickenLifeEnterGame,     4},
};
void dangerous_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void dangerous_initConfigVars() {
	memset(anDangerousMonster,    0,        sizeof(anDangerousMonster));
}
int leader_back_to_town();
int protectExitGame() {
	if (fIsHardCoreGame&&tHardCoreAllBackTownIfProtect.isOn) {
		leader_back_to_town();
	}
	ExitGame();
	return 0;
}
int ProtectAction(wchar_t* wszShowMsg, int action = 1 , BYTE nCol = 8) {
	//1 exit game, 2 back to home
	wchar_t wszTemp[0x100];
	wcscpy( wszTemp	,	wszShowMsg);
	if ( fInGame ) {
		if (action == 1 ) {
			if (PLAYER->dwMode==PlayerMode_Death||PLAYER->dwMode==PlayerMode_Dead) {
				if (fIsHardCoreGame||dwPlayerLevel>=dwNoQuitIfDeadClevel) {
					if (dwGameLng) wcscat(wszTemp,L"已经死了不退出");
					else wcscat(wszTemp,	L"Already dead, no action.");
					gameMessageWColor(nCol,wszTemp);
					return 0;
				}
			}
			wcscat(wszTemp,	L"Exit game.");
			gameMessageWColor(nCol,wszTemp);
			protectExitGame();
		} else if ( action == 2) {
			if (!dwBackToTownTimeout) {
				wcscat(wszTemp	,	L"Back to town.");
				gameMessageWColor(nCol,wszTemp);
				BackToTown();
			}
		}
	}
	return 1;
}
static void rescanMercPointers() {
	int n=0;
	int *lastM=pMercHpPercent;pMercHpPercent=NULL;
	int *lastG=pGolemHpPercent;pGolemHpPercent=NULL;
	if (dwHirePotionColumn&&dwHirePotionLifePercent
		||dwIronGolemLifeAlertPercent
		||dwIronGolemLifeTownPercent
		||dwIronGolemLifeExitPercent
		) {
		for (PetUnit *pPetUnit = *d2client_pPetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) {
			if (pPetUnit->dwOwerType!=UNITNO_PLAYER) continue;
			if (pPetUnit->dwOwnerId!=dwPlayerId) continue;
			if (pPetUnit->dwPetType==3&&pPetUnit->dwTxtFileNo==291) {
				if (lastG!=(int *)&pPetUnit->dwHpPercent) {
					fIronGolemIsRuneword=0;
					UnitAny *golem=d2client_GetUnitFromId(pPetUnit->dwUnitId, UNITNO_MONSTER) ;
					if (golem) {
						UnitAny *item=d2common_GetFirstItemInInv(golem->pInventory);
						if (item&&d2common_CheckItemFlag(item, ITEMFLAG_RUNEWORD, 0, "?"))
							fIronGolemIsRuneword=1;
					}
					if (lastG) gameMessageWColor(0,L"Iron Golem HP pointer changed");
				}
				pGolemHpPercent=(int *)&pPetUnit->dwHpPercent;
				if ((n++) >=2) break;
			} else if (pPetUnit->dwPetType==7) {
				if (lastM&&lastM!=(int *)&pPetUnit->dwHpPercent) {
					gameMessageWColor(0,L"Merc HP pointer changed");
				}
				pMercHpPercent=(int *)&pPetUnit->dwHpPercent;
				if ((n++) >=2) break;
			}
		}
	}
}

extern ToggleVar tBugAutoQuitHell,tBugAutoQuitHellAct4,tBugAutoQuitHellAct5,tChickenLife,tSocketProtect;
extern ToggleVar tDropProtectionToggle,tResetProtectionToggle,tRunewordProtect,tAutoTeleportEvade;
void ChickenLifeNewGame() {
	static int bugHell=0,bugHellA4=0,bugHellA5=0,autoEvade=0;
	if (tBugAutoQuitHell.isOn) bugHell=1;
	if (tBugAutoQuitHellAct4.isOn) bugHellA4=1;
	if (tBugAutoQuitHellAct5.isOn) bugHellA5=1;
	if (tAutoTeleportEvade.isOn) autoEvade=1;
	if (dwChickenLifeEnterGame) {
		tChickenLife.isOn=1;
		tSocketProtect.isOn=1;
		tRunewordProtect.isOn=1;
		tDropProtectionToggle.isOn=1;
		tResetProtectionToggle.isOn=1;
		if (bugHell) tBugAutoQuitHell.isOn=1;
		if (bugHellA4) tBugAutoQuitHellAct4.isOn=1;
		if (bugHellA5) tBugAutoQuitHellAct5.isOn=1;
		if (autoEvade) tAutoTeleportEvade.isOn=1;
	}
	lastChickenLifeThreshold=-1;
	dwCheckMercMs=dwCurMs+3000;manaMs=0;healingMs=0;golemMs=0;hireMs=0;
	pMercHpPercent=NULL;pGolemHpPercent=NULL;
	fIronGolemIsRuneword=0;fIronGolemTownProtection=0;
	dwCheckRelationMs=dwCurMs+100;
	if (EXPANSION) rescanMercPointers();
}

void ChickenLifeLoop() {
	if (dwCheckRelationMs&&dwCurMs>dwCheckRelationMs) {
		/*
		LOG("checkRelation pid=%d\n",dwPlayerId);
		for (RosterUnit *pUnit=PLAYERLIST;pUnit;pUnit=pUnit->pNext) {
			LOG(" uid=%d:\n",pUnit->dwUnitId);
			if (!pUnit->pPvPInfo) continue;
			for (PvPInfo *pPvPInfo=*(pUnit->pPvPInfo);pPvPInfo;pPvPInfo=pPvPInfo->pNext) {
				LOG("  %d:%X\n",pPvPInfo->dwUnitId,pPvPInfo->dwFlag);
			}
		}
		*/
		static int lastState=0;
		hasHostilePlayer=0;
		for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext ) {
			int pvp=testPvpFlag(pUnit->dwUnitId);if (pvp==3) continue;
			//LOG("%d %X %s\n",pvp,pUnit->dwPartyFlags,pUnit->szName);
			if (pvp==0) hasHostilePlayer++;
		}
		if (hasHostilePlayer!=lastState) {
			gameMessage("HostilePlayer: %d",hasHostilePlayer);
			lastState=hasHostilePlayer;
		}
		dwCheckRelationMs=0;
	}
	if (PLAYER->dwMode==PlayerMode_Death) {
		if (fIsHardCoreGame||dwPlayerLevel>=dwNoQuitIfDeadClevel) {
			if (!fLifeProtectOn) {
				fLifeProtectOn=1;
				gameMessageW(dwGameLng?L"已经挂了不退出":L"Already dead, no action.");
			}
			return;
		}
	}
	if (PLAYER->dwMode==PlayerMode_Dead) return;
	if (tChickenLife.isOn || tChickenHostile.isOn){
		if (fPlayerInTown ) {
			fLifeProtectOn = FALSE ;
			fIronGolemTownProtection=0;
			return;
		}
		if ( fLifeProtectOn ) return ;
		if (EXPANSION&&dwCheckMercMs&&dwCurMs>dwCheckMercMs) {
			rescanMercPointers();
			dwCheckMercMs=dwCurMs+10000;
		}
		if (tAutoPotion.isOn) {
			if (fState106ManaMs&&dwCurMs>fState106ManaMs+10000) fState106ManaMs=0;
			if (tManaPotion.isOn&&(!manaMs||dwCurMs>=manaMs)&&!fState106ManaMs) {
				if (dwPlayerMaxMana>=dwManaPotionMax) {
					if (LEVELNO!=121&&dwPlayerMana<=dwManaPotionValue
						||LEVELNO==121&&dwPlayerMana<=dwManaPotionNTValue) {
							if (dwMPotionCount>0) {
								partyMessageWColor(0,L"Auto mana potion");
								manaMs=dwCurMs+dwManaPotionDelayMs;
								usePotion(1);
							}
					}
				}
			}
			if (tHealingPotion.isOn&&dwHealingPotionLifePercent&&(!healingMs||dwCurMs>=healingMs)&&!fState100HP) {
				if(dwPlayerHP*100 <= dwPlayerMaxHP*dwHealingPotionLifePercent ){
					if (dwHPotionCount>0) {
						partyMessageWColor(0,L"Auto healing potion");
						healingMs=dwCurMs+dwHealingPotionDelayMs;
						usePotion(0);
					}
				}
			}
			if (EXPANSION&&dwHirePotionColumn&&dwHirePotionLifePercent
				&&pMercHpPercent&&0<*pMercHpPercent&&*pMercHpPercent<dwHirePotionLifePercent
				&&(!hireMs||dwCurMs>=hireMs)&&d2client_pBeltNotEmpty[dwHirePotionColumn-1]) {
				hireMs=dwCurMs+dwHirePotionDelayMs;
				UnitAny *pet=GetUnitPet(PLAYER);
				if (!pet||pet->dwMode==12) {
					pMercHpPercent=NULL;
				} else if (dwPlayerLevel>=dwHirePotionMinCLevel) {
					partyMessageWColor(0,L"Auto merc potion");
					useBelt(dwHirePotionColumn-1,1);
				} else d2client_PlaySound(PLAYER->dwUnitId, UNITNO_PLAYER,0x19);
			}
		}
		if (EXPANSION&&fIronGolemIsRuneword&&pGolemHpPercent&&0<*pGolemHpPercent) {
			if (dwIronGolemLifeExitPercent&&*pGolemHpPercent<dwIronGolemLifeExitPercent) {
				ProtectAction(L"<Hackmap>: Iron Golem Life below threshold,", 1);
				fLifeProtectOn = TRUE;
			} else if (!fIronGolemTownProtection
				&&dwIronGolemLifeTownPercent&&*pGolemHpPercent<dwIronGolemLifeTownPercent) {
				ProtectAction(L"<Hackmap>: Iron Golem Life below threshold,", 2);
				fIronGolemTownProtection = TRUE;
			} else if (dwIronGolemLifeAlertPercent&&*pGolemHpPercent<dwIronGolemLifeAlertPercent) {
				wchar_t wszbuf[256];
				wsprintfW(wszbuf, L"IronGolem%02d%%",*pGolemHpPercent);
				setBottomAlertMsg(0,wszbuf,100,1,1,2);
				if (!golemMs||dwCurMs>=golemMs) {
					golemMs=dwCurMs+2000;
					d2client_PlaySound(PLAYER->dwUnitId, UNITNO_PLAYER,0x19);
				}
			}
		}

		if ((fIsHardCoreGame||dwPlayerLevel>=(int)dwChickenLifeMinClevel) && tChickenLife.isOn
			||dwPlayerLevel>=(int)dwChickenLifeForcedClevel){
			int threshold;
			if (dwPlayerMaxHP>=dwChickenLifeCondition) threshold=dwChickenLife;
			else {
				if (fIsHardCoreGame) {
					threshold=dwPlayerMaxHP*dwHCChickenLifePercent/100;
				} else {
					threshold=dwPlayerMaxHP*dwChickenLifePercent/100;
				}
			}
			if (threshold!=lastChickenLifeThreshold) {
				if (dwGameLng) gameMessageW(L"血保数值%d",threshold);
				else gameMessage("Life Protect Threshold %d",threshold);
				lastChickenLifeThreshold=threshold;
			}
			if (dwPlayerHP<=threshold) {
				ProtectAction(L"<Hackmap>: Life below chicken threshold,", nChickenLifeAction);
				fLifeProtectOn = TRUE;
				return;
			}
		}
		if (tChickenHostile.isOn&&hasHostilePlayer) {
			for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext ) {
				if (testPvpFlag(pUnit->dwUnitId)==0) {
					if ( (dwChickenHostileLife && dwPlayerHP<=dwChickenHostileLife) 
						|| (dwChickenHostileLifePercent && dwPlayerHP*100 <= dwPlayerMaxHP*dwChickenHostileLifePercent) ){
						ProtectAction(L"<Hackmap>: Life below chicken hostile threshold,",nChickenLifeAction);
						fLifeProtectOn = TRUE;
						return;
					}
				}
			}
		}
	}
	if (ceHp) {
		//range 8 yard
		wchar_t wszbuf[32];
		wsprintfW(wszbuf, L"CE %d %d%%",(ceDisRaw256*2/3)>>8,ceHp);
		setBottomAlertMsg(0,wszbuf,300,1,3,0);
		if (ceDisRaw256<=9*3/2*256) setBottomAlertBg(0,0x10,0x62);
	}
	if (feHp) {
		//2.7 yard safe, 2.1 yard hit
		wchar_t wszbuf[32];
		wsprintfW(wszbuf, L"FE %d %d%%",(feDisRaw256*2/3)>>8,feHp);
		setBottomAlertMsg(1,wszbuf,300,1,1,0);
		if (feDisRaw256<=3*3/2*256) {
			setBottomAlertBg(1,0x10,0x62);
			if (fIsHardCoreGame&&dwHardCoreFEHpProtect&&feHp<=dwHardCoreFEHpProtect) {
				gameMessageW(dwGameLng?L"FE保护,血量%d%%距离%d":L"FE Protect,HP%d%% %d yard",feHp,(feDisRaw256*2/3)>>8);
				protectExitGame();
			}
		}
	}
	if (cowKingHp) {
		wchar_t wszbuf[32];
		wsprintfW(wszbuf, L"CowKing %d %d%%",cowKingDis,cowKingHp);
		setBottomAlertMsg(1,wszbuf,300,1,1,1);
	}
	cowKingHp=0;cowKingDis=1000;
	feHp=0;ceHp=0;
	ceDisRaw256=12*3/2*256;
	feDisRaw256=6*3/2*256;
}
extern int HasCowKing;
int isCowKing(MonsterData *pMonsterData) {
	if (!HasCowKing) return 0;
	if (LEVELNO!=39) return 0;
	if (!pMonsterData->fUnique) return 0;
	int le=0,m=0;
	for (int i = 0; i < 9; i++) {
		int enchno = pMonsterData->anEnchants[i];
		if (enchno==8) m=i+1;
		else if (enchno==17) le=i+1;
	}
	return m&&le&&m<le;
}
void checkBossMonster(UnitAny *pUnit) {
	MonsterData *pMonsterData = pUnit->pMonsterData;
	if (HasCowKing&&LEVELNO==39&&isCowKing(pMonsterData)) {//cow level 
		cowKingHp=d2common_GetMonsterHpPercent(pUnit);
		cowKingDis=(getPlayerDistanceM256(pUnit)*2/3)>>8;
		return;
	}
	for (int i = 0; i < 9; i++) {
		int enchno = pMonsterData->anEnchants[i];
		if (enchno==9||enchno==18) { //FE CE
			int dis=getPlayerDistanceM256(pUnit);
			if (enchno==9) {
				if (dis<feDisRaw256) {
					feDisRaw256=dis;
					feHp=d2common_GetMonsterHpPercent(pUnit);
				}
			} else {
				if (dis<ceDisRaw256) {
					ceDisRaw256=dis;
					ceHp=d2common_GetMonsterHpPercent(pUnit);
				}
			}
			break;
		}
	}
}

void CheckDangerousPlayer( UnitAny  *pUnit ) {
	if ( fPlayerInTown ){
		fLifeProtectOn = FALSE ;
		return ;
	}
	if (hasHostilePlayer&&tChickenHostileNearby.isOn && pUnit->dwMode ){
		if ( fLifeProtectOn==FALSE && testPvpFlag(pUnit->dwUnitId) ==0){
			if ( (dwChickenHostileNearbyLife && dwPlayerHP<=dwChickenHostileNearbyLife) 
				|| (dwChickenHostileNearbyLifePercent && dwPlayerHP*100 <= dwPlayerMaxHP*dwChickenHostileNearbyLifePercent) ){
				ProtectAction(L"<Hackmap>: Life below chicken hostile nearby threshold,", nChickenLifeAction );
				fLifeProtectOn = TRUE;
			}
		}
	}
}

BOOL TestMonsterOnScreen(UnitAny *mon, UnitAny *pla) {
	return abs((int)mon->pMonPath->drawX-(int)pla->pMonPath->drawX) < SCREENSIZE.x/2
		&& abs((int)mon->pMonPath->drawY-(int)pla->pMonPath->drawY) < (SCREENSIZE.y-48)/2;
}
void CheckDangerousMonster( UnitAny  *pUnit ) {
	static DWORD statindexs[] = {STAT_FIRE_RESIST,
					STAT_COLD_RESIST, 
					STAT_LIGHTING_RESIST, 
					STAT_POSION_RESIST};
	if ( fPlayerInTown ){
		fDangerousMonsterActive = FALSE ;
		return ;
	}
	if( tChickenDangerousMonster.isOn && pUnit->dwMode && pUnit->dwMode!=0x0C){
		if ( fDangerousMonsterActive == FALSE ) {
			if (anDangerousMonster[pUnit->dwTxtFileNo][0] && anDangerousMonster[pUnit->dwTxtFileNo][0] < 5 && TestMonsterOnScreen(pUnit, PLAYER)) {
				int stat = GetUnitResist( PLAYER, statindexs[anDangerousMonster[pUnit->dwTxtFileNo][0]-1] );
				if (anDangerousMonster[pUnit->dwTxtFileNo][1] >= stat) {
					fDangerousMonsterActive = TRUE;
					ProtectAction(L"<Hackmap>: player's resistance below chicken threshold,",3);
				}
			}
		}
	}
}
int canLeaveTown() {
	if (!fIsHardCoreGame) return 0;
	if (fIsHardCoreSafe) {
		gameMessageW(dwGameLng?L"专家级标记为safe的人物不能出城":L"Hard Core Safe can't leave town");
		return 0;
	}
	int lvl=hardCoreLeaveTownResist[DIFFICULTY][0];
	int minFR=hardCoreLeaveTownResist[DIFFICULTY][1];
	int minLR=hardCoreLeaveTownResist[DIFFICULTY][2];
	int minCR=hardCoreLeaveTownResist[DIFFICULTY][3];
	int minPR=hardCoreLeaveTownResist[DIFFICULTY][4];
	//LOG("leaveTown %d %d %d %d\n",lvl,minFR,minLR,minCR,minPR);
	if (tHardCoreResistProtect.isOn&&dwPlayerLevel>=lvl) {
		int fr=d2common_GetUnitStat(PLAYER,STAT_FIRE_RESIST,0);
		int lr=d2common_GetUnitStat(PLAYER,STAT_LIGHTING_RESIST,0);
		int cr=d2common_GetUnitStat(PLAYER,STAT_COLD_RESIST,0);
		int pr=d2common_GetUnitStat(PLAYER,STAT_POSION_RESIST,0);
		if (fr<minFR||lr<minLR||cr<minCR||pr<minPR) {
			char keyname[256];formatKey(keyname,tHardCoreResistProtect.key);
			wchar_t wbuf[128];int pos=0;
			pos+=wsprintfW(wbuf+pos,dwGameLng?L"大号抗性保护:级别%d>=%d":L"Resist Protect:Lv%d>=%d",dwPlayerLevel,lvl);
			if (fr<minFR) pos+=wsprintfW(wbuf+pos,dwGameLng?L"火抗%d<%d":L" FR%d<%d",fr,minFR);
			if (lr<minLR) pos+=wsprintfW(wbuf+pos,dwGameLng?L"闪抗%d<%d":L" LR%d<%d",lr,minLR);
			if (cr<minCR) pos+=wsprintfW(wbuf+pos,dwGameLng?L"冰抗%d<%d":L" CR%d<%d",cr,minCR);
			if (pr<minPR) pos+=wsprintfW(wbuf+pos,dwGameLng?L"毒抗%d<%d":L" PR%d<%d",pr,minPR);
			pos+=wsprintfW(wbuf+pos,L"(%hs)",keyname);
			gameMessageW(wbuf);
			return 0;
		}
		//LOG("leaveTown %d %d %d %d\n",dwPlayerLevel,fr,lr,cr,pr);
	}
	if (tHardCoreLeaveTownDurability.isOn) {
		for (UnitAny *pUnit = d2common_GetFirstItemInInv(PLAYER->pInventory);pUnit;
			pUnit = d2common_GetNextItemInInv(pUnit)) {
			if (pUnit->dwUnitType!=UNITNO_ITEM) continue;
			if (pUnit->pItemData->nLocation!=3) continue; //not on body
			if (pUnit->pItemData->nItemLocation!=255) continue; //equipped
			int max_durability=d2common_GetUnitStat(pUnit, STAT_MAXDURABILITY, 0); 
			if (!max_durability) continue;
			int minDur=dwHardCoreLeaveTownDurabilityPercent*max_durability/100;
			if (minDur<2) minDur=2;
			int durability=d2common_GetUnitStat(pUnit, STAT_DURABILITY, 0); 
			if (durability<minDur) {
				char keyname[256];formatKey(keyname,tHardCoreLeaveTownDurability.key);
				gameMessageW(dwGameLng?L"出城耐久度保护,耐久度只有%d(%hs)"
					:L"Out town durability protect, only %d(%hs)",durability,keyname);
				return 0;
			}
		}
	}
	return 1;
}
