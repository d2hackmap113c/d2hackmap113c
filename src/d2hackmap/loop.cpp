#include "stdafx.h"
#include "d2ptrs.h"
#include "multi.h"
#include "auto.h"

char *szVersion="20241127";
static char *wszProgTitle = "<Hackmap>: Sting's Hackmap For Diablo II fixed by ding (v1.13c.1.4) (%s) https://github.com/d2hackmap113c/d2hackmap113c/";
int fDebugMs=0;
int dwChangeLeftSkill;
int dwLevelChangeMs=0;
static BOOL fImmEnable = 1;

static int dwMinLoopMs=0,dwMinHackmapMs=0,dwMinHackmapMsInTown=0,dwSkillChangeCountVerify=0;
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_INT, "MinLoopMs",&dwMinLoopMs,4},
	{CONFIG_VAR_TYPE_INT, "MinHackmapMs",&dwMinHackmapMs,4},
	{CONFIG_VAR_TYPE_INT, "MinHackmapMsInTown",&dwMinHackmapMsInTown,4},
};
void loop_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}

extern int dwDrawCount,dwBackToTownTimeout,dwPartyResponseMs,dwOrgMode,dwNpcTradeCheckMs,dwQuestAlertMs;
extern ToggleVar tBugAllHellAlert,tAutoMap;
extern int dwUpdateQuestMs;
extern int dwSendPacketCount;

void ResponseInvite();
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

#pragma comment(lib,"imm32.lib")   
void ToggleIMEInput(BOOL fChatInput){
	static HIMC hIMC = NULL; 
	if (fChatInput==fImmEnable) return;
	if (fChatInput) ImmAssociateContext(d2gfx_GetHwnd(), hIMC); 
	else hIMC=ImmAssociateContext(d2gfx_GetHwnd(), NULL); 
	fImmEnable = fChatInput;
}
extern ToggleVar tBugAutoQuitHellAct1,tBugAutoQuitHellAct3,tBugAutoQuitHellAct4,tBugAutoQuitHellAct5;
static void checkTag(char *tag) {
	if (_stricmp(tag,"bugKM")==0) tBugAutoQuitHellAct3.isOn=1;
	else if (_stricmp(tag,"bugKD")==0) tBugAutoQuitHellAct4.isOn=1;
	else if (_stricmp(tag,"bugKB")==0) tBugAutoQuitHellAct5.isOn=1;
	else if (_stricmp(tag,"bugKCountess")==0) tBugAutoQuitHellAct1.isOn=1;
}
char *getCharTag(char *name);
extern int fPartyListValid,dwPetListChangeCount,dwPetListChangeVerify;
static void gameStart() {
	fPartyListValid=0;dwPetListChangeCount=1;dwPetListChangeVerify=0;
//--- m_pub.h ---
	dwPlayerFcrMs=600;dwSkillChangeCount=1;dwSkillChangeCountVerify=0;
	dwCurMs=GetTickCount();
	dwSinglePlayerContext=0;
	dwPlayerId = PLAYER->dwUnitId;
	dwPlayerClass = getPlayerClass(dwPlayerId);
	//LOG("player class=%d\n",dwPlayerClass);
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
	if (dwEnterGameShowDifficultyMs) {
		if (EXPANSION) SetBottomAlertMsg1(dwGameLng?L"资料片":L"Expansion",dwEnterGameShowDifficultyMs,2,0);
		else SetBottomAlertMsg1(dwGameLng?L"经典版":L"Classic",dwEnterGameShowDifficultyMs,2,0);
		switch (DIFFICULTY) {
			case 1:SetBottomAlertMsg2(dwGameLng?L"噩梦难度":L"Nightmare Difficulty",dwEnterGameShowDifficultyMs,2,0);break;
			case 2:SetBottomAlertMsg2(dwGameLng?L"地狱难度":L"Hell Difficulty",dwEnterGameShowDifficultyMs,2,0);break;
			default:SetBottomAlertMsg2(dwGameLng?L"普通难度":L"Normal Difficulty",dwEnterGameShowDifficultyMs,2,0);break;
		}
	}
	if (tPacketHandler.isOn) SetBottomAlertMsg3(L"Warning: Network Packet Patch Installed",6000,1,1);
	dwSendPacketCount=0;
	char *name=(*d2client_pGameInfo)->szCharName;
	char *tag=getCharTag(name);
	while (tag&&*tag) {
		char *e=strchr(tag,',');
		if (e) {*e=0;checkTag(tag);*e=',';tag=e+1;}
		else {checkTag(tag);break;}
	}
}

void GameLoopPatch() {
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	static int lastAct,lastLevel,lastMs=0,calFpsMs,lastLoopMs=0,dwLoopCount=0;
	if (!fInGame) {
		if (gameHeap) HeapDestroy(gameHeap);gameHeap=HeapCreate(0,0,0);
		fInGame=1;
		fSinglePlayer=!(*d2client_pGameInfo)->szGameServerIp[0];
		gameMessage(wszProgTitle,szVersion);
		ShowWarningMessage();
		if(fEnterGameSound) MessageBeep(MB_ICONEXCLAMATION);
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
	lastLoopMs=GetTickCount();
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
	//QueryPerformanceCounter(&perfStart);
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
	if (lastAct!=dwCurrentAct) {lastAct=dwCurrentAct;QuestNewAct();}
	if (lastLevel!=dwCurrentLevel) {
		lastLevel=dwCurrentLevel;
		dwLevelChangeMs=dwCurMs;
		dwAutoSummonStartMs=dwCurMs;
		if (tBugAllHellAlert.isOn&&DIFFICULTY==2
			||dwCurrentLevel==46||66<=dwCurrentLevel&&dwCurrentLevel<=72) QuestNewLevel();
		AutoMapNewLevel();
	}
	if (dwChangeLeftSkill!=-1) {selectSkill(0,dwChangeLeftSkill);dwChangeLeftSkill=-1;}
	if (dwBackToTownTimeout) quickLoop();
	if (dwPartyResponseMs&&dwCurMs>dwPartyResponseMs) ResponseInvite();
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
}

void GameEndPatch() {
	if (!fInGame) return;
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	QueryPerformanceFrequency(&perfFreq);
	QueryPerformanceCounter(&perfStart);
	fInGame=0;fPlayerInTown=1;
	PartyHelpEndGame();WinMessageEndGame();MultiClientEndGame();SnapshotEndGame();MessageLogExitGame();
	quick_EndGame();GameMonitorEndGame();ToggleIMEInput(1);
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

