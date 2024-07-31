#include "stdafx.h"

extern int dwNecMana,dwNecInfoExpireMs,dwReviveTimePercent;
extern int dwInteractEntityCount;

void MultiClientSendPos();
static D2Window d2wins[8];
static int waitToMs=0,enterRoomAttempts;
static int followState=0,followerInteractType,followerInteractId,followerInteractTimeout,fSendInfoErr,followerWaypointToArea,followerClickCount;
static HWND followHWND;
enum FollowState {
	FS_FOLLOW=0,
	FS_CLICK_OBJECT,
	FS_CLICKED_OBJECT,
	FS_WAIT_WAYPOINT_MENU,
	FS_CLICK_WAYPOINT_MENU,
};

static const int waypointAreas[5][9]={
{1,3,4,5,6,27,29,32,35},
{40,48,42,57,43,44,52,74,46},
{75,76,77,78,79,80,81,83,101},
{103,106,107,0,0,0,0,0,0},
{109,111,112,113,115,123,117,118,129},
};
void MultiClientNewGame() {
	dwMultiClientCount=0;dwMultiClientFollowId=0;waitToMs=0;enterRoomAttempts=0;followState=0;followHWND=0;fSendInfoErr=0;
	dwMultiClientClickUnitId=0;fTransferClick=0;wszTransferClick[0]=0;followerInteractType=0;followerInteractId=0;fAutoFollowMoving=0;followerClickCount=0;
}
extern int dwScreenBlockMask;
int canRemoteControl() {
	if (fWinActive) return 0;
	int screenBlock=p_D2ScreenBlocked[0];
	if (fScreenSaverActive) screenBlock&=~dwScreenBlockMask;
	if ((screenBlock&3)==3) return 0;
	if (D2CheckUiStatus(UIVAR_WAYPOINT)) {
		int esc=getGameControlKey(56);
		HWND hwnd=D2GetHwnd();
		PostMessage(hwnd, WM_KEYDOWN, esc, 0);
		PostMessage(hwnd, WM_KEYUP, esc, 0);
	}
	return 1;
}
extern int mouseW,mouseL;
//return -1:error 1:clicked tab 2:clicked menu
int takeWaypointToArea(int level) {
/*
type=2 (object)
	waypoint:txt=119,156,157,237,238,288,323,324,398,402,429,496,511
	blue portal txt=59 
	red portal txt=60
expansion tab position: 118+62*[0-4],78
classic tab position: 124+76*[0-3]
mouse=112, 137+36*[0-8]
*/
	if (!D2CheckUiStatus(UIVAR_WAYPOINT)) return -1;
	int ret=0,act=-1,menuId=-1;
	for (int i=0;i<5;i++) {
		for (int j=0;j<9;j++) {
			if (waypointAreas[i][j]==level) {act=i;menuId=j;break;}
		}
	}
	if (menuId<0) {gameMessage("Waypoint to area %d doesn't exist",level);return -1;}
	int xpos,ypos;
	if (*p_D2WaypointMenuCurTab != act) {
		if (EXPANSION) xpos=118+62*act;
		else {
			xpos=124+76*act;
			if (act>=4) {gameMessage("Waypoint to area %d doesn't exist",level);return -1;}
		}
		ypos=78;
		ret=1;
	} else {
		struct WaypointMenuItem *pitem=NULL;
		xpos=112;
		for (int i=0;i< *p_D2WaypointMenuCount;i++) {
			struct WaypointMenuItem *p=&p_D2WaypointMenuItems[i];
			if (p->areaId==level) {pitem=p;ypos=137+36*i;break;}
		}
		if (!pitem) {gameMessage("Waypoint to area %d doesn't match",level);return -1;}
		if (!pitem->enable) {gameMessage("Waypoint to area %d inactive",level);return -1;}
		ret=2;
	}
	if (fScreenSaverActive) delayScreenSaver(80);
	xpos+=(SCREENSIZE.x-800)/2;
	ypos+=(SCREENSIZE.y-600)/2;
	int wParam=0;//mouseW;
	HWND hwnd=D2GetHwnd();
	PostMessage(hwnd, WM_MOUSEMOVE, wParam, MAKELONG(xpos, ypos));
	PostMessage(hwnd, WM_LBUTTONDOWN, wParam, MAKELONG(xpos, ypos));
	PostMessage(hwnd, WM_LBUTTONUP, wParam, MAKELONG(xpos, ypos));
	PostMessage(hwnd, WM_MOUSEMOVE, wParam, mouseL);
	return ret;
}
void leader_click_object(int type,int id) {
	int lParam=((type&0xF)<<28)|(id&0xFFFFFFF);
	int err=0;
	for (int i=0;i<dwMultiClientCount;i++) {
		if (!PostMessageA(d2wins[i].hwnd,WM_KEYUP,VK_AUTOFOLLOW_CLICK_OBJECT,lParam)) err++;
	}
	if (err) gameMessage("MultiClient error: %d",err);
}
void leader_recv_info(int info) {
	if (dwPlayerClass==2) return;
	int type=(info>>24)&0xFF;
	switch (type) {
		case 1:
			dwSkeletonCount=(info>>16)&0xFF;
			dwSkeletonMageCount=(info>>8)&0xFF;
			dwReviveCount=info&0xFF;
			dwNecInfoExpireMs=dwCurMs+1000;
			break;
		case 2:
			dwSkeletonHPPercent=(info>>16)&0xFF;
			dwReviveTimePercent=(info>>8)&0xFF;
			dwNecInfoExpireMs=dwCurMs+1000;
			break;
		case 3:
			dwNecMana=info&0xFFFF;
			dwNecInfoExpireMs=dwCurMs+1000;
			break;
	}
}
void follower_send_info(int info) {
	if (fSendInfoErr||!dwMultiClientFollowId) return;
	if (!followHWND) {
		D2Window d2wins[8];
		int n=getOtherClients(d2wins,8,-1);
		for (int i=0;i<n;i++) {
			if (d2wins[i].uid==dwMultiClientFollowId) {followHWND=d2wins[i].hwnd;break;}
		}
	}
	if (!followHWND) {fSendInfoErr=1;gameMessage("Multiclient send info no target");return;}
	int err=0;
	if (!PostMessageA(followHWND,WM_KEYUP,VK_MULTICLIENT_INFO,info)) {
		fSendInfoErr=1;
		gameMessage("MultiClient send info error: %d",err);
	}
}
void follower_stop_follow() {
	dwMultiClientFollowId=0;enterRoomAttempts=0;followerInteractType=0;followerInteractId=0;followState=0;fAutoFollowMoving=0;followHWND=0;
	gameMessage("stop follow");
}
void follower_click_object(int type,int id) {
	switch (type) {
		case 2:case 5:followerWaypointToArea=0;break;
		case 8: type=2;followerWaypointToArea=0;break; //waypoint
		case 9: followerWaypointToArea=id;return;//waypoint to level
	}
	followerInteractType=type;followerInteractId=id;followerInteractTimeout=dwCurMs+6000;
	followState=FS_CLICK_OBJECT;followerClickCount=0;
	waitToMs=dwCurMs+dwMultiClientCheckInterval;
}
static void interactObject() {
	static int dwInteractEntityCountVerify=0;
	switch (followState) {
		case FS_CLICK_OBJECT: {
			UnitAny *portal=GetUnitFromIdSafe(followerInteractId,followerInteractType);
			if (!portal) {followState=0;gameMessage("can't find type=%d id=%d",followerInteractType,followerInteractId);return;}
			if (portal&&portal->dwTxtFileNo==267) {followState=0;return;} //stash
			if (fScreenSaverActive) delayScreenSaver(80);
			fAutoFollowMoving=1;dwInteractEntityCountVerify=dwInteractEntityCount;LeftClickObject(portal);
			followState=FS_CLICKED_OBJECT;followerClickCount++;
			break;
		}
		case FS_CLICKED_OBJECT: {
			if (dwInteractEntityCountVerify==dwInteractEntityCount) { //not clicked yet
				if (dwCurMs>followerInteractTimeout) {
					followState=0;
					gameMessage("click type=%d id=%d clicked=%d timeout",
						followerInteractType,followerInteractId,followerClickCount);
				} else {
					followState=FS_CLICK_OBJECT;
				}
			} else {
				followState=followerWaypointToArea?FS_WAIT_WAYPOINT_MENU:0;
			}
			break;
		}
		case FS_WAIT_WAYPOINT_MENU: {
			if (!D2CheckUiStatus(UIVAR_WAYPOINT)) {
				if (dwCurMs>followerInteractTimeout) {followState=0;gameMessage("wait waypoint menu timeout");}
			} else {
				followState=FS_CLICK_WAYPOINT_MENU;
			}
			break;
		}
		case FS_CLICK_WAYPOINT_MENU: {
			int ret=takeWaypointToArea(followerWaypointToArea);
			if (ret==1) {
				if (dwCurMs>followerInteractTimeout) {followState=0;gameMessage("click waypoint tab timeout");}
			} else {
				followState=0;
			}
			break;
		}
		default:followState=0;break;
	}
}
void MultiClientLoop() {
	if (dwMultiClientClickTimeout) {
		dwMultiClientClickTimeout--;
		if (!dwMultiClientClickTimeout) dwMultiClientClickUnitId=0;
	}
	if (!dwMultiClientFollowId) return;
	if (fWinActive) {if (dwMultiClientFollowId&&fPlayerInTown) follower_stop_follow();return;}
	if (waitToMs&&dwCurMs<waitToMs) return;
	if (followState) {interactObject();waitToMs=dwCurMs+dwMultiClientCheckInterval;return;}
	int targetX=0,targetY=0;
	RosterUnit *pRU=getRosterUnit(dwMultiClientFollowId);
	if (!pRU) {follower_stop_follow();return;}
	UnitAny *pUnit=D2GetUnitFromId(dwMultiClientFollowId , UNITNO_PLAYER) ;
	if (pUnit) {
		targetX=pUnit->pMonPath->wPosX;
		targetY=pUnit->pMonPath->wPosY;
	}
	int x0=PLAYER->pMonPath->wPosX;
	int y0=PLAYER->pMonPath->wPosY;
	int dx=targetX-x0;
	int dy=targetY-y0;
	int abs_x=dx<0?-dx:dx;
	int abs_y=dy<0?-dy:dy;
	int dis=abs_x>abs_y?abs_x:abs_y;
	if (!pUnit||!canRemoteControl() ||dis<dwMultiClientDistance||dis>dwMultiClientMaxDistance) {
		fAutoFollowMoving=0;
		return;
	}
	fAutoFollowMoving=1;
	if (PLAYER->dwMode==0xA||!canUseSkillNow(PLAYER,dwRightSkill)) return;
	int d2=dx*dx+dy*dy;
	int fd2=dwMultiClientDistance*dwMultiClientDistance;
	float expectDistance=sqrt((float)fd2);
	float actualDistance=sqrt((float)d2);
	float moveDistance=actualDistance-expectDistance;
	if (moveDistance>dwMultiClientMoveDistance) moveDistance=(float)dwMultiClientMoveDistance;
	float percent=moveDistance/actualDistance;
	int mx=(int)(dx*percent);
	int my=(int)(dy*percent);
	if (!mx&&!my) {
		if (dx>0) mx=1;else if (dx<0) mx=-1;
		if (dy>0) my=1;else if (dy<0) my=-1;
	}
	int tx=x0+mx;
	int ty=y0+my;
	RunToPos(tx,ty);
	waitToMs=dwCurMs+dwMultiClientCheckInterval;
	enterRoomAttempts=0;
}
void follower_start_follow(int uid) {
	dwMultiClientFollowId=uid;waitToMs=0;enterRoomAttempts=0;followState=0;fAutoFollowMoving=0;fSendInfoErr=0; followerWaypointToArea=0;
	gameMessage("Start follow %d",dwMultiClientFollowId);
}
void follower_enter_door(int uid,int area) {
	dwMultiClientFollowId=0;enterRoomAttempts=0;
	if (!tMultiClient.isOn||!fInGame) return;
	if (!canRemoteControl()) return;
	RosterUnit *pRU=getRosterUnit(uid);
	if (!pRU) return;
	enterRoomAttempts=0;
	UnitAny *portal=NULL;
	char *name;
	if (area==39) {//Secret cow level
		portal=getPortalToArea(area,pRU->szName);
		if (!portal) portal=getPortalToArea(pRU->dwLevelNo,NULL);
		name="";
	} else {
		portal=getPortalToArea(-1,pRU->szName);
		name=pRU->szName;
	}
	wchar_t wszbuf[256];int pos=0;
	if (portal) {
		LeftClickObject(portal);
		pos+=wsprintfW(wszbuf+pos, L"Auto Follow: enter portal to %d name %hs",pRU->dwLevelNo,name);
	} else
		pos+=wsprintfW(wszbuf+pos, L"Auto Follow: can't find portal to %d name %hs",pRU->dwLevelNo,name);
	D2ShowGameMessage(wszbuf, 0);
}
void follower_back_to_town(int uid) {
	dwMultiClientFollowId=0;enterRoomAttempts=0;
	if (!tMultiClient.isOn||!fInGame) return;
	UnitAny *portal=getPortalToArea(0,NULL);
	if (portal) {
		int x0=PLAYER->pMonPath->wPosX;
		int y0=PLAYER->pMonPath->wPosY;
		int dx=portal->pItemPath->dwPosX-x0;
		int dy=portal->pItemPath->dwPosY-y0;
		int abs_x=dx<0?-dx:dx;
		int abs_y=dy<0?-dy:dy;
		if (abs_x<5&&abs_y<5) {
			D2ShowGameMessage(L"Auto Follow: retreat to existing portal",   0 );
			LeftClickObject(portal);
			return;
		}
	}
	D2ShowGameMessage(L"Auto Follow: retreat back to town",   0 );
	BackToTown();
}
void follower_left_click_xy(int x,int y) {
	if (!canRemoteControl()) return;
	RunToPos(x,y);
}
void follower_right_click_xy(int x,int y) {
	if (!canRemoteControl()) return;
	RightSkill(NULL,x,y);
}
void follower_left_click_unit(int type,int id) {
	if (!canRemoteControl()) return;
	dwMultiClientClickTimeout=20;
	dwMultiClientClickRight=0;
	dwMultiClientClickUnitType=type;
	dwMultiClientClickUnitId=id;
	if (fScreenSaverActive) delayScreenSaver(80);
}
void follower_right_client_unit(int type,int id) {
	if (!canRemoteControl()) return;
	dwMultiClientClickTimeout=20;
	dwMultiClientClickRight=1;
	dwMultiClientClickUnitType=type;
	dwMultiClientClickUnitId=id;
	if (fScreenSaverActive) delayScreenSaver(80);
}
int MultiClientEnterDoor() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	D2Window d2wins[8];
	int dwMultiClientCount=getOtherClients(d2wins,8,-1);
	int err=0;
	int lParam=((LEVELNO&0xFF)<<24)|(dwPlayerId&0xFFFFFF);
	for (int i=0;i<dwMultiClientCount;i++) {
		if (!PostMessageA(d2wins[i].hwnd,WM_KEYUP,VK_MULTICLIENT_ENTER_DOOR,lParam)) err++;
	}
	if (err) LOG("MultiClient error: %d\n",err);
	return 1;
}
int leader_back_to_town() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	D2Window d2wins[8];
	int dwMultiClientCount=getOtherClients(d2wins,8,-1);
	int err=0;
	for (int i=0;i<dwMultiClientCount;i++) {
		if (!PostMessageA(d2wins[i].hwnd,WM_KEYUP,VK_MULTICLIENT_BACK_TO_TOWN,dwPlayerId)) err++;
	}
	if (err) LOG("MultiClient error: %d\n",err);
	return 1;
}
int MultiClientStartClick(int param) {fTransferClick=1;return 1;}
int MultiClientStopClick(int param) {fTransferClick=0;return 1;}
int MultiClientInitClick() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	if (!dwMultiClientCount) {
		dwMultiClientCount=getOtherClients(d2wins,8,dwCurrentLevel);
		if (!dwMultiClientCount) {
			D2ShowGameMessage(L"Transfer Click: No client found",   0 );
			return 0;
		}
		int pos=0;
		pos+=wsprintfW(wszTransferClick+pos, L"->");
		for (int i=0;i<dwMultiClientCount;i++) {
			if (pos>32) {wsprintfW(wszTransferClick+pos, L"...");break;}
			pos+=wsprintfW(wszTransferClick+pos, L" %hs",d2wins[i].name);
		}
		//D2ShowGameMessage(wszTransferClick, 0);
	}
	return 1;
}
void refreshFollowClient() {
	dwMultiClientCount=getOtherClients(d2wins,8,dwCurrentLevel);
}
int MultiClientStartFollow() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	refreshFollowClient();
	if (!dwMultiClientCount) {
		D2ShowGameMessage(L"Auto Follow: No client found",   0 );
		return 0;
	}
	wchar_t wszbuf[256];int pos=0;
	pos+=wsprintfW(wszbuf+pos, L"Auto Follow:");
	for (int i=0;i<dwMultiClientCount;i++) {
		pos+=wsprintfW(wszbuf+pos, L" %hs",d2wins[i].name);
	}
	D2ShowGameMessage(wszbuf, 0);
	int err=0;
	for (int i=0;i<dwMultiClientCount;i++) {
		if (!PostMessageA(d2wins[i].hwnd,WM_KEYUP,VK_AUTOFOLLOW_START,dwPlayerId)) err++;
	}
	if (err) LOG("MultiClient error: %d\n",err);
	return 1;
}
int MultiClientStopFollow() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	for (int i=0;i<dwMultiClientCount;i++) {
		PostMessageA(d2wins[i].hwnd,WM_KEYUP,VK_AUTOFOLLOW_STOP,dwPlayerId);
	}
	dwMultiClientCount=0;
	D2ShowGameMessage(L"Auto Follow disabled",   0 );
	return 1;
}
int MultiClientToggleFollow() {
	if (dwMultiClientCount) return MultiClientStopFollow();
	else return MultiClientStartFollow();
}
int IsKeyDown(int keyCode);
int transferClick(int right,UnitAny *pUnit,int x,int y) {
	if (!IsKeyDown(tMultiClientClick.key)&&!IsKeyDown(tMultiClientClick2.key)) {fTransferClick=0;return 0;}
	for (int runId=0;runId<2;runId++) {
		if (!dwMultiClientCount) {
			MultiClientInitClick();
			if (!dwMultiClientCount) {fTransferClick=0;return 0;}
		}
		int err=0;
		int w,l;
		if (pUnit) {
			w=right?VK_MULTICLIENT_RIGHT_UNIT:VK_MULTICLIENT_LEFT_UNIT;
			l=((pUnit->dwUnitType&0xF)<<28)|(pUnit->dwUnitId&0xFFFFFFF);
			//LOG("transfer click: %d %d\n",pUnit->dwUnitType,pUnit->dwUnitId);
		} else {
			w=right?VK_MULTICLIENT_RIGHT_XY:VK_MULTICLIENT_LEFT_XY;
			l=(x&0xFFFF)|((y&0xFFFF)<<16);
		}
		for (int i=0;i<dwMultiClientCount;i++) {
			int uid=d2wins[i].uid;
			RosterUnit *pRU=getRosterUnit(uid);
			if (!pRU) {err++;continue;}
			if (pRU->dwLevelNo!=LEVELNO) {
				//LOG("transfer click: area diff\n");
				continue;
			}
			UnitAny *pUnit=D2GetUnitFromId(uid , UNITNO_PLAYER) ;
			if (!pUnit) {
				//LOG("transfer click: player not around\n");
				continue;
			}
			//int dx=pUnit->pMonPath->wPosX-PLAYER->pMonPath->wPosX;
			//int dy=pUnit->pMonPath->wPosY-PLAYER->pMonPath->wPosY;
			if (!PostMessageA(d2wins[i].hwnd,WM_KEYUP,w,l)) err++;
		}
		if (err) {
			D2ShowGameMessage(L"MultiClient post message error",0);
			dwMultiClientCount=0;
			fTransferClick=0;
		} else break;
	}
	return 1;
}
int canTranferClick() {
	if (p_D2ScreenBlocked[0]&1) { //right half blocked
		if (p_D2MousePos->y>=SCREENSIZE.x/2) return 0; //x,y is inverted
	} else if (p_D2ScreenBlocked[0]&2) { //left half blocked
		if (p_D2MousePos->y<=SCREENSIZE.x/2) return 0;
	}
	return 1;
}
int __fastcall shouldLeftClick(int *args) {
	int src=args[1];
	UnitAny *pUnit=(UnitAny *)args[2];
	int x=args[3];
	int y=args[4];
	if (fTransferClick&&canTranferClick()) {
		transferClick(0,pUnit,x,y);
		if (fTransferClick) return 0;
	}
	if (pUnit&&tNecNoAttackInHell.isOn&&DIFFICULTY==2&&dwPlayerClass==2
		&&dwLeftSkill==0&&!fPlayerInTown&&pUnit->dwUnitType==UNITNO_MONSTER)
		args[2]=0;
	return 1;
}
int __fastcall shouldRightClick(int *args) {
	int src=args[1];
	UnitAny *pUnit=(UnitAny *)args[2];
	int x=args[3];
	int y=args[4];
	if (fTransferClick&&canTranferClick()) {
		transferClick(1,pUnit,x,y);
		if (fTransferClick) return 0;
	}
	return 1;
}
/*
LeftClick
6FACBDE0 - 83 EC 08              - sub esp,08 { 8 }
6FACBDE3 - 8B CE                 - mov ecx,esi
6FACBDE5 - E8 F6C7FFFF           - call 6FAC85E0
*/
void __declspec(naked) LeftClick_Patch_ASM() {
	__asm {
		pushad
		call shouldLeftClick
		test eax,eax
		je cancel_click
		popad
		call dword ptr [D2ShouldClick85E0]
		ret
cancel_click:
		popad
		xor eax,eax
		ret
	}
}
/*
RightClick
6FACBC40 - 8B CE                 - mov ecx,esi
6FACBC42 - E8 99C9FFFF           - call 6FAC85E0 return 0 to cancel
	buf[0]=left:0x45 right:0x46
	buf[1]=(int)PLAYER;
	buf[2]=object
	buf[3]=x;
	buf[4]=y;
*/
void __declspec(naked) RightClick_Patch_ASM() {
	__asm {
		pushad
		call shouldRightClick
		test eax,eax
		je cancel_click
		popad
		call dword ptr [D2ShouldClick85E0]
		ret
cancel_click:
		popad
		xor eax,eax
		ret
	}
}
