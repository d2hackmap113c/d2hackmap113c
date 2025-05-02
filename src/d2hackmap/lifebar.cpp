#include "stdafx.h"
#include "header.h"

void InitTCList();
BYTE afMonsterDeathAnims[1000];
ToggleVar 	tMonsterLevel={				TOGGLEVAR_ONOFF,	0,	-1,	1	,"Monster Level"};
ToggleVar 	tMonsterTC={					TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterTC"};
ToggleVar 	tMonsterRuneTC={				TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterRuneTC"};
ToggleVar 	tMonsterResists={			TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterResists"};
ToggleVar 	tMonsterHPPercent={		    TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterHPPercent"};
ToggleVar 	tMonsterDistance={			TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterDistance"};
ToggleVar 	tMonsterID={					TOGGLEVAR_ONOFF,	0,	-1,	1	,"MonsterID"};
ToggleVar 	tSPMonsterHP={		    TOGGLEVAR_ONOFF,	0,	-1,	1	,"SinglePlayerMonsterHP"};
ToggleVar 	tSPMonsterExp={		    TOGGLEVAR_ONOFF,	0,	-1,	1	,"SinglePlayerMonsterExp"};
BYTE 			nLifeBarTrans=				(BYTE)-1;
BYTE 			nLifeBarColour=				(BYTE)-1;
ToggleVar 	tPlayerLevel={				TOGGLEVAR_ONOFF,	0,	-1,	1, "PlayerLevel"};
ToggleVar 	tPlayerDistance={			TOGGLEVAR_ONOFF,	0,	-1,	1, "PlayerDistance"};
ToggleVar 	tPlayerHPPercent={			TOGGLEVAR_ONOFF,	0,	-1,	1, "PlayerHPPercent"};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_CHAR_ARRAY0, "MonsterDeathAnims",      &afMonsterDeathAnims, 1,{1000}},
  {CONFIG_VAR_TYPE_KEY, "MonsterTCToggle",         &tMonsterTC         },
  {CONFIG_VAR_TYPE_KEY, "MonsterRuneTCToggle",     &tMonsterRuneTC     },
  {CONFIG_VAR_TYPE_KEY, "MonsterLevelToggle",      &tMonsterLevel      },
  {CONFIG_VAR_TYPE_KEY, "MonsterResistsToggle",    &tMonsterResists    },                                       
  {CONFIG_VAR_TYPE_KEY, "MonsterHPPercentToggle",  &tMonsterHPPercent  },
  {CONFIG_VAR_TYPE_KEY, "MonsterDistanceToggle",   &tMonsterDistance   },
  {CONFIG_VAR_TYPE_KEY, "MonsterIDToggle",		 &tMonsterID		 },
  {CONFIG_VAR_TYPE_KEY, "SinglePlayerMonsterHPToggle",  &tSPMonsterHP  },
  {CONFIG_VAR_TYPE_KEY, "SinglePlayerMonsterExpHPToggle",  &tSPMonsterExp  },
  {CONFIG_VAR_TYPE_INT, "LifeBarTransparency",     &nLifeBarTrans  ,   1 },
  {CONFIG_VAR_TYPE_INT, "LifeBarColour",           &nLifeBarColour ,   1 },
  {CONFIG_VAR_TYPE_KEY, "PlayerLevelToggle",       &tPlayerLevel       },
  {CONFIG_VAR_TYPE_KEY, "PlayerDistanceToggle",    &tPlayerDistance    },
  {CONFIG_VAR_TYPE_KEY, "PlayerHPPercentToggle",   &tPlayerHPPercent   },
};
void lifebar_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void lifebar_initConfigVars() {
	memset(afMonsterDeathAnims,   1 ,     sizeof(afMonsterDeathAnims));
}
void lifebarNewGame() {
	InitTCList();
}

struct MonTC {
	WORD wGroup;
	WORD wLevel;
	int nTc;
	int nRtc;
	BOOL fInit;	
};
MonTC *GetTcLevel(WORD tcno , int mlvl);

//返回调整后的Xpos
long __fastcall MonsterNamePatch(wchar_t *wszName, long xPos , UnitAny *pMon ,long xDrawSize ,long xDrawOffset) {
	int orilen=wcslen(wszName);
	int trimlen=orilen;
	while (trimlen>0&&wszName[trimlen-1]==' ') trimlen--;
	wszName[trimlen]=0;
	wchar_t *ptr=wszName+trimlen;

	if ( tMonsterLevel.isOn || tMonsterRuneTC.isOn ||  tMonsterTC.isOn){
		int lvl = 0;
		MonsterTxt *pMonTxt= pMon->pMonsterData->pMonsterTxt;
		if( pMonTxt->fBoss == 0 && DIFFICULTY && EXPANSION){
			lvl = GetAreaLevel();
		}
		else{
			lvl = d2common_GetUnitStat(pMon, STAT_LEVEL, 0);
		}

		if ( pMonTxt->fBoss == 1 && pMon->pMonsterData->wUniqueNo == 0 && !(pMonTxt->hcIdx>=546 && pMonTxt->hcIdx<=550 )) {
				//Putrid Defiler 不受场景等级影响但受+2 +3影响 
				//超级金怪受+3规则影响
		} else {
			if ( pMon->pMonsterData->fChamp ){
				lvl += 2;
			} else if ( pMon->pMonsterData->fBoss || pMon->pMonsterData->fMinion ) {
				lvl += 3; 
			}
		}
		if (tMonsterLevel.isOn) ptr+=wsprintfW(ptr, L" (L%d)", lvl);
		if (tMonsterRuneTC.isOn ||  tMonsterTC.isOn) {
			BYTE tcmode;
			WORD nTc;
			if (pMon->pMonsterData->fChamp )
				tcmode=1;
			else if ( pMon->pMonsterData->fBoss || pMon->pMonsterData->fMinion ) 
				tcmode=2; 
			else
				tcmode=0;

			if ( pMon->pMonsterData->fUnique ){
				SuperUniqueTxt *pSuperUniqueTxt = d2common_GetSuperUniqueTxt(pMon->pMonsterData->wUniqueNo);
				nTc = pSuperUniqueTxt->Tcs[DIFFICULTY];
			}else{
				nTc = pMonTxt->Tcs[DIFFICULTY][tcmode];
			}
			MonTC *pMonTC = GetTcLevel(nTc , lvl);
			if (tMonsterTC.isOn) ptr+=wsprintfW(ptr, L" (tc%d)", pMonTC->nTc);
			if (tMonsterRuneTC.isOn&&EXPANSION) ptr+=wsprintfW(ptr, L" (rtc%d)", pMonTC->nRtc);
		}
	}

	if ( tMonsterHPPercent.isOn ){
		//d2common_GetMonsterHpPercent(pMon)
		int hp = d2common_GetUnitStat(pMon, STAT_HP, 0);
		int maxhp = d2common_GetUnitStat(pMon, STAT_MAXHP, 0);
		if (maxhp>=0x10000) {hp>>=16;maxhp>>=16;}
		int div=hp*10000/maxhp;
		ptr+=wsprintfW(ptr,L" (%d.%02d%%)", div/100,div%100);
	}
	
	if (fIsSinglePlayer&&(tSPMonsterHP.isOn||tSPMonsterExp.isOn)){
		UnitAny *real=getSinglePlayerUnit(pMon->dwUnitId,pMon->dwUnitType);
		if (real) {
			char buf[64];int pos=0;
			pos+=sprintf(buf+pos," id%d",pMon->dwUnitId);
			pos+=sprintf(buf+pos," txt%d",pMon->dwTxtFileNo);
			if (tSPMonsterHP.isOn) {
				int hp = d2common_GetUnitStat(real, STAT_HP, 0)>>8;
				int maxhp = d2common_GetUnitStat(real, STAT_MAXHP, 0)>>8;
				buf[pos++]=' ';
				if (hp>1000) pos+=sprintf(buf+pos,"%d,%03d",hp/1000,hp%1000);
				else pos+=sprintf(buf+pos,"%d",hp);
				buf[pos++]='/';
				if (hp>1000) pos+=sprintf(buf+pos,"%d,%03d",maxhp/1000,maxhp%1000);
				else pos+=sprintf(buf+pos,"%d",maxhp);
			}
			if (tSPMonsterExp.isOn) {
				int exp = d2common_GetUnitStat(real, STAT_EXP, 0);
				pos+=sprintf(buf+pos," EXP");
				if (exp>1000) pos+=sprintf(buf+pos,"%d,%03d",exp/1000,exp%1000);
				else pos+=sprintf(buf+pos,"%d",exp);
			}
			ptr+=wsprintfW(ptr, L"%hs", buf);
		}
	}
	if (tMonsterDistance.isOn) {
		float dis=getPlayerDistanceYard(pMon);
		int d10=(int)(dis*10+0.5);
		ptr+=wsprintfW(ptr, L" (%d.%d yards)",d10/10,d10%10);
	}
	if (tMonsterID.isOn) ptr+=wsprintfW(ptr, L" (ID:%d)", pMon->dwTxtFileNo);
	if(tMonsterResists.isOn){
		int dr = d2common_GetUnitStat(pMon, STAT_DAMAGE_REDUCED,	0);
		int mr = d2common_GetUnitStat(pMon, STAT_MAGIC_RESIST,	0);
		int fr = d2common_GetUnitStat(pMon, STAT_FIRE_RESIST,		0);
		int lr = d2common_GetUnitStat(pMon, STAT_LIGHTING_RESIST,	0);
		int cr = d2common_GetUnitStat(pMon, STAT_COLD_RESIST,		0);
		int pr = d2common_GetUnitStat(pMon, STAT_POSION_RESIST,	0);
		if (dr|mr|fr|lr|cr|pr) {
			ptr+=wsprintfW(ptr, L" (Res:");
#define SC(color) *ptr++=(wchar_t)255;*ptr++='c';*ptr++=color;
			if (dr)	{SC('7');ptr+=wsprintfW(ptr,L" D%d", dr);} // Golden
			if (mr)	{SC('8');ptr+=wsprintfW(ptr,L" M%d", mr);} // Deep yellow
			if (fr)	{SC('1');ptr+=wsprintfW(ptr,L" F%d", fr);} // Red
			if (lr)	{SC('9');ptr+=wsprintfW(ptr,L" L%d", lr);} // Light yellow
			if (cr)	{SC('3');ptr+=wsprintfW(ptr,L" C%d", cr);} // Blue
			if (pr)	{SC('2');ptr+=wsprintfW(ptr,L" P%d", pr);} // Green
#undef SC
			*ptr++=')';
		}
	}
	
	int l = orilen - trimlen;
	while (l-->0) *ptr++=' ';*ptr=0;
	int newlen = GetTextWidth(wszName);
	xPos = xDrawSize + xDrawOffset ;
	if ( newlen > xPos) return 1;
	return  ( xPos - newlen ) /2;
}


void __declspec(naked) MonsterNamePatch_ASM()
{
	//ecx  wszName
	//edx  xpos
	//ebx  pUnit
	//[esp+0x38] xpos offset
	__asm {
		push ecx

		push [esp+0x3C]
		push [esp+0x48]
		push ebx
		call MonsterNamePatch

		pop ecx
		mov edx , eax
		jmp d2win_DrawUnitLifeBar
	}
}

static wchar_t szMonsterDescComma[] = L", ";
void __declspec(naked) MonsterDescCommaPatch1_ASM()
{
	__asm {
		mov eax,offset szMonsterDescComma
		ret
	}
}

void __declspec(naked) MonsterDescCommaPatch2_ASM()
{
	__asm {
		mov edx,offset szMonsterDescComma
		ret
	}
}

void __fastcall MonsterDescCommaPatch3(wchar_t *wszStr, MonsterTxt *pMonTxt)
{
	if (pMonTxt->wFlag == 0x1506) {
		wcscat(wszStr, szMonsterDescComma);
	}
}

void __declspec(naked) MonsterDescCommaPatch3_ASM()
{
	__asm {
//ecx = ptr string
//[esp+0x20] = ptr monstats.txt
		mov edx,[esp+0x20]
		call MonsterDescCommaPatch3
		mov ecx, [esp+0x20];
		ret
	}
}

void __declspec(naked) LifeBarPatch_ASM()
{
	__asm {
		pop ebp
//originalish code
		movzx eax,[nLifeBarTrans]
		cmp al,-1
		jne nodefault1
		mov al,1
nodefault1:
		push eax
		mov al,[nLifeBarColour]
		cmp al,-1
		jne nodefault2
		mov al,5
nodefault2:
		push eax
		sar ecx,7
		jmp ebp
	}
}

void __declspec(naked) MonsterDeathPatch_ASM() {
	__asm {
		cmp [afMonsterDeathAnims+eax],0
		je noanim
		cmp eax,0x100 //original code
		ret
noanim:
		pop eax // discard return address
		pop esi // restore esi, check prolog of this function carefully
		ret 4 // return to upper level function
	}
}

/*
TC 计算
struct TreasureClassItem {
	DWORD _1[2];		//+00
	DWORD dwIdx;		//+08
	union {
		DWORD dwProp;
		struct {
			DWORD _3:2;
			DWORD fTc:1;
		};				//+0C
	};
};

struct TreasureClass {			//size = 0x2C
	WORD wGroup;				//+00
	WORD wLevel;				//+02
	DWORD dwItems;				//+04
	DWORD _1[8];				//+08
	TreasureClassItem *pItems;	//+28
};


在第一次进游戏时，对所有TC值计算对应的TC等级(取其子项的最大等级),TreasureClass最多1012个
TreasureClassEx.txt中，从第一行有效数据起从1开始编号，再加上160，即为游戏中的TC结构索引
游戏内索引1-160为基础TC，其所有子项都指向一个itemtype(bow，weap，mele，armo，abow),按32个一组拆分,

每个TreasureClass的子项TreasureClassItem可以指向TreasureClass，最终递归指向具体的itemtype
当指向itemtype时， dwIdx为gptItemTables的索引,否则为一个TC索引

TreasureClass的group属性第一次进游戏时为0或者实际group+5，以后每进一个新的房间+5  ;

tc 从3开始以3递增，实际最大87 
runeTC  每2个rune 一组，共17组， 33独占一组


TC提升规则
如果mlvl>tc->level 且 tc->group 不为0 , 将tc提升到同group中level不小于mlvl的tc项
*/


static MonTC anMonsterTCs[1014];
static BOOL fIsTcInit = FALSE ; 	

void CalculateHighestTC(int tcno , WORD offset) {
	TreasureClass *pTc = d2common_GetTreasureClasses(tcno, 0);
	if(!pTc)return;
	anMonsterTCs[tcno].wLevel = pTc->wLevel;
	anMonsterTCs[tcno].wGroup = ( ( pTc->wGroup==0 ) ? 0 : pTc->wGroup-offset );
	anMonsterTCs[tcno].fInit = false;
	for (int i = 0; i < (int)pTc->dwItemNums; i++) {
		DWORD dwThingNo = pTc->pItems[i].dwThingNo;
		if (pTc->pItems[i].off0C&4) {
			if ( dwThingNo<= 160) {
				int groupno = (dwThingNo-1)/32;
				int tc = (((dwThingNo-1)%32)+1)*3;
				if (groupno==1 && tc>anMonsterTCs[tcno].nTc ) anMonsterTCs[tcno].nTc=tc;
			} else {
				if ( anMonsterTCs[ dwThingNo ].fInit==false ){
					CalculateHighestTC(dwThingNo , offset);
				}
				if ( anMonsterTCs[dwThingNo].nTc > anMonsterTCs[tcno].nTc ){
					anMonsterTCs[tcno].nTc = anMonsterTCs[dwThingNo].nTc;
				}
				if ( anMonsterTCs[dwThingNo].nRtc > anMonsterTCs[tcno].nRtc ){
					anMonsterTCs[tcno].nRtc = anMonsterTCs[dwThingNo].nRtc;
				}

			}
		} else {
			ItemTxt *itemtxt = d2common_GetItemTxt(dwThingNo);
			if (itemtxt && itemtxt->nType == 74) {
				int runeno = (itemtxt->szCode[1]-'0')*10+(itemtxt->szCode[2]-'0');
				int runetc = (int)(runeno/2)+runeno%2;
				if (runetc >anMonsterTCs[tcno].nRtc) anMonsterTCs[tcno].nRtc = runetc;
			}
		}
	}
	anMonsterTCs[tcno].fInit = true;
}

void InitTCList() {	
	if(fIsTcInit) return ;
	memset( anMonsterTCs ,		0 ,			sizeof(anMonsterTCs) );
	int i;
	WORD offset;
	TreasureClass *pTc=d2common_GetTreasureClasses(262, 0) ;
	offset= pTc->wGroup - 2;//获得修正值; 262 为Act 1 Melee A   group 为2
	for (i = 1; i <1014; i++) {
		CalculateHighestTC(i ,offset );
	}
	fIsTcInit = true;
}
MonTC *GetTcLevel(WORD tcno , int mlvl) {
	int tcidx = tcno;
	if (anMonsterTCs[tcidx].wGroup>0 && mlvl>anMonsterTCs[tcidx].wLevel) {
		WORD tempgroup = anMonsterTCs[tcidx].wGroup ;
		while ( anMonsterTCs[tcidx+1].wGroup == tempgroup){
			tcidx++;
			if (mlvl<=anMonsterTCs[tcidx].wLevel) break;

		}
	}
	return &anMonsterTCs[tcidx];
}
int GetPlayerHpPercent(UnitAny *pUnit){
	for( RosterUnit *pRosterUnit = PLAYERLIST ;pRosterUnit ; pRosterUnit=pRosterUnit->pNext ){
		if( pUnit->dwUnitId == pRosterUnit->dwUnitId) return pRosterUnit->dwPartyLife ;
	}
	return 0;
}
void __fastcall PlayerNamePatch(wchar_t *wszName, UnitAny *pUnit){
	if (tPlayerLevel.isOn) {
		int lvl = d2common_GetUnitStat(pUnit, STAT_LEVEL, 0);
		wsprintfW(wszName+wcslen(wszName),  L" (L%d)", lvl);
	}
	if ( tPlayerHPPercent.isOn ){
		wsprintfW(wszName+wcslen(wszName),  L" (%d%%)", GetPlayerHpPercent(pUnit));
	}
	if ( tPlayerDistance.isOn ){
		float dis=getPlayerDistanceYard(pUnit);
		int d10=(int)(dis*10+0.5);
		wsprintfW(wszName+wcslen(wszName), L" (%d.%d yards)",d10/10,d10%10);
	}
}

void __declspec(naked) PlayerNamePatch_ASM() {
	//ecx  wszName
	//ebp  pUnit
	__asm {
		push ecx
		push edx
		mov edx ,ebp
		call PlayerNamePatch
		pop edx
		pop ecx
		jmp d2win_DrawUnitLifeBar
	}
}

