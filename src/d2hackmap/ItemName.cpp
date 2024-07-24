#include "stdafx.h"

#ifdef MODULE_ITEMNAME
	
BOOL ItemHaveLevel(BYTE nType)
{	
	if (nType==74)return FALSE;
	if (nType==4)return FALSE;
	if (nType==22)return FALSE;
	if (nType>=76 && nType<=81)return FALSE;
	if (nType>=91 && nType<=102)return FALSE;
	return TRUE;
}

void __fastcall ItemNamePatch(wchar_t *wscNameOld, UnitAny *pItem ,ItemTxt *pItemTxt ){
	wchar_t wscTemp[0x40];
	memset(wscTemp,0,sizeof(wscTemp));

	wchar_t wscName[0x100];
	memset(wscName,0,sizeof(wscName));
	wcscpy(wscName,wscNameOld);


	int showValue=tItemValue.isOn;
	BYTE nColour = GetItemColour(pItem,0);
	int itemIndex = GetItemIndex(pItem->dwTxtFileNo);
	if (fSimplifiedPotionName) {
		static wchar_t *potions[2][10]={
			{L"H1",L"H2",L"H3",L"H4",L"H",
			L"M1",L"M2",L"M3",L"M4",L"M",},
			{L"疗1",L"疗2",L"疗3",L"疗4",L"疗",
			L"法1",L"法2",L"法3",L"法4",L"法",}
			};
		wchar_t *name=NULL;
		if (itemIndex==2007) name=L"紫";
		else if (itemIndex==2008) name=L"全紫";
		else if (2079<=itemIndex&&itemIndex<=2088) name=potions[dwGameLng][itemIndex-2079];
		else if (2049<=itemIndex&&itemIndex<=2078) showValue=0;//gem
		if (name) {wcscpy(wscName,name);showValue=0;}
	}
	trimspaces(wscName);
	if (pItemTxt->nType==4) {// gold
		wcsrcat(wscName, wsprintfW2(wscTemp, "%d ", D2GetUnitStat(pItem, STAT_GOLD, 0) ) );
	}else{
		DWORD dwQuality =(pItem->pItemData->dwQuality-1)&7;
		if ( (nColour == (BYTE)-1 ) && pItemTxt->fQuest) {
			nColour = nQuestItemColour; //任务物品
		}

		if ( pItemTxt->fQuest && pItemTxt->fDiffCheck  && D2GetUnitStat(pItem, STAT_ITEMDIFFICULTY, 0)< DIFFICULTY ){
			nColour = (BYTE)-1 ;  //无效任务道具,原代码会校验，不着色
		}
		if ( D2CheckItemFlag(pItem, ITEMFLAG_BROKEN, 0, "?") ){
			nColour = 1 ; //损坏的道具
		};

		// ethereal
		if ( D2CheckItemFlag(pItem, ITEMFLAG_ETHEREAL, 0, "?") )
		{

			if (wszEtherealItemPrefix[0]){
				wcsrcat(wscName, wscolorcpy(wscTemp,wszEtherealItemPrefix,nColour));
			}
			if (wszEtherealItemPostfix[0]){
				wscolorcpy(wscName+wcslen(wscName),wszEtherealItemPostfix,nColour);
			}
		}
		// rune numbers
		if (tRuneNumbers.isOn && (pItemTxt->nType == 74)) {
			int bktnum = (pItemTxt->szCode[1]-'0')*10+(pItemTxt->szCode[2]-'0');
			wsprintfW(wscName+wcslen(wscName), L" (%d)", bktnum);
		}
	
		// socket number
		if (tSocketNumbers.isOn && D2CheckItemFlag(pItem, ITEMFLAG_SOCKETED, 0, "?") ) {
			wsprintfW(wscName+wcslen(wscName), L" (%d)", D2GetUnitStat(pItem, STAT_NUMSOCKETS, 0) );
		}

		// item level
		if (tItemLevel.isOn && ItemHaveLevel(pItemTxt->nType)) {
			wsprintfW(wscName+wcslen(wscName), L" (%ld)", pItem->pItemData->dwItemLevel);
		}
		// item value
		if (showValue && (pItemTxt->fQuest == 0) && D2CheckUiStatus(UIVAR_NPCTRADE)==0 ) {
			int value=D2GetItemValue( PLAYER, pItem, DIFFICULTY, QUESTDATA, dwItemValueNpc[ACTNO], 1);
			if (value>=tItemValue.value32)
				wsprintfW(wscName+wcslen(wscName), L" ($%d)", value);
		}
		// unit number
		if (tUnitNumber.isOn) {
			wsprintfW(wscName+wcslen(wscName), L" [0x%X]", pItem->dwUnitId);
		}

		// item index
		if (tItemIndex.isOn) {
			wsprintfW(wscName+wcslen(wscName), L" [i%d]", itemIndex + 1 );
		}
		
		// item Qlevel
		if (tItemQLevel.isOn) {
			wsprintfW(wscName+wcslen(wscName), L" (Q:%ld)", pItemTxt->nQLevel);
		}
		// item Magiclevel
		if (tItemMLevel.isOn) {
			wsprintfW(wscName+wcslen(wscName), L" (M:%ld)", pItemTxt->nMagicLvl);
		}
		
		// item Alevel
		if (tItemALevel.isOn) {
			if (dwQuality==3 || dwQuality==5 || dwQuality==7){
				wsprintfW(wscName+wcslen(wscName), L" (A:%ld)", GetItemAlvl( pItem->pItemData->dwItemLevel,pItemTxt->nQLevel,pItemTxt->nMagicLvl) );
			}
		}
		
		if (tAfterCube.isOn)
		{
			if (dwQuality==3)
			{
				int iLvl = GetItemILvlAfterCube( 1 , pItem->pItemData->dwItemLevel ,D2GetUnitStat(PLAYER, STAT_LEVEL, 0) );
				wcsrcat(wscName, wsprintfW2(wscTemp, "[After Craft Ilvl: %ld Alvl: %ld]\n",iLvl,GetItemAlvl(iLvl,pItemTxt->nQLevel,pItemTxt->nMagicLvl)) );
			}
			else if (dwQuality==5)
			{
				int iLvl = GetItemILvlAfterCube( 2 , pItem->pItemData->dwItemLevel ,D2GetUnitStat(PLAYER, STAT_LEVEL, 0));
				wcsrcat(wscName, wsprintfW2(wscTemp, "[After Cube Ilvl: %ld Alvl: %ld]\n",iLvl,GetItemAlvl(iLvl,pItemTxt->nQLevel,pItemTxt->nMagicLvl)) );
			}
		}

		if ( D2CheckItemFlag(pItem, ITEMFLAG_RUNEWORD, 0, "?") ){
			if ( tLifePerLevel.isOn ){
				int iPerlvValue=D2GetUnitStat(pItem, 216, 0);
				if (iPerlvValue)
				{
					char strVal [10] ;
					DblToStr(((double)iPerlvValue)/2048,3,strVal);
					wcsrcat(wscName, wsprintfW2(wscTemp, "[%s life/level]\n",strVal) );
				}
			}
			if (tShowBaseED.isOn){
				int ed = 0 ;
				if( itemIndex<1000 ){//武器
					ed = GetUnitMagicStat(pItem, 0x11);
				}else{//防具
					ed = GetUnitMagicStat(pItem, 0x10);
				}
				wcsrcat(wscName, wsprintfW2(wscTemp, "[Base Ed: %ld%%]\n",ed) );

			}
		}
	
		// item file index
		if (tItemFileIndex.isOn) {
			wcsrcat(wscName, wsprintfW2(wscTemp, " [TXT:%d F:%d Type:%d]\n",pItem->dwTxtFileNo, D2GetItemFileIndex(pItem) ,pItemTxt->nType ) );
		}

	}
	if (nColour != (BYTE)-1) ColourD2String(wscName, nColour);
	
	wscName[124] = 0 ;
	wcscpy(wscNameOld,wscName);

}
void __declspec(naked) ItemNamePatch_ASM()
{
	__asm {
//ebx = ptr unit item
//edi = ptr item name string
//ebp = pItemTxt
		push ebx
		push ebp
		push edi

		mov ecx, edi
		mov edx, ebx
		push ebp
		call ItemNamePatch
		
		pop edi
		pop ebp
		pop ebx

		mov al, [ebp+0x12A]
		ret
	}
}


wchar_t * __fastcall OpenCubeStringPatch(wchar_t *wcstr, DWORD dwColour)
{
	BYTE nColour = (BYTE)anItemColours[2041][0][0][0][0];
	if ( nColour != (BYTE)-1){
		return ColourD2String(wcstr, nColour);
	}
	if ( nQuestItemColour != (BYTE)-1 ){
		return ColourD2String(wcstr, nQuestItemColour);
	}else{
		return  wcstr;
	}
}

void __declspec(naked) OpenCubeStringPatch_ASM()
{
	__asm {
	//edi = str
	//esi = col
	mov ecx, edi
	mov edx, esi
	call OpenCubeStringPatch
	ret 4
	}
}







#endif
