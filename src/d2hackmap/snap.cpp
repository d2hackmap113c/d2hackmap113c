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
static int isSocketable(int nType) {
	char *type;int s=0;
	switch (nType) {
		case 2:type="Shield盾";s=1;break;
		case 3:type="Armor盔甲";s=1;break;
		case 4:type="Gold";break;
		case 5:type="Arrow箭矢";break;
		case 6:type="Bolts十字弓弹";break;
		case 7:type="Ear";break;
		case 8:type="Herb";break;
		case 10:type="Ring戒指";break;
		case 11:type="Elixir";break;
		case 12:type="Amulet项链";break;
		case 15:type="Boot靴";break;
		case 16:type="Glove手套";break;
		case 18:type="Book";break;
		case 19:type="Belt腰带";break;
		case 21:type="Torch";break;
		case 22:type="Scroll卷轴";break;
		case 24:type="Sceptre权杖";s=1;break;
		case 25:type="Wand法杖";s=1;break;
		case 26:type="Staff棍";s=1;break;
		case 27:type="Bow弓";s=1;break;
		case 28:type="Axe斧";s=1;break;
		case 29:type="Club棒";s=1;break;
		case 30:type="Sword剑";s=1;break;
		case 31:type="Maul/Hammer";s=1;break;
		case 32:type="Dagger";s=1;break;
		case 33:type="Spear";s=1;break;
		case 34:type="Polearm";s=1;break;
		case 35:type="CrossBow十字弓";break;
		case 36:type="Maces钉头锤";s=1;break;
		case 37:type="Helm头盔";s=1;break;
		case 38:type="ThrowingPotion";break;
		case 39:type="Quest";break;
		//case 39:type="BodyPart";break;
		case 41:type="Skeleton Key 钥匙";break;
		case 42:type="ThrowingKnife飞刀";break;
		case 43:type="ThrowingAxe飞斧";break;
		case 44:type="Javelins标枪";break;
		case 58:type="Jewel珠宝";break;
		case 67:type="Claws爪";s=1;break;
		case 68:type="Orb水晶";s=1;break;
		case 69:type="NecHeads男巫盾牌";s=1;break;
		case 70:type="PalShield游侠盾牌";s=1;break;
		case 71:type="Helm头盔";s=1;break;
		case 72:type="DruHelm";s=1;break;
		case 74:type="Rune符文";break;
		case 75:type="Circlet头环";s=1;break;
		case 76:type="healing potions";break;
		case 77:type="mana potions";break;
		case 78:type="Rejuvenation Potion回复活力药剂";break;
		case 79:type="Stamina Potion体力药剂";break;
		case 80:type="Antidote Potion 解毒剂";break;
		case 81:type="Thawing Potion 解冻剂";break;
		case 82:type="Charm Small小护身符";break;
		case 83:type="Charm Large大型护身符";break;
		case 84:type="Charm Grand超大型护身符";break;
		case 85:type="AmaBow";s=1;break;
		case 86:type="AmaSpear";s=1;break;
		case 87:type="AmaJavelin";s=1;break;
		case 88:type="Claw";s=1;break;
		case 96:type="Amethyst紫宝石";break;
		case 97:type="Diamond钻石";break;
		case 98:type="Emerald绿宝石";break;
		case 99:type="Ruby红宝石";break;
		case 100:type="Sapphire蓝宝石";break;
		case 101:type="Topaz黄宝石";break;
		case 102:type="Skull骷髅";break;
		case 103:type="Collector";break;
		default:type="?";break;
	}
	return s;
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
	int canSocket=isSocketable(pItemTxt->nType);
	//char code[8];memcpy(code,pItemTxt->szCode,4);code[4]=0;
	int showItemName=0,showSpeed=0;
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
					show[nShow++]=STAT_Defense;
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
			showItemName=1;
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
						show[nShow++]=STAT_FCR;showItemName=1;
						break;
					case 103: //W103 洞察 Insight
						show[nShow++]=STAT_MinDamage;show[nShow++]=STAT_MaxDamage;
						show[nShow++]=STAT_AURA;
						showSpeed=1;
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
	if (showItemName) {
		acpLocaleName(name,d2lang_GetLocaleText(pItemTxt->wLocaleTxtNo),128);
		fprintf(fpout," %s",name);
	}
	if (showSpeed) {
		fprintf(fpout," speed%d",pItemTxt->dwSpeed);
	}
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
			fprintf(fpout," 所有属性attr%d",ps->dwStatValue);i+=3;ps+=3;
		} else if (ps->wStatId==STAT_FIRE_RESIST&&n-i>=4&&isAllResist(ps)) {
			fprintf(fpout," 所有抗性res%d",ps->dwStatValue);i+=3;ps+=3;
		} else if (ps->wStatId==STAT_MinDamage&&n-i>=2&&ps[1].wStatId==STAT_MaxDamage) {
			fprintf(fpout," 伤害%d-%d",ps->dwStatValue,ps[1].dwStatValue);i++;ps++;
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
	if (!hasSocket) {
		if (canSocket) fprintf(fpout," canSocket");
		if (needSocket) fprintf(fpout," needSocket");
	}
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
