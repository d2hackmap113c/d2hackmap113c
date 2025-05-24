#include "stdafx.h"
#include "header.h"

#define MAX_NAMES 512
FILE *openDbgFile(char *name);
void dumpStat(FILE *fp,int id,int param,int *pvalue);
void hex(FILE *fp,int addr,void *buf1,int n);
static void dumpItemTxt() {
	char buf[128];
	FILE *fp=openDbgFile("_txtItem.txt");if (!fp) return;
	int nTxt=*d2common_pItemTxtCount;
	ItemTxt *ptxt=*d2common_pItemTxts;
	for (int id=0;id<nTxt;ptxt=(ItemTxt *)((char *)ptxt+0x1a8),id++) {
		char code[8];
		int idx=GetItemIndex(id)+1;
		memcpy(code,ptxt->szCode,4);code[4]=0;
		fprintf(fp,"%d: txt%d %s",idx,id,code);
		fprintf(fp," type=%d",ptxt->nType);
		wchar_t *wName=d2lang_GetLocaleText(ptxt->wLocaleTxtNo);
		acpLocaleName(buf,wName,128);
		fprintf(fp," %s\n",buf);//itemNames[idx]?itemNames[idx]:"");
	}
	fclose(fp);
}
static void dumpProp(FILE *fp,ItemProp *p) {
	fputc(' ',fp);
	switch (p->PropId) {
		case 41:fprintf(fp," AllResist+(%d-%d)",p,p->MinVal,p->MaxVal);break;
		default:
			PropertiesTxt *pPropertiesTxt = d2common_GetPropertiesTxt(p->PropId);
			if ( !pPropertiesTxt ) return;
			fprintf(fp," P%d",p->PropId);
			short *pStatNo = pPropertiesTxt->wStatNo;
			for (int i=0;i<7;i++) {
				short statId=*pStatNo++;if (statId<0) break;
				fputc(' ',fp);
				int value=pPropertiesTxt->wVal[0];
				if (value) dumpStat(fp,statId,p->MaxVal,&value);
				else dumpStat(fp,statId,p->Param,&p->MaxVal);
				if (p->MinVal!=p->MaxVal) fprintf(fp,"(%d-%d)",p->MinVal,p->MaxVal);
			}
			break;
	}
}
static void dumpUniqueTxt() {
	char buf[128];
	FILE *fp=openDbgFile("_txtUnique.txt");if (!fp) return;
	int ptable=*(int *)d2common_pDataTables;
	int nUnique=*(int *)(ptable+0xC28);
	UniqueItemTxt *pu=(UniqueItemTxt *)*(int *)(ptable+0xC24);
	for (int i=0;i<nUnique;i++,pu++) {
		char code[5];memcpy(code,pu->szCode,4);code[4]=0;
		fprintf(fp,"L%02d",pu->wReqlvl);
		fprintf(fp," U%d %s",pu->dwIndex,pu->szDesc);
		fprintf(fp," %s",code);
		fprintf(fp," lv%d",pu->wQlvl);
		fprintf(fp," cost=(*%d+%d)",pu->dwCostMult,pu->dwCostAdd);
		for (int i=0;i<12;i++) {
			ItemProp *p=&pu->Prop[i];
			if (p->PropId==-1) break;
			dumpProp(fp,p);
		}
		wchar_t *wName=d2lang_GetLocaleText(pu->wLocaleTxtNo);
		acpLocaleName(buf,wName,128);
		//if (uniqueNames[pu->dwIndex]) 
		fprintf(fp," %s",buf);//uniqueNames[pu->dwIndex]);
		fputc('\n',fp);
		fflush(fp);
	}
	fclose(fp);
}
static void dumpSetTxt() {
	char buf[128];
	FILE *fp=openDbgFile("_txtSet.txt");if (!fp) return;
	int ptable=*(int *)d2common_pDataTables;
	int nSet=*(int *)(ptable+0xC1C);
	int pSets=*(int *)(ptable+0xC18);
	for (int i=0;i<nSet;i++,pSets+=0x1B8) {
		SetItemTxt *pu=(SetItemTxt *)pSets;
		char code[5];memcpy(code,pu->szCode,4);code[4]=0;
		fprintf(fp,"L%02d",pu->wReqlvl);
		fprintf(fp," S%d %s",pu->dwIndex,pu->szDesc);
		fprintf(fp," %s",code);
		fprintf(fp," lv%d",pu->wQlvl);
		fprintf(fp," cost=(*%d+%d)",pu->dwCostMult,pu->dwCostAdd);
		for (int i=0;i<9;i++) {
			ItemProp *p=&pu->Prop[i];
			if (p->PropId==-1) break;
			dumpProp(fp,p);
		}
		wchar_t *wName=d2lang_GetLocaleText(pu->wLocaleTxtNo);
		acpLocaleName(buf,wName,128);
		fprintf(fp," %s",buf);
		//if (0<=pu->dwIndex&&pu->dwIndex<MAX_NAMES&&setItemNames[pu->dwIndex]) 
		//	fprintf(fp," %s",setItemNames[pu->dwIndex]);
		fputc('\n',fp);
		fflush(fp);
	}
	fclose(fp);
}
void dumpRunewordTxt() {
	char buf[128];
	FILE *fp=openDbgFile("_txtRunewords.txt");if (!fp) return;
	int n=*d2common_pRuneWords;
	RuneWordTxt *txt=*d2common_pRuneWordTxt;
	for (int i=0;i<n;i++,txt++) {
		fprintf(fp,"W%d %s ",i,txt->szName);
		wchar_t *wName=d2lang_GetLocaleText(txt->wLocaleTxtNo);
		acpLocaleName(buf,wName,128);
		fprintf(fp," %s ",buf);
		for (int j=0;j<6;j++) {
			int rtxtid=txt->runeTxtId[j];if (rtxtid<=0) break;
			int rid=rtxtid-0x261;
			if (j>0) fputc('+',fp);
			fprintf(fp,"%d",rid);
		}
		fputc(' ',fp);
		for (int i=0;i<7;i++) {
			ItemProp *p=&txt->Prop[i];
			if (p->PropId==-1) break;
			dumpProp(fp,p);
		}
		fputc('\n',fp);
		fflush(fp);
	}
	fclose(fp);
}
void dumpTxt() {
	dumpItemTxt();
	dumpUniqueTxt();
	dumpSetTxt();
	dumpRunewordTxt();
}
