#include "stdafx.h"
#include "header.h"
#include "auto.h"
#include "multi.h"

#define VK_WHEELDOWN 0x87
#define VK_WHEELUP 0x86
void __cdecl partyMessage(char *fmt, ...);
extern int fDebugMs;
extern int dwCpuUser,dwCpuKernel;
extern ToggleVar tDropProtectionToggle,tPermShowItems;
int fSkipPainting=0,fSkipPaintingMenu=0;
int dwQuickSwapItemMs=0;
int dwScreenBlockMask=0;
int dwDrawCount=0;
int fInMainMenu=0;
int fWinActive=1;
int fLeftBtnDown=0,fRightBtnDown=0,dwBtnReleaseMs=0,fUserOperating=0;
int makeRuntimeInfoPath(char *path,int id);
void follower_cmd(int lParam);
int getMouseQuestId(int x,int y);
int client_reset_quest(int questId);
int client_unassign_stat(int type,int batch);
int client_unassign_skill(int skillId,int batch);
int client_mouse_stat(int mx,int my);
int client_mouse_skillBtn(int mx,int my);
int DropProtection(UnitAny *pItem);

int need_paint=0;
int altDown=0,ctrlDown=0,shiftDown=0;
static int dwCheckCTAMs=0,checkCTA=0,dwSwapWeaponKey=0;
static int isKeyDown[256],isCombKeyDown[0x800];
extern ToggleVar *tKeyEventMap[8*256];
int mouseW,mouseL;
static int checkLockMouseMs=0;

int switchSkillStart(int id);
int switchSkillEnd(int id);
D2EditBox *isLoginScreen();
void refreshScreenSaver();
void refreshMenuScreenSaver();
int FullScreen();
int SaveGame();
int AttractNPC();
int LockMouse();
int SwitchWindow(int id);

static int dwMinFrameMs=0,dwMinFrameMsInTown=0;
int dwQuickSwapItemDelayMs= 500;
int dwAutoCallToArm= 500;
int fNoBackgroundImage= 0;
ToggleVar tLockMouseToggle={TOGGLEVAR_ONOFF, 0, 0, 0, "Lock Mouse Toggle",&LockMouse};
ToggleVar tHardCoreEscapeProtect={TOGGLEVAR_ONOFF, 0, 0, 0, "HardCoreEscapeProtect"};
ToggleVar tAutoHideMinimapToggle={TOGGLEVAR_ONOFF, 0, 0, 0, "Auto Hide MiniMap"};
ToggleVar tAutoMinimapTeleportToggle={TOGGLEVAR_ONOFF, 0, 0, 0, "Auto Teleport MiniMap"};
int dwAutoHideMinimapKey= 0;
ToggleVar tFullScreen={TOGGLEVAR_DOWN, 0, -1, 1, "FullScreen", &FullScreen,0,0,2};
ToggleVar tFullScreenNoCaption={TOGGLEVAR_ONOFF, 0, -1, 1, "FullScreenNoCaption", 0,0,0,2};
ToggleVar tResizeWindowBar={TOGGLEVAR_ONOFF, 0, -1, 1, "ResizeWindowBar", 0,0,0,2};
ToggleVar tAttractNPC={TOGGLEVAR_DOWN, 0, -1, 1, "AttractNPC", &AttractNPC,0,0,0};
ToggleVar tEnableScreenSaver={TOGGLEVAR_ONOFF, 0, 0, 0, "EnableScreenSaver",&refreshScreenSaver};
ToggleVar tEnableMenuScreenSaver={TOGGLEVAR_ONOFF, 0, 0, 0, "EnableMenuScreenSaver",&refreshMenuScreenSaver,0,0,2};
int dwGameControlKeys[64][4] ={0};
int dwGameSoundKeys[128]={0};
ToggleVar tSwitchWindowKeys[16][2]={0};
int dwSwitchSkillKeyCount={0};
int dwSwitchSkillKeys[32]={0};
int dwSwitchSkillLeft[32][8][4]={0};
int dwSwitchSkillRight[32][8][4]={0};
int dwSwitchSkillLeftUp[32][8]={0};
int dwSwitchSkillRightUp[32][8]={0};
int dwSwitchSkillStandStill[32]={0};
int dwSwitchSkillLeftOrg[32]={0};
int dwSwitchSkillRightOrg[32]={0};
ToggleVar tNoHide={TOGGLEVAR_ONOFF,	1,	-1,		1,	"No Hide"};
int  fFullWindows=0;
ToggleVar tSaveGame={TOGGLEVAR_DOWN,0,-1,	1,"SaveGame",&SaveGame};
int dwTpSwitchBackground[8]={0};
int dwEnterDoorInBackgroundSkill[7][200]={0};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY, "SaveGameKey",&tSaveGame},
	{CONFIG_VAR_TYPE_INT, "MinFrameMs",&dwMinFrameMs,4},
	{CONFIG_VAR_TYPE_INT, "MinFrameMsInTown",&dwMinFrameMsInTown,4},
	{CONFIG_VAR_TYPE_KEY, "NoHideToggle",			  &tNoHide     },		
  {CONFIG_VAR_TYPE_INT, "NoBackgroundImage",				&fNoBackgroundImage,     4},
  {CONFIG_VAR_TYPE_INT, "AutoCallToArm",				&dwAutoCallToArm,     4},
  {CONFIG_VAR_TYPE_KEY, "LockMouseToggle",       &tLockMouseToggle        },
  {CONFIG_VAR_TYPE_KEY, "HardCoreEscapeProtect",       &tHardCoreEscapeProtect        },
  {CONFIG_VAR_TYPE_KEY, "AutoHideMinimapToggle",       &tAutoHideMinimapToggle        },
  {CONFIG_VAR_TYPE_KEY, "AutoTeleportMinimapToggle",       &tAutoMinimapTeleportToggle        },
  {CONFIG_VAR_TYPE_INT, "AutoHideMinimapKey",				&dwAutoHideMinimapKey,     4},
  {CONFIG_VAR_TYPE_INT, "QuickSwapItemDelayMs",				&dwQuickSwapItemDelayMs,     4},
	{CONFIG_VAR_TYPE_KEY, "FullScreenKey",			&tFullScreen     },
	{CONFIG_VAR_TYPE_KEY, "FullScreenNoCaption",			&tFullScreenNoCaption     },
	{CONFIG_VAR_TYPE_KEY, "ResizeWindowBar",			&tResizeWindowBar     },
	{CONFIG_VAR_TYPE_KEY, "AttractNPCKey",			&tAttractNPC     },
	{CONFIG_VAR_TYPE_KEY, "EnableScreenSaverToggle",   &tEnableScreenSaver     },
	{CONFIG_VAR_TYPE_KEY, "EnableMenuScreenSaverToggle",   &tEnableMenuScreenSaver     },
  {CONFIG_VAR_TYPE_INT_ARRAY1, "GameControlKeys",    &dwGameControlKeys,  4, {64} },
  {CONFIG_VAR_TYPE_INT_ARRAY1, "GameSoundKeys",    &dwGameSoundKeys,  1, {128} },
  {CONFIG_VAR_TYPE_KEY_ARRAY1,"SwitchWindowKeys", tSwitchWindowKeys,2,{16},2,"SwitchWindowKeys[%d]", SwitchWindow  },
  {CONFIG_VAR_TYPE_SWITCH_SKILL, "SwitchSkillKeys",    &dwSwitchSkillKeys,  1, {32} },
  {CONFIG_VAR_TYPE_SWITCH_SKILL, "SwitchSkillLeft",    &dwSwitchSkillLeft,  4, {32,8} },
  {CONFIG_VAR_TYPE_SWITCH_SKILL, "SwitchSkillRight",    &dwSwitchSkillRight,  4, {32,8} },
  {CONFIG_VAR_TYPE_SWITCH_SKILL, "SwitchSkillLeftUp",    &dwSwitchSkillLeftUp,  1, {32,8} },
  {CONFIG_VAR_TYPE_SWITCH_SKILL, "SwitchSkillRightUp",    &dwSwitchSkillRightUp,  1, {32,8} },
  {CONFIG_VAR_TYPE_SWITCH_SKILL, "SwitchSkillStandStill",    &dwSwitchSkillStandStill,  1, {32} },
  {CONFIG_VAR_TYPE_INT_ARRAY1,"TpSwitchBackground",&dwTpSwitchBackground,1,{8}},
  {CONFIG_VAR_TYPE_INT_ARRAY0,"BackgroundEnterDoorSkillSwitch",&dwEnterDoorInBackgroundSkill,1,{7,200}},
};
void winmsg_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void winmsg_initConfigVars() {
	memset(tSwitchWindowKeys ,       0 ,      sizeof(tSwitchWindowKeys) );
	memset(dwGameControlKeys ,       0 ,      sizeof(dwGameControlKeys) );
	memset(dwGameSoundKeys ,       0 ,      sizeof(dwGameSoundKeys) );
	dwSwitchSkillKeyCount=0;
	memset(dwSwitchSkillKeys ,       0 ,      sizeof(dwSwitchSkillKeys) );
	memset(dwSwitchSkillLeft ,       0xFF ,      sizeof(dwSwitchSkillLeft) );
	memset(dwSwitchSkillRight ,       0xFF ,      sizeof(dwSwitchSkillRight) );
	memset(dwSwitchSkillLeftUp ,       0xFF ,      sizeof(dwSwitchSkillLeftUp) );
	memset(dwSwitchSkillRightUp ,       0xFF ,      sizeof(dwSwitchSkillRightUp) );
	memset(dwSwitchSkillStandStill,0,sizeof(dwSwitchSkillStandStill));
	memset(dwTpSwitchBackground,0,sizeof(dwTpSwitchBackground));
	memset(dwEnterDoorInBackgroundSkill,0,sizeof(dwEnterDoorInBackgroundSkill));
}
void WinMessageFixValues() {
	for ( int i = 0; i < _ARRAYSIZE(tSwitchWindowKeys); i++ ) {
		for ( int j = 0; j < _ARRAYSIZE(tSwitchWindowKeys[0]); j++ ) {
			tSwitchWindowKeys[i][j].type=4;
			tSwitchWindowKeys[i][j].func=NULL;
			tSwitchWindowKeys[i][j].funcUp=SwitchWindow;
		}
	}
	fFullWindows = IsFullWindow() ;
}
static const char *gamecontrol_names[64]={
"CharacterScreen"/*0*/, "InventoryScreen"/*1*/, "PartyScreen"/*2*/, "MessageLog"/*3*/, "QuestLog"/*4*/, "Chat"/*5*/,
"HelpScreen"/*6*/, "Automap"/*7*/, "CenterAutomap"/*8*/, "FadeAutomap"/*9*/, "PartyonAutomap"/*10*/,
"NamesonAutomap"/*11*/, "SkillTree"/*12*/, "SkillSpeedBar"/*13*/, "Skill1"/*14*/, "Skill2"/*15*/,
"Skill3"/*16*/, "Skill4"/*17*/, "Skill5"/*18*/, "Skill6"/*19*/, "Skill7"/*20*/,
"Skill8"/*21*/, "ShowBelt"/*22*/, "UseBelt1"/*23*/, "UseBelt2"/*24*/, "UseBelt3"/*25*/,
"UseBelt4"/*26*/, "SayHelp"/*27*/, "SayFollowme"/*28*/, "SayThisisforyou"/*29*/, "SayThanks"/*30*/,
"SaySorry"/*31*/, "SayBye"/*32*/, "SayNowyoudie"/*33*/, "Run"/*34*/, "ToggleRun/Walk"/*35*/,
"StandStill"/*36*/, "ShowItems"/*37*/, "ClearScreen"/*38*/, "SelectPreviousSkill"/*39*/, "SelectNextSkill"/*40*/,
"ClearNessage"/*41*/,0, "ShowPortraits"/*43*/, "SwapWeapons"/*44*/, "ToggleMiniMap"/*45*/,
"Skill9"/*46*/, "Skill10"/*47*/, "Skill11"/*48*/, "Skill12"/*49*/, "Skill13"/*50*/,
"Skill14"/*51*/, "Skill15"/*52*/, "Skill16"/*53*/, "HiringScreen"/*54*/, "SayRetreat"/*55*/,
"ESC"/*56*/,0,0,0, "HireUseBelt1"/*60*/,
"HireUseBelt2"/*61*/, "HireUseBelt3"/*62*/, "HireUseBelt4"/*63*/,
};
void WinMessage_initKeyEvent() {
	char buf[256];
	for (int i=0;i<_ARRAYSIZE(dwGameControlKeys);i++) {
		for (int j=0;j<_ARRAYSIZE(dwGameControlKeys[0]);j++) {
			int k=dwGameControlKeys[i][j];
			if (1<=k&&k<8*256) {
				ToggleVar *p=addExtraKey(k);if (!p) {LOG("ERROR: too many keys\n");return;}
				p->type=TOGGLEVAR_DOWNUPPARAM;p->key=k;
				p->param=i;p->keyType=0;
				p->funcInt=GameControlStart;
				p->funcUp=GameControlEnd;
				sprintf(buf,"GameControl[%d:%s]",i+1,gamecontrol_names[i]? gamecontrol_names[i]:"");
				p->desc=heap_strdup(confHeap,buf);
			}
		}
	}
	for (int i=0;i<_ARRAYSIZE(dwGameSoundKeys);i++) {
		int k=dwGameSoundKeys[i];
		if (1<=k&&k<8*256) {
			ToggleVar *p=addExtraKey(k);if (!p) {LOG("ERROR: too many keys\n");return;}
			p->type=TOGGLEVAR_DOWNPARAM;p->key=k;
			p->param=i;p->keyType=0;
			p->funcInt=GameSound;
			sprintf(buf,"GameSound[%d]",i+1);
			p->desc=heap_strdup(confHeap,buf);
		}
	}
	for (int i=0;i<dwSwitchSkillKeyCount;i++) {
		int k=dwSwitchSkillKeys[i];
		if (1<=k&&k<8*256) {
			ToggleVar *p=addExtraKey(k);if (!p) {LOG("ERROR: too many keys\n");return;}
			p->type=TOGGLEVAR_DOWNUPPARAM;p->key=k;
			p->param=i;p->keyType=0;
			p->funcInt=switchSkillStart;
			sprintf(buf,"SwitchSkill[%d]",i+1);
			p->desc=heap_strdup(confHeap,buf);
			p->funcUp=switchSkillEnd;
		}
	}
}
int IsKeyDown(int keyCode) {if (keyCode<=0) return 0;return isKeyDown[keyCode&0xFF];}
int LockMouse() {
	static RECT rcOldClip;
	static int locked=0,hasOldClip=0;
	WINDOWPLACEMENT wp;
	int needLock=tLockMouseToggle.isOn&&fWinActive&&fInGame&&!fPlayerInTown;
	HWND hwnd=d2gfx_GetHwnd();
	if (!GetWindowPlacement(hwnd,&wp)) return 0;
	if (wp.showCmd==SW_SHOWMAXIMIZED) needLock=0;
	if (locked==needLock) return 0;
	if (needLock) {
		RECT rcClip,client;POINT topleft,bottomright;
		if (!hasOldClip) {
			if (!GetClipCursor(&rcOldClip)) return 0; 
			hasOldClip=1;
			LOG("Lock mouse old clip %d %d %d %d\n",rcOldClip.left,rcOldClip.top,rcOldClip.right,rcOldClip.bottom);
		}
		if (!GetClientRect(hwnd, &client)) return 0; 
		topleft.x=client.left;topleft.y=client.top;
		bottomright.x=client.right;bottomright.y=client.bottom;
		if (!ClientToScreen(hwnd,&topleft)) return 0;
		if (!ClientToScreen(hwnd,&bottomright)) return 0;
		rcClip.top=topleft.y;rcClip.left=topleft.x;
		rcClip.bottom=bottomright.y;rcClip.right=bottomright.x;
		if (!ClipCursor(&rcClip)) return 0;
	} else {
		RECT drect;
		HWND dhwnd=GetDesktopWindow();
		GetClientRect(dhwnd, &drect); 
		if (!ClipCursor(&drect)) return 0;
		//if (!ClipCursor(&rcOldClip)) return 0;
	}
	locked=needLock;
	return 1;
}
int switchSkillStart(int id) {
	int* pleft=dwSwitchSkillLeft[id][dwPlayerClass];
	int* pright=dwSwitchSkillRight[id][dwPlayerClass];
	dwSwitchSkillLeftOrg[id]=PLAYER->pSkill->pLeftSkill->pSkillInfo->wSkillId;
	dwSwitchSkillRightOrg[id]=PLAYER->pSkill->pRightSkill->pSkillInfo->wSkillId;
	int handled=0;
	for (int i=0;i<4;i++) {
		int left=pleft[i];
		if (left==-3) {
			GameControl(44);handled=1;break;
		} else if (left>0&&hasSkill(left)) {
			selectSkill(0,left);handled=1;
			break;
		}
	}
	for (int i=0;i<4;i++) {
		int right=pright[i];
		if (right==-3) {
			GameControl(44);handled=1;break;
		} else if (right>0&&hasSkill(right)) {
			selectSkill(1,right);handled=1;
			break;
		}
	}
	if (handled&&dwSwitchSkillStandStill[id]) GameControlStart(36);
	return handled;
}
int switchSkillEnd(int id) {
	if (dwSwitchSkillStandStill[id]) GameControlEnd(36);
	int left0=dwSwitchSkillLeftOrg[id];
	int right0=dwSwitchSkillRightOrg[id];
	int left=dwSwitchSkillLeftUp[id][dwPlayerClass];
	int right=dwSwitchSkillRightUp[id][dwPlayerClass];
	int handled=0;
	if (left==-3) {
		GameControl(44);handled=1;
	} else if (left==-2) {//restore
		if (left0>=0&&hasSkill(left0)) {selectSkill(0,left0);handled=1;}
	} else if (left>0&&hasSkill(left)) {
		selectSkill(0,left);handled=1;
	}
	if (right==-3) {
		GameControl(44);handled=1;
	} else if (right==-2) {//restore
		if (right0>=0&&hasSkill(right0)) {selectSkill(1,right0);handled=1;}
	} else if (right>0&&hasSkill(right)) {
		dwSwitchSkillRightOrg[id]=PLAYER->pSkill->pRightSkill->pSkillInfo->wSkillId;
		selectSkill(1,right);handled=1;
	}
	return handled;
}
int GameSound(int snd) {
	BYTE pkg[3] = {0x3f};
	*(WORD*)&pkg[1] = snd;
	//SendPacket(pkg,sizeof(pkg));		//Doesn't work on network
	d2client_PlaySound(PLAYER->dwUnitId,PLAYER->dwUnitType,snd);
	return 1;
}
extern int fLoadedLib;
char *getCharTag(char *name);
static void setWinTitle(HWND hwnd,int inGame) {
	char buf[256];
	char *realm=(*d2client_pGameInfo)->szRealmName;
	char *account=(*d2client_pGameInfo)->szAccountName;
	char *name=(*d2client_pGameInfo)->szCharName;
	char *ip=(*d2client_pGameInfo)->szGameServerIp;
	char *game=(*d2client_pGameInfo)->szGameName;
	char *tag=getCharTag(name);
	if (!tag) tag="";
	if (!inGame) {
		if (realm[0]&&account[0]) {
			sprintf(buf,"[%d]%s@%s %s Diablo II",dwGameWindowId,name,account,tag);
		} else {
			sprintf(buf,"[%d]%s %s Diablo II",dwGameWindowId,name,tag);
		}
	} else {
		if (realm[0]&&account[0]) {
			sprintf(buf,"[%d]%s@%s %s %s Diablo II",dwGameWindowId,name,account,game,tag);
		} else {
			sprintf(buf,"[%d]%s %s Diablo II",dwGameWindowId,name,tag);
		}
	}
	SetWindowTextA(hwnd,buf);
}
void fitResolution();
void setResolutionPatch(int install);
int FullScreen() {
	WINDOWPLACEMENT wp;
	HWND hwnd=d2gfx_GetHwnd();
	if (!GetWindowPlacement(hwnd,&wp)) return 0;
	int style=GetWindowLongA(hwnd,GWL_STYLE);
	if (wp.showCmd==SW_SHOWMAXIMIZED) {
		if (tResizeWindowBar.isOn) style|=WS_THICKFRAME;
		if (!(style&WS_CAPTION)) {
			style|=WS_CAPTION;
			SetWindowLongA(hwnd,GWL_STYLE,style);
		}
		ShowWindow(hwnd,SW_SHOWNORMAL);
		setResolutionPatch(0);
	} else {
		setResolutionPatch(1);
		if (tFullScreenNoCaption.isOn&&(style&WS_CAPTION)) {
			style=style&(~(WS_CAPTION|WS_THICKFRAME));
			SetWindowLongA(hwnd,GWL_STYLE,style);
		}	
		ShowWindow(hwnd,SW_SHOWMAXIMIZED);
	}
	LockMouse();
	return 1;
}
int AttractNPC() {
	UnitAny *pSelectedUnit = d2client_GetSelectedUnit();
	if (pSelectedUnit&&pSelectedUnit->dwUnitType==1) {
		pSelectedUnit->dwUnitId;
		BYTE packet[17] = {0x59};
		*(DWORD*)&packet[1] = pSelectedUnit->dwUnitType;
		*(DWORD*)&packet[5] = pSelectedUnit->dwUnitId;
		int x0=PLAYER->pMonPath->wUnitX;
		int y0=PLAYER->pMonPath->wUnitY;
		*(DWORD*)&packet[9] = x0;
		*(DWORD*)&packet[13] = y0;
		SendPacket(packet,sizeof(packet));
		return 1;
	}
	return 0;
}
int SwitchWindow(int id) {
	if (isLoginScreen()) return 0;
	id++;
	if (id==dwGameWindowId) return 0;
	HWND hwnd=getGameWindowHwnd(id);
	if (!hwnd) return 0;
	SetForegroundWindow(hwnd);
	return 1;
}
extern int fHighRes;
extern int forceStandStill;
void fitWindowToResolution();
void WinMessageNewGame() {
	refreshScreenSaver();
	refreshMenuScreenSaver();
	dwDrawCount=0;
	forceStandStill=0;
	dwBtnReleaseMs=0;fUserOperating=0;hydraLock=0;
	dwQuickSwapItemMs=0;dwCheckCTAMs=0;dwSwapWeaponKey=0;
	LockMouse();
	HWND hwnd=d2gfx_GetHwnd();
	setWinTitle(hwnd,1);
	if (tResizeWindowBar.isOn) {
		int style=GetWindowLongA(hwnd,GWL_STYLE);
		if (!(style&WS_THICKFRAME)) {
			style|=WS_THICKFRAME;
			SetWindowLongA(hwnd,GWL_STYLE,style);
			fitWindowToResolution();
		}
	}
}
void WinMessageEndGame() {
	refreshScreenSaver();
	refreshMenuScreenSaver();
	LockMouse();
	HWND hwnd=d2gfx_GetHwnd();
	setWinTitle(hwnd,0);
}
void multiclient_winactive();
void refreshScreenSaver() {
	fSkipPainting=tEnableScreenSaver.isOn&&!fWinActive;
}
void refreshMenuScreenSaver() {
	fSkipPaintingMenu=tEnableMenuScreenSaver.isOn&&!fWinActive;
}
void winActiveMsg(int active) {
	fWinActive=active;
	refreshScreenSaver();
	refreshMenuScreenSaver();
	forceStandStill=0;altDown=0;ctrlDown=0;shiftDown=0;
	fUserOperating=0;fLeftBtnDown=0;fRightBtnDown=0;dwBtnReleaseMs=0;
	memset(isKeyDown,0,sizeof(isKeyDown));
	memset(isCombKeyDown,0,sizeof(isCombKeyDown));
	checkAutoSkillStatus();
	if (active) multiclient_winactive();
	else {
		if (dwTpSwitchBackground[dwPlayerClass]
			&&(dwRightSkill==Skill_Teleport
				||dwRightSkill==Skill_Telekinesis
				||dwRightSkill==219||dwRightSkill==220
				||dwRightSkill==Skill_StaticField&&dwCurrentLevel==Level_ArreatSummit
			)) {
			int skillId=dwTpSwitchBackground[dwPlayerClass];
			if (hasSkill(skillId)&&dwRightSkill!=skillId) selectSkill(1,skillId);
		}
		need_paint=1;
	}
}
int hasCTA() {
	for (UnitAny *pUnit = d2common_GetFirstItemInInv(PLAYER->pInventory);pUnit;pUnit = d2common_GetNextItemInInv(pUnit)) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) continue ;
		if (pUnit->pItemData->nItemLocation!=255) continue;
		if (pUnit->pItemData->nBodyLocation==4) {
			if ( d2common_CheckItemFlag(pUnit, ITEMFLAG_RUNEWORD, 0, "?")
				&&d2common_GetRuneWordTxtIndex( pUnit->pItemData->wMagicPrefix[0])==27) {
				return 1;
			} else return 0;
		}
	}
	return 0;
}
void WinMessageRunLoop() {
	static int inTown=0;
	//static int flag=0;if (!flag) {flag=1;if (tShowTestInfo.isOn) LOG("THREADID(Game loop): %d\n",GetCurrentThreadId());}
	if (fPlayerInTown!=inTown) {LockMouse();inTown=fPlayerInTown;}
	if (fUserOperating&&!fLeftBtnDown&&!fRightBtnDown) {
		if (dwCurMs>=dwBtnReleaseMs+dwAutoSkillIdleDelay) {
			fUserOperating=0;checkAutoSkillStatus();
		}
	}
	if (checkLockMouseMs&&dwCurMs>=checkLockMouseMs) {LockMouse();checkLockMouseMs=0;}
	if (dwCheckCTAMs) {
		static int swapBackMs=0,ctaWeapon;
		if (!dwSwapWeaponKey) {
			dwSwapWeaponKey=getGameControlKey(44);
			if (dwSwapWeaponKey<=0) dwSwapWeaponKey='W';
		}
		if (GetKeyState(dwSwapWeaponKey)&0x8000) *d2client_pStandStill=1;
		if (dwCurMs>dwCheckCTAMs) {
			if (checkCTA) {
				checkCTA=0;
				if (hasCTA()) {
					if (PLAYER->pSkill->pLeftSkill->pSkillInfo->wSkillId!=155) selectSkill(0,155);
					if (PLAYER->pSkill->pRightSkill->pSkillInfo->wSkillId!=149) selectSkill(1,149);
					ctaWeapon=*d2client_pActiveWeapon;
					swapBackMs=0;
				} else dwCheckCTAMs=0;
			} else {
				if (!d2client_pStandStill[0]) {
					if (ctaWeapon==*d2client_pActiveWeapon) {
						if (!swapBackMs||dwCurMs>swapBackMs) {
							GameControl(44); //swap back
							swapBackMs=dwCurMs+dwAutoCallToArm;
							//LOG("CTA auto swap back\n");
						}
					} else dwCheckCTAMs=0;
				}
			}
		}
		if (!dwCheckCTAMs)
			*d2client_pStandStill=0;
	}
	if (dwQuickSwapItemMs) {//&&dwCurMs>dwQuickSwapItemMs) {
		UnitAny *pItem = PLAYER->pInventory->pCursorItem;
		if (pItem) {
			HWND hwnd=d2gfx_GetHwnd();
			PostMessage(hwnd, WM_RBUTTONDOWN, 0, mouseL);
			PostMessage(hwnd, WM_RBUTTONUP, 0, mouseL);
			dwQuickSwapItemMs=0;
		}
	}
	if (tAutoHideMinimapToggle.isOn) {
		int on=0;
		if (tAutoMinimapTeleportToggle.isOn&&dwRightSkill==54) on=1;
		if (dwAutoHideMinimapKey>0&&isKeyDown[dwAutoHideMinimapKey]) on=!on;
		int ison=*d2client_pAutomapOn;//D2CheckUiStatus(UIVAR_AUTOMAP);
		if (ison!=on) GameControl(7);
	}
}
extern int marketItemCount;
extern ToggleVar tViewSocketable;
static int shouldProcessKey(int vk,int winMsg) {
//winMsg 0:gameKeyEvent 1:windowKeyEvent 2:mouseClickAndWheel
	if (*d2client_pUiDropGoldWindow) return 0;
	if (*d2client_pUiChatOn) return 0;
	if (fInGame) {
		if ((d2client_pScreenBlocked[0]&3)==3) {
			if (marketItemCount>10) {
				if (vk==tViewSocketable.key) return winMsg!=0;
			}
			if (*d2client_pUiCubeOn) return winMsg!=0; //auto merge
			if (*d2client_pUiStashOn) return winMsg!=0; //auto simple item stack
			if ('0'<=vk&&vk<='9'||'A'<=vk&&vk<='Z'||vk==VK_BACK) return ctrlDown||altDown?1:0;
			return winMsg!=1;
		}
		return winMsg!=0;
	} else {
		return 1;
	}
}
int canExitGame() {
	if (fIsHardCoreGame&&fInGame
		&&PLAYER&&(PLAYER->dwMode==PlayerMode_Death||PLAYER->dwMode==PlayerMode_Dead)) {
		if (tHardCoreEscapeProtect.isOn) {
			wchar_t wbuf[256];
			char keyname[256];formatKey(keyname,tHardCoreEscapeProtect.key);
			gameMessageW(dwGameLng?L"专家级退出保护，按%hs解锁":L"Hard Core exit protect, press %hs to unlock",keyname);
			if (*d2client_pGameInfo) {
				char *game=(*d2client_pGameInfo)->szGameName;
				char *password=(*d2client_pGameInfo)->szGamePassword;
				wsprintfW(wbuf,dwGameLng?L"游戏名:%hs":L"Game:%hs",game);
				setBottomAlertMsg(0,wbuf,3000,1,2,1);
				wsprintfW(wbuf,dwGameLng?L"密码:%hs":L"Password:%hs",password);
				setBottomAlertMsg(1,wbuf,3000,1,2,1);
			}
			return 0;
		}
	}
	return 1;
}
static int __fastcall keyDown(int vk,int winMsg) {
	if (vk<1||vk>=256) return 0;
	switch (vk) {
		case VK_SHIFT:shiftDown=1;break;
		case VK_CONTROL:ctrlDown=1;break;
		case VK_MENU:altDown=1;break;
	}
	if (vk==VK_ESCAPE&&fIsHardCoreGame&&fInGame
		&&PLAYER&&(PLAYER->dwMode==PlayerMode_Death||PLAYER->dwMode==PlayerMode_Dead)) {
		if (!canExitGame()) return 1;
	}
	if (!shouldProcessKey(vk,winMsg)) return 0;
	if (isKeyDown[vk]) return 0;
	isKeyDown[vk]=1;
	if (vk!=VK_MENU&&altDown) vk|=0x100;
	if (vk!=VK_CONTROL&&ctrlDown) vk|=0x200;
	if (vk!=VK_SHIFT&&shiftDown) vk|=0x400;
	if (vk=='N'&&!fInGame) {
		HMODULE loadhackmap = GetModuleHandle("loadhackmap.dll");
		if (loadhackmap) {
			void (__stdcall *ClearMainMenuMessage)()=(void (__stdcall *)())GetProcAddress(loadhackmap,"ClearMainMenuMessage");
			if (ClearMainMenuMessage) {
				ClearMainMenuMessage();
			}
		}
	}
	isCombKeyDown[vk]=1;
	int handled=0;
	int inGame=fInGame&&!d2client_CheckUiStatus(UIVAR_CHATINPUT);
	for (ToggleVar *p = tKeyEventMap[vk];p;p=p->next) {
		if(vk==p->key) {
			if (!fInGame) {
				if (p->keyType==0) continue;
			} else if (inGame) {
				if (p->keyType==1) continue;
			}
			switch (p->type) {
			case TOGGLEVAR_ONOFF:
				p->isOn = !p->isOn;
				if (p->funcVoid) (p->funcVoid)();
				if (p->desc) partyMessage("<Hackmap>: %hs %hs.", p->desc, p->isOn ? "on" : "off" );
				handled=1;
				break;
			case TOGGLEVAR_DOWN:if (p->funcVoid&&p->funcVoid()) handled=1;break;
			case TOGGLEVAR_DOWNPARAM:case TOGGLEVAR_DOWNUPPARAM:
				if (p->funcInt&&p->funcInt(p->param)) handled=1;break;
			case TOGGLEVAR_DOWNPTR:
				if (p->funcPtr&&p->funcPtr(p->paramPtr)) handled=1;break;
			}
		}
	}
	if (vk==VK_MENU&&!tPermShowItems.isOn) return 0;
	return handled;
}
static int __fastcall keyUp(int vk,int winMsg) {
	if (vk<0||vk>=256) return 0;
	switch (vk) {
		case VK_SHIFT:shiftDown=0;break;
		case VK_CONTROL:ctrlDown=0;break;
		case VK_MENU:altDown=0;break;
	}
	if (!shouldProcessKey(vk,winMsg)) return 0;
	if (!isKeyDown[vk]) return 0;
	isKeyDown[vk]=0;
	int handled=0;
	for (int comb=0;comb<0x800;comb+=0x100) {
		int vk2=comb|vk;if (!isCombKeyDown[vk2]) continue;isCombKeyDown[vk2]=0;
		for (ToggleVar *p = tKeyEventMap[vk2];p;p=p->next) {
			if(vk2==p->key) {
				if (!fInGame) {
					if (p->keyType==0) continue;
				} else {
					if (p->keyType==1) continue;
				}
				if(p->type==TOGGLEVAR_DOWNUPPARAM) {if (p->funcUp&&p->funcUpInt(p->param)) handled=1;}
			}
		}
	}
	if (vk==VK_MENU&&!tPermShowItems.isOn) return 0;
	return handled;
}
int getGameWinId(int mouseX);
void toggle_follower(int gameId);
int WinMessage(int retAddr,int retAddr2,HWND hwnd,int msg,int w,int l) {
	static int savedRuntime=0;
	if (! *d2win_pRunningFlag) return 0;
	switch (msg) {
		case WM_MOUSEWHEEL: {
			short z=(short)(w>>16);
			int k=z>0?VK_WHEELUP:VK_WHEELDOWN;
			if (keyDown(k,2)) {keyUp(k,2);return 0;}
			keyUp(k,2);
			break;
		}
		case WM_MBUTTONDOWN:if (keyDown(VK_MBUTTON,2)) return 0;break;
		case WM_MBUTTONUP:keyUp(VK_MBUTTON,2);break;
		case WM_XBUTTONDOWN:
			if (w&MK_XBUTTON1) keyDown(VK_XBUTTON1,2);
			if (w&MK_XBUTTON2) keyDown(VK_XBUTTON2,2);
			break;
		case WM_XBUTTONUP:
			keyUp(VK_XBUTTON1,2);
			keyUp(VK_XBUTTON2,2);
			break;
		case WM_ACTIVATE: //w=1 WA_ACTIVE, w=2 WA_CLICKACTIVE, w=0x200001 minimized
			winActiveMsg(w&0xFFFF?1:0);
			if (w&0xFFFF) checkLockMouseMs=dwCurMs+200;
			else LockMouse();
			if (!savedRuntime) {savedRuntime=1;saveRuntimeInfo(hwnd);}
			break;
		case WM_NCMOUSELEAVE:
			break;
		case WM_LBUTTONDOWN:
			//{static int flag=0;if (!flag) {flag=1;if (tShowTestInfo.isOn) LOG("THREADID(Windows message): %d\n",GetCurrentThreadId());}}
			if (fInGame) {
				if (PLAYER->pInventory->pCursorItem) {
					if (tDropProtectionToggle.isOn) {
						if (
							(!(d2client_pScreenBlocked[0]&1)&&*d2client_pMouseX>SCREENSIZE.x/2)
							||
							(*d2client_pMouseX<SCREENSIZE.x/2&&
								!(*d2client_pUiModItemOn)
								&&(!(d2client_pScreenBlocked[0]&2)
								||(*d2client_pUiNpcTradeOn)
								)
							)
							) {
							if (DropProtection(PLAYER->pInventory->pCursorItem)) {
								char keyname[256];formatKey(keyname,tDropProtectionToggle.key);
								gameMessageWColor(0,dwGameLng?L"装备保护,按%hs取消保护"
									:L"Drop Protection on, press %hs to disable protection",keyname);
								return 0;
							}
						}
					}
				} else {
					dwQuickSwapItemMs=0;
					if ((*d2client_pScreenBlocked&1) 
						&&(GetKeyState(VK_SHIFT)&0x8000||GetKeyState(VK_MENU)&0x8000||GetKeyState(VK_CONTROL)&0x8000)) {
						if (d2client_CheckUiStatus(UIVAR_INVENTORY)||d2client_CheckUiStatus(UIVAR_CUBE)
							||d2client_CheckUiStatus(UIVAR_STASH)||d2client_CheckUiStatus(UIVAR_PPLTRADE))
							dwQuickSwapItemMs=dwCurMs+(fIsSinglePlayer?0:dwQuickSwapItemDelayMs);
						//click map on left half screen
						if (!(d2client_pScreenBlocked[0]&2)&&*d2client_pMouseX<=SCREENSIZE.x/2)
							dwQuickSwapItemMs=0;
					}
					if (18<=*d2client_pMouseY&&*d2client_pMouseY<60) {
						if (GetKeyState(VK_MENU)&0x8000) { //ALT switch window
							int gid=getGameWinId(*d2client_pMouseX);
							if (gid) {SwitchWindow(gid-1);return 0;}
						} else if (GetKeyState(VK_CONTROL)&0x8000) { //CTRL add/remove follower
							int gid=getGameWinId(*d2client_pMouseX);
							if (gid) {toggle_follower(gid);return 0;}
						}
					}
					if (altDown&&ctrlDown) {
						if (*d2client_pUiCharacterOn&&*d2client_pMouseX<SCREENSIZE.x/2) { 
							int cmd=client_mouse_stat(*d2client_pMouseX,*d2client_pMouseY);
							if (cmd) {client_unassign_stat(cmd,shiftDown);return 0;}
						}
						if (*d2client_pUiSkillTreeOn&&*d2client_pMouseX>SCREENSIZE.x/2) {
							int skillId=client_mouse_skillBtn(*d2client_pMouseX,*d2client_pMouseY);
							if (skillId) {client_unassign_skill(skillId,shiftDown);return 0;}
						}
						if (shiftDown&&*d2client_pUiQuestOn) {
							int qId=getMouseQuestId(*d2client_pMouseX,*d2client_pMouseY);
							client_reset_quest(qId);
							return 0;
						}
					}

					fLeftBtnDown=1;fUserOperating=1;mouseW=w;mouseL=l;hydraLock=0;
					checkAutoSkillStatus();
				}
			}
			break;
		case WM_LBUTTONUP:
			if (altDown&&ctrlDown) {
				if (*d2client_pUiCharacterOn&&*d2client_pMouseX<SCREENSIZE.x/2) { 
					int cmd=client_mouse_stat(*d2client_pMouseX,*d2client_pMouseY);
					if (cmd) return 0;
				}
				if (*d2client_pUiSkillTreeOn&&*d2client_pMouseX>SCREENSIZE.x/2) {
					int skillId=client_mouse_skillBtn(*d2client_pMouseX,*d2client_pMouseY);
					if (skillId) return 0;
				}
			}
			fLeftBtnDown=0;
			if (!fLeftBtnDown&&!fRightBtnDown) dwBtnReleaseMs=dwCurMs;
			break;
		case WM_RBUTTONDOWN:
			fRightBtnDown=1;fUserOperating=1;mouseW=w;mouseL=l;hydraLock=0;
			checkAutoSkillStatus();
			break;
		case WM_RBUTTONUP:
			fRightBtnDown=0;
			if (!fLeftBtnDown&&!fRightBtnDown) dwBtnReleaseMs=dwCurMs;
			break;
		case WM_MOUSEMOVE:
			mouseW=w;mouseL=l;
			break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
			if (keyDown(w,1)) return 0;
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			switch (w) {
				case VK_MULTICLIENT_INFO:
					multiclient_recv_info(l);
					return 0;
				case VK_AUTOFOLLOW_CMD:
					follower_cmd(l);
					return 0;
				case VK_AUTOFOLLOW_CLICK_OBJECT:
					follower_click_object((l>>28)&0xF,l&0xFFFFFFF);
					return 0;
				case VK_MULTICLIENT_LEFT_XY:
					follower_left_click_xy(l&0xFFFF,(l>>16)&0xFFFF);
					return 0;
				case VK_MULTICLIENT_RIGHT_XY:
					follower_right_click_xy(l&0xFFFF,(l>>16)&0xFFFF);
					return 0;
				case VK_MULTICLIENT_LEFT_UNIT:
					follower_left_click_unit((l>>28)&0xF,l&0xFFFFFFF);
					return 0;
				case VK_MULTICLIENT_RIGHT_UNIT:
					follower_right_client_unit((l>>28)&0xF,l&0xFFFFFFF);
					return 0;
				case VK_MULTICLIENT_TP:
					follower_tp(l&0xFFFF,(l>>16)&0xFFFF);
					return 0;
				default:
					keyUp(w,1);
			}
			break;
		case WM_SIZE:fitResolution();break;
		case WM_PAINT:need_paint=1;break;
		case WM_CLOSE: {
			char path[512];
			makeRuntimeInfoPath(path,dwGameWindowId);
			DeleteFileA(path);
			break;
		}
	}
	return *d2win_pRunningFlag;
}
/*
window message processor: 6f8f7740
D2Win.dll+17740 - A1 C014906F           - mov eax,[D2Win.dll+214C0] { (1) }
6F8F7740 - A1 C014906F           - mov eax,[6F9014C0] { (1) }
		mov eax ,[d2client_pRunningFlag]
		mov eax ,[eax]
*/
void __declspec(naked) WinMessagePatch_ASM() {
	__asm {
		call WinMessage
		ret
	}
}
/*
D2CLIENT.DLL+38BB0 - 57                    - push edi
D2CLIENT.DLL+38BB1 - 8B 7C 24 08           - mov edi,[esp+08]
D2CLIENT.DLL+38BB5 - F7 47 0C 00000040     - test [edi+0C],40000000 { 2.00 } <-----------
D2CLIENT.DLL+38BBC - 0F85 E3000000         - jne D2CLIENT.DLL+38CA5
D2CLIENT.DLL+38BC2 - 56                    - push esi
*/
void __declspec(naked) KeydownPatch_ASM() {
	__asm {
//edi = ptr to (hwnd, msg, wparam, lparam)
		movzx ecx,byte ptr [edi+0x08] //nVirtKey (wparam)
		//mov dl,[edi+0x0c+3] //lKeyData (lparam)
		//and dl,0x40 //bit 30 of lKeyData (lparam)
		//cmp dl,0
		//jne skip //repeat
		mov edx, 0
		pushad
		call keyDown
		cmp eax,0
		jne handled
		popad
//original code
		test byte ptr [edi+0x0c+3],0x40 //bit 30 of lKeyData (lparam)
		ret
handled:
		popad
		pop edi
		pop edi
		ret 4
	}
}
/*
D2CLIENT.DLL+38AD0 - 53                    - push ebx
D2CLIENT.DLL+38AD1 - 56                    - push esi
D2CLIENT.DLL+38AD2 - 57                    - push edi
D2CLIENT.DLL+38AD3 - 8B 7C 24 10           - mov edi,[esp+10]
D2CLIENT.DLL+38AD7 - 8B 77 08              - mov esi,[edi+08]
D2CLIENT.DLL+38ADA - 33 D2                 - xor edx,edx
D2CLIENT.DLL+38ADC - B9 E0A2BC6F           - mov ecx,D2CLIENT.DLL+11A2E0 { (0) }
D2CLIENT.DLL+38AE1 - 0FB7 41 04            - movzx eax,word ptr [ecx+04]  <-----------
D2CLIENT.DLL+38AE5 - 3B C6                 - cmp eax,esi
D2CLIENT.DLL+38AE7 - 75 23                 - jne D2CLIENT.DLL+38B0C
D2CLIENT.DLL+38AE9 - 8B 01                 - mov eax,[ecx]
*/
void __declspec(naked) KeyupPatch_ASM() {
	// edi = ptr to (hwnd, msg, wparam, lparam)
	//esi = keycode
	__asm {
		pushad
		mov ecx, esi
		mov edx, 0
		call keyUp
		popad
//original code
		movzx eax, word ptr [ecx+0x04]
		cmp eax, esi
		ret
	}
}
// 6FB5BF86 - 8B 0D 694BCBC6F        - mov ecx,[6FBCBC94] { (1) }
// 6FB5BF8C - 33 C0                 - xor eax,eax
void __declspec(naked) ReceiveSwapWeapon_ASM() {
	__asm {
		mov eax, dword ptr [d2client_pScreenBlocked]
		mov eax, dword ptr [eax]
		test eax, eax
		jne original
		mov eax, dword ptr [dwAutoCallToArm]
		test eax, eax
		je original
		mov ecx, dword ptr [dwCurMs]
		add ecx, eax
		mov dword ptr [dwCheckCTAMs], ecx
		xor eax,eax
		mov dword ptr [dwSwapWeaponKey], eax
		mov eax,1
		mov dword ptr [checkCTA], eax
//original code
original:
		mov eax, dword ptr [d2client_pActiveWeapon]
		mov ecx, dword ptr [eax]
		ret
	}
}
DWORD newTextBoxData[]={4,567,550,200,20,0,0,0,0,0,0,2};//type,x,y,l,h
static void createVersionTextBox() {
	char buf[64];
	if (*d2launch_plaunchNumChildren>=39) return;
	void* textbox = d2win_CreateTextBox(newTextBoxData);
	sprintf(buf, "d2hackmap %s", szVersion);
	d2win_PrintLineOnTextBox(textbox, buf, 4);
	d2launch_plaunchChildren[*d2launch_plaunchNumChildren]=textbox;
	d2launch_plaunchNumChildren[0]++;
}
//6FA5805B - 68 F011A66F           - push 6FA611F0 { (6) } <--- d2launch
void __declspec(naked) ShowVersion_Patch_ASM() {
	__asm {
		pop esi
		mov ecx, d2launch_pcreateTextBoxParam11F0
		push ecx
		push esi
		call createVersionTextBox
		ret
	}
}
/*
6FAC621B - 56                    - push esi
6FAC621C - 50                    - push eax
6FAC621D - FF 15 B4F0B76F        - call dword ptr [6FB7F0B4] { ->77D561B3 }
*/
void __declspec(naked) SetCursorPosPatch_ASM() {
	__asm {
		cmp fWinActive,0
		je skip
		jmp dword ptr [SetCursorPos]
skip:
		ret 8
	}
}

/*
6FAD71C0 - 83 EC 48              - sub esp,48 { 72 }
6FAD71C3 - 57                    - push edi
6FAD71C4 - 33 C0                 - xor eax,eax
...
6FAD7288 - 83 C4 48              - add esp,48 { 72 }
6FAD728B - C3                    - ret 
*/
void __declspec(naked) DrawLeftBackImagePatch_ASM() {
	__asm {
		cmp fNoBackgroundImage, 0
		jne skip
		cmp fHighRes,0
		jne skip
		pop eax
		sub esp, 0x48
		push edi
		push eax
		xor eax,eax
		ret
skip:
		pop eax
		xor eax,eax
		ret
	}
}
/*
6FAD70D0 - 8B 15 7CA7BC6F        - mov edx,[6FBCA77C] { (0EB20000) }
6FAD70D6 - 83 EC 48              - sub esp,48 { 72 }
6FAD70D9 - 85 D2                 - test edx,edx
...
6FAD71AE - 83 C4 48              - add esp,48 { 72 }
6FAD71B1 - C3                    - ret 
*/
void __declspec(naked) DrawRightBackImagePatch_ASM() {
	__asm {
		cmp fNoBackgroundImage, 0
		jne skip
		cmp fHighRes,0
		jne skip
		pop eax
		sub esp, 0x48
		push eax
		test edx,edx
		ret
skip:
		pop eax
		xor eax,eax
		ret
	}
}
static void debugMsDrawUnitStart() {LOG("	drawUnitStart=%d ms\n",GetTickCount()-dwCurMs);}
// 6FAF436F - E8 9C6D0400           - call 6FB3B110 <--- which draw units
void __declspec(naked) DrawGroundAndUnitPatch_ASM() {
	__asm {
		inc dwDrawCount
		cmp fDebugMs,0
		je start
		pushad
		call debugMsDrawUnitStart
		popad
start:
		jmp d2client_DrawAllUnits
	}
}
extern ToggleVar tScreenRouteGuide,tAutomapActiveRect;
void AutoRouteDrawScreenPath();
void drawUnitsInfo();
void drawActiveRectAreaOnScreen();
static void beforeDrawControl() {
	drawUnitsInfo();
	if (!(*d2client_pAutomapOn)) {
		if (tScreenRouteGuide.isOn) AutoRouteDrawScreenPath();
		if (tAutomapActiveRect.isOn&&dwCurrentLevel==Level_ArreatSummit) drawActiveRectAreaOnScreen();
	}
}
static void debugMsDrawControlStart() {LOG("	drawControlStart=%d ms\n",GetTickCount()-dwCurMs);}
static void debugMsDrawControlEnd() {LOG("	drawControlEnd=%d ms\n",GetTickCount()-dwCurMs);}
//d2client_4437A: push edx
//d2client_4437B: call d2client_DrawControls(1 args)
void __declspec(naked) DrawControlsPatch_ASM() {
	__asm {
		pushad
		call beforeDrawControl
		cmp fDebugMs,0
		je start
		call debugMsDrawControlStart
start:
		popad
		mov edx, [esp+4]
		push edx
		call d2client_DrawControls
		cmp fDebugMs,0
		je end
		pushad
		call debugMsDrawControlEnd
		popad
end:
		ret 4
	}
}
static void clearScreen() {
	if (!need_paint) return;
	char buf[128];
	RECT rect;
	HWND hwnd=d2gfx_GetHwnd();
	HDC hdc=GetDC(hwnd);
	need_paint=0;
	GetClientRect(hwnd, &rect); 
	StretchBlt(hdc,0,0,rect.right,rect.bottom,
		hdc,0,0,rect.right,rect.bottom,BLACKNESS);
	sprintf(buf, "Screen Saver");
	TextOut(hdc, 10, 10, buf,strlen(buf));
	ReleaseDC(hwnd,hdc);
}
static int shouldSkipAllDrawing() {
	if (fSkipPainting) {
		if (need_paint) return 0;
		if (*d2client_pNpcConvData) {
			__asm {
				mov eax, d2client_pNpcConvData
				mov eax, [eax]
				call d2client_drawNpcConversition
			}
		}
		return 1;
	}
	static int lastDrawMs=0;
	int ms=dwCurMs-lastDrawMs;
	if (fPlayerInTown&&ms<dwMinFrameMsInTown||!fPlayerInTown&&ms<dwMinFrameMs) {Sleep(1);return 1;}
	lastDrawMs=dwCurMs;
	return 0;
}
int dwDrawUnitCount=0;
static int shouldSkipDrawUnitAndControl() {
	if (fSkipPainting) {
		DrawCenterText(3, L"Screen Saver" , SCREENSIZE.x/2, SCREENSIZE.y/2,0,1,0);
		if (need_paint>=2) {need_paint=1;return 0;}
		need_paint=0;
		return 1;
	}
	dwDrawUnitCount++;
	return 0;
}
/*
	d2client_44361: 83 3D 14 C4 BC 6F 03  cmp dword ptr [d2client_11C414 int d2client_ScreenBlocked](->0), 3<--- install here
	d2client_44368: jz d2client_44374 ->+10 B44374
	...
	d2client_4436F: call d2hackmap.DrawGroundAndUnitPatch_ASM
	...
	d2client_4437B: call d2client_C39E0 d2hackmap.DrawControlsPatch_ASM
	d2client_44380: call d2client_D40E->d2win_187E0 void *__fastcall d2win_DrawMainMenu()(0 args)  <--- skip to here
*/
void __declspec(naked) SkipDrawUnitAndControlPatch_ASM() {
	__asm {
		pushad
		call shouldSkipDrawUnitAndControl
		test eax,eax
		popad
		jne skip
		mov eax, d2client_pScreenBlocked
		cmp dword ptr [eax], 3
		ret
skip:
		mov eax, 3
		cmp eax, 3
		//add dword ptr [esp], 0x18
		ret
	}
}
//d2client_4423C: call d2hackmap.SkipAllDrawingPatch_ASM ->D2gfx.Ordinal10043 //return 1 if skip all paintings
void __declspec(naked) SkipAllDrawingPatch_ASM() {
	__asm {
		pushad
		call shouldSkipAllDrawing
		test eax,eax
		popad
		jne skip
		mov eax,d2gfx_pWinState
		mov eax,[eax]
		ret
skip:
		mov eax,1
		ret
	}
}
int get_cpu_usage(int *ucpu,int *kcpu);
extern int dwAutoLoginMs,dwAutoSelectCharMs,autoJoinGameMs;
void autoLogin();
void autoSelectChar();
void autoJoinGame();
int mainMenuLoopCount; 
static int mainMenuSkip() {
	static int calFpsMs=0,ln=0,nextDrawMs=0;
	mainMenuLoopCount++;dwCurMs=GetTickCount();
	fInMainMenu=1;
	if (dwAutoLoginMs&&dwCurMs>dwAutoLoginMs&&(*d2win_pFocusedControl)) {dwAutoLoginMs=0;autoLogin();}
	if (dwAutoSelectCharMs&&dwCurMs>dwAutoSelectCharMs) {dwAutoSelectCharMs=0;autoSelectChar();}
	if (autoJoinGameMs&&dwCurMs>autoJoinGameMs) {autoJoinGameMs=0;autoJoinGame();}
	if (fSkipPaintingMenu) {
		if (need_paint) {
			return 0;
		}
		if (dwCurMs>nextDrawMs) {
			need_paint=2;
			nextDrawMs=dwCurMs+6000; //repaint screen every 6 seconds
		}
		return 1;
	}
	return 0;
}
static int skipDrawMenu2() {
	if (fSkipPaintingMenu) {
		DrawCenterText(3, L"Screen Saver" , SCREENSIZE.x/2, SCREENSIZE.y/2,0,1,0);
		if (need_paint>=2) {need_paint=1;return 0;}
		need_paint=0;
		return 1;
	}
	return 0;
}
/*
	d2win_1893D: 8B 35 A0 14 90 6F  mov esi, [d2win_214A0]
*/
void __declspec(naked) SkipDrawMenuPatch2_ASM() {
	__asm {
		pushad
		call skipDrawMenu2
		cmp eax,0
		popad
		jne skip
		mov esi, d2win_pvar_214A0
		mov esi, [esi]
		ret
skip:
		xor esi,esi
		ret
	}
}
//d2win_187E3: E8 10 F0 FE FF     call d2win_77F8->d2gfx_88B0 d2gfx/Ordinal10043
void __declspec(naked) SkipDrawMenuPatch_ASM() {
	__asm {
		pushad
		call mainMenuSkip
		cmp eax,0
		popad
		jne skip
		call d2gfx_Ordinal10043
		ret
skip:
		mov eax,1
		ret
	}
}
/*
6F8F73E5 - 83 3D 9814906F 01     - cmp dword ptr [6F901498],01 { (1),1 }
6F8F73EC - C7 05 FC9C9A6F 01000000 - mov [6F9A9CFC],00000001 { (1),1 }
6F8F73F6 - 74 07                 - je 6F8F73FF <----
6F8F73F8 - 6A 01                 - push 01 { 1 }
6F8F73FA - E8 9303FFFF           - call 6F8E7792 { ->->6FA87FC0  d2gfx_MinimizeWindows }
6F8F73FF - E8 B203FFFF           - call 6F8E77B6 { ->->6FA87E60 d2gfx.Ordinal10063}
*/
void __declspec(naked) NoHidePatch_ASM()
{
	__asm{
		jne hidewin
		cmp [fFullWindows] , 0
		jne justret
		cmp [tNoHide.isOn] , 1
		je justret
hidewin: 
		push 1
		call d2gfx_MinimizeWindows
justret:
		xor ecx, ecx
		call d2win_ToggleSound
		ret 
	}
}
