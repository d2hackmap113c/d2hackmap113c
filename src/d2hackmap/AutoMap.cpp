#include "stdafx.h"
#ifdef MODULE_AUTOMAP

struct LevelInfo{
	DrlgLevel* pDrlgLevel;
	BOOL fReady;
	BOOL fRevealed;
};
static LevelInfo aLevelInfo[137] = {{0},};

void ResetMapStatus() {
	memset(afRevealedActs,		0,		sizeof(afRevealedActs));
	memset(aLevelInfo,			0,		sizeof(aLevelInfo));
}

void InitAutomapLayer(DWORD levelno) {	
	LevelDefTxt *pLayerTxt = D2GetLevelDefTxt( levelno ) ;
	D2InitAutomapLayer( pLayerTxt->dwLayerNo );
}

void RevealAutomapRoom(DrlgRoom2 *pRoom2) {
	D2RevealAutomapRoom(pRoom2->pRoom1, 1, LAYER);
	#ifdef MODULE_AUTOMAPCELL
		AddAutoMapCell(pRoom2);
	#endif
}
void RevealAutomapLevel(DrlgLevel *pDrlgLevel){
	if ( aLevelInfo[pDrlgLevel->dwLevelNo].fRevealed==FALSE ){
		DrlgRoom2 *pRoom2 = pDrlgLevel->pRoom2First;
		while (pRoom2) {
			if(pRoom2->dwPresetType == 2) {
				DWORD dwDefNo = pRoom2->pLvlPreset->dwDef;
				if( dwDefNo == 38 || dwDefNo == 39 || 
					(dwDefNo >= 401 && dwDefNo <= 405)||
					dwDefNo == 836 || dwDefNo == 863 ){
					pRoom2 = pRoom2->pNext;
					continue;
				}
			}
			if (pRoom2->pRoom1) {
				//玩家所在区域
				RevealAutomapRoom(pRoom2);
			} else {
				//玩家之外的区域
				BYTE cmdbuf[6];
				*(WORD *)(cmdbuf+1) = (WORD)pRoom2->dwPosX;
				*(WORD *)(cmdbuf+3) = (WORD)pRoom2->dwPosY;
				cmdbuf[5] = (BYTE)pRoom2->pDrlgLevel->dwLevelNo;
				D2RecvCommand07(cmdbuf);
				RevealAutomapRoom(pRoom2);
				D2RecvCommand08(cmdbuf);
			}
			pRoom2 = pRoom2->pNext;
		}
		aLevelInfo[pDrlgLevel->dwLevelNo].fRevealed = TRUE;
	}
}

void RevealLevel(DrlgLevel *pDrlgLevel){
	if (!pDrlgLevel) return;
	if (!pDrlgLevel->pRoom2First) {
		D2InitDrlgLevel(pDrlgLevel);
	}
	InitAutomapLayer( pDrlgLevel->dwLevelNo );
	RevealAutomapLevel( pDrlgLevel );

}

int RevealLevelPlayerIn(){
	DrlgLevel *pDrlgLevel = PLAYER->pMonPath->pRoom1->pRoom2->pDrlgLevel;
	if (!pDrlgLevel) return -1;
	RevealAutomapLevel( pDrlgLevel );
	return 0;
}

void InitDrlgLevelInfo(DrlgMisc *drlgmisc){
	for (DrlgLevel* pDrlgLevel = drlgmisc->pLevelFirst; pDrlgLevel; pDrlgLevel = pDrlgLevel->pNext) {
		aLevelInfo[pDrlgLevel->dwLevelNo].pDrlgLevel = pDrlgLevel;
		aLevelInfo[pDrlgLevel->dwLevelNo].fReady = TRUE;
	}

	for ( DWORD lvl = actlvls[ACTNO];  lvl < actlvls[ACTNO+1];  lvl++ ) {
		if ( aLevelInfo[lvl].fReady == FALSE ){
			aLevelInfo[lvl].pDrlgLevel = D2GetDrlgLevel(drlgmisc, lvl);
			aLevelInfo[lvl].fReady = TRUE;
		}
	}
}

int RevealAutomap(){
	if ( afRevealedActs[ACTNO] == TRUE  ) return 0;
	InitDrlgLevelInfo(ACT->pDrlgMisc);
	for ( DWORD lvl = actlvls[ACTNO];  lvl < actlvls[ACTNO+1];  lvl++ ) {
		DrlgLevel *pDrlgLevel = aLevelInfo[lvl].pDrlgLevel;
		if (!pDrlgLevel) continue;
		if (!pDrlgLevel->pRoom2First) {
			D2InitDrlgLevel(pDrlgLevel);
		}
		InitAutomapLayer( lvl );
		RevealAutomapLevel( pDrlgLevel );

	}

	InitAutomapLayer( LEVELNO ) ; //恢复当前Layer指针
	
	afRevealedActs[ACTNO] = TRUE;

	#ifdef MODULE_MINIMAPPOINT
		BuildMiniMapPoint( ACTNO );
	#endif
	return 0;
}
void RevealAutomapLoop() {
	if ( fAutoRevealAct == FALSE )return;
	if ( afRevealedActs[ACTNO] == TRUE  ) return;
	RevealAutomap();
}

void ShowAutoMap(){
	if ( tAutoMap.isOn ){
		D2ShowMap();
	}
}
#endif
