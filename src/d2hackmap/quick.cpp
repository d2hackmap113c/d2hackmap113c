#include "stdafx.h"
#include "header.h"
#include "multi.h"

extern int need_paint;
int QuickNextGame(int addnum);
void SaveGameName();
int QuickExitGame();
int fCanExitGame=0,dwBackToTownTimeout=0,nTownportalAlertNums=-1;
static int fCastedTownPortal=0,fEnterPortal=0;
int fAutoNextGameName=0,fAutoNextGamePassword=0,fAutoNextGameDescription=0;
static int takeWPtoTownDis=6;
wchar_t	wszGameName[32]={0};
wchar_t wszGamePassword[32]={0};
wchar_t wszGameDescription[32]={0};
ToggleVar tBackToTown={TOGGLEVAR_DOWN,	0,	-1,		1,	"Quick Back Town",		&BackToTown};
ToggleVar tExitGame={TOGGLEVAR_DOWN,	0,	-1,	 1, 	"ExitGame",	&QuickExitGame, 0};
ToggleVar tQuickNextGame={TOGGLEVAR_DOWNPARAM,	0,	-1,	 1, "NextGame",	&QuickNextGame,	1};
ToggleVar tQuickNextGame2={TOGGLEVAR_DOWNPARAM,	0,	-1,	 1, "NextGame2",	&QuickNextGame,	2};
ToggleVar tAllNextGame={TOGGLEVAR_DOWNPARAM,	0,	-1,	 1, "AllNextGame",	&QuickNextGame,	3};
ToggleVar tAutoJoinGame={TOGGLEVAR_ONOFF,	0,	-1,	 1, "AutoJoinGame",0,0,0,2};
ToggleVar tCancelAutoJoinIfForeground={TOGGLEVAR_ONOFF,	0,	-1,	 1, "CancelAutoJoinIfForeground",0,0,0,2};
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY, "AutoJoinGameToggle",     &tAutoJoinGame},
  {CONFIG_VAR_TYPE_KEY, "CancelAutoJoinIfForeground",     &tCancelAutoJoinIfForeground},
	{CONFIG_VAR_TYPE_KEY, "QuickBackToTownKey",   &tBackToTown		    	},
  {CONFIG_VAR_TYPE_QUICK_TO_LEVEL, "QuickToLevel",NULL,0,{0}},
	{CONFIG_VAR_TYPE_INT, "TakeWaypointToTownDis",  &takeWPtoTownDis, 4    },
	{CONFIG_VAR_TYPE_INT, "TownportalNumsAlert",  &nTownportalAlertNums, 4    },
  {CONFIG_VAR_TYPE_KEY, "QuickExitGameKey",      &tExitGame              },
  {CONFIG_VAR_TYPE_KEY, "QuickNextGameKey",      &tQuickNextGame         },
  {CONFIG_VAR_TYPE_KEY, "QuickNextGameKey2",     &tQuickNextGame2        },
  {CONFIG_VAR_TYPE_KEY, "AllNextGameKey",     &tAllNextGame},
  {CONFIG_VAR_TYPE_INT, "AutoNextGameName",      &fAutoNextGameName  ,     4 },
  {CONFIG_VAR_TYPE_INT, "AutoNextGamePassword",  &fAutoNextGamePassword  , 4 },
  {CONFIG_VAR_TYPE_INT, "AutoNextGameDescription",  &fAutoNextGameDescription  , 4 },
  {CONFIG_VAR_TYPE_WSTR, "DefaultGameName",       &wszGameName,     1,  {32 }},
  {CONFIG_VAR_TYPE_WSTR, "DefaultGamePassword",   &wszGamePassword, 1,  {32 }},
  {CONFIG_VAR_TYPE_WSTR, "DefaultGameDescription",   &wszGameDescription, 1,  {32 }},
};
void quick_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void quick_NewGame() {
	SaveGameName();
	fCanExitGame = TRUE;
}
void quick_EndGame() {
	fCanExitGame = FALSE;
}
int skillCount(int skillid){
	if (PLAYER->pSkill){
		Skill *pSkill = PLAYER->pSkill->pFirstSkill;
		while(pSkill){
			if(pSkill->pSkillInfo->wSkillId == skillid){
				return pSkill->dwQuality;
			}
			pSkill= pSkill->pNextSkill;
		}
	}
	return 0;
}

int takeWaypointToLvl(int lvl,int maxdis);
static int lastSkill=-1;
int BackToTown() {
	if (fPlayerInTown) {
		dwBackToTownTimeout=0;
		if (ACTNO>0) {
			if (takeWPtoTownDis) takeWaypointToLvl(Level_RogueEncampment,takeWPtoTownDis);
		} else {
			if (takeWPtoTownDis) takeWaypointToLvl(Level_Harrogath,takeWPtoTownDis);
		}
		return 0;
	}
	if (takeWPtoTownDis&&takeWaypointToLvl(actlvls[ACTNO],takeWPtoTownDis)) return 0;
	dwBackToTownTimeout=dwCurMs+1000;fCastedTownPortal=0;fEnterPortal=0;
	lastSkill=dwRightSkill;
	switchRightSkill(skillCount(219)>0?219:220);//use scroll first
	return 0;
}
struct stQuickToLevel {
	int key,area1,area2;struct stQuickToLevel *next;
};
static stQuickToLevel *quickToLevels=NULL;
int QuickToLevel(stQuickToLevel *p) {
	int lvl=p->area1;
	if (dwCurrentLevel==lvl) lvl=p->area2;
	if (takeWPtoTownDis) return takeWaypointToLvl(lvl,takeWPtoTownDis);
	return 0;
}
void addQuickToLevel(int key,int area1,int area2) {
	for (stQuickToLevel *p=quickToLevels;p;p=p->next) {
		if (p->key==key) {p->area1=area1;p->area2=area2;return;}
	}
	stQuickToLevel *p=(stQuickToLevel *)HeapAlloc(confHeap,0,sizeof(stQuickToLevel));
	p->key=key;p->area1=area1;p->area2=area2;
	p->next=quickToLevels;quickToLevels=p->next;
	ToggleVar *ptv=addExtraKey(key);if (!ptv) {LOG("ERROR: too many keys\n");return;}
	ptv->type=TOGGLEVAR_DOWNPTR;ptv->key=key;
	ptv->paramPtr=p;ptv->keyType=0;
	ptv->func=&QuickToLevel;ptv->funcUp=NULL;
	ptv->desc="QuickToLevel";
}
void quickLoop() {
	if (fPlayerInTown||dwCurMs>dwBackToTownTimeout) {dwBackToTownTimeout=0;return;}
	if (!fCastedTownPortal) {
		if (!fCanUseRightSkill) return;
		if (PLAYER->dwMode==PlayerMode_Attacking1||PLAYER->dwMode==PlayerMode_Attacking2
			||PLAYER->dwMode==PlayerMode_Cast) return;
		BYTE rightSkill[5] = {0x0C};
		*(short *)&rightSkill[1] = PLAYER->pMonPath->wTargetX;
		*(short *)&rightSkill[3] = PLAYER->pMonPath->wTargetY;
		SendPacket(rightSkill,sizeof(rightSkill));
		fCastedTownPortal=1;fEnterPortal=0;
	}
}

extern int dwTeamMemberCount;
void __fastcall AutoBackToTown(BYTE *aPacket) {
	//LOG("QuickBackTown packet %02x\n",aPacket[0]);
	/*if (aPacket[0]==0x60) {
		LOG("QuickBackTown state=%d area=%d id=%d\n",
			aPacket[1],aPacket[2],*(int *)(aPacket+3));
	}
	*/
	if (aPacket[0]==0x60&&aPacket[1]==0x00) {//00限定只能走通向城里的传送门
		if (dwBackToTownTimeout&&!fEnterPortal) {
			fEnterPortal=1;
			int id=*(int*)&aPacket[3];// portal ID
			BYTE castMove[9] = {0x13};
			*(int*)&castMove[1] = 2;
			*(int*)&castMove[5] = id; 
			if (fWinActive&&dwTeamMemberCount) leader_click_object(UNITNO_OBJECT,id);
			SendPacket(castMove,sizeof(castMove));
			if (lastSkill>=-1) {switchRightSkill(lastSkill);lastSkill=-1;}
		}
		if( nTownportalAlertNums!= (DWORD)-1 ){
			int srollcount = skillCount(220);
			if ( srollcount<= nTownportalAlertNums ){
				gameMessageWColor(8,
					dwGameLng?L"只有%d个回城卷轴了":L"<Hackmap>: Warning !! Only %d Townportals Left ", srollcount);
			}
		}
	}
}


void __declspec(naked) RecvCommand_60_Patch_ASM() {
	__asm {
		//ecx = cmdbuf
		push ecx
		call AutoBackToTown;
		pop ecx
		pop edx //ret code
//org:
		sub esp, 8
		mov eax, dword ptr [ecx]
		jmp edx
	}
}
void setGameNamePassword(char *name,char *pass) {
	if (!name||!name[0]) return;
	MultiByteToWideChar(CP_ACP, 0, name, -1, wszGameName, 32);
	if (pass&&pass[0]) {
		MultiByteToWideChar(CP_ACP, 0, pass, -1, wszGamePassword, 32);
	} else {
		wszGamePassword[0]=0;
	}
}
static int autoJoinState=0;
int autoJoinGameMs=0;
extern int mainMenuLoopCount; 
void autoJoinGame() {
	if (!autoJoinState) return;
	if (fWinActive&&tCancelAutoJoinIfForeground.isOn) {
		autoJoinState=0;return;
	}
	void *p=*d2win_pvar_214A0;if (!p) {
		LOG("auto join: no screen\n");
		return;
	}
	int type=*(int *)p;if (type!=6) {
		LOG("auto join: screen type=%d\n",type);
		return;
	}
	D2EditBox *focus=(*d2win_pFocusedControl);
	if (focus) {
		LOG("joinGame state=%d focus=%d %d %d %d\n",autoJoinState,focus->dwPosX,focus->dwPosY,focus->dwSizeX,focus->dwSizeY);
	} else {
		LOG("joinGame state=%d menuLoop=%d\n",autoJoinState,mainMenuLoopCount);
	}
	switch (autoJoinState) {
		case 1: {
			autoJoinGameMs=dwCurMs+100;need_paint=2;
			if (mainMenuLoopCount>10) autoJoinState=2;
			break;
		}
		case 2: {
			if (focus) return;
			LOG("click join button\n");
			int mx=704,my=461;
			HWND hwnd=d2gfx_GetHwnd();int wParam=0;
			SendMessage(hwnd,WM_MOUSEMOVE,wParam, MAKELONG(mx,my));
			SendMessage(hwnd,WM_LBUTTONDOWN,wParam,MAKELONG(mx,my));
			SendMessage(hwnd,WM_LBUTTONUP,wParam, MAKELONG(mx,my));
			//SendMessage(hwnd,WM_MOUSEMOVE,wParam, MAKELONG(mx,my));
			autoJoinGameMs=dwCurMs+100;need_paint=2;
			autoJoinState=3;
			break;
		}
		case 3: {
			if (!focus) {
				LOG("ERROR join game screen doesn't show up\n");
				return;
			}
			if (focus->dwPosX!=432) return;
			if (focus->dwPosY!=148) return;
			if (focus->dwSizeX!=155) return;
			if (focus->dwSizeY!=20) return;
			LOG("click join game button\n");
			int mx=680,my=420;
			HWND hwnd=d2gfx_GetHwnd();int wParam=0;
			SendMessage(hwnd,WM_MOUSEMOVE,wParam, MAKELONG(mx,my));
			SendMessage(hwnd,WM_LBUTTONDOWN,wParam,MAKELONG(mx,my));
			SendMessage(hwnd,WM_LBUTTONUP,wParam, MAKELONG(mx,my));
			//SendMessage(hwnd,WM_MOUSEMOVE,wParam, MAKELONG(mx,my));
			autoJoinGameMs=0;need_paint=2;
			autoJoinState=0;
			break;
		}
	}
}
extern int exitGameMs;
void joinGame(char *name,char *pass) {
	setGameNamePassword(name,pass);
	if (!tAutoJoinGame.isOn) return;
	LOG("Auto join game: game=%s password=%s mainMenuLoopCount=%d\n",name,pass,mainMenuLoopCount);
	autoJoinState=1;autoJoinGameMs=dwCurMs+100;need_paint=2;
	if (mainMenuLoopCount<10) autoJoinGameMs=dwCurMs+500;
}
void SaveGameName() {
	if(GAMEINFO->szGameName[0]) {
		int n=MultiByteToWideChar(CP_ACP, 0, GAMEINFO->szGameName, -1, wszGameName, 32);
		wchar_t *dst=wszGamePassword;
		char *src=GAMEINFO->szGamePassword;
		do {*dst++=*src++;} while (*src);
		MultiByteToWideChar(CP_ACP, 0, GAMEINFO->szGameDescription, -1, wszGameDescription, 32);
	}
}
int canExitGame();
int QuickExitGame(){
	if (!canExitGame()) return -1;
	if (fCanExitGame==FALSE) return -1;
	fCanExitGame = FALSE;
	ExitGame();
	return 0;
}

int MultiClientAllNextGame();
int QuickNextGame(int addnum) {	
	if (!canExitGame()) return -1;
	if (fCanExitGame==FALSE) return -1;
	fCanExitGame = FALSE;
	if (addnum==3) {
		MultiClientAllNextGame();
		addnum=1;
	}
	SaveGameName();
	wchar_t *s = wszGameName;
	wchar_t *endptr;
	int len=wcslen(wszGameName);
	int end=0;
	for (int i=len-1;i>=0;i--) {
		wchar_t c=wszGameName[i];
		if ('0'<=c&&c<='9') {end=i+1;break;}
	}
	if (end) {
		wchar_t fmt[8],buf[32];
		int start=end-1;
		for (int i=start-1;i>=0;i--) {
			wchar_t c=wszGameName[i];
			if ('0'<=c&&c<='9') start=i;else break;
		}
		fmt[0]='%';fmt[1]='0';
		wsprintfW(fmt+2, L"%dd%%s",end-start);
		wcscpy(buf,wszGameName+end);
		long n = wcstol(wszGameName+start, &endptr, 10);
		wsprintfW(wszGameName+start, fmt, n+addnum,buf);
	} else {
		wszGameName[len]='1';
		wszGameName[len+1]=0;
	}
	ExitGame();
	return 0;
}

void __fastcall NextGameNamePatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,int,char*)) {
	if (fAutoNextGameName) {
		d2win_SetEditBoxText(box, wszGameName);
		d2win_SelectEditBoxText(box);
	}
	// original code
	d2win_SetEditBoxProc(box, FunCallBack);
}
void __fastcall NextGamePasswordPatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,int,char*)) {
	if (fAutoNextGamePassword) {
		d2win_SetEditBoxText(box, wszGamePassword);
		d2win_SelectEditBoxText(box);
	}
	// original code
	d2win_SetEditBoxProc(box, FunCallBack);
}
void __fastcall NextGameDecriptionPatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,int,char*)) {
	if (fAutoNextGameDescription) {
		d2win_SetEditBoxText(box, wszGameDescription);
		d2win_SelectEditBoxText(box);
	}
	// original code
	d2win_SetEditBoxProc(box, FunCallBack);
}
void *__fastcall SelectGamePatch(D2EditBox* box, wchar_t* wcszTxt) {
	if (!fInGame&&box) {
		D2EditBox *next=box->pNext2;
		if (next&&next->pNext2==box&&box->dwPosY==148&&next->dwPosY==148
			&&box->dwPosX==432&&next->dwPosX==606) {
			d2win_SetEditBoxText(next, L"");
		}
	}
	if (wcszTxt[1]==':') wcszTxt+=2;
	// original code
	return d2win_SetEditBoxText(box, wcszTxt);
}

