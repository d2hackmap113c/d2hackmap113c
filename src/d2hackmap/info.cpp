#include "stdafx.h"
#include "header.h"
#include "multi.h"
#include "auto.h"

extern int dwEnterGameMs;
int SetLifeManaShow();
UnitAny* GetViewStatUnit();
UnitAny*  GetViewEquUnit();
void DrawScrollOffset(wchar_t*wbuf ,int posy);
int ViewUnit(int nType);
void SetViewUnit();
int SwitchStatPage(int nStep);
int fViewingTargetUnit=0;
ToggleVar 	tServerIp={			    TOGGLEVAR_ONOFF,	1,	-1,		1,	"Server Ip" };
ToggleVar 	tGameTime={				TOGGLEVAR_ONOFF,	1,	-1,		1,	"Game Time" };
ToggleVar 	tClock={					TOGGLEVAR_ONOFF,	1,	-1,		1,	"Clock" };
ToggleVar 	tLayerLevelNo={			TOGGLEVAR_ONOFF,	0,	-1,		1,	"Layer LevelNo" };
ToggleVar 	tAreaLevel={				TOGGLEVAR_ONOFF,	0,	-1,		1,	"Area Level" };
ToggleVar 	tPermShowOrb={		TOGGLEVAR_ONOFF,	1,	-1,	 1,  "Perm Show Life/Mana" , &SetLifeManaShow};
ToggleVar 	tShowExp={				TOGGLEVAR_ONOFF,	0,	-1,	  2,	"Show Exp Toggle"  };
ToggleVar 	tShowPing={				TOGGLEVAR_ONOFF,	0,	-1,	  1,	"Show Ping Toggle" };
ToggleVar 	tShowPartyPosition={				TOGGLEVAR_ONOFF,	0,	-1,	1,	"ShowPartyPosition"};
ToggleVar 	tShowPartyLevel={				TOGGLEVAR_ONOFF,	0,	-1,	1,	"ShowPartyLevel"};
int   currentgameexp  =0;
ToggleVar 	tViewEquipmentKey={	    TOGGLEVAR_DOWNPARAM,	0,	-1,  1,	"ViewEquipmentKey", &ViewUnit ,UIVAR_INVENTORY};
ToggleVar 	tViewPlayerStatsKey={    TOGGLEVAR_DOWNPARAM,	0,	-1,	 1, "ViewPlayerStatsKey", &ViewUnit ,UIVAR_STATS};
ToggleVar 	tShowBaseStat={				TOGGLEVAR_ONOFF,	1,	-1,	1 , "Show Player Base Stat"};
ToggleVar 	tFirstPlayerStat={			TOGGLEVAR_DOWNPARAM,	0,	-1,	1 , "FirstPlayerStat", &SwitchStatPage		,0};
ToggleVar 	tNextPlayerStat={			TOGGLEVAR_DOWNPARAM,	0,	-1,	1 , "NextPlayerStat", &SwitchStatPage		,1};
ToggleVar 	tPrevPlayerStat={			TOGGLEVAR_DOWNPARAM,	0,	-1,	1 , "PrevPlayerStat", &SwitchStatPage		,-1};
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY, "ServerIpToggle",			&tServerIp        },
  {CONFIG_VAR_TYPE_KEY, "GameTimeToggle",         &tGameTime        },
  {CONFIG_VAR_TYPE_KEY, "ClockToggle",            &tClock           },
  {CONFIG_VAR_TYPE_KEY, "LayerLevelNoToggle",     &tLayerLevelNo    },
  {CONFIG_VAR_TYPE_KEY, "AreaLevelToggle",        &tAreaLevel       },
	{CONFIG_VAR_TYPE_KEY, "PermShowOrbsToggle",    &tPermShowOrb       },
	{CONFIG_VAR_TYPE_KEY, "ShowExpToggle",	&tShowExp   },
	{CONFIG_VAR_TYPE_KEY, "ShowPingToggle",	&tShowPing  }, 
	{CONFIG_VAR_TYPE_KEY, "ShowPartyPositionToggle",		&tShowPartyPosition     },
	{CONFIG_VAR_TYPE_KEY, "ShowPartyLevelToggle",			&tShowPartyLevel		},
  {CONFIG_VAR_TYPE_KEY, "ViewEquipmentKey",      &tViewEquipmentKey    },
  {CONFIG_VAR_TYPE_KEY, "ViewPlayerStatsKey",    &tViewPlayerStatsKey  },
  {CONFIG_VAR_TYPE_KEY, "FirstPlayerStatKey",     &tFirstPlayerStat   },
  {CONFIG_VAR_TYPE_KEY, "NextPlayerStatKey",      &tNextPlayerStat    },
  {CONFIG_VAR_TYPE_KEY, "PreviousPlayerStatKey",  &tPrevPlayerStat    },
  {CONFIG_VAR_TYPE_KEY, "ShowBaseStat",           &tShowBaseStat      },
};
void info_AddConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void InitExp();
void infoNewGame() {
	SetLifeManaShow();
	InitExp();
	SwitchStatPage(0);
}
#define INFOX (*d2client_pInfoPositionX)-16
#define INFOY (*d2client_pInfoPositionY)
wchar_t * __cdecl wsprintfW2(wchar_t *dest, char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	int len = wvsprintf((char *)dest, fmt, va);
	for (int i = len; i >= 0; i--) {
		dest[i] = ((char *)dest)[i];
	}
	return dest;
}
void __fastcall DrawGameInfoPatch(int dwFont) {
	wchar_t wbuf[512];
	int xpos=INFOX;
	int ypos = INFOY;
	int dwOldFone = d2win_SetTextFont(8);
	if (tLayerLevelNo.isOn) {
		wsprintfW(wbuf, L"Layer No: %d", LAYER->dwLayerNo);
		drawBgTextLeft(wbuf ,	xpos , ypos , 4 , 0x10 );
		ypos += 16;
		wsprintfW(wbuf, L"Level No: %d", LEVELNO);
		drawBgTextLeft(wbuf ,	xpos , ypos , 4 , 0x10 );
		ypos += 16;
	}
	if (tAreaLevel.isOn) {
		wsprintfW(wbuf, L"Area Level: %d", GetAreaLevel());
		drawBgTextLeft(wbuf ,	xpos , ypos , 4 , 0x10 );
		ypos += 16;
	}
	if ( tClock.isOn ) {
		SYSTEMTIME t;
		GetLocalTime(&t);
		wsprintfW(wbuf, L"%.2d:%.2d:%.2d", t.wHour, t.wMinute, t.wSecond );
		drawBgTextLeft(wbuf ,	xpos , ypos , 4 , 0x10 );
		ypos += 16;
	}
	if (tGameTime.isOn) {
		int secs = (dwCurMs - dwEnterGameMs)/1000;
		wsprintfW(wbuf, L"%.2d:%.2d:%.2d", secs/3600, (secs/60)%60, secs%60);
		drawBgTextLeft(wbuf ,	xpos , ypos , 4 , 0x10 );
		ypos += 16;
	}
	if (tServerIp.isOn && GAMEINFO->szGameServerIp[0] ) {
		wsprintfW(wbuf, L"%hs", GAMEINFO->szGameServerIp);
		drawBgTextLeft(wbuf ,	xpos , ypos , 4 , 0x10 );
		ypos += 16;
	}
	DrawScrollOffset(wbuf,ypos);
	d2win_SetTextFont(dwOldFone);
	d2win_SetTextFont(dwFont);
}

extern int dwSkeletonMageHPPercent;
extern int dwEnchantSkeletonCount;

void __fastcall DrawPetHeadPatch(int xpos , UnitAny *pUnit){
	if ( tShowPartyLevel.isOn ){
		wchar_t wszTemp[512];
		wsprintfW( wszTemp, L"%d" ,  d2common_GetUnitStat(pUnit, STAT_LEVEL, 0) );
		DrawD2Text(1, wszTemp ,	xpos+5 ,	57 ,	0 );
	}
}

void __declspec(naked) DrawPetHeadPatch_ASM() {
	//ecx  xpos
	//eax  ypos
	//ebx  pPet
	__asm {
		push esi
		mov edx , ebx 
		call DrawPetHeadPatch
		pop esi
//org
		mov [esp+0x56], 0
		ret
	}
}

extern ToggleVar tSwitchWindowKeys[16][2];
static D2Window *partyHeadCache[16]={0};
struct PartyHeadPos {int x,gameId;};
static PartyHeadPos partyHeadPos[16];
static int partyHeadCachePos=0;
int getGameWinId(int mouseX) {
	for (int i=0;i<partyHeadCachePos;i++) {
		int x=partyHeadPos[i].x;
		if (x<=mouseX&&mouseX<=x+46) return partyHeadPos[i].gameId;
	}
	return 0;
}
void __fastcall DrawPartyHeadPatch(int xpos , RosterUnit *pRosterUnit){
	wchar_t wszTemp[32];
	if ( tShowPartyLevel.isOn ){
		wsprintfW( wszTemp, L"%d" , pRosterUnit->wLevel );
		DrawD2Text(1, wszTemp ,	xpos+5 ,	57 ,	0 );
	}
	if (d2winLastId) {
		D2Window *pwinC=partyHeadCache[partyHeadCachePos];
		if (!pwinC||pwinC->uid!=pRosterUnit->dwUnitId) {
			pwinC=NULL;
			for (int i=1;i<=d2winLastId;i++) {
				D2Window *pwin=&d2wins[i];
				if (pwin->uid==pRosterUnit->dwUnitId) {
					pwinC=pwin;partyHeadCache[partyHeadCachePos]=pwin;
					break;
				}
			}
		}
		partyHeadPos[partyHeadCachePos].x=xpos;
		partyHeadPos[partyHeadCachePos].gameId=pwinC?pwinC->index:0;
		if (pwinC&&pwinC->manaMax&&pwinC->infoMs&&dwCurMs<pwinC->infoMs) {
			if (!pwinC->switchkey[0]&&pwinC->index>=1&&tSwitchWindowKeys[pwinC->index][0].key>0) {
				formatKey(pwinC->switchkey,tSwitchWindowKeys[pwinC->index-1][0].key);
			}
			int rw=46*pwinC->mana/pwinC->manaMax;if (rw>46) rw=46;
			d2gfx_DrawRectangle(xpos,8,xpos+rw,13,0x91,5);
			d2gfx_DrawRectangle(xpos+rw,8,xpos+46,13,0,5);
			int x=xpos,y=92;
			if (pwinC->isTeam) {
				wszTemp[0]='T';wszTemp[1]=0;
				x+=drawBgText(wszTemp,x,y,0,0x76)+3;
			}
			if (pwinC->hPotion) {
				wsprintfW(wszTemp,L"%d",pwinC->hPotion);
				x+=drawBgText(wszTemp,x,y,0,0x0A)+3;
			}
			if (pwinC->mPotion) {
				wsprintfW(wszTemp,L"%d",pwinC->mPotion);
				x+=drawBgText(wszTemp,x,y,0,0x91)+3;
			}
			if (pwinC->rPotion) {
				wsprintfW(wszTemp,L"%d",pwinC->rPotion);
				x+=drawBgText(wszTemp,x,y,0,0x4B)+3;
			}
			x=xpos;y=105;
			if (pwinC->minions) {
				if (pwinC->enMinions) {
					wsprintfW(wszTemp,L"%d/",pwinC->enMinions);
					x+=drawBgText(wszTemp,x,y,1,0x76);
				}
				wsprintfW(wszTemp,L"%d",pwinC->minions);
				x+=drawBgText(wszTemp,x,y,0,0x76)+3;
			}
			if (pwinC->quantity) {
				wsprintfW(wszTemp,L"%d",pwinC->quantity);
				x+=drawBgText(wszTemp,x,y,6,0x68)+3;
			}
			y=106;
			if (pwinC->minionsHp) {
				int rw=46*pwinC->minionsHp/100;if (rw>46) rw=46;
				d2gfx_DrawRectangle(xpos,y,xpos+rw,y+5,0x76,5);
				d2gfx_DrawRectangle(xpos+rw,y,xpos+46,y+5,0,5);
			}
			if (pwinC->switchkey[0]) {
				wsprintfW( wszTemp, L"%hs",pwinC->switchkey);
				d2win_DrawText(wszTemp, xpos+46-GetTextWidth(wszTemp), 57, 4, 1);
			}
		}
	}
	if ( tShowPartyPosition.isOn ){
		wsprintfW( wszTemp, L"%d" , pRosterUnit->dwLevelNo );
		DrawCenterText(1, wszTemp ,	xpos+20 ,	15 ,	4 ,1,1);
	}
	partyHeadCachePos++;
}
/*
6FB0BA99 - 56                    - push esi
6FB0BA9A - 8B F0                 - mov esi,eax
6FB0BA9C - 85 F6                 - test esi,esi
6FB0BA9E - 57                    - push edi
6FB0BA9F - C7 44 24 14 00000000  - mov [esp+14],00000000 { 0 }
*/
void __declspec(naked) StartDrawPartyHeadPatch_ASM() {
	__asm {
		mov partyHeadCachePos,0
		mov dword ptr [esp+0x18],0
		ret
	}
}
void __declspec(naked) DrawPartyHeadPatch_ASM() {
	//[ebx]  xpos
	//eax  ypos
	//[esp+0C]  pRosterUnit
	__asm {
		mov ecx, dword ptr [ebx]
		mov edx, dword ptr [esp+0xC]

		push ebx
		push edi
		push eax
		
		call DrawPartyHeadPatch
		
		pop eax
		pop edi
		pop ebx

		mov ecx, dword ptr [ebx]
		mov edx, dword ptr [esp+0xC]
		ret
	}
}

void __fastcall DrawSkeletonHeadPatch(int type,int x) {
	int percent;
	switch (type) {
		case 4:percent=dwSkeletonHPPercent;
			if (dwEnchantSkeletonCount) {
				wchar_t wszTemp[32];int y=92;int w,h;
				wsprintfW(wszTemp,L"%d",dwEnchantSkeletonCount);d2win_GetTextAreaSize(wszTemp, &w, &h);
				d2gfx_DrawRectangle(x,y-12,x+w,y,0x0A,5);d2win_DrawText(wszTemp,x,y,0,0);
			}
			break;
		case 5:percent=dwSkeletonMageHPPercent;break;
		default:return;
	}
	int rw=46*percent/100;if (rw>46) rw=46;
	d2gfx_DrawRectangle(x,8,x+rw,13,0x76,5);
	d2gfx_DrawRectangle(x+rw,8,x+46,13,0,5);
}
//edx=x ebx=type 4:skeleton 5:mage 6:revive
//6FB0B86B - E8 2618FBFF           - call 6FABD096 { ->->6FA8B080 D2gfx.Ordinal10041 } //draw skeleton image
void __declspec(naked) DrawSkeletonHeadPatch_ASM() {
	__asm {
		pushad
		mov ecx, ebx
		call DrawSkeletonHeadPatch
		popad
		jmp d2gfx_DrawCellFile
	}
}

struct D2ScreenHookStruct
{
	BYTE *show;
	int xpos;
	int ypos;
	wchar_t wszMsg[512];
	BYTE color;
};
static D2ScreenHookStruct D2ScreenHook[10];

void UpdateExp() {
	if( fInGame==FALSE )return;
	if (!d2client_pAutomapOn[0]) return;

	char temp_s[512];
	UnitAny *ptPlayer = PLAYER;
	
	int exp = d2common_GetUnitStat(ptPlayer, STAT_EXP, 0);
	DWORD lvl = d2common_GetUnitStat(ptPlayer, STAT_LEVEL, 0);
	DWORD player_class = ptPlayer->dwTxtFileNo;

	int nextlvlexp = d2common_GetMinExpToLevel(player_class, lvl);
	int currentlvlexp = 0;
	if (lvl>1)
	{
		currentlvlexp = d2common_GetMinExpToLevel(player_class, lvl-1);
	}
	
	int totalexpneedtoupgrade = nextlvlexp - currentlvlexp;
	int currentlvlgainedexp = exp - currentlvlexp;
	int currentgamegainedexp = exp - currentgameexp;
	if (currentgamegainedexp > currentlvlgainedexp) currentgamegainedexp = currentlvlgainedexp;

	if (tShowExp.value32==2)
	{
		sprintf(temp_s,"+Exp:[%.2f%%%%]%d", ((float)(currentgamegainedexp)*100)/((float)totalexpneedtoupgrade),currentgamegainedexp);
		wsprintfW2(D2ScreenHook[0].wszMsg, temp_s);
		sprintf(temp_s,"Lv%d:[%.2f%%%%]%d / %d", lvl,((float)currentlvlgainedexp*100)/((float)totalexpneedtoupgrade), currentlvlgainedexp,totalexpneedtoupgrade);
	}
	else
	{
		if(currentgamegainedexp<0){
			sprintf(temp_s,"Lv%d:[%.2f%%%%][%.2f%%%%]%d", lvl,((float)currentlvlgainedexp*100)/((float)totalexpneedtoupgrade),((float)(currentgamegainedexp)*100)/((float)totalexpneedtoupgrade), currentgamegainedexp);
		}else{
			sprintf(temp_s,"Lv%d:[%.2f%%%%][+%.2f%%%%]%d", lvl,((float)currentlvlgainedexp*100)/((float)totalexpneedtoupgrade),((float)(currentgamegainedexp)*100)/((float)totalexpneedtoupgrade), currentgamegainedexp);
		}
	}
	wsprintfW2(D2ScreenHook[1].wszMsg, temp_s);
}

void InitExp(){
	currentgameexp = d2common_GetUnitStat(PLAYER, STAT_EXP, 0);
	UpdateExp();
}

void __declspec(naked) UpdateExp_ASM()
{
		__asm {
		cmp [tShowExp.isOn],0
		je org
		call UpdateExp
org:
		pop edi
		pop esi
		pop ebp
		pop ebx
		ret 
	}
}

void info_FixValues() {
	for (int i=0 ;i<10 ;i++) D2ScreenHook[i].show = 0 ;
	if ( tShowExp.value32==2 ){
		D2ScreenHook[0].show = (BYTE *)&tShowExp.isOn ;
		D2ScreenHook[0].xpos = 120;
		D2ScreenHook[0].ypos = -65;
		D2ScreenHook[0].color = 0;
	}
	D2ScreenHook[1].show = (BYTE *)&tShowExp.isOn ;
	D2ScreenHook[1].xpos = 120;
	D2ScreenHook[1].ypos = -50;
	D2ScreenHook[1].color = 0;
	D2ScreenHook[2].show = (BYTE *)&tShowPing.isOn ;
	D2ScreenHook[2].xpos = -70;
	D2ScreenHook[2].ypos = -30;
	D2ScreenHook[3].color = 2;
	D2ScreenHook[3].show = (BYTE *)&tShowPing.isOn ;
	D2ScreenHook[3].xpos = 65;
	D2ScreenHook[3].ypos = -30;
	D2ScreenHook[3].color = 2;
}

void DrawMonitorInfo();
void DrawClientPatch() {
	static int needUpdateExp=0;
	if ( tShowPing.isOn ){
		if (PING >= 700)
			D2ScreenHook[2].color = 1;
		else
			D2ScreenHook[2].color = 2;
		wsprintfW2(D2ScreenHook[2].wszMsg, "%d",PING);
		D2ScreenHook[3].color = 2;
		wsprintfW2(D2ScreenHook[3].wszMsg, "%d",FPS);
	}

	if (d2client_pAutomapOn[0]) {
		if (needUpdateExp) {UpdateExp();needUpdateExp=0;}
		for (int i=0 ;i<10 ;i++){
			if ( D2ScreenHook[i].show && *( D2ScreenHook[i].show )  ){
				int xpos = D2ScreenHook[i].xpos ;
				int ypos = D2ScreenHook[i].ypos ;
				if( xpos<0 ) xpos = xpos+SCREENSIZE.x;
				if( ypos<0 ) ypos = ypos+SCREENSIZE.y;
				d2win_DrawText( D2ScreenHook[i].wszMsg, xpos, ypos, D2ScreenHook[i].color, 0 );
			}
		}
	} else needUpdateExp=1;
	DrawCenterAlertMsg();
	DrawMonitorInfo();
}

extern int fSkipPainting;
void __declspec(naked) DrawClientPatch_ASM()
{
	__asm {
		cmp fSkipPainting,0
		jne skip
		call d2client_DrawClient;
		push ebx
		call DrawClientPatch
		pop ebx
skip:
		ret;
	}
}

char *szOrbPattern = "%d/%d (%d%%)";
DWORD __stdcall ShowLifeWithMyPattern(DWORD callBack,int min,int max) {
	wchar_t wszTemp[64];
	int iPercent =  100*min/max ;
	wsprintfW2(wszTemp	,szOrbPattern , min,	max,	iPercent);
	drawBgTextMiddle(wszTemp,75,SCREENSIZE.y-95,0,0x10);
	return callBack;
}
DWORD __stdcall ShowManaWithMyPattern(DWORD callBack,int min,int max) {
	wchar_t wszTemp[64];
	int iPercent =  100*min/max ;
	wsprintfW2(wszTemp	,szOrbPattern , min,	max,	iPercent);
	drawBgTextMiddle(wszTemp,SCREENSIZE.x-65,SCREENSIZE.y-95,0,0x10);
	return callBack;
}
void __declspec(naked) ShowLifePatch_ASM() {
	__asm {
		cmp [tPermShowOrb.value32],2
		je showme
		mov ecx, 0x00001045
		ret
showme:
		call ShowLifeWithMyPattern
		push eax
		add dword ptr [esp], 0x4E
		ret
	}
}
void __declspec(naked) ShowManaPatch_ASM() {
	__asm {
		cmp [tPermShowOrb.value32],2
		je showme
		mov ecx, 0x00001046
		ret
showme:
		call ShowManaWithMyPattern
		push eax
		add dword ptr [esp], 0x5B
		ret
	}
}
int SetLifeManaShow(){
	if ( tPermShowOrb.isOn ){
		*d2client_pShowLifeStr = 1;
		*d2client_pShowManaStr = 1;
	}else{
		*d2client_pShowLifeStr = 0;
		*d2client_pShowManaStr = 0;
	}
	return 0;
}

struct ViewUnitInfo {
	DWORD dwUiNo;
	DWORD dwUnitId;
	DWORD dwUnitType;
	UnitAny *pUnit;
};

ViewUnitInfo viewStatUnit = {2,	(DWORD)-1,	(DWORD)-1 , NULL};
ViewUnitInfo viewEquUnit =	{1,	(DWORD)-1,	(DWORD)-1 , NULL};

UnitAny*  GetViewEquUnit(){
	return viewEquUnit.pUnit?viewEquUnit.pUnit:PLAYER;
}

UnitAny* GetViewStatUnit(){
	return viewStatUnit.pUnit?viewStatUnit.pUnit:PLAYER;
}

void SetViewUnit(){
	fViewingTargetUnit=0;
	if ( viewEquUnit.pUnit != NULL ){
		if ( d2client_CheckUiStatus(UIVAR_INVENTORY) ){
			UnitAny *pUnit = d2client_GetUnitFromId(viewEquUnit.dwUnitId, viewEquUnit.dwUnitType);
			viewEquUnit.pUnit =  ( (pUnit&&pUnit->pInventory)?pUnit:PLAYER);
			fViewingTargetUnit=1;
		}else{
			viewEquUnit.pUnit = NULL;
		}
	}
	if ( viewStatUnit.pUnit != NULL ){
		if ( d2client_CheckUiStatus(UIVAR_STATS) ){
			UnitAny *pUnit = d2client_GetUnitFromId(viewStatUnit.dwUnitId, viewStatUnit.dwUnitType);
			viewStatUnit.pUnit =  ( (pUnit&&pUnit->pInventory)?pUnit:PLAYER);
			fViewingTargetUnit=1;
		}else{
			viewStatUnit.pUnit = NULL;
		}
	}
}

int ViewUnit(int nType) {
	UnitAny *pUnit = d2client_GetSelectedUnit();

	if (!pUnit || !CheckUnitSelect(pUnit)) return -1;
	ViewUnitInfo *pViewUnitInfo ; 
	if( nType==1 ){
		pViewUnitInfo = &viewEquUnit ; 
	}else if( nType==2 ){
		pViewUnitInfo = &viewStatUnit ; 
	}
	pViewUnitInfo->dwUnitId = pUnit->dwUnitId;
	pViewUnitInfo->dwUnitType = pUnit->dwUnitType;
	pViewUnitInfo->pUnit = pUnit;
	fViewingTargetUnit=1;

	if ( !d2client_CheckUiStatus(pViewUnitInfo->dwUiNo)){
		d2client_SetUiStatus(pViewUnitInfo->dwUiNo, 0, 1);
	}
	return 0;

}


void __declspec(naked) GetViewingStatsPlayerPatch1_ASM()
{
	__asm {
		push eax
		call GetViewStatUnit
		cmp ebx,eax
		pop eax
		ret;
	}
}

void __declspec(naked) GetViewingStatsPlayerPatch2_ASM()
{
	__asm {
		push eax
		call GetViewStatUnit
		mov esi,eax
		pop eax
		ret;
	}
}

void __declspec(naked) GetUnitDrawEquipItemPatch_ASM()
{
	__asm {
		push eax;
		call GetViewEquUnit;
		mov esi, eax;
		pop eax;
		ret;
	}
}

void __declspec(naked) GetUnitDrawItemDescPath_ASM()
{
	__asm {
		push eax;
		call GetViewEquUnit;
		mov ebx, eax;
		pop eax;
		ret;
	}
}


void __declspec(naked) GetUnitTestHasItem_ASM()
{
	__asm {
//edi = default unit
		push eax
		mov eax, d2client_pPlayerUnit
		mov eax, [eax]
		cmp edi, eax
		jnz viewinvold
		call GetViewEquUnit
		mov edi,eax
viewinvold:
		pop eax
//original code
		test eax,eax
		mov ecx, [edi+0x60]
		ret
	}
}

wchar_t* __fastcall GetUnitText(wchar_t* str, UnitAny* pUnit) {
	*str = 0;
	if (pUnit->dwUnitType == UNITNO_PLAYER){
		wsprintfW(str, L"%hs", pUnit->pPlayerData->szName);
		return str;
	}else{
		return wcscpy (str,	d2lang_GetLocaleText(pUnit->pMonsterData->pMonsterTxt->wLocaleTxtNo));
	}

}

UnitAny *ViewDrawInvItemPatch()
{
	UnitAny *pUnit = GetViewEquUnit();
	if (pUnit != PLAYER) {
		wchar_t temp[80];
		wchar_t *p = GetUnitText(temp, pUnit);
		DrawCenterText(3, p, 484+DRAWOFFSET.x, 327-DRAWOFFSET.y, 8);
	}

	return pUnit;
}
void __declspec(naked) GetUnitDrawInvItemPatch_ASM(){
	__asm {
		call ViewDrawInvItemPatch;
		ret;
	}

}

void __declspec(naked) GetUnitProtectClickEquipPatch_ASM(){
	__asm {
		call GetViewEquUnit;
		mov ebx ,d2client_pPlayerUnit
		mov ebx ,[ebx]
		cmp ebx, eax

		je oldcode

		push 0x13
		push ebx
		call d2client_ItemProtect
		add dword ptr [esp], 0xE1
		ret 4
oldcode:
		push [esp+4]
		call d2common_GetCursorItem
		ret 4
	}

}

void __declspec(naked) GetUnitProtectClickInvPatch_ASM(){
	__asm {
		push eax
		call GetViewEquUnit;
		mov ecx ,d2client_pPlayerUnit
		mov ecx ,[ecx]
		cmp ecx, eax
		pop eax

		je oldcode

		push 0x13
		push ecx
		call d2client_ItemProtect
		add dword ptr [esp], 0x176
		ret

oldcode:
		mov ecx, dword ptr [ebp+0x08]
		mov edx, dword ptr [esp+0x30]
		ret
	}

}

struct PlayerStat {
	DWORD dwStatNo;
	char* szDesc;
	char* szSuffix;
};
static const int PAGESHOWNUMS = 7;
static int nCurrentPage = 0 ; 

void DrawDefaultFontText(wchar_t *wStr, int xpos, int ypos, DWORD dwColor, int div = 1 , DWORD dwAlign = 0 );
void DrawDefaultFontText(wchar_t *wStr, int xpos, int ypos, DWORD dwColor, int div , DWORD dwAlign) {
	d2win_DrawText(wStr, xpos-(d2win_GetTextPixelLen(wStr)>>div), ypos, dwColor, dwAlign);//dwAlign:多行时对齐有用 1居中 0 靠左
}
int SwitchStatPage(int nStep){
	if ( nStep == 0 ){
		nCurrentPage = 0 ;
	}else{
		nCurrentPage = nCurrentPage + nStep ;
	}
	if( nCurrentPage<0 )nCurrentPage = 2;
	if( nCurrentPage>2 )nCurrentPage = 0;
	return 0;
}

void DrawUnitStat(int align,int unitstat,int xpos,int ypos,char *szFormat,char* szSuffix,
	char* szDesc,DWORD dwColor=0 , int desclen =0, int vallen=0) {
	wchar_t wszTemp[256];
	xpos = xpos + DRAWOFFSET.x;
	ypos = ypos - DRAWOFFSET.y;
	if (*szDesc){
		wsprintfW(wszTemp, L"%hs", szDesc);
		d2win_DrawText(wszTemp, xpos, ypos, dwColor, 0);
		xpos += desclen ;
		d2win_DrawText(L":", xpos, ypos, dwColor, 0);
		xpos += vallen;
	}
	wsprintfW2(wszTemp, szFormat ,unitstat,szSuffix);
	DrawDefaultFontText(wszTemp,	xpos ,	ypos ,	dwColor , align);
}

void __fastcall ViewUnitStatPatch(UnitAny *pUnit){
	
	/*
		这几个固定位置显示
		{STAT_MAGIC_FIND,			"MF" ,	"%"},
		{STAT_EXTRA_GOLD,			"EG" ,	"%"},
		{STAT_PLR,					"PLR",	 ""},
		{STAT_DAMAGE_REDUCED,		"DR" ,	"%"},
	*/

	static PlayerStat aPlayerStats[] = {
		{STAT_HPREGEN,				"REP" ,	" "},	
		{STAT_FCR,					"FCR" , "%"},
		{STAT_FHR,					"FHR" , "%"},
		{STAT_FRW,					"FRW" , "%"},
		{STAT_IAS,					"IAS" , "%"},
		{STAT_CB,					"CB" ,	"%"},
		{STAT_OW,					"OW" ,	"%"},

		{STAT_FBR,					"FBR" , "%"},
		{STAT_DS,					"DS" ,	"%"},
		{STAT_LIFE_LEECH,			"LL" ,	"%"},
		{STAT_MANA_LEECH,			"LM" ,	"%"},
		{STAT_DTM,					"DTM",	"%"},
		{STAT_MAGIC_DAMAGE_REDUCED, "MDR",	" "},
		{STAT_EK,					"EK",	" "},

		{STAT_FIRE_ABSORB,			"FA" ,	"%"},
		{STAT_LIGHTING_ABSORB,		"LA" ,	"%"},
		{STAT_COLD_ABSORB,			"CA" ,	"%"}
	}; // size 17 ;
	
	DWORD dwOldFone = d2win_SetTextFont(1);

	BOOL blStatButton = d2common_GetUnitStat(pUnit, STAT_STATPOINTSLEFT, 0);

	int xPos = 20 ;
	int yPos = blStatButton ? 330:345 ;
	char *szFormat = "%3d%s";
	for( int i = nCurrentPage*PAGESHOWNUMS,j=0 ; i< _ARRAYSIZE(aPlayerStats) && j<PAGESHOWNUMS ; i++,j++ ){

		if( j==2 && blStatButton ) yPos+=25;
		int unitstat = d2common_GetUnitStat(pUnit, aPlayerStats[i].dwStatNo, 0) ;
		DrawUnitStat(0, unitstat , xPos, yPos+j*13 ,szFormat , aPlayerStats[i].szSuffix , aPlayerStats[i].szDesc , 0 , 33 , 45 );
	}
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_MAGIC_FIND, 0)  ,	 xPos,	196 , "%4d%s" , "%" , "MF" , 2 , 25 , 54 );
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_EXTRA_GOLD, 0)  ,	 xPos,	213 , "%4d%s" , "%" , "EG" , 4 , 25 , 54 );
	DrawUnitStat(1, d2common_GetUnitStat(pUnit, STAT_DAMAGE_REDUCED, 0) , 175,	330 , "%d%s" , "%" , "DR" , 0 , 25 , 25 );
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_PLR, 0)  ,		   250,	330 , "%d"	 ,  "" , "PLR", 0 , 30 , 30 );
	xPos = 187;
	yPos = 350;
	DrawUnitStat(1, GetUnitResist(pUnit,STAT_FIRE_RESIST),    xPos,	yPos,      "%d" , "" ,"", 1);
	DrawUnitStat(1, GetUnitResist(pUnit,STAT_COLD_RESIST),    xPos,	yPos+23,   "%d" , "" ,"", 3);
	DrawUnitStat(1, GetUnitResist(pUnit,STAT_LIGHTING_RESIST),xPos,	yPos+23*2, "%d" , "" ,"", 9);
	DrawUnitStat(1, GetUnitResist(pUnit,STAT_POSION_RESIST),  xPos,	yPos+23*3, "%d" , "" ,"",12);
	if ( tShowBaseStat.isOn ){
		xPos = blStatButton ? 95 : 140 ;
		yPos = blStatButton ? 118 : 100 ;
		DrawUnitStat(1, d2common_GetUnitBaseStat( pUnit,STAT_STRENGTH ,  0 ),    xPos,	yPos, "[%d]" , "" ,"");
		//+62
		yPos = yPos + 62 ;
		DrawUnitStat(1, d2common_GetUnitBaseStat( pUnit,STAT_DEXTERITY , 0 ),    xPos,	yPos, "[%d]" , "" ,"");
		//+85
		yPos = yPos + 85 ;
		DrawUnitStat(1, d2common_GetUnitBaseStat( pUnit,STAT_VITALITY ,  0 ),    xPos,	yPos, "[%d]" , "" ,"");
		
		//+25  +63
		yPos = yPos + (blStatButton? 25 : 63) ;
		DrawUnitStat(1, d2common_GetUnitBaseStat( pUnit,STAT_ENERGY ,    0 ),    xPos,	yPos, "[%d]" , "" ,"");
	}
	
	d2win_SetTextFont(dwOldFone);
}

void __declspec(naked) ViewUnitStatPatch_ASM()
{
	//esp+1C  pUnit
	__asm {
		mov ecx, [esp+0x1C]
		call ViewUnitStatPatch
//org
		pop edi
		pop esi
		pop ebp
		pop ebx
		add esp, 0x370
		ret
	}
}

void __fastcall ViewPetStatPatch(UnitAny *pUnit){

	int xPos = 10 ;
	int yPos = 395 ;
	DWORD dwColor = 0 ;

	DWORD dwOldFone = d2win_SetTextFont(1);

	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_MAGIC_FIND, 0)  ,	 xPos,	yPos ,    "%3d%s" , "%" , "MF" , 2 , 22 , 43 );
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_EXTRA_GOLD, 0)  ,	 xPos,	yPos+15 , "%3d%s" , "%" , "EG" , 4 , 22 , 43 );
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_DAMAGE_REDUCED, 0), xPos,	yPos+30 , "%3d%s" , "%" , "DR" ,  dwColor , 22 , 43 );

	xPos = xPos + 95 ; 
	dwColor = 0 ;
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_FHR, 0)  ,	 xPos,	yPos ,    "%2d%s" , "%" , "FHR" , dwColor , 26 , 39 );
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_IAS, 0)  ,	 xPos,	yPos+15 , "%2d%s" , "%" , "IAS" , dwColor , 26 , 39 );
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_CB, 0)   ,	 xPos,	yPos+30 , "%2d%s" , "%" , "CB" ,  dwColor , 26 , 39 );
	
	xPos = xPos + 90 ; 
	dwColor = 0 ;
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_DS, 0)  ,	 xPos,	yPos ,    "%2d%s" , "%" , "DS" , dwColor , 25 , 39 );
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_OW, 0)  ,	 xPos,	yPos+15 , "%2d%s" , "%" , "OW" , dwColor , 25 , 39 );
	DrawUnitStat(0, d2common_GetUnitStat(pUnit, STAT_LIFE_LEECH, 0),xPos,	yPos+30 , "%2d%s" , "%" , "LL" ,  dwColor , 25 , 39 );
	
	d2win_SetTextFont(dwOldFone);

}
void __declspec(naked) ViewPetStatPatch_ASM()
{
	//ebx = pUnit
	__asm {
		push ecx
		push ebx
		mov ecx , ebx
		call ViewPetStatPatch;
		pop ebx
		pop ecx
		add ecx, 0x00000110
		ret;
	}
}

//d2client_27BF7: 8D 04 B6           lea eax, [esi+esi*4]
//d2client_27BFA: C1 E0 04           shl eax, 4
//esi:hp ecx:maxhp
void __declspec(naked) HpOrbPatch_ASM() {
	__asm {
		cmp ecx, 0x200000
		jb done
		mov eax, dwPlayerMaxHP
		test eax, eax
		jz done
		mov esi, dwPlayerHP
		mov ecx, dwPlayerMaxHP
done:
		lea eax,[esi+esi*4]
		shl eax,4
		ret
	}
}
