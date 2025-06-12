/*
	Coordinate coversion
	tileX=unitX*5	
	tileY=unitY*5
	drawX=(unitX-unitY)*16
	drawY=(unitX+unitY)*8
*/
#ifndef D2_STRUCTS_H
#define D2_STRUCTS_H

struct UnitAny;
struct World;
struct PlayerData;
struct MonsterData;
struct ObjectData;
struct MissileData;
struct ItemData;
struct UnitInventory ;

struct CharStatsTxt;
struct TxtField;
struct GfxCell;
struct CellFile;
struct CellContext;
struct PresetUnit;
struct AreaTile;
struct AreaRectData;
struct AreaRectInfo;
struct DrlgLevel;
struct DrlgMisc;
struct DrlgAct;
struct AutomapNode;
struct AutomapLayer;
struct LevelDefTxt;
struct ObjectTxt;
struct LevelTxt ;
struct ItemTxt;
struct MonsterTxt;
struct SuperUniqueTxt;
struct SetItemTxt;
struct ItemProp;
struct UniqueItemTxt;
struct RuneWordTxt;


struct StaticPath;
struct DynamicPath;
struct NetClient;
struct RosterUnit;
struct PetUnit;
struct MonsterMinion;


struct TreasureClassItem ;
struct TreasureClass;
struct GameStructInfo;
struct D2MSG;
struct D2EditBox ;
struct QuestInfo;
struct Waypoint;
struct StatList;
struct SkillInfo;
struct Skill;

struct D2Seed {DWORD	dwLowSeed,dwHighSeed;};
struct DrlgAct {
	DWORD  _1[4];			//+00
	AreaRectData *pAreaRectData;		//+10
	DWORD	dwActNo;		//+14
	DWORD  _2[12];			//+18
	DrlgMisc *pDrlgMisc;	//+48
};

struct DrlgMisc {
	DWORD	_1[37];				//+00
	DWORD	dwStaffTombLvl;		//+94
	DWORD	_2[248];			//+98
	DWORD	*pMemPool;			//+478
	DrlgLevel *pLevelFirst;		//+47C
	DWORD	_3;					//+480
	DWORD	dwBossTombLvl;		//+484
};

struct DrlgLevel {
	DWORD	_1[4];			//+00
	AreaRectInfo *pAreaRectInfo; //+10 first
	DWORD	_2[102];		//+14
	DrlgLevel *pNext;		//+1AC
	DWORD	_3;				//+1B0
	DrlgMisc  *pDrlgMisc;	//+1B4 
	DWORD	_4[2];			//+1B8	
	DWORD	dwLvlType;		//+1C0
	DWORD	_5[3];			//+1C4			
	DWORD	dwLevelNo;		//+1D0
};
struct LvlPreset {
	DWORD dwDef;		
};
struct AreaRectInfo {
	DWORD   _1[2];			 //+00
	AreaRectInfo **pInfoNear; //+08
	DWORD   _2[5];			 //+0C
	LvlPreset *pLvlPreset;		 //+20
	AreaRectInfo  *pNext;		 //+24
	int dwAreaTiles;	 //+28
	int	nearbyRectCount;	 //+2C
	AreaRectData *pAreaRectData;	     //+30
	int	tileX;			 //+34
	int	tileY;			 //+38
	int	tileW;		 //+3C
	int	tileH;		 //+40
	DWORD   _4;				 //+44
	DWORD	dwPresetType;	 //+48
	AreaTile *pAreaTiles;	 //+4C
	DWORD   _5[2];			 //+50
	DrlgLevel *pDrlgLevel;	 //+58
	PresetUnit *pPresetUnits;//+5C	
};
struct AreaRectTile {
	int xPixel; //+00
	int yPixel; //+04
	int xi; //+08
	int yi; //+0C
	int off10; //+10
	int flags; //+14
	int off18[6]; //+18
};
struct AreaRectTiles {
	int off0,off4; //+00
	AreaRectTile *tiles; //+08
	int count; //+0C
};
struct AreaRectBitmap {
	int unitX,unitY,unitW,unitH; //+00
	int tileX,tileY,tileW,tileH; //+10
	unsigned short *bitmap;
};
struct AreaRectData {
	AreaRectData **paDataNear; //+00
	DWORD off04;			 //+04				
	AreaRectTiles *tiles;			 //+08
	DWORD off0C;			 //+0C
	AreaRectInfo *pAreaRectInfo;		 //+10
	DWORD	off14,off18;			 //+14
	UnitAny *off1C; //+1C ->UnitAny offE0
	AreaRectBitmap *bitmap; //+20
	int	nearbyRectCount;	 //+24
	int off28[2]; //+28
	int off30; //+30
	char off34; //+34 =4
	char off35[11]; //+35
	int off40[3]; //+40
	int unitX,unitY,unitW,unitH; //+4C
	int tileX,tileY,tileW,tileH; //+5C
	int off6C; //+6C
	int off70; //+70
	UnitAny *pUnit; //+74 first
	int off78; //+78
	AreaRectData *pNext;		 //+7C
};

struct AreaTile {
	AreaRectInfo *pAreaRectInfo;	//+00
	AreaTile  *pNext;	//+04
	DWORD	  _1[2];	//+08
	DWORD	  *pNum;	//+10
};

struct PresetUnit {
	DWORD	_1;			 //+00
	DWORD	dwTxtFileNo; //+04
	DWORD	unitDx;		 //+08
	PresetUnit *pNext;   //+0C
	DWORD	_2;			 //+10
	DWORD	dwUnitType;  //+14
	DWORD	unitDy;		 //+18
};

struct AutomapNode {
	DWORD	fSaved;		//+00
	WORD	wCellNo;	//+04
	WORD	drawXDiv10;		//+06
	WORD	drawYDiv10;		//+08
	WORD	wWeight;	//+0A = -1, 0, 1
	AutomapNode *pLess; //+0C
	AutomapNode *pMore; //+10
};

struct AutomapLayer {
	DWORD	dwLayerNo;		//+00
	DWORD	fSaved;			//+04
	AutomapNode	*pFloors;	//+08
	AutomapNode *pWalls;	//+0C
	AutomapNode *pObjects;	//+10
	AutomapNode *pExtras;	//+14
	AutomapLayer *pNext;	//+18
};


struct GfxCell {
	DWORD	dwFlags;	//+00
	DWORD	dwWidth;	//+04
	DWORD	dwHeight;	//+08
	DWORD	dwXoffs;	//+0c
	DWORD	dwYoffs;	//+10
	DWORD	_2;
	DWORD	dwParent;	//+18
	DWORD	dwLength;	//+1c
	BYTE	nCols[1];	//+20
};

struct CellFile {
	DWORD	dwVersion;		//+00
	WORD dwFlags;//+04
	BYTE mylastcol;
	BYTE mytabno;
	DWORD	eFormat;		//+08
	DWORD	dwTermination;	//+0C
	DWORD	dwNumdirs;		//+10
	DWORD	dwNumCells;		//+14
	GfxCell	*pCells[1];		//+18
};

struct CellContext { //size=0x48
	DWORD		dwCellNo;		//+00
	DWORD		dwDirNo;		//+04  (0-63, other values asplode even when file has more dirs) 
	DWORD		dwDirection;	//+08
	int		uk_0C;			//+0C
	int		uk_10[4];			//+10
	int uk_20[3]; //+20
	char *invName; //+2C
	int uk_30; //+30
	CellFile	*pCellFile ;	//+34
	int uk_38; //+38
	void *uk_3C; //+3C
	int	uk_40[2];//+40
};

struct NetClient {
	int	clientID;					//+00
	char uk1[0x06];					//+04
	union {
		WORD	flag;//+0A 0x4:isHardCoreGame 0x20:expansion
		struct {
			char charStat;
			char progress;
		};
	};
	char uk2; //+0C
	char name[16]; //+0D
	char account[16]; //+1D
	char uk3[0x13F]; //+2D
	int unitType; //+16C or act?
	int unitId; //+170
	UnitAny *player; //+174
	int off178; //+178
	char*	savefile;	//+17C
	int	finalSize;//+180
	int	counter;//+184
	int	currentSize;//+188
	char uk4[0x1C];//+18C
	World* world;	//+1A8
	int off1AC,off1B0; //+1AC
	AreaRectData *pAreaRectData; //+1B4
	int off1B8[188]; //+1B8
	NetClient *ptNextClient; //+4A8
};

struct UnitAny {
	int	dwUnitType;		//+00
	int	dwTxtFileNo;	//+04
	DWORD	*pMemPool ;		//+08	not used, always NULL 
	int	dwUnitId;		//+0C
	int	dwMode;			//+10
			/*mode index from   PlrMode.txt  MonMode.txt  ObjMode.txt  
					Missiles, Items and VisTiles have their own modes (for missiles this holds the collision type) 
					eItemModes 3=on the floor
			*/
	union {
		ItemData *pItemData; 
		PlayerData *pPlayerData; 
		MonsterData *pMonsterData; 
		ObjectData *pObjectData; 
		MissileData *pMissileData; 
	};	//+14
	int	dwAct;			//+18
	DrlgAct *pDrlgAct;		//+1C
	D2Seed	seed;			//+20	
	DWORD	dwInitSeed ;	//+28
	union {
		StaticPath	*pItemPath;	//+2C	(2:Objects, 5:VisTiles, 4:Items) 
    DynamicPath	*pMonPath;	//+2C	(0:Players, 1:Monsters, 3:Missiles) 
	};
	DWORD	_1[11];	//+30
	StatList *pStatList;	//+5C	
	UnitInventory *pInventory; //+60
	int	off64[2];		//+64
	short off6C,off6E; //+6C
	int	off70[4];		//+70
	World *world; //+80
	int	off80[3];		//+84
	int	off90;		//+90
	int	dwOwnerType;	//+94
	int	dwOwnerId;		//+98
	int	_3[3];		//+9C
	SkillInfo *pSkill;		//+A8
	int	_4[6];			//+AC
	DWORD	dwFlags1;		//+C4
	DWORD	dwFlags2;		//+C8
	int	offCC;			//+CC
	int	clientId;			//+D0
	int	offD4[3];			//+D4
	UnitAny *offE0; //+E0 ->AreaRectData+1C off1C
	UnitAny *pHashNext;		//+E4
	UnitAny *pRectNext;		//+E8
};

struct PvPInfo {			//size = 0x0C
	int	dwUnitId;		//+00
	DWORD	dwFlag;			//+04	0000 正常 0001 开锁 0010 静音 0100  缄默 1000 敌对
	PvPInfo *pNext;			//+08
};
struct MonsterMinion {		//随从信息,一般都空
	int	dwUnitId;		//+00
	MonsterMinion *pNext;	//+04
};
struct RosterUnit { 
   char		szName[16];			//0x00 
   int	dwUnitId;			//0x10 
   int	dwPartyLife;		//0x14 
   int	dwKills;			//0x18 ?
   int	dwClassId;			//0x1C 
   WORD		wLevel;				//0x20 
   WORD		wPartyId;			//0x22 
   int	dwLevelNo;			//0x24 
   int	dwPosX;				//0x28 
   int	dwPosY;				//0x2C 
   DWORD	dwPartyFlags;		//0x30 
   PvPInfo	**pPvPInfo;			//0x34 
   MonsterMinion **pMinon;		//0x38 
   DWORD	_6[10];				//0x3C 
   WORD		_7;					//0x64 
   char		szName2[16];		//0x66 
   WORD		_8;					//0x76 
   DWORD	_9[2];				//0x78 
   RosterUnit * pNext;			//0x80 
};
struct PetUnit {
	int	dwTxtFileNo;		//+00 对应monstats.tx的hcidx
	int	dwPetType;			//+04 对应pettype.txt
	int	dwUnitId;			//+08
	int	dwOwnerId;			//+0C 主人GUID
	DWORD	dwFlag1;			//+10
	int	dwOffsetX;			//+14 不确定
	int	dwOffsetY;			//+18 不确定
	int	dwHpPercent;		//+1C
	int	dwOwerType;			//+20  
	DWORD	dwInitSeed;			//+24  雇佣时生成，不再变化
	WORD	wLocaleTxtNo;		//+28  名字索引 for pet
	BYTE	_1[6];				//+2A
	PetUnit *pNext;				//+30
};
struct StatEx {			//size 8
	WORD wParam;	
	WORD wStatId;		//statNo from ItemStatCost.txt
	DWORD dwStatValue;
};
struct Stat{
	StatEx	*pStats;
	WORD	wStats;			
	WORD	wSizeInBits;
};
// StatList 和StatListEx多态结构，通过dwListFlag区分 , 0x80000000为StatListEx
// sizeof(StatListEx) = 0x64  sizeof(StatList) = 0x40  
struct StatList {				
	DWORD	pMemPool;			//+00	always NULL
	UnitAny	*pUnit;				//+04	the unit to which the list is attached   for StatListEx & item = item owner
	int	dwOwnerType;		//+08
	int	dwOwnerId;			//+0C
	DWORD	dwListFlag;			//+10
	DWORD	dwStateNo;			//+14
	DWORD	dwExpireFrame;		//+18	the frame at which the state expires 
	int	dwSkillNo;			//+1C	Id of the skill that created the stat list
	int	dwSkillLvl;			//+20	level of the skill that created the stat list 
	Stat	sBaseStat ;			//+24
	StatList *pPrevList;		//+2C	previous list on this unit 
	StatList *pNextLis;			//+30	next list on this unit 
	StatList *pPrev;			//+34	previous list;  for StatListEx , if this one is owned by a item, this points to the item owners list
	union {
		StatList *pNextListEx;						   //next StatListEx
		void (__fastcall *func)(UnitAny* ,DWORD ,BOOL);//StatList  function to call when the list is removed 
	};							//+38
	StatList *pNext;			//+3C	next list  for StatListEx = MyLastList
	//only for StatListEx
	StatList *pMyStatList;		//+40
	UnitAny	 *pUnitOwner;		//+44	StatListEx owner
	Stat	sFullStat ;			//+48
	Stat	sModStat ;			//+50
	void	*StatFlags;			//+58
	void	*fCallback;			//+5C function to call by SetStat, AddStat when a fcallback stat changes
	void	*pGame;				//+60 on server
};
struct DynamicPath {
	WORD	wOffsetX;		//+00
	WORD	wUnitX;			//+02
	WORD	wOffsetY;		//+04
	WORD	wUnitY;			//+06
	int	drawX;		//+08
	int	drawY;		//+0C
	WORD	wTargetX;		//+10
	WORD	wTargetY;		//+12
	DWORD	_2[2];			//+14
	AreaRectData *pAreaRectData;		//+1C
	AreaRectData *pAreaRectDataUnk;	//+20
	DWORD	_3[3];			//+24
	UnitAny *pUnit;			//+30
	DWORD	dwFlags;		//+34
	DWORD	_4;				//+38
	DWORD	dwPathType;		//+3C
	DWORD	dwPrevPathType;	//+40
	DWORD	dwUnitSize;		//+44
	DWORD	_5[4];			//+48
	UnitAny *pTargetUnit;	//+58
	DWORD	dwTargetType;	//+5C
	DWORD	dwTargetId;		//+60
	BYTE	nDirection;		//+64
};
struct StaticPath {
	AreaRectData *pAreaRectData;		//+00
	int	drawX; //+04
	int	drawY; //+08
	int	unitX; //+0C pos for item in socket
	int	unitY; //+10
};

struct SkillTxt {
	WORD	wSkillId; //+00
	char flags1; //+02
	char flags2; //+03
	char flags3; //+04 0x10:passiveSkill 0x20:aura
	char flags4; //+05 1:canUseInTown 0x10:createMinion
};
struct SkillInfo {
	DWORD *pGame1C;			//+00
	Skill *pFirstSkill;		//+04
	Skill *pLeftSkill;		//+08
	Skill *pRightSkill;		//+0C
	Skill *pCurrentSkill;	//+10 can't swap weapon if not null
};
struct Skill {
	SkillTxt *pSkillInfo;			//+00
	Skill	*pNextSkill;	//+04
	DWORD	dwMode;			//+08
	DWORD	dwFlag0;		//+0C
	DWORD	_1[2];			//+10
	DWORD	dwTagets;		//+18
	DWORD	dwTargetType;	//+1C unit type
	DWORD	dwTargetId;		//+20 unit id
	DWORD	_2;				//+24
	DWORD	dwSkillLevel;	//+28
	DWORD	dwLevelBonus;	//+2C
	DWORD	dwQuality;		//+30  质量 ==> 回城书之类的数量
	DWORD	dwItemId;		//+34 item charge, -1 for player skill 
};
struct QuestInfo {
	unsigned short *quests;					//0x00
	DWORD _flag;
};
struct Waypoint {
	DWORD flags;					//0x00
};
struct PlayerData {
	char szName[16];				//+00   len of charname <= 15 
	QuestInfo *quests[3];		//+10
	Waypoint *waypoints[3];		//+1C
	int off28[2]; //+28
	int off30[24]; //+30
	int off90[3]; //+90
	NetClient* client; //+9C
};
struct MissileData{
	BYTE	_1[8];			//+00
	WORD	wActivateFrame;	//+08	Activate column from Missiles.txt 
	WORD	wLvl;			//+0A
	WORD	wSkillNo;		//+0C
	short	wTotalFrames;	//+0E	the number of frames before the missile vanishes from Missiles.txt 
	int	dwCurrentFrame;	//+10	the current traveling frame
	DWORD	dwMissileFlags;	//+14	this depends on the missile functions used 
	int	dwOwnerType;	//+18	unit type of the missile owner 
	int	dwOwnerId;		//+1C	global unique identifier of the missile owner 
	int	dwUnitType;		//+20	unit type of a target, for homing missiles 
	int	dwUnitId;		//+24	global unique identifier of a target, for homing missiles 
	DWORD	dwHomeStatus;	//+28	for homing missiles like Guided Arrow 
	struct {
		WORD	xPos;
		WORD	yPos;
	}	sCoordShort;
};

struct MonsterData {
	MonsterTxt	*pMonsterTxt;	//+00
	BYTE	nComponents[16];	//+04	Order: HD, TR, LG, RA, LA, RH, LH, SH, S1, S2, S3, S4, S5, S6, S7, S8
	WORD	wNameSeed;			//+14;
	union{
		BYTE	bTypeFlags;	//+16
		struct {
			BYTE fOther:1;		//set for some champs, uniques
			BYTE fUnique:1;		//super unique
			BYTE fChamp:1;		
			BYTE fBoss:1;		//unique monster ,usually boss
			BYTE fMinion:1;		
			BYTE fPoss:1;		//possessed
			BYTE fGhost:1;		//ghostly
			BYTE fMulti:1;		//multishotfiring
		};
	};
	BYTE	nLastMode;			//+17
	DWORD	dwDuriel;			//+18
	BYTE	anEnchants[9];		//+1C
	BYTE	_1;					//+25
	WORD	wUniqueNo;			//+26	hcIdx from superuniques.txt for superuniques 
	void	*pAiGeneral;		//+28
	wchar_t	*wszMonName;		//+2C	server side is pAiParams
	BYTE	_2[16];				//+30
	DWORD	dwNecroPet;			//+40
	BYTE	_3[16];				//+44
	DWORD	dwAiState;			//+54	this is used to tell monsters what special state has been set, this tells them they just got attacked etc
	DWORD	dwLevelNo;			//+58	the Id from levels.txt of the level they got spawned in 
	BYTE	nSummonerFlags;		//+5C	byte used only by the summoner
};
struct ItemData { 
	DWORD	dwQuality;			//+00 
	D2Seed	seed;				//+04
	int	dwOwnerId;			//+0C -1 when not owned by player, otherwise equal to player GUID 
	DWORD	dwFingerPrint;		//+10	initial spawning seed
	DWORD	dwCommandFlags;		//+14
	DWORD	dwItemFlags;		//+18 
	int	off1C;				//+1C 
	int	off20;				//+20 
	DWORD	ActionStamp;		//+24
	DWORD	dwFileIndex;		//+28 index from data files UniqueItems.txt, SetItems.txt, QualityItems.txt, LowQualityItems.txt
	DWORD	dwItemLevel;		//+2C	ILvl
	WORD	wItemFormat;		//+30
	WORD	wRarePrefix;		//+32
	WORD	wRareSuffix;		//+34
	WORD	wAutoPrefix;		//+36
	WORD	wMagicPrefix[3];	//+38 
	WORD	wMagicSuffix[3];	//+3E 
	BYTE	nBodyLocation;		//+44 Id from BodyLocs.txt;this field isn't always cleared , use D2Common.#11003 instead of checking this 
	BYTE	nItemLocation;		//+45 -1 equiped 0 Inventory , 3 cube , 4 stash  
	BYTE	_2[2];				//+46 
	BYTE	nEarLevel;			//+48
	BYTE	nInvGfxIdx;			//+49
	char	szPlayerName[16];	//+4A	//used for Ears and Personalized items 
	BYTE	_3[2];				//+5A
	UnitInventory *pOwnerInventory;		//0x5C for socketed items this points to the inventory of the parent item
	DWORD	_4;					//+60 
	UnitAny *pNextInvItem;		//+64	item filling the next socket, if pNodeOwnerInventory is set 
	BYTE	_11;				//+68 
	BYTE	nLocation;			//+69 Actual location, 0 ground, 1 cube/stash/inv,2 belt,3 body
}; 
struct EquippedGears { //size=0x10
	int uk0,uk4;
	char bodyLocs,count,uk_a[2]; //+08 bodyLocs=13 count=1
	UnitAny **pItems; //+0C size is bodyLocs*count*4
};
struct UnitInventory {  //size=0x40 64
	DWORD	dwInvStamp;			//+00 0x1020304, used to verify the inventory is valid 
	void	*pMemPool;			//+04 always NULL
	UnitAny *pOwner;			//+08 
	UnitAny *pFirstItem;		//+0C 
	UnitAny *pLastItem;			//+10 
	EquippedGears *pInvInfo;	//+14 list of pointers to equipped gear 
	DWORD	dwInvInfoCount;		//+18 count for above 
	DWORD	dwWeaponId;			//+1C
	UnitAny *pCursorItem;		//+20 points to self on items that aren't placed into sockets ,for player point to cursor item
	int	dwOwnerId;			//+24 
	int	dwFilledSockets;	//+28 
	int uk_2C; //+2C
	int uk_30[4]; //+30
};
struct TreasureClassItem {	//size = 0x1C 长度一定要凑足，不然使用时有问题
	DWORD _1[2];		//+00
	DWORD dwThingNo;	//+08
	int off0C; //+0C fTc:0x4
	DWORD _3[3];		//+10
};
struct TreasureClass {	//size = 0x2C
	WORD wGroup;		//+00
	WORD wLevel;		//+02
	int dwItemNums;	//+04
	DWORD _1[8];		//+08
	TreasureClassItem *pItems; //+28
};
struct D2MSG {
	HWND	hwnd;				//+00
	DWORD	dwMessage;			//+04
	int	wParam;				//+08
	union {
		DWORD lParam;
		struct {
			WORD xpos;
			WORD ypos;
		};
	};							//+0C
	DWORD	_1;					//+10
	int	hwnd2;				//+14
	DWORD	_2[2];				//+18
};
struct D2EditBox {
	DWORD	dwType;				//+00
	CellFile *pCellFile;		//+04
	DWORD   dwFlag;				//+08
	int	dwPosX;				//+0C   
	int	dwPosY;				//+10
	int	dwSizeX;			//+14
	int	dwSizeY;			//+18
	void (__fastcall *fnCallBack)(D2EditBox*); // +1C
	DWORD _3[7];				//+20
	D2EditBox *pNext;			//+3C
	DWORD   _4;					//+40
	int   dwOffsetY;			//+44
	union {
		DWORD   dwMaxLength;		//+48
		DWORD	dwScrollEntries;	//+48
	};
	DWORD   dwScrollPosition;	//+4C ?
	DWORD	_5;					//+50
	DWORD	dwSelectStart;		//+54
	DWORD	dwSelectEnd;		//+58
	wchar_t wszText[256];		//+5C
	DWORD	dwCursorPos;		//+25C
	DWORD	dwIsCloaked;		//+260
	DWORD _6[6]; //+264
	D2EditBox *pNext2; //+27c
};
struct Damage {
	DWORD	dwHitFlags;			//+00
	/*
		0x00000001 - HITFLAG_SRCDAMAGE_MISSILE 
		0x00000002 - HITFLAG_SUCCESSFUL_CRITICAL_HIT 
		0x00000010 - HITFLAG_FRIENDLY_FIRE 
		0x00000020 - HITFLAG_SRCDAMAGE_MELEE 
		0x00000100 - HITFLAG_BYPASS_UNDEAD 
		0x00000200 - HITFLAG_BYPASS_DEMONS 
		0x00000400 - HITFLAG_BYPASS_BEASTS 
	*/

	DWORD	dwResultFlags;			//+04
	/*
		0x00000001 - RESULTFLAG_HIT 
		0x00000002 - RESULTFLAG_DEATH 
		0x00000004 - RESULTFLAG_GETHIT 
		0x00000008 - RESULTFLAG_KNOCKBACK 
		0x00000010 - RESULTFLAG_BLOCK 
		0x00000020 - RESULTFLAG_DISALLOW_EVENTS 
		0x00000100 - RESULTFLAG_DODGE 
		0x00000200 - RESULTFLAG_AVOID 
		0x00002000 - RESULTFLAG_SOFTHIT 
		0x00008000 - RESULTFLAG_WEAPONBLOCK 
	*/
	DWORD	dwPhysicalDamage;		//+08
	DWORD	dwEnhancedDamagePercent;//+0C
	DWORD	dwFireDamage;			//+10
	DWORD	dwBurnDamage;			//+14
	DWORD	dwBurnLength;			//+18
	DWORD	dwLightningDamage;		//+1C
	DWORD	dwMagicDamage;			//+20
	DWORD	dwColdDamage;			//+24
	DWORD	dwPoisonDamage;			//+28
	DWORD	dwPoisonLength;			//+2C
	DWORD	dwColdLength;			//+30
	DWORD	dwFreezeLength;			//+34
	DWORD	dwLifeSteal;			//+38
	DWORD	dwManaSteal;			//+3C
	DWORD	dwStaminaSteal;			//+40
	DWORD	dwStunLength;			//+44
	DWORD	dwAbsorbedLife;			//+48
	DWORD	dwDamageTotal;			//+4C
	DWORD	_1;						//+50	
	DWORD	dwPiercePct;			//+54	chance to pierce target for missiles 
	DWORD	dwDamageRate;			//+58
	DWORD	_2;						//+5C
	DWORD	dwHitClass;				//+60
	BYTE	fHitClassActiveSet;		//+64
	BYTE	nType;					//+65
	WORD	_3;						//+66
	DWORD	dwConvPercent;			//+68
};


struct SuperUniqueTxt {		//size = 0x34
	WORD	wUniqueNo;		//+00
	WORD	wLocaleTxtNo;	//+02
	DWORD	wMonIdx;		//+04	index in monstat.txt
	DWORD	dwUniqueNo;		//+08
	DWORD	dwMod1;			//+0C
	DWORD	dwMod2;			//+10
	DWORD	dwMod3;			//+14
	DWORD	dwMonSound;		//+18
	DWORD	dwMinGrp;		//+1C
	DWORD	dwMaxGrp;		//+20
	DWORD	_1;				//+24
	BYTE	Utrans[4];		//+28
	WORD	Tcs[4];			//+2C

};

struct ObjectTxt {				//size = 0x1C0
	char	szName[64];			//+00
	wchar_t wszName[64];		//+40
	char	szToken[3];			//+C0
	BYTE	nSpwanMax;			//+C3
	BYTE	nSelectable[8];		//+C4
	WORD	wTrapProp;			//+CC
	BYTE	_1[2];				//+CE
	DWORD	dwSizeX;			//+D0
	DWORD	dwSizeY;			//+D4
	DWORD	dwFrameCnt[8];		//+D8	需要右移8位
	WORD	wFrameDelta[8];		//+F8
	BYTE	nCycleAnim[8];		//+108
	BYTE	nLit[8];			//+110
	BYTE	nBlockLight[8];		//+118
	BYTE	nHasCollision[8];	//+120
	BYTE	fAttackable;		//+128
	BYTE	nStrart[8];			//+129
	BYTE	nOrderFlag[8];		//+131
	BYTE	nEnvEffect;			//+139
	BYTE	nIsDoor;			//+13A
	BYTE	nBlocksVis;			//+13B
	BYTE	nOrientation;		//+13C
	BYTE	nPreOperate;		//+13D
	BYTE	nTrans;				//+13E
	BYTE	nMode[8];			//+13F
	BYTE	_2;					//+147
	DWORD	dwXoffset;			//+148
	DWORD	dwYoffset;			//+14C
	BYTE	nDraw;				//+150
	BYTE	nHd;				//+151
	BYTE	nTR;				//+152
	BYTE	nLG;				//+153
	BYTE	nRA;				//+154
	BYTE	nLA;				//+155
	BYTE	nRH;				//+156
	BYTE	nLH;				//+157
	BYTE	nSH;				//+158
	BYTE	nS[8];				//+159	S1-S8
	BYTE	nTotalPieces;		//+161
	BYTE	nXSpace;			//+162
	BYTE	nYSpace;			//+163
	BYTE	nRed;				//+164
	BYTE	nGreen;				//+165
	BYTE	nBlue;				//+166
	BYTE	nSubClass;			//+167
	DWORD	dwNameOffset;		//+168
	BYTE	_3[2];				//+16C
	BYTE	nOperateRange;		//+16E
	BYTE	nShrineFnc;			//+16F
	BYTE	nAct;				//+170
	BYTE	_4[7];				//+171
	DWORD	dwParm[8];			//+178
	BYTE	_5[4];				//+198
	BYTE	nDamage;			//+19C
	BYTE	_6[3];				//+19D
	DWORD	dwLeft;				//+1A0
	DWORD	dwTop;				//+1A4
	DWORD	dwWidth;			//+1A8
	DWORD	dwHeight;			//+1AC
	BYTE	_7;					//+1B0
	BYTE	nInitFn;			//+1B1
	BYTE	nPopulateFn;		//+1B2
	BYTE	nOperateFn;			//+1B3
	BYTE	nClientFn;			//+1B4
	BYTE	_8[7];				//+1B5
	DWORD	nAutoMap;			//+1BC

};

struct LevelTxt	{				//size = 0x220
	WORD	wLevelNo;			//+00
	BYTE	nPal;				//+02
	BYTE	nAct;				//+03
	BYTE off4,weather,off6,off7; //+04
	DWORD	off08;				//+08
	DWORD	dwWarpDist;			//+0C
	WORD	nMonLv[2][3];		//+10	Area Level
	DWORD	nMonDen[3];			//+1C 
	BYTE	monumin[3];			//+28
	BYTE	monumax[3];			//+2B
	BYTE	MonWndr;			//+2E
	BYTE	MonSpcWalk;			//+2F
	DWORD	_2[49];				//+30
	BYTE	_3;					//+F4
	char	szName[40];			//+F5
	char	szEntranceText[40];	//+11D
	char	szLvlDesc[40];		//+145
	BYTE	_4;					//+16D	
	wchar_t wszName[40];		//+16E
	wchar_t wszEntranceText[40];//+1BE
	BYTE	_5[2];				//+20E
	DWORD	wSoundEnv;			//+210
	DWORD	_6[2];				//+214
	WORD	wThemes;			//+21C
	WORD	_7;					//+21E
};

struct LevelDefTxt {			//size = 0x9C
	DWORD	dwQuestFlag;		//+00
	DWORD	dwQuestFlagEx;		//+04
	DWORD	dwLayerNo;			//+08
	DWORD	dwSizeX[3];			//+0C
	DWORD	dwSizeY[3];			//+18
	DWORD	dwOffsetX;			//+24
	DWORD	dwOffsetY;			//+28
	DWORD	dwDepend;			//+2C
	DWORD	dwDrlgType;			//+30 1=随机的生成  2=固定地图 3=随机生成，但地图大小不做改变
	DWORD	dwLevelType;		//+34
	DWORD	dwSubType;			//+38
	DWORD	dwSubTheme;			//+3C
	DWORD	dwSubWaypoint;		//+40
	DWORD	dwSubShrine;		//+44
	DWORD	dwVis[8];			//+48
	DWORD	dwWarp[8];			//+68
	BYTE	nIntensity;			//+88
	BYTE	nRed;				//+89
	BYTE	nGreen;				//+8A
	BYTE	nBlue;				//+8B
	DWORD	dwPortal;			//+8C
	DWORD	dwPosition;			//+90
	DWORD	dwSaveMonsters;		//+94
	DWORD	dwIsInside;			//+98
};

struct ItemTxt {				//size = 0x1A8
	char szFlippyfile[32];		//+00
	char szInvfile[96];			//+20 ==>three names 32*3
	union{
		DWORD dwCode[5];
		char szCode[20]; //itemCode, classCodes[3], class
	};							//+80
	BYTE nPotionflag;			//+94 //potion cube ,book 
	BYTE _1[39];				//+95  have 2byte only misc have ,and heal potion is diff
	char szMisc1[4];			//+BC
	char szWeap1[8];			//+C0
	char szMisc2[4];			//+C8
	int dwMinDef;				//+CC
	int dwMaxDef;				//+D0
	DWORD dwGambleCost;			//+D4
	int  dwSpeed;				//+D8
	int  dwBitfield1;			//+DC
	DWORD dwCost;				//+E0
	DWORD _2[2];				//+E4
	BYTE nContainerType;		//+EC
	BYTE _3[7];					//+ED
	WORD wLocaleTxtNo;			//+F4
	BYTE nVersion;				//+F6
	BYTE _4[5];					//+F7
	BYTE nRarity;				//+FC
	BYTE nQLevel;				//+FD
	BYTE nMindam;				//+FE one hand
	BYTE nMaxdam;				//+FF
	BYTE _5[2];					//+100
	BYTE nMindam2;				//+102 two hand
	BYTE nMaxdam2;				//+103
	BYTE nRangeAdder;			//+104
	BYTE _6;					//+105
	WORD wStrBonus;				//+106
	WORD wDexBonus;				//+108
	WORD wReqstr;				//+10A
	WORD wReqDex;				//+10C
	BYTE nAbsorbs;				//+10E
	BYTE nInvwidth;				//+10F
	BYTE nInvheight;			//+110
	BYTE _8;					//+111
	BYTE nDurability;			//+112
	BYTE _8a[2];				//+113
	BYTE nComponent;			//+115
	BYTE _8b[8];				//+116
	BYTE nType;					//+11E
	BYTE _3a[11];				//+11F
	BYTE fQuest;				//+12A
	BYTE fDiffCheck;			//+12B
	BYTE _9[12];				//+12C
	BYTE nSocket;				//+138
	BYTE _10[6];				//+139
	BYTE nLevelReq;				//+13F
	BYTE nMagicLvl;				//+140
	BYTE off141,off142;				//+141
	BYTE compact;				//+143 isCompact?
	int off144[25]; //+144
};



struct ItemProp { //size=0x10
	int PropId;
	int Param;
	int MinVal;
	int MaxVal;
};

struct UniqueItemTxt {  //size=0x14C
	WORD dwIndex ;			//+00
	char  szDesc[32];		//+02
	WORD  wLocaleTxtNo;		//+22
	char  szUnk1[4];		//+24 version
	char  szCode[4];		//+28
	DWORD	_1[2];			//+2C
	WORD  wQlvl;			//+34
	WORD  wReqlvl;			//+36
	DWORD	_2[17];			//+38
	DWORD dwCostMult;		//+7C
	DWORD dwCostAdd;		//+80
	DWORD	_3[2];			//+84	
	ItemProp Prop[12] ;		//+8C
};
struct SetItemTxt{			//size = 0x1B8
	WORD dwIndex ;			//+00
	char  szDesc[32];		//+02
	char  szUnk1[2];		//+22 version
	WORD  wLocaleTxtNo;		//+24
	WORD  wUnk;				//+26
	char  szCode[4];		//+28
	DWORD  dwSetIdx;		//+2C
	WORD  wQlvl;			//+30
	WORD  wReqlvl;			//+32
	WORD  wRarity;			//+34
	WORD  _2 ;				//+36
	DWORD dwCostMult;		//+38
	DWORD dwCostAdd;		//+3C
	DWORD	_3[18];			//+40
	ItemProp Prop[9] ;		//+88
	int off118[40]; //+118
};

struct RuneWordTxt {	//size=0x120
	char szCode[64];	//+00
	char szName[64];	//+40
	WORD wComplete;		//+80
	WORD wLocaleTxtNo;	//+82
	WORD wServer;		//+84 only on battle.net server
	WORD wIType[6];		//+86 can be one of these types d2common_checkItemTypeFlags
	WORD wEType[3];		//+92 can't be these types d2common_checkItemTypeFlags
	int runeTxtId[6];	//+98 
	ItemProp Prop[7] ;	//+B0
};


struct ItemTypeTxt{  //size = 0xE4
	char szCode[8];		//+00
	BYTE _1[18];		//+08
	BYTE nMaxSock1;		//+1A
	BYTE nMaxSock25;	//+1B
	BYTE nMaxSock40;	//+1C
	BYTE _2[0xC7];		//+1D
};

struct ItemStatCostTxt {   //size = 0x144
	int dwStatNo;			//+00
	char flags04;//+04 bit0:send_other,bit1:signed,bit2:damagerelated,bit3:itemspecific,bit4:direct
	char flags05;//+05 bit1:updateanimrate,bit2:fmin(has minaccr),bit3:fcallback,bit4:saved,bit5:cvssigned
	char flags06,flags07;//+06
	char send_bits,send_param_bits; //+08
	char csvbits; //+0A only the first 16 stat for player save file stat
	char csvparam; //+0B all zeros
	int dwDivide;			//+0C all =1024 except 181-187=0
	int dwMultiply;		//+10
	int dwAdd;			//+14
	char valshift; //+18 stat6-11,216,217=8 others=0
	char save_bits,save_bits109,pad_1B; //+19
	int save_add,save_add109;		//+1C
	int save_param_bits,pad28; //+24
	int minaccr;			//+2C stat0-3,7=1, others=0
	BYTE nItemEvent[2];		//+30
	BYTE nItemEventFunc[2];	//+32	
	WORD wDescPriority;		//+34
	BYTE nDescFunc;			//+36
	BYTE nDescVal;			//+37
	WORD wDescStrpos;		//+38
	WORD wDescStrneg;		//+3A
	WORD wDescStr2;			//+3C
	WORD nDgrp;				//+3E
	BYTE nDgrpFunc;			//+40
	BYTE nDgrpVal;			//+41
	WORD wDgrpStrpos;		//+42
	WORD wDgrpStrneg;		//+44
	WORD wDgrpStr2;			//+46
	BYTE _3[252];			//+48
	// +54 ==>OP
};

struct PropertiesTxt {	//size = 0x2E
	WORD   wPropId ;	//+00
	BYTE   nSet[8] ;	//+02
	short  wVal[7];		//+0A
	BYTE   nFunc[8];	//+18
	short  wStatNo[7] ;	//+20
};


struct MonsterTxt {			//size = 0x1A8
	WORD	hcIdx ;			//+00
	WORD	hcIdx2;			//+02
	BYTE	_1[2];			//+04
	WORD	wLocaleTxtNo;	//+06
	WORD	wFlag;			//+08
	WORD	_2;				//+0A
	union{
		BYTE	nflag1; //+0C fBoss:0x40 fPrimeevil:0x80
		struct{
			BYTE _2a:6 ;
			BYTE fBoss:1 ;
			BYTE fPrimeevil:1 ;
		};
	};
	union{
		BYTE	nflag2; //+0D fNpc:1
		struct{
			BYTE fNpc:1 ;
		};
	};
	BYTE	flag3;			//+0E
	union{
		BYTE	nflag4; //+0F fInventory:1
		struct {
			BYTE fInventory:1 ;
		};
	};
	char   szCode[4];		//+10
	char	_3[30];			//+14
	WORD	velocity;		//+32
	WORD	run;			//+34
	BYTE	_4[24];			//+36
	BYTE	nTreat;			//+4E
	BYTE	Aidel[3];		//+4F
	BYTE	Aidst[3];		//+52
	BYTE	_5;				//+55
	WORD	Aip1[3];		//+56
	WORD	Aip2[3];		//+5C
	WORD	Aip3[3];		//+62
	WORD	Aip4[3];		//+68
	char	_6[24];			//+6E
	WORD	Tcs[3][4];		//+86
	BYTE	nTcQuestId;		//+9E
	BYTE	nTcQuestCp;		//+9F
	BYTE	Drain[3];		//+A0
	BYTE	ToBlock[3];		//+A3
	BYTE	nCrit;			//+A6
	BYTE	_7[3];			//+A7
	WORD	Level[3];		//+AA
	WORD	MinHp[3];		//+B0
	WORD	MaxHp[3];		//+B6
	WORD	Ac[3];			//+BC
	WORD	A1TH[3];		//+C2
	WORD	A2TH[3];		//+C8
	WORD	S1TH[3];		//+CE
	WORD	Exp[3];			//+D4
	WORD	A1MinD[3];		//+DA
	WORD	A1MaxD[3];		//+E0
	WORD	A2MinD[3];		//+E6
	WORD	A2MaxD[3];		//+EC
	WORD	S1MinD[3];		//+F2
	WORD	S1MaxD[3];		//+F8
	WORD	_8[3];			//+FE
	char	szDesc[48];		//+104
	char	_9[116];		//+134

};

struct SkillDescTxt {
	WORD	wSkillNo ;
	BYTE	nSkillPage;
	BYTE	nSkillRow;
	BYTE	nSkillColumn;
	BYTE	nListRow;
	BYTE	nListPool;
	BYTE	nIconCel;
	WORD	wLocaleTxtNo;
};


struct GameStructInfo{
	BYTE	_1[27];				//+00
	char	szGameName[24];		//+1B
	char	szGameServerIp[86];	//+33
	char	szAccountName[48];	//+89
	char	szCharName[24];		//+B9
	char	szRealmName[24];	//+D1
	BYTE	_2[258];			//+E9 
	union{
		BYTE nGameMode; //+1EB 
		struct
		{
			BYTE nCharStat:2;
			BYTE nHardcore:2;
			BYTE _unk1:1;
			BYTE nExpansion:1;
			BYTE nLadder:2;	
		};
	};
	BYTE	nReadyAct;			//+1EC  quest info 
	BYTE	_3[60];				//+1ED
	char	szServerVersion[24];//+229  battle net gameversion
	char	szGamePassword[24];	//+241
	char	szGameDescription[32];	//+259
};

#pragma pack(1)
struct WaypointMenuItem {
	BYTE areaId;
	BYTE	_1[3];	//+1
	BYTE enable;	//+4
};
#pragma pack(1)
struct KeyboardSetting {
/*
cmd table:
0   Character Screen 1   Inventory Screen 2   Party Screen 3   Message Log 4   Quest Log
5   Chat 6   Help Screen
7   Automap (Show/Hide) 8   Center Automap 9   Fade Automap 10  Party on Automap 11  Names on Automap
12  Skill Tree 13  Skill Speed Bar
14  Skill1 15  Skill2 16  Skill3 17  Skill4 18  Skill5 19  Skill6 20  Skill7 21  Skill8
22  Show Belt 23  Use Belt 1 24  Use Belt 2 25  Use Belt 3 26  Use Belt 4
27  Say Help 28  Say Follow me 29  Say This is for you 30  Say Thanks 31  Say Sorry
32  Say Bye 33  Say Now you die
34  Run (up/down) CONTROL 35  Toggle Run/Walk 36  Stand Still (up/down) SHIFT
37  Show Items (up/down) ALT 38  Clear Screen  39  Select Previous Skill 40  Select Next Skill
41  Clear Nessage 43  Show Portraits 44  Swap Weapons 45  Toggle MiniMap (Left/Right)
46  Skill9 47  Skill10 48  Skill11 49  Skill12 50  Skill13 51  Skill14 52  Skill15 53  Skill16
54  Hiring Screen 55  Say Retreat 56  ESC
*/
	int cmd; //+00
	short key; //+04
	short off6[2]; //+06 total size 10
};
struct D2AccountInfo {
	char off00[0x48];
	char accountName[16];
};
struct MonStatsBIN;
struct World10F4_18 {
	char uk_00[0x1C]; //+00
	int passClassic; //+1C
	char uk_20[0x68]; //+20
	int passExpansion; //+88
};	
struct World10F4 {
	int type; //+00 0x17:passClassic 0x24:passExpansion
	World *world; //+04
	char uk_08[0x10]; //+08
	World10F4_18 *uk_18; //+18
	int uk_1C; //+1C
	char uk_20[0xD4]; //+20
	World10F4 *next; //+F4
};
struct World {
	int worldID; //+00
	void *worldInfo; //+04 WorldInfo+4
	World *next; //+08
	int off0C; //+0C
	int off10,off14; //+10
	int off18; //+18 RtlLeaveCriticalSection
	int off1C; //+1C
	char off20[16]; //+20
	char off30[16];//+30
	int off40[4]; //+40
	int off50[4]; //+50
	char off60[13]; //+60
	char difficulty; //+6D 0,1,2
	char off6E[2]; //+6E
	int expansion; //+70	
	int off74; //+74
	short itemFormat; //+78 =65h
	short off7A; //+7A
	int off7C; //+7C
	int off80[2]; //+80
	NetClient* client;				//+88
	int off8C; //+8C
	int off90[4]; //+90
	int offA0[2]; //+A0
	int	frame;					//+A8
	int offAC; //+AC
	int offB0[3]; //+B0
	DrlgAct *mapAct[5];					//+BC
	int offD0[12]; //+D0
	int off100[960]; //+100
	int off1000[61]; //+1000
	World10F4 **off10F4; //+10F4 pass related
	int off10F8[2]; //+10F8
	int off1100[8]; //+1100
	UnitAny *playerHash[128]; //+1120
	UnitAny *monsterHash[128]; //+1320
	UnitAny *objectHash[128]; //+1520
	UnitAny *itemHash[128]; //+1720
	UnitAny *missileHash[128]; //+1920
	UnitAny *areaEntrances; //+1B20
	char uniqueDroppedBitmap[128]; //+1B24
};
struct ObjectData {
	ObjectTxt *pObjectTxt;		//+00
	BYTE nShrineNo,off05,off06,off07; //+04
	int off08,off0C; //+08
	int off10,off14,off18,off1C; //+10
	int off20,off24; //+20
	char owner[16]; //+28
};
struct MonStatsBIN { //size=0x1A8 (424)
	BYTE	uk1[0x6];		//+00
	WORD	monsterNameID;	//+06
	BYTE	uk2[0x168];		//+08
	WORD	skill[8];		//+170
	BYTE	skillArg[8];	//+180
	BYTE	uk3[0x20];		//+188
};							//+1A8
struct EquipmentLayout { //size=0x14
	int x0,x1,y0,y1;
	char w,h,uk[2];
};
struct InventoryBIN {// size=0xF0 (240) (source total : fct of dll)
	EquipmentLayout inv; //+00
	EquipmentLayout grid; //+14
	EquipmentLayout rArm; //+28
	EquipmentLayout torso; //+3C
	EquipmentLayout lArm; //+50
	EquipmentLayout head; //+64
	EquipmentLayout neck; //+78
	EquipmentLayout rHand; //+8C
	EquipmentLayout lHand; //+A0
	EquipmentLayout belt; //+B4
	EquipmentLayout feet; //+C8
	EquipmentLayout gloves; //+DC
};
struct TxtField {
	char *name; //+00 last one is "end"
	int type; //+04 2:int 4:char
	int arraySize; //+08 =0
	int offset; //+0C
	void *link; //+10 =0
};
struct AIParam {
	void*			control;            //+0x00
	void*			u_0x04;             //+0x04
	UnitAny*			target;				//+0x08
	void*			u_0x0C;             //+0x0C
	void*			u_0x10;             //+0x10
	int			distanceToTarget;   //+0x14
	int			engagedInCombat;    //+0x18
	MonStatsBIN*	ptMonStatsBIN;		//+0x1C
	void*			ptMonStats2BIN;		//+0x20
};
struct SpawnMon { //size 0x26
	World* game;//+00
	AreaRectData *pData;int off08,txt; 
	int mode,uid,x,y; //+10
	int off20;//+20
	short off24; 
};
/*mode: npc,mon:1 nec:8
off08: 0
off20: -1
off24: npc:0 mon:1,0x62 nec:0x42
*/
struct ExperienceTxt {
	int maxLvl[7],pad1C; //+00
	int expLvl[99][8]; //+20
};
struct CharStatsTxt { //size=0xC4=196
	short	nameU[0x10];		//+00
	char	name[0x10];			//+20
	char	baseSTR;			//+30
	char	baseDEX;			//+31
	char	baseENE;			//+32
	char	baseVIT;			//+33
	char	baseStamina;		//+34
	char	hpadd;				//+35
	char	percentStr;			//+36
	char	percentInt;			//+37
	char	percentDex;			//+38
	char	percentVit;			//+39
	char	manaRegen;			//+3A
	char	__3B;				//+3B
	int	toHitFactor;		//+3C
	char	walkVelocity;		//+40
	char	runVelocity;		//+41
	char	runDrain;			//+42
	char	lifePerLevel;		//+43
	char	staminaPerLevel;	//+44
	char	manaPerLevel;		//+45
	char	lifePerVitality;	//+46
	char	staminaPerVitality;	//+47
	char	manaPerMagic;		//+48
	char	blockFactor;		//+49
	short	__4A_startSkill;	//+4A
	int	baseWClass;			//+4C
	char	StatPerLevel;		//+50
	char	__51;				//+51
	short	StrAllSkills;		//+52
	short	StrSkillTab1;		//+54
	short	StrSkillTab2;		//+56
	short	StrSkillTab3;		//+58
	short	StrClassOnly;		//+5A
	int	item1;
	char	item1Loc;
	char	item1Count;
	short	item1Unknown;
	int	item2;
	char	item2Loc;
	char	item2Count;
	short	item2Unknown;
	int	item3;
	char	item3Loc;
	char	item3Count;
	short	item3Unknown;
	int	item4;
	char	item4Loc;
	char	item4Count;
	short	item4Unknown;
	int	item5;
	char	item5Loc;
	char	item5Count;
	short	item5Unknown;
    int	item6;
	char	item6Loc;
	char	item6Count;
	short	item6Unknown;
	int	item7;
	char	item7Loc;
	char	item7Count;
	short	item7Unknown;
	int	item8;
	char	item8Loc;
	char	item8Count;
	short	item8Unknown;
	int	item9;
	char	item9Loc;
	char	item9Count;
	short	item9Unknown;
	int	item10;
	char	item10Loc;
	char	item10Count;
	short	item10Unknown;
	char	unknown2[0x18];
};
struct D2DataTables {
	void * pPlayerClassTxt; //+0
	void * pPlayerClassLink; //+4
	void * pBodyLocsTxt; //+8
	void * pBodyLocsLink; //+C
	void * pStorePageTxt; //+10
	void * pStorePageLink; //+14
	void * pElemTypesTxt; //+18
	void * pElemTypesLink; //+1C
	void * pHitClassTxt; //+20
	void * pHitClassLink; //+24
	void * pMonModeTxt; //+28
	void * pMonModeLink; //+2C
	void * pPlrModeTxt; //+30
	void * pPlrModeLink; //+34
	void * pSkillCalcTxt; //+38
	void * pSkillCalcLink; //+3C
	void * pSkillCalcCache; //+40
	int nSkillCalcCache; //+44
	int nSkillCalcCacheDelta; //+48
	void * pSkillDescCalcCache; //+4C
	int nSkillDescCalcCache; //+50
	int nSkillDescCalcCacheDelta; //+54
	void * pMissCalcTxt; //+58
	void * pMissCalcLink; //+5C
	void * pMissCalcCache; //+60
	int nMissCalcCache; //+64
	int nMissCalcCacheDelta; //+68
	void * pSkillCodesBin; //+6C
	void * pSkillCodesLink; //+70
	void * pEventsTxt; //+74
	void * pEventsLink; //+78
	void * pCompCodesTxt; //+7C
	void * pCompCodesLink; //+80
	int nCompCodes; //+84
	void * pMonAiTxt; //+88
	void * pMonAiLink; //+8C
	int nMonAi; //+90
	void * pItemLink; //+94
	void * pItemCalcCache; //+98
	int nItemCalcCache; //+9C
	int nItemCalcCacheDelta; //+A0
	PropertiesTxt *pPropertiesTxt; //+A4
	void * pPropertiesLink; //+A8
	int nProperties; //+AC
	void * pRunesLink; //+B0
	void * pHireDescTxt; //+B4
	void * pHireDescLink; //+B8
	void * pStatesTxt; //+BC
	void * pStatesLink; //+C0
	int nStates; //+C4
	void * pStateMaskFirst; //+C8
	void * pStateMaskArray; //+CC
	int offD0[39]; //+D0
	void * pProgressiveStates; //+16C
	int nProgressiveStates; //+170
	void * pCurseStates; //+174
	int nCurseStates; //+178
	void * pTransformStates; //+17C
	int nTransformStates; //+180
	void * pActionStates; //+184
	int nActionStates; //+188
	void * pColorStates; //+18C
	int nColorStates; //+190
	void * pSoundCodesBin; //+194
	void * pSoundCodesLink; //+198
	int nSoundCodes; //+19C
	void * pHirelingsTxt; //+1A0
	int nHirelings; //+1A4
	void * HireNameListStart; //+1A8
	int off1AC[255]; //+1AC
	void * HireNameListEnd; //+5A8
	int off5AC[255]; //+5AC
	void * pNpcsTxt; //+9A8
	int nNpcs; //+9AC
	void * pColorsTxt; //+9B0
	void * pColorsLink; //+9B4
	void * pTreasureClassExLink; //+9B8
	void * pTreasureClassExTxt; //+9BC
	int nTreasureClassEx; //+9C0
	void * pChestTreasureClassList; //+9C4
	int off9C8[44]; //+9C8
	void * pMonStatsTxt; //+A78
	void * pMonStatsLink; //+A7C
	int nMonStats; //+A80
	void * pMonSoundsTxt; //+A84
	void * pMonSoundsLink; //+A88
	int nMonSounds; //+A8C
	void * pMonStats2Txt; //+A90
	void * pMonStats2Link; //+A94
	int nMonStats2; //+A98
	void * pMonPlaceTxt; //+A9C
	void * pMonPlaceLink; //+AA0
	int nMonPlace; //+AA4
	void * pMonPresetTxt; //+AA8
	void * pMonPresetActList; //+AAC
	int offAB0[4]; //+AB0
	int nMonPresetEachAct; //+AC0
	int offAC4[4]; //+AC4
	void * pSuperUniquesTxt; //+AD4
	void * pSuperUniquesLink; //+AD8
	int nSuperUniques; //+ADC
	void * SuperUniqueHcIdxList; //+AE0
	int offAE4[32]; //+AE4
	void * pMissilesTxt; //+B64
	void * pMissilesLink; //+B68
	int nMissiles; //+B6C
	void * pMonLvlTxt; //+B70
	int nMonLvl; //+B74
	void * pMonSeqTxt; //+B78
	void * pMonSeqLink; //+B7C
	int nMonSeq; //+B80
	void * pMonSequences; //+B84
	int nMonSequences; //+B88
	void * pSkillDescTxt; //+B8C
	void * pSkillDescLink; //+B90
	int nSkillDesc; //+B94
	void * pSkillsTxt; //+B98
	void * pSkillsLink; //+B9C
	int nSkills; //+BA0
	void * pPlayerSkillCount; //+BA4
	int nPlayerSkillCount; //+BA8
	void * pPlayerSkillList; //+BAC
	int nPassiveSkillCount; //+BB0
	void * pPassiveSkillList; //+BB4
	void * pOverlayTxt; //+BB8
	void * pOverlayLink; //+BBC
	int nOverlay; //+BC0
	CharStatsTxt *pCharStatsTxt; //+BC4
	int nCharStats; //+BC8
	ItemStatCostTxt * pItemStatCostTxt; //+BCC
	void * pItemStatCostLink; //+BD0
	int nItemStatCost; //+BD4
	void * pOpStatNesting; //+BD8
	int nOpStatNesting; //+BDC
	void * pMonEquipTxt; //+BE0
	int nMonEquip; //+BE4
	void * pPetTypesTxt; //+BE8
	void * pPetTypesLink; //+BEC
	int nPetTypes; //+BF0
	void * pItemTypesLink; //+BF4
	void * pItemTypesTxt; //+BF8
	int nItemTypes; //+BFC
	int nItemTypeNesting; //+C00
	void * pItemTypeNesting; //+C04
	void * pSetsLink; //+C08
	void * pSetsTxt; //+C0C
	int nSets; //+C10
	void * pSetItemsLink; //+C14
	SetItemTxt * pSetItemsTxt; //+C18
	int nSetItems; //+C1C
	void * pUniqueItemsLink; //+C20
	UniqueItemTxt *pUniqueItemsTxt; //+C24
	int nUniqueItems; //+C28
	void * pMonPropLink; //+C2C
	void * pMonPropTxt; //+C30
	int nMonProp; //+C34
	void * pMonTypeLink; //+C38
	void * pMonTypeTxt; //+C3C
	int nMonType; //+C40
	void * pMonTypeNesting; //+C44
	int nMonTypeNesting; //+C48
	void * pMonUModLink; //+C4C
	void * pMonUModTxt; //+C50
	int nMonUMod; //+C54
	void * pLevelsTxt; //+C58
	int nLevels; //+C5C
	void * pLvlDefsBin; //+C60
	void * pLvlPrestTxt; //+C64
	int nLvlPrest; //+C68
	void * ItemStatCostStuff; //+C6C
	int offC70[1]; //+C70
	void * pAnimTables; //+C74
	ExperienceTxt *pExperienceTxt; //+C78
	void * pDifficultyLevelsTxt; //+C7C
	int nDifficultyLevels; //+C80
	void * pExpFieldD2; //+C84
	void * ExpFieldRuntimeData; //+C88
	int offC8C[9]; //+C8C
	void * pLvlSubExtraStuff; //+CB0
	int nLvlSubExtraStuff; //+CB4
	void * pCharTemplateTxt; //+CB8
	int nCharTemplate; //+CBC
	void * pArenaTxt; //+CC0
	void * pLvlTypesTxt; //+CC4
	void * pWaypoints; //+CC8
	int nWaypoints; //+CCC
	int nLvlTypes; //+CD0
	void * pLvlWarpTxt; //+CD4
	int nLvlWarp; //+CD8
	void * pLvlMazeTxt; //+CDC
	int nLvlMaze; //+CE0
	void * pLvlSubTxt; //+CE4
	int nLvlSub; //+CE8
	void * pLvlSubCache; //+CEC
	int offCF0[3]; //+CF0
	void * pMapCache; //+CFC
	int nMapCache; //+D00
	void * pCubeMainTxt; //+D04
	int nCubeMain; //+D08
};
struct EventTable {
	void * 	fpCloseGame; //00
	void * 	fpLeaveGame; //04
	void * 	fpGetDatabaseCharacter; //08
	void * 	fpSaveDatabaseCharacter; //0C
	void * 	fpServerLogMessage; //10
	void *	fpEnterGame; //14
	void * 	fpFindPlayerToken; //18
	void * 	fpSaveDatabaseGuild; //1C
	void * 	fpUnlockDatabaseCharacter; //20
	void * 	fpReserved1; //24
	void * 	fpUpdateCharacterLadder; //28
	void * 	fpUpdateGameInformation; //2C
	void * 	fpReserved2; //30
	void * 	fpSetGameData; //34
	void * 	fpRelockDatabaseCharacter; //38
	void *	fpLoadComplete; //3C
	void * 	fpReservedDebug[10]; 	/* ignore this,just for internal debug */
};
struct GameServerInfo {
	char *szVersion;//+00
	int	dwLibVersion; //+04
	int	bStop; //+08
	int	bIsNT; //+0C
	int	bEnablePatch; //+10
	int	hEventInited; //+14
	void *fpEventLog; //+18
	void *fpErrorHandle;
	EventTable	*fpCallback; //+20
	int	bPreCache;
	int	dwIdleSleep; //+28
	int	dwBusySleep; //+2C
	int	dwMaxGame; //+30
	int	dwMaxPacketPerSecond;
	int	dwGSId;
	int	dwACVersion;
	int	dwCheckSum0; //+40
	int	dwRealCheckSumCount;
	int	dwCheckSumArray[16]; //+48
	int	dwGameDifficultyCount[3][2]; //+88
};
struct ServerPatch { //size 2C
	char *dllname; //+00
	int offset; //+04
	int size1; //+08
	char *patch1; //+0C
	int size2; //+10
	char *patch2; //+14
	int settings,typeFlag,off20,off24,off28; //+18
};
struct PlayerInfo {
	int PlayerMark,dwReserved; //+00
	char CharName[16]; //+08
	char AcctName[16]; //+18
};
struct WorldEvent {
	int bHasMsg;
	int WorldEventBaseCount;
	int WorldEventCurrentSpawnCount;
	int WorldEventLastSpawnCount;
	int WorldEventNextSpawnCount;
	int WorldEventLastSellTime; // DWORD LastSellTickCount;
	int WorldEventLastSpawnTime; // DWORD LastSpawnTickCount;
	int WorldEventTotalSpawn;
	char WorldEventKeyItem[128];
};
struct ExtendGameInfoStruct {
	int rand_l,rand_h; //+00
	int PortalOpenedFlag; //+08
	int BOSS_A_AREA,BOSS_B_AREA,BOSS_C_AREA; //+0C
	int BOSS_D0_AREA,BOSS_D1_AREA,BOSS_D2_AREA; //+18
	int Diablo_rand_l,Diablo_rand_h; //+24
	int Meph_rand_l,Meph_rand_h; //+2C
	int LoD_Game; //+34
};

struct CriticalSection {
	void *DebugInfo; //PRTL_CRITICAL_SECTION_DEBUG
	int LockCount,RecursionCount; //+04
	int OwningThread,LockSemaphore; //+0C
	void *SpinCount; //+14
};
struct ClientInfo {
	int clientId; //+00
	ClientInfo *next; //+04
	int off08[2]; //+08
	int off10[4]; //+10
	int worldID; //+20
};
struct ClientInfos {
	int firstId,lastId; //+00 ?
	int hashsize; //+08 ?
	int hashmask; //+0C
	int off10; //+10
	ClientInfo **infos; //+14
	int off18; //+18
	CriticalSection *ctSecs; //+1C
};
struct WorldInfo {
	int size; //+00 =4 (int)world+size+4 is the next world
	void *ptr; //+04 =world+4
	World *world; //+08
};
struct WorldHash {
	int off00[4]; //+00
	int off10[3]; //+10
	WorldInfo *worldInfos; //+1C
	int off20; //+20
	int hashMask; //+24
	int off28[2]; //+28
	int criticalSection[6]; //+30
};
struct MCPPacket05 {
	char type; //+00 =5
	short seqId; //+01 increase next refresh
	int idx; //+03
	char nPlayers; //+07
	char off08; //+08 -2,-1,=4
	char difficulty; //+09 0,0x10,hell:0x20
	char off0A; //+0A =20h
	char off0B; //+0B =30h
	char game[29]; //+0C gameName 00 description 00
};
struct Mcp05Buf { //size=0x63
	int hasExtraData; //+00
	int seqId,idx,uk_0C,uk_10; //+04
	int nPlayers,off08; //+14
	char game[16]; //+1C
	char description[16]; //+2C
	char uk_3C[0x27]; //+3C
};
struct Mcp05Game { //size=0x3B 59
	char uk_0; //+00
	char game[16]; //+01
	char description[16]; //+11
	char uk_20[16]; //+21
	short nPlayers,idx; //+31
	char uk_34[6]; //+35
};
struct D2FloatingWindow {
	int off0,off4,off8,hasCellFile; //+00
	int off10,off14,off18,off1C; //+10
	int off20,off24,off28,x; //+20
	int y,w,h,off3C; //+30
	int off40,off44,off48,off4C; //+40
	int off50,off54,off58,off5C; //+50
};
struct d2lang_map {
	short off0,mapSize; //+00
	int count; //+04
	char uk_08[9]; //+08
	int totalSize; //+11
	short map[1]; //+15 size=mapSize*2
	//after map is count*17
};

#endif 
