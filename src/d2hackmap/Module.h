//--- m_start.h ---
#ifdef VARIABLE_DEFINE
#ifndef GVARDEF
	#define GVARDEF
	#define GVAR(d1,v1,v2) extern d1 v1 ;
	#define GVAR2(d1,v1,...) extern d1 v1 ;
#else
	#define GVAR(d1,v1,v2) d1 v1 = v2 ;
	#define GVAR2(d1,v1,...) d1 v1 = { __VA_ARGS__ } ;
#endif
#endif
//--- m_pub.h ---
#ifdef FUNCTION_DECLARE
void GameLoopPatch_ASM();
void GameEndPatch_ASM();
void KeyupPatch_ASM();
void KeydownPatch_ASM();
//void MButtonDownPatch_ASM();
//void XButton1DownPatch_ASM();
//void XButton2DownPatch_ASM();
int saveRuntimeInfo();
#endif
#ifdef VARIABLE_DEFINE
	GVAR (int,			dwGameWindowId,					1)
	GVAR(char,			szRuntimePath[256]	,		"runtime")
	GVAR (BOOL,		fSinglePlayer,				0)
	GVAR (volatile int,			dwCurMs,					0)
	GVAR (int,			dwSinglePlayerContext,					0)
	GVAR (BOOL,			fInGame,				FALSE)
	GVAR (BOOL,			fPlayerInTown,			TRUE)
	GVAR (DWORD,		dwCurrentAct,				0)
	GVAR (DWORD,		dwCurrentLevel,				1)
	GVAR (BOOL,			fDefault,			    TRUE)
	GVAR (BOOL,			fKeyRepeat,				FALSE)
	GVAR (DWORD,		dwKeyDown,					0)
	GVAR (int,			dwGameLng,					-1)
	GVAR (DWORD,		dwPlayerId,					0)
	GVAR (DWORD,		dwPlayerClass,					0)
	GVAR (int,			dwLeftSkill,					-1)
	GVAR (int,			dwRightSkill,					-1)
	GVAR (BOOL,			fEnterGameSound,				0)
	GVAR (int,			dwEnterGameShowDifficultyMs,				0)
	GVAR2(DWORD,		actlvls[],	1, 40, 75, 103, 109, 137 )
	GVAR2(ToggleVar,	tReloadConfig,			TOGGLEVAR_DOWN,	0,	-1,		1, "ReloadConfig",			&ReloadConfig)
	GVAR2(char,			szCFGVersion[2][256]	,		{'\0'})
	GVAR2(wchar_t,		wszCFGVersion[2][256]	,		{L'\0'})
	GVAR (char,			aszUnitBlobFiles[11][50],       {'\0'})
	GVAR2 (CellFile*,	pUnitBlobCells[11],	{NULL,})
	GVAR2 (CellFile*,	apMyShrineCells[2][NUMOF_SHRINES],	{0})
	GVAR (CellFile*,	pWaypointCell,						0)
	GVAR (CellFile*,	pMinimapObjectCell,				    0)
	GVAR (DWORD,		fShowCheckInfo,					1)
	GVAR (BYTE,			fAutomapPartyDefault,			1)
	GVAR (BYTE,			fAutomapNamesDefault,			1)
	GVAR (BOOL,			fLanguageCheck,			TRUE)
	GVAR (BOOL,			fLocalizationSupport,		FALSE)
	GVAR (BOOL,			fAutoSetCHISupport,		1)
	GVAR (BOOL,			fLoadItemColours,		1)
#endif
//--- m_AutoMap.h ---
#ifdef FUNCTION_DECLARE
#define MODULE_AUTOMAP
int RevealAutomap();
void RevealAutomapLoop();
void ResetMapStatus();	
void ShowAutoMap();
int RevealLevelPlayerIn();
#endif
#ifdef VARIABLE_DEFINE
GVAR (int,			fAutoRevealAct,			0)
GVAR2 (int,			afRevealedActs[5],			0)
GVAR2(ToggleVar,	tRevealAct,				TOGGLEVAR_DOWN,	0,	-1,		1,  "RevealAutomap",			&RevealAutomap)
GVAR2(ToggleVar,	tRevealLevel,			TOGGLEVAR_DOWN,	0,	-1,		1,  "RevealLevel",			&RevealLevelPlayerIn)
GVAR2(ToggleVar,	tAutoMap,				TOGGLEVAR_ONOFF,	1,	-1,		1, "Auto Map Toggle"	)
#endif
//--- m_AutoMapCell.h ---
#ifdef FUNCTION_DECLARE
#define MODULE_AUTOMAPCELL
void AddAutoMapCell(DrlgRoom2 *pRoom2);
void	OverrideShrinePatch_ASM();
void	AddShrinePatch_ASM();
void  __stdcall  DrawAutomapCellPatch(CellContext *pCellContext, DWORD xpos, DWORD ypos, RECT *cliprect, DWORD bright);
void RemoveMyAutomapCells();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tAutomapLevelNames,	    TOGGLEVAR_ONOFF,	0,	-1,		1, "Automap Level Names Toggle" )
GVAR2(ToggleVar,	tMiniShrine,			TOGGLEVAR_ONOFF,	0,	-1,		1, "Mini Shrine Toggle" )
GVAR (wchar_t,		wszStaffTombLvlDesc[50],      {L'\0'})
GVAR (BYTE,			nCaveNameTextCol,					0)
#endif
//--- m_RoomArea.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ROOMAREA
	void  RoomPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tAutomapActiveRoom,		TOGGLEVAR_ONOFF,	0,	  -1	,1,"AutomapActiveRoom" )
GVAR2(ToggleVar,	tAutomapReadyRoom,		TOGGLEVAR_ONOFF,	0,	  -1	,1,"AutomapReadyRoom")
GVAR2(ToggleVar,	tAutomapScreenArea,		TOGGLEVAR_ONOFF,	0,	  -1	,1,"AutomapScreenArea")
GVAR (BYTE,			nAutomapActiveRoomColors,	 0x16)
GVAR (BYTE,			nAutomapReadyColors,			7)
GVAR (BYTE,			nAutomapScreenColors,		 0x16)
#endif
//--- m_MinimapPoint.h ---
#ifdef FUNCTION_DECLARE
#define MODULE_MINIMAPPOINT
	void AddMinimapCell( DrlgLevel* pDrlgLevel , short cellno ,short xpos , short ypos );
	void ResetMiniCells();
	void BuildMiniMapPoint( DWORD dwAct );
	void DrawMinimapPoint();
#endif
#ifdef VARIABLE_DEFINE
	GVAR (BYTE,			nMinimapSize,					    1)
	GVAR (BYTE,			nMinimapCellCol,			       12)
	GVAR2(ToggleVar,	tMinimap,				TOGGLEVAR_ONOFF,	0,	 -1,	1, "Minimap Pointer " )
#endif
//--- m_AutoMapBlob.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_AUTOMAPBLOB
	void ForceDrawBlob_ASM();
	void DrawBlobPath_ASM();
	void MixedBlobColPatch_ASM();
	void DrawPartyPlayerBlobPatch_ASM();
	void HostilePlayerColor_ASM();
	void DrawPlayerTextPatch_ASM();
	void ResetMonsterColours();
	void MonsterBlobDescPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tAutomapCorpse,		      TOGGLEVAR_ONOFF,  0,  -1, 1, "AutomapCorpse")
GVAR2(ToggleVar,    tAutomapMonsters,     TOGGLEVAR_ONOFF,  0,  -1, 1, "AutomapMonsters")
GVAR2(ToggleVar,    tAutomapChests,       TOGGLEVAR_ONOFF,  0,  -1, 1, "Chests")
GVAR2(ToggleVar,    tAutomapMissiles,     TOGGLEVAR_ONOFF,  0,  -1, 1, "Missiles")
GVAR2(ToggleVar,    tAutomapItems,        TOGGLEVAR_ONOFF,  0,  -1, 1, "Show automap items")
GVAR (BYTE,			nHostileMissileColor,		 0x62)
GVAR (BYTE,			nGuidedMissileColor,		 0x5B)
GVAR (BYTE,			nTracerMissileColor,		 0xFF)
GVAR (BYTE,			nOtherMissileColor,			 0xFF)
GVAR (BYTE,			nClosedChestColor,				9)
GVAR (BYTE,			nLockedChestColor,				9)
GVAR (BYTE,			nNormalCorpseColor,				8)
GVAR (BYTE,			nBossCorpseColor,					8)
GVAR (BYTE,			nMinionCorpseColor,				8)
GVAR (BYTE,			nChampionCorpseColor,			8)
GVAR (BYTE,			nMonsterNormalColor,		 0x62)
GVAR (BYTE,			nMonsterBossColor,			 0x5b)
GVAR (BYTE,			nMonsterMinionColor,		 0x5b)
GVAR (BYTE,			nMonsterChampColor,			 0x5b)
GVAR (BYTE,			nSuperUniqueColor,		 (BYTE)-1)
GVAR (BYTE,			nNeutralPlayerColor,		 0x62)
GVAR (BYTE,			nHostilePlayerColor,		 0x5b)
GVAR (BYTE,			nFriendMinionColor,			 0x7f)
GVAR (BYTE,			nPlayerMinionColor,			 0xcb)
GVAR (BYTE,			nPartyTextColour,				2)
GVAR (BYTE,			nNeutralTextColour,				9)
GVAR (BYTE,			nHostileTextColour,				1)
GVAR (BYTE,			nMonsterTextCol,				1)
GVAR2(wchar_t,		awszEnchantDescs[50][20]	,	{L'\0'})
GVAR2(wchar_t,		awszAuraDescs[7][20]	,		{L'\0'})
GVAR2(wchar_t,		awszImmuneDescs[6][20]	,		{L'\0'})
GVAR2(BYTE,			anMonsterCorpseColors[1000]	,   {0})
GVAR2(BYTE,			anMissileColours[1000]	,	    {0})
GVAR2(ToggleVar,	tBossNameToggle,			TOGGLEVAR_ONOFF,	0,	-1,	1 , "BossNameToggle")
GVAR2(BYTE,			anMonsterColours[1000]	,   {0})
GVAR (BYTE,			nSuperUniqueDescFont,			0)
#endif
//--- m_Environment.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ENVIRONMENT
	void WeatherPatch_ASM();
	void LightingPatch_ASM();
	void InfravisionPatch_ASM();
	void __stdcall ShakeScreenPatch(DWORD *pPosX, DWORD *pPosY);
	void EnvNewGame();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tFullVisuals,			TOGGLEVAR_ONOFF,	0,	-1,		1,	"FullVisuals")
GVAR2(ToggleVar,	tWeather,				TOGGLEVAR_ONOFF,	0,	-1,		1,	"Weather")
GVAR2(ToggleVar,	tLightRadiu,			TOGGLEVAR_ONOFF,	0,	-1,		1,	"LightRadiu")
GVAR2(ToggleVar,	tInfravision,			TOGGLEVAR_ONOFF,	1,	-1,		1,	"Infravision")
GVAR2(ToggleVar,	tScreenshake,			TOGGLEVAR_ONOFF,	0,	-1,		1,	"Screenshake")
GVAR2(BYTE,			afMonsterCorpses[1000]	,	    {0})
GVAR2(BYTE,			afMissileCorpses[1000]	,	    {0})
GVAR2(ToggleVar,    tHiddenCorpse,        TOGGLEVAR_ONOFF,  0,  -1,  1,	"HiddenCorpse")
#endif
//--- m_MonsterDeath.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_MONSTERDEATH
	void MonsterDeathPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(BYTE,			afMonsterDeathAnims[1000]	,   {0})
#endif
//--- m_ScrollLock.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_SCROLLLOCK
	void SetScrollMapPos(char xinc, char yinc);
	int MiniMapScroll(int id);
	void InitScrollLock(BOOL fInit);
	int CenterMap();
	void SetMapShake();
	void DrawScollOffset(wchar_t*wbuf ,int posy);
	int ScrollMap( int idx );
	int ScrollMapEnd( int idx );
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tDiagonalScroll,		TOGGLEVAR_ONOFF,	1,	-1,		1 ,"Diagonal scroll Toggle" )
GVAR2(ToggleVar,	tScrollMap,				TOGGLEVAR_ONOFF,	0,	-1,		1 ,"ScrollMap")
GVAR2(ToggleVar,	tCenterMap,			    TOGGLEVAR_DOWN,	0,	-1,		1,	0,		&CenterMap)
GVAR (DWORD,		nMapScrollSpeed,	        16)
GVAR (DWORD,		nScrollMode,				1)
GVAR2(ToggleVar,	tMapScrollKey[9]	, 	   {0})
GVAR2(char,			nMapScrollOffset[9][5][2], {0})
GVAR2(int,			miniMapScrollKeys[9][2], 0)
GVAR2(int,			miniMapScrollOffset[9][2], 0)
#endif
//--- m_QuickBackTown.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_QUICKBACKTOWN
	void RecvCommand_60_Patch_ASM();
	int BackToTown();
#endif
#ifdef VARIABLE_DEFINE
 GVAR (BOOL,			fBackToTown,				FALSE)
 GVAR2(ToggleVar,		tBackToTown,		TOGGLEVAR_DOWN,	0,	-1,		1,	0,		&BackToTown)
 GVAR (DWORD,			nTownportalAlertNums,			(DWORD)-1)
#endif
//--- m_PartyHelp.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_PARTYHELP
	BOOL CheckInvite() ;
	void InviteAll() ;
	void PermitAll() ;
	void RecvCommand_5A_Patch_ASM();
	void RecvCommand_5B_Patch_ASM();
	void ResponseInvite();
#endif
#ifdef VARIABLE_DEFINE
GVAR (BOOL,			fCanInvite,			FALSE)
GVAR (BOOL,			fCanPermit,			FALSE)
GVAR2(ToggleVar,	tAutoParty,			TOGGLEVAR_ONOFF,	0,	-1, 1,	"Auto Party Toggle")
GVAR2(ToggleVar,	tAutoInvite,		TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Invite Toggle")
GVAR2(ToggleVar,	tAutoPermit,		TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Loot Permit Toggle")
GVAR (DWORD,		mAutoPartyDelays,			500)
GVAR (int,		fNeedPartyResponse,			0)
#endif
//--- m_OutTownSelect.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_OUTTOWNSELECT
	void OutTownSelectPatch_ASM1();
	void OutTownSelectPatch_ASM2();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tOutTownSelect,				TOGGLEVAR_ONOFF,	1,	-1,	1,	"Out of town select")
#endif
//--- m_WinMessage.h ---
#ifdef FUNCTION_DECLARE
	int GameControlStart(int cmd);
	int GameControlEnd(int cmd);
	void GameControl(int cmd);
	int GameSound(int snd);
	int switchSkillStart(int id);
	int switchSkillEnd(int id);
	void WinMessageNewGame();
	void WinMessageEndGame();
	void WinMessageRunLoop();
	void WinMessagePatch_ASM();
	void SetCursorPosPatch_ASM();
	int inputUserPassword(int id);
	void delayScreenSaver(int ms);
	int canAutoClick();
	int FullScreen();
	int LockMouse();
	void StandStillPatch_ASM();
	int getOtherClients(D2Window *hwnd,int cap,int area);
	int SwitchWindow(int id);
#endif
#ifdef VARIABLE_DEFINE
	GVAR (int,		fWinActive,						1)
	GVAR (int,		dwQuickSwapItemDelayMs,						500)
	GVAR (int,		dwAutoCallToArm,						500)
	GVAR (int,		fScreenSaverActive,						0)
	GVAR(DWORD,	forceStandStill,				0)
	GVAR2(ToggleVar,	tLockMouseToggle,				TOGGLEVAR_ONOFF,	0,	0,	 0,	"Lock Mouse Toggle",&LockMouse)
	GVAR2(ToggleVar,	tAutoHideMinimapToggle,				TOGGLEVAR_ONOFF,	0,	0,	 0,	"Auto Hide MiniMap")
	GVAR2(ToggleVar,	tAutoMinimapTeleportToggle,				TOGGLEVAR_ONOFF,	0,	0,	 0,	"Auto Teleport MiniMap")
	GVAR (int,		dwAutoHideMinimapKey,						0)
	GVAR2(ToggleVar,	tFullScreen,			TOGGLEVAR_DOWN,	0,	-1,		1, "FullScreen",			&FullScreen,0,0,2)
	GVAR(DWORD,	dwAutoMaximizeWidth,				1600)
	GVAR(DWORD,	dwAutoMaximizeHeight,				900)
	GVAR(DWORD,	dwHideCaptionBorder,				1)
	GVAR2(ToggleVar,	tAutoClickLeft,				TOGGLEVAR_ONOFF,	0,	0,	 0,	"AutoClickLeft")
	GVAR2(ToggleVar,	tAutoClickRight,				TOGGLEVAR_ONOFF,	0,	0,	 0,	"AutoClickRight")
	GVAR(DWORD,	dwAutoClickLeftDownFs,				0)
	GVAR(DWORD,	dwAutoClickLeftUpFs,				0)
	GVAR(DWORD,	dwAutoClickRightDownFs,				0)
	GVAR(DWORD,	dwAutoClickRightUpFs,				0)
	GVAR (DWORD,		dwEnableScreenSaver,						0)
	GVAR (int,		dwLoadLibraryKey,						0)
	GVAR2(char,			szLoadLibraryCmd[256]	,			0)
	GVAR2(char,			szLibraryPath[256]	,			0)
	GVAR2(int,			dwGameControlKeys[64][4]		,0)
	GVAR2(int,			dwGameSoundKeys[128],			 0)
	GVAR2(ToggleVar,tAccountKeys[16],			 0)
	GVAR2(ToggleVar,tSwitchWindowKeys[8],	0)
	GVAR2(char,			szAccountNames[16][64],		0)
	GVAR2(char,			szAccountPasswords[16][64],		0)
	GVAR2(int,			dwSwitchSkillKeys[32],			 0)
	GVAR2(int,			dwSwitchSkillLeft[32][8][4],			 0)
	GVAR2(int,			dwSwitchSkillRight[32][8][4],			 0)
	GVAR2(int,			dwSwitchSkillLeftUp[32][8],			 0)
	GVAR2(int,			dwSwitchSkillRightUp[32][8],			 0)
	GVAR2(int,			dwSwitchSkillLeftOrg[32],			 0)
	GVAR2(int,			dwSwitchSkillRightOrg[32],			 0)
	GVAR2(int,			dwSwitchSkillStandStill[32],			 0)
#endif
//--- m_AutoEnchant.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_AUTOENCHANT
	void AutoEnchantUnit(UnitAny *pUnit);
	void AutoEnchantNewGame();
	void AutoEnchantRunLoop();
	void AddUnitPatch_ASM();
	void AutoEnchantPlayerNeedHelp(int id);
	void RecvCommand_2C_Patch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tAutoEnchant,				TOGGLEVAR_ONOFF,	0,	-1,	 1,	"Auto Enchant")
GVAR(int,	dwEnchantMana,				66)
GVAR(int,	dwRenewEnchantSeconds,				300)
GVAR(int,	dwAutoEnchantScreenSaverDelayMs,				5000)
GVAR(int,	dwAutoEnchantGroup,				3)
GVAR(int,	dwAutoEnchantGroupId,				0)
GVAR2(BYTE,	dwAutoEnchantMonsterGroup[1024],				0)
#endif
//--- m_AutoSummon.h ---
#ifdef FUNCTION_DECLARE
	void AutoSummonUnit(UnitAny *pUnit);
	void AutoSummonNewGame();
	void AutoSummonRunLoop();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tAutoSummon,				TOGGLEVAR_ONOFF,	0,	-1,	 0,	"Auto Summon")
GVAR(int,	dwAutoSummonStartMs,				0)
GVAR(int,	dwAutoSummonMode,				0)
GVAR(int,	dwAutoSummonMana,				100)
GVAR(int,	dwAutoSummonSwitchSkillMs,				100)
GVAR(int,	dwAutoSummonSkeletonLevelAdjust,				8)
GVAR(int,	dwAutoSummonSkeletonMageLevelAdjust,				8)
#endif
//--- m_Snapshot.h ---
#ifdef FUNCTION_DECLARE
	void SnapshotNewGame();
	void SnapshotEndGame();
	void SnapshotRunLoop();
	int DoSnapshot();
	void dumpScanUnit(UnitAny *unit);
	void dumpScanUnitEnd();
	void RecvCommand_63_Patch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR(int, enDumpUnit, 0)
GVAR(char,			szSnapshotPath[256]	,			"snapshot")
GVAR(char,			szSnapshotNamesPath[256]	,			"names.txt")
GVAR2(ToggleVar,	tDoSnapshot,			  TOGGLEVAR_DOWN,	0,	-1,	 1, "Do Snapshot",			&DoSnapshot)
GVAR2(ToggleVar,	tSnapshotNewGame,		TOGGLEVAR_ONOFF,	1,	-1,	 1,	"Snapshot Enter Game")
GVAR2(ToggleVar,	tSnapshotEndGame,		TOGGLEVAR_ONOFF,	1,	-1,	 1,	"Snapshot Exit Game")
GVAR2(ToggleVar,	tSnapshotLog,				TOGGLEVAR_ONOFF,	1,	-1,	 1,	"Enable Snapshot Log")
GVAR2(ToggleVar,	tSnapshotSaveD2s,				TOGGLEVAR_ONOFF,	0,	-1,	 1,	"Snapshot d2s file")
#endif
//--- m_ScreenHook.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_SCREENHOOK
	void InitScreenHook() ;
	void DrawClientPatch_ASM();
	void InitExp();
	void UpdateExp_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tShowExp,				TOGGLEVAR_ONOFF,	0,	-1,	  2,	"Show Exp Toggle"  )
GVAR2(ToggleVar,	tShowPing,				TOGGLEVAR_ONOFF,	0,	-1,	  1,	"Show Ping Toggle" )
GVAR(int , currentgameexp  ,0)
#endif
//--- m_MessageLog.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_MESSAGELOG
	void MessageLogPatch1_ASM();
	void MessageLogPatch2_ASM();
	void LogExitMessage();
#endif
#ifdef VARIABLE_DEFINE
 GVAR2(ToggleVar,	tLogMessage,		TOGGLEVAR_ONOFF,	0,	-1,	1,"LogMessage")
 GVAR (DWORD,		dwMessageFileSize,		1048576)
#endif
//--- m_QuickSwapItem.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_QUICKSWAPITEM
	void RButtonUpHandlerPatch_ASM();
	void RButtonInTradePatch_ASM();
	void CtrlClickHandlerPatch_ASM();
	void LButtonUpHandlerPatch_ASM();
	void DealCtrlClick();
#endif
#ifdef VARIABLE_DEFINE
  GVAR2(ToggleVar,	tRightClickSwap,    TOGGLEVAR_ONOFF,	1,	-1,	1 ,	"Right Click Swap")
	//GVAR2(ToggleVar,	tCtrlClickSwap,     0,	0,	-1,	1 ,	"Ctrl Click Swap")
	GVAR2(ToggleVar,	tQuickDropToggle,	TOGGLEVAR_ONOFF,	0,	-1,	1 , "Quick Drop")
#endif
//--- m_SkipNpcMessage.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_SKIPNPCMESSAGE
	void NPCQuestMessageStartPatch_ASM();
	void NPCQuestMessageEndPatch1_ASM();
	void NPCQuestMessageEndPatch2_ASM();
	void NPCMessageLoopPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
	GVAR (DWORD,		mSkipQuestMessage,			0)
#endif
//--- m_TransGoldHelp.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_TRANSGOLDHELP
	void SetGoldPatch_ASM();
	void SetDefaultGoldPatch_ASM();
#endif
//--- m_ULCPath.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ULCPATH
	void ULCPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tULCMask,				TOGGLEVAR_ONOFF,	0,	-1,		1	,"ULC Mask Toggle" )
#endif
//--- m_NsPassBugFix.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_NSPASSBUGFIX
	void NsPassingActBugFixPatch_ASM();
#endif
//--- m_RoomInfo.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ROOMINFO
	void SetTickCount();
	void __fastcall DrawGameInfoPatch(DWORD dwFont) ;
#endif
#ifdef VARIABLE_DEFINE
	GVAR2(ToggleVar,	tServerIp,			    TOGGLEVAR_ONOFF,	1,	-1,		1,	"Server Ip" )
	GVAR2(ToggleVar,	tGameTime,				TOGGLEVAR_ONOFF,	1,	-1,		1,	"Game Time" )
	GVAR2(ToggleVar,	tClock,					TOGGLEVAR_ONOFF,	1,	-1,		1,	"Clock" )
	GVAR2(ToggleVar,	tLayerLevelNo,			TOGGLEVAR_ONOFF,	0,	-1,		1,	"Layer LevelNo" )
	GVAR2(ToggleVar,	tAreaLevel,				TOGGLEVAR_ONOFF,	0,	-1,		1,	"Area Level" )
#endif
//--- m_QuickGame.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_QUICKGAME
	int QuickNextGame(int addnum);
	void SaveGameName();
	int QuickExitGame();
	void __fastcall NextGameNamePatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,DWORD,char*));
	void __fastcall NextGamePasswordPatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,DWORD,char*));
	void __fastcall NextGameDecriptionPatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,DWORD,char*));
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tExitGame,			TOGGLEVAR_DOWN,	0,	-1,	 1, 	"ExitGame",	&QuickExitGame, 0)
GVAR2(ToggleVar,	tQuickNextGame,		TOGGLEVAR_DOWNPARAM,	0,	-1,	 1, "NextGame",	&QuickNextGame,	1)
GVAR2(ToggleVar,	tQuickNextGame2,	TOGGLEVAR_DOWNPARAM,	0,	-1,	 1, "NextGame2",	&QuickNextGame,	2)
GVAR2(wchar_t,		wszGameName[32]	,			{L'\0'})
GVAR2(wchar_t,		wszGamePassword[32]	,		{L'\0'})
GVAR2(wchar_t,		wszGameDescription[32]	,		{L'\0'})
GVAR (BOOL,			fAutoNextGameName,				0)
GVAR (BOOL,			fAutoNextGamePassword,			0)
GVAR (BOOL,			fAutoNextGameDescription,			0)
GVAR (BOOL,			fCanExitGame,				FALSE)
#endif
//--- m_GameFilter.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_GAMEFILTER
	void D2MCPPacketHandler_05_Patch_ASM();
	void CreateGameBoxPatch_ASM();
	void __stdcall DestroyGamelistPatch( D2EditBox* pListBox );
	void GameListRefreshTimePatch_ASM();
	void DrawGameListPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
  GVAR (BOOL,			fGameFilter,		FALSE)
#endif
//--- m_PartyInfo.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_PARTYINFO
	void DrawPetHeadPath_ASM();
	void DrawPartyHeadPath_ASM();
#endif
#ifdef VARIABLE_DEFINE
	GVAR2(ToggleVar,	tShowPartyPosition,				TOGGLEVAR_ONOFF,	0,	-1,	1,	"ShowPartyPosition")
	GVAR2(ToggleVar,	tShowPartyLevel,				TOGGLEVAR_ONOFF,	0,	-1,	1,	"ShowPartyLevel")
#endif
//--- m_GameCount.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_GAMECOUNT
	void KillCountPatch_ASM();
	void ShowGameCount();
	void UnitModeChangePatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tKillCount,				TOGGLEVAR_ONOFF,	0,	-1,		1, "KillCount",	)
GVAR2(ToggleVar,	tGetHitCount,			TOGGLEVAR_ONOFF,	0,	-1,		1, "GitHitCount",	)
GVAR (DWORD,		dwKillSum,				0)
GVAR (DWORD,		dwGameKills,			0)
GVAR (DWORD,		dwLastKills,			0) 
GVAR (DWORD,		dwGetHitSum,			0)
GVAR (DWORD,		dwGetHits,				0)
GVAR (DWORD,		dwOrgMode,				0)
#endif
//--- m_GameMonitor.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_GAMEMONITOR
	void DrawMonitorInfo();
	void RecvCommand_A8_Patch_ASM();
	void RecvCommand_A9_Patch_ASM();
	void RecvCommand_A7_Patch_ASM();
	void ResetMonitor();
#endif
#ifdef VARIABLE_DEFINE
	GVAR2(ToggleVar,		tCountDown,	TOGGLEVAR_ONOFF,	0,	-1,	1,	"Count Down")	
	GVAR (int,				dwGameMonitorX,			-10)
	GVAR (int,				dwGameMonitorY,			-110)
	GVAR (int,				dwCountDownFontSize,			3)
	GVAR (int,				dwCountDownGap,			25)
	GVAR (int,				dwCountDownFlashSecond,			30)
	GVAR2(StatMonitor,		sMonitorStr[200]	,		 { {L'\0'}})
	GVAR (int,				nMonitors,			0)
	GVAR2(ToggleVar,		tStateMonitorToggle,	TOGGLEVAR_ONOFF,	0,	-1,	1,	"State Monitor")	
	GVAR (int,				fMonitorQuantity,			0)
	GVAR (int,				fMonitorDurability,			0)
#endif
//--- m_ShowOrbs.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_SHOWORBS
	void ShowLifePatch_ASM();
	void ShowManaPatch_ASM();
	int SetLifeManaShow();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tPermShowOrb,		TOGGLEVAR_ONOFF,	1,	-1,	 1,  "Perm Show Life/Mana" , &SetLifeManaShow)
#endif
//--- m_ItemDefine.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ITEMDEFINE
	int SwitchItemMode();
	BYTE GetItemColour(UnitAny *pItem,DWORD arridx);
	int GetItemIndex(int pos);
#endif
#ifdef VARIABLE_DEFINE
GVAR (DWORD,		dwGoldGoodNum,					   0)
GVAR (char,			anGoldGoodCol[4],		  {(BYTE)-1})
GVAR (char,			anGoldPoorCol[4],		  {(BYTE)-1})
GVAR2(char,			anRuneColours[100][4]	,		 {0})
GVAR2(char,			anItemColours[3000][8][2][7][4]	,{0})
GVAR2(char,			anItemHideLevel[3000][8]		,{0})
GVAR2(char,			anItemShowCLevel[3000]		,{0})
GVAR2(ToggleVar,	tSwitchItemMode,		TOGGLEVAR_DOWN,	0,	-1,		1,	"SwitchItemMode",	&SwitchItemMode)
GVAR (DWORD,	    nItemShowMode,				   1)
#endif
//--- m_ItemName.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ITEMNAME
	void OpenCubeStringPatch_ASM();
	void ItemNamePatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR (int,			fSimplifiedPotionName,			0)
GVAR (BYTE,			nQuestItemColour,			(BYTE)-1)
GVAR (wchar_t,		wszEtherealItemPrefix[30],  {L'\0'})
GVAR (wchar_t,		wszEtherealItemPostfix[30], {L'\0'})
GVAR2(ToggleVar,	tRuneNumbers,		TOGGLEVAR_ONOFF,	1,	-1,	1,"Rune numbers")
GVAR2(ToggleVar,	tSocketNumbers,		TOGGLEVAR_ONOFF,	1,	-1,	1,"Socket numbers")
GVAR2(ToggleVar,	tItemLevel,			TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemLevel")
GVAR2(ToggleVar,	tItemValue,			TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemValue")
GVAR2(ToggleVar,	tItemIndex,			TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemIndex")
GVAR2(ToggleVar,	tItemFileIndex,		TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemFileIndex")
GVAR2(ToggleVar,	tUnitNumber,		TOGGLEVAR_ONOFF,	0,	-1,	1, "UnitNumber")
GVAR2(ToggleVar,	tItemQLevel,		TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemQLevel")
GVAR2(ToggleVar,	tItemMLevel,		TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemMLevel")
GVAR2(ToggleVar,	tItemALevel,		TOGGLEVAR_ONOFF,	0,	-1,	1, "ItemALevel")
GVAR2(ToggleVar,	tAfterCube,			TOGGLEVAR_ONOFF,	0,	-1,	1, "AfterCube")
GVAR2(ToggleVar,	tLifePerLevel,		TOGGLEVAR_ONOFF,	0,	-1,	1, "LifePerLevel")
GVAR2(DWORD,		dwItemValueNpc[5],	154,178,255,405,513 )
GVAR2(ToggleVar,	tShowBaseED,				TOGGLEVAR_ONOFF,	0,	-1,	1 , "Show Base ED")
#endif
//--- m_ItemInGround.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ITEMINGROUND
	BOOL  CheckItemVisible(UnitAny *pItem);
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tShowIdentifiedItem,  TOGGLEVAR_ONOFF,	0,	-1,		1,	"Show Identified items")
GVAR2(ToggleVar,    tHiddenItems,         TOGGLEVAR_ONOFF,	1,	VK_ADD,	1,	"Show hidden items")
#endif
//--- m_PermShowItem.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_PERMSHOWITEM
	void	PermShowItemsPatch1_ASM();
	void	PermShowItemsPatch2_ASM();
	void	PermShowItemsPatch3_ASM();
	void	PermShowItemsPatch4_ASM();	
#endif
#ifdef VARIABLE_DEFINE
 GVAR2(ToggleVar,	tPermShowItems,				TOGGLEVAR_ONOFF,	0,	-1,	1,	"Perm show items")
 GVAR2(ToggleVar,	tLockAltToggle,				TOGGLEVAR_ONOFF,	0,	-1,	1,	"Lock Alt")
#endif
//--- m_ItemDesc.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ITEMDESC
	void ItemBasicStatPatch_ASM();
	void ItemSocketViewPatch_ASM();
	void SocketItemStatViewPatch_ASM();
	void __fastcall ItemDescPath( wchar_t *wszMsg , int xpos , int ypos , int tran, int color );
	StatList * __stdcall GetItemExtStatListPatch(UnitAny *pUnit, DWORD dwStatNo, DWORD dwListFlag) ;
#endif
#ifdef VARIABLE_DEFINE
	GVAR2(ToggleVar,	tItemBasicStat,				TOGGLEVAR_ONOFF,	0,	-1,	1 , "Item basic stat")
	GVAR2(ToggleVar,	tViewSocketable,			TOGGLEVAR_ONOFF,	0,	-1,	1 , "View socketables")
	GVAR2(ToggleVar,	tViewSocketBase,			TOGGLEVAR_ONOFF,	0,	-1,	1 , "View socketed base")
#endif
//--- m_ItemVariableProp.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ITEMVARIABLEPROP
	void GetItemPropStringPatch_ASM();
	void StrcatItemPropStringPatch_ASM();
	void StrcatSocketStringPatch_ASM();
	void StrcatDefenseStringPatch_ASM();
	void StrcatDamageStringPatch_ASM1();
	void StrcatDamageStringPatch_ASM2();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tShowItemVariableProp,	  TOGGLEVAR_ONOFF,	0,	-1,	1 , "Item Variable Prop")
#endif
//--- m_ItemExtInfo.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ITEMEXTINFO
	void ShowItemExtInfo();
	int LoadExtInfo();
	void UnLoadExtInfo();
	void SetExtInfoPos(int xPos , int yPos , DWORD txtWidth , DWORD txtHeight);
	void DrawHoverPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
	GVAR2(char,			szItemExtInfoCfgName[2][256]	,			{'\0'})
	GVAR2(ToggleVar,	tItemExtInfo,			TOGGLEVAR_ONOFF,	0,	-1,	1 , "ItemExtInfo" , &LoadExtInfo)
	GVAR (BYTE,			nDefaultExtInfoColour,				8)
#endif
//--- m_ViewUnitStat.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_VIEWUNITSTAT
	void ViewUnitStatPatch_ASM();
	void ViewPetStatPatch_ASM();
	int SwitchStatPage(int nStep);
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tShowBaseStat,				TOGGLEVAR_ONOFF,	1,	-1,	1 , "Show Player Base Stat")
GVAR2(ToggleVar,	tFirstPlayerStat,			TOGGLEVAR_DOWNPARAM,	0,	-1,	1 , "FirstPlayerStat", &SwitchStatPage		,0)
GVAR2(ToggleVar,	tNextPlayerStat,			TOGGLEVAR_DOWNPARAM,	0,	-1,	1 , "NextPlayerStat", &SwitchStatPage		,1)
GVAR2(ToggleVar,	tPrevPlayerStat,			TOGGLEVAR_DOWNPARAM,	0,	-1,	1 , "PrevPlayerStat", &SwitchStatPage		,-1)
#endif
//--- m_ViewTargetUnitInfo.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_VIEWTARGETUNITINFO
	void GetViewingStatsPlayerPatch1_ASM();
	void GetViewingStatsPlayerPatch2_ASM();
	UnitAny* GetViewStatUnit();
	UnitAny*  GetViewEquUnit();
	void SetViewUnit();
	int ViewUnit(int nType);
	void GetUnitDrawEquipItemPatch_ASM();
	void GetUnitDrawItemDescPath_ASM();
	void GetUnitTestHasItem_ASM();
	void GetUnitDrawInvItemPatch_ASM();
	void GetUnitProtectClickEquipPatch_ASM();
	void GetUnitProtectClickInvPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR (int,			fViewingTargetUnit,				0)
 GVAR2(ToggleVar,	tViewEquipmentKey,	    TOGGLEVAR_DOWNPARAM,	0,	-1,  1,	"ViewEquipmentKey", &ViewUnit ,UIVAR_INVENTORY)
 GVAR2(ToggleVar,	tViewPlayerStatsKey,    TOGGLEVAR_DOWNPARAM,	0,	-1,	 1, "ViewPlayerStatsKey", &ViewUnit ,UIVAR_STATS)
#endif
//--- m_MonsterLifeBar.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_MONSTERLIFEBAR
	void MonsterDescCommaPatch1_ASM();
	void MonsterDescCommaPatch2_ASM();
	void MonsterDescCommaPatch3_ASM();
	void MonsterNamePatch_ASM();
	void InitTCList();
	MonTC *GetTcLevel(WORD tcno , DWORD mlvl);
	void LifeBarPatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tMonsterLevel,				TOGGLEVAR_ONOFF,	0,	-1,	1	,"Monster Level")
GVAR2(ToggleVar,	tMonsterTC,					TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterTC")
GVAR2(ToggleVar,	tMonsterRuneTC,				TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterRuneTC")
GVAR2(ToggleVar,	tMonsterResists,			TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterResists")
GVAR2(ToggleVar,	tMonsterHPPercent,		    TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterHPPercent")
GVAR2(ToggleVar,	tMonsterDistance,			TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterDistance")
GVAR2(ToggleVar,	tMonsterID,					TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterID")
GVAR2(ToggleVar,	tSPMonsterHP,		    TOGGLEVAR_ONOFF,	0,	-1,	1	,"SinglePlayerMonsterHP")
GVAR2(ToggleVar,	tSPMonsterExp,		    TOGGLEVAR_ONOFF,	0,	-1,	1	,"SinglePlayerMonsterExp")
GVAR (BYTE,			nLifeBarTrans,				(BYTE)-1)
GVAR (BYTE,			nLifeBarColour,				(BYTE)-1)
#endif
//--- m_PlayerLifeBar.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_PLAYERLIFEBAR
	void PlayerNamePatch_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tPlayerLevel,				TOGGLEVAR_ONOFF,	0,	-1,	1, "PlayerLevel")
GVAR2(ToggleVar,	tPlayerDistance,			TOGGLEVAR_ONOFF,	0,	-1,	1, "PlayerDistance")
GVAR2(ToggleVar,	tPlayerHPPercent,			TOGGLEVAR_ONOFF,	0,	-1,	1, "PlayerHPPercent")
#endif
//--- m_CheckDangerous.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_CHECKDANGEROUS
	void ChickenLifeNewGame();
	void ChickenLife();
	void CheckDangerousPlayer( UnitAny  *pUnit );
	void CheckDangerousMonster( UnitAny  *pUnit );
#endif
#ifdef VARIABLE_DEFINE
GVAR (BOOL,			fLifeProtectOn,			FALSE)
GVAR (BOOL,			fDangerousMonsterActive,FALSE)
GVAR (int,		dwAutoPotionCheckMs,						1000)
GVAR (int,		dwIronGolemLifeAlertPercent,						0)
GVAR (int,		dwIronGolemLifeTownPercent,						0)
GVAR (int,		dwIronGolemLifeExitPercent,						0)
GVAR (int,		dwIronGolemTownLifePercent,			0)
GVAR (int,		dwHirePotionMinCLevel,						0)
GVAR (int,		dwHirePotionLifePercent,						0)
GVAR (int,		dwHirePotionColumn,						0)
GVAR (int,		dwHirePotionDelayMs,						3000)
GVAR (int,		dwManaPotionMax,						0)
GVAR (int,		dwManaPotionValue,						0)
GVAR (int,		dwManaPotionNTValue,						0) 
GVAR (int,		dwManaPotionColumn,						0)
GVAR (int,		dwManaPotionDelayMs,						3000)
GVAR (DWORD,		dwChickenLife,						0)
GVAR (DWORD,		dwChickenMaxLife,						0)
GVAR (DWORD,		dwChickenHostileLife,				0)
GVAR (DWORD,		dwChickenHostileNearbyLife,			0)
GVAR (DWORD,		dwChickenLifePercent,				0)
GVAR (DWORD,		dwChickenHostileLifePercent,		0)
GVAR (DWORD,		dwChickenHostileNearbyLifePercent,	0)
GVAR (DWORD,		nChickenLifeAction,					1)
GVAR2(ToggleVar,	tChickenLife,			TOGGLEVAR_ONOFF,	0,	-1,	1,	"Chicken life")
GVAR2(ToggleVar,	tChickenHostile,		TOGGLEVAR_ONOFF,	0,	-1,	1,	"Chicken hostile")
GVAR2(ToggleVar,	tChickenHostileNearby,	TOGGLEVAR_ONOFF,	0,	-1,	1,	"Chicken hostile nearby")
GVAR2(ToggleVar,	tChickenDangerousMonster,		TOGGLEVAR_ONOFF,	0,	-1,	1, "Chicken Dangerous Monster")
GVAR2(ToggleVar,	tAutoPotion,			TOGGLEVAR_ONOFF,	0,	-1,	1,	"Auto Potion")
GVAR2(char,			anDangerousMonster[1000][2]	,		{0})
GVAR (DWORD,		nDangerousMonsterAction,			2)
GVAR (DWORD,		dwChickenLifeMinClevel,						0)
GVAR (DWORD,		dwChickenLifeForcedClevel,						90)
GVAR (int,		dwChickenLifeEnterGame,						1)
#endif
//--- m_QuestProtect.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_QUESTPROTECT
	void DrawQuestPagePatch_ASM();
	int ReSetTimer();
	void InitBugInfo();
	void SetBugQuestStatus();
	void ShowBugInfo() ;
	void RecvCommand_5D_Patch_ASM();
	void RecvCommand_89_Patch_ASM();
	void CheckUnitKM();
	void checkAllBugQuests();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tBugKCountess,TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug KCountess Protect" , &ReSetTimer)
GVAR2(ToggleVar,	tBugKM,				TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug KM Protect" , &ReSetTimer)
GVAR2(ToggleVar,	tBugKD,				TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug KD Protect" , &ReSetTimer)
GVAR2(ToggleVar,	tBugKB,				TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug KB Protect" , &ReSetTimer)
GVAR2(ToggleVar,	tBugAutoQuit,	    TOGGLEVAR_ONOFF,	1,	-1,	1, "Bug Auto Quit Toggle"	,&ReSetTimer)
GVAR2(ToggleVar,	tBugAutoQuitHell,	    TOGGLEVAR_ONOFF,	1,	-1,	1, "Bug Auto Quit Toggle(Hell)"	,&ReSetTimer)
GVAR2(ToggleVar,	tBugAutoQuitHellAct5,	TOGGLEVAR_ONOFF,	1,	-1,	1, "Bug Auto Quit Toggle(HellAct5)"	,&ReSetTimer)
GVAR2(ToggleVar,	tBugAllHellAlert,	    TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug All Hell Aert Toggle(Hell)"	,&ReSetTimer)
GVAR (DWORD,		dwBugAlertTimes,			10)
GVAR2(ToggleVar,	tAlertNoBug,		TOGGLEVAR_ONOFF,	1,	-1,	1, "AlertNoBug")
GVAR2(wchar_t,wszBugKBAlertMessage[256],0)
#endif
//--- m_IMEProtect.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_IMEPROTECT
	void ToggleIMEInput(BOOL fEnable);
#endif
//--- m_ItemSocketProtect.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_ITEMSOCKETPROTECT
	void SocketProtectPatch1_ASM();
	void SocketProtectPatch2_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tSocketProtect,				TOGGLEVAR_ONOFF,	0,	-1,	1 , "Socket protect")
#endif
//--- m_GameChat.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_GAMECHAT
	void WisperPatch_ASM();
	void InputLinePatch1_ASM();
	void InputLinePatch2_ASM();
	void CheckD2WinEditBox();
	void SelectTextPatch1_ASM();
	void SelectTextPatch2_ASM();
#endif
#ifdef VARIABLE_DEFINE
 GVAR2(ToggleVar,	tWisperNotice,				TOGGLEVAR_ONOFF,	0,	-1,	1, "Wisper Notice")
 GVAR2(ToggleVar,	tInputLine,					TOGGLEVAR_ONOFF,	1,	-1,	1, "Input line Patch")
 GVAR2(ToggleVar,	tUseCustomFont,				TOGGLEVAR_ONOFF,	0,	-1,	1, "Use Custom Font")
 GVAR (D2EditBox*,	pD2WinEditBox,				0)
#endif
//--- m_UnicodeSupport.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_UNICODESUPPORT
	LPCSTR __fastcall D2Lang_Unicode2WinPatch(LPSTR lpWinStr, LPWSTR lpUnicodeStr, DWORD dwBufSize);
	LPWSTR __fastcall D2Lang_Win2UnicodePatch(LPWSTR lpUnicodeStr, LPCSTR lpWinStr, DWORD dwBufSize);
	DWORD __fastcall ChannelEnterCharPatch(D2EditBox *hWnd, BYTE bKeyCode);
	void	MultiByteFixPatch_ASM();
	void	IsPritableCharacterPatch_ASM();
#endif
//--- m_Socket.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_SOCKET
	void FixSocketTimeOut_ASM();
	void FixSocketErrRet_ASM();
#endif
#ifdef VARIABLE_DEFINE
GVAR (DWORD,		dwSocketTimeOutSec,		100)
GVAR (DWORD,		dwSocketTimeOut,		100000) 
GVAR (BOOL,			fFixSocketError,		TRUE)
#endif
//--- m_GameWindow.h ---
#ifdef FUNCTION_DECLARE
	#define MODULE_GAMEWINDOW
	int KeepWindowPatch();
	void RemoveKeepWindowPatch();
	void NoHidePatch_ASM();
	BOOL IsFullWindow();
#endif
#ifdef VARIABLE_DEFINE
GVAR2(ToggleVar,	tKeepGameWindow,		TOGGLEVAR_ONOFF,	0,	-1,		1 , "Keep game window",		&KeepWindowPatch)
 GVAR2(ToggleVar,	tNoHide,				TOGGLEVAR_ONOFF,	1,	-1,		1,	"No Hide")
 GVAR(BOOL  , fFullWindows , 0)
#endif
//--- m_MultiClient.h ---
#define MODULE_PACKETHANDLER
#ifdef FUNCTION_DECLARE
	void MultiClientNewGame();
	void MultiClientLoop();
	int MultiClientStartFollow();
	int MultiClientStopFollow();
	int MultiClientEnterDoor();
	int MultiClientRetreat();
	int MultiClientStartClick(int param);
	int MultiClientStopClick(int param);
	void startFollowId(int uid);
	void stopFollow();
	void enterDoor(int uid,int area);
	void retreat(int uid);
	void multiclient_left_xy(int x,int y);
	void multiclient_right_xy(int x,int y);
	void multiclient_left_unit(int type,int id);
	void multiclient_right_unit(int type,int id);
#endif
#ifdef VARIABLE_DEFINE
	GVAR2(ToggleVar,	tMultiClient,				TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Follow" )
	GVAR2(ToggleVar,	tMultiClientStartFollow,	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientStartFollow",&MultiClientStartFollow)
	GVAR2(ToggleVar,	tMultiClientStopFollow,	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientStopFollow",&MultiClientStopFollow)
	GVAR2(ToggleVar,	tMultiClientEnterDoor,	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientEnterDoor",&MultiClientEnterDoor)
	GVAR2(ToggleVar,	tMultiClientRetreat,	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientRetreat",&MultiClientRetreat)
	GVAR2(ToggleVar,	tMultiClientClick,	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientClick",&MultiClientStartClick)
	GVAR (int,			fTransferClick,					0)
	GVAR2 (wchar_t,			wszTransferClick[64],					0)
	GVAR (int,			dwMultiClientDistance,					5)
	GVAR (int,			dwMultiClientMaxDistance,					100)
	GVAR (int,			dwMultiClientMoveDistance,					20)
	GVAR (int,			dwMultiClientOverAreaDistance,					20)
	GVAR (int,			dwMultiClientCount,					0)
	GVAR (int,			dwMultiClientFollowId,					0)
	GVAR (int,			dwMultiClientClickUnitType,					0)
	GVAR (int,			dwMultiClientClickUnitId,					0)
	GVAR (int,			dwMultiClientClickRight,					0)
	GVAR (int,			dwMultiClientClickTimeout,					0)
	GVAR2(ToggleVar,	tNecNoAttackInHell,				TOGGLEVAR_ONOFF,	0,	-1,	1,  "NecNoAttackInHell")
#endif
//--- m_DropProtection.h ---
#ifdef FUNCTION_DECLARE
	int DropProtection(UnitAny *pItem);
#endif
#ifdef VARIABLE_DEFINE
	GVAR2(ToggleVar,	tDropProtectionToggle,				TOGGLEVAR_ONOFF,	0,	-1,	1,  "Drop Protection" )
	GVAR2(ToggleVar,	tResetProtectionToggle,				TOGGLEVAR_ONOFF,	0,	-1,	1,  "Reset Protection" )
	GVAR2(char,			aDropProtectRune[34]	,0)
	GVAR2(char,			aDropProtectRuneword[256]	,0)
	GVAR2(char,			aDropProtectUnique[512]	,0)
	GVAR2(char,			aDropProtectSet[256]	,0)
#endif
//--- m_AutoPickup.h ---
#ifdef FUNCTION_DECLARE
	void AutoPickupNewGame();
	void AutoPickupLoop();
	int AutoPickupItem();
#endif
#ifdef VARIABLE_DEFINE
	GVAR2(ToggleVar,	tAutoPickupToggle,				TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Pickup" )
	GVAR2(char,			aAutoPickupItem[3000]	,0)
#endif
//--- m_PacketHandler.h ---
#define MODULE_PACKETHANDLER
#ifdef FUNCTION_DECLARE
	void GamePacketReceivedInterceptPatch_ASM();
	void GamePacketSendInterceptPatch_ASM();
	void ResetPacketHandlerTimer();
	int DoTest();
#endif
#ifdef VARIABLE_DEFINE
	GVAR (int,			fMonitorPacket,					0)
	GVAR2(ToggleVar,	tPacketHandler,				TOGGLEVAR_ONOFF,	0,	-1,	1,  "NetworkPacket",&ResetPacketHandlerTimer )
	GVAR2 (char,			fMonitorSendCmd[256],					0)
	GVAR2 (char,			fMonitorRecvCmd[256],					0)
	GVAR2(ToggleVar,	tTestKey,			TOGGLEVAR_DOWN,	0,	-1,		1, "TestKey",			&DoTest)
	GVAR2(ToggleVar,	tNecTeleportAttract,				TOGGLEVAR_ONOFF,	0,	-1,	1,  "NecTeleportAttract")
#endif
//--- m_end.h ---
#ifdef VARIABLE_DEFINE
	#undef GVAR
	#undef GVAR2
#endif
