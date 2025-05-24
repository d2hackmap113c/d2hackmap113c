#include "stdafx.h"
#include "header.h"

/*
      1VIT           1Energy
ama   3HP 1Stamina   1.5 Mana
sor   2HP 1Stamina   2 Mana
nec   2HP 1Stamina   2 Mana
pal   3HP 1Stamina   1.5 Mana
bar   4HP 1Stamina   1 Mana
dru   2HP 1Stamina   2 Mana
ass   3HP 1.25Stamina   1.75 Mana
*/
extern int dwOakSageLvl;
extern int fWerewolf,fWerebear;
static const int hpPerVIT[7]={3,2,2,3,4,2,3};
static const int manaPer4Energy[7]={6,8,8,6,4,8,7};
int guessEnchantLevel() {
// time=lv*24+120
// S48 min S49 max
	static const int qhMin[41]={
504, 559, 616, 691, 769, 851, 935, 1022, 1112, 1223, //lv20-29
1337, 1456, 1578, 1704, 1834, 1968, 2105, 2246, 2390, 2539,//lv30-39
2691, 2847, 3006, 3169, 3336, 3507, 3681, 3860, 4041, 4227,//lv40-49
4416, 4609, 4806, 5007, 5211, 5419, 5631, 5846, 6065, 6288,//lv50-59
6514,//lv60
	};
	static const int qhMax[41]={
659, 725, 794, 882, 973, 1068, 1165, 1266, 1371, 1496, //lv20-29
1627, 1761, 1899, 2041, 2188, 2338, 2493, 2652, 2815, 2982, //lv30-39
3153, 3329, 3508, 3692, 3879, 4071, 4267, 4467, 4671, 4879, //lv40-49
5092, 5308, 5529, 5754, 5982, 6215, 6452, 6694, 6939, 7188, //lv50-59
7442,
	};
	int fireMin=d2common_GetUnitStat(PLAYER, 48, 0);
	int fireMax=d2common_GetUnitStat(PLAYER, 49, 0);
	int lv=0;
	if (fireMin<504) lv=0;
	else if (fireMin>=6514) lv=60;
	else {
		for (int i=0;i<40;i++) {
			if (qhMin[i]<=fireMin&&fireMin<qhMin[i+1]) {lv=20+i;break;}
		}
	}
	return lv;
}
int guessBOLevel(int debug) {
/*
Battle Order time 10*lvl+20 HP,Mana,Stamina +(3*lvl+32)%
Mana=(Mana+S9)*(1+S77+BO%)+S1ENG*?
Stamina=(Stamina+S11)(1+BO%)+S3VIT*?
Oak Sage add life: (25+5*lvl)%
*/
	int manabase=0,hpbase;
	StatList *plist=PLAYER->pStatList;
	Stat *stat=&plist->sBaseStat;
	for (int i=0;i<stat->wStats;i++) {
		StatEx *se=&stat->pStats[i];
		if (se->wStatId==STAT_MAXMANA) {manabase=se->dwStatValue>>8;}
		else if (se->wStatId==STAT_MAXHP) {hpbase=se->dwStatValue>>8;}
	}
	if (!manabase) return 0;
	int level=d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0);
	int add_mana=0,add_eng=0,add_mana_percent=0;
	int add_mana_lv=0,add_eng_lv=0;
	int add_hp=0,add_vit=0,add_hp_percent=0;
	int add_hp_lv=0,add_vit_lv=0;
	for (UnitAny *pUnit = d2common_GetFirstItemInInv(PLAYER->pInventory);pUnit;pUnit = d2common_GetNextItemInInv(pUnit)) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) continue ;
		if (pUnit->pItemData->nItemLocation==255) { //equipment
			if (pUnit->pItemData->nBodyLocation==11) continue;
			if (pUnit->pItemData->nBodyLocation==12) continue;
			if (pUnit->pItemData->dwItemFlags&0x4000) continue; //requirement not met?
		} else if (pUnit->pItemData->nItemLocation==0) { //bag
			int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
			if (2096<=index&&index<=2098) {//charm
			} else continue;
		} else continue;
		DWORD dwQuality = pUnit->pItemData->dwQuality;
		switch (dwQuality) {
			case ITEM_QUALITY_RARE:break;
			case ITEM_QUALITY_MAGIC:break;
			case ITEM_QUALITY_UNIQUE: {
				int fileindex = d2common_GetItemFileIndex(pUnit);
				UniqueItemTxt *pUniqueTxt = d2common_GetUniqueItemTxt(fileindex);
				if (pUniqueTxt&&level<pUniqueTxt->wReqlvl) continue;
			}
			break;
			case ITEM_QUALITY_SET: {
				int fileindex = d2common_GetItemFileIndex(pUnit);
				SetItemTxt *pSetTxt = d2common_GetSetItemTxt(fileindex);
				if (pSetTxt&&level<pSetTxt->wReqlvl) continue;
			}
			break;
		}
		add_eng+=d2common_GetUnitStat(pUnit, STAT_ENERGY, 0);
		add_mana+=d2common_GetUnitStat(pUnit, STAT_MAXMANA, 0)>>8;
		add_mana_lv+=d2common_GetUnitStat(pUnit, 217, 0);
		add_eng_lv+=d2common_GetUnitStat(pUnit, 223, 0);
		add_mana_percent+=d2common_GetUnitStat(pUnit, 77, 0);
		add_vit+=d2common_GetUnitStat(pUnit, STAT_VITALITY, 0);
		add_hp+=d2common_GetUnitStat(pUnit, STAT_MAXHP, 0)>>8;
		add_hp_lv+=d2common_GetUnitStat(pUnit, 216, 0);
		add_vit_lv+=d2common_GetUnitStat(pUnit, 222, 0);
		add_hp_percent+=d2common_GetUnitStat(pUnit, 76, 0);
	}
	if (add_eng_lv) add_eng+=add_eng_lv*level/2048;
	if (add_mana_lv) add_mana_lv=add_mana_lv*level/2048;
	if (add_vit_lv) add_vit+=add_vit_lv*level/2048;
	if (add_hp_lv) add_hp_lv=add_hp_lv*level/2048;
	int mana_actual=d2common_GetUnitStat(PLAYER, STAT_MAXMANA, 0)>>8;
	int hp_actual=d2common_GetUnitStat(PLAYER, STAT_MAXHP, 0)>>8;
	if (debug&&logfp) {
		fprintf(logfp,"base mana=%d hp=%d\n",manabase,hpbase);
		fprintf(logfp,"add mana=%d hp=%d\n",add_mana,add_hp);
		fprintf(logfp,"add eng=%d vit=%d\n",add_eng,add_vit);
		fprintf(logfp,"add_percent mana=%d hp=%d\n",add_mana_percent,add_hp_percent);
		int m=manabase+add_mana,hp=hpbase+add_hp;
		fprintf(logfp,"added: mana=%d hp=%d\n",m,hp);
		m=m*(100+add_mana_percent)/100;
		hp=hp*(100+add_hp_percent)/100;
		fprintf(logfp,"added percent mana=%d hp=%d\n",m,hp);
		m+=add_eng*manaPer4Energy[dwPlayerClass]/4+add_mana_lv;
		hp+=add_vit*hpPerVIT[dwPlayerClass]+add_hp_lv;
		fprintf(logfp,"calculated/actual: mana=%d/%d hp=%d/%d\n",m,mana_actual,hp,hp_actual);
	}
	float fMana_Mul=(float)(mana_actual-add_eng*manaPer4Energy[dwPlayerClass]/4.0f-add_mana_lv)
		/(manabase+add_mana);
	float fHP_Mul=(float)(hp_actual-add_vit*hpPerVIT[dwPlayerClass]-add_hp_lv)
		/(hpbase+add_hp);
	float fBO_addpercent=fMana_Mul*100-add_mana_percent-100;
	float fBO=(fBO_addpercent-32)/3.0f;
	float fOakSage_AddPercent=fHP_Mul*100-add_hp_percent-fBO_addpercent-100;
	if ((int)(fOakSage_AddPercent+0.5f)==0) dwOakSageLvl=0;
	else {
		//werewolf +25% werebear +75% Lycanthropy 15+lvl*5
		if (fWerewolf||fWerebear) {
			int ly=getSkillLevel(PLAYER,224); //Lycanthropy
			if (debug)
				LOG("OakSage add %f lycanthropy=%d wolf=%d bear=%d\n",fOakSage_AddPercent,ly,
					fWerewolf,fWerebear);
			if (ly) fOakSage_AddPercent-=15+ly*5;
			if (fWerewolf) fOakSage_AddPercent-=25;
			if (fWerebear) fOakSage_AddPercent-=75;
		}
		if ((int)(fOakSage_AddPercent+0.5f)==0) dwOakSageLvl=0;
		else dwOakSageLvl=(int)((fOakSage_AddPercent-25)/5+0.5f);
	}
	if (debug) {
		LOG("Mana mul: %f\n",fMana_Mul);
		LOG("HP mul: %f\n",fHP_Mul);
		LOG("BO %f add %f\n",fBO,fBO_addpercent);
		LOG("OakSage %d add %f\n",dwOakSageLvl,fOakSage_AddPercent);
	}
	return (int)(fBO+0.5f);
}
