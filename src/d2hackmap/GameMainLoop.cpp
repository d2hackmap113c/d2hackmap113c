#include "stdafx.h"

static wchar_t *wszProgTitle = L"<Hackmap>: Sting's Hackmap For Diablo II fixed by ding (v1.13c.1.4) (20240624)";
void recheckBelt();
int dwChangeLeftSkill;
extern int dwRecheckBeltMs;
static LONGLONG framePerf;
static int frameN,enterMs;
static BOOL fImmEnable = 1;
static int dwClearDifficultyMs;
#pragma comment(lib,"imm32.lib")   
void ToggleIMEInput(BOOL fChatInput){
	static HIMC hIMC = NULL; 
	if (fChatInput==fImmEnable) return;
	if (fChatInput) ImmAssociateContext(D2GetHwnd(), hIMC); 
	else hIMC=ImmAssociateContext(D2GetHwnd(), NULL); 
	fImmEnable = fChatInput;
}
void runOnce() {
//--- m_pub.h ---
	dwCurMs=GetTickCount();
	dwSinglePlayerContext=0;
	dwPlayerId = PLAYER->dwUnitId;
	dwPlayerClass = getPlayerClass(dwPlayerId);
	//LOG("player class=%d\n",dwPlayerClass);
	if (dwPlayerClass<0) dwPlayerClass=0;
	if (dwPlayerClass>6) dwPlayerClass=6;
	dwRecheckBeltMs=dwCurMs+1000;
	dwChangeLeftSkill=-1;
	if ( dwGameLng<0 ){
		if ( fLanguageCheck ){
			dwGameLng = GetGameLanguage();
			if (dwGameLng==2) {
				if ( fLocalizationSupport==1 && fAutoSetCHISupport&&szCFGVersion[0]){
					fLocalizationSupport = 2;
					wchar_t temp[512];
					GB2GBK(szCFGVersion[1]);
					MyMultiByteToWideChar(CP_ACP, 0, szCFGVersion[1], -1,temp , 0x200);
					wsprintfW(wszCFGVersion[1], L"<Hackmap>: %s " , temp);
				}
				dwGameLng = 1;
			}
		}else{
			dwGameLng = 0 ;
		}
	}
	InitCellFiles();
//--- m_AutoMap.h ---
	ResetMapStatus();
	ShowAutoMap();
//--- m_MinimapPoint.h ---
	ResetMiniCells();
//--- m_PartyHelp.h ---
	if( CheckInvite() ){
		if ( tAutoInvite.isOn ) InviteAll();
		fCanInvite = TRUE;
	}

	if ( tAutoPermit.isOn ){
		PermitAll();
		fCanPermit = TRUE ;
	}
//--- m_WinMessage.h ---
	WinMessageNewGame();
//--- m_AutoEnchant.h ---
	AutoEnchantNewGame();
//--- m_Snapshot.h ---
	SnapshotNewGame();
//--- m_ScreenHook.h ---
	InitExp();
//--- m_RoomInfo.h ---
	SetTickCount();
//--- m_QuickGame.h ---
	SaveGameName();
	fCanExitGame = TRUE;
//--- m_GameCount.h ---
	dwGetHits = 0;
	dwGameKills = 0;
	dwLastKills = 0;
//--- m_ShowOrbs.h ---
	SetLifeManaShow();
//--- m_ItemExtInfo.h ---
	LoadExtInfo();
//--- m_ViewUnitStat.h ---
	SwitchStatPage(0);
//--- m_MonsterLifeBar.h ---
	InitTCList();
//--- m_QuestProtect.h ---
	SetBugQuestStatus();
//--- m_MultiClient.h ---
	MultiClientNewGame();
	AutoSummonNewGame();
//--- m_CheckDangerous.h ---
	ChickenLifeNewGame();
//--- m_PacketHandler.h ---
	ResetPacketHandlerTimer();
//--- m_GameMonitor.h ---
	ResetMonitor();
	if (dwEnterGameShowDifficultyMs) {
		dwClearDifficultyMs=dwCurMs+dwEnterGameShowDifficultyMs;
		switch (DIFFICULTY) {
			case 1:
				SetCenterAlertMsgParam(3,8,500);
				SetCenterAlertMsg(1,dwGameLng?L"噩梦难度":L"Nightmare Difficulty");
				break;
			case 2:
				SetCenterAlertMsgParam(1,2,500);
				SetCenterAlertMsg(1,dwGameLng?L"地狱难度":L"Hell Difficulty");
				break;
			default:
				SetCenterAlertMsgParam(0,6,500);
				SetCenterAlertMsg(1,dwGameLng?L"普通难度":L"Normal Difficulty");
				break;
		}
	} else dwClearDifficultyMs=0;
}
void GameLoopPatch() {
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	static int lastLevel;
	if ( fInGame==FALSE ) {
		fSinglePlayer=!(*p_D2GameInfo)->szRealmName[0];
		fInGame = TRUE;
		D2ShowGameMessage( wszProgTitle, 0);
		ShowWarningMessage();
		if(fEnterGameSound) MessageBeep(MB_ICONEXCLAMATION);
		QueryPerformanceCounter(&perfStart);
		runOnce();
		QueryPerformanceCounter(&perfEnd);
		QueryPerformanceFrequency(&perfFreq);
		double timeMs=(perfEnd.QuadPart-perfStart.QuadPart)*1000.0/perfFreq.QuadPart;
		LOG("Enter game time %.3lf ms\n",timeMs);
		if (wszCFGVersion[dwGameLng][0] ) D2ShowGameMessage( wszCFGVersion[dwGameLng], 9);
		framePerf=0;
		frameN=0;
		lastLevel=-1;
		enterMs=GetTickCount();
	}
	dwCurMs=GetTickCount();
	//QueryPerformanceCounter(&perfStart);
	dwCurrentAct = PLAYER->dwAct;
	dwCurrentLevel = PLAYER->pMonPath->pRoom1->pRoom2->pDrlgLevel->dwLevelNo;
	if (lastLevel!=dwCurrentLevel) {
		lastLevel=dwCurrentLevel;
		if (tBugAllHellAlert.isOn&&DIFFICULTY==2) checkAllBugQuests();
	}
	fPlayerInTown = (LEVELNO==actlvls[ACTNO]);
	dwLeftSkill=PLAYER->pSkill->pLeftSkill->pSkillInfo->wSkillId;
	dwRightSkill=PLAYER->pSkill->pRightSkill->pSkillInfo->wSkillId;
	if (dwChangeLeftSkill!=-1) {selectSkill(0,dwChangeLeftSkill);dwChangeLeftSkill=-1;}
	if (dwClearDifficultyMs&&dwCurMs>dwClearDifficultyMs) {
		dwClearDifficultyMs=0;
		SetCenterAlertMsg(0,L"");
	}
//--- m_AutoMap.h ---
	if (!afRevealedActs[ACTNO]) RevealAutomapLoop();
//--- m_QuickBackTown.h ---
	if( fPlayerInTown )fBackToTown = FALSE;
//--- m_PartyHelp.h ---
	if (fNeedPartyResponse) ResponseInvite();
//--- m_WinMessage.h ---
	WinMessageRunLoop();
//--- m_AutoEnchant.h ---
	if (dwRightSkill==52) AutoEnchantRunLoop();
//--- m_Snapshot.h ---
	if (enDumpUnit) SnapshotRunLoop();
//--- m_QuickSwapItem.h ---
	//DealCtrlClick();
//--- m_GameCount.h ---
	dwOrgMode = 0;
//--- m_ViewTargetUnitInfo.h ---
	if (fViewingTargetUnit) SetViewUnit();
//--- m_CheckDangerous.h ---
	ChickenLife();
//--- m_QuestProtect.h ---
	//if (ACTNO==2) CheckUnitKM();
	ShowBugInfo();
//--- m_IMEProtect.h ---
	int fChatInput=D2CheckUiStatus(UIVAR_CHATINPUT);
	if (fChatInput!=fImmEnable) ToggleIMEInput(fChatInput);
//--- m_GameChat.h ---
	if (pD2WinEditBox) CheckD2WinEditBox();
//--- m_MultiClient.h ---
	if (dwMultiClientFollowId||dwMultiClientClickTimeout) MultiClientLoop();
	if (tAutoSummon.isOn&&(dwRightSkill==70||dwRightSkill==80)) AutoSummonRunLoop();
	else if (dwAutoSummonStartMs) dwAutoSummonStartMs=0;
	if (dwRecheckBeltMs&&dwCurMs>dwRecheckBeltMs) recheckBelt();
	//QueryPerformanceCounter(&perfEnd);
	//framePerf+=perfEnd.QuadPart-perfStart.QuadPart;
	frameN++;
}

void GameEndPatch() {
	if (!fInGame) return;
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	QueryPerformanceFrequency(&perfFreq);
	/*double timeMs=framePerf*1000.0/frameN/perfFreq.QuadPart;
	LOG("Game time %d second %d frames avgFrame %.3fms\n", (GetTickCount()-enterMs)/1000,frameN,timeMs);
	*/
	QueryPerformanceCounter(&perfStart);

	fInGame = FALSE;
	fPlayerInTown = TRUE;
//--- m_pub.h ---
	DeleteCellFiles();
//--- m_PartyHelp.h ---
	fCanInvite = FALSE;
	fCanPermit = FALSE;
//--- m_WinMessage.h ---
	WinMessageEndGame();
//--- m_Snapshot.h ---
	SnapshotEndGame();
//--- m_MessageLog.h ---
	LogExitMessage();
//--- m_QuickGame.h ---
	fCanExitGame = FALSE;
//--- m_GameMonitor.h ---
	ResetMonitor();
//--- m_IMEProtect.h ---
	ToggleIMEInput(1);
//--- m_GameChat.h ---
	if (pD2WinEditBox) {
		*(DWORD*)p_D2LastChatMessage = 0;
		wchar_t *p = wcscpy(p_D2LastChatMessage, D2GetEditBoxText(pD2WinEditBox));
		*p_D2ChatTextLength = wcslen(p);
		D2DestroyEditBox(pD2WinEditBox);
		pD2WinEditBox = NULL;
		FOCUSECONTROL = NULL;
	}
	QueryPerformanceCounter(&perfEnd);
	double timeMs=(perfEnd.QuadPart-perfStart.QuadPart)*1000.0/perfFreq.QuadPart;
	LOG("Exit game time %.3lf ms\n",timeMs);
}

void __declspec(naked) GameLoopPatch_ASM()
{
	__asm {
		pop eax
		sub esp, 0x20
		mov [esp+0x0c], ecx
		push eax
		jmp GameLoopPatch
	}
}

void __declspec(naked) GameEndPatch_ASM()
{
	__asm {
		call GameEndPatch
		mov ecx,0xb4
		ret
	}
}

