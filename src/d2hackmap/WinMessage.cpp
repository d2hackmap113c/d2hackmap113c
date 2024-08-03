#include "stdafx.h"

int fSkipPainting=0;
int dwQuickSwapItemMs=0;
int dwScreenBlockMask=0;
void MoveItem(D2MSG *pMsg );
void formatKey(char *buf,int vk);
static int altDown=0,ctrlDown=0,shiftDown=0;
static int dwCheckCTAMs=0,checkCTA=0,dwSwapWeaponKey=0;
static int isKeyDown[256];
extern ToggleVar *tKeyEventMap[8*256];
int lbtnDown=0,rbtnDown=0,mouseW,mouseL,lbtnFs=0,rbtnFs=0,lbtnSimUp=0,rbtnSimUp=0;
int enAutoLeftClick=0,enAutoRightClick=0;
int screenSaverMs=0;
static int checkLockMouseMs=0;
int IsKeyDown(int keyCode) {return isKeyDown[keyCode&0xFF];}
int LockMouse() {
	static RECT rcOldClip;
	static int locked=0,hasOldClip=0;
	WINDOWPLACEMENT wp;
	int needLock=tLockMouseToggle.isOn&&fWinActive&&fInGame&&!fPlayerInTown;
	HWND hwnd=D2GetHwnd();
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
		if (!ClipCursor(&rcOldClip)) return 0;
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
		} else if (left>0&&hasSkill(left-1)) {
			selectSkill(0,left-1);handled=1;
			break;
		}
	}
	for (int i=0;i<4;i++) {
		int right=pright[i];
		if (right==-3) {
			GameControl(44);handled=1;break;
		} else if (right>0&&hasSkill(right-1)) {
			selectSkill(1,right-1);handled=1;
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
	} else if (left>0&&hasSkill(left-1)) {
		selectSkill(0,left-1);handled=1;
	}
	if (right==-3) {
		GameControl(44);handled=1;
	} else if (right==-2) {//restore
		if (right0>=0&&hasSkill(right0)) {selectSkill(1,right0);handled=1;}
	} else if (right>0&&hasSkill(right-1)) {
		dwSwitchSkillRightOrg[id]=PLAYER->pSkill->pRightSkill->pSkillInfo->wSkillId;
		selectSkill(1,right-1);handled=1;
	}
	return handled;
}
int GameSound(int snd) {
	BYTE pkg[3] = {0x3f};
	*(WORD*)&pkg[1] = snd;
	//SendPacket(pkg,sizeof(pkg));		//Doesn't work on network
	D2PlaySound(PLAYER->dwUnitId,PLAYER->dwUnitType,snd);
	return 1;
}
int canAutoClick() {
	if (!fInGame) return 0;
	int x=mouseL&0xFFFF;
	int y=mouseL>>16;
	// potions (420-548,464)
	if (y>=SCREENSIZE.y-45) return 0;
	if (D2CheckUiStatus(UIVAR_BELT)) {
		if (y>=SCREENSIZE.y-136) {
			if (SCREENSIZE.x/2+20<=x&&x<=SCREENSIZE.x/2+150) return 0;
		}
	}
	return (*p_D2ScreenBlocked)==0;
}

void enableScreenSaver(int enable) {
	if (enable) {
		if (!tEnableScreenSaver.isOn) return;
		if (fScreenSaverActive) return;
		dwScreenBlockMask=p_D2ScreenBlocked[0]^3;
		*p_D2ScreenBlocked|=dwScreenBlockMask; 
		fScreenSaverActive=1;
	} else {
		*p_D2ScreenBlocked&=~dwScreenBlockMask;
		fScreenSaverActive=0;
	}
}
void delayScreenSaver(int ms) {
	if (fScreenSaverActive) enableScreenSaver(0);
	screenSaverMs=GetTickCount()+ms;
}
extern int fLoadedLib;
static void setWinTitle(HWND hwnd,int inGame) {
	char buf[256];
	char *realm=(*p_D2GameInfo)->szRealmName;
	char *account=(*p_D2GameInfo)->szAccountName;
	char *name=(*p_D2GameInfo)->szCharName;
	char *ip=(*p_D2GameInfo)->szGameServerIp;
	char *game=(*p_D2GameInfo)->szGameName;
	if (!inGame) {
		if (realm[0]&&account[0]) {
			sprintf(buf,"[%d]%s@%s Diablo II",dwGameWindowId,name,account);
		} else {
			sprintf(buf,"[%d]%s Diablo II",dwGameWindowId,name);
		}
	} else {
		if (realm[0]&&account[0]) {
			sprintf(buf,"[%d]%s@%s %s Diablo II",dwGameWindowId,name,account,game);
		} else {
			sprintf(buf,"[%d]%s Diablo II",dwGameWindowId,name);
		}
	}
	SetWindowTextA(hwnd,buf);
}
int FullScreen() {
	WINDOWPLACEMENT wp;
	HWND hwnd=D2GetHwnd();
	if (!GetWindowPlacement(hwnd,&wp)) return 0;
	int style=GetWindowLongA(hwnd,GWL_STYLE);
	if (wp.showCmd==SW_SHOWMAXIMIZED) {
		if (!(style&WS_CAPTION)) {
			style=style|WS_CAPTION;
			SetWindowLongA(hwnd,GWL_STYLE,style);
		}
		ShowWindow(hwnd,SW_SHOWNORMAL);
	} else {
		if (style&WS_CAPTION) {
			style=style&(~WS_CAPTION);
			SetWindowLongA(hwnd,GWL_STYLE,style);
		}
		ShowWindow(hwnd,SW_SHOWMAXIMIZED);
	}
	LockMouse();
	return 1;
}
int AttractNPC() {
	UnitAny *pSelectedUnit = D2GetSelectedUnit();
	if (pSelectedUnit&&pSelectedUnit->dwUnitType==1) {
		pSelectedUnit->dwUnitId;
		BYTE packet[17] = {0x59};
		*(DWORD*)&packet[1] = pSelectedUnit->dwUnitType;
		*(DWORD*)&packet[5] = pSelectedUnit->dwUnitId;
		int x0=PLAYER->pMonPath->wPosX;
		int y0=PLAYER->pMonPath->wPosY;
		*(DWORD*)&packet[9] = x0;
		*(DWORD*)&packet[13] = y0;
		SendPacket(packet,sizeof(packet));
		return 1;
	}
	return 0;
}
static int makeRuntimeInfoPath(char *path,int id) {
	char dir[512];
	if (!szRuntimePath[0]) sprintf(dir,"%sruntime",szPluginPath);
	else if (szRuntimePath[0]&&szRuntimePath[1]==':') sprintf(dir,"%s",szRuntimePath);
	else sprintf(dir,"%s%s",szPluginPath,szRuntimePath);
	CreateDirectoryA(dir,NULL);
	sprintf(path,"%s\\d2_%d.txt",dir,id);
	return 0;
}
int saveRuntimeInfo(HWND hwnd) {
	char path[512];
	makeRuntimeInfoPath(path,dwGameWindowId);
	FILE *fp=fopen(path,"w+");if (!fp) return -1;
	fprintf(fp,"hwnd: 0x%x\n",hwnd);
	if (fInGame) {
		char *ip=(*p_D2GameInfo)->szGameServerIp;
		char *game=(*p_D2GameInfo)->szGameName;
		char *name=(*p_D2GameInfo)->szCharName;
		if (ip[0]) fprintf(fp,"ip: %s\n",ip);
		if (game[0]) fprintf(fp,"game: %s\n",game);
		if (name[0]) fprintf(fp,"name: %s\n",name);
		fprintf(fp,"uid: %d\n",dwPlayerId);
	}
	fprintf(fp,"Command Line: %s\n",GetCommandLine());
	fclose(fp);
	return 0;
}
int loadRuntimeInfo(D2Window *pwin,int id) {
	char buf[256],path[512];
	makeRuntimeInfoPath(path,id);
	if (!fileExist(path)) return 0;
	memset(pwin,0,sizeof(D2Window));
	FILE *fp=fopen(path,"r");if (!fp) return 0;
	while (1) {
		char *line=fgets(buf,256,fp);if (!line) break;
		int len=strlen(line);
		if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[len-1]=='\r') {len--;line[len]=0;}
		char *name=line;char *value=strchr(line,':');
		if (!value) continue;*value=0;value++;while (*value==' ') value++;
		if (strcmp(name,"ip")==0) strcpy(pwin->ip,value);
		else if (strcmp(name,"game")==0) strcpy(pwin->game,value);
		else if (strcmp(name,"name")==0) strcpy(pwin->name,value);
		else if (strcmp(name,"hwnd")==0) pwin->hwnd=(HWND)strtol(value,0,16);
		else if (strcmp(name,"uid")==0) pwin->uid=strtol(value,0,10);
	}
	fclose(fp);
	int len=GetClassName(pwin->hwnd,buf,256);
	if (len>0&&strcmp(buf,"Diablo II")==0) return 1;
	memset(pwin,0,sizeof(D2Window));
	return 0;
}
int find_team_members(D2Window *wins,int cap,int groupSize,int distance) {
	int n=0;
	char *ip=(*p_D2GameInfo)->szGameServerIp;
	char *game=(*p_D2GameInfo)->szGameName;
	if (!ip[0]||!game[0]) return 0;
	for (int i=1;i<=8;i++) {
		if (n>=cap) break;
		if (i==dwGameWindowId) continue;
		if (dwGameWindowId<=groupSize&&i>groupSize||dwGameWindowId>groupSize&&i<=groupSize) continue;
		D2Window *pwin=&wins[n];
		if (!loadRuntimeInfo(pwin,i)) continue;
		if (strcmp(pwin->ip,ip)==0&&strcmp(pwin->game,game)==0) {
			//RosterUnit *pRU=getRosterUnit(pwin->uid);
			//if (!pRU||area>=0&&pRU->dwLevelNo!=area) continue;
			if (distance) {
				UnitAny *pUnit=D2GetUnitFromId(pwin->uid, UNITNO_PLAYER) ;
				if (!pUnit) continue;
				float d=getUnitDistance(PLAYER,pUnit);
				if (d>distance) continue;
			}
			n++;
		}
	}
	return n;
}
int isLoginScreen() {
	if (fInGame) return 0;
	D2EditBox *focus=(*p_D2FocusedControl);
	if (!focus) return 0;
	D2EditBox *next=focus->pNext2;
	if (!next) return 0;
	if (next->pNext2!=focus) return 0;
	D2EditBox *nameBox,*passBox;
	if (focus->dwPosY<next->dwPosY) {nameBox=focus;passBox=next;}
	else {nameBox=next;passBox=focus;}
	if (nameBox->dwPosY!=342) return 0;
	if (passBox->dwPosY!=396) return 0;
	return 1;
}
int SwitchWindow(int id) {
	if (isLoginScreen()) return 0;
	id++;
	if (id==dwGameWindowId) return 0;
	D2Window win;
	if (!loadRuntimeInfo(&win,id)) return 0;
	SetForegroundWindow(win.hwnd);
	//SwitchToThisWindow(win.hwnd,1);
	return 1;
}
void WinMessageNewGame() {
	fScreenSaverActive=0;
	fSkipPainting=0;
	forceStandStill=0;
	dwQuickSwapItemMs=0;dwCheckCTAMs=0;dwSwapWeaponKey=0;
	LockMouse();
	HWND hwnd=D2GetHwnd();
	if (SCREENSIZE.x>=(int)dwAutoMaximizeWidth||SCREENSIZE.y>=(int)dwAutoMaximizeHeight) {
		if (dwHideCaptionBorder) {
			int style=GetWindowLongA (hwnd,GWL_STYLE);
			if (style&WS_CAPTION) {
				style=style&(~WS_CAPTION);
				SetWindowLongA(hwnd,GWL_STYLE,style);
			}
		}
		ShowWindow(hwnd,SW_SHOWMAXIMIZED);
	} 
	setWinTitle(hwnd,1);
	saveRuntimeInfo(hwnd);
}
void WinMessageEndGame() {
	fScreenSaverActive=0;
	fSkipPainting=0;
	LockMouse();
	WINDOWPLACEMENT wp;
	HWND hwnd=D2GetHwnd();
	if (SCREENSIZE.x>=(int)dwAutoMaximizeWidth||SCREENSIZE.y>=(int)dwAutoMaximizeHeight) {
		if (GetWindowPlacement(hwnd,&wp)) {
			if (wp.showCmd==SW_SHOWMAXIMIZED) ShowWindow(hwnd,SW_SHOWNORMAL);
		}
		if (dwHideCaptionBorder) {
			int style=GetWindowLongA(hwnd,GWL_STYLE);
			if (!(style&WS_CAPTION)) {
				style=style|WS_CAPTION;
				SetWindowLongA(hwnd,GWL_STYLE,style);
			}
		}
	}
	setWinTitle(hwnd,0);
	saveRuntimeInfo(hwnd);
}
void refreshScreenSaver() {
	fSkipPainting=tEnableScreenSaver.isOn&&!fWinActive;
	if(!fWinActive&&tAutoEnchant.isOn&&PLAYER->pSkill->pRightSkill->pSkillInfo->wSkillId ==52 ) 
		screenSaverMs=GetTickCount()+dwAutoEnchantScreenSaverDelayMs;
	else
		screenSaverMs=GetTickCount();
}
void winActiveMsg(int active) {
	if (!fInGame) return;
	enAutoLeftClick=0;
	enAutoRightClick=0;
	forceStandStill=0;
	fWinActive=active;
	refreshScreenSaver();
}
int hasCTA() {
	for (UnitAny *pUnit = D2GetFirstItemInInv(PLAYER->pInventory);pUnit;pUnit = D2GetNextItemInInv(pUnit)) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) continue ;
		if (pUnit->pItemData->nItemLocation!=255) continue;
		if (pUnit->pItemData->nBodyLocation==4) {
			if ( D2CheckItemFlag(pUnit, ITEMFLAG_RUNEWORD, 0, "?")
				&&D2GetRuneWordTxtIndex( pUnit->pItemData->wMagicPrefix[0])==27) {
				return 1;
			} else return 0;
		}
	}
	return 0;
}
void WinMessageRunLoop() {
	static int inTown=0;
	if (fPlayerInTown!=inTown) {LockMouse();inTown=fPlayerInTown;}
	if (checkLockMouseMs&&dwCurMs>=checkLockMouseMs) {LockMouse();checkLockMouseMs=0;}
	int curMs=GetTickCount();
	if (dwCheckCTAMs) {
		static int swapBackMs=0,ctaWeapon;
		if (!dwSwapWeaponKey) {
			dwSwapWeaponKey=getGameControlKey(44);
			if (dwSwapWeaponKey<=0) dwSwapWeaponKey='W';
		}
		if (GetKeyState(dwSwapWeaponKey)&0x8000) *p_D2StandStill=1;
		if (dwCurMs>dwCheckCTAMs) {
			if (checkCTA) {
				checkCTA=0;
				if (hasCTA()) {
					if (PLAYER->pSkill->pLeftSkill->pSkillInfo->wSkillId!=155) selectSkill(0,155);
					if (PLAYER->pSkill->pRightSkill->pSkillInfo->wSkillId!=149) selectSkill(1,149);
					ctaWeapon=*p_D2ActiveWeapon;
					swapBackMs=0;
				} else dwCheckCTAMs=0;
			} else {
				if (!p_D2StandStill[0]) {
					if (ctaWeapon==*p_D2ActiveWeapon) {
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
			*p_D2StandStill=0;
	}
	if (dwQuickSwapItemMs&&dwCurMs>dwQuickSwapItemMs) {
		UnitAny *pItem = PLAYER->pInventory->pCursorItem;
		if (pItem) {
			HWND hwnd=D2GetHwnd();
			PostMessage(hwnd, WM_RBUTTONDOWN, 0, mouseL);
			PostMessage(hwnd, WM_RBUTTONUP, 0, mouseL);
		}
		dwQuickSwapItemMs=0;
	}
	if (screenSaverMs&&curMs>=screenSaverMs) {
		if (fScreenSaverActive==fWinActive) {
			enableScreenSaver(!fWinActive);
		}
		screenSaverMs=0;
	}
	if (tAutoHideMinimapToggle.isOn) {
		int on=0;
		if (tAutoMinimapTeleportToggle.isOn&&dwRightSkill==54) on=1;
		if (dwAutoHideMinimapKey>0&&isKeyDown[dwAutoHideMinimapKey]) on=!on;
		int ison=*p_D2AutomapOn;//D2CheckUiStatus(UIVAR_AUTOMAP);
		if (ison!=on) GameControl(7);
	}
	if (lbtnFs) lbtnFs--;
	if (rbtnFs) rbtnFs--;
	if (enAutoLeftClick&&lbtnFs<=0) {
		if (!lbtnDown) {
			enAutoLeftClick=0;
		} else {
			HWND hwnd=D2GetHwnd();
			if (lbtnSimUp==0) {
				PostMessageA(hwnd,WM_LBUTTONUP,mouseW|0x80000000,mouseL);
				lbtnSimUp=1;lbtnFs=dwAutoClickLeftUpFs;
			} else {
				if (canAutoClick()) {
					PostMessageA(hwnd,WM_LBUTTONDOWN,mouseW|0x80000000,mouseL);
					lbtnSimUp=0;lbtnFs=dwAutoClickLeftDownFs;
				} else enAutoLeftClick=0;
			}
		}
	}
	if (enAutoRightClick&&rbtnFs<=0) {
		if (!rbtnDown) {
			enAutoRightClick=0;
		} else {
			HWND hwnd=D2GetHwnd();
			if (rbtnSimUp==0) {
				PostMessageA(hwnd,WM_RBUTTONUP,mouseW|0x80000000,mouseL);
				rbtnSimUp=1;rbtnFs=dwAutoClickRightUpFs;
			} else {
				if (canAutoClick()) {
					PostMessageA(hwnd,WM_RBUTTONDOWN,mouseW|0x80000000,mouseL);
					rbtnSimUp=0;rbtnFs=dwAutoClickRightDownFs;
				} else enAutoRightClick=0;
			}
		}
	}
}
static void c2w(char *p,wchar_t *w) {
	while (1) {*w=(*p)&0xFF;if (!*p) break;w++;p++;}
}
int inputUserPassword(int id) {
	char *name=szAccountNames[id];
	char *password=szAccountPasswords[id];
	wchar_t wb[64]={0};
	D2EditBox *focus=(*p_D2FocusedControl);
	if (!focus) return -1;
	D2EditBox *next=focus->pNext2;
	if (!next) return -1;
	if (next->pNext2!=focus) return -1;
	D2EditBox *nameBox,*passBox;
	if (focus->dwPosY<next->dwPosY) {nameBox=focus;passBox=next;}
	else {nameBox=next;passBox=focus;}
	if (nameBox->dwPosY!=342) return -1;
	if (passBox->dwPosY!=396) return -1;
	c2w(password,wb);D2SetEditBoxText(passBox,wb);
	c2w(name,wb);D2SetEditBoxText(nameBox,wb);
	return 0;
}
static void setupAutoClick(int left) {
	if (!fInGame) return;
	if (!tAutoClickLeft.isOn&&!tAutoClickRight.isOn) return;
	if (!canAutoClick()) return;
	if (left) {
		if (!lbtnDown||!tAutoClickLeft.isOn) {enAutoLeftClick=0;return;}
		lbtnFs=dwAutoClickLeftDownFs;enAutoLeftClick=1;lbtnSimUp=0;
	} else {
		if (!rbtnDown||!tAutoClickRight.isOn) {enAutoRightClick=0;return;}
		rbtnFs=dwAutoClickRightDownFs;enAutoRightClick=1;rbtnSimUp=0;
	}
}
static int __fastcall keyDown(int vk,int winMsg) {
	if (vk<1||vk>=256) return 0;
	switch (vk) {
		case VK_SHIFT:shiftDown=1;break;
		case VK_CONTROL:ctrlDown=1;break;
		case VK_MENU:altDown=1;break;
	}
	if (winMsg&&fInGame) return 0;//Don't know how to detect gold number input box
	if (isKeyDown[vk]) return 0;
	isKeyDown[vk]=1;
	if (vk!=VK_MENU&&altDown) vk|=0x100;
	if (vk!=VK_CONTROL&&ctrlDown) vk|=0x200;
	if (vk!=VK_SHIFT&&shiftDown) vk|=0x400;
	int handled=0;
	int inGame=fInGame&&!D2CheckUiStatus(UIVAR_CHATINPUT);
	for (ToggleVar *p = tKeyEventMap[vk];p;p=p->next) {
		if(vk==p->key) {
			if (!fInGame) {
				if (p->keyType==0) continue;
			} else if (inGame) {
				if (p->keyType==1) continue;
			}
			if(p->type==TOGGLEVAR_ONOFF){
				p->isOn = !p->isOn;
				if (p->funcVoid) (p->funcVoid)();
				if (p->desc){
					wchar_t wszbuf[512];
					memset(wszbuf , 0  , sizeof(wszbuf));
					wszbuf[0] = L'\0';
					D2ShowPartyMessage( wsprintfW2(wszbuf, "<Hackmap>: %s %s.", p->desc, p->isOn ? "on" : "off" ) ,   0 );
				}
				handled=1;
			}else if(p->type==TOGGLEVAR_DOWN){
				if (p->funcVoid&&p->funcVoid()) handled=1;
			}else if(p->type==TOGGLEVAR_DOWNPARAM||p->type==TOGGLEVAR_DOWNUPPARAM){
				if (p->funcParam&&p->funcParam(p->param)) handled=1;
			}
		}
	}
	//if (vk==0x34D) dumpSymbols();//Ctrl+Alt+M
	return handled;
}
static int __fastcall keyUp(int vk,int winMsg) {
	if (vk<0||vk>=256) return 0;
	switch (vk) {
		case VK_SHIFT:shiftDown=0;break;
		case VK_CONTROL:ctrlDown=0;break;
		case VK_MENU:altDown=0;break;
	}
	if (winMsg&&fInGame) return 0;//Don't know how to detect gold number input box
	if (!isKeyDown[vk]) return 0;
	isKeyDown[vk]=0;
	if (vk!=VK_MENU&&altDown) vk|=0x100;
	if (vk!=VK_CONTROL&&ctrlDown) vk|=0x200;
	if (vk!=VK_SHIFT&&shiftDown) vk|=0x400;
	if (vk==dwKeyDown) fKeyRepeat=0;
	int handled=0;
	for (ToggleVar *p = tKeyEventMap[vk];p;p=p->next) {
		if(vk==p->key) {
			if (!fInGame) {
				if (p->keyType==0) continue;
			} else {
				if (p->keyType==1) continue;
			}
			if(p->type==TOGGLEVAR_DOWNUPPARAM) {if (p->funcUp&&p->funcUp(p->param)) handled=1;}
		}
	}
	return handled;
}
int WinMessage(int retAddr,int retAddr2,HWND hwnd,int msg,int w,int l) {
	static int savedRuntime=0;
	if (! *p_D2RunningFlag) return 0;
	switch (msg) {
		case WM_MOUSEWHEEL: {
			short z=(short)(w>>16);
			int k=z>0?VK_WHEELUP:VK_WHEELDOWN;
			if (keyDown(k,0)) {keyUp(k,0);return 0;}
			keyUp(k,0);
			break;
		}
		case WM_MBUTTONDOWN:if (keyDown(VK_MBUTTON,0)) return 0;break;
		case WM_MBUTTONUP:keyUp(VK_MBUTTON,0);break;
		case WM_XBUTTONDOWN:
			if (w&MK_XBUTTON1) keyDown(VK_XBUTTON1,0);
			if (w&MK_XBUTTON2) keyDown(VK_XBUTTON2,0);
			break;
		case WM_XBUTTONUP:
			keyUp(VK_XBUTTON1,0);
			keyUp(VK_XBUTTON2,0);
			break;
		case WM_ACTIVATE: //w=1 WA_ACTIVE, w=2 WA_CLICKACTIVE, w=0x200001 minimized
			winActiveMsg(w&&w!=0x200001?1:0);
			if (w) checkLockMouseMs=dwCurMs+200;
			else LockMouse();
			if (!savedRuntime) {savedRuntime=1;saveRuntimeInfo(hwnd);}
			break;
		case WM_NCMOUSELEAVE:
			break;
		case WM_LBUTTONDOWN:
			if (fScreenSaverActive) return 0;
			if (w&0x80000000) { //simulate click
				w&=0x7FFFFFFF;if (!lbtnDown) return 0;
			} else {
				if (fInGame) {
					if (PLAYER->pInventory->pCursorItem) {
						if (tDropProtectionToggle.isOn) {
							if (
								(!(p_D2ScreenBlocked[0]&1)&&p_D2MousePos->y>SCREENSIZE.x/2) //x,y is inverted
								||
								(p_D2MousePos->y<SCREENSIZE.x/2&&(!(p_D2ScreenBlocked[0]&2)||D2CheckUiStatus(UIVAR_NPCTRADE)))
								) {
								if (DropProtection(PLAYER->pInventory->pCursorItem)) {
									char keyname[256];formatKey(keyname,tDropProtectionToggle.key);
									wchar_t wszbuf[256];
									wsprintfW(wszbuf, L"Drop Protection on, press %hs to disable protection",keyname);
									D2ShowGameMessage(wszbuf, 0);
									return 0;
								}
							}
						}
					} else {
						dwQuickSwapItemMs=0;
						if ((*p_D2ScreenBlocked&1) &&(GetKeyState(VK_SHIFT)&0x8000||GetKeyState(VK_MENU)&0x8000)) {
							if (D2CheckUiStatus(UIVAR_INVENTORY)||D2CheckUiStatus(UIVAR_CUBE)
								||D2CheckUiStatus(UIVAR_STASH)||D2CheckUiStatus(UIVAR_PPLTRADE))
								dwQuickSwapItemMs=dwCurMs+(fSinglePlayer?0:dwQuickSwapItemDelayMs);
							//click map on left half screen
							if (!(p_D2ScreenBlocked[0]&2)&&p_D2MousePos->y<=SCREENSIZE.x/2)
								dwQuickSwapItemMs=0;
						}
						lbtnDown=1;mouseW=w;mouseL=l;
						setupAutoClick(1);
					}
				}
			}
			break;
		case WM_LBUTTONUP:
			if (fScreenSaverActive) return 0;
			if (w&0x80000000) { //simulate click
				w&=0x7FFFFFFF;if (!lbtnDown) return 0;
			} else {
				lbtnDown=0;enAutoLeftClick=0;
			}
			break;
		case WM_RBUTTONDOWN:
			if (fScreenSaverActive) return 0;
			if (w&0x80000000) { //simulate click
				w&=0x7FFFFFFF;if (!rbtnDown) return 0;
			} else {
				rbtnDown=1;mouseW=w;mouseL=l;
				setupAutoClick(0);
			}
			break;
		case WM_RBUTTONUP:
			if (fScreenSaverActive) return 0;
			if (w&0x80000000) { //simulate click
				w&=0x7FFFFFFF;if (!rbtnDown) return 0;
			} else {
				rbtnDown=0;enAutoRightClick=0;
			}
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
				case VK_AUTOFOLLOW_START:
					follower_start_follow(l);
					return 0;
				case VK_AUTOFOLLOW_STOP:
					follower_stop_follow();
					return 0;
				case VK_MULTICLIENT_ENTER_DOOR:
					follower_enter_door(l&0xFFFFFF,(l>>24)&0xFF);
					return 0;
				case VK_MULTICLIENT_BACK_TO_TOWN:
					follower_back_to_town(l);
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
				case VK_AUTOFOLLOW_CLICK_OBJECT:
					follower_click_object((l>>28)&0xF,l&0xFFFFFFF);
					return 0;
				case VK_MULTICLIENT_INFO:
					leader_recv_info(l);
					return 0;
				default:
					keyUp(w,1);
			}
			break;
		case WM_CLOSE: {
			char path[512];
			makeRuntimeInfoPath(path,dwGameWindowId);
			DeleteFileA(path);
			break;
		}
	}
	return *p_D2RunningFlag;
}
/*
window message processor: 6f8f7740
D2Win.dll+17740 - A1 C014906F           - mov eax,[D2Win.dll+214C0] { (1) }
6F8F7740 - A1 C014906F           - mov eax,[6F9014C0] { (1) }
		mov eax ,[p_D2RunningFlag]
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
		mov eax, dword ptr [p_D2ScreenBlocked]
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
		mov eax, dword ptr [p_D2ActiveWeapon]
		mov ecx, dword ptr [eax]
		ret
	}
}
// 6FA8B0A1 - FF 90 84000000        - call dword ptr [eax+00000084]
void __declspec(naked) DrawCellFile_Patch_ASM() {
	__asm {
		cmp fSkipPainting, 0
		je original
		xor eax,eax
		ret 16
		//original code
original:
		jmp dword ptr [eax+0x84]
	}
}
// 6FA8B00C - FF 90 98000000        - call dword ptr [eax+00000098]
void __declspec(naked) DrawAutomapCell_Patch_ASM() {
	__asm {
		cmp fSkipPainting, 0
		je original
		xor eax,eax
		ret 12
		//original code
original:
		jmp dword ptr [eax+0x98]
	}
}
DWORD newTextBoxData[]={4,567,550,200,20,0,0,0,0,0,0,2};//type,x,y,l,h
static void createVersionTextBox() {
	char buf[64];
	if (*p_D2launchNumChildren>=39) return;
	void* textbox = D2CreateTextBox(newTextBoxData);
	sprintf(buf, "d2hackmap %s", szVersion);
	D2PrintLineOnTextBox(textbox, buf, 4);
	p_D2launchChildren[*p_D2launchNumChildren]=textbox;
	p_D2launchNumChildren[0]++;
}
//6FA5805B - 68 F011A66F           - push 6FA611F0 { (6) } <--- d2launch
void __declspec(naked) ShowVersion_Patch_ASM() {
	__asm {
		pop esi
		mov ecx, p_D2createTextBoxParam11F0
		push ecx
		push esi
		call createVersionTextBox
		ret
	}
}
