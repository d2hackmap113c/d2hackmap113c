#include "stdafx.h"
#include "header.h"
#include "multi.h"
#include "auto.h"

extern char *szVersion,*compileTime;
static char *wszProgTitle = "<Hackmap>: Sting's Hackmap fixed by ding then ...(%s) (%s) https://github.com/d2hackmap113c/d2hackmap113c/";
int dwLoopCount=0;
extern int mainMenuLoopCount; 
int fDebugMs=0;
int dwChangeLeftSkill;
int dwLevelChangeMs=0;
static BOOL fImmEnable = 1;
int dwCpuUser=0,dwCpuKernel=0,dwLoopFPS=0,dwDrawFPS=0;
int exitGameMs;
volatile int dwCurMs=0;
int dwEnterGameMs=0,fStartingGame=0,fInGame=0,fPlayerInTown=1;
int dwEnterGameShowDifficultyMs=0;
int actlvls[6]={1, 40, 75, 103, 109, 137};
int dwCurrentAct=0,dwCurrentLevel=1;
int dwPlayerX=0,dwPlayerY=0,dwPlayerId=0,dwPlayerPartyId=0,dwPlayerClass=0;
Skill *pLeftSkill=NULL,*pRightSkill=NULL;
int dwLeftSkill=-1,dwRightSkill=-1;
int dwLeftSkillStatus=0,dwRightSkillStatus=0;
int fCanUseLeftSkill=1,fCanUseRightSkill=1;
int dwPlayerLevel=0;
int dwPlayerHP=0,dwPlayerMaxHP=0;
int dwPlayerMana=0,dwPlayerMaxMana=0;
int dwPlayerFcrFrame=0,dwPlayerFcrMs=0;
int dwSlowMs=100,dwSlowSkipMs=1000,dwSlowRestoreMs=0;
int screenDrawX,screenDrawY;
int autoTerminateNpcInteractMs;
extern int autoNpcTxt,npcChatTxt,clickAcceptTradeMs;

static int fShowDllVersion=1,dwMinLoopMs=0,dwMinHackmapMs=0,dwMinHackmapMsInTown=0,dwSkillChangeCountVerify=0;
static int lastExp;
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_INT, "ShowDllVersion",&fShowDllVersion,4},
	{CONFIG_VAR_TYPE_INT, "MinLoopMs",&dwMinLoopMs,4},
	{CONFIG_VAR_TYPE_INT, "MinHackmapMs",&dwMinHackmapMs,4},
	{CONFIG_VAR_TYPE_INT, "MinHackmapMsInTown",&dwMinHackmapMsInTown,4},
	{CONFIG_VAR_TYPE_INT, "EnterGameShowDifficultyMs",&dwEnterGameShowDifficultyMs,  4},
	{CONFIG_VAR_TYPE_INT, "SlowMs",&dwSlowMs,4},
	{CONFIG_VAR_TYPE_INT, "SlowSkipMs",&dwSlowSkipMs,4},
};
void loop_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}

extern int dwDrawCount,dwBackToTownTimeout,dwPartyResponseMs,dwOrgMode,dwNpcTradeCheckMs,dwQuestAlertMs;
extern int dwPartyInvideMs;
extern World *singlePlayerWorld;
extern ToggleVar tBugAllHellAlert,tAutoMap;
extern int nDrawInvItems;
extern int dwUpdateQuestMs;
extern int dwSendPacketCount;
extern int marketItemCount,marketItemCount1;
extern int bossHps[3],bossX,bossY,bossId,nBoss;
extern ToggleVar tLogMonsterExp;

void recheckSelfItems();
void ShowWarningMessage();
void ResponseInvite();void AutoInvite();
void CheckD2WinEditBox();
extern D2EditBox*	pD2WinEditBox;
void SetViewUnit();
extern int fViewingTargetUnit;
void WinMessageNewGame();void WinMessageRunLoop();void WinMessageEndGame();
void PartyHelpNewGame();void PartyHelpEndGame();
void AutoSkillNewGame();void AutoSkillRunLoop();
void AutoWarCryNewGame();void AutoWarCryRunLoop();
void AutoEnchantNewGame();void AutoEnchantRunLoop();
void QuestNewGame();void QuestRunLoop();void QuestNewLevel();void QuestNewAct();
void SnapshotNewGame();void SnapshotEndGame();void SnapshotRunLoop();
void NpcTradeNewGame();void NpcTradeLoop();
void ChickenLifeNewGame();void ChickenLifeLoop();
void drawInvItemLoop();
void quickLoop();
void PacketNewGame();
void item_NewGame();
void quick_NewGame();void quick_EndGame();
void lifebarNewGame();
void infoNewGame();
void GameMonitorNewGame();void GameMonitorEndGame();
void MessageLogExitGame();
void ItemExtInfoNewGame();
void AutoMapNewGame();void AutoMapNewLevel();
void AutoRouteNewGame();
void WaypointNewGame();
void panelNewGame();void panelEndGame();
void quickswap_NewGame();
extern int chatPasteMs;
void processChatPaste();
void pressESC();
void playerAddExp(int exp);
int canLeaveTown();

#pragma comment(lib,"imm32.lib")   
void ToggleIMEInput(BOOL fChatInput){
	static HIMC hIMC = NULL; 
	if (fChatInput==fImmEnable) return;
	if (fChatInput) ImmAssociateContext(d2gfx_GetHwnd(), hIMC); 
	else hIMC=ImmAssociateContext(d2gfx_GetHwnd(), NULL); 
	fImmEnable = fChatInput;
}
extern ToggleVar tBugAutoQuitHellAct1,tBugAutoQuitHellAct3,tBugAutoQuitHellAct4,tBugAutoQuitHellAct5;
int dwBugFlag=0,fIsHardCoreSafe=0;
/*
static void checkCharName(char *name) {
	if (strstr(name,"bugKM")) {
		if (!tBugAutoQuitHellAct3.isOn) gameMessage("BugAutoQuitHellAct3 is on");
		tBugAutoQuitHellAct3.isOn=1;dwBugFlag|=4;
	} else if (strstr(name,"bugKD")) {tBugAutoQuitHellAct4.isOn=1;dwBugFlag|=8;}
	else if (strstr(name,"bugKB")) {tBugAutoQuitHellAct5.isOn=1;dwBugFlag|=16;}
	else if (strstr(name,"bugKCountess")) {tBugAutoQuitHellAct1.isOn=1;dwBugFlag|=1;}
	if (dwBugFlag) gameMessage("BugFlag=%X",dwBugFlag);
}
*/
static void checkTag(char *tag) {
	if (_stricmp(tag,"bugKM")==0) {tBugAutoQuitHellAct3.isOn=1;dwBugFlag|=4;}
	else if (_stricmp(tag,"bugKD")==0) {tBugAutoQuitHellAct4.isOn=1;dwBugFlag|=8;}
	else if (_stricmp(tag,"bugKB")==0) {tBugAutoQuitHellAct5.isOn=1;dwBugFlag|=16;}
	else if (_stricmp(tag,"bugKCountess")==0) {tBugAutoQuitHellAct1.isOn=1;dwBugFlag|=1;}
	else if (_stricmp(tag,"safe")==0) {fIsHardCoreSafe=1;}
}
char *getCharTag(char *name);
extern int fPartyListValid,dwPetListChangeCount,dwPetListChangeVerify;
extern int fInMainMenu;
int fUsingHardCoreConfig=0;
int fIsHardCoreGame=0,fHasHardCoreConfig=0,needReloadConfig=0;
static void gameStart() {
	if (fInMainMenu) fInMainMenu=0;
	LOG("start game looping\n");
	fPartyListValid=0;dwPetListChangeCount=1;dwPetListChangeVerify=0;
//--- m_pub.h ---
	dwPlayerFcrMs=600;dwSkillChangeCount=1;dwSkillChangeCountVerify=0;
	dwCurMs=GetTickCount();
	singlePlayerWorld=NULL;
	dwPlayerId = PLAYER->dwUnitId;
	dwPlayerClass = getPlayerClass(dwPlayerId);
	fIsHardCoreGame=*d2client_pGameInfo&&(*d2client_pGameInfo)->nGameMode&4;
	if (dwPlayerClass<0) dwPlayerClass=0;
	if (dwPlayerClass>6) dwPlayerClass=6;
	dwRecheckSelfItemMs=dwCurMs+1000;
	dwChangeLeftSkill=-1;
	dwRightSkill=-1;fAutoSummon=0;fAutoSkill=0;fAutoEnchant=0;
	if (tAutoMap.isOn) d2client_ShowMap();
	AutoRouteNewGame();
	AutoMapNewGame();PartyHelpNewGame();WinMessageNewGame();AutoEnchantNewGame();AutoWarCryNewGame();
	SnapshotNewGame();infoNewGame();quick_NewGame();GameMonitorNewGame();ItemExtInfoNewGame();
	lifebarNewGame();QuestNewGame();MultiClientNewGame();AutoSummonNewGame();AutoSkillNewGame();
	NpcTradeNewGame();ChickenLifeNewGame();PacketNewGame();item_NewGame();WaypointNewGame();
	panelNewGame();quickswap_NewGame();
	if (dwEnterGameShowDifficultyMs) {
		int color=fIsHardCoreGame?1:2;
		if (EXPANSION) setBottomAlertMsg(0,dwGameLng?L"资料片":L"Expansion",dwEnterGameShowDifficultyMs,0,color,0);
		else setBottomAlertMsg(0,dwGameLng?L"经典版":L"Classic",dwEnterGameShowDifficultyMs,0,color,0);
		switch (DIFFICULTY) {
			case 1:setBottomAlertMsg(1,dwGameLng?L"噩梦难度":L"Nightmare Difficulty",dwEnterGameShowDifficultyMs,0,color,0);break;
			case 2:setBottomAlertMsg(1,dwGameLng?L"地狱难度":L"Hell Difficulty",dwEnterGameShowDifficultyMs,0,color,0);break;
			default:setBottomAlertMsg(1,dwGameLng?L"普通难度":L"Normal Difficulty",dwEnterGameShowDifficultyMs,0,color,0);break;
		}
		if (fIsHardCoreGame) setBottomAlertMsg(2,dwGameLng?L"专家级":L"Hard Core",dwEnterGameShowDifficultyMs,0,1,0);
	}
	if (tPacketHandler.isOn) setBottomAlertMsg(3,L"Warning: Network Packet Patch Installed",6000,1,1,2);
	dwSendPacketCount=0;
	char *name=(*d2client_pGameInfo)->szCharName;
	dwBugFlag=0;fIsHardCoreSafe=0;
	//checkCharName(name);
	char *tag=getCharTag(name);
	while (tag&&*tag) {
		char *e=strchr(tag,',');
		if (e) {*e=0;checkTag(tag);*e=',';tag=e+1;}
		else {checkTag(tag);break;}
	}
	autoTerminateNpcInteractMs=0;
	lastExp=d2common_GetUnitStat(PLAYER,STAT_EXP,0);
	if (fHasHardCoreConfig&&fUsingHardCoreConfig!=fIsHardCoreGame) {
		if (fIsHardCoreGame)
			gameMessageW(dwGameLng?L"进入专家游戏，需重新加载配置":L"Enter Hard Core game, need to reload config");
		else
			gameMessageW(dwGameLng?L"进入非专家游戏，需重新加载配置":L"Enter Non-HC game, need to reload config");
		needReloadConfig=1;
	}
}

void GameLoopPatch() {
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	static int lastAct,lastLevel,lastMs=0,calFpsMs,lastLoopMs=0;
	if (!fInGame) {
		if (!gameHeap) gameHeap=HeapCreate(0,0,0); //Destroyed at packet.cpp:case 0x68: {//Game Join Request
		fInGame=1;fStartingGame=0;
		fIsSinglePlayer=(*d2client_pGameInfo)->szGameServerIp[0]==0;
		fIsRealmClient=(*d2client_pGameInfo)->szRealmName[0]!=0;
		fIsTcpIp=!fIsRealmClient&&!fIsSinglePlayer;
		if (fShowDllVersion) gameMessage(wszProgTitle,szVersion,compileTime);
		ShowWarningMessage();
		//if(fEnterGameSound) MessageBeep(MB_ICONEXCLAMATION);
		QueryPerformanceCounter(&perfStart);
		gameStart();
		QueryPerformanceCounter(&perfEnd);
		QueryPerformanceFrequency(&perfFreq);
		double timeMs=(perfEnd.QuadPart-perfStart.QuadPart)*1000.0/perfFreq.QuadPart;
		LOG("Enter game time %.3lf ms\n",timeMs);
		lastAct=-1;lastLevel=-1;
		dwCurMs=dwEnterGameMs=GetTickCount();
		lastMs=dwCurMs-dwMinHackmapMs;lastLoopMs=0;
		calFpsMs=dwCurMs+1000;dwLoopCount=0;
	}
	dwCurMs=GetTickCount();
	int loopMs=GetTickCount()-lastLoopMs;
	if (loopMs<dwMinLoopMs) Sleep(dwMinLoopMs-loopMs);
	if (loopMs>dwSlowMs&&dwCurMs-dwEnterGameMs>3000) dwSlowRestoreMs=dwCurMs+dwSlowSkipMs;
	lastLoopMs=GetTickCount();
	if (dwSlowRestoreMs&&dwCurMs>dwSlowRestoreMs) dwSlowRestoreMs=0;
	dwLoopCount++;
	if (dwCurMs>calFpsMs) {
		static int ln=0,dn=0;
		dwLoopFPS=dwLoopCount-ln;dwDrawFPS=dwDrawCount-dn;
		ln=dwLoopCount;dn=dwDrawCount;
		get_cpu_usage(&dwCpuUser,&dwCpuKernel);
		calFpsMs=dwCurMs+1000;
		if (!fWinActive) multiclient_send_info((MCI_CpuUsage<<24)|((dwCpuUser+dwCpuKernel)<<8)|(dwGameWindowId<<24));
	}
	if (fDebugMs) fDebugMs=0;
	if (dwCurMs-lastMs>100) {
		/*if (tShowTestInfo.isOn&&dwCurMs-dwEnterGameMs>10000) {
			static int slowN=0;
			slowN++;
			if (slowN<30) {
				fDebugMs=1;
				LOG("slow frame=%d ms\n",dwCurMs-lastMs);
			}
		}*/
	}
	if (!fPlayerInTown&&dwCurMs-lastMs<dwMinHackmapMs||fPlayerInTown&&dwCurMs-lastMs<dwMinHackmapMsInTown) return;
	lastMs=dwCurMs;
	nBoss=bossId;
	bossId=0;
	//QueryPerformanceCounter(&perfStart);
	screenDrawX=d2client_GetScreenDrawX();
	screenDrawY=d2client_GetScreenDrawY();
	dwCurrentAct = PLAYER->dwAct;//ACT->dwActNo
	dwCurrentLevel = PLAYER->pMonPath->pAreaRectData->pAreaRectInfo->pDrlgLevel->dwLevelNo;
	dwPlayerX=PLAYER->pMonPath->wUnitX;
	dwPlayerY=PLAYER->pMonPath->wUnitY;
	dwPlayerLevel=d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0);
	fPlayerInTown = (LEVELNO==actlvls[ACTNO]);
	pLeftSkill=PLAYER->pSkill->pLeftSkill;
	pRightSkill=PLAYER->pSkill->pRightSkill;
	fCanUseLeftSkill=skillUnusable(PLAYER,PLAYER->pSkill->pLeftSkill)==0;
	fCanUseRightSkill=skillUnusable(PLAYER,PLAYER->pSkill->pRightSkill)==0;
	dwPlayerMaxHP = d2common_GetUnitStat(PLAYER, STAT_MAXHP, 0)>>8;
	dwPlayerHP = d2common_GetUnitStat(PLAYER, STAT_HP, 0)>>8;
	dwPlayerMaxMana = d2common_GetUnitStat(PLAYER, STAT_MAXMANA, 0)>>8;
	dwPlayerMana = d2common_GetUnitStat(PLAYER, STAT_MANA, 0)>>8;
	for (RosterUnit *pUnit=PLAYERLIST;pUnit;pUnit=pUnit->pNext) {
		if (pUnit->dwUnitId==dwPlayerId) dwPlayerPartyId=pUnit->wPartyId;
	}
	int lk=pLeftSkill->pSkillInfo->wSkillId;
	int rk=pRightSkill->pSkillInfo->wSkillId;
	if (dwLeftSkill!=lk||dwRightSkill!=rk) {
		dwLeftSkill=lk;dwRightSkill=rk;
		checkAutoSkill();
	}
	if (dwSkillChangeCountVerify!=dwSkillChangeCount) {
		dwPlayerFcrFrame=getFcrFrame(dwPlayerClass,PLAYER);
		dwPlayerFcrMs=dwPlayerFcrFrame*40;
		dwSkillChangeCountVerify=dwSkillChangeCount;
	}
	if (lastAct!=dwCurrentAct) {
		lastAct=dwCurrentAct;
		QuestNewAct();
		bossHps[0]=0;
		bossHps[1]=0;
		bossHps[2]=0;
	}
	if (lastLevel!=dwCurrentLevel) {
		lastLevel=dwCurrentLevel;
		dwLevelChangeMs=dwCurMs;
		dwAutoSummonStartMs=dwCurMs;
		if (tBugAllHellAlert.isOn&&DIFFICULTY==2
			||dwCurrentLevel==46||66<=dwCurrentLevel&&dwCurrentLevel<=72) QuestNewLevel();
		AutoMapNewLevel();
		bossX=0;bossY=0;
		if (fIsHardCoreGame&&!fPlayerInTown&&!canLeaveTown()) {
			if (PLAYER&&PLAYER->dwMode!=PlayerMode_Death&&PLAYER->dwMode!=PlayerMode_Dead) {
				gameMessage("Hardcore leave town protect. Exit game");
				ExitGame();
			}
		}
	}
	{
		int exp=d2common_GetUnitStat(PLAYER,STAT_EXP,0);
		int add=exp-lastExp;
		if (add) {
			playerAddExp(add);
			if (tLogMonsterExp.isOn) {
				if (add>=1000000) {LOG("Player exp +%.2fM\n",add/1000000.0);}
				else if (add>=1000) {LOG("Player exp +%.2fK\n",add/1000.0);}
				else {LOG("Player exp %+d\n",add);}
			}
		}
		lastExp=exp;
	}
	if (dwChangeLeftSkill!=-1) {selectSkill(0,dwChangeLeftSkill);dwChangeLeftSkill=-1;}
	if (dwBackToTownTimeout) quickLoop();
	if (dwPartyResponseMs&&dwCurMs>dwPartyResponseMs) ResponseInvite();
	if (dwPartyInvideMs&&dwCurMs>dwPartyInvideMs) AutoInvite();
	if (fViewingTargetUnit) SetViewUnit();
	if (dwQuestAlertMs&&dwCurMs>=dwQuestAlertMs
		||dwUpdateQuestMs&&dwCurMs>=dwUpdateQuestMs) QuestRunLoop();
	dwOrgMode = 0;
	ChickenLifeLoop();
	int fChatInput=*d2client_pUiChatOn;//d2client_CheckUiStatus(UIVAR_CHATINPUT);
	if (fChatInput!=fImmEnable) ToggleIMEInput(fChatInput);
	if (pD2WinEditBox) CheckD2WinEditBox();
	MultiClientLoop();
	if (dwPlayerClass==2||fAutoSummon) AutoSummonRunLoop();
	else if (dwAutoSummonStartMs) dwAutoSummonStartMs=0;
	if (fAutoEnchant) AutoEnchantRunLoop();
	if (fAutoWarCry) AutoWarCryRunLoop();
	if (fAutoSkill) AutoSkillRunLoop();
	else if (dwAutoSkillCheckMs) dwAutoSkillCheckMs=0;
	if (dwNpcTradeCheckMs&&dwCurMs>dwNpcTradeCheckMs) NpcTradeLoop();
	WinMessageRunLoop();
	if (dwRecheckSelfItemMs&&dwCurMs>dwRecheckSelfItemMs) recheckSelfItems();
	*d2client_pShowLifeStr=*d2client_pAutomapOn;
	*d2client_pShowManaStr=*d2client_pAutomapOn;
	if (chatPasteMs&&dwCurMs>chatPasteMs) processChatPaste();
	marketItemCount=marketItemCount1;
	marketItemCount1=0;
	if (!fWinActive) {
		if (autoTerminateNpcInteractMs&&dwCurMs>autoTerminateNpcInteractMs) {
			if (npcChatTxt) { //*d2client_pUiInteractOn
				autoTerminateNpcInteractMs=dwCurMs+300;
				LOG("AutoTerminateNpcInteract\n");
				pressESC();
			} else if (!autoNpcTxt) 
				autoTerminateNpcInteractMs=0;
		}
		if (clickAcceptTradeMs&&dwCurMs>clickAcceptTradeMs) {
			clickAcceptTradeMs=0;
			d2client_clickAcceptTradeMenuItem();
		}
	}
	if (needReloadConfig) {
		needReloadConfig=0;
		ReloadConfig();
	}
}

void GameEndPatch() {
	if (!fInGame) return;
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	QueryPerformanceFrequency(&perfFreq);
	QueryPerformanceCounter(&perfStart);
	fStartingGame=0;fInGame=0;fPlayerInTown=1;
	PartyHelpEndGame();WinMessageEndGame();MultiClientEndGame();SnapshotEndGame();MessageLogExitGame();
	quick_EndGame();GameMonitorEndGame();ToggleIMEInput(1);
	panelEndGame();
	if (pD2WinEditBox) {
		*(DWORD*)d2client_pLastChatMessage = 0;
		wchar_t *p = wcscpy(d2client_pLastChatMessage, d2win_GetEditBoxText(pD2WinEditBox));
		*d2client_pChatTextLength = wcslen(p);
		d2win_DestroyEditBox(pD2WinEditBox);
		pD2WinEditBox = NULL;
		FOCUSECONTROL = NULL;
	}
	if (gameHeap) {HeapDestroy(gameHeap);gameHeap=NULL;}
	QueryPerformanceCounter(&perfEnd);
	double timeMs=(perfEnd.QuadPart-perfStart.QuadPart)*1000.0/perfFreq.QuadPart;
	LOG("Exit game time %.3lf ms\n",timeMs);
	LOG("Send %d packet\n",dwSendPacketCount);
	dwPlayerId=0;
	exitGameMs=dwCurMs;
	mainMenuLoopCount=0;
}
void __declspec(naked) GameLoopPatch_ASM() {
	__asm {
		pop eax
		sub esp, 0x20
		mov [esp+0x0c], ecx
		push eax
		jmp GameLoopPatch
	}
}
void __declspec(naked) GameEndPatch_ASM() {
	__asm {
		call GameEndPatch
		mov ecx,0xb4
		ret
	}
}

