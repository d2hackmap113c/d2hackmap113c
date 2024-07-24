#include "stdafx.h"

#ifdef MODULE_CHECKDANGEROUS

extern int fState100HP,fState106Mana;
int dwCheckMercMs;
static int checkMs=0,manaMs=0,golemMs=0,dwClearAlertMs,hireMs=0,dwMaxlife=0,dwMaxMana=0,dwLvl=0;
static int *pMercHpPercent=NULL;
static int *pGolemHpPercent=NULL;
static int fIronGolemIsRuneword,fIronGolemTownProtection;

void ProtectAction(wchar_t* wszShowMsg, int action = 1 , BYTE nCol = 8) {
	//  1 exit game  , 2 back to home , 3 show message  4 drink
	wchar_t wszTemp[0x100];
	wcscpy( wszTemp	,	wszShowMsg);
	if ( fInGame ) {
		if (action == 1 ) {
			wcscat(wszTemp	,	L"Exit game.");
			D2ShowGameMessage(	wszTemp, nCol);
			ExitGame();
		} else if ( action == 2) {
			if ( fBackToTown==FALSE ){
				wcscat(wszTemp	,	L"Back to town.");
				D2ShowGameMessage( wszTemp, nCol );
				BackToTown();
			}
		} else if ( action == 3) {
			wcscat(wszTemp	,	L"Be careful.");
			D2ShowGameMessage( wszTemp, nCol );
		} else if ( action == 4) {
			//todo
		}
	}
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
		for (PetUnit *pPetUnit = *p_D2PetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) {
			if (pPetUnit->dwOwerType!=UNITNO_PLAYER) continue;
			if (pPetUnit->dwOwnerId!=dwPlayerId) continue;
			if (pPetUnit->dwPetType==3&&pPetUnit->dwTxtFileNo==291) {
				if (lastG!=(int *)&pPetUnit->dwHpPercent) {
					fIronGolemIsRuneword=0;
					UnitAny *golem=D2GetUnitFromId(pPetUnit->dwUnitId, UNITNO_MONSTER) ;
					if (golem) {
						UnitAny *item=D2GetFirstItemInInv(golem->pInventory);
						if (item&&D2CheckItemFlag(item, ITEMFLAG_RUNEWORD, 0, "?"))
							fIronGolemIsRuneword=1;
					}
					if (lastG) D2ShowGameMessage(L"Iron Golem HP pointer changed", 0);
				}
				pGolemHpPercent=(int *)&pPetUnit->dwHpPercent;
				if ((n++) >=2) break;
			} else if (pPetUnit->dwPetType==7) {
				if (lastM&&lastM!=(int *)&pPetUnit->dwHpPercent) {
					D2ShowGameMessage(L"Merc HP pointer changed", 0);
				}
				pMercHpPercent=(int *)&pPetUnit->dwHpPercent;
				if ((n++) >=2) break;
			}
		}
	}
}

void ChickenLifeNewGame() {
	static int bugHell=0,bugHellA5=0;
	if (tBugAutoQuitHell.isOn) bugHell=1;
	if (tBugAutoQuitHellAct5.isOn) bugHellA5=1;
	if (dwChickenLifeEnterGame) {
		tChickenLife.isOn=1;
		tSocketProtect.isOn=1;
		tDropProtectionToggle.isOn=1;
		tResetProtectionToggle.isOn=1;
		if (bugHell) tBugAutoQuitHell.isOn=1;
		if (bugHellA5) tBugAutoQuitHellAct5.isOn=1;
	}
	dwCheckMercMs=dwCurMs+3000;checkMs=0;manaMs=0;golemMs=0;dwClearAlertMs=0;
	hireMs=0;dwMaxlife=0;dwMaxMana=0;dwLvl=0;
	pMercHpPercent=NULL;pGolemHpPercent=NULL;
	fIronGolemIsRuneword=0;fIronGolemTownProtection=0;
	rescanMercPointers();
}

void ChickenLife() {
	if (dwClearAlertMs&&dwCurMs>dwClearAlertMs) {dwClearAlertMs=0;SetCenterAlertMsg(0,L"");}
	if (PLAYER->dwMode==0x11) return ;
	if (tChickenLife.isOn || tChickenHostile.isOn){
		if (fPlayerInTown ) {
			fLifeProtectOn = FALSE ;
			fIronGolemTownProtection=0;
			return;
		}
		if ( fLifeProtectOn ) return ;
		if (dwCheckMercMs&&dwCurMs>dwCheckMercMs) {
			rescanMercPointers();
			dwCheckMercMs=dwCurMs+10000;
		}
		if (!checkMs||dwCurMs>=checkMs) {
			dwLvl = D2GetUnitStat(PLAYER, STAT_LEVEL, 0);
			dwMaxlife = D2GetUnitStat(PLAYER, STAT_MAXHP, 0)>>8;
			dwMaxMana = D2GetUnitStat(PLAYER, STAT_MAXMANA, 0)>>8;
			checkMs=dwCurMs+dwAutoPotionCheckMs;
		}
		if (tAutoPotion.isOn) {
			if (dwManaPotionColumn&&(!manaMs||dwCurMs>=manaMs)) {
				if (dwMaxMana>=dwManaPotionMax) {
					int dwMana = D2GetUnitStat(PLAYER, STAT_MANA, 0)>>8;
					if (LEVELNO!=121&&dwMana<=dwManaPotionValue
						||LEVELNO==121&&dwMana<=dwManaPotionNTValue) {
						if (p_D2BeltNotEmpty[dwManaPotionColumn-1]&&!fState106Mana) {
							D2ShowGameMessage(L"Auto mana potion", 0);
							manaMs=dwCurMs+dwManaPotionDelayMs;
							GameControl(22+dwManaPotionColumn);
						}
					}
				}
			}
			if (dwHirePotionColumn&&dwHirePotionLifePercent
				&&pMercHpPercent&&0<*pMercHpPercent&&*pMercHpPercent<dwHirePotionLifePercent
				&&(!hireMs||dwCurMs>=hireMs)&&p_D2BeltNotEmpty[dwHirePotionColumn-1]) {
				hireMs=dwCurMs+dwHirePotionDelayMs;
				if (dwLvl>=dwHirePotionMinCLevel) {
					D2ShowGameMessage(L"Auto merc potion", 0);
					useBelt(dwHirePotionColumn-1,1);
				} else D2PlaySound(PLAYER->dwUnitId, UNITNO_PLAYER,0x19);
			}
		}
		if (fIronGolemIsRuneword&&pGolemHpPercent&&0<*pGolemHpPercent) {
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
				SetCenterAlertMsgParam(1,2,250);
				SetCenterAlertMsg(1,wszbuf);
				dwClearAlertMs=dwCurMs+100;
				if (!golemMs||dwCurMs>=golemMs) {
					golemMs=dwCurMs+2000;
					D2PlaySound(PLAYER->dwUnitId, UNITNO_PLAYER,0x19);
				}
			}
		}
		DWORD dwLife = D2GetUnitStat(PLAYER, STAT_HP, 0)>>8;

		if (dwLvl>=(int)dwChickenLifeMinClevel && tChickenLife.isOn
			||dwLvl>=(int)dwChickenLifeForcedClevel){
			if( dwChickenLife && (int)dwChickenMaxLife<=dwMaxlife && dwLife<=dwChickenLife) {
				dwMaxlife = D2GetUnitStat(PLAYER, STAT_MAXHP, 0)>>8;
				if(dwChickenLife && (int)dwChickenMaxLife<=dwMaxlife && dwLife<=dwChickenLife) {
					ProtectAction(L"<Hackmap>: Life below chicken threshold,", nChickenLifeAction);
					fLifeProtectOn = TRUE;
					return;
				}
			}
			if(dwLife<dwChickenMaxLife && dwChickenLifePercent 
				&& dwLife*100 <= dwMaxlife*dwChickenLifePercent ){
				ProtectAction(L"<Hackmap>: Life below chicken threshold,", nChickenLifeAction);
				fLifeProtectOn = TRUE;
				return;
			}
		}
		if ( tChickenHostile.isOn ) {
			for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext ) {
				if ( TestPvpFlag(pUnit->dwUnitId , dwPlayerId) ==0 ){
					if ( (dwChickenHostileLife && dwLife<=dwChickenHostileLife) || (dwChickenHostileLifePercent && dwLife*100 <= dwMaxlife*dwChickenHostileLifePercent) ){
						ProtectAction(L"<Hackmap>: Life below chicken hostile threshold,",nChickenLifeAction);
						fLifeProtectOn = TRUE;
						return;
					}
				}
			}
		}
	}
}


void CheckDangerousPlayer( UnitAny  *pUnit ) {
	if ( fPlayerInTown ){
		fLifeProtectOn = FALSE ;
		return ;
	}
	if ( tChickenHostileNearby.isOn && pUnit->dwMode ){
		if ( fLifeProtectOn==FALSE && TestPvpFlag(dwPlayerId , pUnit->dwUnitId) ==0){
			DWORD dwLife = D2GetUnitStat(PLAYER, STAT_HP, 0)>>8;
			DWORD dwMaxlife = D2GetUnitStat(PLAYER, STAT_MAXHP, 0)>>8;
			if ( (dwChickenHostileNearbyLife && dwLife<=dwChickenHostileNearbyLife) || (dwChickenHostileNearbyLifePercent && dwLife*100 <= dwMaxlife*dwChickenHostileNearbyLifePercent) ){
				ProtectAction(L"<Hackmap>: Life below chicken hostile nearby threshold,", nChickenLifeAction );
				fLifeProtectOn = TRUE;
			}
		}
	}
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

#endif
