#include "stdafx.h"
#include "header.h"

extern int fMonitorQuantity;
int LoadExtInfo();
int SwitchItemMode();
void SetExtInfoPos(int xPos , int yPos , int txtWidth , int txtHeight);
BYTE GetItemColour(UnitAny *pItem,int arridx);
int CheckItemVisible(UnitAny *pItem);
int 		dwGoldGoodNum=					   0;
char 			anGoldGoodCol[4]=		  {(BYTE)-1};
char 			anGoldPoorCol[4]=		  {(BYTE)-1};
char 			anRuneColours[100][4]	={		 {0}};
char 			anItemColours[3000][8][2][7][4]	={{0}};
static char anItemHideLevelDown[3000][8]={{0}};
static char anItemHideLevelUp[3000][8]={{0}};
char 			anItemShowCLevel[3000][2]		={{0}};
ToggleVar 	tSwitchItemMode={		TOGGLEVAR_DOWN,	0,	-1,		1,	"SwitchItemMode",	&SwitchItemMode};
int 	    dwItemShowMode=1;
int 			fSimplifiedPotionName=			0;
BYTE 			nQuestItemColour=			(BYTE)-1;
wchar_t 		wszEtherealItemPrefix[30]=  {L'\0'};
wchar_t 		wszEtherealItemPostfix[30]= {L'\0'};
ToggleVar 	tRuneNumbers={		TOGGLEVAR_ONOFF,	1,	-1,	1,"Rune numbers"};
ToggleVar 	tSocketNumbers={		TOGGLEVAR_ONOFF,	1,	-1,	1,"Socket numbers"};
ToggleVar 	tItemLevel={			TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemLevel"};
ToggleVar 	tItemValue={			TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemValue"};
ToggleVar 	tItemIndex={			TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemIndex"};
ToggleVar 	tItemFileIndex={		TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemFileIndex"};
ToggleVar 	tUnitNumber={		TOGGLEVAR_ONOFF,	0,	-1,	1, "UnitNumber"};
ToggleVar 	tItemQLevel={		TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemQLevel"};
ToggleVar 	tItemMLevel={		TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemMLevel"};
ToggleVar 	tItemALevel={		TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemALevel"};
ToggleVar 	tAfterCube={			TOGGLEVAR_ONOFF,	0,	-1,	1, "AfterCube"};
ToggleVar 	tLifePerLevel={		TOGGLEVAR_ONOFF,	0,	-1,	1, "LifePerLevel"};
int 		dwItemValueNpc[5]={	154,178,255,405,513 };
ToggleVar 	tShowBaseED={				TOGGLEVAR_ONOFF,	0,	-1,	1 , "Show Base ED"};
int  dwShowValuableItem= 0;
ToggleVar 	tShowIdentifiedItem={  TOGGLEVAR_ONOFF,	0,	-1,		1,	"Show Identified items"};
ToggleVar     tHiddenItems={         TOGGLEVAR_ONOFF,	1,	VK_ADD,	1,	"Show hidden items"};
ToggleVar 	tPermShowItems={				TOGGLEVAR_ONOFF,	0,	-1,	1,	"Perm show items"};
ToggleVar 	tLockAltToggle={				TOGGLEVAR_ONOFF,	0,	-1,	1,	"Lock Alt"};
ToggleVar tItemBasicStat={TOGGLEVAR_ONOFF,0,-1,1,"Item basic stat"};
ToggleVar tViewSocketable={TOGGLEVAR_ONOFF,0,-1,1,"View socketables"};
ToggleVar tViewSocketBase={TOGGLEVAR_ONOFF,0,-1,1,"View socketed base"};
ToggleVar tShowItemVariableProp={TOGGLEVAR_ONOFF,0,-1,1,"Item Variable Prop"};
ToggleVar tSocketProtect={				TOGGLEVAR_ONOFF,	0,	-1,	1 , "Socket protect"};
ToggleVar tRunewordProtect={TOGGLEVAR_ONOFF,0,-1,1,"Runeword protect"};
ToggleVar tDrawInvItemInfo={TOGGLEVAR_ONOFF,0,-1,1,"DrawInvItemInfo"};
char 			szItemExtInfoCfgName[2][256]	={			{'\0'}};
ToggleVar 	tItemExtInfo={			TOGGLEVAR_ONOFF,	0,	-1,	1 , "ItemExtInfo" , &LoadExtInfo};
BYTE 			nDefaultExtInfoColour=				8;
int maxTownPortalCount=20,maxIdentifyPortalCount=20;
int batchSimpleItemStackCount=3;
ToggleVar tDrawSimpleItemStackCount={TOGGLEVAR_ONOFF,0,-1,1,"Draw Simple Item Stack Count"};
static int dwDrawMarketItemInfoDy,dwDrawCursorItemInfoDy,showUnidentifiedItemInfo=0;
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_INT, "ShowUnidentifiedItemInfo", &showUnidentifiedItemInfo,     4 },
  {CONFIG_VAR_TYPE_INT, "DrawMarketItemInfoDy", &dwDrawMarketItemInfoDy,     4 },
  {CONFIG_VAR_TYPE_INT, "DrawCursorItemInfoDy", &dwDrawCursorItemInfoDy,     4 },
	{CONFIG_VAR_TYPE_KEY,"DrawSimpleItemStackCount",&tDrawSimpleItemStackCount},
  {CONFIG_VAR_TYPE_INT, "BatchSimpleItemStackCount", &batchSimpleItemStackCount,     4 },
  {CONFIG_VAR_TYPE_INT, "MaxTownPortalCount", &maxTownPortalCount,     4 },
  {CONFIG_VAR_TYPE_INT, "MaxIdentifyPortalCount", &maxIdentifyPortalCount,     4 },
//--- m_ItemDefine.h ---
  {CONFIG_VAR_TYPE_CHAR_ARRAY1, "ItemColours",       &anItemColours,       4, {3000 , 8 , 2 , 7}},
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "WeaponColours",     &anItemColours,       4, {1000 , 8 , 2 , 7}},
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "ArmorColours",      &anItemColours[1000], 4, {1000 , 8 , 2 , 7}},
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "MiscItemColours",   &anItemColours[2000], 4, {1000 , 8 , 2 , 7}},
  {CONFIG_VAR_TYPE_CHAR_ARRAY1, "RuneColours",       &anRuneColours,       4, {100} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "CharmColours",      &anItemColours[2095], 4, {3, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "RejuvPotColours",   &anItemColours[2007], 4, {2, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "HealingPotColours", &anItemColours[2079], 4, {5, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "ManaPotColours",    &anItemColours[2084], 4, {5, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "AmethystColours",   &anItemColours[2049], 4, {5, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "TopazColours",      &anItemColours[2054], 4, {5, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "SapphireColours",   &anItemColours[2059], 4, {5, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "EmeraldColours",    &anItemColours[2064], 4, {5, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "RubyColours",       &anItemColours[2069], 4, {5, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "DiamondColours",    &anItemColours[2074], 4, {5, 8, 2, 7} },
  //{CONFIG_VAR_TYPE_CHAR_ARRAY1, "SkullColours",      &anItemColours[2089], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_CHAR_ARRAY1, "ItemHideLvl",       &anItemHideLevelDown,     1, {3000 , 8} },
  {CONFIG_VAR_TYPE_CHAR_ARRAY1, "ItemHideLvlUp",       &anItemHideLevelUp,     1, {3000 , 8} },
  {CONFIG_VAR_TYPE_CHAR_ARRAY1,"ItemShowCLvl",&anItemShowCLevel,2,{3000}},
  {CONFIG_VAR_TYPE_CHAR_ARRAY1, "GoodGoldColour",      &anGoldGoodCol,     4,{1}},
  {CONFIG_VAR_TYPE_CHAR_ARRAY1, "PoorGoldColour",      &anGoldPoorCol,     4,{1}},
  {CONFIG_VAR_TYPE_INT, "GoodGoldNumber",      &dwGoldGoodNum,     4 },
  {CONFIG_VAR_TYPE_KEY, "SwitchItemShowMode",  &tSwitchItemMode    },
  {CONFIG_VAR_TYPE_INT, "ShowValuableItem", &dwShowValuableItem,     4 },
//--- m_ItemName.h ---
  {CONFIG_VAR_TYPE_INT, "SimplifiedPotionName",        &fSimplifiedPotionName, 1 },
  {CONFIG_VAR_TYPE_INT, "QuestItemColour",        &nQuestItemColour, 1 },
  {CONFIG_VAR_TYPE_KEY, "RuneNumbersToggle",      &tRuneNumbers        },
  {CONFIG_VAR_TYPE_KEY, "SocketNumbersToggle",    &tSocketNumbers      },
  {CONFIG_VAR_TYPE_KEY, "ItemLevelsToggle",       &tItemLevel       },
  {CONFIG_VAR_TYPE_KEY, "ItemValueToggle",        &tItemValue       },
  {CONFIG_VAR_TYPE_KEY, "ItemIndexsToggle",       &tItemIndex       },
  {CONFIG_VAR_TYPE_KEY, "ItemFileIndexsToggle",   &tItemFileIndex   },
  {CONFIG_VAR_TYPE_KEY, "UnitNumbersToggle",      &tUnitNumber      },
  {CONFIG_VAR_TYPE_D2STR, "EtherealItemPrefix",     &wszEtherealItemPrefix, 1,  {30 }},
  {CONFIG_VAR_TYPE_D2STR, "EtherealItemPostfix",    &wszEtherealItemPostfix,1,  {30 }},
  
  {CONFIG_VAR_TYPE_KEY, "ItemQLevelToggle",       &tItemQLevel      },
  {CONFIG_VAR_TYPE_KEY, "ItemMLevelToggle",       &tItemMLevel      },
  {CONFIG_VAR_TYPE_KEY, "ItemALevelToggle",       &tItemALevel      },
  {CONFIG_VAR_TYPE_KEY, "ItemAfterCubeToggle",    &tAfterCube       },

  {CONFIG_VAR_TYPE_KEY, "ItemLifePerLevelToggle", &tLifePerLevel    },
  {CONFIG_VAR_TYPE_KEY, "ShowBaseEDToggle",           &tShowBaseED      },
//--- m_ItemInGround.h ---
  {CONFIG_VAR_TYPE_KEY, "ShowIdentifiedItem",    &tShowIdentifiedItem	 },
  {CONFIG_VAR_TYPE_KEY, "HiddenItemsToggle",     &tHiddenItems         },
//--- m_PermShowItem.h ---
	{CONFIG_VAR_TYPE_KEY, "PermShowItemsToggle",		&tPermShowItems     },
	{CONFIG_VAR_TYPE_KEY, "LockAltToggle",			&tLockAltToggle     },
//--- m_ItemDesc.h ---
  {CONFIG_VAR_TYPE_KEY, "ViewSocketablesToggle",		&tViewSocketable    },
  {CONFIG_VAR_TYPE_KEY, "ItemBasicStatsToggle",		&tItemBasicStat     },
  {CONFIG_VAR_TYPE_KEY, "ViewSocketBaseItemsToggle",  &tViewSocketBase    },
//--- m_ItemVariableProp.h ---
	{CONFIG_VAR_TYPE_KEY, "ShowItemVariablePropToggle",					&tShowItemVariableProp         },
//--- m_ItemExtInfo.h ---
	{CONFIG_VAR_TYPE_STR, "ItemExtInfoFileEng",		&szItemExtInfoCfgName[0], 1,  {255 }},
	{CONFIG_VAR_TYPE_STR, "ItemExtInfoFileChi",		&szItemExtInfoCfgName[1], 1,  {255 }},
	{CONFIG_VAR_TYPE_KEY, "ItemExtInfoToggle",		&tItemExtInfo         },
	{CONFIG_VAR_TYPE_INT, "DefaultExtInfoColour",     &nDefaultExtInfoColour,   1 },
	{CONFIG_VAR_TYPE_KEY, "SocketProtectToggle",   &tSocketProtect   },
	{CONFIG_VAR_TYPE_KEY, "RunewordProtectToggle",   &tRunewordProtect   },
	{CONFIG_VAR_TYPE_KEY, "DrawInvItemInfoToggle",&tDrawInvItemInfo},
};
void item_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void item_initConfigVars() {
  memset(anGoldGoodCol,     -1,       sizeof(anGoldGoodCol));
  memset(anGoldPoorCol,     -1,       sizeof(anGoldPoorCol));
  memset(anItemColours,     -1,       sizeof(anItemColours));
  memset(anRuneColours,     -1,       sizeof(anRuneColours));
  memset(anItemHideLevelDown,   -1,       sizeof(anItemHideLevelDown));
  memset(anItemHideLevelUp,-1,sizeof(anItemHideLevelUp));
  memset(anItemShowCLevel,   -1,       sizeof(anItemShowCLevel));
  memset(wszEtherealItemPrefix ,    0 ,      sizeof(wszEtherealItemPrefix) );
  memset(wszEtherealItemPostfix ,   0 ,      sizeof(wszEtherealItemPostfix) );
}
void item_NewGame() {dwItemShowMode=EXPANSION?1:2;}

extern int dwTownPortalCount,dwIdentifyPortalCount;

wchar_t *wcs_insert(wchar_t *dest, wchar_t *src) {
	memmove(dest+wcslen(src), dest, (wcslen(dest)+1)*sizeof(wchar_t));
	memcpy(dest, src, wcslen(src)*sizeof(wchar_t));
	return dest;
}
static wchar_t * wscolorcpy(wchar_t *dest, wchar_t *src , BYTE color) {
	wcscpy(dest,src) ; 
  wchar_t *p = dest;	
	while(*p) p++;
	if (p[-1]!=255) return dest;
	if (color==(BYTE)-1) {p[-1]=0;return dest;}
	*p++='c';*p++='0'+color;*p=0;
	return dest;
}


int GetItemIndex(int dwTxtFileNo) {
	if (dwTxtFileNo < *d2common_pWeaponsTxts) return dwTxtFileNo;
	dwTxtFileNo-=*d2common_pWeaponsTxts;
	if (dwTxtFileNo<*d2common_pArmorTxts) return dwTxtFileNo+1000;
	return dwTxtFileNo-*d2common_pArmorTxts+2000;
}
int CheckItemVisible(UnitAny *pItem){
	// false 不显示 true 显示
	if( tHiddenItems.isOn ) return TRUE;
	//已辨识物品隐藏判定
	if( tShowIdentifiedItem.isOn && fPlayerInTown && pItem->pItemData->dwQuality>=3){
		if( d2common_CheckItemFlag(pItem, ITEMFLAG_IDENTIFIED, 0, "?") ) return TRUE;
	}
	//神符之语不隐藏
	if ( d2common_CheckItemFlag(pItem, ITEMFLAG_RUNEWORD, 0, "?") ) return TRUE;
	if (dwShowValuableItem) {
		int value=d2common_GetItemPrice( PLAYER, pItem, DIFFICULTY, QUESTDATA, dwItemValueNpc[ACTNO], 1);
		if (value>=dwShowValuableItem) return 1;
	}
	BYTE nColour = GetItemColour(pItem, 1);
	if ( nColour == 0xFE ) return FALSE;
	return TRUE;
}

BYTE GetItemColour(UnitAny *pItem,int isMinimap) {
	ItemTxt *pItemTxt = d2common_GetItemTxt( pItem->dwTxtFileNo );
	int arridx=isMinimap?dwItemShowMode:0;
	if ( pItemTxt->nType==4 ) {//gold
		return (d2common_GetUnitStat(pItem, STAT_GOLD, 0) >= (int)dwGoldGoodNum) ? anGoldGoodCol[arridx] : anGoldPoorCol[arridx];
	}

	if ( pItemTxt->nType == 74) {//rune
		DWORD dwRuneNo = (pItemTxt->szCode[1]-'0')*10+(pItemTxt->szCode[2]-'0')-1;
		if ( dwRuneNo < 100 ) return anRuneColours[dwRuneNo][arridx];
	}

	int index = GetItemIndex(pItem->dwTxtFileNo);
	if ( index >2999 ) index= 2999;

	BYTE nShowClevel = anItemShowCLevel[index][0];
	if (nShowClevel!=(BYTE)-1 ) {
		DWORD lvl = d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0);
		if (lvl<=nShowClevel) {
			if (isMinimap) return -1;
			return anItemShowCLevel[index][1];
		}
	}
	DWORD dwQuality = (pItem->pItemData->dwQuality-1)&7;
	int socknum = d2common_GetUnitStat(pItem, STAT_NUMSOCKETS, 0);
	if (arridx>0) {
		BYTE nHiddenLevel=anItemHideLevelDown[index][dwQuality];//小于隐藏等级的隐藏
		if (nHiddenLevel!=(BYTE)-1 && ( pItem->pItemData->dwItemLevel < (DWORD)(nHiddenLevel>99?0:nHiddenLevel) )  ) return (BYTE)-2;
		BYTE up=anItemHideLevelUp[index][dwQuality];//大于隐藏等级无孔的隐藏
		if (up!=(BYTE)-1&&pItem->pItemData->dwItemLevel>up&&socknum==0) return (BYTE)-2;
	}
	if( socknum > 6) socknum = 6;
	char color = anItemColours[index][dwQuality][!!d2common_CheckItemFlag(pItem, ITEMFLAG_ETHEREAL, 0, "?")][socknum][arridx];
	if (color==-2) {
		if (fMonitorQuantity) {
			if (index==2021&&dwTownPortalCount<maxTownPortalCount) return (BYTE)-1;
			if (index==2022&&dwIdentifyPortalCount<maxIdentifyPortalCount) return (BYTE)-1;
		}
		if (fUsingBow&&index==2018) {
			return (BYTE)-1;
		} else if (fUsingCrossBow&&index==2020) {
			return (BYTE)-1;
		} else if (fUsingThrow) {
			if (pItemTxt->nType==43||pItemTxt->nType==44) return (BYTE)-1;
		}
	}
	return (BYTE)color;
}

int SwitchItemMode(){
	dwItemShowMode++;if (dwItemShowMode>=4) dwItemShowMode=1;
	if (dwGameLng) {
		wchar_t *mode=L"附加设置";
		if (dwItemShowMode==1) mode=L"资料片设置";
		else if (dwItemShowMode==2) mode=L"非资设置";
		partyMessageW(L"<Hackmap>: 切换物品显示设置到%s",mode);
	} else {
		char *mode="Extra Mode";
		if (dwItemShowMode==1) mode="Expansion Mode";
		else if (dwItemShowMode==2) mode="Classic Mode";
		partyMessage("<Hackmap>: Switch to Item Show Mode to %s",mode);
	}
	return 0;
}

void trimspaces(char *str) {
	char *p1 = str, *p2 = str+strlen(str);
	while (isspace(*p1)) p1++;
	do p2--; while ((p2 >= p1) && isspace(*p2));
	*++p2 = 0;
	memmove(str, p1, (p2+1-p1)*sizeof(char));
}
void trimspaces(wchar_t *str) {
	wchar_t *p1 = str, *p2 = str+wcslen(str);
	while (iswspace(*p1)) p1++;
	do p2--; while ((p2 >= p1) && iswspace(*p2));
	*++p2 = 0;
	memmove(str, p1, (p2+1-p1)*sizeof(wchar_t));
}
wchar_t * __fastcall ColourD2String(wchar_t *str, DWORD col) {
	memmove(str+3, str, (wcslen(str)+1)*sizeof(wchar_t));
	str[0] = 0xff;
	str[1] = 'c';
	str[2] = '0'+(int)col;
	return str;
}
BOOL ItemHaveLevel(BYTE nType) {	
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
		if (itemIndex==2007) name=dwGameLng?L"紫":L"r";
		else if (itemIndex==2008) name=dwGameLng?L"全紫":L"R";
		else if (2079<=itemIndex&&itemIndex<=2088) name=potions[dwGameLng][itemIndex-2079];
		else if (2049<=itemIndex&&itemIndex<=2078) showValue=0;//gem
		if (name) {wcscpy(wscName,name);showValue=0;}
	}
	trimspaces(wscName);
	if (pItemTxt->nType==4) {// gold
		wsprintfW(wscTemp, L"%d ", d2common_GetUnitStat(pItem, STAT_GOLD, 0) );
		wcs_insert(wscName,wscTemp);
	}else{
		DWORD dwQuality =(pItem->pItemData->dwQuality-1)&7;
		if ( (nColour == (BYTE)-1 ) && pItemTxt->fQuest) {
			nColour = nQuestItemColour; //任务物品
		}

		if ( pItemTxt->fQuest && pItemTxt->fDiffCheck  && d2common_GetUnitStat(pItem, STAT_ITEMDIFFICULTY, 0)< DIFFICULTY ){
			nColour = (BYTE)-1 ;  //无效任务道具,原代码会校验，不着色
		}
		if ( d2common_CheckItemFlag(pItem, ITEMFLAG_BROKEN, 0, "?") ){
			nColour = 1 ; //损坏的道具
		};

		// ethereal
		if ( d2common_CheckItemFlag(pItem, ITEMFLAG_ETHEREAL, 0, "?") ) {
			if (wszEtherealItemPrefix[0]){
				wcs_insert(wscName, wscolorcpy(wscTemp,wszEtherealItemPrefix,nColour));
			}
			if (wszEtherealItemPostfix[0]){
				wscolorcpy(wscName+wcslen(wscName),wszEtherealItemPostfix,nColour);
			}
		}
		wchar_t *dst=wscName+wcslen(wscName);
		// rune numbers
		if (tRuneNumbers.isOn && (pItemTxt->nType == 74)) {
			int bktnum = (pItemTxt->szCode[1]-'0')*10+(pItemTxt->szCode[2]-'0');
			dst+=wsprintfW(dst, L" (%d)", bktnum);
		}
	
		// socket number
		if (tSocketNumbers.isOn && d2common_CheckItemFlag(pItem, ITEMFLAG_SOCKETED, 0, "?") ) {
			dst+=wsprintfW(dst, L" (%d)", d2common_GetUnitStat(pItem, STAT_NUMSOCKETS, 0) );
		}

		// item level
		if (tItemLevel.isOn && ItemHaveLevel(pItemTxt->nType)) {
			dst+=wsprintfW(dst, L" (%ld)", pItem->pItemData->dwItemLevel);
		}
		// item value
		if (showValue && (pItemTxt->fQuest == 0) && d2client_CheckUiStatus(UIVAR_NPCTRADE)==0 ) {
			int value=d2common_GetItemPrice( PLAYER, pItem, DIFFICULTY, QUESTDATA, dwItemValueNpc[ACTNO], 1);
			if (value>=tItemValue.value32)
				dst+=wsprintfW(dst, L" ($%d)", value);
		}
		// unit number
		if (tUnitNumber.isOn) {
			dst+=wsprintfW(dst, L" [0x%X]", pItem->dwUnitId);
		}

		// item index
		if (tItemIndex.isOn) {
			dst+=wsprintfW(dst, L" [i%d]", itemIndex + 1 );
		}
		
		// item Qlevel
		if (tItemQLevel.isOn) {
			dst+=wsprintfW(dst, L" (Q:%ld)", pItemTxt->nQLevel);
		}
		// item Magiclevel
		if (tItemMLevel.isOn) {
			dst+=wsprintfW(dst, L" (M:%ld)", pItemTxt->nMagicLvl);
		}
		
		// item Alevel
		if (tItemALevel.isOn) {
			if (dwQuality==3 || dwQuality==5 || dwQuality==7){
				dst+=wsprintfW(dst, L" (A:%ld)", GetItemAlvl( pItem->pItemData->dwItemLevel,pItemTxt->nQLevel,pItemTxt->nMagicLvl) );
			}
		}
		
		if (tAfterCube.isOn)
		{
			if (dwQuality==3)
			{
				int iLvl = GetItemILvlAfterCube( 1 , pItem->pItemData->dwItemLevel ,d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0) );
				wsprintfW(wscTemp, L"[After Craft Ilvl: %d Alvl: %d]\n",iLvl,GetItemAlvl(iLvl,pItemTxt->nQLevel,pItemTxt->nMagicLvl));
				wcs_insert(wscName,  wscTemp);
			}
			else if (dwQuality==5)
			{
				int iLvl = GetItemILvlAfterCube( 2 , pItem->pItemData->dwItemLevel ,d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0));
				wsprintfW(wscTemp, L"[After Cube Ilvl: %d Alvl: %d]\n",iLvl,GetItemAlvl(iLvl,pItemTxt->nQLevel,pItemTxt->nMagicLvl));
				wcs_insert(wscName,wscTemp );
			}
		}

		if ( d2common_CheckItemFlag(pItem, ITEMFLAG_RUNEWORD, 0, "?") ){
			if ( tLifePerLevel.isOn ){
				int iPerlvValue=d2common_GetUnitStat(pItem, 216, 0);
				if (iPerlvValue) {
					int d1000=iPerlvValue*1000/2048;
					wsprintfW(wscTemp, L"[%d.%03d life/level]\n",d1000/1000,d1000%1000);
					wcs_insert(wscName,wscTemp);
				}
			}
			if (tShowBaseED.isOn){
				int ed = 0 ;
				if( itemIndex<1000 ){//武器
					ed = GetUnitMagicStat(pItem, 0x11);
				}else{//防具
					ed = GetUnitMagicStat(pItem, 0x10);
				}
				wsprintfW(wscTemp, L"[Base Ed: %d%%]\n",ed);
				wcs_insert(wscName,wscTemp);

			}
		}
	
		// item file index
		if (tItemFileIndex.isOn) {
			if (dwQuality==ITEM_QUALITY_UNIQUE-1) {
				wsprintfW(wscTemp, L" [TXT:%d U%d Type:%d]\n",pItem->dwTxtFileNo, d2common_GetItemFileIndex(pItem) ,pItemTxt->nType );
			} else if (dwQuality==ITEM_QUALITY_SET-1) {
				wsprintfW(wscTemp, L" [TXT:%d S%d Type:%d]\n",pItem->dwTxtFileNo, d2common_GetItemFileIndex(pItem) ,pItemTxt->nType );
			} else
				wsprintfW(wscTemp, L" [TXT:%d F:%d Type:%d]\n",pItem->dwTxtFileNo, d2common_GetItemFileIndex(pItem) ,pItemTxt->nType );
			wcs_insert(wscName,wscTemp);
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
BOOL PermShowItemsPatch1() {
	//ALT 按键是否生效
	if ( tLockAltToggle.isOn || tPermShowItems.isOn || d2client_CheckUiStatus(UIVAR_SHOWITEMS)) return true ;
	return false;
}
BOOL PermShowItemsPatch2() {
	//是否显示人名，false不显示 true 显示
	if( tLockAltToggle.isOn ) return false;
	if ( d2client_CheckUiStatus(UIVAR_SHOWITEMS) ){
		if ( tPermShowItems.isOn && tPermShowItems.value32==3)return true;
		return false;
	}else{
		return tPermShowItems.isOn?true:false;
	}
}
BOOL PermShowItemsPatch3() {
	//是否显示物品名 true 显示  false 不显示
	if( tLockAltToggle.isOn ) return true;
	if ( d2client_CheckUiStatus(UIVAR_SHOWITEMS) ){
		if ( tPermShowItems.isOn==FALSE) return true ;
		return tPermShowItems.isOn && tPermShowItems.value32==1 ;
	}else{
		return tPermShowItems.isOn?true:false;
	}
}
void __declspec(naked) PermShowItemsPatch1_ASM() {
	__asm {
		call PermShowItemsPatch1
		test eax,eax
		ret
	}
}
void __declspec(naked) PermShowItemsPatch2_ASM() {
	__asm {
		call PermShowItemsPatch2
		test eax,eax
		je orgcode
		ret
orgcode:
		mov eax, dword ptr [esp+0x20]
		test eax,eax
		ret
	}
}
void __declspec(naked) PermShowItemsPatch3_ASM() {
	__asm {
		push ebp
		push esi
		call PermShowItemsPatch3
		test eax,eax
		pop esi
		pop ebp
		jz 	outcode
		cmp ebp, 0x20
		jge outcode
		ret
outcode:
		add dword ptr [esp], 0x38A  //to 6FB0DD89
		ret
	}
}
void __declspec(naked) PermShowItemsPatch4_ASM() {
	__asm {
		push eax
		call PermShowItemsPatch1
		mov ecx, eax
		pop eax
		ret
	}
}

BOOL fViewSockFlag = FALSE;
BOOL fViewSocketableOn = FALSE;
wchar_t wszDescText[0x400] = {L'\0'};
void __fastcall ItemDescPath( wchar_t *wszDesc , int xPos , int yPos , int unk1, int unk2 ) {
	fViewSockFlag = TRUE;
	static int yPosition = 0 ;
	int dwWidth, dwHeight;
	if (fViewSocketableOn) {
		wcs_insert(wszDescText, L"\n\n");
		wcs_insert(wszDescText, wszDesc);
		d2win_GetTextAreaSize(wszDescText, &dwWidth, &dwHeight);
		yPos = yPosition + dwHeight;
	} else {
		wcscpy(wszDescText, wszDesc);
		d2win_GetTextAreaSize(wszDescText, &dwWidth, &dwHeight);
		yPosition = yPos - dwHeight;
	}
	wszDescText[0x3FF] = 0;
	d2win_DrawHoverText(wszDescText , xPos , yPos ,unk1 ,unk2);
	SetExtInfoPos(xPos,yPos,dwWidth,dwHeight);
}
void __declspec(naked) __fastcall SocketViewCallRoutine(UnitAny *pUnitView, DWORD arg, void (__stdcall *func)(UnitAny*, DWORD)) {
	__asm {
		mov eax,0x6650
		push edx
		push ecx
		call [esp+0x0c]
		ret 4
	}
}
UnitAny *pViewingItem=NULL;
void __stdcall ItemSocketViewPatch(void (__stdcall *func)(UnitAny*, DWORD), UnitAny *pUnitView, DWORD arg) {
	fViewSockFlag = FALSE;
	SocketViewCallRoutine(pUnitView, arg, func);
	pViewingItem=*d2client_pCurrentViewItem;
	if ( fViewSockFlag && (tViewSocketable.isOn || tViewSocketBase.isOn) )	{
		fViewSocketableOn = TRUE;
		UnitAny *pUnit = *d2client_pCurrentViewItem;
		if ( pUnit && pUnit->pInventory ) {
			UnitAny *pItem = d2common_GetFirstItemInInv(pUnit->pInventory);
			while (pItem) {
				*d2client_pCurrentViewItem = pItem;
				SocketViewCallRoutine(pUnit, 1, func);
				pItem = d2common_GetNextItemInInv(pItem);
			}
		}
		fViewSocketableOn = FALSE;
		*d2client_pCurrentViewItem = pUnit;
	}
}
void __declspec(naked) ItemSocketViewPatch_ASM() {
	__asm {
		pop eax // exchange return address
		pop ecx
		push eax
		push ecx
		jmp ItemSocketViewPatch
	}
}
/*如果开启查看镶嵌物，只查看其实际生效的属性 */
void __declspec(naked) SocketItemStatViewPatch_ASM() {
	__asm {
		cmp fViewSocketableOn, 0
		jz outcode
		pop esi // discard my return adress
		pop esi // restore esi
		pop ebx // restore ebx
		ret
outcode:
		mov ecx, 0xF9E
		ret
	}
}
/*设置是否查看明细属性*/
StatList *  __stdcall GetItemExtStatListPatch(UnitAny *pUnit, DWORD dwStatNo, DWORD dwListFlag) {
	if ( tViewSocketBase.isOn ) return 0;
	return d2common_GetStatListWithFlag(pUnit,dwStatNo,dwListFlag);
}
/*设置是否查看镶嵌物附带的属性*/
void __declspec(naked) ItemBasicStatPatch_ASM() {
	__asm {
		cmp [tItemBasicStat.isOn], 1
		je quitcode
		cmp [tViewSocketBase.isOn],1
		je quitcode
	//org
		push dword ptr [esp+4]
		call d2common_GetFirstItemInInv
		ret 4
quitcode:
		xor eax, eax
		ret 4
	}
}
//变量范围显示
/*
第一次运行时，先初始化所有绿色，暗金，神符之语的属性表
对于某些变量，需要根据类型拆分属性
short aItemPropInfo[3][500][30][4];
第一位 0 暗金 1 绿色 2 神符之语
第二位 各类型对应编号 set 128个 unique 397个  runeword 169个
第三位 装备属性  共359个属性 , 只保存可变的
第三位 0 记录statno值 1 记录param值 2 记录最小值 3 记录最大值
(3*500*20*4*4)/1024 = 469K ;
对于被击中触发技能，攻击时触发技能，gethit触发技能3类属性，如果等级可变，算法比较麻烦，直接if else维护了
*/
/*
TEST
wchar_t wszTemp[512];
wsprintfW(wszTemp, L"[%d] [%d] [%d] [%d]" , mPropId ,  pItemProp[i].Param , pItemProp[i].MinVal,pItemProp[i].MaxVal);
D2ShowGameMessage(wszTemp,0);
*/

static int aItemPropInfo[3][500][20][4];
static int nStatCount = 0 ;
void SetPropInfo( int itemType , int index ,int propMax , ItemProp *pItemProp){
	if (!pItemProp)return;
	int nCount = 0 ;
	int (*pStat)[4] = &aItemPropInfo[itemType][index][0];
	for ( int i=0 ; i<propMax ; i++){//1
		int mPropId = pItemProp[i].PropId ;
		if ( mPropId>=0 && pItemProp[i].MinVal!= pItemProp[i].MaxVal ){//2
			PropertiesTxt *pPropertiesTxt = d2common_GetPropertiesTxt( mPropId );
			if ( !pPropertiesTxt ) continue;
			short *pStatNo = pPropertiesTxt->wStatNo;
			if ( mPropId==267 && pPropertiesTxt->nFunc[0]==36) continue;
			//特殊属性拆分
			if ( pPropertiesTxt->nFunc[0]==21 ){
				//加角色技能
				pStat[nCount][0] = 83;
				pStat[nCount][1] = pPropertiesTxt->wVal[0];
				pStat[nCount][2] = pItemProp[i].MinVal ;
				pStat[nCount][3] = pItemProp[i].MaxVal ;
				nCount++;
				continue;
			}
			if ( mPropId == 29 ){
				//增强伤害
				pStat[nCount][0] = 17;
				pStat[nCount][1] = pItemProp[i].Param;
				pStat[nCount][2] = pItemProp[i].MinVal;
				pStat[nCount][3] = pItemProp[i].MaxVal;
				nCount++;
				pStat[nCount][0] = 18;
				pStat[nCount][1] = pItemProp[i].Param;
				pStat[nCount][2] = pItemProp[i].MinVal;
				pStat[nCount][3] = pItemProp[i].MaxVal;
				nCount++;
				continue;
			}
			//个别属性，删除
			if ( mPropId>=134 && mPropId<=140)continue;  //单独增加伤害上下限
			if ( mPropId==248 )continue;  //冰火电上下限伤害
			if ( mPropId <= 128 &&  mPropId >=126){
				//被打击时，被击中时，攻击时触发技能
				if (pItemProp[i].MaxVal==0){
					int statno = -1 ;
					int min = -1 ;
					int max = -1;
					if ( mPropId == 126 ){
						if ( index==254 ) {
							statno = 195;
							min = 16;
							max = 20;
						}else if ( index==259 ) {
							statno = 195;
							min = 14;
							max = 20;
						}
					}
					if ( mPropId == 127 ){
						if ( index==136 ) {
							statno = 198;
							min = 13;
							max = 20;
						}
					}
					if ( mPropId == 128 ){
						if ( index==264 ) {
							statno = -1; //因为掉落要求太高，这个实际变成了不可变
							min = 20;
							max = 20;
						}else if ( index==270 ) {
							statno = 201;
							min = 13;
							max = 19;
						}else if ( index==245 ) {
							statno = 201;
							min = 7;
							max = 19;
						}else if ( index==137 ) {
							statno = 201;
							min = 12;
							max = 20;
						}
					}
					if ( statno>=0 ){
						pStat[nCount][0] = statno;
						pStat[nCount][1] = pItemProp[i].Param ;
						pStat[nCount][2] = min ;
						pStat[nCount][3] = max ;
						nCount++;
					}
				}
				continue;
			}
			for( int j=0 ; j< 7; j++ ){//3  子属性拆分
				if ( pStatNo[j]>=0 ){//4
					pStat[nCount][0] = pStatNo[j];
					pStat[nCount][1] = pItemProp[i].Param;
					pStat[nCount][2] = pItemProp[i].MinVal ;
					pStat[nCount][3] = pItemProp[i].MaxVal ;
					nCount++;
				}//4
			}//3
		}//2
	}//1
	if ( nCount> nStatCount) nStatCount = nCount;
}
wchar_t *GetStatRange( wchar_t *wszStr , DWORD dwStatId , DWORD wParam = 0) {
	static BOOL fItemStatInit = FALSE;
	if ( fItemStatInit == FALSE ){
		memset(aItemPropInfo,	-1	,	sizeof(aItemPropInfo));
		nStatCount = 0 ;
		//暗金
		for ( int i =0  ; i < 500 ; i++ ){
			UniqueItemTxt *pUniqueTxt = d2common_GetUniqueItemTxt(i);
			if (!pUniqueTxt)break;
			SetPropInfo( 0 , i , 12 , pUniqueTxt->Prop );
		}
		//绿色
		for ( int i =0  ; i < 200 ; i++ ){
			SetItemTxt *pSetTxt = d2common_GetSetItemTxt(i);
			if (!pSetTxt)break;
			SetPropInfo( 1 , i , 9 , pSetTxt->Prop );
		}
		//神符之语
		for ( int i =0  ; i < 169 ; i++ ){
			RuneWordTxt *pRuneWordTxt = d2common_GetRuneWordTxt(i);
			if (!pRuneWordTxt)break;
			SetPropInfo( 2 , i , 7 ,pRuneWordTxt->Prop );
		}
		fItemStatInit = TRUE;
	}
	UnitAny *pUnit = *d2client_pCurrentViewItem ;
	if (pUnit){
		DWORD dwQuality = pUnit->pItemData->dwQuality;
		int idx = -1;
		int fileindex = -1 ;
		if ( dwQuality==ITEM_QUALITY_UNIQUE ) {
			idx = 0 ;
			fileindex = d2common_GetItemFileIndex(pUnit);
		}else if (  dwQuality==ITEM_QUALITY_SET ){
			idx = 1 ;
			fileindex = d2common_GetItemFileIndex(pUnit);
		}else if ( d2common_CheckItemFlag(pUnit, ITEMFLAG_RUNEWORD, 0, "?") ){
			idx = 2 ;
			fileindex = d2common_GetRuneWordTxtIndex( pUnit->pItemData->wMagicPrefix[0] );
		}
		if ( idx>=0 && fileindex>=0 ){
			int (*pStat)[4] = &aItemPropInfo[idx][fileindex][0];
			for ( int i=0 ; i<nStatCount ; i++){
				if ( pStat[i][0]<0 ) break;
				if ( pStat[i][0]==dwStatId &&  pStat[i][1]==wParam ){
					if (dwStatId==194){
						ItemTxt *pItemTxt = d2common_GetItemTxt(pUnit->dwTxtFileNo);
						if ( pStat[i][3] > (pItemTxt->nSocket)){
							pStat[i][3] = (pItemTxt->nSocket);
						}
					}
					if ( dwStatId==216 ){
						int lvl = d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0);
						wsprintfW(wszStr+wcslen(wszStr), L" \xFF\x63\x34(%d-%d)\xFF\x63\x33",pStat[i][2]*lvl/8 , pStat[i][3]*lvl/8 );
					}else{
						int extvalue = 0 ;
						if(idx==2){
							/*对于神符之语，将符文的属性也加到变量范围内*/
							UnitAny *pItem = d2common_GetFirstItemInInv(pUnit->pInventory);
							while (pItem) {
								extvalue = extvalue + d2common_GetUnitStat(pItem,dwStatId,0);
								pItem = d2common_GetNextItemInInv(pItem);
							}
						}
						wsprintfW(wszStr+wcslen(wszStr), L" \xFF\x63\x34(%d-%d)\xFF\x63\x33",pStat[i][2]+extvalue , pStat[i][3]+extvalue);
					}
					break;
				}
			}
		}
	}
	return wszStr;
}
DWORD __fastcall GetItemPropStringPatch( wchar_t *wszStr , StatEx * pStatEx ,DWORD flag){
	if ( tShowItemVariableProp.isOn && !flag ){
		//ShowGameMessage(pStatEx->wParam);
		DWORD dwStatId = pStatEx->wStatId;
		DWORD dwParam = pStatEx->wParam ;
		switch( dwStatId ){
			case 195:
			case 198:
			case 201:
				dwParam = dwParam>>6;
				break;
			case 188:
				{
					int chartype = dwParam/8;
					int skilltab = dwParam%8;
					dwParam = chartype*3+skilltab;
				}
				break;
			default:
				break;
		}
		GetStatRange( wszStr , dwStatId , dwParam );
	}
	return pStatEx->wStatId;
}
void __declspec(naked) GetItemPropStringPatch_ASM() {
	// esp +4 +8*ebp+2D4 = pStatEx
	// ebx = dwStatNo
	// esi = wszPropStr
	__asm {
		lea edx , [esp+4+8*ebp+0x2D4]
		mov ecx , esi
		push [esp+0x10]
		call GetItemPropStringPatch
//org , eax from ebx
		sub eax, 0x17
		ret
	}
}
void __fastcall StrcatItemPropStringPatch( wchar_t *wszMainStr  , wchar_t *wszPropStr ,DWORD dwStatNo ){
	int len = wcslen(wszPropStr);
	if ( len == 0 ) return;
	wszPropStr [ len-1] = L'\0';//去掉回车
	if (tShowItemVariableProp.isOn&&tViewSocketBase.isOn==0) {
		GetStatRange( wszPropStr , dwStatNo );
	}
	wcscat( wszMainStr , wszPropStr );
	wcscat( wszMainStr , L"\n");
}
void __declspec(naked) StrcatItemPropStringPatch_ASM() {
	__asm {
		push [esp+0xC]
		call StrcatItemPropStringPatch
		ret
	}
}
wchar_t * __fastcall StrcatSocketStringPatch( wchar_t *wszStr ) {
	//拼孔数字符串的地方,下一步代码拼接回车
	if ( tShowItemVariableProp.isOn ){
		GetStatRange( wszStr , 194 );
	}
	return wszStr;
}
void __declspec(naked) StrcatSocketStringPatch_ASM() {
	__asm {
		push edi
		mov ecx , eax
		call StrcatSocketStringPatch
		pop edi
		mov edx, dword ptr [esp+0x14]
		mov ecx, eax
		ret
	}
}
wchar_t * __fastcall StrcatDefenseStringPatch( wchar_t *wszStr ) {
	//拼防御字符串的地方,下一步代码拼接回车
	//对于ETH物品 Bug打孔，如果bug出来比正常的非bug最大值还小，则认为是非bug
	if ( tShowItemVariableProp.isOn ){
		UnitAny *pUnit = *d2client_pCurrentViewItem ;
		if (pUnit){
			ItemTxt *pItemTxt = d2common_GetItemTxt( pUnit->dwTxtFileNo );
			if( pItemTxt && pItemTxt->dwMinDef>0 ){
				int mDef = d2common_GetUnitBaseStat(pUnit, 31,0);
				int mMinDef = pItemTxt->dwMinDef;
				int mMaxDef = pItemTxt->dwMaxDef ;
				wchar_t wszPrep[256] = {L"\0"};
				wchar_t wszTemp[512] = {L"\0"};
				if ( d2common_CheckItemFlag(pUnit, ITEMFLAG_ETHEREAL, 0, "?") ){
					mMinDef = (int)(mMinDef * 1.5) ;
					mMaxDef = (int)(mMaxDef * 1.5) ;
					if ( mDef>mMaxDef && pUnit->pItemData->dwQuality==2 && d2common_CheckItemFlag(pUnit, ITEMFLAG_SOCKETED, 0, "?")  ){
						//普通物品 ,带孔，防御比最大防御还大，则为ETH BUG
						mMinDef = (int)(mMinDef * 1.5) ;
						mMaxDef = (int)(mMaxDef * 1.5) ;
						wcscpy(wszPrep , L"(Bug)");
					}
				}
				if ( mDef > mMaxDef ){//超过最大防御，说明天生带ED，防御自动max+1
					wsprintfW( wszTemp, L"%s(Min: %d, Max: %d, Now: %d+%d)\n" ,wszPrep, mMinDef , mMaxDef , mMaxDef ,mDef - mMaxDef );
				}else{
					wsprintfW( wszTemp, L"%s(Min: %d, Max: %d, Now: %d)\n" ,wszPrep, mMinDef , mMaxDef , mDef);
				}
				wcs_insert(wszStr,  wszTemp);
			}
		}
	}
	return wszStr;
}
void __declspec(naked) StrcatDefenseStringPatch_ASM() {
	__asm {
		push ebx
		mov ecx , edi
		call StrcatDefenseStringPatch
		pop ebx
		mov edx, dword ptr [esp+0x1C]
		mov ecx, eax
		ret
	}
}
wchar_t * __fastcall StrcatDamageStringPatch(wchar_t *wszStr) {
	//拼攻击字符串，直接拼接(盾牌也会调用)
	if (tShowItemVariableProp.isOn ){
		UnitAny *pUnit = *d2client_pCurrentViewItem ;
		if (pUnit) {
			ItemTxt *pItemTxt=d2common_GetItemTxt(pUnit->dwTxtFileNo);
			if(pItemTxt&&pUnit->dwTxtFileNo<*d2common_pWeaponsTxts+*d2common_pArmorTxts){
				wchar_t wszTemp[512] = {L"\0"};
				int Sockets = pItemTxt->nSocket;
				if (pUnit->pItemData->dwQuality<=3) {
					int maxSockets = 0;
					int iLevel = pUnit->pItemData->dwItemLevel;
					ItemTypeTxt *pItemTypeTxt = d2common_GetItemTypeTxt(pItemTxt->nType);
					if (iLevel<=25) {
						maxSockets = pItemTypeTxt->nMaxSock1;
					} else if (iLevel<=40) {
						maxSockets = pItemTypeTxt->nMaxSock25;
					} else {
						maxSockets = pItemTypeTxt->nMaxSock40;
					}
					if (Sockets>maxSockets)Sockets = maxSockets;
					wszTemp[0]=0xff;wszTemp[1]='c';wszTemp[2]='0';
					wsprintfW(wszTemp+3,L"(Range: %d, Speed: %d, Sockets: 1-%d)\n" ,pItemTxt->nRangeAdder+1, pItemTxt->dwSpeed , Sockets);
				} else {
					wszTemp[0]=0xff;wszTemp[1]='c';wszTemp[2]='0';
					wsprintfW(wszTemp+3,L"(Range: %d, Speed: %d)\n" ,pItemTxt->nRangeAdder+1, pItemTxt->dwSpeed);
				}
				wcs_insert(wszStr,  wszTemp);
			}
		}
	}
	return wszStr;
}
//一般物品调用
void __declspec(naked) StrcatDamageStringPatch_ASM1() {
	__asm {
		push ebx
		mov ecx , edi
		call StrcatDamageStringPatch
		pop ebx
		mov edx, eax
		lea ecx, dword ptr [esp+0x64]
		ret
	}
}
//绿色武器调用
void __declspec(naked) StrcatDamageStringPatch_ASM2() {
	__asm {
		push ebp
		mov ecx , edi
		call StrcatDamageStringPatch
		pop ebp
		mov edx, eax
		lea ecx, dword ptr [esp+0x194C]
		ret
	}
}
void ShowSocketProtectInfo(){
	char buf[256];formatKey(buf,tSocketProtect.key);
	wchar_t wszbuf[256];
	wsprintfW(wszbuf, L"<Hackmap>: Socket Protect On, press %hs to disable",buf);
	partyMessageWColor(8,wszbuf);
}
void __declspec(naked) SocketProtectPatch_ASM() {
	__asm {
		mov eax, d2client_pPlayerUnit
		mov eax, [eax]
		push 0x13
		push eax
		call d2client_ItemProtect
		call ShowSocketProtectInfo
		ret
	}
}
int __fastcall isValidRuneword(UnitAny *pItem);
/*
	d2client_99148: call d2client_C262->d2common_1E670 int __stdcall d2common_getEquippedItemInfo(UnitInventory * pInventory,int bodyLoc)//1<=bodyLoc<=12(2 args) ; esp-48
	d2client_9914D: mov ecx, ebx         ; esp-40
	d2client_9914F: mov [esp+0x18 (ret_10h)], eax <--- equipped item to socket
	d2client_99153: call d2client_8C2B0(0 args)
	d2client_99158: test eax, eax
	d2client_9915A: jz d2client_99216 ->+182 B99216
	d2client_99160: mov eax, ebx
	d2client_99162: call d2client_81CB0(0 args)
	d2client_99167: test eax, eax
	d2client_99169: jnz d2client_991C8 ->+93 B991C8
	d2client_9916B: call d2client_923F0(0 args)
	d2client_99170: test eax, eax
	d2client_99172: jz d2client_991C8 ->+84 B991C8
//d2client_99174: call d2hackmap_SocketProtectPatch1_ASM <--- patch here
//d2client_99174: 8B 44 24 18        mov eax, [esp+0x18] <--- equipped item to socket
//d2client_99178: 85 C0              test eax, eax
*/
void __declspec(naked) SocketProtectPatch1_ASM() {
	__asm {
		cmp [tRunewordProtect.isOn], 0
		jz not_protect
		mov eax, [esp+4+0x18] 
		pushad
		mov ecx, eax
		call isValidRuneword
		cmp eax,0
		popad
		jnz not_protect
		add dword ptr [esp], 0x47 
		ret
not_protect:
		cmp [tSocketProtect.isOn], 0
		jz original_code
		call SocketProtectPatch_ASM
		add dword ptr [esp], 0x47 
		ret
original_code:
		mov eax, [esp+4+0x18] 
		test eax, eax
		ret
	}
}
//d2client_96FAB: call d2hackmap1_22C60(0 args)
//d2client_96FAB: 8B 44 24 44        mov eax, [esp+0x44] <--- equipment to put in
//d2client_96FAF: 85 C0              test eax, eax
void __declspec(naked) SocketProtectPatch2_ASM() {
	__asm {
		cmp [tRunewordProtect.isOn], 0
		jz not_protect
		mov eax, [esp+4+0x44] // 4 bytes for return address
		pushad
		mov ecx,eax
		call isValidRuneword
		cmp eax,0
		popad
		jnz not_protect
		add dword ptr [esp], 0x36
		ret
not_protect:
		cmp [tSocketProtect.isOn], 0
		jz original_code
		call SocketProtectPatch_ASM
		add dword ptr [esp], 0x36
		ret
original_code:
		mov eax, [esp+4+0x44] // 4 bytes for return address
		test eax, eax
		ret
	}
}
int searchStat(UnitAny *pItem,int statId,int *pParam) {
	if (!pItem->pStatList) return 0;
	StatList *plist=pItem->pStatList;
	if (!(plist->dwListFlag&0x80000000)) return 0;
	if (!plist->sFullStat.pStats) return 0;
	Stat *stat=&plist->sFullStat;
	int n=stat->wStats;
	if (n>=511) return 0;
	StatEx *se=stat->pStats;
	for (int i=0;i<n;i++) {
		int id=se[i].wStatId;if (id!=statId) continue;
		*pParam=se[i].wParam&0xFFFF;
		return se[i].dwStatValue;
	}
	return 0;
}
static char *clsNames[2][8]={
	{"ama","sor","nec","pal","bar","dru","asn","???"},
	{"亚马逊","法师","死灵","圣骑士","野蛮人","德鲁依","刺客","???"}};
extern char *runeWordNames[];
extern char *uniqueNames[];
extern char *setItemNames[];
int cpLocaleName(wchar_t *dst,wchar_t *s,int max);
void drawSimpleItemStackInfo(UnitAny *pUnit,int xpos,int ypos);
int marketItemCount=0,marketItemCount1=0;
void drawInvItemInfo(UnitAny *pItem,int px,int py) {
	wchar_t wbuf[128];int lines[8];
	int index = GetItemIndex(pItem->dwTxtFileNo)+1;
	if (index==simpleItemStackIdx) {
		if (tDrawSimpleItemStackCount.isOn) drawSimpleItemStackInfo(pItem,px,py);
		return;
	}
	ItemTxt *pItemTxt=d2common_GetItemTxt(pItem->dwTxtFileNo);
	int w=pItemTxt->nInvwidth;
	int maxNameLen=2*w;
	int dwQuality=pItem->pItemData->dwQuality;
	int pos=0,color=0,color1=0,ln=0;
	lines[ln++]=pos;
	int dy=0,centerY=1;
	if (pItem->pItemData->nLocation==0) {
		if (pItem==PLAYER->pInventory->pCursorItem) dy=dwDrawCursorItemInfoDy;
		else {
			dy=dwDrawMarketItemInfoDy;centerY=0;marketItemCount1++;
		}
		maxNameLen=6;
	}
	if (dwQuality>=ITEM_QUALITY_MAGIC) {
		if (!d2common_CheckItemFlag(pItem,ITEMFLAG_IDENTIFIED,0,"?")) {
			if (pItem->pItemData->nLocation==1||!showUnidentifiedItemInfo||pItem==PLAYER->pInventory->pCursorItem) {
				switch (dwQuality) {
					case ITEM_QUALITY_MAGIC:color=3;break;
					case ITEM_QUALITY_UNIQUE:color=7;break;
					case ITEM_QUALITY_SET:color=2;break;
					case ITEM_QUALITY_RARE:color=9;break;
					case ITEM_QUALITY_CRAFTED:color=9;break;
					case ITEM_QUALITY_TAMPERED:color=9;break;
				}
				wchar_t *wName=d2lang_GetLocaleText(pItemTxt->wLocaleTxtNo);
				pos+=cpLocaleName(wbuf+pos,wName,maxNameLen);
				wbuf[pos++]=0;lines[ln++]=pos;color1=1;
				wbuf[pos++]='?';
				goto done;
			}
		}
	}
	int showClassSkill=0,showResist=0,showAttr=0,showFcr=0,showMF=0;
	int showLL=0,showSTR=0,showDEX=0,showQuautity=0,checkStat=0,showDiff=0;
	switch (dwQuality) {
		case ITEM_QUALITY_UNIQUE: {
			color=7;
			int fileindex = d2common_GetItemFileIndex(pItem);
			UniqueItemTxt *txt = d2common_GetUniqueItemTxt(fileindex);if (!txt) break;
			if (txt) {
				if (IsBadReadPtr(txt,sizeof(UniqueItemTxt))) return;
				wchar_t *s=d2lang_GetLocaleText(txt->wLocaleTxtNo);if (!s) break;
				pos+=cpLocaleName(wbuf+pos,s,maxNameLen);
			}
			switch (fileindex) {
				case 123: //U123蝮蛇護符
				case 124: //U124國王之杖
				case 125: //U125赫拉迪克法杖
				case 127: //U127克林姆的連枷
					showDiff=1;
					break;
				case 101: //囚房 The Ward
				case 210: //蛇魔法师之皮 Skin of the Vipermagi
				case 272: //马拉的万花筒 Mara's Kaleidoscope
				case 357: //奇拉的守护 Kira's Guardian
					showResist=1;
					break;
				case 104: //运气守护 Chance Guards
				case 120: //拿各的戒指 Nagelring
				case 240: //战争旅者 War Traveler
					showMF=1;
					break;
				case 281: //泰坦的复仇 Titan's Revenge
				case 326: //死神的丧钟 The Reaper's Toll
				case 345: //安达利尔的面貌 Andariel's Visage
					showLL=1;
					break;
				case 364: //卓古拉之握 Dracul's Grasp
					showLL=1;showSTR=1;
					break;
				case 275: //乌鸦之霜 Raven Frost
					showDEX=1;
					break;
				case 400:{ //Helltorch
					showAttr=1;
					showResist=1;
					showClassSkill=1;
					break;
				default:
					if (392<=fileindex&&fileindex<=399) {
						static char *names[]={"死电","死冰","死火","死毒","活电","活冰","活火","活毒",};
						wbuf[pos++]=0;lines[ln++]=pos;
						pos+=wsprintfW(wbuf+pos,L"%hs\n",names[fileindex-392]);
					}
				}
			}
			break;
		}
		case ITEM_QUALITY_SET: {
			color=2;
			int fileindex = d2common_GetItemFileIndex(pItem);
			SetItemTxt *txt = d2common_GetSetItemTxt(fileindex);if (!txt) break;
			if (txt) {
				wchar_t *s=d2lang_GetLocaleText(txt->wLocaleTxtNo);
				pos+=cpLocaleName(wbuf+pos,s,maxNameLen);
			}
			break;
		}
		default:
			if (d2common_CheckItemFlag(pItem, ITEMFLAG_RUNEWORD, 0, "?")) {
				color=7;
				int fileindex = d2common_GetRuneWordTxtIndex( pItem->pItemData->wMagicPrefix[0] );
				RuneWordTxt *txt = d2common_GetRuneWordTxt(fileindex);
				if (txt) {
					wchar_t *s=d2lang_GetLocaleText(txt->wLocaleTxtNo);
					pos+=cpLocaleName(wbuf+pos,s,maxNameLen);
				}
				switch (fileindex) {
					case 37: //橡树之心 Heart of the Oak
						showResist=1;
						break;
					case 49: //精神 Spirit
						showFcr=1;
						break;
				}
			} else {
				int idx=GetItemIndex(pItem->dwTxtFileNo)+1;
				wchar_t *name=NULL;
				switch (idx) {
					case 89: //Wirt's Leg
					case 174: //Khalim's Flail
					case 175: //Khalim's Will
					case 2038: //Potion of Life
					case 2138: //Scroll of Knowledge
					case 2139: //Scroll of Resistance
					case 2039: //A Jade Figurine
					case 2040: //The Golden Bird
					case 2041: //Lam Esen's Tome
					case 2044: //Mephisto's Soulstone
					case 2045: //Book of Skill
					case 2046: //Khalim's Eye
					case 2047: //Khalim's Heart
					case 2048: //Khalim's Brain
						showDiff=1;
						break;
					case 48:showQuautity=1;break;
					case 2140:name=L"A1";break;
					case 2141:name=L"A2";break;
					case 2142:name=L"A5";break;
					case 2147:name=L"A1";break;
					case 2148:name=L"A3";break;
					case 2149:name=L"A4";break;
					case 2150:name=L"A5";break;
					case 2019:case 2021:case 2036:case 2011:case 2012:showQuautity=1;break;
					case 2096:case 2097:case 2098: //charm
					case 2013:case 2015: //ring necklace
					case 2136: //jewel
						checkStat=1;
						break;
					default:
						if (2103<=idx&&idx<=2135) {
							color=0;dy=-8;
							pos+=wsprintfW(wbuf+pos,L"R%d",idx-2102);
						}
						switch (pItemTxt->nType) {
							case 37://Helm头盔
							case 75://Circlet头环
								checkStat=1;
								break;
						}
						break;
				}
				if (name) {
					pos+=wsprintfW(wbuf+pos,L"%s",name);
				}
			}
			break;
	}
	if (checkStat) {
		if (pItem->pStatList) {
			StatList *plist=pItem->pStatList;
			if (plist->dwListFlag&0x80000000) {
				if (plist->sFullStat.pStats) {
					Stat *stat=&plist->sFullStat;
					int n=stat->wStats;
					StatEx *se=stat->pStats;
					if (n<10) {
						for (int i=0;i<n;i++) {
							if (ln>3) break;
							int id=se[i].wStatId;
							int value=se[i].dwStatValue;
							switch (id) {
								case STAT_STRENGTH:showSTR=1;break;
								case STAT_DEXTERITY:showDEX=1;break;
								case STAT_FIRE_RESIST:
									if (n-i>=4&&se[i+1].wStatId==STAT_LIGHTING_RESIST
										&&se[i+2].wStatId==STAT_COLD_RESIST
										&&se[i+3].wStatId==STAT_POSION_RESIST
										&&value==se[i+1].dwStatValue
										&&value==se[i+2].dwStatValue
										&&value==se[i+3].dwStatValue) {
										showResist=1;
										i+=3;
									} else {
										if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
										pos+=wsprintfW(wbuf+pos,L"%dFR",value);
									}
									break;
								case STAT_LIGHTING_RESIST:
									if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
									pos+=wsprintfW(wbuf+pos,L"%dLR",value);
									break;
								case STAT_COLD_RESIST:
									if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
									pos+=wsprintfW(wbuf+pos,L"%dCR",value);
									break;
								case STAT_POSION_RESIST:
									if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
									pos+=wsprintfW(wbuf+pos,L"%dPR",value);
									break;
								case STAT_MAGIC_FIND:
									showMF=1;
									break;
								case STAT_CLASS_ONLY_SKILL: {
									static wchar_t *clsSkills[7][3]={
										{L"弓",L"被",L"标"},
										{L"火",L"电",L"冰"},
										{L"诅",L"毒",L"招"},
										{L"战",L"攻",L"防"},
										{L"斗",L"专",L"嚎"},
										{L"唤",L"形",L"素"},
										{L"陷",L"影",L"武"},
									};
									int param=se[i].wParam;
									if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
									pos+=wsprintfW(wbuf+pos,L"+%d%s",value,clsSkills[param>>3][param&3]);
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	if (showClassSkill) {
		int type;
		int cls=searchStat(pItem,STAT_CLASS_SKILL,&type);
		if (cls) {
			wbuf[pos++]=0;lines[ln++]=pos;
			pos+=wsprintfW(wbuf+pos,L"%hs",clsNames[dwGameLng][type]);
		}
	}
	if (showAttr) {
		int attr=d2common_GetUnitStat(pItem, STAT_STRENGTH, 0);
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%d属",attr);
	}
	if (showResist) {
		int resist=d2common_GetUnitStat(pItem, STAT_FIRE_RESIST, 0);
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%d抗\n",resist);
	}
	if (showFcr) {
		int fcr = d2common_GetUnitStat(pItem, STAT_FCR, 0);
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%dFCR",fcr);
	}
	if (showMF) {
		int fcr = d2common_GetUnitStat(pItem,STAT_MAGIC_FIND,0);
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%dMF",fcr);
	}
	if (showLL) {
		int ll = d2common_GetUnitStat(pItem,STAT_LIFE_LEECH,0);
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%dLL",ll);
	}
	if (showSTR) {
		int ed = d2common_GetUnitStat(pItem,STAT_STRENGTH,0);
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%d力",ed);
	}
	if (showDEX) {
		int ed = d2common_GetUnitStat(pItem,STAT_DEXTERITY,0);
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%d敏",ed);
	}
	if (showQuautity) {
		int value = d2common_GetUnitStat(pItem,STAT_AMMOQUANTITY,0);
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%d",value);
	}
	if (showDiff) {
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		int diff=d2common_GetUnitStat(pItem,STAT_ITEMDIFFICULTY,0);
		if (dwGameLng) {
			switch (diff) {
				case 0:wbuf[pos++]=L'普';wbuf[pos++]=L'通';break;
				case 1:wbuf[pos++]=L'噩';wbuf[pos++]=L'梦';break;
				case 2:wbuf[pos++]=L'地';wbuf[pos++]=L'狱';break;
			}
		} else {
			pos+=wsprintfW(wbuf+pos,L"D%d",diff);
		}
	}
	if (0) {
		if (pos) {wbuf[pos++]=0;lines[ln++]=pos;}
		pos+=wsprintfW(wbuf+pos,L"%d,%d",pItem->pItemData->nLocation,pItem->pItemData->nItemLocation);
	}
done:
	if (!pos) return;
	wbuf[pos]=0;
	int x=px+((pItemTxt->nInvwidth*29)>>1);
	int y=py-((pItemTxt->nInvheight*29)>>1)+dy;
	if (centerY) y+=12-ln*6;
	for (int i=0;i<ln;i++) {
		if (i==0) {
			DrawCenterText(8,wbuf+lines[i],x+1,y+1,6,1,0);
			DrawCenterText(8,wbuf+lines[i],x,y,color,1,0);
		} else if (i==1) {
			DrawCenterText(8,wbuf+lines[i],x+1,y+1,6,1,0);
			DrawCenterText(8,wbuf+lines[i],x,y,color1,1,0);
		} else {
			DrawCenterText(8,wbuf+lines[i],x+1,y+1,6,1,0);
			DrawCenterText(8,wbuf+lines[i],x,y,0,1,0);
		}
		y+=12;
	}
}
/*
	d2client_6B6B2: E8 DF 19 FA FF     call d2client_D096->d2gfx_B080 void __stdcall d2gfx_DrawCellFile(CellContext *context, int xPos, int yPos, DWORD dw1, int dwTransLvl, BYTE *coltab)(6 args)
	*/
struct DrawCellFileArgs {
	CellContext *context;
	int x,y,dw1,dwTransLvl;
	BYTE *coltab;
};
int __fastcall drawInvItemPatch(DrawCellFileArgs *args,UnitAny *pItem) {
	int ret=d2gfx_DrawCellFile(args->context,args->x,args->y,args->dw1,args->dwTransLvl,args->coltab);
	drawInvItemInfo(pItem,args->x,args->y);
	return ret;
}
void __declspec(naked) DrawInvItemPatch_ASM() {
	__asm {
		cmp [tDrawInvItemInfo.isOn], 0
		jz original_code
		pushad
		lea ecx, [esp+0x24]
		mov edx, ebx
		call drawInvItemPatch
		popad
		ret 0x18
original_code:
		jmp d2gfx_DrawCellFile
	}
}
void __fastcall activeBufferItem(int id,int x,int y) {
	UnitAny *pItem=d2client_GetUnitFromId(id,UNITNO_ITEM);if (!pItem) return;
	int index=GetItemIndex(pItem->dwTxtFileNo)+1;
	if (index==simpleItemStackIdx) {
		if (GetKeyState(VK_SHIFT)&0x8000) {
			int x=PLAYER->pMonPath->wUnitX;
			int y=PLAYER->pMonPath->wUnitY;
			BYTE packet[13];
			packet[0]=0x20;*(DWORD*)&packet[1]=id;
			*(DWORD*)&packet[5]=x;
			*(DWORD*)&packet[9]=y;
			for (int i=1;i<3;i++) SendPacket(packet,13);
		}
		return;
	} else if (index==2012) {
		int x=PLAYER->pMonPath->wUnitX;
		int y=PLAYER->pMonPath->wUnitY;
		//LOG("activeIdentify %d (%d,%d)\n",id,x,y);
		//LOG("player (%d,%d)\n",x,y);
	}
}
/*
	d2client_98D26: 8B 4E 0C           mov ecx, [esi+0xC]
	d2client_98D29: 57                 push edi
	d2client_98D2A: 8B D3              mov edx, ebx
	d2client_98D2C: B0 20              mov al, 0x20 (32) (' ')
	d2client_98D2E: E8 AD BB F7 FF     call d2client_148E0 void __fastcall d2client_sendPacketLen13(int arg1,int arg2,int arg3)//eax:char cmd(1 args)
*/
void __declspec(naked) ActiveBufferItemPatch_ASM() {
	__asm {
		pushad
		mov eax, [esp+0x24]
		push eax
		call activeBufferItem
		popad
		jmp d2client_sendPacketLen13
	}
}
