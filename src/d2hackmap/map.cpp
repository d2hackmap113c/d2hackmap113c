#include "stdafx.h"
#include "header.h"
#include "map.h"

int AddRectToMinimap(AreaRectInfo *pAreaRectInfo,int revealed);
int getUnitOwnerId(UnitAny *pUnit);
BYTE GetItemColour(UnitAny *pItem,int arridx);
void AutoMapRoute();
void AutoRouteDrawMinimapPath();
int PrevMapTarget();
int NextMapTarget();
int isWaypointTxt(int txt);

#define MINIMAP_TYPE (*d2client_pMinimapType)
static int afRevealedActs[5];
MinimapLevel minimapLevels[140]={0};
MinimapLevel *pCurMapLevel=NULL;
#define CELLNO_MYSHRINES 1176
#define CELLNO_WAYPOINT 307
#define CELLNO_SHRINE 310
#define NUMOF_SHRINES 23
static CellFile* pUnitBlobCells[11]={0};
static CellFile* apMyShrineCells[2][NUMOF_SHRINES]={0};
static CellFile* pWaypointCell=0;
CellFile* pMinimapObjectCell=0;
static int minimapTargetId=3000;

int dwRevealMapMs=0;
int dwAutoRevealMode=1;
int dwAutoRevealCLevel=0;
ToggleVar tRevealAct={TOGGLEVAR_DOWN,0,-1,1,"RevealAutomap",&RevealCurAct};
ToggleVar tRevealLevel={TOGGLEVAR_DOWN,0,-1,1,"RevealLevel",&RevealLevelPlayerIn};
ToggleVar tPrevMapTarget={TOGGLEVAR_DOWN,0,-1,1,"PrevMapTarget",&PrevMapTarget};
ToggleVar tNextMapTarget={TOGGLEVAR_DOWN,0,-1,1,"NextMapTarget",&NextMapTarget};
ToggleVar tAutomapLevelNames={TOGGLEVAR_ONOFF,0,-1,1,"Automap Level Names Toggle" };
ToggleVar tAutoMap={TOGGLEVAR_ONOFF,1,-1,1,"Auto Map Toggle"};
BYTE anMonsterColours[1000]={0};
ToggleVar tMiniShrine={TOGGLEVAR_ONOFF,0,-1,1,"Mini Shrine Toggle" };
wchar_t wszStaffTombLvlDesc[50]={L'\0'};
BYTE nCaveNameTextCol=0;
ToggleVar tAutomapActiveRect={TOGGLEVAR_ONOFF,0,-1 ,1,"AutomapActiveRect" };
ToggleVar tAutomapReadyRect={TOGGLEVAR_ONOFF,0,-1 ,1,"AutomapReadyRect"};
ToggleVar tAutomapScreenArea={TOGGLEVAR_ONOFF,0,-1 ,1,"AutomapScreenArea"};
BYTE nAutomapActiveRectColors=0x16;
BYTE nAutomapReadyRectColors=7;
BYTE nAutomapScreenColors=0x16;
BYTE nMinimapSize=1;
BYTE routeLineColor=0x84;
ToggleVar tMinimapDirectLineToggle={TOGGLEVAR_ONOFF,0,-1,1,"MinimapDirectLine"};
ToggleVar tMinimapDebugToggle={TOGGLEVAR_ONOFF,0,-1,1,"MinimapDebug"};
ToggleVar tAutomapCorpse={TOGGLEVAR_ONOFF,0,-1,1,"AutomapCorpse"};
ToggleVar tAutomapMonsters={TOGGLEVAR_ONOFF,0,-1,1,"AutomapMonsters"};
ToggleVar tAutomapChests={TOGGLEVAR_ONOFF,0,-1,1,"Chests"};
ToggleVar tAutomapMissiles={TOGGLEVAR_ONOFF,0,-1,1,"Missiles"};
ToggleVar tAutomapItems={TOGGLEVAR_ONOFF,0,-1,1,"Show automap items"};
BYTE fAutomapPartyDefault=1;
BYTE fAutomapNamesDefault=1;
BYTE nHostileMissileColor=0x62;
BYTE nGuidedMissileColor=0x5B;
BYTE nTracerMissileColor=0xFF;
BYTE nOtherMissileColor=0xFF;
BYTE nClosedChestColor=9;
BYTE nLockedChestColor=9;
BYTE nNormalCorpseColor=8;
BYTE nBossCorpseColor=8;
BYTE nMinionCorpseColor=8;
BYTE nChampionCorpseColor=8;
BYTE nMonsterNormalColor=0x62;
BYTE nMonsterBossColor=0x5b;
BYTE nMonsterMinionColor=0x5b;
BYTE nMonsterChampColor=0x5b;
BYTE nSuperUniqueColor=(BYTE)-1;
BYTE nNeutralPlayerColor=0x62,nHostilePlayerColor=0x5b;
BYTE nFriendMinionColor=0x7f;
BYTE nPlayerMinionColor=0xcb;
BYTE nPartyTextColour=2;
BYTE nNeutralTextColour=9;
BYTE nHostileTextColour=1;
BYTE nMonsterTextCol=1;
char aszUnitBlobFiles[11][50]={0};
wchar_t awszEnchantDescs[50][20]={0};
wchar_t awszAuraDescs[7][20]={0};
wchar_t awszImmuneDescs[6][20]={0};
BYTE anMonsterCorpseColors[1000]={0};
BYTE anMissileColours[1000]={0};
ToggleVar tBossNameToggle={TOGGLEVAR_ONOFF,0,-1,1 ,"BossNameToggle"};
BYTE nSuperUniqueDescFont=0;
ToggleVar tScreenshake={TOGGLEVAR_ONOFF,0,-1,1,"Screenshake"};
void SetScrollMapPos(char xinc, char yinc);
int MiniMapScroll(int id);
void InitScrollLock(BOOL fInit);
int CenterMap();
void DrawScrollOffset(wchar_t*wbuf ,int posy);
int ScrollScreenStart( int idx );
int ScrollScreenEnd( int idx );
ToggleVar 	tCenterMap={			  TOGGLEVAR_DOWN,	0,	-1,		1,	"CenterMap",		&CenterMap};
int nScrollMode=3,dwScreenScrollCount=0,dwMinimapScrollCount=0;
ToggleVar 	tScreenScrollKeys[16]	={ 	   {0}};
int 			dwScreenScrollOffset[16][2]={ {0}};
ToggleVar tMiniMapScrollKeys[16]={ 0};
int 			dwMiniMapScrollOffset[16][2]={ 0};
int dwDuranceOfHateLevel3TeleportShift[3]={-20,0,3};
int dwDuranceOfHateLevel3TeleportShiftBugKM[3]={-20,0,3};
static int dwTravincalShift[2];
static int dwWayPointRandom;
static int dwRescueBarIncompleteShift[2],dwRescueBarCompleteShift[2];
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY, "RevealActAutomapKey",      &tRevealAct       },
	{CONFIG_VAR_TYPE_KEY, "RevealLevelAutomapKey",    &tRevealLevel     },
	{CONFIG_VAR_TYPE_KEY, "PrevMapTarget",    &tPrevMapTarget     },
	{CONFIG_VAR_TYPE_KEY, "NextMapTarget",    &tNextMapTarget     },
	{CONFIG_VAR_TYPE_INT,"RouteLineColor",&routeLineColor,1}, 
	{CONFIG_VAR_TYPE_KEY, "AutoMapToggle",            &tAutoMap         },
	{CONFIG_VAR_TYPE_INT, "AutoRevealMode",            &dwAutoRevealMode   ,  4 },
	{CONFIG_VAR_TYPE_INT, "AutoRevealCharacterLevel",            &dwAutoRevealCLevel   ,  4 },
	{CONFIG_VAR_TYPE_KEY, "AutomapLevelNamesToggle",  &tAutomapLevelNames		},
	{CONFIG_VAR_TYPE_KEY, "MiniShrinesToggle",        &tMiniShrine			},
	{CONFIG_VAR_TYPE_INT, "LevelNameColour",          &nCaveNameTextCol , 1	},
	{CONFIG_VAR_TYPE_D2STR, "StaffTombLevelDesc",       &wszStaffTombLvlDesc, 1,  {50 }},
  {CONFIG_VAR_TYPE_KEY, "AutomapActiveRectsToggle",		&tAutomapActiveRect     },
  {CONFIG_VAR_TYPE_KEY, "AutomapReadyRectsToggle",		&tAutomapReadyRect      },
  {CONFIG_VAR_TYPE_KEY, "AutomapScreenAreaToggle",		&tAutomapScreenArea     },
  {CONFIG_VAR_TYPE_INT, "AutomapActiveRectsColour",		&nAutomapActiveRectColors,  1 },
  {CONFIG_VAR_TYPE_INT, "AutomapReadyRectsColour",		&nAutomapReadyRectColors  , 1 },
  {CONFIG_VAR_TYPE_INT, "AutomapScreenAreaColour",		&nAutomapScreenColors , 1 },
	{CONFIG_VAR_TYPE_LEVEL_TARGET, "MapTargets",             0			},
	{CONFIG_VAR_TYPE_KEY, "MinimapDirectLineToggle",            &tMinimapDirectLineToggle			},
	{CONFIG_VAR_TYPE_KEY, "MinimapDebugToggle",            &tMinimapDebugToggle			},
	{CONFIG_VAR_TYPE_INT, "MinimapSize",              &nMinimapSize,      1},
	{CONFIG_VAR_TYPE_INT, "AutomapPartyDefault",      &fAutomapPartyDefault , 1 },
	{CONFIG_VAR_TYPE_INT, "AutomapNamesDefault",      &fAutomapNamesDefault , 1 },
	{CONFIG_VAR_TYPE_KEY, "BossNameToggle",			&tBossNameToggle    },
	{CONFIG_VAR_TYPE_STR, "UnitBlobFiles",            &aszUnitBlobFiles ,   5,  {50}},
	{CONFIG_VAR_TYPE_STR, "PlayerBlobFile",           &aszUnitBlobFiles[0], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "MonsterBlobFile",          &aszUnitBlobFiles[1], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "DimVisionBlobFile",          &aszUnitBlobFiles[10], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "ObjectBlobFile",           &aszUnitBlobFiles[2], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "MissileBlobFile",          &aszUnitBlobFiles[3], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "ItemBlobFile",             &aszUnitBlobFiles[4], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "BossBlobFile",             &aszUnitBlobFiles[6], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "NpcBlobFile",              &aszUnitBlobFiles[7], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "MyBlobFile",               &aszUnitBlobFiles[8], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "CorpseBlobFile",           &aszUnitBlobFiles[9], 1,  {50}},
	{CONFIG_VAR_TYPE_KEY, "AutomapCorpsesToggle",    &tAutomapCorpse     },
	{CONFIG_VAR_TYPE_KEY, "AutomapMonstersToggle",   &tAutomapMonsters   },
	{CONFIG_VAR_TYPE_KEY, "AutomapChestsToggle",     &tAutomapChests     },
	{CONFIG_VAR_TYPE_KEY, "AutomapMissilesToggle",   &tAutomapMissiles     },
	{CONFIG_VAR_TYPE_KEY, "AutomapItemsToggle",      &tAutomapItems      },
	{CONFIG_VAR_TYPE_CHAR_ARRAY0, "MonsterColours",          &anMonsterColours,    1,{1000}},
	{CONFIG_VAR_TYPE_CHAR_ARRAY0, "MonsterCorpseColours",    &anMonsterCorpseColors, 1,{1000}},
	{CONFIG_VAR_TYPE_CHAR_ARRAY1, "MissileColours",          &anMissileColours,    1,{1000}},
	{CONFIG_VAR_TYPE_INT, "HostileMissileColour",    &nHostileMissileColor,  1 },
	{CONFIG_VAR_TYPE_INT, "GuidedMissileColour",     &nGuidedMissileColor, 1 },
	{CONFIG_VAR_TYPE_INT, "TracerMissileColour",     &nTracerMissileColor, 1 },
	{CONFIG_VAR_TYPE_INT, "OtherMissileColour",      &nOtherMissileColor,  1 },
	{CONFIG_VAR_TYPE_INT, "ClosedChestColour",       &nClosedChestColor,   1 },
	{CONFIG_VAR_TYPE_INT, "LockedChestColour",       &nLockedChestColor,   1 },
	{CONFIG_VAR_TYPE_INT, "NormalCorpseColour",      &nNormalCorpseColor,  1 },
	{CONFIG_VAR_TYPE_INT, "BossCorpseColour",        &nBossCorpseColor , 1 },
	{CONFIG_VAR_TYPE_INT, "MinionCorpseColour",      &nMinionCorpseColor,  1 },
	{CONFIG_VAR_TYPE_INT, "ChampionCorpseColour",    &nChampionCorpseColor,  1 },
	{CONFIG_VAR_TYPE_INT, "NormalMonsterColour",     &nMonsterNormalColor, 1 },
	{CONFIG_VAR_TYPE_INT, "BossMonsterColour",       &nMonsterBossColor  , 1 },
	{CONFIG_VAR_TYPE_INT, "MinionMonsterColour",     &nMonsterMinionColor, 1 },
	{CONFIG_VAR_TYPE_INT, "ChampionMonsterColour",   &nMonsterChampColor,  1 },
	{CONFIG_VAR_TYPE_INT, "SuperUniqueColour",       &nSuperUniqueColor  , 1 },
	{CONFIG_VAR_TYPE_INT, "Act5BarbarianColour",     &anMonsterColours[522], 1 },
	{CONFIG_VAR_TYPE_INT, "NeutralPlayerColour",     &nNeutralPlayerColor, 1 },
	{CONFIG_VAR_TYPE_INT, "HostilePlayerColour",     &nHostilePlayerColor, 1 },
	{CONFIG_VAR_TYPE_INT, "FriendMinionColour",      &nFriendMinionColor,  1 },
	{CONFIG_VAR_TYPE_INT, "PlayerMinionColour",      &nPlayerMinionColor,  1 },
	{CONFIG_VAR_TYPE_INT, "PartyPlayerTextColour",   &nPartyTextColour,    1 },
	{CONFIG_VAR_TYPE_INT, "NeutralPlayerTextColour", &nNeutralTextColour,  1 },
	{CONFIG_VAR_TYPE_INT, "HostilePlayerTextColour", &nHostileTextColour,  1 },
	{CONFIG_VAR_TYPE_INT, "SuperUniqueDescFont",	 &nSuperUniqueDescFont,1 },
	{CONFIG_VAR_TYPE_INT, "MonsterDescColour",        &nMonsterTextCol  , 1 },
	{CONFIG_VAR_TYPE_D2STR, "EnchantmentDescs",         &awszEnchantDescs,    50, {20 }},
	{CONFIG_VAR_TYPE_D2STR, "AuraDescs",                &awszAuraDescs,     7,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ImmunityDescs",            &awszImmuneDescs,   6,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ExtraStrongDesc",          &awszEnchantDescs[5], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ExtraFastDesc",            &awszEnchantDescs[6], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "CursedDesc",               &awszEnchantDescs[7], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "MagicResistantDesc",       &awszEnchantDescs[8], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "FireEnchantedDesc",        &awszEnchantDescs[9], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ChampionDesc",             &awszEnchantDescs[16],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "LightningEnchantedDesc",   &awszEnchantDescs[17],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ColdEnchantedDesc",        &awszEnchantDescs[18],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ThiefDesc",                &awszEnchantDescs[24],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ManaBurnDesc",             &awszEnchantDescs[25],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "TeleportationDesc",        &awszEnchantDescs[26],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "SpectralHitDesc",          &awszEnchantDescs[27],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "StoneSkinDesc",            &awszEnchantDescs[28],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "MultipleShotsDesc",        &awszEnchantDescs[29],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "GhostlyDesc",              &awszEnchantDescs[36],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "FanaticDesc",              &awszEnchantDescs[37],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "PossessedDesc",            &awszEnchantDescs[38],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "BerserkerDesc",            &awszEnchantDescs[39],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "MightAuraDesc",            &awszAuraDescs[0],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "HolyFireAuraDesc",         &awszAuraDescs[1],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "BlessedAimAuraDesc",       &awszAuraDescs[2],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "HolyFreezeAuraDesc",       &awszAuraDescs[3],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "HolyShockAuraDesc",        &awszAuraDescs[4],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ConvictionAuraDesc",       &awszAuraDescs[5],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "FanaticismAuraDesc",       &awszAuraDescs[6],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "PhysicalImmunityDesc",     &awszImmuneDescs[0],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "MagicImmunityDesc",        &awszImmuneDescs[1],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "FireImmunityDesc",         &awszImmuneDescs[2],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "LightningImmunityDesc",    &awszImmuneDescs[3],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ColdImmunityDesc",         &awszImmuneDescs[4],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "PoisonImmunityDesc",       &awszImmuneDescs[5],  1,  {20 }},
  {CONFIG_VAR_TYPE_KEY, "ScreenShakeToggle",		&tScreenshake   },
  {CONFIG_VAR_TYPE_KEY, "CenterMapScrollKey",      &tCenterMap },
  {CONFIG_VAR_TYPE_INT, "ScrollMode",              &nScrollMode      , 4 },
  {CONFIG_VAR_TYPE_MINIMAP_SCROLL, "MiniMapScrollOffset",  0, },
  {CONFIG_VAR_TYPE_SCREEN_SCROLL, "ScreenScrollOffset",  0, },
  {CONFIG_VAR_TYPE_INT_ARRAY1, "RescueBarIncompleteShift",&dwRescueBarIncompleteShift,2,{0}},
  {CONFIG_VAR_TYPE_INT_ARRAY1, "RescueBarCompleteShift",&dwRescueBarCompleteShift,2,{0}},
  {CONFIG_VAR_TYPE_INT_ARRAY1, "DuranceOfHateLevel3TeleportShift",&dwDuranceOfHateLevel3TeleportShift,3,{0}},
  {CONFIG_VAR_TYPE_INT_ARRAY1, "DuranceOfHateLevel3TeleportShiftBugKM",&dwDuranceOfHateLevel3TeleportShiftBugKM,3,{0}},
  {CONFIG_VAR_TYPE_INT_ARRAY1, "TravincalShift",&dwTravincalShift,2,{0}},
	{CONFIG_VAR_TYPE_INT,"WayPointRandom",&dwWayPointRandom,1}, 
};
void automap_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void automap_initConfigVars() {
	const static int neutral[]={
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
	memset(minimapLevels,0,sizeof(minimapLevels));
	memset( wszStaffTombLvlDesc ,      0 ,      sizeof(wszStaffTombLvlDesc) );
	memset( aszUnitBlobFiles ,    '\0' ,      sizeof(aszUnitBlobFiles) );
	memset(anMonsterCorpseColors, -1,       sizeof(anMonsterCorpseColors));
	memset(anMissileColours,    -1,       sizeof(anMissileColours));
	memset( awszAuraDescs ,            0 ,      sizeof(awszAuraDescs) );
	memset( awszEnchantDescs ,         0 ,      sizeof(awszEnchantDescs) );
	memset( awszImmuneDescs ,          0 ,      sizeof(awszImmuneDescs) );
	memset(anMonsterColours,-1,734);
	anMonsterColours[522]=0xcb;      //522 Act 5 Combatant (barb fighter)*
	for (int i=0;neutral[i];i++) anMonsterColours[neutral[i]]=-2; 
	if (nCaveNameTextCol>12)nCaveNameTextCol=0;
	if (nMinimapSize>10 ) nMinimapSize = 10 ;
	if (!aszUnitBlobFiles[6][0]) strcpy(aszUnitBlobFiles[6],aszUnitBlobFiles[1]);
	if (!aszUnitBlobFiles[7][0]) strcpy(aszUnitBlobFiles[7],aszUnitBlobFiles[1]);
	if (!aszUnitBlobFiles[8][0]) strcpy(aszUnitBlobFiles[8],aszUnitBlobFiles[0]);
	if (!aszUnitBlobFiles[9][0]) strcpy(aszUnitBlobFiles[9],aszUnitBlobFiles[1]);
	if (!aszUnitBlobFiles[10][0]) strcpy(aszUnitBlobFiles[10],aszUnitBlobFiles[1]);
	if (nMonsterTextCol>12) nMonsterTextCol= 0 ;
	if (nSuperUniqueDescFont>12) nSuperUniqueDescFont = 0;
	dwMinimapScrollCount=0;
	memset(tMiniMapScrollKeys ,       0 ,      sizeof(tMiniMapScrollKeys) );
	memset(dwMiniMapScrollOffset ,       0 ,      sizeof(dwMiniMapScrollOffset) );
	dwScreenScrollCount=0;
	memset(tScreenScrollKeys,		0,		sizeof(tScreenScrollKeys) );
	memset(dwScreenScrollOffset,	0,		sizeof(dwScreenScrollOffset) );
}
void automap_initKeyEvent() {
	for (int i=0;i<dwScreenScrollCount;i++) setupKey(&tScreenScrollKeys[i]);
	for (int i=0;i<dwMinimapScrollCount;i++) setupKey(&tMiniMapScrollKeys[i]);
}

static wchar_t *wscolorcpy(wchar_t *dest, wchar_t *src, BYTE color) {
	wcscpy(dest,src) ; 
  wchar_t *p = dest;	
	while(*p) p++;
	if (p[-1]!=255) return dest;
	if (color==(BYTE)-1) {p[-1]=0;return dest;}
	*p++='c';*p++='0'+color;*p=0;
	return dest;
}
void draw2map(POINT *minimap,int drawX,int drawY) {
	minimap->x=drawX/(*d2client_pMinimapZoom)-(*d2client_pScreenMinimapX)+8;
	minimap->y=drawY/(*d2client_pMinimapZoom)-(*d2client_pScreenMinimapY)-8;
	if (MINIMAP_TYPE) {minimap->x--;minimap->y+=5;}
}
void tile2draw(POINT* pos,int x,int y) {
	pos->x=(5*(x-y))<<4;
	pos->y=(5*(x+y))<<3;
}
void tile2map(POINT* pos,int x,int y) {
	pos->x=(5*(x-y))<<4;
	pos->y=(5*(x+y))<<3;
	draw2map(pos,pos->x,pos->y);
}
void addMapTarget(int srcLvl,int dstLvl,int type,wchar_t *enName,wchar_t *chName) {
	MinimapLevel *pLvl = &minimapLevels[srcLvl];
	MinimapLevelTarget *last=NULL,*pT=NULL;
	for (MinimapLevelTarget *pTarget=pLvl->targets;pTarget;pTarget=pTarget->next) {
		last=pTarget;if (pTarget->dstLvl==dstLvl) pT=pTarget;
	}
	if (!pT) {
		pT=(MinimapLevelTarget *)HeapAlloc(confHeap,HEAP_ZERO_MEMORY,sizeof(MinimapLevelTarget));
		if (last) last->next=pT;else pLvl->targets=pT;
		pT->id=pLvl->n;pT->level=pLvl;pLvl->n++;
	}
	pT->ready=0;pT->type=type;pT->dstLvl=dstLvl;pT->enName=enName;pT->chName=chName;
}
extern int waypointMap[140];
void automap_fixValues() {
	for (int i=1;i<140;i++) 
		if (waypointExist(i)) 
			addMapTarget(i,CELLNO_WAYPOINT,0,L"Waypoint",L"小站");
}

wchar_t wcsLevelTargetName[128];
extern char targetNameColor;
void formatKeyW(wchar_t *buf,int vk);
static void updateTargetName() {
	wchar_t *name=NULL;
	wcsLevelTargetName[0]=0;
	if (!pCurMapLevel||!pCurMapLevel->curTarget) return;
	MinimapLevelTarget *pT=pCurMapLevel->curTarget;
	int dstLvl=pT->dstLvl;
	name=dwGameLng?pT->chName:pT->enName;
	if (!name) {
		switch (dstLvl) {
			case CELLNO_WAYPOINT:name=dwGameLng?L"小站":L"Waypoint";break;
			default:
				name=L"";
				if (0<dstLvl&&dstLvl<=136) {
					LevelTxt *txt=d2common_GetLevelTxt(pCurMapLevel->curTarget->dstLvl);
					if (txt) name=txt->wszName;
				}
		}
	}
	int pos=0;
	if (pCurMapLevel->n>1) {
		wchar_t keyName[32];formatKeyW(keyName,tNextMapTarget.key);
		pos+=wsprintfW(wcsLevelTargetName+pos, L"%d/%d %cc7%s%cc%c ",pCurMapLevel->cur+1,pCurMapLevel->n,
			255,keyName,255,'0'+targetNameColor);
	}
	pos+=wsprintfW(wcsLevelTargetName+pos, L"(%d)%s",dstLvl,name);
}
void AutoMapNewGame() {
	pCurMapLevel=NULL;
	memset(afRevealedActs,0,sizeof(afRevealedActs));
	for (int lvl=0;lvl<140;lvl++ ) {
		MinimapLevel *pLvl=&minimapLevels[lvl];
		pLvl->lvl=lvl;pLvl->revealed=0;
		pLvl->cur=0;pLvl->curTarget=pLvl->targets;
		for (MinimapLevelTarget *pTarget=pLvl->targets;pTarget;pTarget=pTarget->next) {
			pTarget->ready=0;
			pTarget->rectRoute=NULL;pTarget->dstRect=NULL;
			pTarget->tileRoute=NULL;pTarget->dstTile=NULL;
			pTarget->recentNext=0;
		}
	}
	InitScrollLock(0);
}
int PrevMapTarget() {
	if (!pCurMapLevel||!pCurMapLevel->targets) return 0;
	int prevId=0,n=0;MinimapLevelTarget *prev=NULL;
	for (MinimapLevelTarget *pTarget=pCurMapLevel->targets;pTarget;pTarget=pTarget->next) {
		prev=pTarget;prevId=n;
		if (pTarget->next==pCurMapLevel->curTarget) break;
		n++;
	}
	pCurMapLevel->curTarget=prev;pCurMapLevel->cur=prevId;
	updateTargetName();AutoMapRoute();return 1;
}
int NextMapTarget() {
	if (!pCurMapLevel||!pCurMapLevel->curTarget) return 0;
	if (pCurMapLevel->curTarget->next) {pCurMapLevel->curTarget=pCurMapLevel->curTarget->next;pCurMapLevel->cur++;}
	else {pCurMapLevel->curTarget=pCurMapLevel->targets;pCurMapLevel->cur=0;}
	if (!pCurMapLevel->curTarget) return 0;
	updateTargetName();
	//if (dwGameLng) wsprintfW(wszbuf, L"切换到目标 %s",wcsLevelTargetName);
	//else wsprintfW(wszbuf, L"Switch to target %s",wcsLevelTargetName);
	//partyMessageWColor(0,wszbuf);
	AutoMapRoute();
	return 1;
}
void incMapTargetIf(int cur) {
	if (!pCurMapLevel||!pCurMapLevel->curTarget) return;
	if (pCurMapLevel->cur==cur) NextMapTarget();
}
static void selectWaypointTarget() {
	if (!pCurMapLevel||!pCurMapLevel->targets) return;
	int n=0;
	for (MinimapLevelTarget *pTarget=pCurMapLevel->targets;pTarget;pTarget=pTarget->next) {
		if (pTarget->dstLvl==CELLNO_WAYPOINT) {
			pCurMapLevel->curTarget=pTarget;pCurMapLevel->cur=n;
			break;
		}
		n++;
	}
	updateTargetName();
}
int AutoRouteLocateSource(int canRoute);
void AutoMapNewLevel() {
	pCurMapLevel=&minimapLevels[dwCurrentLevel];
	updateTargetName();
	int mode=dwAutoRevealMode;
	if (dwCurrentLevel==Level_RogueEncampment||dwCurrentLevel==Level_TheSecretCowLevel) mode=2;
	if (mode) {
		if (mode>=3&&!afRevealedActs[ACTNO]
			||mode<3&&pCurMapLevel->revealed<dwAutoRevealMode) {
			int autoReveal=dwPlayerLevel>=dwAutoRevealCLevel;
			if (!autoReveal) {
				static int qids[5]={6,14,22,26,40};
				if (QUESTDATA->quests[qids[ACTNO]]&1) autoReveal=1;
			}
			if (autoReveal) dwRevealMapMs=dwCurMs+500;
			if (dwCurrentLevel==Level_RogueEncampment) AutoRouteLocateSource(1);
		}
	}
	if (dwCurrentLevel!=Level_HallsofPain&&waypointExist(dwCurrentLevel)&&!hasWaypoint(dwCurrentLevel))
		selectWaypointTarget();
	else if (dwCurrentLevel==Level_ChaosSanctuary) {
		if (!fIsHardCoreGame&&DIFFICULTY<=1) {
			if (pCurMapLevel&&pCurMapLevel->cur==0) {
				NextMapTarget();
			}
		}
	}
}
void addMinimapTarget(AreaRectInfo *pInfo,int srcLvl,int dstLvl,int unitX,int unitY,int unitType,int unitTxt) {
	//LOG("addMinimapTarget %d %d %d %d %d %d\n",srcLvl,dstLvl,unitX,unitY,unitType,unitTxt);
	int drawX=(unitX-unitY)*16;int drawY=(unitX+unitY)*8;
	if (dstLvl<0) dstLvl=-dstLvl;if (srcLvl<0||srcLvl>=140) return;
	int added=0;
	MinimapLevel *pMapLevel=&minimapLevels[srcLvl];
	for (MinimapLevelTarget *pTarget=pMapLevel->targets;pTarget;pTarget=pTarget->next) {
		if(pTarget->dstLvl==dstLvl) {
			pTarget->ready=1;pTarget->p.drawX=drawX;pTarget->p.drawY=drawY;
			pTarget->p.unitX=unitX;pTarget->p.unitY=unitY;pTarget->p.unitType=unitType;pTarget->p.unitTxt=unitTxt;
			pTarget->p.rectTileX=pInfo->tileX;pTarget->p.rectTileY=pInfo->tileY;
			pTarget->dstRect=NULL;pTarget->dstTile=NULL;added=1;
		}
	}
}
static void target_sort(MinimapLevelTarget **ppt) {
	MinimapLevelTarget *pT=*ppt;if (!pT) return;
	MinimapLevelTarget *next=pT->next;if (!next) return;
	if (pT->dstLvl<3000||next->dstLvl<3000) return;
	if (!pT->ready||!next->ready) return;
	int dis1=getDistanceM256(dwPlayerX-pT->p.unitX,dwPlayerY-pT->p.unitY);
	int dis2=getDistanceM256(dwPlayerX-next->p.unitX,dwPlayerY-next->p.unitY);
	if (dis1>dis2) {
		pT->next=next->next;next->next=pT;*ppt=next;
	}
}
extern int dwBarbrianLeft;
static void modifyRescueBarTarget() {
	MinimapLevel *pMapLevel=&minimapLevels[Level_FrigidHighlands];
	if (!pMapLevel->targets) return;
	target_sort(&pMapLevel->targets);
	if (pMapLevel->targets->next) {
		target_sort(&pMapLevel->targets->next);
		target_sort(&pMapLevel->targets);
	}
	int done=QUESTDATA->quests[36]&3;
	int id=0,lvl=3000;
	for (MinimapLevelTarget *pTarget=pMapLevel->targets;pTarget;pTarget=pTarget->next) {
		//LOG("modifyRescueBarTarget %d %d\n",pTarget->dstLvl,getDistanceM256(dwPlayerX-pTarget->p.unitX,dwPlayerY-pTarget->p.unitY));
		if (pTarget->dstLvl>=3000) {
			pTarget->dstLvl=lvl++;
			if (pTarget->dstLvl==3002) {
				if (done) {
					pTarget->p.unitX+=dwRescueBarCompleteShift[0];pTarget->p.unitY+=dwRescueBarCompleteShift[1];
				} else {
					pTarget->p.unitX+=dwRescueBarIncompleteShift[0];pTarget->p.unitY+=dwRescueBarIncompleteShift[1];
				}
				pTarget->ready|=2;
				pTarget->p.drawX=(pTarget->p.unitX-pTarget->p.unitY)*16;
				pTarget->p.drawY=(pTarget->p.unitX+pTarget->p.unitY)*8;
			}
		}
		pTarget->id=id++;
	}
	pMapLevel->cur=0;pMapLevel->curTarget=pMapLevel->targets;
}
static void modifySealTarget() {
	MinimapLevel *pMapLevel=&minimapLevels[Level_ChaosSanctuary];
	if (!pMapLevel->targets) return;
	int mx=0,my=0,n=0,topY=0x7FFFFFFF;
	MinimapLevelTarget *pTopSeal=NULL,*leftTop=NULL,*leftBottom=NULL,*rightTop=NULL,*rightBottom=NULL,*t;
	MinimapTargetPoint top,lt,lb,rt,rb;
	for (MinimapLevelTarget *pTarget=pMapLevel->targets;pTarget;pTarget=pTarget->next) {
		if (3000<=pTarget->dstLvl&&pTarget->dstLvl<=3004) {
			mx+=pTarget->p.unitX;my+=pTarget->p.unitY;n++;
			if (pTarget->p.unitY<topY) {topY=pTarget->p.unitY;pTopSeal=pTarget;}
		}
	}
	mx/=n;my/=n;
	for (MinimapLevelTarget *pTarget=pMapLevel->targets;pTarget;pTarget=pTarget->next) {
		if (3000<=pTarget->dstLvl&&pTarget->dstLvl<=3004) {
			if (pTarget==pTopSeal) continue;
			if (pTarget->p.unitX<mx) {if (!leftTop) leftTop=pTarget;else leftBottom=pTarget;}
			else {if (!rightTop) rightTop=pTarget;else rightBottom=pTarget;}
		}
	}
	if (leftTop&&leftBottom&&leftTop->p.unitY>leftBottom->p.unitY) {t=leftTop;leftTop=leftBottom;leftBottom=t;}
	if (rightTop&&rightBottom&&rightTop->p.unitY>rightBottom->p.unitY) {t=rightTop;rightTop=rightBottom;rightBottom=t;}
	if (pTopSeal) top=pTopSeal->p;
	if (leftTop) lt=leftTop->p;if (leftBottom) lb=leftBottom->p;
	if (rightTop) rt=rightTop->p;if (rightBottom) rb=rightBottom->p;
	for (MinimapLevelTarget *pTarget=pMapLevel->targets;pTarget;pTarget=pTarget->next) {
		switch (pTarget->dstLvl) {
			case 3000:if (pTopSeal) pTarget->p=top;else pTarget->ready=0;break;
			case 3001:if (leftTop) pTarget->p=lt;else pTarget->ready=0;break;
			case 3002:if (leftBottom) pTarget->p=lb;else pTarget->ready=0;break;
			case 3003:if (rightTop) pTarget->p=rt;else pTarget->ready=0;break;
			case 3004:if (rightBottom) pTarget->p=rb;else pTarget->ready=0;break;
		}
	}
}
static void modifyTempleTarget() {
	MinimapLevel *pMapLevel=&minimapLevels[Level_Travincal];if (!pMapLevel->targets) return;
	for (MinimapLevelTarget *pTarget=pMapLevel->targets;pTarget;pTarget=pTarget->next) {
		if (pTarget->dstLvl==Level_DuranceofHateLevel1) {
				pTarget->p.unitX+=dwTravincalShift[0];
				pTarget->p.unitY+=dwTravincalShift[1];
				pTarget->ready|=2;
				pTarget->p.drawX=(pTarget->p.unitX-pTarget->p.unitY)*16;
				pTarget->p.drawY=(pTarget->p.unitX+pTarget->p.unitY)*8;
		}
	}
}
extern int dwBugFlag;
static void modifyTeleportTarget() {
	MinimapLevel *pMapLevel=&minimapLevels[Level_DuranceofHateLevel3];if (!pMapLevel->targets) return;
	for (MinimapLevelTarget *pTarget=pMapLevel->targets;pTarget;pTarget=pTarget->next) {
		if (pTarget->dstLvl==339) {
			int *settings;
			if (DIFFICULTY==2&&(dwBugFlag&4)) settings=dwDuranceOfHateLevel3TeleportShiftBugKM;
			else settings=dwDuranceOfHateLevel3TeleportShift;
			int random=settings[2];
			int t=((rand()&0x7FFFFFFF)%(random<<1))-random;
			pTarget->p.unitX+=settings[0]+t;
			t=((rand()&0x7FFFFFFF)%(random<<1))-random;
			pTarget->p.unitY+=settings[1]+t;
			pTarget->ready|=2;
			pTarget->p.drawX=(pTarget->p.unitX-pTarget->p.unitY)*16;
			pTarget->p.drawY=(pTarget->p.unitX+pTarget->p.unitY)*8;
		}
	}
}
CellFile *bitmap2dc6(BYTE *pixels, int width, int height) {
	BYTE *outbuf = new BYTE[(width*height*2)+height], *dest = outbuf;
	int len_row=width&3?(width+3)&0xFFFFFFFC:width;
	BYTE *row_src = pixels;
	for (int i = 0; i < height; i++,row_src+=len_row) {
		BYTE *src=row_src,*row_end = row_src+width;
		while (src < row_end) {
			BYTE *scan_limit = src+0x7f;
			BYTE *start=src;
			if (scan_limit>row_end) scan_limit=row_end;
			if (*src) {do src++;while (*src&&src<scan_limit);}
			else {do src++;while (!*src&&src<scan_limit);}
			if (*start) {*dest++=src-start;memcpy(dest,start,src-start);dest+=src-start;}
			else if (src<row_end) *dest++=0x80|(src-start);
		}
		*dest++=0x80;
	}
	static DWORD dc6head[] = {6, 1, 0, 0xeeeeeeee, 1, 1, 0x1c,  0, -1, -1, 0, 0, 0, -1, -1};
	dc6head[8] = width;
	dc6head[9] = height;
	int datasize=dc6head[14] = dest-outbuf;
	int filesize=dc6head[13] = sizeof(dc6head)+datasize+3;
	BYTE *ret = new BYTE[filesize];
	dest=ret;memcpy(dest, dc6head, sizeof(dc6head));dest+=sizeof(dc6head);
	memcpy(dest,outbuf,dc6head[14]);dest+=dc6head[14];
	memset(dest,0xee, 3);
	delete outbuf;
	return (CellFile *)ret;
}
static CellFile *LoadBmpCellFile(char *filename) {
	BYTE *ret = 0;
	FILE *fp=fopen(filename,"rb");if (!fp) return NULL;
	int size;char *buf1=loadFile(NULL,fp,&size);
	BITMAPFILEHEADER *bmphead1 = (BITMAPFILEHEADER *)buf1;
	BITMAPINFOHEADER *bmphead2 = (BITMAPINFOHEADER *)(buf1+sizeof(BITMAPFILEHEADER));
	if (buf1 && (bmphead1->bfType == 'MB') && (bmphead2->biBitCount == 8) && (bmphead2->biCompression == BI_RGB)) {
		ret = (BYTE *)bitmap2dc6((BYTE *)buf1+bmphead1->bfOffBits, bmphead2->biWidth, bmphead2->biHeight);
	}
	free(buf1);
	return (CellFile *)ret;
}
static CellFile *InitCellFile(CellFile *pCellFile) {
	if (pCellFile) d2cmp_InitCellFile(pCellFile, &pCellFile, "?", 0, -1, "?");
	return pCellFile;
}
static void DeleteCellFile(CellFile *pCellFile) {
	if (pCellFile) {d2cmp_DeleteCellFile(pCellFile);delete pCellFile;}
}
void InitCellFiles() {
	char filename[MAX_PATH];
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUMOF_SHRINES; j++) {
			sprintf(filename,"%sBMP\\%s%.2d.BMP",szPluginPath,"MINISHRINE"+(i?0:4) ,j);
			apMyShrineCells[i][j] = InitCellFile(LoadBmpCellFile(filename));
		}
	}
	sprintf(filename,"%sBMP\\%s",szPluginPath,"WAYPOINT.BMP");
	pWaypointCell = InitCellFile(LoadBmpCellFile(filename));
	sprintf(filename,"%sBMP\\%s",szPluginPath,"BLOBCELL.BMP");
	pMinimapObjectCell = InitCellFile(LoadBmpCellFile(filename));
	if(!pMinimapObjectCell) {
		sprintf(filename,"%sBMP\\%s",szPluginPath,"BLOBDOT.BMP");
		pMinimapObjectCell = InitCellFile(LoadBmpCellFile(filename));
	}
	for (int i=0;i<_ARRAYSIZE(pUnitBlobCells);i++) {
		if (aszUnitBlobFiles[i]) {
			sprintf(filename,"%sBMP\\%s.BMP",szPluginPath, aszUnitBlobFiles[i]);
			pUnitBlobCells[i] = InitCellFile(LoadBmpCellFile(filename));
		}
	}
}
void DeleteCellFiles() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUMOF_SHRINES; j++) {
			DeleteCellFile(apMyShrineCells[i][j]);
		}
	}
	DeleteCellFile(pWaypointCell);
	DeleteCellFile(pMinimapObjectCell);
	memset(apMyShrineCells,0,sizeof(apMyShrineCells) );
	pWaypointCell = NULL;
	pMinimapObjectCell = NULL;
	for (int i=0;i<_ARRAYSIZE(pUnitBlobCells);i++) DeleteCellFile(pUnitBlobCells[i]);
	memset(pUnitBlobCells,0,sizeof(pUnitBlobCells));
}
void InitAutomapLayer(int levelno) {	
	LevelDefTxt *pLayerTxt=d2common_GetLevelDefTxt(levelno) ;
	d2client_InitAutomapLayer(pLayerTxt->dwLayerNo);
}
void mapReveal(AreaRectInfo *pInfo) {
	BYTE b[6];*(short *)(b+1)=(short)pInfo->tileX;*(short *)(b+3)=(short)pInfo->tileY;
	b[5]=(BYTE)pInfo->pDrlgLevel->dwLevelNo;d2client_RecvCommand07(b);
}
void mapHide(AreaRectInfo *pInfo) {
	BYTE b[6];*(short *)(b+1)=(short)pInfo->tileX;*(short *)(b+3)=(short)pInfo->tileY;
	b[5]=(BYTE)pInfo->pDrlgLevel->dwLevelNo;d2client_RecvCommand08(b);
}
struct SC {int x,y,c;AreaRectInfo *pInfo;};
static void addCorner(SC *sc,int srcLvl,int dstLvl) {
	addMinimapTarget(sc->pInfo,srcLvl,dstLvl,sc->x*5,sc->y*5,0,0);
}
void addSecretCowLevelCorners(DrlgLevel *pDrlgLevel) {
	SC leftTop,leftBottom,rightBottom,rightTop,p;
	leftTop.c=1000000000;
	leftBottom.c=1000000000;
	rightBottom.c=-1000000000;
	rightTop.c=-1000000000;
	for (AreaRectInfo *pInfo=pDrlgLevel->pAreaRectInfo;pInfo;pInfo=pInfo->pNext) {
		p.x=pInfo->tileX+(pInfo->tileW>>1);
		p.y=pInfo->tileY+(pInfo->tileH>>1);
		p.pInfo=pInfo;
		p.c=p.x+p.y;if (p.c>rightTop.c) rightTop=p;
		if (p.c<leftBottom.c) leftBottom=p;
		p.c=p.x-p.y;if (p.c>rightBottom.c) rightBottom=p;
		if (p.c<leftTop.c) leftTop=p;
	}
	addCorner(&leftTop,pDrlgLevel->dwLevelNo,3000);
	addCorner(&leftBottom,pDrlgLevel->dwLevelNo,3001);
	addCorner(&rightBottom,pDrlgLevel->dwLevelNo,3002);
	addCorner(&rightTop,pDrlgLevel->dwLevelNo,3003);
}
void RevealAutomapLevel(DrlgLevel *pDrlgLevel,int mode) {
	MinimapLevel *pMapLevel=&minimapLevels[pDrlgLevel->dwLevelNo];
	if (pMapLevel->revealed>=mode) return;
	switch (pDrlgLevel->dwLevelNo) {case 40:case 75:case 103:case 109:case 137:return;}
	int ms=GetTickCount();
	InitAutomapLayer(pDrlgLevel->dwLevelNo);
	minimapTargetId=3000;
	for (AreaRectInfo *pInfo=pDrlgLevel->pAreaRectInfo;pInfo;pInfo=pInfo->pNext) {
		if(pInfo->dwPresetType == 2) {
			int dwDefNo = pInfo->pLvlPreset->dwDef;
			if( dwDefNo == 38 || dwDefNo == 39 || 
				(dwDefNo >= 401 && dwDefNo <= 405)||
				dwDefNo == 836 || dwDefNo == 863 ){
				continue;
			}
		}
		int needHide=0,revealed=0;
		if (!pInfo->pAreaRectData) {mapReveal(pInfo);needHide=1;}
		if (mode>=2) {d2client_AddAreaRectToMinimap(pInfo->pAreaRectData,1,LAYER);revealed=1;}
		if (pMapLevel->revealed==0) AddRectToMinimap(pInfo,revealed);
		if (needHide) mapHide(pInfo);
	}
	pMapLevel->revealed=mode;
	switch (pDrlgLevel->dwLevelNo) {
		case Level_Travincal: modifyTempleTarget();break;
		case Level_DuranceofHateLevel3: modifyTeleportTarget();break;
		case Level_ChaosSanctuary: modifySealTarget();break;
		case Level_FrigidHighlands: modifyRescueBarTarget();break;
		case Level_TheSecretCowLevel:addSecretCowLevelCorners(pDrlgLevel);break;
	}
	LOG("RevealAutomapLevel %d mode=%d time=%d ms\n",pDrlgLevel->dwLevelNo,mode,GetTickCount()-ms);
	if (pDrlgLevel->dwLevelNo==dwCurrentLevel) {updateTargetName();AutoMapRoute();}
}
int RevealLevelPlayerIn() {
	DrlgLevel *pDrlgLevel = PLAYER->pMonPath->pAreaRectData->pAreaRectInfo->pDrlgLevel;
	if (!pDrlgLevel) return -1;
	RevealAutomapLevel(pDrlgLevel,2);
	return 0;
}
int RevealCurAct(){
	if (afRevealedActs[ACTNO]) return 0;
	DrlgLevel *pDrlgLevels[137]={0};
	for (DrlgLevel* pDrlgLevel=ACT->pDrlgMisc->pLevelFirst;pDrlgLevel;pDrlgLevel=pDrlgLevel->pNext) {
		pDrlgLevels[pDrlgLevel->dwLevelNo]=pDrlgLevel;
	}
	for (int lvl=actlvls[ACTNO];lvl<actlvls[ACTNO+1];lvl++) {
		if (!pDrlgLevels[lvl]) pDrlgLevels[lvl]=d2common_GetDrlgLevel(ACT->pDrlgMisc,lvl);
	}
	for (int lvl=actlvls[ACTNO];lvl<actlvls[ACTNO+1];lvl++) {
		DrlgLevel *pDrlgLevel=pDrlgLevels[lvl];if (!pDrlgLevel) continue;
		if (!pDrlgLevel->pAreaRectInfo) d2common_InitDrlgLevel(pDrlgLevel);
		RevealAutomapLevel(pDrlgLevel,2);
	}
	InitAutomapLayer(LEVELNO) ; //恢复当前Layer指针
	afRevealedActs[ACTNO]=1;
	return 0;
}
static void ResetAutomapNodeCellNo(AutomapNode *pCell) {
	if (!pCell) return;
	ResetAutomapNodeCellNo(pCell->pLess);
	if (pCell->wCellNo >= CELLNO_MYSHRINES && pCell->wCellNo < CELLNO_MYSHRINES+23) {
		pCell->wCellNo = CELLNO_SHRINE;
	} else if ((short)pCell->wCellNo < 0) {
		pCell->wCellNo = 0;
	}
	ResetAutomapNodeCellNo (pCell->pMore);
}
void RemoveMyAutomapNodes() {	
	if (!fInGame) return;
	int curLayer = LAYER->dwLayerNo;
	for (AutomapLayer *pAutomapLayer=(*d2client_pAutomapLayerList);pAutomapLayer!=NULL;
		pAutomapLayer=pAutomapLayer->pNext) {
		d2client_InitAutomapLayer(pAutomapLayer->dwLayerNo);
		ResetAutomapNodeCellNo( pAutomapLayer->pObjects );
	}
	d2client_InitAutomapLayer(curLayer);
}
void __stdcall DrawAutomapNodePatch(CellContext *pCellContext, int xpos, int ypos, RECT *cliprect, int bright) {
	int cellno = pCellContext->dwCellNo;
	if (cellno < 0) {
		if (tAutomapLevelNames.isOn) { 
			wchar_t temp[1024];
			wcscpy ( temp, d2common_GetLevelTxt(-cellno)->wszName ) ;
			if ( wszStaffTombLvlDesc[0] && (-cellno == (int)ACT->pDrlgMisc->dwStaffTombLvl) ){
				wscolorcpy(temp+wcslen(temp), wszStaffTombLvlDesc ,nCaveNameTextCol);
			}
			if ( -cellno == (int)ACT->pDrlgMisc->dwBossTombLvl ){
				wchar_t temp2[25] = L"<==UniqueBoss Here" ;
				wcscat(temp , temp2 );
			}
			DrawCenterText( 6, temp, xpos, ypos, nCaveNameTextCol);
		}
		return;
	}
	if (cellno == CELLNO_WAYPOINT && pWaypointCell != 0) {
		pCellContext->dwCellNo = 0;
		pCellContext->pCellFile = pWaypointCell;
		xpos += (MINIMAP_TYPE ? 4 : 8)-(pWaypointCell->pCells[0]->dwWidth/2);
		ypos += (MINIMAP_TYPE ? 4 : 0);
	}
	if (((cellno-=CELLNO_MYSHRINES)>=0)&&(cellno<NUMOF_SHRINES)) {
		CellFile *pMyShrine=apMyShrineCells[MINIMAP_TYPE][cellno];
		if ( pMyShrine && tMiniShrine.isOn ) {
			pCellContext->pCellFile = pMyShrine;
			pCellContext->dwCellNo = 0;
			xpos += (MINIMAP_TYPE ? 4 : 8)-(pMyShrine->pCells[0]->dwWidth/2);
		} else {
			pCellContext->dwCellNo = CELLNO_SHRINE;
		}
	}
	d2gfx_DrawAutomapNode(pCellContext, xpos, ypos, cliprect, bright);
}
void __declspec(naked) AddShrinePatch_ASM() {
	__asm {
// 1.11-1.13
//ebx = ptr unit object
//esi = ptr object.txt
//return ecx = cell no
//		mov ecx,[esi]ObjectTxt.nAutoMap //original code
		mov ecx,[esi+0x1BC]
		cmp ecx,CELLNO_SHRINE
		jne notshrine
		xor ecx,ecx
//		test [esi]ObjectTxt.nSubClass,1
		test [esi+0x167],1
		je notshrine
//		mov eax,[ebx]UnitAny.pObjectData
		mov eax,[ebx+0x14]
//		movzx ecx,[eax+0x04]ObjectData.nShrineNo
		movzx ecx,byte ptr [eax+0x04]
		add ecx,CELLNO_MYSHRINES
notshrine:
		ret
	}
}
void __fastcall OverrideShrinePatch(AutomapNode *pNewNode, AutomapNode *pCurNode) {
	if ((pNewNode->wCellNo<CELLNO_MYSHRINES)||(pNewNode->wCellNo>=(CELLNO_MYSHRINES+NUMOF_SHRINES))) return;
	if (pCurNode->wCellNo>=pNewNode->wCellNo) return;
	pCurNode->fSaved = pNewNode->fSaved;
	pCurNode->wCellNo = pNewNode->wCellNo;
}
void __declspec(naked) OverrideShrinePatch_ASM() {
	__asm {
//esi = ptr curr node
//ecx = ptr new node
		pushad
		mov edx,esi
		call OverrideShrinePatch
		popad
//original code
		mov edx,d2client_pAutomapNodeListBaseAddr
		mov edx,[4*eax+edx]
		ret
	}
}
void DrawAutomapNode(CellFile *pCellfile, int xpos, int ypos, char color) {
	if ( !pCellfile )return ;
	CellContext ct;
	memset(&ct, 0, sizeof(ct));
	ct.pCellFile = pCellfile ;
	xpos-=(pCellfile->pCells[0]->dwWidth/2);
	ypos+=(pCellfile->pCells[0]->dwHeight/2);
	int xpos2 = xpos - pCellfile->pCells[0]->dwXoffs ;
	int ypos2 = ypos - pCellfile->pCells[0]->dwYoffs ;
	if ((xpos2>=SCREENSIZE.x)||((xpos2+(int)pCellfile->pCells[0]->dwWidth)<=0)
		||(ypos2>=SCREENSIZE.y)||((ypos2+(int)pCellfile->pCells[0]->dwHeight)<=0)) return;
	static BYTE coltab[2][256];//, tabno = 0, lastcol = 0;
	if (!coltab[0][1]) for (int k = 0; k < 255; k++) coltab[0][k] = coltab[1][k] = k;
	if ((BYTE)color!=pCellfile->mylastcol) {
		if (pCellfile->mytabno&1) pCellfile->mytabno&=0xFE;
		else pCellfile->mytabno|=1;
	}
	pCellfile->mylastcol=coltab[pCellfile->mytabno&1][255]=(BYTE)color;
	d2gfx_DrawCellFile(&ct, xpos, ypos, -1, 5, coltab[pCellfile->mytabno&1]);
}
void __fastcall DrawAutomapBlob(int xpos, int ypos, int dwColor,UnitAny *pUnit) {
	if (MINIMAP_TYPE) {xpos -= 1;ypos += 5;}
	int celltype = 0;
	if (pUnit) {
		celltype = pUnit->dwUnitType;
		if ( celltype==UNITNO_PLAYER ) {
			if (pUnit->dwUnitId==dwPlayerId) celltype = 8;
		}else if (celltype==UNITNO_MONSTER) {
			if (pUnit->dwMode==0x0C) {
				celltype = 9 ;
			} else {
				MonsterTxt *pMonsterTxt = pUnit->pMonsterData->pMonsterTxt;
				if (pMonsterTxt->fNpc) {
					celltype = 7 ;
				} else if (pUnit->pMonsterData->fBoss) {//召唤者回城次后会失去boss标记
					if (pMonsterTxt->Level[0]>0) celltype = 6 ;//影子，陷阱之类的也会被设置fboss标记
				} else {
					if (d2common_CheckUnitState(pUnit, 23)) celltype=10; //dim vision
				}
			}
		}
	}
	if (!pUnitBlobCells[celltype]) {
		static char blines[][2] = {0,-2, 4,-4, 8,-2, 4,0, 8,2, 4,4, 0,2, -4,4, -8,2, -4,0, -8,-2, -4,-4, 0,-2};
		for (int i = 0; i < ARRAYSIZE(blines)-1; i++) {
			d2gfx_DrawLine(xpos+blines[i][0], ypos+blines[i][1], xpos+blines[i+1][0], ypos+blines[i+1][1], dwColor, 0xff);
		}
	} else {
		if(dwColor) {//某些情况下游戏本身会产生黑色的图标，屏蔽掉
			DrawAutomapNode(pUnitBlobCells[celltype], xpos, ypos, dwColor);
		}
	}
}
void __declspec(naked) ForceDrawBlob_ASM() {
	__asm {
		add dword ptr [esp], 0x61
		ret
	}
}
void __declspec(naked) DrawBlobPath_ASM() {
//jmp
//esi ==> pUnit
//[esp+4] ==> color
//ecx = x pos
//eax = y pos
	__asm {
		mov edx , eax
		push esi
		push [esp+8]
		call DrawAutomapBlob
		ret 4
	}
}
void __declspec(naked) DrawPartyPlayerBlobPatch_ASM() {
	__asm {
//dl = col
//ecx = x pos
//eax = y pos
		push 0
		push edx
		mov edx,eax
		call DrawAutomapBlob
		add dword ptr [esp], 0x36
		ret
	}
}
BYTE __fastcall HostilePlayerPatch(UnitAny *pUnit) {
	return testPvpFlag(pUnit->dwUnitId)==0?nHostilePlayerColor:nNeutralPlayerColor;
}
void __declspec(naked) HostilePlayerColor_ASM() {
	__asm {
		mov ecx, esi
		call HostilePlayerPatch
		mov cl, al
		ret
	}
}
BYTE __fastcall DrawPlayerTextPatch(UnitAny *pUnit) {
	if (pUnit->dwMode==17) return 1;
	BYTE fPvPflag = testPvpFlag(pUnit->dwUnitId);
	if ( fPvPflag==2 ) return nPartyTextColour;
	return fPvPflag==0?nHostileTextColour:nNeutralTextColour;
}
void __declspec(naked) DrawPlayerTextPatch_ASM() {
	__asm {
		push ebx
		push edi
		push esi
		mov  ecx , esi
		call DrawPlayerTextPatch
		and eax,0x000000FF
		mov ebx,[esp+0x0C]	//返回地址
		mov [esp+0x0C],eax
		jmp ebx
	}
}
void __fastcall MonsterBlobDesc( UnitAny *pMon ){
	if (pMon->dwMode && pMon->dwMode!=0x0C && getUnitOwnerId(pMon)==-1){
		wchar_t temp[1024];
		memset(temp, L'\0' , sizeof(temp));
		MonsterData *pMonsterData = pMon->pMonsterData;
		if (pMonsterData->fBoss ) {
			int aura = 0, enchno;
			for (int i = 0; i < 9; i++) {
				enchno = pMonsterData->anEnchants[i];
				if (enchno == 0) break;
				if (enchno == 30) aura = 1;
				if (enchno < 32 && awszEnchantDescs[enchno][0] ) {
					wscolorcpy(temp+wcslen(temp), awszEnchantDescs[enchno] ,nMonsterTextCol);
				}
			}
			if (aura) {
				//NOTE :- testing if monster effected by aura, NOT source of aura
				static BYTE states[7] = {0x21, 0x23, 0x28, 0x2b, 0x2e, 0x1c, 0x31};
				for (int i = 0; i < 7; i++) {
					if (awszAuraDescs[i][0] && d2common_CheckUnitState(pMon, states[i]) ) {
						wscolorcpy(temp+wcslen(temp), awszAuraDescs[i] ,nMonsterTextCol);
					}
				}
			}
			if ( tBossNameToggle.isOn ){
				if ( pMonsterData->pMonsterTxt->fBoss == 1 && pMonsterData->wUniqueNo == 0){
					wscolorcpy(temp+wcslen(temp), pMonsterData->wszMonName,nMonsterTextCol);

				}
			}
		}
		static BYTE stats[6] = {0x24, 0x25, 0x27, 0x29, 0x2b, 0x2d};
		for (int i = 0; i < 6; i++) {
			if (awszImmuneDescs[i][0] && d2common_GetUnitStat(pMon, stats[i], 0) >= 100) {
				wscolorcpy(temp+wcslen(temp), awszImmuneDescs[i] ,nMonsterTextCol);
			}
		}
		if(temp[0]){
			POINT ptObj;
			draw2map(&ptObj,pMon->pMonPath->drawX,pMon->pMonPath->drawY);
			if ( nSuperUniqueDescFont>0 && pMon->pMonsterData->fUnique){
				DrawCenterText(nSuperUniqueDescFont,	temp,	ptObj.x,	ptObj.y-10,	nMonsterTextCol);
			}else{
				DrawCenterText(6,	temp,	ptObj.x,	ptObj.y-10,	nMonsterTextCol);
			}
		}
	}
}
void __declspec(naked) MonsterBlobDescPatch_ASM() {
	__asm {
//esi = ptr unit
		mov ecx,esi
		push ebx
		push eax
		push ebp
		call MonsterBlobDesc
		pop ebp
		pop eax
		pop ebx
		mov edx, 9
		ret
	}
}
BYTE __fastcall MonsterBlobCol(UnitAny *pMon) {
	if ( tAutomapMonsters.isOn ) {
		int dwOwnerId=getUnitOwnerId(pMon);
		BYTE color=anMonsterColours[pMon->dwTxtFileNo];
		if (color != (BYTE)-1) return color;
		if (dwOwnerId!=-1) {
			BYTE fPvpFlag = testPvpFlag(dwOwnerId);
			//队伍中，自己的显示0xcb 队友的0x81
			switch (fPvpFlag) {
				case 2:return nFriendMinionColor;
				case 3:return nPlayerMinionColor;
				case 0:return nHostilePlayerColor;
				default:return nNeutralPlayerColor;
			}
		}
		if (pMon->pMonsterData->fChamp ) return nMonsterChampColor;
		if (pMon->pMonsterData->fMinion ) return nMonsterMinionColor;
		if (pMon->pMonsterData->fBoss) {
			if (pMon->pMonsterData->fUnique && nSuperUniqueColor!=(BYTE)-1)
				return nSuperUniqueColor;
			else
				return nMonsterBossColor;
		}
		return nMonsterNormalColor;
	}
	return -1;
}
BYTE __fastcall CorpseBlobCol(UnitAny *pMon) {
	if (tAutomapCorpse.isOn) {
		BOOL flag = d2common_CheckCorpseCanUse(pMon,0);
		if (flag) {
			if (anMonsterCorpseColors[pMon->dwTxtFileNo] == (BYTE)-2) {
				if (pMon->pMonsterData->fChamp ) return nChampionCorpseColor;
				if (pMon->pMonsterData->fMinion ) return nMinionCorpseColor;
				if (pMon->pMonsterData->fBoss ) return nBossCorpseColor;
				return nMonsterNormalColor;
			}
			return anMonsterCorpseColors[pMon->dwTxtFileNo];
		}
	}
	return -1;
}
BYTE __fastcall ObjectBlobCol(UnitAny *pObject) {
	ObjectTxt *pObjTxt = pObject->pObjectData->pObjectTxt;
	if ( tAutomapChests.isOn && !pObject->dwMode ) {
		if (pObjTxt->nSelectable[0] && 
				(
				(pObjTxt->nOperateFn == 1) || //bed, undef grave, casket, sarc
				(pObjTxt->nOperateFn == 3) || //basket, urn, rockpile, trapped soul
				(pObjTxt->nOperateFn == 4) || //chest, corpse, wooden chest, buriel chest, skull and rocks, dead barb
				(pObjTxt->nOperateFn == 5) || //barrel
				(pObjTxt->nOperateFn == 7) || //exploding barrel
				(pObjTxt->nOperateFn == 14) || //loose bolder etc....*
				(pObjTxt->nOperateFn == 19) || //armor stand
				(pObjTxt->nOperateFn == 20) || //weapon rack
				(pObjTxt->nOperateFn == 33) || //writ
				(pObjTxt->nOperateFn == 48) || //trapped soul
				(pObjTxt->nOperateFn == 51) || //stash
				(pObjTxt->nOperateFn == 68)    //evil urn
				)
			)
			return (pObject->pObjectData->nShrineNo&0x80)?nLockedChestColor:nClosedChestColor;
	}
	if ( pObject->dwMode == 2 && pObjTxt->nSubClass&33 ){//已经用掉的shine  1或32(井水)
		POINT ptObj;
		draw2map(&ptObj,pObject->pItemPath->drawX,pObject->pItemPath->drawY);
		wchar_t temp[8] = {L'X',L'\0'};
		DrawCenterText(5, temp, ptObj.x, ptObj.y, 2);
	}
	return -1;
}
BYTE __fastcall MissileBlobCol(UnitAny *pMissile){
	if ( tAutomapMissiles.isOn && !(pMissile->dwFlags1&0x00010000)) {//非死亡的
		if (anMissileColours[pMissile->dwTxtFileNo] == (BYTE)-2) {
			int dwOwnerId = 0;
			if ( pMissile->dwOwnerType == UNITNO_PLAYER ){
				dwOwnerId =  pMissile->dwOwnerId;
			}else if ( pMissile->dwOwnerType ==UNITNO_MONSTER ){
				dwOwnerId =  getUnitOwnerId(pMissile);
			}
			if ( (int)dwOwnerId > 0 ) {
				BYTE fPvPFlag = testPvpFlag(dwOwnerId);
				if ( fPvPFlag > 0 )return nTracerMissileColor;
				if ( fPvPFlag ==0 ) {
					return (pMissile->pMonPath->pTargetUnit == PLAYER) ? nGuidedMissileColor : nHostileMissileColor;
				} 
			}
			if (dwOwnerId==0) return nOtherMissileColor;
			return nHostileMissileColor;
		}
		return anMissileColours[pMissile->dwTxtFileNo];
	}
	return -1;
}
BYTE __fastcall MixedBlobColPatch(UnitAny *pUnit) {
	switch( pUnit->dwUnitType ){
		case UNITNO_MONSTER:
			if (pUnit->dwMode && pUnit->dwMode != 0x0C) return MonsterBlobCol( pUnit );
			return CorpseBlobCol( pUnit );
		case UNITNO_OBJECT:
			return ObjectBlobCol( pUnit );
		case UNITNO_MISSILE:
			return MissileBlobCol( pUnit );
		case UNITNO_ITEM:
			if (tAutomapItems.isOn) {
				BYTE color=GetItemColour(pUnit, 1);
				if ( color==(BYTE)-2) return -1;
				return color;
			}
		default:
			return -1;
	}
}
void __declspec(naked) MixedBlobColPatch_ASM() {
	__asm {
//esi = ptr unit
//ebx = ptr color
//return eax = draw flag
		mov ecx,esi
		push ebx
		call MixedBlobColPatch
		pop ebx
		test al ,al
		jz oldcode
		mov [ebx],al
		sub al,-4
		setb al
oldcode:
		movzx eax,al
		pop esi
		add esp, 0x0C
		ret
	}
}
static int isConnected(AreaRectInfo *pInfo,AreaRectInfo *pNInfo,POINT *path) {
	int x1=pInfo->tileX,y1=pInfo->tileY,w1=pInfo->tileW,h1=pInfo->tileH;
	int x2=pNInfo->tileX,y2=pNInfo->tileY,w2=pNInfo->tileW,h2=pNInfo->tileH;
	int start,end,dx=0,dy=0;
	if (x2==x1+w1||x2+w2==x1) {
		start=y2-y1;if (start>=h1) return 0;
		if (0<=start) end=start+h2;
		else {start=0;end=y2+h2-y1;if (end<=0) return 0;}
		if (end>h1) end=h1;
		dx=x2-x1;
	} else if (y2==y1+h1||y2+h2==y1) {
		start=x2-x1;if (start>=w1) return 0;
		if (0<=start) end=start+w2;
		else {start=0;end=x2+w2-x1;if (end<=0) return 0;}
		if (end>w1) end=w1;
		dy=y2-y1;
	} else return 0;
	start*=5;end*=5;
	AreaRectData *pData=pInfo->pAreaRectData;
	if (!pData||!pData->bitmap) return 0;
	unsigned short *ptr=pData->bitmap->bitmap;
	int w=pData->bitmap->unitW,h=pData->bitmap->unitH;
	int sum=0,n=0;
	if (dx) {
		path->x=-3;if (dx>0) {ptr+=w-1;path->x=w+3;}
		ptr+=start*w;
		for (int y=start;y<end;y++,ptr+=w) if (!(ptr[0]&1)) {sum+=y;n++;}
		if (n) path->y=sum/n;
	} else if (dy) {
		path->y=-3;if (dy>0) {ptr+=w*(h-1);path->y=h+3;}
		ptr+=start;
		for (int x=start;x<end;x++,ptr++) if (!(ptr[0]&1)) {sum+=x;n++;}
		if (n) path->x=sum/n;
	}
	/*int dstLvl=pNInfo->pDrlgLevel->dwLevelNo;
	if (n&&dstLvl==5) {
		ptr=pData->bitmap->bitmap;
		LOG("(%d,%d,%d,%d) (%d,%d,%d,%d) %d-%d\n",x1,y1,w1,h1,x2,y2,w2,h2,start,end);
		LOG("to=%d (%d,%d)(%d,%d,%d,%d)\n",dstLvl,dx,dy,pData->tileX,pData->tileY,pData->tileW,pData->tileH);
		if (logfp) {
			for (int y=0;y<h;y++) {
				for (int x=0;x<w;x++) {
					int b=(*ptr++)&0xF;
					if (b) fprintf(logfp,"%X",b);
					else fputc(' ',logfp);
				}
				fputc('\n',logfp);
			}
		}
	}*/
	return n;
}
int AddRectToMinimap(AreaRectInfo *pInfo,int revealed) {
	int srcLvl=pInfo->pDrlgLevel->dwLevelNo;
	for (int i=0;i<(int)pInfo->nearbyRectCount;i++) {
		POINT path;
		AreaRectInfo *pNInfo=pInfo->pInfoNear[i];
		int dstLvl=pNInfo->pDrlgLevel->dwLevelNo;
		if (dstLvl==srcLvl) continue;
		switch (dstLvl) {
			case 1:case 40:case 75:case 103:case 109:case 137:continue;
			case 7:if (srcLvl==26) continue;break;
			case 26:if (srcLvl==7) continue;break;
		}
		if (!isConnected(pInfo,pNInfo,&path)) continue;
		int unitX=pInfo->tileX*5+path.x,unitY=pInfo->tileY*5+path.y;
		if (dstLvl==Level_ChaosSanctuary) unitY-=30;
		addMinimapTarget(pInfo,srcLvl,dstLvl,unitX,unitY,0,0);
		int needHide=0;
		if (!pNInfo->pAreaRectData) {mapReveal(pNInfo);needHide=1;}
		int add=1;
		if (add) d2client_AddAreaRectToMinimap(pNInfo->pAreaRectData, 1, LAYER);
		if (needHide) mapHide(pNInfo);
		if (add) {
			unitX=(pNInfo->tileX+(pNInfo->tileW>>1))*5;unitY=(pNInfo->tileY+(pNInfo->tileW>>1))*5;
			int drawX=(unitX-unitY)*16,drawY=(unitX+unitY)*8;
			AutomapNode *pCell = d2client_NewAutomapNode();
			pCell->wCellNo = (WORD)(-dstLvl);
			pCell->drawXDiv10 = (WORD)(drawX/10);
			pCell->drawYDiv10 = (WORD)(drawY/10);
			d2client_AddAutomapNode(pCell, &LAYER->pObjects);
		}
	}
	int incTargetId=0;
	for (PresetUnit *pUnit=pInfo->pPresetUnits;pUnit;pUnit=pUnit->pNext) {
		int fAddCell=0,fAddLine=0,fReveal=0,cellno=0,xoffset=0,yoffset=0;
		switch (pUnit->dwUnitType) {
			case UNITNO_MONSTER: 
				switch (pUnit->dwTxtFileNo) {
				case 256:cellno=300;fAddCell=1;fAddLine=1;break;//izual
				//case 434:cellno=minimapTargetId;incTargetId=1;fAddCell=0;fReveal=1;fAddLine=1;break;//prison door
				}
				break;
			case UNITNO_OBJECT: {
					fAddCell=1;fAddLine=1;
					int levelno = pInfo->pDrlgLevel->dwLevelNo;
					switch(pUnit->dwTxtFileNo) {
						case 152:cellno=300;break;//orifice
						case 268:cellno=3000;fAddCell=0;break;//Wirt's Leg
						case 376:cellno=376;fAddCell=0;break;// hell forge
						case 371:cellno=1472;fAddCell=0;break;//countess chest
						case 397:cellno=318;fAddLine=1;break;//chests
						case 460:cellno=1468;break;//drehya = frozen anya
						case 462:cellno=1472;fAddCell=0;break;//nihlathak
						case 473:cellno=minimapTargetId;incTargetId=1;fAddCell=0;fReveal=1;fAddLine=1;break;//barbrian prison
						case 563:if (levelno==131) {cellno=132;fAddCell=0;}break;//Worldstone Chamber
						//case 402:if (srcLvl==46) {cellno=0;break;}//canyon/arcane waypoint
						case 267:if (srcLvl!=75&&srcLvl!=103) {cellno=0;break;}//chests
						case 255:if (srcLvl==Level_ChaosSanctuary) {cellno=255;fAddCell=0;break;} //center point
						case 973: {
							ObjectTxt *pObj = d2common_GetObjectTxt(pUnit->dwTxtFileNo);
							cellno = pObj->nAutoMap;
							break;
						}
						default: {
								if (pUnit->dwTxtFileNo>=573) {
									cellno = 0 ;
								} else {
									ObjectTxt *pObj = d2common_GetObjectTxt(pUnit->dwTxtFileNo);
									cellno = pObj->nAutoMap;
									if (cellno == CELLNO_SHRINE) {//配合AddShrinePatch_ASM
										static WORD parm0cells[4] = {
											CELLNO_MYSHRINES+0, //0 = ??
											CELLNO_MYSHRINES+2, //1 = health
											CELLNO_MYSHRINES+3, //2 = mana
											CELLNO_MYSHRINES+0, //3 = magic
										};
										cellno = (pObj->nSubClass & 1) ? parm0cells[ pObj->dwParm[0] ] : 0;
										fAddLine = 0;
									}
									if (!cellno&&isWaypointTxt(pUnit->dwTxtFileNo)) {
										cellno=CELLNO_WAYPOINT;fAddCell=0;break;
									}
								}
							}
							break;
					}
				}
				break;
			case UNITNO_AREA_ENTRANCE: {
					for (AreaTile *pTile=pInfo->pAreaTiles;pTile;pTile=pTile->pNext) {
						if ( *(pTile->pNum)==pUnit->dwTxtFileNo){
							cellno = -(int)(pTile->pAreaRectInfo->pDrlgLevel->dwLevelNo);
							fAddCell=1;fAddLine=1;
						}
					}
				}
				break;
		}
		if (cellno) {
			int unitX=pInfo->tileX*5+pUnit->unitDx ;
			int unitY=pInfo->tileY*5+pUnit->unitDy ;
			if (fAddCell){//需要结合DrawAutomapNodePatch
				int drawX = (unitX-unitY)*16;
				int drawY = (unitX+unitY)*8;
				if (!revealed) {revealed=1;d2client_AddAreaRectToMinimap(pInfo->pAreaRectData, 1, LAYER);}
				AutomapNode *pCell = d2client_NewAutomapNode();
				pCell->wCellNo = (WORD)cellno;
				pCell->drawXDiv10 = (WORD)(drawX/10+1+xoffset);
				pCell->drawYDiv10 = (WORD)(drawY/10-3+yoffset);
				d2client_AddAutomapNode(pCell, &LAYER->pObjects);
			} else if (fReveal) {
				if (!revealed) {revealed=1;d2client_AddAreaRectToMinimap(pInfo->pAreaRectData, 1, LAYER);}
			}
			if(fAddLine) {
				if (cellno<0) cellno=-cellno;
				switch (srcLvl) {
					case Level_CanyonoftheMagi:
						if (pInfo->pDrlgLevel&&pInfo->pDrlgLevel->pDrlgMisc->dwStaffTombLvl==cellno) cellno=300;
						break;
					case Level_ChaosSanctuary:
						if (cellno==306) {cellno=minimapTargetId;incTargetId=1;}
						break;
				}
				if (dwWayPointRandom>0&&cellno==CELLNO_WAYPOINT) {
					int t=((rand()&0x7FFFFFFF)%(dwWayPointRandom<<1))-dwWayPointRandom;
					unitX+=t;
					t=((rand()&0x7FFFFFFF)%(dwWayPointRandom<<1))-dwWayPointRandom;
					unitY+=t;
				}
				addMinimapTarget(pInfo,srcLvl,cellno,unitX,unitY,pUnit->dwUnitType,pUnit->dwTxtFileNo);
			}
		}
	}
	if (incTargetId) minimapTargetId++;
	return revealed;
}
static void DrawMinimapPoint() {
	MinimapLevel *pMapLevel=&minimapLevels[LEVELNO];
	for (MinimapLevelTarget *pTarget=pMapLevel->targets;pTarget;pTarget=pTarget->next) {
		if (!pTarget->ready) continue;
		int color=routeLineColor;
		POINT ptObj,ptPlayer,ptLine; 
		draw2map(&ptPlayer,PLAYER->pMonPath->drawX,PLAYER->pMonPath->drawY);
		draw2map(&ptObj,pTarget->p.drawX,pTarget->p.drawY);
		int xDistance = ptObj.x-ptPlayer.x;
		int yDistance = ptObj.y-ptPlayer.y;
		if (abs(xDistance)<15&&abs(yDistance)<15) continue;
		ptObj.x=(LONG)(ptPlayer.x+nMinimapSize*xDistance/10);
		ptObj.y=(LONG)(ptPlayer.y+nMinimapSize*yDistance/10);
		ptLine.x=(LONG)(ptPlayer.x+nMinimapSize*xDistance/15);
		ptLine.y=(LONG)(ptPlayer.y+nMinimapSize*yDistance/15);
		if (pMinimapObjectCell) {
			DrawAutomapNode(pMinimapObjectCell,ptObj.x,ptObj.y,color);
		} else {
			wchar_t temp[8]={0xff,'c','0'+color,'*',0};
			DrawCenterText(6, temp, ptObj.x, ptObj.y, color);
		}
		d2gfx_DrawLine(ptPlayer.x,ptPlayer.y,ptLine.x,ptLine.y,color,-1);
	}
}
static void drawAreaRectOnMap(AreaRectInfo *pInfo, int dwColour) {
	POINT tl, tr, bl, br;
	tile2map(&tl,		 pInfo->tileX,					pInfo->tileY);
	tile2map(&tr,		 pInfo->tileX+pInfo->tileW,	pInfo->tileY);
	tile2map(&bl,	 pInfo->tileX,					pInfo->tileY+pInfo->tileH);
	tile2map(&br,   pInfo->tileX+pInfo->tileW,	pInfo->tileY+pInfo->tileH);
	d2gfx_DrawLine(tl.x,		tl.y,	tr.x,	 tr.y,		dwColour, -1);
	d2gfx_DrawLine(bl.x,	bl.y, br.x, br.y,	dwColour, -1);
	d2gfx_DrawLine(tl.x,		tl.y,	bl.x,  bl.y,	dwColour, -1);
	d2gfx_DrawLine(tr.x,	tr.y,	br.x, br.y,	dwColour, -1);
}
static void drawAreaRectOnScreen(AreaRectInfo *pInfo, int dwColour) {
	POINT tl, tr, bl, br;
	tile2draw(&tl,		 pInfo->tileX,					pInfo->tileY);
	tile2draw(&tr,		 pInfo->tileX+pInfo->tileW,	pInfo->tileY);
	tile2draw(&bl,	 pInfo->tileX,					pInfo->tileY+pInfo->tileH);
	tile2draw(&br,   pInfo->tileX+pInfo->tileW,	pInfo->tileY+pInfo->tileH);
	d2gfx_DrawLine(tl.x-screenDrawX,tl.y-screenDrawY,tr.x-screenDrawX,tr.y-screenDrawY,dwColour,-1);
	d2gfx_DrawLine(bl.x-screenDrawX,bl.y-screenDrawY,br.x-screenDrawX,br.y-screenDrawY,dwColour,-1);
	d2gfx_DrawLine(tl.x-screenDrawX,tl.y-screenDrawY,bl.x-screenDrawX,bl.y-screenDrawY,dwColour,-1);
	d2gfx_DrawLine(tr.x-screenDrawX,tr.y-screenDrawY,br.x-screenDrawX,br.y-screenDrawY,dwColour,-1);
}
void drawActiveRectAreaOnMinimap() {
	AreaRectData *pAreaRectData = PLAYER->pMonPath->pAreaRectData;
	for (int i=0;i<pAreaRectData->nearbyRectCount;i++) {
		drawAreaRectOnMap(pAreaRectData->paDataNear[i]->pAreaRectInfo,nAutomapActiveRectColors);
	}
}
void drawActiveRectAreaOnScreen() {
	AreaRectData *pAreaRectData = PLAYER->pMonPath->pAreaRectData;
	for (int i=0;i<pAreaRectData->nearbyRectCount;i++) {
		drawAreaRectOnScreen(pAreaRectData->paDataNear[i]->pAreaRectInfo,dwDrawUnitCount&8?nAutomapActiveRectColors:0x10);
	}
}
void drawScreenAreaOnMinimap() {
	int sx=d2client_GetScreenDrawX(),sy=d2client_GetScreenDrawY();
	POINT ptTopLeft, ptTopRight, ptBottomLeft, ptBottomRight;
	draw2map(&ptTopLeft,sx,sy);
	draw2map(&ptTopRight,sx+SCREENWIDTH,sy);
	draw2map(&ptBottomLeft,sx,sy+SCREENHEIGHT-48);
	draw2map(&ptBottomRight,sx+SCREENWIDTH,sy+SCREENHEIGHT-48);
	d2gfx_DrawLine(ptTopLeft.x,		ptTopLeft.y,	ptTopRight.x,		ptTopRight.y,	 nAutomapScreenColors, -1);
	d2gfx_DrawLine(ptBottomLeft.x,	ptBottomLeft.y,	ptBottomRight.x,	ptBottomRight.y, nAutomapScreenColors, -1);
	d2gfx_DrawLine(ptTopLeft.x,		ptTopLeft.y,	ptBottomLeft.x,		ptBottomLeft.y,  nAutomapScreenColors, -1);
	d2gfx_DrawLine(ptTopRight.x,	ptTopRight.y,	ptBottomRight.x,	ptBottomRight.y, nAutomapScreenColors, -1);
}
void MinimapPatch() {
	if (dwRevealMapMs&&dwCurMs>dwRevealMapMs) {
		dwRevealMapMs=0;
		if (dwAutoRevealMode>=3) RevealCurAct();
		else {
			int mode=dwAutoRevealMode;
			if (dwCurrentLevel==Level_RogueEncampment||dwCurrentLevel==Level_TheSecretCowLevel) mode=2;
			DrlgLevel *pDrlgLevel = PLAYER->pMonPath->pAreaRectData->pAreaRectInfo->pDrlgLevel;
			if (pDrlgLevel) RevealAutomapLevel(pDrlgLevel,mode);
		}
	}
	if (tAutomapReadyRect.isOn) {
		AreaRectData *pAreaRectData = ACT->pAreaRectData;
		while (pAreaRectData && pAreaRectData->pAreaRectInfo) {
			drawAreaRectOnMap(pAreaRectData->pAreaRectInfo, nAutomapReadyRectColors);
			pAreaRectData = pAreaRectData->pNext;	
		}
	}
	if (tAutomapActiveRect.isOn) drawActiveRectAreaOnMinimap();
	if (tAutomapScreenArea.isOn) drawScreenAreaOnMinimap();
	if (tMinimapDirectLineToggle.isOn) DrawMinimapPoint();
	AutoRouteDrawMinimapPath();
}
void __declspec(naked) MinimapPatch_ASM() {
	__asm {
		call MinimapPatch
		mov ecx, d2client_pAutomapLayer
		mov ecx, [ecx]
		ret
	}
}
#define MAPSHAKEX (*d2client_pMapShakeX)
#define MAPSHAKEY (*d2client_pMapShakeY)
#define AUTOMAPPOS (*d2client_pAutomapPos)
/*通过屏幕震动实现卷屏 ,需要ShakeScreenPatch*/
static int scroll_dx,scroll_dy;
static int isOn=0,nSrollIdx=0;
void InitScrollLock(BOOL fInit) {isOn=0;scroll_dx=0;scroll_dy=0;nSrollIdx=0;}
void SetScrollMapPos(int dx, int dy) {
	if (*d2client_pAutomapOn) {AUTOMAPPOS.x+=dx;AUTOMAPPOS.y+=dy;}
	else {scroll_dx+=dx;scroll_dy+=dy;}
}
int MiniMapScroll(int id) {int *p=dwMiniMapScrollOffset[id];SetScrollMapPos(p[0],p[1]);return 0;}
int CenterMap() {scroll_dx=0;scroll_dy=0;return 0;}
void __stdcall ShakeScreenPatch(int *pPosX, int *pPosY) {
	d2client_CalcShake(pPosX, pPosY);
	if (tScreenshake.isOn) {MAPSHAKEX=0;MAPSHAKEY=0;}
	if (tScreenshake.isOn||*d2client_pIsMapShakeOn==0) {MAPSHAKEX+=scroll_dx;MAPSHAKEY+=scroll_dy;}
	*pPosX+=MAPSHAKEX;*pPosY+=MAPSHAKEY;
}
int ScrollScreenEnd(int idx) {
	if (nScrollMode==3) {isOn=0;scroll_dx=0;scroll_dy=0;tAutomapScreenArea.isOn=0;}
	return 0;
}
int ScrollScreenStart(int idx) {
	if (nScrollMode>3) nScrollMode=3;
	switch ( nScrollMode ){
		case 1:isOn=1;break; //permanent scroll
		case 2:isOn=idx==nSrollIdx?!isOn:1;break; //switch
		case 3:isOn=1;break; //press and release
	}
	nSrollIdx=idx;
	if (isOn) {
		scroll_dx=dwScreenScrollOffset[idx][0];
		scroll_dy=dwScreenScrollOffset[idx][1];
	} else {scroll_dx=0;scroll_dy=0;}
	tAutomapScreenArea.isOn=isOn;
	return 0;
}
void DrawScrollOffset(wchar_t* wbuf ,int posy){
	if (isOn){
		wsprintfW(wbuf, L"[%d,%d]",scroll_dx,scroll_dy);
		drawBgTextLeft(wbuf,(*d2client_pInfoPositionX)-16,posy,0,0x10);
	}
}

int NextMapTarget();
void __fastcall recvObjectState(char *packet) {
	if (packet[0]!=0x0e) return;
	if (packet[1]!=2) return;
	int id=*(int *)(packet+2);
	int mode=*(int *)(packet+8);
	if (!mode) return;
	UnitAny *pUnit=d2client_GetUnitFromId(id,UNITNO_OBJECT);if (!pUnit) return;
	switch (pUnit->dwTxtFileNo) {
		case 392:case 393:case 394:case 395:case 396://seal
			if (mode) NextMapTarget();
			break;
	}
}
/*
	d2client_AF580: E8 CB B1 F7 FF     call d2client_2A750(2 args)
*/
void __declspec(naked) RecvPacket0EPatch_ASM() {
	__asm {
		pushad
		mov ecx, eax
		call recvObjectState
		popad
		jmp d2client_setState_2A750
	}
}
