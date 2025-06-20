#include "stdafx.h"
#include "header.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

static char *ItemBinMagic="Ver2";
extern char szSnapshotPath[256];
FILE *openSnapshotFile(char *subfolder,char *ext,char *mode,char sep,char *action,int msg);
void getSnapshotPath1(char *buf,char *realm,char *account,char *name,
	char *subfolder,char *ext,char sep);

static void saveItem(FILE *fp,UnitAny *pUnit) {
	static char *magic1="Item",*magic2="Socket";
	if (pUnit->dwUnitType!=UNITNO_ITEM) return;
	fwrite(magic1,4,1,fp);
	fwrite(&pUnit->dwTxtFileNo,4,1,fp);
	int dwFilledSockets=pUnit->pInventory?pUnit->pInventory->dwFilledSockets:0;
	fwrite(&dwFilledSockets,4,1,fp);
	int x=0,y=0;
	if (pUnit->pItemPath) {x=pUnit->pItemPath->unitX;y=pUnit->pItemPath->unitY;}
	fwrite(&x,4,1,fp);fwrite(&y,4,1,fp);
	fwrite(pUnit->pItemData,0x6A,1,fp);
	int nStat=0;
	StatEx *se=NULL;
	StatList *plist=pUnit->pStatList;
	if (plist) {
		Stat *stat=plist->dwListFlag&0x80000000?&plist->sFullStat:&plist->sBaseStat;
		se=stat->pStats;
		nStat=stat->wStats;
		if (!se||nStat<=0||nStat>511||IsBadReadPtr(se,nStat<<3)) {
			nStat=0;
		}
	}
	fwrite(&nStat,4,1,fp);
	if (nStat) fwrite(se,nStat<<3,1,fp);
	if (pUnit->pInventory) {
		pUnit=pUnit->pInventory->pFirstItem;
		while (pUnit) {
			fwrite(magic2,6,1,fp);
			saveItem(fp,pUnit);
			pUnit=d2common_GetNextItemInInv(pUnit);
		}
	}
}
void saveItems() {
	FILE *fp=openSnapshotFile("dat","itemsP.bin","wb+",'_',"items",0);
	if (!fp) return;
	fwrite(ItemBinMagic,4,1,fp);fwrite(&curStashPage,4,1,fp);
	for (UnitAny *pItem=d2common_GetFirstItemInInv(PLAYER->pInventory);pItem;pItem=d2common_GetNextItemInInv(pItem)) {
		saveItem(fp,pItem);
	}
	for (int i=0;i<nStashPages;i++) {
		if (i==curStashPage) continue;
		fwrite("Page",4,1,fp);fwrite(&i,4,1,fp);
		StashPage *p=stashPages[i];if (!p||p->n<=0) continue;
		for (int j=0;j<p->n;j++) {
			saveItem(fp,p->items[j]);
		}
		LOG("save page %d items=%d\n",i+1,p->n);
	}
	fclose(fp);
	PetUnit *pMerc=GetPetByType(PLAYER,7);
	if (pMerc) {
		UnitAny *m=GetUnitFromIdSafe(pMerc->dwUnitId,UNITNO_MONSTER);
		if (m) {
			int zero=0;
			fp=openSnapshotFile("dat","itemsM.bin","wb+",'_',"mercItems",0);
			if (!fp) return;
			fwrite(ItemBinMagic,4,1,fp);fwrite(&zero,4,1,fp);
			for (UnitAny *pItem=d2common_GetFirstItemInInv(m->pInventory);pItem;pItem=d2common_GetNextItemInInv(pItem)) {
				saveItem(fp,pItem);
			}
			fclose(fp);
		}
	}
}
static int err=0;
static int isAllAttr(StatEx *se) {
	int value=se[0].dwStatValue;
	return se[1].wStatId==STAT_ENERGY
		&&se[2].wStatId==STAT_DEXTERITY
		&&se[3].wStatId==STAT_VITALITY
		&&value==se[1].dwStatValue
		&&value==se[2].dwStatValue
		&&value==se[3].dwStatValue;
}
static int isAllResist(StatEx *se) {
	int value=se[0].dwStatValue;
	return se[1].wStatId==STAT_LIGHTING_RESIST
		&&se[2].wStatId==STAT_COLD_RESIST
		&&se[3].wStatId==STAT_POSION_RESIST
		&&value==se[1].dwStatValue
		&&value==se[2].dwStatValue
		&&value==se[3].dwStatValue;
}
static int snapCurPage;
extern int showStatNum;
int cpLocaleName(wchar_t *dst,wchar_t *s,int max);
void dumpStat(FILE *fp,int id,int param,int *pvalue);
static void formatItem(FILE *fpin,FILE *fpout,int isSocket,int isMerc) {
	char name[128];int nameLen=0;
	StatEx stats[512];
	int show[32];int nShow=0;
	int txt=0,n=0,dwFilledSockets=0;
	int x=0,y=0;
	ItemData item;
	fread(&txt,4,1,fpin);
	fread(&dwFilledSockets,4,1,fpin);
	fread(&x,4,1,fpin);fread(&y,4,1,fpin);
	fread(&item,0x6A,1,fpin);
	fread(&n,4,1,fpin);
	if (n) fread(stats,n<<3,1,fpin);
	if (isSocket) return;
	int index=GetItemIndex(txt)+1;
	if (2080<=index&&index<=2089) { //potions
		return;
	}
	ItemTxt *pItemTxt=d2common_GetItemTxt(txt);
	int ethereal=item.dwItemFlags&ITEMFLAG_ETHEREAL;
	//char code[8];memcpy(code,pItemTxt->szCode,4);code[4]=0;
	if (isMerc) {
		fputs("@merc",fpout);
	} else {
		switch (item.nLocation) { 
			case 0:break;//ground
			case 1://cube/stash/inv
				switch (item.nItemLocation) {
					case 0:fprintf(fpout,"@bag");break;//bag
					case 3:fprintf(fpout,"@cube");break;//cube
					case 4:fprintf(fpout,"@page%d",snapCurPage+1);break;//stash
				}
				fprintf(fpout,"(%d,%d)",y+1,x+1);
				break;
			case 2:fprintf(fpout,"@belt");break;//belt
			case 3:fprintf(fpout,"@body");break;//body
		}
	}
	if (ethereal) fprintf(fpout," ethereal无形的");
	int needSocket=0;
	switch (item.dwQuality) {
		case ITEM_QUALITY_UNIQUE: {
			int fileindex = item.dwFileIndex;
			fprintf(fpout," U%d",fileindex);
			UniqueItemTxt *txt = d2common_GetUniqueItemTxt(fileindex);
			if (txt&&!IsBadReadPtr(txt,sizeof(UniqueItemTxt))) {
				acpLocaleName(name,d2lang_GetLocaleText(txt->wLocaleTxtNo),128);
				fprintf(fpout," %s",name);
			}
			switch (fileindex) {
				case 210: //蛇魔法师之皮 Skin of the Vipermagi
				case 357: //奇拉的守护 Kira's Guardian
				case 101: //囚房 The Ward
					needSocket=1;
					show[nShow++]=STAT_FIRE_RESIST;
					break;
				case 272: //马拉的万花筒 Mara's Kaleidoscope
					show[nShow++]=STAT_FIRE_RESIST;
					break;
				case 104: //运气守护 Chance Guards
				case 120: //拿各的戒指 Nagelring
				case 240: //战争旅者 War Traveler
				case 359: //基得的運氣 Gheed's Fortune
					show[nShow++]=STAT_MAGIC_FIND;
					break;
				case 217: //詩寇蒂的憤怒 Skullder's Ire 
				case 218: //U218 守護天使 Guardian Angel
					needSocket=1;
					show[nShow++]=STAT_Defense;
					break;
				case 246: //雷神之力 Thundergod's Vigor
					show[nShow++]=STAT_Defense;
					break;
				case 61: //渡鳥之爪 Raven Claw
				case 262: //巫師之刺 Wizardspike
				case 248: //諧角之冠 Harlequin Crest
					needSocket=1;
					nShow=-1;
					break;
				case 105: //法師之拳 Magefist
				case 106: //U106 霜燃 Frostburn
				case 237: //U237 地獄闊步 Infernostride
				case 274: //矮人之星 Dwarf Star
				case 276: //大君之怒 Highlord's Wrath
				case 300: //大自然的和平 Nature's Peace
				case 373: //蜘蛛之網 Arachnid Mesh
					nShow=-1;
					break;
				case 319: //U319 鬼火投射者 Wisp Projector
					show[nShow++]=STAT_MAGIC_FIND;
					show[nShow++]=STAT_LIGHTING_ABSORB_PERCENT;
					break;
				case 326: //死神的丧钟 The Reaper's Toll
				case 345: //安达利尔的面貌 Andariel's Visage
					show[nShow++]=STAT_LIFE_LEECH;
					needSocket=1;
					break;
				case 281: //泰坦的复仇 Titan's Revenge
					show[nShow++]=STAT_LIFE_LEECH;
					break;
				case 364: //卓古拉之握 Dracul's Grasp
					show[nShow++]=STAT_STRENGTH;
					show[nShow++]=STAT_LIFE_LEECH;
					break;
				case 275: //乌鸦之霜 Raven Frost
					show[nShow++]=STAT_DEXTERITY;
					break;
				case 400: //Helltorch
					show[nShow++]=STAT_STRENGTH;
					show[nShow++]=STAT_FIRE_RESIST;
					show[nShow++]=STAT_CLASS_SKILL;
					break;
			}
			break;
		}
		case ITEM_QUALITY_SET: {
			int fileindex = item.dwFileIndex;
			fprintf(fpout," S%d",fileindex);
			SetItemTxt *txt = d2common_GetSetItemTxt(fileindex);
			if (txt) {
				acpLocaleName(name,d2lang_GetLocaleText(txt->wLocaleTxtNo),128);
				fprintf(fpout," %s",name);
			}
			switch (fileindex) {
				case 40: //S40 西剛的守護
					nShow=-1;
					break;
				case 69: //艾爾多的成長 Aldur's Advance
					show[nShow++]=STAT_FIRE_RESIST;
					break;
			}
			break;
		}
		default:
			if (item.dwItemFlags&ITEMFLAG_RUNEWORD) {
				int fileindex=d2common_GetRuneWordTxtIndex(item.wMagicPrefix[0]);
				fprintf(fpout," W%d",fileindex);
				RuneWordTxt *txt=d2common_GetRuneWordTxt(fileindex);
				if (txt) {
					acpLocaleName(name,d2lang_GetLocaleText(txt->wLocaleTxtNo),128);
					fprintf(fpout," %s",name);
				}
				switch (fileindex) {
					case 37: //橡树之心 Heart of the Oak
						show[nShow++]=STAT_FIRE_RESIST;
						break;
					case 49: //精神 Spirit
						show[nShow++]=STAT_FCR;
						break;
					case 103: //W103 洞察 Insight
						show[nShow++]=STAT_MinDamage;
						show[nShow++]=STAT_MaxDamage;
						show[nShow++]=STAT_AURA;
						break;
				}
			} else {
				fprintf(fpout," i%d",index);
				if (item.dwQuality<=3) {
					if (255<=index&&index<=259 //merc weapon
						||index==1142||1193<=index&&index<=1197 //shield
						)
						needSocket=1;
				}
			}
			break;
	}
	acpLocaleName(name,d2lang_GetLocaleText(pItemTxt->wLocaleTxtNo),128);
	fprintf(fpout," %s",name);
	int hasSocket=0;
	StatEx *ps=stats;
	int showPos=0;
	int nextStat=show[showPos++];
	for (int i=0;i<n;i++,ps++) {
		if (ps->wStatId==STAT_NUMSOCKETS) hasSocket=ps->dwStatValue;
		if (nShow>0) {
			if (ps->wStatId<nextStat) continue;
			while (ps->wStatId>nextStat) {
				if (showPos>=nShow) {nShow=-1;break;}
				nextStat=show[showPos++];
			}
		}
		if (nShow<0) continue;
		showStatNum=nShow==0;
		fputc(' ',fpout);
		if (ps->wStatId==STAT_STRENGTH&&n-i>=4&&isAllAttr(ps)) {
			fprintf(fpout," 所有属性AllAttr%+d",ps->dwStatValue);i+=3;ps+=3;
		} else if (ps->wStatId==STAT_FIRE_RESIST&&n-i>=4&&isAllResist(ps)) {
			fprintf(fpout," 所有抗性AllResist%+d",ps->dwStatValue);i+=3;ps+=3;
		} else {
			dumpStat(fpout,ps->wStatId,ps->wParam,(int *)&ps->dwStatValue);
		}
		if (nShow>0) {
			while (ps->wStatId>=nextStat) {
				if (showPos>=nShow) {nShow=-1;break;}
				nextStat=show[showPos++];
			}
		}
	}
	if (needSocket&&!hasSocket) fprintf(fpout," needSocket");
	fputc('\n',fpout);
	showStatNum=1;
	//LOG("txt=%d n=%d\n",txt,n);
}
static int formatItems2(FILE *fpin,FILE *fpout,int isMerc) {
	char magic[16];
	int total=0;
	int n=fread(magic,4,1,fpin);if (!n) {err++;return -1;}
	if (memcmp(magic,ItemBinMagic,4)!=0) {err++;return -2;}
	n=fread(&snapCurPage,4,1,fpin);if (!n) {err++;return -3;}
	while (1) {
		int n=fread(magic,4,1,fpin);if (!n) return total;
		if (memcmp(magic,"Page",4)==0) {
			int n=fread(&snapCurPage,4,1,fpin);if (!n) {err++;return -4;}
		} else if (memcmp(magic,"Item",4)==0) {
			formatItem(fpin,fpout,0,isMerc);total++;
		} else if (memcmp(magic,"Sock",4)==0) {
			n=fread(magic,6,1,fpin);if (!n) {err++;return -5;}
			if (memcmp(magic,"etItem",6)!=0) {err++;return -6;}
			formatItem(fpin,fpout,1,isMerc);
		}
	}
	return total;
}
void formatItems(char *realm,char *account,char *name) {
	char binpath[256],txtpath[256];
	getSnapshotPath1(txtpath,realm,account,name,NULL,"txt",'_');
	getSnapshotPath1(binpath,realm,account,name,"dat","itemsM.bin",'_');
	FILE *fp=fopen(binpath,"rb"),*fpout=NULL;
	if (fp) {
		if (!fpout) {
			LOG("rebuild %s\n",txtpath);
			fpout=fopen(txtpath,"w+");
		}
		if (fpout) formatItems2(fp,fpout,1);
		fclose(fp);
	}
	getSnapshotPath1(binpath,realm,account,name,"dat","itemsP.bin",'_');
	fp=fopen(binpath,"rb");
	if (!fp) {
		LOG("can't open %s\n",binpath);
	} else {
		if (!fpout) {
			LOG("rebuild %s\n",txtpath);
			fpout=fopen(txtpath,"w+");
		}
		if (fpout) formatItems2(fp,fpout,0);
		fclose(fp);
	}
	if (fpout) fclose(fpout);
}
void scan_realm(char *realm) {
	WIN32_FIND_DATAA fd;
	char path[256],buf[256];
	char *accounts[32],*names[32];
	int accountN=0,nameN=0;
	_snprintf(path,256,"%s\\%s\\account.txt",szSnapshotPath,realm);
	FILE *fp=fopen(path,"r");
	if (fp) {
		while (1) {
			char *line=fgets(buf,256,fp);if (!line) break;
			int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
			if (accountN<32) accounts[accountN++]=_strdup(line);
		}
		fclose(fp);
	} else {
		sprintf(path,"%s\\%s\\account\\*.txt",szSnapshotPath,realm);
		printf("%s\n",path);
		int hasMore=1;
		for (HANDLE h=FindFirstFile(path,&fd);h!=INVALID_HANDLE_VALUE&&hasMore;hasMore=FindNextFile(h,&fd)) {
			if (fd.nFileSizeLow==0) continue;
			char *e=strrchr(fd.cFileName,'.');if (e) *e=0;
			if (accountN<32) accounts[accountN++]=_strdup(fd.cFileName);
		}
	}
	if (accountN==0) return;
	for (int i=0;i<accountN;i++) {
		sprintf(path,"%s\\%s\\account\\%s.txt",szSnapshotPath,realm,accounts[i]);
		FILE *fp=fopen(path,"r");if (!fp) continue;
		nameN=0;
		while (1) {
			char *line=fgets(buf,256,fp);if (!line) break;
			int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
			names[nameN++]=_strdup(line);
		}
		for (int j=0;j<nameN;j++) {
			formatItems(realm,accounts[i],names[j]);
		}
	}
}
void rebuild_snap() {
	char path[256],path2[256];
	_snprintf(path,256,"%s\\*",szSnapshotPath);
	WIN32_FIND_DATAA fd;
	int hasMore=1;
	err=0;
	for (HANDLE h=FindFirstFile(path,&fd);h!=INVALID_HANDLE_VALUE&&hasMore;hasMore=FindNextFile(h,&fd)) {
		int isdir=fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
		if (!isdir) continue;
		_snprintf(path2,256,"%s\\%s\\account",szSnapshotPath,fd.cFileName);
		int r=GetFileAttributesA(path2);if (r==-1) continue;
		LOG("%X %s\n",r,path2);
		if (_stricmp(fd.cFileName,".")==0) continue;
		if (_stricmp(fd.cFileName,"..")==0) continue;
		if (_stricmp(fd.cFileName,"dat")==0) continue;
		if (_stricmp(fd.cFileName,"debug")==0) continue;
		if (_stricmp(fd.cFileName,"log")==0) continue;
		scan_realm(fd.cFileName);
	}
	if (err) gameMessage("rebuild snapshot error %d",err);
	else gameMessage("rebuild snapshot done");
}
