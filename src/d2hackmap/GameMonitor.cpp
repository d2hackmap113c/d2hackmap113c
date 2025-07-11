#include "stdafx.h"
#include "header.h"
#include "bitstream.h"
#include "multi.h"
#include "auto.h"

extern ToggleVar tDrawInvItemInfo;
extern int dwCpuUser,dwCpuKernel,dwLoopFPS,dwDrawFPS;
extern ToggleVar tMinimapDebugToggle;
extern BYTE nMonsterMinionColor,nMonsterChampColor,nSuperUniqueColor;
int dwRecheckSelfItemMs=0;
UnitAny *leftWeapon=0,*rightWeapon=0;
int dwTownPortalCount=0,dwIdentifyPortalCount=0,fUsingBow=0,fUsingCrossBow=0,fUsingThrow=0;
int dwHPotionCount=0,dwMPotionCount=0,dwRPotionCount=0,dwArrowCount=0,dwCArrowCount=0,dwThrowCount=0;
//--- simple item stack support for some server
int simpleItemStackTxt=660,simpleItemStackIdx=2153,simpleItemStackStatId=359;
//--- multiple stash page support for some server
int changeStashPageCmd=0x9D;
int changeStashPageCmdOffset=1;
int changeStashPageCmdValue=0x2c;
int changeStashPageValueOffset=0x15;
int curStashPage=0,nStashPages=0;
StashPage *stashPages[MAX_STASH_PAGES]={0};
static int dwDrawMultiPageStashXY[2]={80,-95};
static int dwSelfNameColor[3]={6,0x84,0x10};
static int dwLocalNameColor[3]={6,0x84,0x10};
static int dwPartyNameColor[3]={6,0x20,0x5b};
static int dwNeutralNameColor[3]={0,0x5b,0x10};
static int dwHostileNameColor[3]={6,0x62,0x6f};
int bossX=0,bossY=0,bossHps[3],bossId=0,nBoss=0;
#define LAST_EXP_N 8
static int lastExpChange[LAST_EXP_N],expTail=0,expN=0,expId=1;
void playerAddExp(int exp) {
	if (expTail<0||expTail>=LAST_EXP_N) expTail=0;if (expN<0||expN>LAST_EXP_N) expN=0;
	lastExpChange[expTail++]=exp;if (expN<LAST_EXP_N) expN++;expId++;
}

void ShowGameCount();
ToggleVar tCountDown={TOGGLEVAR_ONOFF,0,-1,1,"Count Down"};
ToggleVar tDrawMultiPageStashCount={TOGGLEVAR_ONOFF,0,-1,1,"Draw Multi Page Stash Count"};
int dwGameMonitorX=-10,dwGameMonitorY=-110;
int dwCountDownFontSize=			3;
int dwCountDownGap=			25;
int dwCountDownFlashSecond=			30;
int fMonitorQuantity=			0;
int fMonitorDurability=			0;
ToggleVar tKillCount={				TOGGLEVAR_ONOFF,	0,	-1,		1, "KillCount",	};
ToggleVar tGetHitCount={			TOGGLEVAR_ONOFF,	0,	-1,		1, "GitHitCount",	};
int dwKillSum=0;
int dwGameKills=0;
int dwLastKills=0;
int dwGetHitSum=0;
int dwGetHits=0;
int dwOrgMode=0;
char targetNameColor=0;
ToggleVar tNecAutomapCorpsesToggle={TOGGLEVAR_ONOFF,0,-1,1,"NecAutomapCorpsesToggle"};
ToggleVar tDrawBossHpToggle={TOGGLEVAR_ONOFF,0,-1,1,"DrawBossHpToggle"};
int bossLineColor=0x62;
ToggleVar tDrawPlayerNameToggle={TOGGLEVAR_ONOFF,0,-1,1,"DrawPlayerName",0,0,0,2};
ToggleVar tAnalyzeMonsterExp={TOGGLEVAR_ONOFF,0,-1,1,"AnalyzeMonsterExp",0,0,0,2};
ToggleVar tLogMonsterExp={TOGGLEVAR_ONOFF,0,-1,1,"LogMonsterExp",0,0,0,2};
int dwDrawPlayerNameDy=66;
int HasCowKing=1;
ToggleVar tShowPlayerCastPath={TOGGLEVAR_ONOFF,0,-1,1,"Player Cast Path"};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY,"ShowPlayerCastPath",&tShowPlayerCastPath},
	{CONFIG_VAR_TYPE_INT,"HasCowKing",&HasCowKing,4},
	{CONFIG_VAR_TYPE_INT,"TargetNameColor",&targetNameColor,1},
	{CONFIG_VAR_TYPE_INT,"MonitorQuantity",&fMonitorQuantity,  4},
	{CONFIG_VAR_TYPE_INT,"MonitorDurability",&fMonitorDurability,  4},
	{CONFIG_VAR_TYPE_KEY,"CountDown",&tCountDown},
	{CONFIG_VAR_TYPE_INT,"CountDownFontSize",&dwCountDownFontSize,4},
	{CONFIG_VAR_TYPE_INT,"GameMonitorX",&dwGameMonitorX,4},
	{CONFIG_VAR_TYPE_INT,"GameMonitorY",&dwGameMonitorY,4},
	{CONFIG_VAR_TYPE_INT,"CountDownGap",&dwCountDownGap,4},
	{CONFIG_VAR_TYPE_INT,"CountDownFlashSecond",&dwCountDownFlashSecond,4},
	{CONFIG_VAR_TYPE_KEY,"KillCountToggle",&tKillCount},
	{CONFIG_VAR_TYPE_KEY,"GetHitCountToggle",&tGetHitCount},
	{CONFIG_VAR_TYPE_KEY,"DrawMultiPageStashCount",&tDrawMultiPageStashCount},
	{CONFIG_VAR_TYPE_KEY,"AnalyzeMonsterExp",&tAnalyzeMonsterExp},
	{CONFIG_VAR_TYPE_KEY,"LogMonsterExp",&tLogMonsterExp},
  {CONFIG_VAR_TYPE_INT_ARRAY1,"DrawMultiPageStashXY",&dwDrawMultiPageStashXY,2,{0}},
	{CONFIG_VAR_TYPE_INT,"SimpleItemStackTxt",&simpleItemStackTxt,4},
	{CONFIG_VAR_TYPE_INT,"SimpleItemStackIdx",&simpleItemStackIdx,4},
	{CONFIG_VAR_TYPE_INT,"SimpleItemStackStatId",&simpleItemStackStatId,4},
	{CONFIG_VAR_TYPE_INT,"ChangeStashPageCmd",&changeStashPageCmd,4},
	{CONFIG_VAR_TYPE_INT,"ChangeStashPageCmdOffset",&changeStashPageCmdOffset,4},
	{CONFIG_VAR_TYPE_INT,"ChangeStashPageCmdValue",&changeStashPageCmdValue,4},
	{CONFIG_VAR_TYPE_INT,"ChangeStashPageValueOffset",&changeStashPageValueOffset,4},
  {CONFIG_VAR_TYPE_KEY,"NecAutomapCorpsesToggle",&tNecAutomapCorpsesToggle},
  {CONFIG_VAR_TYPE_KEY,"DrawBossHpToggle",&tDrawBossHpToggle},
	{CONFIG_VAR_TYPE_INT,"DrawBossLineColor",&bossLineColor,1},
  {CONFIG_VAR_TYPE_KEY,"DrawPlayerNameToggle",&tDrawPlayerNameToggle},
	{CONFIG_VAR_TYPE_INT,"DrawPlayerNameDy",&dwDrawPlayerNameDy,4},
	{CONFIG_VAR_TYPE_INT_ARRAY1, "SelfNameColor",&dwSelfNameColor,3,{0}},
	{CONFIG_VAR_TYPE_INT_ARRAY1, "LocalNameColor",&dwLocalNameColor,3,{0}},
	{CONFIG_VAR_TYPE_INT_ARRAY1, "PartyNameColor",&dwPartyNameColor,3,{0}},
	{CONFIG_VAR_TYPE_INT_ARRAY1, "NeutralNameColor",&dwNeutralNameColor,3,{0}},
	{CONFIG_VAR_TYPE_INT_ARRAY1, "HostileNameColor",&dwHostileNameColor,3,{0}},
};
void gamemonitor_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void gamemonitor_initConfigVars() {
	dwGameMonitorX=-10;
	dwGameMonitorY=-110;
	dwCountDownFontSize=3;
	dwCountDownGap=25;
	dwCountDownFlashSecond=30;
}

extern int dwNecInfoExpireMs;

int guessEnchantLevel();
int guessBOLevel(int debug);
void drawNpcTradeInfo(int color);

void hex(FILE *fp,int addr,void *buf,int n);
int fState100HP=0,fState106ManaMs=0;
int dwOakSageLvl;
static int fBC=0;
int dwGuessBOMs=0;
int dwGuessEnchantMs=0;
struct CountDown {
	const char *name;
	int skill;
	int addS,stepS,startMs,endMs;
	int active,lv,selfskill;
};
//werewolf: time 40s
//Lycanthropy: time+=20+lvl*20
int fWerewolf,fWerebear;
static struct CountDown cd_QH={"QH",52,120,24,0,0,0}; //sor fire enchant
static struct CountDown cd_BO={"BO",149,30,10,0,0,0}; //bar warcries battleorder
static struct CountDown cd_HS={"HS",117,5,25,0,0,0};//pal combat holyshield
static struct CountDown cd_werewolf={"Wolf",223,60,20,0,0,0};//dru shape werewolf
static struct CountDown cd_werebear={"Bear",228,60,20,0,0,0};//dru shape werebear
static struct CountDown cd_Fade={"Fade",267,108,12,0,0,0};//asn shadow fade
static struct CountDown cd_Burst={"Burst",258,108,12,0,0,0};//asn shadow burst of speed
static struct CountDown cd_Cloak={"Cloak",264,8,1,0,0,0};//asn shadow Cloak of Shadows
static struct CountDown cd_KD={"KD",0,0,0,0,0,0};//KD countdown
static struct CountDown cd_KB={"KB",0,0,0,0,0,0};//KB countdown
struct CountDown *countDowns[]={&cd_QH,&cd_BO,&cd_HS,
	&cd_werewolf,&cd_werebear,
	&cd_Fade,&cd_Burst,&cd_Cloak,&cd_KD,&cd_KB};

unsigned char beltCount[4],beltType[4];
int dwNeedPotionType; //bit 1:HP 2:Mana 3:Rejuvenation
int beltLayers;
int dwNextHPotionId,dwNextMPotionId;
static int minHPotionType,minMPotionType;
static int throwWeaponIdx;
int dwThrowWeaponId,dwNextStackingId,dwNextStackX,dwNextStackY,dwNextStackLoc;

static void checkWeapon(UnitAny *pUnit) {
	if (pUnit->dwUnitType!=UNITNO_ITEM) return;
	ItemTxt *pItemTxt=d2common_GetItemTxt(pUnit->dwTxtFileNo);
	switch (pItemTxt->nType) {
		case 27:case 85:fUsingBow=1;break;
		case 35:fUsingCrossBow=1;break;
		case 38://Throwing Potion
		case 42:case 43://Throwing
		case 44:case 87://Javelins
			if (pUnit->pItemData->dwQuality!=ITEM_QUALITY_UNIQUE) {
				fUsingThrow=1;dwThrowWeaponId=pUnit->dwUnitId;throwWeaponIdx=GetItemIndex(pUnit->dwTxtFileNo)+1;break;
			}
	}
}
void usePotion(int mana) {
	int *id=mana?&dwNextMPotionId:&dwNextHPotionId;
	if (!(*id)) return;
	UnitAny *pUnit=d2client_GetUnitFromId(*id,UNITNO_ITEM);
	if (!pUnit) return;
	BYTE packet[13];
	if (pUnit->pItemData->nLocation==2) { //belt
		packet[0]=0x26;*(DWORD*)&packet[1]=*id;
		*(DWORD*)&packet[5]=0;*(DWORD*)&packet[9]=0;
	} else {
		packet[0]=0x20;*(DWORD*)&packet[1]=*id;
		*(DWORD*)&packet[5]=PLAYER->pMonPath->wUnitX;*(DWORD*)&packet[9]=PLAYER->pMonPath->wUnitY;
	}
	SendPacket(packet,13);
	*id=0;
}
//location: 0:bag 1:belt 2:cube
static void checkItem(UnitAny *pUnit,int location) {
	int type=0,lv=0;
	int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
	switch (index) {
		case 2019:dwArrowCount+=d2common_GetUnitStat(pUnit, 70, 0);break;
		case 2021:dwCArrowCount+=d2common_GetUnitStat(pUnit, 70, 0);break;
		case 2008:case 2009:type=3;break;
		case 2080:case 2081:case 2082:case 2083:case 2084:type=1;lv=index-2080;break; //H1:45 H2:90 H3:100 H5:320
		case 2085:case 2086:case 2087:case 2088:case 2089:type=2;lv=index-2085;break; //M1:40 M2:80 M4:300 M5:500
	}
	lv<<=16;
	if (location==0) lv|=(pUnit->pItemPath->unitX<<8)|(pUnit->pItemPath->unitY<<4);
	if (index==throwWeaponIdx) {
		int left=d2common_GetUnitStat(pUnit, 70, 0);
		int usable=left-dwAutoThrowMinQuantity;
		if (usable>0) dwThrowCount+=usable;
		if (!dwNextStackingId&&left+d2common_GetUnitStat(PLAYER, 70, 0)>60) {
			dwNextStackingId=pUnit->dwUnitId;
			dwNextStackX=pUnit->pItemPath->unitX;
			dwNextStackY=pUnit->pItemPath->unitY;
			dwNextStackLoc=pUnit->pItemData->nItemLocation;
		}
	}
	if (location==2) return;
	if (location==1) {
		int x=pUnit->pItemPath->unitX;
		beltCount[x&3]++;
		if (0<=x&&x<=3) beltType[x&3]=type;
		lv|=(1<<15);
	}
	if (!type) return;
	switch (type) {
		case 1:if (lv<minHPotionType) {dwNextHPotionId=pUnit->dwUnitId;minHPotionType=lv;}
			dwHPotionCount++;break;
		case 2:if (lv<minMPotionType) {dwNextMPotionId=pUnit->dwUnitId;minMPotionType=lv;}
			dwMPotionCount++;break;
		case 3:dwRPotionCount++;break;
	}
}
void checkAutoSkill();
void updateCurStashPage() {
	if (nStashPages<0||nStashPages>1000) {LOG("ERROR nStashPages=%d\n",nStashPages);nStashPages=0;}
	if (curStashPage<0||curStashPage>=MAX_STASH_PAGES) return;
	StashPage *p=stashPages[curStashPage];
	if (!p) {
		p=(StashPage *)HeapAlloc(dllHeap,0,sizeof(StashPage));
		stashPages[curStashPage]=p;
	}
	//LOG("updateStashPage1 %d/%d 0x%X\n",curStashPage,nStashPages,p);
	p->n=0;
	for (UnitAny *pUnit = d2common_GetFirstItemInInv(PLAYER->pInventory);pUnit;pUnit = d2common_GetNextItemInInv(pUnit)) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) continue ;
		switch (pUnit->pItemData->nLocation) { 
			case 0:break;//ground
			case 1://cube/stash/inv
				if (pUnit->pItemData->nItemLocation==4) {
					if (p->n<MAX_ITEMS_PER_PAGE) p->items[p->n++]=pUnit;
				}
				break;
			case 2:break; //belt
			case 3:break;//body
		}
	}
	//LOG("updateStashPage %d/%d: n=%d\n",curStashPage,nStashPages,p->n);
}
void recheckSelfItems() {
	static int n=0;
	if (!fInGame) return;
	int ub=fUsingBow,ucb=fUsingCrossBow,ut=fUsingThrow;
	*(int *)beltCount=0;*(int *)beltType=0;dwNeedPotionType=0;
	leftWeapon=NULL;rightWeapon=NULL;fUsingBow=0;fUsingCrossBow=0;fUsingThrow=0;
	beltLayers=1;dwNextHPotionId=0;dwNextMPotionId=0;dwHPotionCount=0;dwMPotionCount=0;dwRPotionCount=0;
	minHPotionType=0x7FFFFFFF;minMPotionType=0x7FFFFFFF;
	throwWeaponIdx=0;dwArrowCount=0;dwCArrowCount=0;dwThrowCount=0;dwNextStackingId=0;
	dwThrowWeaponId=0;
	for (UnitAny *pUnit = d2common_GetFirstItemInInv(PLAYER->pInventory);pUnit;pUnit = d2common_GetNextItemInInv(pUnit)) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) continue;
		if (pUnit->pItemData->nLocation!=3) continue; //not on body
		if (pUnit->pItemData->nItemLocation==255) { //equipped
			switch (pUnit->pItemData->nBodyLocation) {
				case 4:rightWeapon=pUnit;checkWeapon(pUnit);break;
				case 5:leftWeapon=pUnit;checkWeapon(pUnit);break;
				case 8: //belt
					int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
					if (index==1039||index==1040) beltLayers=2;
					else if (index==1041||index==1042) beltLayers=3;
					else beltLayers=4;
					break;
			}
		} 
	}
	//if (nStashPages) updateCurStashPage();
	for (UnitAny *pUnit = d2common_GetFirstItemInInv(PLAYER->pInventory);pUnit;pUnit = d2common_GetNextItemInInv(pUnit)) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) continue ;
		switch (pUnit->pItemData->nLocation) { 
			case 0:break;//ground
			case 1: {//cube/stash/inv
				switch (pUnit->pItemData->nItemLocation) {
					case 0: checkItem(pUnit,0);break;//bag
					//case 3:checkItem(pUnit,2);break;//cube
					case 4: break;//stash
				}
				break;
			}
			case 2:checkItem(pUnit,1);break; //belt
			case 3:break;//body
		}
	}
	for (int i=0;i<4;i++) if (beltCount[i]<beltLayers) {
		if (beltType[i]) dwNeedPotionType|=1<<(beltType[i]-1);
	}
	dwRecheckSelfItemMs=0;
	if (ub!=fUsingBow||ucb!=fUsingCrossBow||ut!=fUsingThrow) checkAutoSkill();
}
void setKDCountDown(int en) {
	cd_KD.active=en;
	cd_KD.endMs=dwCurMs+15000;
}
extern int dwKBSkillSwitch[7][6];
void setKBCountDown(int team,int en) {
	if (!fWinActive&&team&&dwKBSkillSwitch[dwPlayerClass][team-1]) {
		int skillId=dwKBSkillSwitch[dwPlayerClass][team-1];
		if (dwRightSkill!=0&&hasSkill(skillId)&&dwRightSkill!=skillId) selectSkill(1,skillId);
	}
	cd_KB.active=en;
	cd_KB.lv=team;
	cd_KB.endMs=dwCurMs+11750;
}
void setCountDownEndTime(struct CountDown *cd) {
	int time=cd->addS+cd->lv*cd->stepS;
	if (cd->skill==223||cd->skill==228) { //werewolf werebear
		int lv=getSkillLevel(PLAYER,224); //Lycanthropy
		if (lv==0) time=40;
		else time=cd->addS+lv*cd->stepS;
	}
	cd->endMs=cd->startMs+time*1000;
	if (tPacketHandler.isOn) {
		LOG("%s self=%d level=%d time=%d\n",cd->name,cd->selfskill,cd->lv,time);
	}
}
void setupCountdown(struct CountDown *cd) {
	cd->lv=getSkillLevel(PLAYER,cd->skill); 
	cd->startMs=dwCurMs;
	cd->selfskill=1;
	setCountDownEndTime(cd);
}
void onRightSkillMap(int x,int y) {
	switch (dwRightSkill) {
		case 52: setupCountdown(&cd_QH);break;
		case 117: setupCountdown(&cd_HS);break;
		case 149: setupCountdown(&cd_BO);break;
		case 223: setupCountdown(&cd_werewolf);break;
		case 228: setupCountdown(&cd_werebear);break;
		case 258: setupCountdown(&cd_Burst);break;
		case 264: setupCountdown(&cd_Cloak);break;
		case 267: setupCountdown(&cd_Fade);break;
	}
}
void onRightSkillUnit(int type,int id) {
	switch (dwRightSkill) {
		//case 52: setupCountdown(&cd_QH);break;
		case 117: setupCountdown(&cd_HS);break;
		case 149: setupCountdown(&cd_BO);break;
		case 223: setupCountdown(&cd_werewolf);break;
		case 228: setupCountdown(&cd_werebear);break;
		case 258: setupCountdown(&cd_Burst);break;
		case 264: setupCountdown(&cd_Cloak);break;
		case 267: setupCountdown(&cd_Fade);break;
	}
}
void GameMonitorNewGame() {
	dwGetHits=0;dwGameKills=0;dwLastKills=0;
	dwGuessBOMs=0;dwOakSageLvl=0;
	dwGuessEnchantMs=0;
	fBC=0;fWerewolf=0;fWerebear=0;
	leftWeapon=NULL;rightWeapon=NULL;
	for (int i=0;i<_ARRAYSIZE(countDowns);i++) countDowns[i]->active=0;
	expTail=0;expN=0;expId=1;
}
void GameMonitorEndGame() {
	GameMonitorNewGame();
	curStashPage=0;nStashPages=0;
}
extern wchar_t wszNpcTradeInfo[256];

void ShowGameCount() {
	int xpos = SCREENSIZE.x/2 + 70;
	int ypos = SCREENSIZE.y - 50;
	DWORD dwOldFone = d2win_SetTextFont(1);
	if ( tGetHitCount.isOn ){	
		wchar_t wszTemp[512];
		DWORD dwColor = tGetHitCount.value32-1 ;
		wcscpy( wszTemp , L"GetHits:" );
		d2win_DrawText(wszTemp, xpos,  ypos, dwColor, 0);
		wsprintfW(wszTemp ,L"%d" , dwGetHits );
		d2win_DrawText(wszTemp, xpos+65,  ypos, dwColor, 0);
		wsprintfW(wszTemp ,L"Total: %d" , dwGetHitSum );
		d2win_DrawText(wszTemp, xpos+105,  ypos, dwColor , 0);
		ypos = ypos-15;
	}
	if ( tKillCount.isOn ){	
		wchar_t wszTemp[512];
		DWORD dwColor = tKillCount.value32-1 ;
		wcscpy( wszTemp , L"Kills:" );
		d2win_DrawText(wszTemp, xpos+20,  ypos, dwColor, 0);
		wsprintfW(wszTemp ,L"%d" , dwGameKills );
		d2win_DrawText(wszTemp, xpos+65,  ypos, dwColor, 0);
		wsprintfW(wszTemp ,L"Total: %d" , dwKillSum );
		d2win_DrawText(wszTemp, xpos+105,  ypos, dwColor, 0);
	}
	d2win_SetTextFont(dwOldFone);
}
void __fastcall SetKills( DWORD newKills ){
	if ( (int)newKills > (int)dwLastKills ) {
		dwKillSum+=newKills-dwLastKills;
		dwGameKills+=newKills-dwLastKills;
	}
	dwLastKills = newKills;
}
void __declspec(naked) KillCountPatch_ASM() {
	//死亡一次会减少一次杀敌数，所以可能变负数
	//记录下上次值，如果小于上次，认为死亡~
	__asm{
		push esi
		push eax
		movsx ecx, si
		call SetKills
		pop eax
		pop esi
		test esi,0x8000
		ret 
	}
}

void __declspec(naked) UnitModeChangePatch_ASM() {
	__asm{
		cmp edi , 4
		jne org
		mov edx, dword ptr [esi]
		cmp edx, 0
		jne org
		cmp [dwOrgMode] ,0
		je gocount
		mov [dwOrgMode] ,0
		jmp org
gocount:
		mov edx , dword ptr [esi+0x10] //单机会连着两次，但战网模式正常,记录下上一次调用的原值，并在全局循环中重置
		mov [dwOrgMode] , edx 
		add [dwGetHits] , 1
		add [dwGetHitSum] , 1
org:
		mov edx, dword ptr [esi+0xC4]
		ret 
	}
}
void drawSimpleItemStackInfo(UnitAny *pUnit,int xpos,int ypos) {
	wchar_t wbuf[32];
	if (!pUnit->pStatList) return;
	StatList *plist=pUnit->pStatList;
	if (!(plist->dwListFlag&0x80000000)) return;
	if (!plist->sFullStat.pStats) return;
	Stat *stat=&plist->sFullStat;
	int n=stat->wStats;
	if (n>=511) return;
	StatEx *se=stat->pStats;
	xpos+=14;
	for (int i=0;i<n;i++) {
		int id=se[i].wStatId;if (id!=simpleItemStackStatId) continue;
		int count=se[i].dwStatValue;
		int txt=se[i].wParam&0xFFFF;
		int index = GetItemIndex(txt)+1;
		if (2103<=index&&index<=2135) {
			int r=index-2102;
			wsprintfW(wbuf,L"R%d",r);
			drawBgTextMiddle(wbuf,xpos,ypos-14,7,0x10);
		} else if (2050<=index&&index<=2079) {
			static char *gemNames[6]={"紫","黄","蓝","绿","红","白"};
			static int gemColors[6]={11,9,3,2,1,0};
			int t=index-2050;
			wsprintfW(wbuf,L"%hs%d",gemNames[t/5],(t%5)+1);
			drawBgTextMiddle(wbuf,xpos,ypos-14,gemColors[t/5],0x10);
		} else if (2090<=index&&index<=2094) {
			int t=index-2090;
			wsprintfW(wbuf,L"骷%d",t+1);
			drawBgTextMiddle(wbuf,xpos,ypos-14,0,0x10);
		}
		wsprintfW(wbuf,L"%d",count);
		drawBgTextMiddle(wbuf,xpos,ypos,2,0x10);
		break;
	}
}

extern wchar_t wcsLevelTargetName[128];
extern int dwTargetDistance,dwTargetDistanceMs;
extern struct InstalledPatch *networkPatch;
void DrawMonitorInfo(){
	int sw,sh;
	ShowGameCount();
	if (dwGuessBOMs&&dwCurMs>dwGuessBOMs) {
		dwGuessBOMs=0;
		struct CountDown *cd=&cd_BO;
		cd->selfskill=0;
		cd->lv=guessBOLevel(0);
		setCountDownEndTime(cd);
	}
	if (dwGuessEnchantMs&&dwCurMs>dwGuessEnchantMs) {
		dwGuessEnchantMs=0;
		int lv=guessEnchantLevel();
		if (lv) {
			struct CountDown *cd=&cd_QH;
			cd->selfskill=0;cd->lv=lv;
			setCountDownEndTime(cd);
		}
	}
	wchar_t wszTemp[512];
	if (tShowTestInfo.isOn) {
		int pos=wsprintfW(wszTemp, L"(%d,%d)",*d2client_pMouseX,*d2client_pMouseY);
		int drawX=d2client_GetScreenDrawX()+*d2client_pMouseX;
		int drawY=d2client_GetScreenDrawY()+*d2client_pMouseY;
		int unitX=((drawX>>1)+drawY)>>4;
		int unitY=(drawY-(drawX>>1))>>4;
		int dis=getDistanceM256(dwPlayerX-unitX,dwPlayerY-unitY);
		dis=(dis*2/3)>>8;
		pos+=wsprintfW(wszTemp+pos, L" %dyard",dis);
		if (0) 
		pos+=wsprintfW(wszTemp+pos, L" %d,%d,%c",
			d2common_getSkillStatus(PLAYER,PLAYER->pSkill->pLeftSkill),
			d2common_getSkillStatus(PLAYER,PLAYER->pSkill->pRightSkill),
			*d2client_pCurFrame<*d2client_pCastingDelayEndFrame?'d':'-');
		pos+=wsprintfW(wszTemp+pos, L" FPS%d l%d d%d cpu(%d%%,%d%%)", 
			FPS,dwLoopFPS,dwDrawFPS,dwCpuUser,dwCpuKernel);
		for (int i=1;i<=d2winLastId;i++) {
			if (i==dwGameWindowId) continue;
			D2Window *pwin=&d2wins[i];
			if (dwCurMs>pwin->cpuMs) continue;
			pos+=wsprintfW(wszTemp+pos, L" %d:%d%%",i,pwin->cpu); 
		}
		d2win_DrawText(wszTemp, 230 , SCREENSIZE.y-50, 0, 0);
	}
	int xpos = SCREENSIZE.x+dwGameMonitorX;
	int ypos = SCREENSIZE.y+dwGameMonitorY;
	if (*d2client_pUiNewSkillOn) ypos-=45;
	DWORD dwTimer = dwCurMs;
	DWORD dwOldFone = d2win_SetTextFont(8);
	if (wszNpcTradeInfo[0]) {
		static int bms,bcolor=0;
		if (dwCurMs>bms) {bcolor=bcolor==1?2:1;bms=(dwCurMs+256)&0xFFFFFF00;}
		d2win_SetTextFont(3);
		if (!d2client_CheckUiStatus(UIVAR_NPCTRADE)) wszNpcTradeInfo[0]=0;
		else {
			d2win_DrawText(wszNpcTradeInfo, 120 , SCREENSIZE.y-75, bcolor, 0);
			d2win_SetTextFont(8);
			drawNpcTradeInfo(bcolor);
		}
		d2win_SetTextFont(8);
	}
	if (networkPatch) {
		static int bms,bcolor=0;
		if (dwCurMs>bms) {bcolor=bcolor==1?2:1;bms=(dwCurMs+256)&0xFFFFFF00;}
		drawBgTextLeft(L"Network Packet Patch",xpos,ypos,bcolor,0x10);ypos-=15;
	}
	if (0&&nStashPages) {
		int pos=0;
		pos+=wsprintfW(wszTemp+pos, L"Stash: %d/%d",curStashPage+1,nStashPages);
		drawBgTextLeft(wszTemp,xpos,ypos,0,0x10);ypos-=15;
	}
	if (wcsLevelTargetName[0]) {
		int pos=0;
		pos+=wsprintfW(wszTemp+pos, L"%s", wcsLevelTargetName);
		if (dwCurMs<dwTargetDistanceMs) pos+=wsprintfW(wszTemp+pos, L": %d", dwTargetDistance*2/3);
		drawBgTextLeft(wszTemp,xpos,ypos,targetNameColor,0x10);ypos-=15;
	}
	if (tShowTestInfo.isOn) {
		UnitAny *pSelectedUnit = d2client_GetSelectedUnit();
		if (pSelectedUnit) {
			int pos=0,color=0;
			pos+=wsprintfW(wszTemp+pos, L"%d:%d txt%d",  
				pSelectedUnit->dwUnitType,pSelectedUnit->dwUnitId,pSelectedUnit->dwTxtFileNo);
			if (pSelectedUnit->dwUnitType==2) {
				ObjectTxt *pObjTxt=pSelectedUnit->pObjectData->pObjectTxt;
				//d2common_GetObjectTxt(pSelectedUnit->dwTxtFileNo);
				pos+=wsprintfW(wszTemp+pos, L" subclass=%d map=%d",pObjTxt->nSubClass,pObjTxt->nAutoMap);
			}
			pos+=wsprintfW(wszTemp+pos, L" mode=%d", pSelectedUnit->dwMode);
			{
				int x,y;
				switch (pSelectedUnit->dwUnitType) {
					case UNITNO_PLAYER:case UNITNO_MONSTER:case UNITNO_MISSILE:
						x=pSelectedUnit->pMonPath->wUnitX;y=pSelectedUnit->pMonPath->wUnitY;break;
					case UNITNO_OBJECT:case UNITNO_ITEM:case UNITNO_AREA_ENTRANCE:
						x=pSelectedUnit->pItemPath->unitX;y=pSelectedUnit->pItemPath->unitY;break;
				}
				pos+=wsprintfW(wszTemp+pos, L" (%d,%d)",x-dwPlayerX,y-dwPlayerY);
			}
			float dis=getPlayerDistanceYard(pSelectedUnit);
			int dis10=(int)(dis*10+0.5);
			pos+=wsprintfW(wszTemp+pos, L" %d.%d yard", dis10/10,dis10%10);
			pos+=wsprintfW(wszTemp+pos, L" size=%d", d2common_getUnitSize(pSelectedUnit));
			if (pSelectedUnit->dwUnitType==1) {
				//if (d2common_IsUnitBlocked(PLAYER,pSelectedUnit,2)) pos+=wsprintfW(wszTemp+pos, L" notvisible");
				if (d2common_IsUnitBlocked(PLAYER,pSelectedUnit,4)) {color=1;pos+=wsprintfW(wszTemp+pos, L" unattackable");}
				MonsterTxt *pMonTxt= pSelectedUnit->pMonsterData->pMonsterTxt;
				pos+=wsprintfW(wszTemp+pos, L" type%X",pSelectedUnit->pMonsterData->bTypeFlags);
				if (pMonTxt->fBoss)
					pos+=wsprintfW(wszTemp+pos, L" TB");
				if (pSelectedUnit->pMonsterData->fBoss) 
					pos+=wsprintfW(wszTemp+pos, L" B");
				if (pSelectedUnit->pMonsterData->fUnique) 
					pos+=wsprintfW(wszTemp+pos, L" U");
				if (pSelectedUnit->pMonsterData->fChamp)
					pos+=wsprintfW(wszTemp+pos, L" C");
			}

			int x=xpos-GetTextWidth(wszTemp);if (x<0) x=0;
			d2win_DrawText(wszTemp, x , ypos, color, 0);
			ypos = ypos -15;
		}
	}
	if (fMonitorQuantity) {
		int qx=(SCREENSIZE.x>>1)+150;
		int qy=SCREENSIZE.y-45;
		d2win_SetTextFont(8);
		if (dwHPotionCount) {
			wsprintfW(wszTemp,L"%d",dwHPotionCount);d2win_GetTextAreaSize(wszTemp, &sw, &sh);
			d2gfx_DrawRectangle(qx,qy-12,qx+sw,qy,0x0A,5);d2win_DrawText(wszTemp,qx,qy,0,0);qx+=sw+3;
		}
		if (dwMPotionCount) {
			wsprintfW(wszTemp,L"%d",dwMPotionCount);d2win_GetTextAreaSize(wszTemp, &sw, &sh);
			d2gfx_DrawRectangle(qx,qy-12,qx+sw,qy,0x91,5);d2win_DrawText(wszTemp,qx,qy,0,0);qx+=sw+3;
		}
		if (dwRPotionCount) {
			wsprintfW(wszTemp,L"%d",dwRPotionCount);d2win_GetTextAreaSize(wszTemp, &sw, &sh);
			d2gfx_DrawRectangle(qx,qy-12,qx+sw,qy,0x4B,5);d2win_DrawText(wszTemp,qx,qy,0,0);qx+=sw+3;
		}
		if (fUsingBow||fUsingCrossBow||fUsingThrow||dwRightSkill==35) {
			int pos=0,n=d2common_GetUnitStat(PLAYER, 70, 0); //quantity
			if (n) {
				d2win_SetTextFont(3);
				pos+=wsprintfW(wszTemp+pos, L"%d",  n);
				n=0;
				if (fUsingBow) n=dwArrowCount;
				else if (fUsingCrossBow) n=dwCArrowCount;
				else if (fUsingThrow) n=dwThrowCount;
				if (n) {
					pos+=wsprintfW(wszTemp+pos, L"+%d",  n);
				}
				d2win_GetTextAreaSize(wszTemp, &sw, &sh);
				int x=xpos-sw;
				d2gfx_DrawRectangle(x,ypos-12,x+sw,ypos,0x10,5);d2win_DrawText(wszTemp, x , ypos, 0, 0);
				ypos = ypos-25;
				d2win_SetTextFont(8);
			}
		}
		if (!d2client_CheckUiStatus(UIVAR_BELT)) {
			static wchar_t *num[4]={L"0",L"1",L"2",L"3"};
			int x=SCREENSIZE.x/2+33;
			int y=SCREENSIZE.y-40;
			for (int i=0;i<4;i++) {
				if (beltCount[i]<beltLayers) d2win_DrawText(num[beltCount[i]&7],x,y,0,0);
				x+=30;
			}
		}
		/*if (!fPlayerInTown) {
			wsprintfW(wszTemp, L"ID%d Town%d", dwIdentifyPortalCount,dwTownPortalCount);
			int x=xpos-GetTextWidth(wszTemp);
			d2win_DrawText(wszTemp, x , ypos, 0, 0);
			d2win_DrawText(wszTemp, x+1 , ypos+1, 6, 0);
			ypos = ypos -15;
		}
		*/
	}
	if (fMonitorDurability) {
		d2win_SetTextFont(8);
		for (int i=0;i<2;i++) {
			UnitAny *pUnit=i==0?rightWeapon:leftWeapon;
			if (!pUnit) continue;
			int max_durability=d2common_GetUnitStat(pUnit, STAT_MAXDURABILITY, 0); 
			if (max_durability) {
				int durability=d2common_GetUnitStat(pUnit, STAT_DURABILITY, 0); 
				if (durability<max_durability) {
					wsprintfW(wszTemp, L"%s %d/%d",i==0?L"DurR":L"DurL",durability,max_durability);
					d2win_GetTextAreaSize(wszTemp, &sw, &sh);
					int x=xpos-sw;
					d2gfx_DrawRectangle(x,ypos-12,x+sw,ypos,0x10,5);d2win_DrawText(wszTemp, x , ypos, 0, 0);
					ypos = ypos -15;
				}
			}
		}
	}
	if (expN&&tAnalyzeMonsterExp.isOn) {
		int pos=expTail-expN;if (pos<0) pos+=LAST_EXP_N;
		int id=expId-expN;
		for (int i=0;i<expN;i++,id++) {
			int add=lastExpChange[pos++];if (pos>=LAST_EXP_N) pos=0;
			if (add>=3000000) {wsprintfW(wszTemp,L"%d:exp+%dM",id,add/1000000);}
			else if (add>=3000) {wsprintfW(wszTemp,L"%d:exp+%dK",id,add/1000);}
			else {wsprintfW(wszTemp,L"%d:exp+%d",id,add);}
			drawBgTextLeft(wszTemp,xpos,ypos,0,0x10);ypos-=15;
		}
	}
	if (tCountDown.isOn) {
		d2win_SetTextFont(dwCountDownFontSize);
		for (int i=0;i<_ARRAYSIZE(countDowns);i++) {
			struct CountDown *cd=countDowns[i];
			if (!cd->active) continue;
			int color=6;
			int secs;
			if (cd->endMs) {
				int ms=cd->endMs-dwCurMs;
				secs = ms/1000;
				if (0<=secs&&secs<30) color=(ms%1000)<500?1:2;
				wsprintfW(wszTemp, L"%hs%d: %d", cd->name,cd->lv,secs);
			} else {
				secs=(dwCurMs-cd->startMs)/1000;
				wsprintfW(wszTemp, L"%hs: %d", cd->name,secs);
			}
			d2win_GetTextAreaSize(wszTemp, &sw, &sh);
			int x=xpos-sw;
			d2gfx_DrawRectangle(x,ypos-12,x+sw,ypos,0x10,5);d2win_DrawText(wszTemp, x , ypos, 0, 0);
			ypos = ypos-dwCountDownGap;
		}
		if (dwOakSageLvl) {
			wsprintfW(wszTemp, L"OakSage%d", dwOakSageLvl);
			d2win_GetTextAreaSize(wszTemp, &sw, &sh);
			int x=xpos-sw;
			d2gfx_DrawRectangle(x,ypos-12,x+sw,ypos,0x10,5);d2win_DrawText(wszTemp, x , ypos, 0, 0);
			ypos = ypos-dwCountDownGap;
		}
	}
	if (tDrawBossHpToggle.isOn) {
		for (int i=0;i<3;i++) {
			if (bossHps[i]==0) continue;
			wsprintfW(wszTemp, L"Boss%d: %d%%",i+1,bossHps[i]);
			d2win_GetTextAreaSize(wszTemp, &sw, &sh);
			int x=xpos-sw;
			d2gfx_DrawRectangle(x,ypos-12,x+sw,ypos,0x10,5);d2win_DrawText(wszTemp, x , ypos, 0, 0);
			ypos = ypos-12;
		}
		if (dwCurrentLevel>=Level_FurnaceofPain&&bossX&&bossY) {
			int drawX=(bossX-bossY)*16;int drawY=(bossX+bossY)*8;
			if (*d2client_pAutomapOn) {
				POINT ptMon,ptPlayer; 
				draw2map(&ptPlayer,PLAYER->pMonPath->drawX,PLAYER->pMonPath->drawY);
				draw2map(&ptMon,drawX,drawY);
				d2gfx_DrawLine(ptPlayer.x,ptPlayer.y,ptMon.x,ptMon.y,bossLineColor,-1);
			} else {
				d2gfx_DrawLine(
					PLAYER->pMonPath->drawX-screenDrawX,
					PLAYER->pMonPath->drawY-screenDrawY,
					drawX-screenDrawX,
					drawY-screenDrawY, bossLineColor,-1);
			}
		}
	}
	for (int i=1;i<=d2winLastId;i++) {
		if (i==dwGameWindowId) continue;
		D2Window *pwin=&d2wins[i];
		if (pwin->autoSkillId) {
			if (dwCurMs>pwin->autoSkillMs+6000) pwin->autoSkillId=0;
			int pos=wsprintfW(wszTemp, L"%d: %c->%d",i,pwin->autoLeft?'L':'R',pwin->autoSkillId); 
			if (pwin->netDelay)
				pos+=wsprintfW(wszTemp+pos,L" net=%dms",pwin->netDelay);
			int x=xpos-GetTextWidth(wszTemp);if (x<0) x=0;
			d2win_DrawText(wszTemp, x , ypos, 0, 0);
			ypos = ypos -15;
		}
	}
	if (nStashPages&&*d2client_pUiStashOn&&tDrawMultiPageStashCount.isOn) {
		int x=(SCREENSIZE.x-800)/2+dwDrawMultiPageStashXY[0];
		int y=(SCREENSIZE.y-600)/2+600+dwDrawMultiPageStashXY[1];
		for (int i=0;i<nStashPages;i++) {
			StashPage *p=stashPages[i];
			if (p&&p->n) wsprintfW(wszTemp,L"%d",p->n);
			else {wszTemp[0]='-';wszTemp[1]=0;}
			if (i==curStashPage) 
				x+=drawBgText(wszTemp,x,y,6,0x20);
			else 
				x+=drawBgText(wszTemp,x,y,0,0x10);
			x+=10;
			if (x>SCREENSIZE.x-20) break;
		}
	}
	d2win_SetTextFont(dwOldFone);
}

void __stdcall SetState( DWORD dwStateNo , BOOL fSet ){
	//if (tPacketHandler.isOn) {LOG("set state %d %d\n",dwStateNo,fSet);}
	struct CountDown *cd=NULL;
	switch (dwStateNo) {
		case 16: cd=&cd_QH;break;
		case 23: if (!fSet&&fAutoSkill&&dwRightSkill==71) dwAutoSkillCheckMs=0;break; //Dim Vision
		case 32: cd=&cd_BO;break;
		case 51: 
			if (fBC!=fSet) {fBC=fSet;dwSkillChangeCount++;}
			break; //Battle Command
		case 58: if (!fSet&&fAutoSkill&&dwRightSkill==82) dwAutoSkillCheckMs=0;break; //Life Tap
		case 60: if (!fSet&&fAutoSkill&&dwRightSkill==87) dwAutoSkillCheckMs=0;break; //Decrepify
		case 100: fState100HP=fSet;break;
		case 101: cd=&cd_HS;break;
		case 106: fState106ManaMs=fSet?dwCurMs:0;break;
		case 134: dwSkillChangeCount++;break; //ShrineSkill
		case 139: fWerewolf=fSet;cd=&cd_werewolf;break;
		case 140: fWerebear=fSet;cd=&cd_werebear;break;
		case 149: dwGuessBOMs=dwCurMs+500;break; //Oak Sage
		case 153: cd=&cd_Cloak;break;
		case 157: cd=&cd_Burst;break;
		case 159: cd=&cd_Fade;break;
	}
	if (cd) {
		cd->active=fSet;
		if (fSet) {
			if (cd->startMs<=dwCurMs&&dwCurMs<cd->startMs+500) {//self skill
				if (dwStateNo==16) dwGuessEnchantMs=dwCurMs+500;
				else if (dwStateNo==32) dwGuessBOMs=dwCurMs+500;
			} else {
				cd->endMs=0;
				if (dwStateNo==16) dwGuessEnchantMs=dwCurMs+500;
				else if (dwStateNo==32) dwGuessBOMs=dwCurMs+500;
			}
			cd->startMs=dwCurMs;
		} else {
			cd->startMs=cd->endMs=0;
		}
	}
}
static void __fastcall recvState(BYTE *buf) {
	int type=buf[1];
	int id=*(int *)(buf+2);
	int state=buf[0]==0xa8?buf[7]:buf[6];
	switch (buf[0]) {
		case 0xa7:LOG("Delayed State %d:%d %d\n",type,id,state);break;
		case 0xa8:LOG("State %d:%d %d\n",type,id,state);break;
		case 0xa9:LOG("End State %d:%d %d\n",type,id,state);break;
	}
}
void __fastcall autoSkillDimVision(char *packet);
//delayed state
void __declspec(naked) RecvCommand_A7_Patch_ASM() {
	__asm{
		cmp tPacketHandler.isOn,0
		je next
		push ecx
		call recvState
		pop ecx
next:
		mov esi, ecx
		movzx eax, byte ptr [esi+06] //stateId
		cmp eax,0x10
		jne not_enchant
		mov dwAutoSummonCheckEnchantMs,1
not_enchant:
		cmp eax,23
		jne not_dimvision
		push esi
		call autoSkillDimVision
		pop esi
not_dimvision:
		movzx edx, byte ptr [esi+1] //type
		mov ecx, dword ptr [esi+2] //id

		cmp edx , 0
		jne org
		cmp ecx ,[dwPlayerId]
		jne org
		
		push ecx
		push edx
		push esi 

		push 1
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
org:
		ret
	}
}

void __declspec(naked) RecvCommand_A8_Patch_ASM() {
	__asm{
		cmp tPacketHandler.isOn,0
		je next
		pushad
		call recvState
		popad
next:
		mov esi, ecx
		movzx eax, byte ptr [esi+07]
		cmp eax,0x10
		jne not_enchant
		mov dwAutoSummonCheckEnchantMs,1
not_enchant:
		movzx edx, byte ptr [esi+1]
		mov ecx, dword ptr [esi+2]
		cmp edx , 0
		jne  org
		cmp ecx ,[dwPlayerId]
		jne org
		
		push ecx
		push edx
		push esi 

		push 1
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
org:
		ret
	}
}

void __declspec(naked) RecvCommand_A9_Patch_ASM() {
	__asm{
		cmp tPacketHandler.isOn,0
		je next
		pushad
		call recvState
		popad
next:
		mov esi, ecx
		movzx eax, byte ptr [esi+6]
		cmp eax,0x10
		jne not_enchant
		mov dwAutoSummonCheckEnchantMs,1
not_enchant:
		movzx edx, byte ptr [esi+1]
		mov ecx, dword ptr [esi+2]

		cmp edx , 0
		jne  org
		cmp ecx ,[dwPlayerId]
		jne org
		
		push ecx
		push edx
		push esi 

		push 0
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
org:
		ret
	}
}

void itemAction(struct bitstream *bs,char *buf) {
	int cmd=buf[0]&0xFF;
	bitstream_seek(bs,61-16);
	int equipped=bitstream_rbit(bs, 4);
	bitstream_seek(bs,73-16);
	int location=bitstream_rbit(bs, 3);//bit73-75 0:belt bag:1 cube:4 stash:5
	if (EXPANSION) {
		dwSkillChangeCount++;
	} else {
		if (equipped) dwSkillChangeCount++;
	}
	if (location==0 //belt changed
		||equipped==4||equipped==5||equipped==11||equipped==12 //weapon changed
		) {
		leftWeapon=NULL;rightWeapon=NULL;
	}
	dwRecheckSelfItemMs=dwCurMs+300;
	if (tPacketHandler.isOn&&logfp) {
		LOG("	RECV %02x_itemAction location=%d equipped=%d\n",buf[0]&0xFF,location,equipped);
		if ((buf[0]&0xFF)==0x9C) hex(logfp,0,buf,8);
		else hex(logfp,0,buf,13);
/*|  1    | Helmet                  |
|  2    | Amulet                  |
|  3    | Armor                   |
|  4    | Weapon (Right)          |
|  5    | Weapon (Left)           |
|  6    | Ring (Right)            |
|  7    | Ring (Left)             |
|  8    | Belt                    |
|  9    | Boots                   |
| 10    | Gloves                  |
| 11    | Alternate Weapon (Right)|
| 12    | Alternate Weapon (Left) |
*/
	}
}
void changeStashPage(int id) {
	if (nStashPages>1000) {LOG("ERROR nStashPages=%d\n",nStashPages);nStashPages=0;}
	if (id==curStashPage) return;
	//LOG("changeStashPage %d->%d/%d\n",curStashPage,id,nStashPages);
	updateCurStashPage();
	curStashPage=id;
	if (id>=nStashPages) {
		for (int i=nStashPages;i<=id&&i<MAX_STASH_PAGES;i++) {
			if (stashPages[i]) stashPages[i]->n=0;
		}
		nStashPages=id+1;
	}
}
void __fastcall itemAction9C(char *buf) {
	struct bitstream bs;
	bitstream_init(&bs,buf+8,buf+8+12);
	itemAction(&bs,buf);
	if (changeStashPageCmd==0x9C&&buf[changeStashPageCmdOffset]==changeStashPageCmdValue) {
		int id=buf[changeStashPageValueOffset]&0xFF;
		changeStashPage(id);
	}
}
void __fastcall itemAction9D(char *buf) {
	struct bitstream bs;
	bitstream_init(&bs,buf+13,buf+13+12);
	itemAction(&bs,buf);
	if (changeStashPageCmd==0x9D&&buf[changeStashPageCmdOffset]==changeStashPageCmdValue) {
		int id=buf[changeStashPageValueOffset]&0xFF;
		changeStashPage(id);
	}
}
void __fastcall updateItemSkill(char *buf) {
	//22 [WORD Unknown (Unit Type?)] [DWORD Unit Id] [WORD Skill] [BYTE Amount] [WORD Unknown] 
	//22 00 db 01 - 00 00 00 dc - 00 02 a9 00 -             |"               
	int skill=*(unsigned short *)(buf+7);
	int count=buf[9];
	if (skill==220) dwTownPortalCount=count;
	else if (skill==218) dwIdentifyPortalCount=count;
	if (tPacketHandler.isOn) 
		LOG("	RECV 22_updateItemSkill skill=%d count=%d\n", skill,count);
}

// 6FB5E090 - B9 3F000000           - mov ecx,0000003F { 63 }
void __declspec(naked) RecvCommand_9C_Patch_ASM() {
	__asm{
		pushad
		call itemAction9C
		popad
		mov ecx, 0x3F
		ret
	}
}
//d2client_AE880: B9 40 00 00 00     mov ecx, 0x40 (64)
void __declspec(naked) RecvCommand_9D_Patch_ASM() {
	__asm{
		pushad
		call itemAction9D
		popad
		mov ecx, 0x40
		ret
	}
}
/*
6FB5C800 - 56                    - push esi
6FB5C801 - 8B F1                 - mov esi,ecx
6FB5C803 - 8A 46 0B              - mov al,[esi+0B]
6FB5C806 - 84 C0                 - test al,al
*/
void __declspec(naked) RecvCommand_22_Patch_ASM() {
	__asm{
		pushad
		call updateItemSkill
		popad
		mov esi,ecx
		mov al,byte ptr [esi+0xB]
		ret
	}
}
#define FontHalfH 6
extern BYTE nNeutralPlayerColor,nHostilePlayerColor;
struct LocalNamePos {int x,y,w,h,gid;};
static LocalNamePos localNamePos[16];
static int nLocalNames=0;
int getOverheadNameGid(int mx,int my) {
	LocalNamePos *p=localNamePos;
	for (int i=0;i<nLocalNames;i++,p++) {
		if (p->x<=mx&&mx<p->x+p->w&&p->y<=my&&my<p->y+p->h) return p->gid;
	}
	return 0;
}
static void drawPlayerName(UnitAny *pUnit) {
	int pvp=testPvpFlag(pUnit->dwUnitId);
	int *colors=dwNeutralNameColor;
	int local=isLocalPlayer(pUnit->dwUnitId);
	switch (pvp) {
		case 0:colors=dwHostileNameColor;break; //hostile
		case 1:colors=dwNeutralNameColor;break; //neutral
		case 2:colors=local?dwLocalNameColor:dwPartyNameColor;break;//party
		case 3:colors=dwSelfNameColor;break; //self
	}
	int fg=colors[0],bg=colors[1],bg2=colors[2];
	int drawX = pUnit->pMonPath->drawX;
	int drawY = pUnit->pMonPath->drawY;
	wchar_t wname[20];char *s=pUnit->pPlayerData->szName;
	for (int i=0;i<16;i++) {char c=*s++;wname[i]=c;if (!c) break;}
	wname[16]=0;
	int hp=0,mana=0;
	D2Window *pWin=NULL;
	if (pUnit==PLAYER) {
		hp=dwPlayerMaxHP<=0?0:dwPlayerHP*100/dwPlayerMaxHP;
		mana=dwPlayerMaxMana<=0?0:dwPlayerMana*100/dwPlayerMaxMana;
	} else if (local) {
		D2Window *pwin=&d2wins[1];
		for (int i=1;i<=d2winLastId;i++,pwin++) {
			if (pwin->uid==pUnit->dwUnitId) {
				pWin=pwin;
				int maxhp=pwin->hpMax;
				hp=maxhp<=0?0:pwin->hp*100/maxhp;
				int maxmana=pwin->manaMax;
				mana=maxmana<=0?0:pwin->mana*100/maxmana;
			}
		}
	} else {
		for (RosterUnit *pRU=PLAYERLIST;pRU;pRU=pRU->pNext) {
			if (pRU->dwUnitId==pUnit->dwUnitId) {hp=pRU->dwPartyLife;break;}
		}
	}
	int w,h;d2win_GetTextAreaSize(wname,&w,&h);
	int mrw=mana?w*mana/100:0;if (mrw>w) mrw=w;
	int rw=hp?w*hp/100:0;if (rw>w) rw=w;
	int x=drawX-screenDrawX-(w>>1);
	int y=drawY-screenDrawY-dwDrawPlayerNameDy;
	if (mana) {
		d2gfx_DrawRectangle(x,y-12-5,x+mrw,y-12,0x91,5);
		d2gfx_DrawRectangle(x+mrw,y-12-5,x+w,y-12,0,5);
	}
	if (pWin&&nLocalNames<16) {
		LocalNamePos *p=&localNamePos[nLocalNames++];
		p->x=x;p->y=y-12;p->w=w;p->h=12;p->gid=pWin->index;
	}
	d2gfx_DrawRectangle(x,y-12,x+rw,y,bg,5);
	d2gfx_DrawRectangle(x+rw,y-12,x+w,y,bg2,5);
	d2win_DrawText(wname,x,y,fg,0);
	y-=17;
	if (pWin&&pWin->replyMs&&dwCurMs<pWin->replyMs) {
		wchar_t wbuf[32];
		wchar_t *fmt=L"%d:";
		int fg=6,bg=0x84;
		switch (pWin->reply) {
			case MCR_OK:fmt=dwGameLng?L"%d:好滴":L"%d:ok";bg=0x68;break;
			case MCR_DONE:fmt=dwGameLng?L"%d:搞定":L"%d:done";break;
			case MCR_ERROR:fmt=dwGameLng?L"%d:错误":L"%d:error";bg=0x62;break;
			case MCR_GOT_RUNES:fmt=dwGameLng?L"%d:有了":L"%d:rune";break;
			case MCR_FAILED:fmt=dwGameLng?L"%d:搞砸了":L"%d:error";bg=0x62;break;
			case MCR_PASSED:fmt=dwGameLng?L"%d:过了":L"%d:passed";break;
		}
		wsprintfW(wbuf,fmt,pWin->index);
		drawBgText(wbuf,x,y,fg,bg);
		y-=12;
	}
	if (pUnit==PLAYER) {
		if (dwCurrentLevel==Level_LutGholein) {
			short q=QUESTDATA->quests[14];
			if (q&8) drawBgText(L"找杰海因",x,y,6,0x68);
			else if (q&0x10) drawBgText(L"找马席夫",x,y,6,0x68);
		}
	} else if (pWin) {
		if (pWin->isTeam) drawBgText(L"T",x,y,0,0x76);
	}
}
static int cos_sin[16][2]={
	{256,0},{237,98},{181,181},{98,237},
	{0,256},{-98,237},{-181,181},{-237,98},
	{-256,0},{-237,-98},{-181,-181},{-98,-237},
	{0,-256},{98,-237},{181,-181},{237,-98},
};
static void drawEmit(UnitAny *pUnit,int r,int color) {
	r=(r<<1)+r;
	int drawX0=pUnit->pMonPath->drawX,drawY0=pUnit->pMonPath->drawY-70;
	int x=pUnit->pMonPath->wUnitX,y=pUnit->pMonPath->wUnitY;
	int *p=cos_sin[0];
	for (int i=0;i<16;i++,p+=2) {
		int dx=(r*p[0])>>9;
		int dy=(r*p[1])>>9;
		int ux=x+dx,uy=y+dy;
		int drawX=(ux-uy)*16;
		int drawY=(ux+uy)*8;
		d2gfx_DrawLine(drawX0-screenDrawX,drawY0-screenDrawY,drawX-screenDrawX,drawY-screenDrawY,color,-1);
	}
}
static void drawCircle(UnitAny *pUnit,int r2,int color) {
	r2=(r2<<1)+r2;
	int x,y;
	switch (pUnit->dwUnitType) {
		case UNITNO_PLAYER:case UNITNO_MONSTER:case UNITNO_MISSILE:
			x=pUnit->pMonPath->wUnitX;y=pUnit->pMonPath->wUnitY;break;
		case UNITNO_OBJECT:case UNITNO_ITEM:case UNITNO_AREA_ENTRANCE:
			x=pUnit->pItemPath->unitX;y=pUnit->pItemPath->unitY;break;
		default:return;
	}
	int *p=cos_sin[0];
	int lx,ly,x0,y0;
	for (int i=0;i<16;i++,p+=2) {
		int dx=r2*p[0];
		int dy=r2*p[1];
		int ux=(x<<10)+dx,uy=(y<<10)+dy;
		int drawX=(ux-uy)>>6;
		int drawY=(ux+uy)>>7;
		if (i==0) {x0=drawX;y0=drawY;}
		else {
			d2gfx_DrawLine(lx-screenDrawX,ly-screenDrawY,drawX-screenDrawX,drawY-screenDrawY,color,-1);
			if (i==15)
				d2gfx_DrawLine(x0-screenDrawX,y0-screenDrawY,drawX-screenDrawX,drawY-screenDrawY,color,-1);
		}
		lx=drawX;ly=drawY;
	}
}
extern ToggleVar tShowTestInfo;
static void drawBossHp(UnitAny *pUnit,int boss,int lineColor,int value) {
	wchar_t wbuf[128];
	if (pUnit->dwMode==0||pUnit->dwMode==12) return;
	int maxhp=d2common_GetUnitStat(pUnit, STAT_MAXHP, 0);
	int hp=d2common_GetUnitStat(pUnit, STAT_HP, 0);
	int hpPercent;
	if (maxhp<=0) hpPercent=0;
	else if (maxhp==0x8000) hpPercent=(hp*100)>>15;
	else hpPercent=hp*100/maxhp;
	if (boss) {
		bossX=pUnit->pMonPath->wUnitX;
		bossY=pUnit->pMonPath->wUnitY;
		if (bossId<3) bossHps[bossId++]=hpPercent;
	}
	int drawX = pUnit->pMonPath->drawX;
	int drawY = pUnit->pMonPath->drawY;
	if (dwCurrentLevel==Level_ArreatSummit
		&&(pUnit->dwTxtFileNo==Mon_LightningSpire||pUnit->dwTxtFileNo==Mon_FireTower)) {
		int W=16;
		if (pUnit->dwTxtFileNo==Mon_FireTower) W<<=1; //*2
		if (pUnit->pMonsterData->bTypeFlags&8) W<<=1; //*2
		else if (pUnit->pMonsterData->bTypeFlags&0x10) W=((W<<1)+W)>>1; //*1.5
		int rw;
		if (maxhp==0x8000) rw=(W*hp)>>15;
		else rw=W*hp/maxhp;
		if (rw>W) rw=W;
		int xpos=drawX-screenDrawX-(W>>1);
		int ypos=drawY-screenDrawY-FontHalfH-12;
		d2gfx_DrawRectangle(xpos,ypos,xpos+rw,ypos+5,0x62,5);
		d2gfx_DrawRectangle(xpos+rw,ypos,xpos+W,ypos+5,0,5);
		if (tShowTestInfo.isOn) {
			if (pUnit->dwTxtFileNo==Mon_LightningSpire) {
				drawCircle(pUnit,33<<1,dwDrawUnitCount&8?0x10:0x20);
			} else if (pUnit->dwTxtFileNo==Mon_FireTower) {
				drawEmit(pUnit,23,dwDrawUnitCount&8?0x62:0x20);
			}
		}
		int pos=0;
		//if (value) pos+=wsprintfW(wbuf+pos,L"%d",value*100/256);
		//pos+=wsprintfW(wbuf+pos,L"%X",maxhp);
		if (pUnit->pMonsterData->fUnique) wbuf[pos++]='U';
		if (pUnit->pMonsterData->fChamp) wbuf[pos++]='C';
		if (pUnit->pMonsterData->fBoss) wbuf[pos++]='B';
		if (pUnit->pMonsterData->fMinion) wbuf[pos++]='m';
		int ce=0,fe=0,tp=0;
		if (pUnit->pMonsterData->fBoss||pUnit->pMonsterData->fChamp) {
			for (int i = 0; i < 9; i++) {
				int enchno = pUnit->pMonsterData->anEnchants[i];if (!enchno) break;
				switch (enchno) {
					case 9:fe=1;wbuf[pos++]='F';wbuf[pos++]='e';break; //FireEnchantedDesc
					//case 17:wbuf[pos++]='L';wbuf[pos++]='e';break; //LightningEnchantedDesc
					case 18:ce=1;wbuf[pos++]='C';wbuf[pos++]='e';break; //ColdEnchantedDesc
					case 25:wbuf[pos++]='M';wbuf[pos++]='b';break; //ManaBurnDesc
					case 26:tp=1;wbuf[pos++]='T';wbuf[pos++]='p';break;//Teleport
					case 27:wbuf[pos++]='S';wbuf[pos++]='h';break; //SpectralHit
				}
			}
			if (ce) {
				drawCircle(pUnit,8<<1,dwDrawUnitCount&8?0x97:0x9E);
				int dis256=getPlayerDistanceM256(pUnit);
				if (dis256<=9*256*3/2) {
					drawCircle(pUnit,15,dwDrawUnitCount&8?0x97:0x62);
					drawCircle(pUnit,14,dwDrawUnitCount&8?0x97:0x62);
					drawCircle(pUnit,13,dwDrawUnitCount&8?0x97:0x62);
					drawCircle(pUnit,12,dwDrawUnitCount&8?0x97:0x62);
					drawCircle(pUnit,11,dwDrawUnitCount&8?0x97:0x62);
				}
			}
			if (fe) {
				drawCircle(pUnit,3<<1,dwDrawUnitCount&8?0x62:0x68);
				int dis256=getPlayerDistanceM256(pUnit);
				if (dis256<=3*256*3/2) {
					drawCircle(pUnit,5,dwDrawUnitCount&8?0x62:0x84);
					drawCircle(pUnit,4,dwDrawUnitCount&8?0x62:0x84);
					drawCircle(pUnit,3,dwDrawUnitCount&8?0x62:0x84);
				}
			}
			if (tp) {
				if (fe) setBottomAlertMsg(5,dwGameLng?L"有FE传送塔":L"FE Teleport",300,1,0,6);
				else if (ce) setBottomAlertMsg(5,dwGameLng?L"有CE传送塔":L"CE Teleport",300,1,0,6);
				else setBottomAlertMsg(5,dwGameLng?L"有传送塔":L"Teleport",300,1,0,6);
				if (fe) setBottomAlertBg(5,0x10,0x62);
			}
		}
		int fr=d2common_GetUnitStat(pUnit,STAT_FIRE_RESIST,0);
		wbuf[pos++]=0xFF;wbuf[pos++]='c';wbuf[pos++]=dwDrawUnitCount&8?'0':'1';
		wsprintfW(wbuf+pos,L"%d",fr);
		if (pos) {
			if (fr>=80) {
				static int blinkMs=0,color=0,bg=0x97;
				if (dwCurMs>blinkMs) {
					blinkMs=dwCurMs+500;if (color) {color=0;bg=0x10;} else {color=2;bg=0x62;}
				}
				drawBgTextMiddle(wbuf,drawX-screenDrawX,drawY-screenDrawY-FontHalfH-12,color,bg);
			} else if (ce||fe) {
				static int blinkMs=0,color=0,bg=0x10;
				if (dwCurMs>blinkMs) {
					blinkMs=dwCurMs+300;if (color) {color=0;bg=ce?0x97:0x62;} else {color=ce?3:1;bg=0x10;}
				}
				drawBgTextMiddle(wbuf,drawX-screenDrawX,drawY-screenDrawY-FontHalfH-12,color,bg);
			} else {
				int w,h;d2win_GetTextAreaSize(wbuf,&w,&h);
				d2win_DrawText(wbuf,drawX-screenDrawX-(w>>1),drawY-screenDrawY-FontHalfH-12,dwDrawUnitCount&8?0:6,0);
			}
		}
	} else {
		wsprintfW(wbuf,L"%d%%",hpPercent);
		int w,h;d2win_GetTextAreaSize(wbuf,&w,&h);
		drawBgTextMiddle(wbuf,drawX-screenDrawX,drawY-screenDrawY-FontHalfH,0,0x10);
	}
	if (!lineColor) return;
	if (*d2client_pAutomapOn) {
		POINT ptMon,ptPlayer; 
		draw2map(&ptPlayer,PLAYER->pMonPath->drawX,PLAYER->pMonPath->drawY);
		draw2map(&ptMon,pUnit->pMonPath->drawX,pUnit->pMonPath->drawY);
		d2gfx_DrawLine(ptPlayer.x,ptPlayer.y,ptMon.x,ptMon.y,lineColor,-1);
	} else {
		d2gfx_DrawLine(
			PLAYER->pMonPath->drawX-screenDrawX,
			PLAYER->pMonPath->drawY-screenDrawY,
			pUnit->pMonPath->drawX-screenDrawX,
			pUnit->pMonPath->drawY-screenDrawY, lineColor,-1);
	}
}
//return 128,192,256,320,384,426,640,853
int getTowerValue(int isFire,int type) {
/*
火焰塔生命是闪电塔的2倍
08生命是00的2倍
10生命是00的1.5倍
0C经验是00的3倍
08经验是00的5倍
10经验是00的5倍
升级速度
00红色火焰塔 0.5 
0C黄色头目火焰塔 0.75
00红色闪电塔 1
08粉色头目火焰塔 1.25
0C黄色头目闪电塔 1.5
10粉色随从火焰塔 1.6
08粉色头目闪电塔 2.5
10粉色随从闪电塔 3.3
*/
	int hp=256,exp=256;
	if (isFire) hp<<=1; //*2
	if (type&8) hp<<=1; //*2
	else if (type&0x10) hp=((hp<<1)+hp)>>1; //*1.5
	if ((type&0xC)==0xC) exp=(exp<<1)+exp; //*3
	else if (type&0x18) exp=(exp<<2)+exp; //85
	return (exp<<8)/hp;
}
static void drawUnitsInfoInRect(AreaRectData *pData) {
	for (UnitAny* pUnit = pData->pUnit; pUnit; pUnit = pUnit->pRectNext) {
		switch (pUnit->dwUnitType) {
			case UNITNO_OBJECT:
				switch (pUnit->dwTxtFileNo) {
					case 473:
						if (LEVELNO==Level_FrigidHighlands) {//&&dwBarbrianLeft==5) {
							drawCircle(pUnit,13<<1,dwDrawUnitCount&8?0x97:0x9E);
							int dis256=getPlayerDistanceM256(pUnit);
							if (dis256<=13*256*3/2) {
								for (int d=12;d>=8;d--)
									drawCircle(pUnit,d<<1,dwDrawUnitCount&8?0x97:0x62);
							}
						}
						break;
				}
				break;
			case UNITNO_PLAYER:
				if (!(*d2client_pAutomapOn)&&tDrawPlayerNameToggle.isOn) drawPlayerName(pUnit);
				break;	
			case UNITNO_MONSTER: 
				if (tDrawBossHpToggle.isOn) {
					switch (pUnit->dwTxtFileNo) {
						case Mon_Andariel:
						case Mon_Duriel:
						case Mon_Mephisto:
						case Mon_Diablo:
						case Mon_BaalCrab:
						case Mon_TheSummoner:
						case Mon_Nihlathak:
						case Mon_Talic:case Mon_Madawc:case Mon_Korlic:
						case Mon_UberMephisto:case Mon_UberDiablo:case Mon_UberIzual:
						case Mon_UberAndariel:case Mon_UberDuriel:case Mon_UberBaal:
							drawBossHp(pUnit,1,bossLineColor,0);
							break;
						case Mon_CouncilMember1:
						case Mon_CouncilMember2:
						case Mon_CouncilMember3:
							if (dwCurrentLevel==Level_Travincal&&pUnit->pMonsterData->fBoss&&pUnit->pMonsterData->fUnique) {
								drawBossHp(pUnit,1,bossLineColor,0);
							}
							break;
						case Mon_OblivionKnight1: 
						case Mon_StormCaster1:
						case Mon_VenomLord:
						case Mon_DarkStalker:
							if (pUnit->pMonsterData->fBoss&&pUnit->pMonsterData->fUnique) {
								drawBossHp(pUnit,1,bossLineColor,0);
							}
							break;
						default:
							switch (dwCurrentLevel) {
								case Level_ArreatSummit:
									if (pUnit->dwTxtFileNo==Mon_LightningSpire||pUnit->dwTxtFileNo==Mon_FireTower) {
										int fr=d2common_GetUnitStat(pUnit,STAT_FIRE_RESIST,0);
										int value=getTowerValue(pUnit->dwTxtFileNo==Mon_FireTower,pUnit->pMonsterData->bTypeFlags);
										//return 128,192,256,320,384,426,640,853
										if (value<=200) {
											//if (fr>=100||tShowTestInfo.isOn) 
											drawBossHp(pUnit,0,0,value);
										} else if (value<=300) {
											//drawBossHp(pUnit,0,fr>=100?0:0x84,value); //green line
											drawBossHp(pUnit,0,0,value);
										} else if (value<=500) {
											drawBossHp(pUnit,0,fr>=100?0:0x62,value); //red line
										} else {
											drawBossHp(pUnit,0,fr>=100?0:0x6B,value); //pink line
										}
									}
									break;
							}
							break;
					}
				}
				if (tNecAutomapCorpsesToggle.isOn&&dwPlayerClass==2&&pUnit->dwMode==12) {
					if (!d2common_CheckCorpseCanUse(pUnit,0)) continue;
					if (d2common_CheckUnitState(pUnit, 0x76)) continue;//used corpse
					int drawX = pUnit->pMonPath->drawX;
					int drawY = pUnit->pMonPath->drawY;
					if (*d2client_pAutomapOn) {
						POINT p;
						draw2map(&p,drawX,drawY);
						drawBgTextMiddle(L"c",p.x,p.y-FontHalfH,0,0x10);
					} else {
						drawBgTextMiddle(L"c",drawX-screenDrawX,drawY-screenDrawY-FontHalfH,0,0x10);
					}
				}
				if (tMinimapDebugToggle.isOn) {
					wchar_t temp[16];
					wsprintfW(temp,L"%d:%d",pUnit->dwTxtFileNo,pUnit->dwMode);
					drawBgTextMiddle(temp,pUnit->pMonPath->drawX-screenDrawX,pUnit->pMonPath->drawY-screenDrawY-FontHalfH,0,0x10);
				}
				break;
		}
	}
}
struct PlayerCast {
	int x1,y1,x2,y2;
	int color,ms;
};
static PlayerCast playerCasts[16];
int nPlayerCast=0;
void drawUnitsInfo() {
	if (!tDrawPlayerNameToggle.isOn&&!tNecAutomapCorpsesToggle.isOn
		&&!tDrawBossHpToggle.isOn&&!tMinimapDebugToggle.isOn) return;
	nLocalNames=0;
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	drawUnitsInfoInRect(pData);
	for (int i=0;i<pData->nearbyRectCount;i++) {
		drawUnitsInfoInRect(pData->paDataNear[i]);
	}
	if (nPlayerCast) {
		static int colors[16]={0x20,0xAB,0xA4,0x87,0x9E,0x9F,0x7F,0x79,0x75,0x74,0x73,0x72,0x71,0x13,0x12,0x10};
		PlayerCast *pc=playerCasts;
		for (int i=0;i<nPlayerCast;) {
			if (pc->color>=16) {
				if (nPlayerCast-i>1) memcpy(pc,pc+1,(nPlayerCast-i-1)*sizeof(PlayerCast));
				nPlayerCast--;
			} else {
				int lineColor=colors[pc->color];
				if (dwCurMs>=pc->ms) {pc->ms=dwCurMs+40;pc->color++;}
				/*if (*d2client_pAutomapOn) {
					POINT ptMon,ptPlayer; 
					draw2map(&ptPlayer,pc->x1,pc->y1);
					draw2map(&ptMon,pc->x2,pc->y2);
					d2gfx_DrawLine(ptPlayer.x,ptPlayer.y,ptMon.x,ptMon.y,lineColor,-1);
				} else {
				*/
					d2gfx_DrawLine(pc->x1-screenDrawX,pc->y1-screenDrawY,
						pc->x2-screenDrawX,pc->y2-screenDrawY, lineColor,-1);
				//}
				i++;pc++;
			}
		}
	}
}
static void __fastcall playerSkillOnTarget(char *packet) {
	if (!fWinActive||nPlayerCast>=16) return;
	int pid=*(int *)(packet+2);
	int mid=*(int *)(packet+0xA);
	UnitAny *player=d2client_GetUnitFromId(pid,UNITNO_PLAYER);if (!player) return;
	UnitAny *mon=d2client_GetUnitFromId(mid,UNITNO_MONSTER);if (!mon) return;
	PlayerCast *pc=&playerCasts[nPlayerCast++];
	pc->x1=player->pMonPath->drawX;pc->y1=player->pMonPath->drawY;
	pc->x2=mon->pMonPath->drawX;pc->y2=mon->pMonPath->drawY;
	pc->color=0;pc->ms=dwCurMs+40;
	D2Window *pwin=&d2wins[1];
	for (int i=1;i<=d2winLastId;i++,pwin++) {
		if (pwin->uid==pid) {
			pwin->netDelay=GetTickCount()-pwin->autoSkillMs;
			//if (tShowTestInfo.isOn)
			//LOG("[%d] PlayerSkillOnTarget %d->%d delay=%d\n",GetTickCount(),pid,mid,GetTickCount()-pwin->autoSkillMs);
			break;
		}
	}
}
void __fastcall playerSkillOnMap(char *packet) {
	if (!fWinActive||nPlayerCast>=16) return;
	int pid=*(int *)(packet+2);
	int x=*(short *)(packet+0xB);
	int y=*(short *)(packet+0xD);
	UnitAny *player=d2client_GetUnitFromId(pid,UNITNO_PLAYER);if (!player) return;
	PlayerCast *pc=&playerCasts[nPlayerCast++];
	pc->x1=player->pMonPath->drawX;pc->y1=player->pMonPath->drawY;
	int drawX=(x-y)<<4;int drawY=(x+y)<<3;
	pc->x2=drawX;pc->y2=drawY;
	pc->color=0;pc->ms=dwCurMs+40;
}
/*
Unit Skill on Target   4c 1:[BYTE Unit Type] 2:[DWORD Unit Id] 6:[WORD  Skill] [BYTE Unknown] [BYTE Unknown] a:[DWORD Target Id] 00 00 
4C:d2client_AF1D0 
	d2client_AF205: 8B 50 0A           mov edx, [eax+0xA]
	d2client_AF208: 0F B6 40 08        movzx eax, byte ptr [eax+0x8]
*/
void __declspec(naked) RecvCommand_4C_Patch_ASM() {
	__asm {
		movzx edx,byte ptr [eax+1]
		cmp edx,0
		jne original
		pushad
		mov ecx,eax
		call playerSkillOnTarget
		popad
original:
		mov edx,[eax+0xA]
		movzx eax,byte ptr [eax+8]
		ret
	}
}
/*
Unit Cast Skill      4d 1:[BYTE Unit Type] 2:[DWORD Unit Id] 6:[DWORD  Skill] a:[BYTE Unknown] b:[WORD X] d:[WORD Y] 00 00 
4D:d2client_AF160 
	d2client_AF194: 0F B7 50 0D        movzx edx, word ptr [eax+0xD]
	d2client_AF198: 0F B6 40 0A        movzx eax, byte ptr [eax+0xA]
*/
/*
void __declspec(naked) RecvCommand_4D_Patch_ASM() {
	__asm {
		movzx edx,byte ptr [eax+1]
		cmp edx,0
		jne original
		pushad
		mov ecx,eax
		call playerSkillOnMap
		popad
original:
		movzx edx, word ptr [eax+0xD]
		movzx eax, byte ptr [eax+0xA]
		ret
	}
}
*/
