#include "stdafx.h"
#include "header.h"

ToggleVar 	tNpcTradeShowInfo={				TOGGLEVAR_ONOFF,	0,	-1,	 0,	"Npc Trade Info"};
int	dwNpcTradeCheckMs=0;
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY, "NpcTradeShowInfo",   &tNpcTradeShowInfo     },
};
void NpcTrade_AddConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
static char *clsNames[8]={"ama","sor","nec","pal","bar","dru","asn","???"};
static int curTab,allSkill,cb,ow,ias,ll,lm,autoInc;
static int clsSkillAdd[8];
static int clsSkillN[8];
struct Info {int x,y;char info[64];};
static struct Info infos[32];
static int infoN;
void NpcTradeNewGame() {
	dwNpcTradeCheckMs=0;
}
static void checkItem(UnitAny *pUnit,int mode) {
	int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
	int isArmor=1000<=index&&index<=2000;
	StatList *plist=pUnit->pStatList;
	Stat *stat=plist->dwListFlag&0x80000000?&plist->sFullStat:&plist->sBaseStat;
	if (!stat->pStats) return;
	int n=stat->wStats;if (n>511) return;
	StatEx *se=stat->pStats;
	if (!se||IsBadReadPtr(se,n*8)) return;
	int last=-1;
	char buf[256];
	int pos=0;
	buf[0]=0;
	for (int i=0;i<n;i++) {
		int id=se[i].wStatId;
		if (id<last) return; //invalid list
		last=id;
		int value=se[i].dwStatValue;
		int param=se[i].wParam&0xFFFF;
		if (mode==1) {
			if (infoN>=32) return;
			switch (id) {
				case 60: pos+=sprintf(buf+pos,"%dLL\n",value);break;
				case 62: pos+=sprintf(buf+pos,"%dLM\n",value);break;
				case 68: if (isArmor) pos+=sprintf(buf+pos,"s%d\n",value);break;
				case 83: pos+=sprintf(buf+pos,"+%d\n",value);break;//class skill
				case 127: pos+=sprintf(buf+pos,"+%d\n",value);break;
				case 135: pos+=sprintf(buf+pos,"%dOW\n",value);break;
				case 136: pos+=sprintf(buf+pos,"%dCB\n",value);break;
				case 253: pos+=sprintf(buf+pos,"INC\n",value);break;
			}
			if (pos>240) break;
		} else {
			switch (id) {
				case 188: break; //cls*8+type //type skill
				case 83: //class skill
					if (param<0||param>=7) break;
					if (!value) break;
					if (value==clsSkillAdd[param]) clsSkillN[param]++;
					else if (value>clsSkillAdd[param]) {clsSkillAdd[param]=value;clsSkillN[param]=1;}
					break;
				case 60: if (value>ll) ll=value;break;
				case 62: if (value>lm) lm=value;break;
				case 68: if (isArmor&&value>ias) ias=value;break;
				case 127: if (value>allSkill) allSkill=value;break;
				case 135: if (value>ow) ow=value;break;
				case 136: if (value>cb) cb=value;break;
				case 253: autoInc++;break;
			}
		}
	}
	if (mode==1&&buf[0]) {
		struct Info *pinfo=&infos[infoN++];
		pinfo->x=pUnit->pItemPath->unitX;
		pinfo->y=pUnit->pItemPath->unitY;
		if (pos>63) buf[63]=0;
		strcpy(pinfo->info,buf);
	}
}
static void checkItems(int curtab,int mode) {
	allSkill=0;cb=0;ow=0;ias=0;ll=0;lm=0;autoInc=0;memset(clsSkillAdd,0,sizeof(clsSkillAdd));
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_ITEM*128+i];pUnit;pUnit=pUnit->pHashNext) {
			//if (IsBadReadPtr(pUnit,sizeof(UnitAny))) break;
			if (pUnit->dwUnitType!=UNITNO_ITEM) break;
			if ((pUnit->dwUnitId&0x7f)!=i) break;
			//nItemLocation;	//+45 -1 equiped 0 Inventory , 3 cube , 4 stash  
			if (pUnit->pItemData->nLocation!=1) continue;
			if (pUnit->pItemData->nBodyLocation!=0) continue;
			if (pUnit->pItemData->dwOwnerId!=(DWORD)-1) continue;
			int tab=pUnit->pItemData->nItemLocation;
			if (curtab>=0&&tab!=curtab) continue;
			checkItem(pUnit,mode);
		}
	}
}

void drawNpcTradeInfo(int color) {
	if (!d2client_CheckUiStatus(UIVAR_NPCTRADE)) return;
	if (curTab!=*d2client_pNpcTradeCurrentTab) {
		curTab=*d2client_pNpcTradeCurrentTab;
		infoN=0;checkItems(curTab,1);
	}
	int dx=(SCREENSIZE.x-800)/2;
	int dy=(SCREENSIZE.y-600)/2;
	wchar_t wszTemp[32];
	for (int i=0;i<infoN;i++) {
		char buf[64];
		struct Info *pinfo=&infos[i];
		strcpy(buf,pinfo->info);
		char *p=buf,*end=buf+64;
		int x=96+pinfo->x*29+dx;
		int y=140+pinfo->y*29+dy;
		while (p<end) {
			char *e=strchr(p,'\n');if (!e) break;
			*e=0;
			wsprintfW(wszTemp,L"%hs",p);
			d2win_DrawText(wszTemp,x,y,color,0);y+=12;
			p=e+1;
		}
	}
}
wchar_t wszNpcTradeInfo[256];
void NpcTradeLoop() {
	if (!dwNpcTradeCheckMs||dwCurMs<dwNpcTradeCheckMs) return;
	curTab=-1;
	checkItems(-1,0);
	dwNpcTradeCheckMs=0;
	int pos=0;
	pos+=wsprintfW(wszNpcTradeInfo+pos, L"NPC:");
	int pos0=pos;
	if (allSkill) pos+=wsprintfW(wszNpcTradeInfo+pos, L" all+%d",allSkill);
	for (int i=0;i<7;i++) {
		if (clsSkillAdd[i]) {
			pos+=wsprintfW(wszNpcTradeInfo+pos, L" %hs+%d",clsNames[i],clsSkillAdd[i]);
			if (clsSkillN[i]>1) {
				pos+=wsprintfW(wszNpcTradeInfo+pos, L"*%d",clsSkillN[i]);
			}
		}
	}
	if (cb) pos+=wsprintfW(wszNpcTradeInfo+pos, L" %dCB",cb);
	if (ow) pos+=wsprintfW(wszNpcTradeInfo+pos, L" %dOW",ow);
	if (ias) pos+=wsprintfW(wszNpcTradeInfo+pos, L" %dIAS",ias);
	if (ll) pos+=wsprintfW(wszNpcTradeInfo+pos, L" %dLL",ll);
	if (lm) pos+=wsprintfW(wszNpcTradeInfo+pos, L" %dLM",lm);
	if (autoInc) pos+=wsprintfW(wszNpcTradeInfo+pos, L" AutoInc:%d",autoInc);
	if (pos==pos0) wszNpcTradeInfo[0]=0;
}

/*
click trade menu
6FAFAEE0 - A1 AC97BC6F           - mov eax,[6FBC97AC] { (0) }
*/
void __declspec(naked) NpcTradePatch_ASM() {
	__asm {
		mov eax, tNpcTradeShowInfo.isOn
		cmp eax, 0
		je original
		mov eax, dwCurMs
		add eax, 300
		mov dwNpcTradeCheckMs, eax
original:
		mov eax, d2client_pTradeRelated
		mov eax, dword ptr [eax]
		ret
	}
}
