#include "stdafx.h"
#include "header.h"
#include "multi.h"
#include "auto.h"

int autoNpcTxt=0;
extern int npcChatTxt;
int dwMultiClientMaxWindowId=0;
int d2winLastId=0;
wchar_t wszTransferClick[64]={0};
int fAutoFollowMoving=0;
void send_multi_reply(int info);
int acceptTradeFromGid=0,acceptTradeFromUid=0,acceptTradeMs=0,clickAcceptTradeMs;
extern int autoTerminateNpcInteractMs,hasRune789;
static char tradingName[20]={0};
static int tradingId=0;
int hydraLock=0,hydraUid=0,hydraX,hydraY;
extern ToggleVar t3BBProtect;

int QuickNextGame(int addnum);
void joinGame(char *name,char *pass);
extern int dwEnterDoorInBackgroundSkill[7][200];
int takeWaypointToAreaUI(int level);
extern int dwTakeWaypointToLevel;
int BackToTown();
int MultiClientTalkToNpc();
extern int fSkipPainting;
extern int dwNecInfoExpireMs;
extern int dwInteractEntityCount;
extern int dwAutoSkillReloading;
extern int dwEnchantSkeletonCount;
ToggleVar 	tMultiClient={				TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Follow" };
ToggleVar 	tMultiClientToggleFollow={	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientToggleFollow",&MultiClientToggleFollow};
ToggleVar 	tMultiClientStartFollow1={	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientStartFollow1",&MultiClientStartFollow1};
ToggleVar 	tMultiClientStartFollow2={	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientStartFollow2",&MultiClientStartFollow2};
ToggleVar 	tMultiClientStopFollow={	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientStopFollow",&MultiClientStopFollow};
ToggleVar 	tMultiClientEnterDoor={	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientEnterDoor",&MultiClientEnterDoor};
ToggleVar 	tMultiClientRetreat={	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientRetreat",&leader_back_to_town};
ToggleVar 	tMultiClientClick={	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientClick",&MultiClientStartClick};
ToggleVar 	tMultiClientClick2={	TOGGLEVAR_DOWN,	0,	-1,	1,  "MultiClientClick2",&MultiClientStartClick};
ToggleVar tMultiClientTalkToNpc={TOGGLEVAR_DOWN,0,-1,1,"MultiClientTalkToNpc",&MultiClientTalkToNpc};
ToggleVar tMCFormationKeys[32]={			 0};
signed char aMCFormation[32][16]={					0};
int 			dwMultiClientGroupSize1=					8;
int 			dwMultiClientGroupSize2=					8;
int 			fTransferClick=0;
int 			dwAutoFollowDistance=					4;
int 			dwAutoFollowMaxDistance=					33;
int 			dwAutoFollowMoveDistance=					10;
int 			dwAutoFollowCheckInterval=					200;
int 			dwAutoFollowClickObjectDelayMs=					300;
int 			dwWaitTeamEnterPortalMaxDelayMs=					300;
char 	    aAutoFollowClickObject[1024]	={0};
int 			dwTeamMemberCount=					0;
int 			dwLeaderId=					0;
ToggleVar 	tNecNoAttackInHell={				TOGGLEVAR_ONOFF,	0,	-1,	1,  "NecNoAttackInHell"};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY, "MultiClientToggle",          &tMultiClient         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientToggleFollowKey",          &tMultiClientToggleFollow         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientStartFollowKey1",          &tMultiClientStartFollow1         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientStartFollowKey2",          &tMultiClientStartFollow2         },
	{CONFIG_VAR_TYPE_KEY,"MultiClientTalkToNpcKey",&tMultiClientTalkToNpc},
	{CONFIG_VAR_TYPE_KEY, "MultiClientStopFollowKey",          &tMultiClientStopFollow         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientEnterDoorKey",          &tMultiClientEnterDoor         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientRetreatKey",          &tMultiClientRetreat         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientTransferClickKey",          &tMultiClientClick         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientTransferClickKey2",          &tMultiClientClick2         },
	{CONFIG_VAR_TYPE_INT, "MultiClientMaxWindowId",	   &dwMultiClientMaxWindowId,4},
	{CONFIG_VAR_TYPE_INT, "MultiClientGroupSize1",	   &dwMultiClientGroupSize1,4},
	{CONFIG_VAR_TYPE_INT, "MultiClientGroupSize2",	   &dwMultiClientGroupSize2,4},
	{CONFIG_VAR_TYPE_INT, "AutoFollowDistance",	   &dwAutoFollowDistance,4},
	{CONFIG_VAR_TYPE_INT, "AutoFollowMaxDistance",	   &dwAutoFollowMaxDistance,4},
	{CONFIG_VAR_TYPE_INT, "AutoFollowMoveDistance",	   &dwAutoFollowMoveDistance,4},
	{CONFIG_VAR_TYPE_INT, "AutoFollowCheckInterval",	   &dwAutoFollowCheckInterval,4},
	{CONFIG_VAR_TYPE_INT, "AutoFollowClickObjectDelayMs",	   &dwAutoFollowClickObjectDelayMs,4},
	{CONFIG_VAR_TYPE_INT, "WaitTeamEnterPortalMaxDelayMs",	   &dwWaitTeamEnterPortalMaxDelayMs,4},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "AutoFollowClickObject", &aAutoFollowClickObject,       1, {1024}},
	{CONFIG_VAR_TYPE_KEY, "NecNoAttackInHellToggle",          &tNecNoAttackInHell         },
};
void multiclient_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void multiclient_initConfigVars() {
	aAutoFollowClickObject[59]=1; //blue portal
	aAutoFollowClickObject[60]=1; //red portal
	aAutoFollowClickObject[74]=1; //Lut Gholein sewer door
	aAutoFollowClickObject[192]=1; //Arcane Sanctuary teleport pad
	aAutoFollowClickObject[298]=1; //Palace Cellar Level 3<->Arcane Sanctuary portal
}
void multiclient_FixValues() {
	tMultiClientClick.type=TOGGLEVAR_DOWNUPPARAM;
	tMultiClientClick.funcUp=MultiClientStopClick;
	tMultiClientClick2.type=TOGGLEVAR_DOWNUPPARAM;
	tMultiClientClick2.funcUp=MultiClientStopClick;
}

void refresh_team_info();
void multiclient_winactive();
static int refreshClientMs=0;
void refresh_clients();
D2Window d2wins[32]={0};
void MultiClientSendPos();
static int dwLeaderGameId;
static int waitToMs=0;
static int infoMs=0;
static int clickState=0,followerInteractType,followerInteractId,followerInteractTimeout,fSendInfoErr,followerWaypointToArea,followerClickCount;
static HWND activeHwnd;
static int transferClickType,transferClickTimeout; //1:leftXY, 2:rightXY, 3:leftUnit, 4:rightUnit
static int transferClickX,transferClickY,transferClickUnitType,transferClickUnitId;
int dwWaitTeamEnterPortalMs,dwWaitTeamEnterPortalId;
enum CLICK_STATE {
	CS_FOLLOW=0,
	CS_CLICK_OBJECT,
	CS_CLICKED_OBJECT,
	CS_WAIT_WAYPOINT_MENU,
	CS_CLICK_WAYPOINT_MENU,
};

void MultiClientNewGame() {
	if (dwMultiClientMaxWindowId>=_ARRAYSIZE(d2wins)) dwMultiClientMaxWindowId=_ARRAYSIZE(d2wins)-1;
	dwTeamMemberCount=0;dwLeaderGameId=0;dwLeaderId=0;
	waitToMs=0;clickState=0;activeHwnd=0;fSendInfoErr=0;
	fTransferClick=0;wszTransferClick[0]=0;transferClickType=0;
	followerInteractType=0;followerInteractId=0;fAutoFollowMoving=0;followerClickCount=0;d2winLastId=0;
	autoNpcTxt=0;acceptTradeFromUid=0;acceptTradeMs=0;tradingId=0;clickAcceptTradeMs=0;hasRune789=0;
	saveRuntimeInfo(d2gfx_GetHwnd());
	refresh_clients();
}
void MultiClientEndGame() {
	saveRuntimeInfo(d2gfx_GetHwnd());
}
int makeRuntimeInfoPath(char *path,int id) {
	char dir[512];
	if (!szRuntimePath[0]) sprintf(dir,"%sruntime",szPluginPath);
	else if (szRuntimePath[0]&&szRuntimePath[1]==':') sprintf(dir,"%s",szRuntimePath);
	else sprintf(dir,"%s%s",szPluginPath,szRuntimePath);
	CreateDirectoryA(dir,NULL);
	sprintf(path,"%s\\d2_%d.txt",dir,id);
	return 0;
}
int saveRuntimeInfo1(HWND hwnd,int uid) {
	char path[512];
	makeRuntimeInfoPath(path,dwGameWindowId);
	FILE *fp=fopen(path,"w+");if (!fp) return -1;
	fprintf(fp,"hwnd: 0x%x\n",hwnd);
	GameStructInfo *pinfo=*d2client_pGameInfo;
	char *game="",*name="";
	if (pinfo) {
		char *realm=pinfo->szRealmName;
		char *ip=pinfo->szGameServerIp;
		game=pinfo->szGameName;
		char *password=pinfo->szGamePassword;
		name=pinfo->szCharName;
		if (realm[0]) fprintf(fp,"realm: %s\n",realm);
		if (ip[0]) fprintf(fp,"ip: %s\n",ip);
		if (game[0]) fprintf(fp,"game: %s\n",game);
		if (password[0]) fprintf(fp,"password: %s\n",password);
		if (name[0]) fprintf(fp,"name: %s\n",name);
	}
	if (PLAYER) {
		uid=PLAYER->dwUnitId;
		fprintf(fp,"uid: %d\n",uid);
	}
	LOG("save runtime info game=%s name=%s uid=%d\n",game,name,uid);
	fprintf(fp,"Command Line: %s\n",GetCommandLine());
	fclose(fp);
	return 0;
}
int saveRuntimeInfo(HWND hwnd) {
	return saveRuntimeInfo1(hwnd,-1);
}
int loadRuntimeInfo(D2Window *pwin,int id) {
	char buf[256],path[512];
	makeRuntimeInfoPath(path,id);
	if (!fileExist(path)) return 0;
	memset(pwin,0,sizeof(D2Window));
	pwin->index=id;
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
		else if (strcmp(name,"password")==0) strcpy(pwin->password,value);
		else if (strcmp(name,"name")==0) strcpy(pwin->name,value);
		else if (strcmp(name,"hwnd")==0) pwin->hwnd=(HWND)strtol(value,0,16);
		else if (strcmp(name,"uid")==0) pwin->uid=strtol(value,0,10);
	}
	fclose(fp);
	int len=GetClassName(pwin->hwnd,buf,256);
	if (len>0&&strcmp(buf,"Diablo II")==0) return 1;
	memset(pwin,0,sizeof(D2Window)-8);
	return 0;
}
D2Window *getLocalPlayer(int uid) {
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (!pwin->hwnd||!pwin->sameGame) continue;
		if (pwin->uid==uid) return pwin;
	}
	return NULL;
}
int isLocalPlayerName(char *name) {
	if (!d2winLastId) refresh_clients();
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		//LOG("isLocalPlayer %d %X %d %d\n",i,pwin->hwnd,pwin->sameGame,pwin->uid);
		if (!pwin->hwnd||!pwin->sameGame) continue;
		if (strcmp(pwin->name,name)==0) return 1;
	}
	return 0;
}
int isLocalPlayer(int uid) {
	if (!d2winLastId) refresh_clients();
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		//LOG("isLocalPlayer %d %X %d %d\n",i,pwin->hwnd,pwin->sameGame,pwin->uid);
		if (!pwin->hwnd||!pwin->sameGame) continue;
		if (pwin->uid==uid) return 1;
	}
	return 0;
}
void refresh_clients() {
	GameStructInfo *info=*d2client_pGameInfo;
	if (IsBadReadPtr(info,sizeof(GameStructInfo))) info=NULL;
	int last=d2winLastId;
	for (int i=1;i<=dwMultiClientMaxWindowId;i++) {
		if (i==dwGameWindowId) {d2winLastId=i;continue;}
		D2Window *pwin=&d2wins[i];
		if (!loadRuntimeInfo(pwin,i)) continue;
		if (info) {
			pwin->sameGame=strcmp(pwin->ip,info->szGameServerIp)==0
				&&(!pwin->game[0]&&!info->szGameName[0]||strcmp(pwin->game,info->szGameName)==0);
		}
		d2winLastId=i;
	}
	refreshClientMs=dwCurMs;
	if (fWinActive&&d2winLastId) multiclient_winactive();
}
void setGameNamePassword(char *name,char *pass);
void multiclient_load_config() {
	refresh_clients();
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd) continue;
		if (pwin->game[0]) {setGameNamePassword(pwin->game,pwin->password);break;}
	}
}
void multiclient_winactive() {
	if (!d2winLastId) refresh_clients();
	int hwnd=(int)d2gfx_GetHwnd();
	int infoL=(MCI_WinActiveL<<24)|((hwnd&0xFFFF)<<8)|(dwGameWindowId<<24)|(dwPlayerId&0xFF);
	int infoH=(MCI_WinActiveH<<24)|(((hwnd>>16)&0xFFFF)<<8)|(dwGameWindowId<<24)|((dwPlayerId>>8)&0xFF);
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
		int err=0;
		if (!PostMessageA(pwin->hwnd,WM_KEYUP,VK_MULTICLIENT_INFO,infoL)) err++;
		if (!PostMessageA(pwin->hwnd,WM_KEYUP,VK_MULTICLIENT_INFO,infoH)) err++;
		if (err) {
			//LOG("ERROR: send active info to %d\n",i);
			pwin->hwnd=0;
		}
	}
}
HWND getGameWindowHwnd(int id) {
	if (id<1||id>31) return 0;
	if (id>d2winLastId) refresh_clients();
	return d2wins[id].hwnd;
}
extern int dwScreenBlockMask;
extern int need_paint;
void pressESC() {
	need_paint=2;
	//LOG("press ESC\n");
	int esc=getGameControlKey(56);HWND hwnd=d2gfx_GetHwnd();
	PostMessage(hwnd, WM_KEYDOWN, esc, 0);PostMessage(hwnd, WM_KEYUP, esc, 0);
}
int canRemoteControl() {
	if (fWinActive) return 0;
	if (PLAYER->dwMode==PlayerMode_Death||PLAYER->dwMode==PlayerMode_Dead) return 0;
	if (*d2client_pUiGameMenuOn) {
		pressESC();
		return 0;
	}
	if (npcChatTxt||*d2client_pUiInteractOn) {
		pressESC();
		return 0;
	} else if (*d2client_pUiInventoryOn
		||*d2client_pUiCubeOn
		||*d2client_pUiStashOn
		||*d2client_pUiNpcTradeOn
		||*d2client_pUiWaypointOn) {
		GameControl(56);
	}
	return (d2client_pScreenBlocked[0]&3)!=3 ;
}
static int teamMemberInArea(int area) {
	int n=0;
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
		if (!pwin->isTeam) continue;
		RosterUnit *pRU=getRosterUnit(pwin->uid);if (!pRU) continue;
		if (pRU->dwLevelNo==area) n++;
	}
	return n;
}
void leader_click_object(int type,int id) {
	int lParam=((type&0xF)<<28)|(id&0xFFFFFFF);
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
		if (!pwin->isTeam) continue;
		if (!d2client_GetUnitFromId(pwin->uid, UNITNO_PLAYER)) continue; //follower is not around
		if (!PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CLICK_OBJECT,lParam)) pwin->hwnd=NULL;
	}
}
int loadRuntimeInfo(D2Window *pwin,int id);
void send_multi_skill_info(int info) {
	int gid24=dwGameWindowId<<24;
	multiclient_send_info((MCI_Skill<<24)|(info&0xFFFFFF)|gid24);
}
void recv_multi_skill_info(D2Window *pwin,int info) {
	wchar_t wbuf[32];
	int value=(info>>8)&0xFFFF;
	switch (info&0xFF) {
		case MCS_HydraLock:
			wsprintfW(wbuf,L"Hydra %d",value);setBottomAlertMsg(3,wbuf,1000,1,0,6);
			break;
	}
}
void send_multi_quest_info(int info) {
	int gid24=dwGameWindowId<<24;
	multiclient_send_info((MCI_Quest<<24)|(info&0xFFFFFF)|gid24);
}
void incMapTargetIf(int cur);
extern int dwBarbrianLeft;
void recv_multi_quest_info(D2Window *pwin,int info) {
	//gameMessage("recv quest info 0x%X",info);
	switch (info&0xFF) {
	case MCQ_10BB:
		dwBarbrianLeft=10;
		if (dwCurrentLevel==Level_FrigidHighlands) {
			setBottomAlertMsg(0,L"搞定1/3",3000,0,2,0);
			incMapTargetIf(0);
		}
		break;
	case MCQ_5BB:
		dwBarbrianLeft=5;
		if (dwCurrentLevel==Level_FrigidHighlands) {
			setBottomAlertMsg(0,L"搞定2/3",3000,0,2,0);
			incMapTargetIf(1);
		}
		break;
	case MCQ_0BB:
		dwBarbrianLeft=0;
		if (dwCurrentLevel==Level_FrigidHighlands) {
			setBottomAlertMsg(0,L"全部搞定",3000,0,2,0);
		}
		break;
	case MCQ_BB_ERR:
		if (dwCurrentLevel==Level_FrigidHighlands) {
			setBottomAlertMsg(0,L"搞砸了",3000,1,1,0);
		}
		break;
	}
}
void send_multi_reply(int info) {
	int gid24=dwGameWindowId<<24;
	multiclient_send_info((MCI_Reply<<24)|(info&0xFFFFFF)|gid24);
}
void recv_multi_quest_reply(D2Window *pwin,int info) {
	switch (info) {
		case MCR_FAILED:setBottomAlertMsg(0,L"搞砸了",3000,1,1,0);break;
		case MCR_PASSED:setBottomAlertMsg(0,L"搞定",3000,0,2,0);break;
	}
	pwin->reply=info;pwin->replyMs=dwCurMs+18000;
}
void multiclient_recv_info(int info) {
	int type=(info>>24)&0xF0;
	int gid=(info>>24)&0x0F;
	if (gid<1||gid>dwMultiClientMaxWindowId) return;
	D2Window *pwin=&d2wins[gid];
	switch (type) {
		case MCI_CpuUsage:pwin->cpu=(info>>8)&0xFF;pwin->cpuMs=dwCurMs+2000;break;
		case MCI_WinActiveL:case MCI_WinActiveH: {//active window
			int hwnd16=(info>>8)&0xFFFF;
			int uid8=info&0xFF;
			if (!pwin->hwnd
				||!pwin->sameGame
				||gid>d2winLastId
				||type==MCI_WinActiveL&&((int)pwin->hwnd&0xFFFF)!=hwnd16
				||type==MCI_WinActiveL&&((int)pwin->uid&0xFF)!=uid8
				||type==MCI_WinActiveH&&(((int)pwin->hwnd>>16)&0xFFFF)!=hwnd16
				||type==MCI_WinActiveH&&(((int)pwin->uid>>8)&0xFF)!=uid8
				) refresh_clients();
			activeHwnd=pwin->hwnd;infoMs=0;
			break;
		}
		case MCI_HMRPotion:pwin->hPotion=(info>>16)&0xFF;pwin->mPotion=(info>>8)&0xFF;
			pwin->rPotion=info&0xFF;break;
		case MCI_Quantity:pwin->quantity=(info>>8)&0xFFFF;break;
		case MCI_Mana:pwin->manaMax=(info>>12)&0xFFF;pwin->mana=(info)&0xFFF;pwin->infoMs=dwCurMs+1500;break;
		case MCI_Hp:pwin->hpMax=(info>>12)&0xFFF;pwin->hp=(info)&0xFFF;pwin->infoMs=dwCurMs+1500;break;
		case MCI_Minions:pwin->minionsHp=(info>>16)&0xFF;pwin->minions=(info>>8)&0xFF;
			pwin->enMinions=info&0xFF;break;
		case MCI_StartFollow:pwin->isTeam=1;break;break;
		case MCI_StopFollow:pwin->isTeam=0;break;break;
		case MCI_AutoSkill:pwin->autoSkillId=info&0x7FFFFF;pwin->autoLeft=info&0x800000;break;
		case MCI_Quest:recv_multi_quest_info(pwin,info&0xFFFFFF);break;
		case MCI_Skill:recv_multi_skill_info(pwin,info&0xFFFFFF);break;
		case MCI_Reply:recv_multi_quest_reply(pwin,info&0xFFFFFF);break;
	}
}
void multiclient_send_info(int info) {
	if (fWinActive||!activeHwnd) return;
	if (!PostMessageA(activeHwnd,WM_KEYUP,VK_MULTICLIENT_INFO,info)) {
		activeHwnd=0;
		//gameMessage("send to active window error");
	}
}
void follower_click_object(int type,int id) {
	if (type==1) { //healing
		transferClickType=3;transferClickUnitType=type;
		transferClickUnitId=id;transferClickTimeout=dwCurMs+1000;
		return;
	}
	if (type==9) {followerWaypointToArea=id;return;}//waypoint target level
	else if (type==8) type=0x102; //take waypoint
	else followerWaypointToArea=0;
	if (tPacketHandler.isOn) LOG("follower recv click object %d:%d\n",type,id);
	followerInteractType=type;followerInteractId=id;
	clickState=type==0?0:CS_CLICK_OBJECT;followerClickCount=0;
	if (type&100) { //waypoint
		waitToMs=dwCurMs+dwAutoFollowCheckInterval;
	} else if (type==5) {
		waitToMs=dwCurMs+dwAutoFollowClickObjectDelayMs;
	} else {
		UnitAny *pUnit=d2client_GetUnitFromId(followerInteractId,followerInteractType&0xFF);
		if (pUnit&&pUnit->dwTxtFileNo==59) waitToMs=dwCurMs+dwAutoFollowCheckInterval; //blue portal
		else waitToMs=dwCurMs+dwAutoFollowClickObjectDelayMs;
	}
}
static void interactObject() {
	static int dwInteractEntityCountVerify=0;
	static int clickDis2,clickArea;
	switch (clickState) {
		case CS_CLICK_OBJECT: {
			UnitAny *pUnit=d2client_GetUnitFromId(followerInteractId,followerInteractType&0xFF);
			if (!pUnit) {clickState=0;gameMessage("can't find unit%d:%d",followerInteractType,followerInteractId);return;}
			if (followerInteractType&0x100 //way point
				||followerInteractType==5
				||aAutoFollowClickObject[pUnit->dwTxtFileNo]) {
			} else {clickState=0;return;}
			clickDis2=(int)getUnitYardDistance2(PLAYER,pUnit);
			if (clickDis2>dwAutoFollowMaxDistance*dwAutoFollowMaxDistance) {
				clickState=0;gameMessage("unit%d:%d too far away",followerInteractType,followerInteractId);
				return;
			}
			if (followerInteractType&0x100) {//way point
				dwTakeWaypointToLevel=followerWaypointToArea;followerWaypointToArea=0;
			}
			clickArea=LEVELNO;followerInteractTimeout=dwCurMs+3000;
			fAutoFollowMoving=1;dwInteractEntityCountVerify=dwInteractEntityCount;LeftClickUnit(pUnit);
			clickState=CS_CLICKED_OBJECT;followerClickCount++;
			if (tPacketHandler.isOn) LOG("follower click object %d:%d\n",followerInteractType,followerInteractId);
			break;
		}
		case CS_CLICKED_OBJECT: {
			int dis2=1000; 
			if (clickArea!=LEVELNO||followerWaypointToArea&&d2client_CheckUiStatus(UIVAR_WAYPOINT)) {
				clickState=0;//followerWaypointToArea?CS_WAIT_WAYPOINT_MENU:0;
				break;
			}
			UnitAny *pUnit=d2client_GetUnitFromId(followerInteractId,followerInteractType&0xFF);
			if (pUnit) {
				dis2=(int)getUnitYardDistance2(PLAYER,pUnit);
				if (dis2<clickDis2) {
					clickDis2=dis2;followerInteractTimeout=dwCurMs+3000;
					if (tPacketHandler.isOn) LOG("follower to %d:%d dis2=%d\n",followerInteractType,followerInteractId,dis2);
					return;//getting closer
				} 
			}
			if (dis2<36||dwInteractEntityCountVerify==dwInteractEntityCount) { //not clicked yet
				if (dwCurMs>followerInteractTimeout) {
					clickState=0;
					gameMessage("click type=%d id=%d clicked=%d timeout",
						followerInteractType,followerInteractId,followerClickCount);
				} else clickState=CS_CLICK_OBJECT; //click again
			} else {
				clickState=0;//followerWaypointToArea?CS_WAIT_WAYPOINT_MENU:0;
				if (tPacketHandler.isOn) LOG("follower clicked object %d:%d\n",followerInteractType,followerInteractId);
			}
			break;
		}
		default:clickState=0;break;
	}
}
void send_multi_skill_info(int info);
void autoHydra() {
	if (!hydraLock) return;
	if (dwRightSkill!=Skill_Hydra) {hydraLock=0;return;}
	if (PLAYER->dwMode==PlayerMode_Attacking1||PLAYER->dwMode==PlayerMode_Attacking2||PLAYER->dwMode==PlayerMode_Cast) return;
	if (!fCanUseRightSkill) return;
	if (!fWinActive) send_multi_skill_info((hydraLock<<8)|MCS_HydraLock);
	wchar_t wbuf[32];
	if (hydraUid) {
		UnitAny *pUnit=d2client_GetUnitFromId(hydraUid,UNITNO_MONSTER);
		if (!pUnit||pUnit->dwMode==MonsterMode_Death||pUnit->dwMode==MonsterMode_Dead) {hydraLock=0;return;}
		RightSkillUnit(pUnit);
		wsprintfW(wbuf,L"Hydra %d:%d",hydraLock++,hydraUid);
	} else {
		RightSkillPos(hydraX,hydraY);
		wsprintfW(wbuf,L"Hydra %d:(%d,%d)",hydraLock++,hydraX,hydraY);
	}
	setBottomAlertMsg(3,wbuf,1000,0,0,6);
}
void MultiClientLoop() {
	if (hydraLock) autoHydra();
	if (!fWinActive&&dwCurMs>infoMs) {
		int n=0;
		if (fUsingBow) n=dwArrowCount+d2common_GetUnitStat(PLAYER, 70, 0);
		else if (fUsingCrossBow) n=dwCArrowCount+d2common_GetUnitStat(PLAYER, 70, 0);
		else if (fUsingThrow) n=dwThrowCount+d2common_GetUnitStat(PLAYER, 70, 0)-dwAutoThrowMinQuantity;
		int gid24=dwGameWindowId<<24;
		multiclient_send_info((MCI_HMRPotion<<24)|(dwHPotionCount<<16)|(dwMPotionCount<<8)|dwRPotionCount|gid24);
		multiclient_send_info((MCI_Quantity<<24)|(n<<8)|gid24);
		if (dwPlayerMaxMana<4096) 
			multiclient_send_info((MCI_Mana<<24)|(dwPlayerMaxMana<<12)|(dwPlayerMana)|gid24);
		else
			multiclient_send_info((MCI_Mana<<24)|((dwPlayerMaxMana>>4)<<12)|(dwPlayerMana>>4)|gid24);
		if (dwPlayerMaxHP<4096)
			multiclient_send_info((MCI_Hp<<24)|(dwPlayerMaxHP<<12)|(dwPlayerHP)|gid24);
		else
			multiclient_send_info((MCI_Hp<<24)|((dwPlayerMaxHP>>4)<<12)|(dwPlayerHP>>4)|gid24);
		int minions=dwCurMs>dwNecInfoExpireMs?0:dwSkeletonCount+dwSkeletonMageCount+dwReviveCount;
		multiclient_send_info((MCI_Minions<<24)|(dwSkeletonHPPercent<<16)|(minions<<8)|dwEnchantSkeletonCount|gid24);
		infoMs=dwCurMs+1000;
	}
	if (dwAutoSkillReloading) return;
	if (transferClickType) {
		UnitAny *pUnit;
		if (dwCurMs>transferClickTimeout) {gameMessage("transfer click timeout");transferClickType=0;return;}
		if (PLAYER->dwMode==PlayerMode_Attacking1||PLAYER->dwMode==PlayerMode_Attacking2||PLAYER->dwMode==PlayerMode_Cast) return;
		switch (transferClickType) {
			case 1:LeftClickPos(transferClickX,transferClickY);break;
			case 2:RightSkillPos(transferClickX,transferClickY);break;
			case 3:pUnit=d2client_GetUnitFromId(transferClickUnitId,transferClickUnitType);
				if (pUnit) LeftClickUnit(pUnit);break;
			case 4:pUnit=d2client_GetUnitFromId(transferClickUnitId,transferClickUnitType);
				if (pUnit) RightSkillUnit(pUnit);
		}
		if (fAutoSkill) dwAutoSkillCheckMs=dwCurMs+500;
		transferClickType=0;
	}
	if (dwWaitTeamEnterPortalMs) {
		if (!fPlayerInTown||dwCurMs>dwWaitTeamEnterPortalMs) {dwWaitTeamEnterPortalMs=0;return;}
		if (teamMemberInArea(LEVELNO)) {waitToMs=dwCurMs+dwAutoFollowCheckInterval;return;}
		UnitAny *pUnit=d2client_GetUnitFromId(dwWaitTeamEnterPortalId, UNITNO_OBJECT) ;
		if (!pUnit) {dwWaitTeamEnterPortalMs=0;return;}
		LeftClickUnit(pUnit);dwWaitTeamEnterPortalMs=0;
		return;
	}
	if (!dwLeaderId) {fAutoFollowMoving=0;return;}
	if (fWinActive) {
		if (dwLeaderId&&(fPlayerInTown||dwCurrentLevel==Level_ArreatSummit))
			follower_stop_follow();
		fAutoFollowMoving=0;return;
	}
	if (waitToMs&&dwCurMs<waitToMs) return;
	RosterUnit *pRU=getRosterUnit(dwLeaderId);
	if (!pRU) {follower_stop_follow();return;}
	if (clickState) {interactObject();waitToMs=dwCurMs+dwAutoFollowCheckInterval;return;}
	UnitAny *pUnit=d2client_GetUnitFromId(dwLeaderId, UNITNO_PLAYER) ;
	if (!pUnit) {fAutoFollowMoving=0;return;}
	int targetX=0,targetY=0;
	targetX=pUnit->pMonPath->wUnitX;
	targetY=pUnit->pMonPath->wUnitY;
	int x0=PLAYER->pMonPath->wUnitX;
	int y0=PLAYER->pMonPath->wUnitY;
	int dx=targetX-x0;
	int dy=targetY-y0;
	float dis2=(dx*dx+dy*dy)*4.0f/9.0f;
	if (!canRemoteControl()||dis2<dwAutoFollowDistance*dwAutoFollowDistance
		||dis2>dwAutoFollowMaxDistance*dwAutoFollowMaxDistance) {
		fAutoFollowMoving=0;
		return;
	}
	float dis=sqrt(dis2);
	fAutoFollowMoving=1;
	if (!canUseSkillNow(PLAYER,dwRightSkill)||PLAYER->dwMode==PlayerMode_Attacking1
		||PLAYER->dwMode==PlayerMode_Attacking2||PLAYER->dwMode==PlayerMode_Cast) return;
	float moveDistance=dis-dwAutoFollowDistance;
	if (moveDistance>dwAutoFollowMoveDistance) moveDistance=(float)dwAutoFollowMoveDistance;
	float percent=moveDistance/dis;
	int mx=(int)(dx*percent);
	int my=(int)(dy*percent);
	if (!mx&&!my) {
		if (dx>0) mx=1;else if (dx<0) mx=-1;
		if (dy>0) my=1;else if (dy<0) my=-1;
	}
	int tx=x0+mx;
	int ty=y0+my;
	LeftClickPos(tx,ty);
	waitToMs=dwCurMs+dwAutoFollowCheckInterval;
}
void follower_left_click_xy(int x,int y) {
	if (!canRemoteControl()) return;
	transferClickType=1;transferClickX=x;transferClickY=y;transferClickTimeout=dwCurMs+1000;
}
void follower_right_click_xy(int x,int y) {
	if (!canRemoteControl()) return;
	transferClickType=2;transferClickX=x;transferClickY=y;transferClickTimeout=dwCurMs+1000;
}
void follower_left_click_unit(int type,int id) {
	if (!canRemoteControl()) return;
	transferClickType=3;transferClickUnitType=type;transferClickUnitId=id;transferClickTimeout=dwCurMs+1000;
}
void follower_right_client_unit(int type,int id) {
	if (!canRemoteControl()) return;
	transferClickType=4;transferClickUnitType=type;transferClickUnitId=id;transferClickTimeout=dwCurMs+1000;
}
void refresh_followers(int groupSize) {
	refresh_clients();
	int d2raw=dwAutoFollowMaxDistance*dwAutoFollowMaxDistance*9/4;
	dwTeamMemberCount=0;
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		pwin->isTeam=0;
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
		if (dwGameWindowId<=groupSize&&i>groupSize||dwGameWindowId>groupSize&&i<=groupSize) continue;
		UnitAny *pUnit=d2client_GetUnitFromId(pwin->uid, UNITNO_PLAYER) ;
		if (!pUnit) continue;
		int d2=getUnitRawDistance2(PLAYER,pUnit);
		if (d2>d2raw) continue;
		pwin->isTeam=1;dwTeamMemberCount++;
	}
}
static int sendStartFollowCmd(D2Window *pwin) {
	return PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CMD,(MCC_StartFollow<<24)|dwGameWindowId);
}
static int sendStopFollowCmd(D2Window *pwin) {
	return PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CMD,(MCC_StopFollow<<24)|dwGameWindowId);
}
void toggle_follower(int gameId) {
	if (gameId>d2winLastId) refresh_clients();
	D2Window *pwin=&d2wins[gameId];
	if (gameId==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) return;
	pwin->isTeam=!pwin->isTeam;
	if (pwin->isTeam) {
		dwTeamMemberCount++;
		sendStartFollowCmd(&d2wins[gameId]);
	} else {
		dwTeamMemberCount--;
		sendStopFollowCmd(&d2wins[gameId]);
	}
}
void follower_stop_follow() {
	if (dwLeaderId&&dwLeaderGameId) {
		if (d2wins[dwLeaderGameId].hwnd)
			PostMessageA(d2wins[dwLeaderGameId].hwnd,
				WM_KEYUP,VK_MULTICLIENT_INFO,(MCI_StopFollow<<24)|(dwGameWindowId<<24));
	}
	dwLeaderGameId=0;dwLeaderId=0;followerInteractType=0;
	followerInteractId=0;clickState=0;fAutoFollowMoving=0;
}
UnitAny *findClosetPortal() {
	int minDis2=0x7fffffff;UnitAny *minUnit=NULL;
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[2*128+i];pUnit;pUnit=pUnit->pHashNext) {
			//blue portal 59 red portal 60
			if ((pUnit->dwTxtFileNo==59||pUnit->dwTxtFileNo==60)&&pUnit->pObjectData) {
				char *name=pUnit->pObjectData->owner;
				int canEnter=1;
				if (name[0]) {
					int len=strlen(name)+1;if (len>16) len=16;
					for (RosterUnit *pRU=PLAYERLIST;pRU;pRU=pRU->pNext) {
						if (memcmp(name,pRU->szName,len)==0) {
							if (pRU->wPartyId!=dwPlayerPartyId) canEnter=0;
						}
					}
				}
				if (!canEnter) continue;
				int dis2=getUnitRawDistance2(PLAYER,pUnit);if (dis2<minDis2) {minDis2=dis2;minUnit=pUnit;}
			}
		}
	}
	return minUnit;
}
void dropRune789();
void talkToNpc() {
	if (hasRune789) {
		dropRune789();
		return;
	}
	int npc1=0,npc2=0,qid=0;
	autoNpcTxt=0;
	switch (dwCurrentLevel) {
		case Level_RogueEncampment:npc1=Mon_Warriv1;break;
		case Level_LutGholein:npc1=Mon_Jerhyn;npc2=Mon_Meshif1;qid=14;break;
		case Level_KurastDocks:npc1=Mon_DeckardCain3;qid=21;break;
		case Level_Harrogath:npc1=Mon_QualKehk;qid=36;break;
		default:return;
	}
	UnitAny *npc=NULL;
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_MONSTER*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (pUnit->dwUnitType!=UNITNO_MONSTER) break;
			if (npc1&&pUnit->dwTxtFileNo==npc1||npc2&&pUnit->dwTxtFileNo==npc2) {npc=pUnit;break;}
		}
	}
	if (!npc) {
		send_multi_reply(MCR_ERROR);
		return;
	}
	short q=QUESTDATA->quests[qid];
	if (qid) LOG("auto npc quest %d = %X\n",qid,q);
	switch (npc->dwTxtFileNo) {
		case Mon_Jerhyn://200C->2014
			if (q&&(!(q&8))) goto done; //already talked
			break;
		case Mon_DeckardCain3://2018->2009
		case Mon_QualKehk:
			if (q&1) goto done; //already finish
			break;
	}
	send_multi_reply(MCR_OK);
	autoNpcTxt=npc->dwTxtFileNo;
	LeftClickUnit(npc);
	autoTerminateNpcInteractMs=dwCurMs+500;
	return;
done:
	send_multi_reply(MCR_DONE);
}
void follower_cmd(int lParam) {
	if (hydraLock) hydraLock=0;
	int cmd=(lParam>>24)&0xFF;
	switch (cmd) {
		case MCC_StartFollow: {
			int gid=lParam&0xFF;int uid=d2wins[gid].uid;
			if (dwLeaderId&&dwLeaderId!=uid) follower_stop_follow();
			dwLeaderGameId=gid;dwLeaderId=uid;
			waitToMs=0;clickState=0;fAutoFollowMoving=0;fSendInfoErr=0;followerWaypointToArea=0;
			if (dwLeaderId&&dwLeaderGameId&&d2wins[dwLeaderGameId].hwnd)
				PostMessageA(d2wins[dwLeaderGameId].hwnd,WM_KEYUP,VK_MULTICLIENT_INFO,
					(MCI_StartFollow<<24)|(dwGameWindowId<<24));
			break;
		}
		case MCC_StopFollow:follower_stop_follow();break;
		case MCC_BackToTown: {
			if (dwLeaderId) follower_stop_follow();
			if (!tMultiClient.isOn||!fInGame||fPlayerInTown) return;
			UnitAny *portal=findClosetPortal();
			if (portal) {
				int dis2=getUnitRawDistance2(PLAYER,portal);
				if (dis2<25) {
					gameMessage("Auto Follow: retreat to existing portal");
					transferClickType=3;transferClickUnitType=portal->dwUnitType;
					transferClickUnitId=portal->dwUnitId;transferClickTimeout=dwCurMs+1000;
					return;
				}
			}
			gameMessage("Auto Follow: back to town");
			BackToTown();
			break;
		}
		case MCC_EnterDoor: {
			int uid=lParam&0xFF;
			int leaderArea=(lParam>>16)&0xFF;
			int remoteArea=(lParam>>8)&0xFF;
			if (dwLeaderId) follower_stop_follow();
			if (!tMultiClient.isOn||!fInGame) return;
			if (!canRemoteControl()) return;
			char *name="";
			if (uid) {
				for (RosterUnit *pRU=PLAYERLIST;pRU;pRU=pRU->pNext) {
					if ((pRU->dwUnitId&0xFF)==uid) {name=pRU->szName;break;}
				}
			}
			int area=LEVELNO==leaderArea?remoteArea:leaderArea;
			UnitAny *portal=getPortalToArea(area,name);
			if (!portal) {gameMessage("Auto Follow: can't find portal to %d name %s",area,name);return;}
			if (dwEnterDoorInBackgroundSkill[dwPlayerClass][dwCurrentLevel]) {
				int skillId=dwEnterDoorInBackgroundSkill[dwPlayerClass][dwCurrentLevel];
				if (dwRightSkill!=0&&hasSkill(skillId)&&dwRightSkill!=skillId) selectSkill(1,skillId);
			}
			transferClickType=3;transferClickUnitType=portal->dwUnitType;
			transferClickUnitId=portal->dwUnitId;transferClickTimeout=dwCurMs+1000;
			if (tPacketHandler.isOn) gameMessage("enter %d:%d",transferClickUnitType,transferClickUnitId);
			break;
		}
		case MCC_TalkToNpc: {
			talkToNpc();
			break;
		}
		case MCC_Trade: {
			int gid=lParam&0xFF;
			acceptTradeFromGid=gid;
			acceptTradeFromUid=d2wins[gid].uid;acceptTradeMs=dwCurMs+10000;
			break;
		}
		case MCC_NextGame:QuickNextGame(1);break;
		case MCC_JoinGame: {
			if (fInGame) return;
			int gid=lParam&0xFF;
			D2Window *pwin=&d2wins[gid];
			if (!loadRuntimeInfo(pwin,gid)) return;
			joinGame(pwin->game,pwin->password);
			break;
		}
	}
}
static int curGrpSize;
int MultiClientStartFollow(int groupSize) {
	if (!tMultiClient.isOn||!fInGame) return 0;
	if (dwTeamMemberCount&&curGrpSize!=groupSize) MultiClientStopFollow();
	refresh_followers(groupSize);
	if (!dwTeamMemberCount) {gameMessage("Auto Follow: No team member found");return 0;}
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame||!pwin->isTeam) continue;
		sendStartFollowCmd(pwin);
	}
	curGrpSize=groupSize;
	return 1;
}
int MultiClientStopFollow() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
		sendStopFollowCmd(pwin);
		pwin->isTeam=0;
	}
	dwTeamMemberCount=0;
	return 1;
}
int MultiClientStartFollow1() {return MultiClientStartFollow(dwMultiClientGroupSize1);}
int MultiClientStartFollow2() {return MultiClientStartFollow(dwMultiClientGroupSize2);}
int MultiClientToggleFollow() {
	if (dwTeamMemberCount) return MultiClientStopFollow();
	else return MultiClientStartFollow(curGrpSize);
}
int MultiClientTalkToNpc() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	MultiClientStopFollow();
	refresh_followers(dwMultiClientGroupSize1);
	if (!dwTeamMemberCount) {gameMessage("Auto Follow: No team member found");return 0;}
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame||!pwin->isTeam) continue;
		PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CMD,(MCC_TalkToNpc<<24)|dwGameWindowId);
	}
	return 1;
}
int MultiClientAllNextGame() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	MultiClientStopFollow();
	refresh_followers(dwMultiClientGroupSize1);
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
		PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CMD,(MCC_NextGame<<24)|dwGameWindowId);
	}
	return 1;
}
int MultiClientJoinGame() {
	if (!tMultiClient.isOn) return 0;
	saveRuntimeInfo(d2gfx_GetHwnd());
	refresh_clients();
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd) continue;
		PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CMD,(MCC_JoinGame<<24)|dwGameWindowId);
	}
	return 1;
}
int leader_back_to_town() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
		if (!PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CMD,(MCC_BackToTown<<24)|dwGameWindowId)) pwin->hwnd=0;
	}
	return 1;
}
int MultiClientEnterDoor() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	UnitAny *portal=findClosetPortal();
	if (!portal) {gameMessage("no portal around");return 0;}
	char *name=portal->pObjectData->owner;
	int area=portal->pObjectData->nShrineNo;
	int uid=0;
	if (name[0]) {
		int len=strlen(name)+1;if (len>16) len=16;
		for (RosterUnit *pRU=PLAYERLIST;pRU;pRU=pRU->pNext) {
			if (memcmp(name,pRU->szName,len)==0) {uid=pRU->dwUnitId;break;}
		}
	}
	if (!d2winLastId||dwCurMs-refreshClientMs>6000) refresh_clients();
	int lParam=(MCC_EnterDoor<<24)|((LEVELNO&0xFF)<<16)|((area&0xFF)<<8)|(uid&0xFFFF);
	for (int i=1;i<=d2winLastId;i++) {
		D2Window *pwin=&d2wins[i];
		if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
		if (!PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CMD,lParam)) pwin->hwnd=0;
	}
	return 1;
}
int IsKeyDown(int keyCode);
int MultiClientStartClick(int param) {fTransferClick=1;return 1;}
int MultiClientStopClick(int param) {fTransferClick=0;return 1;}
int MultiClientInitClick() {
	if (!tMultiClient.isOn||!fInGame) return 0;
	if (!dwTeamMemberCount) {
		refresh_followers(dwMultiClientGroupSize1);
		if (!dwTeamMemberCount) {gameMessage("Transfer Click: No client found");return 0;}
	}
	return 1;
}
static int doTransferClick(int right,UnitAny *pUnit,int x,int y) {
	for (int runId=0;runId<2;runId++) {
		if (!dwTeamMemberCount) {
			MultiClientInitClick();
			if (!dwTeamMemberCount) {fTransferClick=0;return 0;}
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
		int d2raw=dwAutoFollowMaxDistance*dwAutoFollowMaxDistance*9/4;
		for (int i=1;i<=d2winLastId;i++) {
			D2Window *pwin=&d2wins[i];
			pwin->isTransferClick=0;
			if (i==dwGameWindowId||!pwin->hwnd||!pwin->sameGame) continue;
			UnitAny *pUnit=d2client_GetUnitFromId(pwin->uid, UNITNO_PLAYER);if (!pUnit) continue;
			int d2=getUnitRawDistance2(PLAYER,pUnit);if (d2>d2raw) continue;
			if (!PostMessageA(pwin->hwnd,WM_KEYUP,w,l)) err++;
			pwin->isTransferClick=1;
		}
		if (err) {
			gameMessage("MultiClient post message error");
			dwTeamMemberCount=0;
			fTransferClick=0;
		} else break;
	}
	return 1;
}
int transferClick(int right,UnitAny *pUnit,int x,int y) {
	if (!IsKeyDown(tMultiClientClick.key)&&!IsKeyDown(tMultiClientClick2.key)) {fTransferClick=0;return 0;}
	return doTransferClick(right,pUnit,x,y);
}
int canTranferClick() {
	if (d2client_pScreenBlocked[0]&1) { //right half blocked
		if (*d2client_pMouseX>=SCREENSIZE.x/2) return 0;
	} else if (d2client_pScreenBlocked[0]&2) { //left half blocked
		if (*d2client_pMouseX<=SCREENSIZE.x/2) return 0;
	}
	return 1;
}
extern int ctrlDown;
static int protect3BB() {
	if (DIFFICULTY==2&&t3BBProtect.isOn) {
		char keyname[256];
		formatKey(keyname,t3BBProtect.key);
		gameMessage("3BB protect (%s)",keyname);
		return 1;
	}
	return 0;
}
int __fastcall shouldLeftClick(int *args) {
	int src=args[1];
	UnitAny *pUnit=(UnitAny *)args[2];
	int x=args[3];
	int y=args[4];
	if (tPacketHandler.isOn) {
		LOG("left click arg=0x%x x=%d y=%d unit=%x\n",args[0],x,y,pUnit);
	}
	if (pUnit&&pUnit->dwTxtFileNo==546&&protect3BB()) return 0;
	if (fPlayerInTown&&pUnit&&pUnit->dwUnitType==UNITNO_PLAYER&&isLocalPlayer(pUnit->dwUnitId)) {
		D2Window *pwin=getLocalPlayer(pUnit->dwUnitId);
		if (pwin) {
			acceptTradeFromGid=pwin->index;
			acceptTradeFromUid=pwin->uid;
			PostMessageA(pwin->hwnd,WM_KEYUP,VK_AUTOFOLLOW_CMD,(MCC_Trade<<24)|dwGameWindowId);
		}
	}
	if (fTransferClick&&canTranferClick()) {
		if (dwTeamMemberCount) MultiClientStopFollow();
		transferClick(0,pUnit,x,y);
		if (fTransferClick) return 0;
	} else if (ctrlDown&&canTranferClick()) {
		if (dwTeamMemberCount) MultiClientStopFollow();
		doTransferClick(0,pUnit,x,y);
	}
	if (pUnit&&tNecNoAttackInHell.isOn&&DIFFICULTY==2&&dwPlayerClass==2
		&&dwLeftSkill==0&&!fPlayerInTown&&pUnit->dwUnitType==UNITNO_MONSTER)
		args[2]=0;
	if (dwLeftSkill==2) {
		if (d2common_GetUnitStat(PLAYER, 70, 0)<=dwAutoThrowMinQuantity
			&&!PLAYER->pInventory->pCursorItem
			&&((!(d2client_pScreenBlocked[0]&1)&&*d2client_pMouseX>SCREENSIZE.x/2)
			||(*d2client_pMouseX<SCREENSIZE.x/2&&!(d2client_pScreenBlocked[0]&2)))) {
			gameMessage("Only %d left,need repair",dwAutoThrowMinQuantity);
			return 0; //quantity, keep some to repair
		}
	}
	if (pUnit&&fWinActive&&dwTeamMemberCount) {
		switch (pUnit->dwUnitType) {
			case UNITNO_OBJECT:
				if (fPlayerInTown&&pUnit->dwTxtFileNo==59) { //blue portal
					char *selfname=(*d2client_pGameInfo)->szCharName;
					char *name=pUnit->pObjectData->owner;
					if (name&&strcmp(selfname,name)==0) { //need all team member to enter first
						int n=teamMemberInArea(LEVELNO);
						if (n) {
							leader_click_object(pUnit->dwUnitType,pUnit->dwUnitId);
							dwWaitTeamEnterPortalId=pUnit->dwUnitId;
							dwWaitTeamEnterPortalMs=dwCurMs+dwWaitTeamEnterPortalMaxDelayMs;
							return 0;
						}
					}
				}
				break;
			/*case UNITNO_MONSTER:
				switch (pUnit->dwTxtFileNo) {
					case 148://Akara
					case 176://Atma
					case 178://Fara
					case 255://Ormus
					case 405://Jamella
					case 513://Malah
						leader_click_object(pUnit->dwUnitType,pUnit->dwUnitId);
				}
				break;
			*/
		}
	}
	if (dwWaitTeamEnterPortalMs) dwWaitTeamEnterPortalMs=0;
	return 1;
}
int __fastcall shouldRightClick(int *args) {
	int src=args[1];
	UnitAny *pUnit=(UnitAny *)args[2];
	int x=args[3];
	int y=args[4];
	if (tPacketHandler.isOn) {
		LOG("right click arg=0x%x x=%d y=%d unit=%x\n",args[0],x,y,pUnit);
	}
	if (pUnit&&pUnit->dwTxtFileNo==546&&protect3BB()) {
		if (dwRightSkill!=Skill_Teleport) return 0;
		args[2]=0;
	}
	if (fWinActive&&fUserOperating&&shiftDown&&dwRightSkill==Skill_Hydra) {
		hydraLock=1;hydraUid=pUnit?pUnit->dwUnitId:0;hydraX=x;hydraY=y;
	}
	if (fTransferClick&&canTranferClick()) {
		transferClick(1,pUnit,x,y);
		if (fTransferClick) return 0;
	}
	if (dwRightSkill==2) {
		if (d2common_GetUnitStat(PLAYER, 70, 0)<=dwAutoThrowMinQuantity
			&&((!(d2client_pScreenBlocked[0]&1)&&*d2client_pMouseX>SCREENSIZE.x/2) //x,y is inverted
			||(*d2client_pMouseX<SCREENSIZE.x/2&&!(d2client_pScreenBlocked[0]&2)))) {
			gameMessage("Only %d left,need repair",dwAutoThrowMinQuantity);
			return 0; //quantity, keep some to repair
		}
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
		call dword ptr [d2client_ShouldClick85E0]
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
		call dword ptr [d2client_ShouldClick85E0]
		ret
cancel_click:
		popad
		xor eax,eax
		ret
	}
}
static void __fastcall recvCommand77(char *packet) {
	if (fWinActive) return;
	int cmd=packet[1];
	if (tPacketHandler.isOn) LOG("button action 0x%X\n",cmd);
	switch (cmd) {
		case 1: //trade request
			tradingId=0;
			if (acceptTradeFromUid&&dwCurMs<acceptTradeMs) {
				need_paint=2;clickAcceptTradeMs=dwCurMs+100;
			}
			break;
		case 6: //start trade
			break;
		case 5: {//remote complete trade
			if (!acceptTradeFromUid) return;
			if (acceptTradeFromUid!=tradingId) {
				gameMessage("Trading ID mismatch %d %d",acceptTradeFromUid,tradingId);
				return;
			}
			if (!isLocalPlayer(acceptTradeFromUid)) {
				gameMessage("Not local player %d",acceptTradeFromUid);
				return;
			}
			if (strcmp(d2wins[acceptTradeFromGid].name,tradingName)!=0) {
				gameMessage("Trading name mismatch %s %s",d2wins[acceptTradeFromGid].name,tradingName);
				return;
			}
			BYTE packet[7]={0x4f,4,0};
			SendPacket(packet,sizeof(packet));
			break;
		}
		case 0x10: //open stash
			break; 
		case 0xD: //trade end
			tradingId=0;
			break;
	}
}
static void __fastcall recvCommand78(char *packet) {
	memcpy(tradingName,packet+1,16);
	tradingId=*(int *)(packet+0x11);
	LOG("trade id=%d name=%s\n",tradingId,tradingName);
}
/*
	d2client_AF930: 8B C1              mov eax, ecx
	d2client_AF932: E9 99 A5 FA FF     jmp d2client_59ED0 <--
*/
void __declspec(naked) RecvCommand_77_Patch_ASM() {
	__asm {
		pushad
		call recvCommand77
		popad
		add esp,4
		jmp d2client_recvCmd77ButtonAction
		ret
	}
}
/*
	d2client_ABFD3: 8B 01              mov eax, [ecx] <--
	d2client_ABFD5: 8B 51 04           mov edx, [ecx+0x4]
*/
void __declspec(naked) RecvCommand_78_Patch_ASM() {
	__asm {
		pushad
		call recvCommand78
		popad
		mov eax, [ecx]
		mov edx, [ecx+4]
		ret
	}
}
