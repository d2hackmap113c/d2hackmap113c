#include "stdafx.h"

static char *clsNames[8]={"ama","sor","nec","pal","bar","dru","asn","???"};
static int allSkill,cb,ow,autoInc;
static int clsSkillAdd[8];
static int clsSkillN[8];
void NpcTradeNewGame() {
	dwNpcTradeCheckMs=0;
}
static void checkItem(UnitAny *pUnit) {
	StatList *plist=pUnit->pStatList;
	Stat *stat=plist->dwListFlag&0x80000000?&plist->sFullStat:&plist->sBaseStat;
	if (!stat->pStats) return;
	int n=stat->wStats;if (n>511) return;
	StatEx *se=stat->pStats;
	if (!se||IsBadReadPtr(se,n*8)) return;
	int last=-1;
	for (int i=0;i<n;i++) {
		int id=se[i].wStatId;
		if (id<last) return; //invalid list
		last=id;
		int value=se[i].dwStatValue;
		int param=se[i].wParam&0xFFFF;
		switch (id) {
			case 188: break; //cls*8+type //type skill
			case 83: //class skill
				if (param<0||param>=7) break;
				if (!value) break;
				if (value==clsSkillAdd[param]) clsSkillN[param]++;
				else if (value>clsSkillAdd[param]) {clsSkillAdd[param]=value;clsSkillN[param]=1;}
				break;
			case 127: if (value>allSkill) allSkill=value;break;
			case 135: if (value>ow) ow=value;break;
			case 136: if (value>cb) cb=value;break;
			case 253: autoInc++;break;
		}
	}
}
static void checkItems() {
	allSkill=0;cb=0;ow=0;autoInc=0;memset(clsSkillAdd,0,sizeof(clsSkillAdd));
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=p_D2UnitTable[UNITNO_ITEM*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (IsBadReadPtr(pUnit,sizeof(UnitAny))) break;
			if (pUnit->dwUnitType!=UNITNO_ITEM) break;
			if ((pUnit->dwUnitId&0x7f)!=i) break;
			//nItemLocation;	//+45 -1 equiped 0 Inventory , 3 cube , 4 stash  
			if (pUnit->pItemData->nLocation!=1) continue;
			if (pUnit->pItemData->nBodyLocation!=0) continue;
			if (pUnit->pItemData->nItemLocation<0||pUnit->pItemData->nItemLocation>2) continue; //which tab
			if (pUnit->pItemData->dwOwnerId!=(DWORD)-1) continue;
			checkItem(pUnit);
		}
	}
}

wchar_t wszNpcTradeInfo[256];
void NpcTradeLoop() {
	if (!dwNpcTradeCheckMs||dwCurMs<dwNpcTradeCheckMs) return;
	checkItems();
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
		mov eax, p_D2TradeRelated
		mov eax, dword ptr [eax]
		ret
	}
}
