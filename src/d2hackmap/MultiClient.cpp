#include "stdafx.h"

void MultiClientSendPos();
static D2Window d2wins[8];
static int waitF=0,enterRoomAttempts,fClientType;

void MultiClientNewGame() {
	dwMultiClientCount=0;dwMultiClientFollowId=0;waitF=0;enterRoomAttempts=0;
	dwMultiClientClickUnitId=0;fTransferClick=0;wszTransferClick[0]=0;fClientType=0;
}
int getRoomtileTxtToLevel(int toLevel) {
	//only appear if distance < 20
	for(RoomTile *pRoomtile = PLAYER->pMonPath->pRoom1->pRoom2->pRoomTiles ;pRoomtile ; pRoomtile = pRoomtile->pNext ){
		int txt=*(pRoomtile->pNum);
		int lvl=pRoomtile->pRoom2->pDrlgLevel->dwLevelNo;
		if (lvl==toLevel) return txt;
	}
	return 0;
}
UnitAny *getRoomtileByTxt(int txt) {
	for (int i=0;i<128;i++) {
		UnitAny *pUnit=p_D2UnitTable[5*128+i];
		while (pUnit) {
			if (pUnit->dwTxtFileNo==txt) return pUnit;
			pUnit=pUnit->pHashNext;
		}
	}
	return NULL;
}
//level 0:town -1:any
UnitAny *getPortalToArea(int level,char *owner) {
	for (int i=0;i<128;i++) {
		UnitAny *pUnit=p_D2UnitTable[2*128+i];
		while (pUnit) {
			//blue portal 59 red portal 60
			if ((pUnit->dwTxtFileNo==0x3b||pUnit->dwTxtFileNo==0x3c)&&pUnit->pObjectData) {
				int lvl=pUnit->pObjectData->nShrineNo;
				if (level!=0&&level!=-1&&lvl!=level) goto next;
				if (level==0&&lvl!=1&&lvl!=40&&lvl!=75&&lvl!=103&&lvl!=109) goto next;
				char *name=(char *)pUnit->pObjectData;name+=0x28;
				if (!owner||strcmp(owner,name)==0) return pUnit;
			}
next:
			pUnit=pUnit->pHashNext;
		}
	}
	return NULL;
}
void MultiClientLoop() {
	if (dwMultiClientClickTimeout) {
		dwMultiClientClickTimeout--;
		if (!dwMultiClientClickTimeout) dwMultiClientClickUnitId=0;
	}
	if (!dwMultiClientFollowId) return;
	if (fWinActive) {
		if (dwMultiClientFollowId&&fPlayerInTown) dwMultiClientFollowId=0;
		return;
	}
	if (waitF) {waitF--;return;}
	int targetX,targetY;
	RosterUnit *pRU=getRosterUnit(dwMultiClientFollowId);
	if (!pRU) {dwMultiClientFollowId=0;return;}
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
	if (pRU->dwLevelNo!=LEVELNO) {
		wchar_t wszbuf[256];
		if (enterRoomAttempts>10) {
			dwMultiClientFollowId=0;
			wsprintfW(wszbuf, L"Auto Follow: can't follow to level %d",pRU->dwLevelNo);
			D2ShowGameMessage(wszbuf, 0);
			return;
		}
		int txt=getRoomtileTxtToLevel(pRU->dwLevelNo);
		if (txt) {
			UnitAny *room=getRoomtileByTxt(txt);
			if (room) {
				LeftClickObject(room);
				enterRoomAttempts++;
				//wsprintfW(wszbuf, L"Auto Follow: enter level %d",pRU->dwLevelNo);
				//D2ShowGameMessage(wszbuf, 0);
				waitF=12;
				return;
			}
		}
		UnitAny *portal=getPortalToArea(pRU->dwLevelNo,NULL);
		if (portal) {
			LeftClickObject(portal);
			enterRoomAttempts++;
			//wsprintfW(wszbuf, L"Auto Follow: enter portal to level %d",pRU->dwLevelNo);
			//D2ShowGameMessage(wszbuf, 0);
			waitF=12;
			return;
		}
		if (!pUnit) {
			wsprintfW(wszbuf, L"Auto Follow: can't follow to level %d",pRU->dwLevelNo);
			D2ShowGameMessage(wszbuf, 0);
			dwMultiClientFollowId=0;
			return;
		}
	}
	if (dis<dwMultiClientDistance) return;
	if (dis>dwMultiClientMaxDistance) return;
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
	waitF=12;enterRoomAttempts=0;
}
void startFollowId(int uid) {
	dwMultiClientFollowId=uid;waitF=0;enterRoomAttempts=0;
	wchar_t wszbuf[64];
	wsprintfW(wszbuf, L"Start follow %d\n",dwMultiClientFollowId);
	D2ShowGameMessage(wszbuf, 0);
}
void stopFollow() {
	dwMultiClientFollowId=0;enterRoomAttempts=0;
	wchar_t wszbuf[64];
	wsprintfW(wszbuf, L"Stop follow\n");
	D2ShowGameMessage(wszbuf, 0);
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
void enterDoor(int uid,int area) {
	dwMultiClientFollowId=0;enterRoomAttempts=0;
	if (!tMultiClient.isOn||!fInGame) return;
	if (!canRemoteControl()) return;
	RosterUnit *pRU=getRosterUnit(uid);
	if (!pRU) return;
	enterRoomAttempts=0;
	UnitAny *portal=NULL;
	char *name;
	if (area==39) {//Secret cow level
		portal=getPortalToArea(pRU->dwLevelNo,NULL);
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
void retreat(int uid) {
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
void multiclient_left_xy(int x,int y) {
	if (!canRemoteControl()) return;
	RunToPos(x,y);
}
void multiclient_right_xy(int x,int y) {
	if (!canRemoteControl()) return;
	RightSkill(NULL,x,y);
}
void multiclient_left_unit(int type,int id) {
	if (!canRemoteControl()) return;
	dwMultiClientClickTimeout=20;
	dwMultiClientClickRight=0;
	dwMultiClientClickUnitType=type;
	dwMultiClientClickUnitId=id;
	delayScreenSaver(80);
}
void multiclient_right_unit(int type,int id) {
	if (!canRemoteControl()) return;
	dwMultiClientClickTimeout=20;
	dwMultiClientClickRight=1;
	dwMultiClientClickUnitType=type;
	dwMultiClientClickUnitId=id;
	delayScreenSaver(80);
}
int MultiClientEnterDoor() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	dwMultiClientCount=getOtherClients(d2wins,8,-1);fClientType=1;
	int err=0;
	int lParam=((LEVELNO&0xFF)<<24)|(dwPlayerId&0xFFFFFF);
	for (int i=0;i<dwMultiClientCount;i++) {
		if (!PostMessageA(d2wins[i].hwnd,WM_KEYUP,VK_AUTOFOLLOW_ENTERDOOR,lParam)) err++;
	}
	if (err) LOG("MultiClient error: %d\n",err);
	return 1;
}
int MultiClientRetreat() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	dwMultiClientCount=getOtherClients(d2wins,8,-1);fClientType=1;
	int err=0;
	for (int i=0;i<dwMultiClientCount;i++) {
		if (!PostMessageA(d2wins[i].hwnd,WM_KEYUP,VK_AUTOFOLLOW_RETREAT,dwPlayerId)) err++;
	}
	if (err) LOG("MultiClient error: %d\n",err);
	return 1;
}
int MultiClientStartClick(int param) {fTransferClick=1;return 1;}
int MultiClientStopClick(int param) {fTransferClick=0;return 1;}
int MultiClientInitClick() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	if (!dwMultiClientCount||fClientType!=2) {
		dwMultiClientCount=getOtherClients(d2wins,8,dwCurrentLevel);fClientType=2;
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
int MultiClientStartFollow() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	dwMultiClientCount=getOtherClients(d2wins,8,dwCurrentLevel);fClientType=3;
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
int IsKeyDown(int keyCode);
int transferClick(int right,UnitAny *pUnit,int x,int y) {
	if (!IsKeyDown(tMultiClientClick.key)) {fTransferClick=0;return 0;}
	for (int runId=0;runId<2;runId++) {
		if (!dwMultiClientCount||fClientType!=2) {
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
