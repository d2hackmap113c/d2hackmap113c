#include "stdafx.h"

#ifdef MODULE_ITEMDEFINE

extern int dwTownPortalCount,dwIdentifyPortalCount;
//物品定义相关

int GetItemIndex(int pos) {
	if (pos < *p_D2WeaponsTxts) return pos;
	pos -= *p_D2WeaponsTxts;
	if (pos < *p_D2ArmorTxts) return pos+1000;
	return pos - *p_D2ArmorTxts + 2000;
}

BYTE GetItemColour(UnitAny *pItem,DWORD arridx) {
	ItemTxt *pItemTxt = D2GetItemTxt( pItem->dwTxtFileNo );

	if ( pItemTxt->nType==4 ) {//gold
		return (D2GetUnitStat(pItem, STAT_GOLD, 0) >= (int)dwGoldGoodNum) ? anGoldGoodCol[arridx] : anGoldPoorCol[arridx];
	}

	if ( pItemTxt->nType == 74) {//rune
		DWORD dwRuneNo = (pItemTxt->szCode[1]-'0')*10+(pItemTxt->szCode[2]-'0')-1;
		if ( dwRuneNo < 100 ) return anRuneColours[dwRuneNo][arridx];
	}

	int index = GetItemIndex(pItem->dwTxtFileNo);
	if ( index >2999 ) index= 2999;

	BYTE nShowClevel = anItemShowCLevel[index];
	if (nShowClevel!=(BYTE)-1 ) {
		DWORD lvl = D2GetUnitStat(PLAYER, STAT_LEVEL, 0);
		if (lvl<=nShowClevel) return (BYTE)-1;
	}
	DWORD dwQuality = (pItem->pItemData->dwQuality-1)&7;
	if ( arridx>0 ){
		BYTE nHiddenLevel = anItemHideLevel[index][dwQuality];//小于隐藏等级的隐藏
		if (nHiddenLevel!=(BYTE)-1 && ( pItem->pItemData->dwItemLevel < (DWORD)(nHiddenLevel>99?0:nHiddenLevel) )  ) return (BYTE)-2;
	}
	int socknum = D2GetUnitStat(pItem, STAT_NUMSOCKETS, 0);
	if( socknum > 6) socknum = 6;
	char color = anItemColours[index][dwQuality][!!D2CheckItemFlag(pItem, ITEMFLAG_ETHEREAL, 0, "?")][socknum][arridx];
	if (color==-2) {
		if (fMonitorQuantity) {
			if (index==2021&&dwTownPortalCount<20) return (BYTE)-1;
			if (index==2022&&dwIdentifyPortalCount<20) return (BYTE)-1;
		}
	}
	return (BYTE)color;
}


int SwitchItemMode(){
	DWORD lastmode = nItemShowMode;
	nItemShowMode = (++nItemShowMode)%4 ;
	if(nItemShowMode==0)nItemShowMode=1;
	wchar_t temp[512];
	wsprintfW(temp, L"<Hackmap>: Switch to Item Show Mode  %d to %d",lastmode, nItemShowMode);
	D2ShowPartyMessage(temp, 0);
	return 0;
}



#endif
