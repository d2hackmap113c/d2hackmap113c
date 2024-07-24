#include "stdafx.h"

#define printf ERROR
static int debug=0;
struct ItemIdName {int id;const char *name;};
#define MAX_NAMES 512
int dwMercSeed=0,dwMercNameId;
char *questNames[41];
static char *clsNames[8]={"Ama","Sor","Nec","Pal","Bar","Dru","Ass","???"};
static char *clsSkillNames[8]={"AmaSkill","SorSkill","NecSkill","PalSkill","BarSkill","DruSkill","AssSkill","???"};
static char *clsSkills[7][3]={
	{"bow","passive","javalin"},
	{"fire","light","cold"},
	{"curse","poision","necSummon"},
	{"palCombat","offensive","defensive"},
	{"barCombat","mastery","warCry"},
	{"druSummon","shape","element"},
	{"trap","shadow","martial"},
};
static char *runeWordNames[MAX_NAMES];
static char *uniqueNames[MAX_NAMES];
static char *setItemNames[MAX_NAMES];
static char *skillNames[MAX_NAMES];
static char *propNames[MAX_NAMES];
static char *itemNames[3000]={0};
static int itemNamesLoaded=0;
static int startGameMs=0,startLv,startLvPercent;
static void dumpInventory();
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
//static FILE *fp=NULL;
void hex(FILE *fp,int addr,void *buf1,int n) {
	if (!fp||!buf1) return;
	u8 *buf=(u8 *)buf1;
	for (int i=0;i<n;i+=16) {
		fprintf(fp,"%06x: ",addr+i);
		for (int j=0;j<16;j++) {
			if (i+j>n) fprintf(fp,"   ");
			else fprintf(fp,"%02x ",buf[i+j]);
			if (j==3||j==7||j==11) fprintf(fp,"- ");
		}
		fprintf(fp,"|");
		for (int j=0;j<16;j++) {
			if (i+j>n) {fprintf(fp," ");continue;}
			u8 c=buf[i+j];
			if (c<' '||c>127) fprintf(fp," ");
			else fprintf(fp,"%c",c);
		}
		fprintf(fp,"\n");
	}
	fprintf(fp,"\n");
}
extern HANDLE confHeap;
void LoadLogNames(FILE *logfp) {
	if (!szSnapshotNamesPath[0]) return;
	FILE *fp=openFile(szSnapshotNamesPath,"rb");if (!fp) return;
	int size=0;char *p=loadFile(confHeap,fp,&size);if (!p) return;
	fclose(fp);
	char *end=p+size;
	memset(runeWordNames,0,MAX_NAMES*sizeof(char *));
	memset(uniqueNames,0,MAX_NAMES*sizeof(char *));
	memset(setItemNames,0,MAX_NAMES*sizeof(char *));
	memset(skillNames,0,MAX_NAMES*sizeof(char *));
	memset(propNames,0,MAX_NAMES*sizeof(char *));
	memset(itemNames,0,3000*sizeof(char *));
	memset(questNames,0,sizeof(questNames));
	while (p<end) {
		char *e=strchr(p,'\n');if (!e) e=end;
		char *line=p;p=e+1;
		*e=0;if (e[-1]=='\r') {e--;*e=0;}
		int len=e-line;
		if (line[0]=='/'&&line[1]=='/') continue;
		char *value=strchr(line,'=');if (!value) continue;*value=0;value++;
		if (memcmp(line,"runeWord",8)==0) {
			int id=strtol(line+8,NULL,10);
			if (id<MAX_NAMES) runeWordNames[id]=value;
		} else if (memcmp(line,"unique",6)==0) {
			int id=strtol(line+6,NULL,10);
			if (id<MAX_NAMES) uniqueNames[id]=value;
		} else if (memcmp(line,"set",3)==0) {
			int id=strtol(line+3,NULL,10);
			if (id<MAX_NAMES) setItemNames[id]=value;
		} else if (memcmp(line,"item",4)==0) {
			int id=strtol(line+4,NULL,10);
			if (id<3000) itemNames[id]=value;
		} else if (memcmp(line,"skill",5)==0) {
			int id=strtol(line+5,NULL,10);
			if (id<MAX_NAMES) skillNames[id]=value;
		} else if (memcmp(line,"prop",4)==0) {
			int id=strtol(line+4,NULL,10);
			if (id<MAX_NAMES) propNames[id]=value;
		} else if (memcmp(line,"classSkill",10)==0) {
			int id=strtol(line+10,NULL,10);
			if (0<=id&&id<7) clsSkillNames[id]=value;
		} else if (memcmp(line,"class",5)==0) {
			int id=strtol(line+5,NULL,10);
			if (0<=id&&id<7) clsNames[id]=value;
		} else if (memcmp(line,"amaSkill",8)==0) {
			int id=strtol(line+8,NULL,10);
			if (0<=id&&id<=2) clsSkills[0][id]=value;
		} else if (memcmp(line,"sorSkill",8)==0) {
			int id=strtol(line+8,NULL,10);
			if (0<=id&&id<=2) clsSkills[1][id]=value;
		} else if (memcmp(line,"necSkill",8)==0) {
			int id=strtol(line+8,NULL,10);
			if (0<=id&&id<=2) clsSkills[2][id]=value;
		} else if (memcmp(line,"palSkill",8)==0) {
			int id=strtol(line+8,NULL,10);
			if (0<=id&&id<=2) clsSkills[3][id]=value;
		} else if (memcmp(line,"barSkill",8)==0) {
			int id=strtol(line+8,NULL,10);
			if (0<=id&&id<=2) clsSkills[4][id]=value;
		} else if (memcmp(line,"druSkill",8)==0) {
			int id=strtol(line+8,NULL,10);
			if (0<=id&&id<=2) clsSkills[5][id]=value;
		} else if (memcmp(line,"asnSkill",8)==0) {
			int id=strtol(line+8,NULL,10);
			if (0<=id&&id<=2) clsSkills[6][id]=value;
		} else if (memcmp(line,"questName",9)==0) {
			int id=strtol(line+9,NULL,10);
			if (0<=id&&id<=40) questNames[id]=value;
		}
	}
	itemNamesLoaded=1;
}
static FILE *openSnapshotFile(char *subfolder,char *ext,char *mode,char sep,char *action,int msg) {
	char buf[1024];
	char *realm=(*p_D2GameInfo)->szRealmName;
	char *account=(*p_D2GameInfo)->szAccountName;
	char *name=(*p_D2GameInfo)->szCharName;
	if (!szSnapshotPath[0])
		sprintf(buf,"%ssnapshot",szPluginPath);
	else if (szSnapshotPath[0]&&szSnapshotPath[1]==':') //absolute path
		sprintf(buf,"%s",szSnapshotPath);
	else
		sprintf(buf,"%s%s",szPluginPath,szSnapshotPath);
	CreateDirectoryA(buf,NULL);
	if (realm[0]&&account[0]) {
		strcat(buf,"\\");strcat(buf,realm);CreateDirectoryA(buf,NULL);
		if (subfolder) {
			strcat(buf,"\\");strcat(buf,subfolder);CreateDirectoryA(buf,NULL);
		}
		if (sep=='/'||sep=='\\') {
			strcat(buf,"\\");strcat(buf,account);CreateDirectoryA(buf,NULL);
			int len=strlen(buf);
			sprintf(buf+len,"\\%s.%s",name,ext);
		} else {
			int len=strlen(buf);
			sprintf(buf+len,"\\%s_%s.%s",account,name,ext);
		}
	} else {
		if (subfolder) {
			strcat(buf,"\\");strcat(buf,subfolder);CreateDirectoryA(buf,NULL);
		}
		int len=strlen(buf);
		sprintf(buf+len,"\\%s.%s",name,ext);
	}
	FILE *fp=fopen(buf,mode);
	if (!msg) return fp;
	if (!fp) {
		LOG("can't open %s for %s\n", buf,action);
	} else {
		LOG(" %s to %s\n", action,buf);
		/*wchar_t temp[256];
		wsprintfW(temp,  L"%hs to %hs", action,buf);
		D2ShowGameMessage(temp, 0);
		*/
	}
	return fp;
}
FILE *openDbgFile(char *name) {
	char path[1024];
	if (!szSnapshotPath[0]) sprintf(path,"%ssnapshot",szPluginPath);
	else if (szSnapshotPath[0]&&szSnapshotPath[1]==':') //absolute path
		sprintf(path,"%s",szSnapshotPath);
	else sprintf(path,"%s%s",szPluginPath,szSnapshotPath);
	CreateDirectoryA(path,NULL);
	strcat(path,"\\");strcat(path,"debug");CreateDirectoryA(path,NULL);
	strcat(path,"\\");strcat(path,name);
	return fopen(path,"w+");
}
static int getLvPercent(UnitAny *pUnit) {
	int exp = D2GetUnitStat(pUnit, STAT_EXP, 0);
	DWORD lvl = D2GetUnitStat(pUnit, STAT_LEVEL, 0);
	DWORD player_class = pUnit->dwTxtFileNo;
	int nextlvlexp = D2GetMinExpToLevel(player_class, lvl);
	int currentlvlexp = 0;
	if (lvl>1) currentlvlexp = D2GetMinExpToLevel(player_class, lvl-1);
	int totalexpneedtoupgrade = nextlvlexp - currentlvlexp;
	int currentlvlgainedexp = exp - currentlvlexp;
	if (totalexpneedtoupgrade>200000) {
		return (int)((double)currentlvlgainedexp*10000.0/(double)totalexpneedtoupgrade);
	} else
		return currentlvlgainedexp*10000/totalexpneedtoupgrade;
}
void SnapshotNewGame() {
	startGameMs=GetTickCount();
	startLv = D2GetUnitStat(PLAYER, STAT_LEVEL, 0);
	startLvPercent=getLvPercent(PLAYER);
	dwMercSeed=0;dwMercNameId=0;
	if (tSnapshotNewGame.isOn) dumpInventory();
	if (logfp&&*p_D2GameInfo) {
		char *game=(*p_D2GameInfo)->szGameName;
		char *name=(*p_D2GameInfo)->szCharName;
		if (game) fprintf(logfp,"Game:%s",game);
		if (name) fprintf(logfp," Character:%s",name);
		fprintf(logfp," %s Lv%d.%04d\n",clsNames[dwPlayerClass],startLv,startLvPercent);
	}
}
void SnapshotEndGame() {
	if (tSnapshotLog.isOn&&GetTickCount()-startGameMs>60000) {
		FILE *fp=openSnapshotFile("log","log","a+",'_',"log",1);
		if (fp) {
			char *ip=(*p_D2GameInfo)->szGameServerIp;
			char *game=(*p_D2GameInfo)->szGameName;
			char *password=(*p_D2GameInfo)->szGamePassword;
			char *realm=(*p_D2GameInfo)->szRealmName;
			DWORD lvl = D2GetUnitStat(PLAYER, STAT_LEVEL, 0);
			int lvP=getLvPercent(PLAYER);
			fprintf(fp,"%s/%s",ip,game);
			fprintf(fp," Lv%d.%04d",startLv,startLvPercent);
			fprintf(fp,"->%d.%04d", lvl, lvP);
			int inc=(lvl-startLv)*10000+lvP-startLvPercent;
			fprintf(fp," +%d.%04d", inc/10000,inc%10000);
			int totalS=(GetTickCount()-startGameMs)/1000;
			fprintf(fp," %d:%02d:%02d\n",totalS/3600,totalS/60%60,totalS%60);
			fclose(fp);
		}
	}
	if (tSnapshotEndGame.isOn) dumpInventory();
}
static int dumpStat(FILE *fp,Stat *stat) {
	if (!stat||!stat->pStats) return 0;
	int sockets=0,fr=0,cr=0,pr=0,lr=0;
	int n=stat->wStats;if (n>511) return 0;
	StatEx *se=stat->pStats;
	if (!se||IsBadReadPtr(se,n*8)) return 0;
	if (debug) fprintf(fp," list=%x ",se);
	int last=-1;
	for (int i=0;i<n;i++) {
		int id=se[i].wStatId;
		if (id<last) return 0; //invalid list
		last=id;
		int value=se[i].dwStatValue;
		int value2=se[i].wParam&0xFFFF;
		fputc(' ',fp);
		int unknown=id<0||id>=MAX_NAMES||!propNames[id];
		if (unknown||debug) {
			fprintf(fp,"S(%d",id);
			if (value||value2) {
				fprintf(fp,",%d",value);
				if (value2) {
					if (value2>10000) fprintf(fp,",0x%x)",value2);
					else fprintf(fp,",%d)",value2);
				}
			}
			fprintf(fp,")");
			if (unknown) continue;
		}
		switch (id) {
			case 6:
			case 8:
			case 10:
				if (i<n-1&&se[i+1].wStatId==id+1) {
					fprintf(fp,propNames[id],value>>8,se[i+1].dwStatValue>>8);
					i++;
				}
				break;
			case 7:
			case 9:
			case 11:
				fprintf(fp,propNames[id],value>>8);break;
			case 39:fr+=value;break;
			case 41:lr+=value;break;
			case 43:cr+=value;break;
			case 45:pr+=value;break;
			case 83:fprintf(fp,propNames[id],value,clsSkillNames[value2]);break;
			case 97: 
			case 107:
			case 151:
				fprintf(fp,propNames[id],value,skillNames[value2],"");break;
			case 188:fprintf(fp,propNames[id],value,clsSkills[value2>>3][value2&3],clsNames[value2>>3]);break;
			case 194:
				sockets=value;
				fprintf(fp," 有%d孔",value);
				//if (debug) fprintf(fp," %x",&se[i].dwStatValue);
				break;
			case 195:
			case 196:
			case 197:
			case 198:
			case 199:
			case 201:
				fprintf(fp,propNames[id],value,value2&0x3f,skillNames[value2>>6]);break;
			case 204:
				fprintf(fp,propNames[id],value2&0x3f,skillNames[value2>>6],value&0xFF,value>>8);
				break;
			case 216:case 217:
				fprintf(fp,propNames[id],value/2048.0f,value2);break;
				break;
			default:
				if (214<=id&&id<=250) {
					fprintf(fp,propNames[id],value/8.0f,value2);break;
				} else {
					fprintf(fp,propNames[id],value,value2);break;
				}
		}
	}
	if (fr&&fr==lr&&fr==cr&&fr==pr) {
		fprintf(fp," 所有抗性resist+%d",fr);
	} else {
		if (fr) fprintf(fp," FR+%d",fr);
		if (lr) fprintf(fp," LR+%d",lr);
		if (cr) fprintf(fp," CR+%d",cr);
		if (pr) fprintf(fp," PR+%d",pr);
	}
	return sockets;
}
static void countGemRunes(FILE *fp,UnitAny *pUnit) {
	static char *gemClasses[5]={"CGem","FGem","NGem","FlGem","PGem"};
	static char *gemCls[5]={"c","f","n","fl","p"};
	static char *gemNames[7]={"Ame紫","Top黄","Sap蓝","Eme绿","Rub红","Dia钻","Sku骷"};
	int nRune[34]={0};
	int nGem[5][7]={0},hasGem[5]={0};
	int hasLRune=0,hasHRune=0;
	while (pUnit) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) goto next;
		int index = GetItemIndex(pUnit->dwTxtFileNo)+1;//?
		if ( index >2999 ) index= 2999;
		if (2103<=index&&index<=2135) {
			int r=index-2102;nRune[r]++;
			if (r<22) hasLRune++;
			else hasHRune++;
		} else if (2050<=index&&index<=2079) {
			int t=index-2050;
			nGem[t%5][t/5]++;
			hasGem[t%5]++;
		} else if (2090<=index&&index<=2094) {
			int t=index-2090;
			nGem[t][6]++;
			hasGem[t]++;
		}
next:
		pUnit = D2GetNextItemInInv(pUnit);
	}
	if (hasLRune) {
		fprintf(fp,"LRune:");
		for (int i=1;i<=21;i++) {
			if (nRune[i]) {
				fprintf(fp," r%02d",i);
				if (nRune[i]>1) fprintf(fp,"*%d",nRune[i]);
			}
		}
		fprintf(fp,"\n");
	}
	if (hasHRune) {
		fprintf(fp,"HRune:");
		for (int i=22;i<=33;i++) {
			if (nRune[i]) {
				fprintf(fp," r%02d",i);
				if (nRune[i]>1) fprintf(fp,"*%d",nRune[i]);
			}
		}
		fprintf(fp,"\n");
	}
	for (int i=0;i<5;i++) {
		if (!hasGem[i]) continue;
		fprintf(fp,"%s:",gemClasses[i]);
		for (int j=0;j<7;j++) {
			if (nGem[i][j]) {
				fprintf(fp," %s%s",gemCls[i],gemNames[j]);
				if (nGem[i][j]>1) fprintf(fp,"*%d",nGem[i][j]);
			}
		}
		fprintf(fp,"\n");
	}
}
static int lastLocation=0;
static int nLoc[8];
int dumpStatList(FILE *fp,StatList *plist,int dumpBaseStat) {
	int nsockets=0;
	if (debug) {
		fprintf(fp,"\n");
		fprintf(fp,"  StatList=%x",plist);
		fprintf(fp," prev=%x next=%x",plist->pPrevList,plist->pNextLis);
		fprintf(fp,"\n");
		if (plist->sBaseStat.pStats) {
			fprintf(fp,"  BaseStat:");nsockets=dumpStat(fp,&plist->sBaseStat);
			fprintf(fp,"\n");
		}
		if (plist->dwListFlag&0x80000000) {
			if (plist->sModStat.pStats) {
				fprintf(fp,"  ModStat:");dumpStat(fp,&plist->sModStat);
				fprintf(fp,"\n");
			}
			if (plist->sFullStat.pStats) {
				fprintf(fp,"  FullStat:");nsockets=dumpStat(fp,&plist->sFullStat);
				fprintf(fp,"\n");
			}
		}
	} else {
		if (plist->dwListFlag&0x80000000) {
			if (dumpBaseStat) {
				if (plist->sBaseStat.pStats) {
					fprintf(fp,"  BaseStat:");dumpStat(fp,&plist->sBaseStat);
					fprintf(fp,"\n");
				}
				if (plist->sFullStat.pStats) {
					fprintf(fp,"  FullStat:");nsockets=dumpStat(fp,&plist->sFullStat);
					fprintf(fp,"\n");
				}
			} else {
				nsockets=dumpStat(fp,&plist->sFullStat);
			}
		} else {
			nsockets=dumpStat(fp,&plist->sBaseStat);
		}
	}
	return nsockets;
}
static void dumpItem(FILE *fp,UnitAny *pUnit,int level,int loc) {
	if (loc>=0&&loc!=lastLocation) {
		static char *locNames[]={"body","bag","cube","box","unknown"};
		fprintf(fp,"------- %s %d -------\n",locNames[loc],nLoc[loc]);
		lastLocation=loc;
	}
		char *type="";
		int index = GetItemIndex(pUnit->dwTxtFileNo)+1;// original d2hackmap has everything from one not zero
		int needSocket=0;//=1<=index&&index<=1074||1090<=index&&index<2000;
		ItemTxt *pItemTxt = D2GetItemTxt( pUnit->dwTxtFileNo );
		DWORD dwQuality = pUnit->pItemData->dwQuality;
		int ethereal=D2CheckItemFlag(pUnit, ITEMFLAG_ETHEREAL, 0, "?");
		int location= pUnit->pItemData->nItemLocation;
		char code[5];
		memcpy(code,pItemTxt->szCode,4);code[4]=0;
		for (int i=0;i<level;i++) fprintf(fp,"\t");
		fprintf(fp,"--");
		if (pUnit->pItemData->nItemLocation==255) { //equipped
			static const char *equipedLocations[13]={
				"","Helmet","Amulet","Armor","Weapon(R)","Weapon(L)","Ring(R)","Ring(L)",
				"Belt","Boot","Glove","Weapon2(R)","Weapon2(L)"};
			if (pUnit->pItemData->nBodyLocation<13) {
				fprintf(fp," @%s",equipedLocations[pUnit->pItemData->nBodyLocation]);
			}
		if (debug&&pUnit->pItemData->nItemLocation==255) fprintf(fp, " body=%d", pUnit->pItemData->nBodyLocation);
		}
		if (debug) {
			fprintf(fp," id=%d pItem=%x pItemData=%x pPath=%x",pUnit->dwUnitId,pUnit,pUnit->pItemData,pUnit->pItemPath);
			fprintf(fp," mode=%x",pUnit->dwMode);
			fprintf(fp," flags1=%x",pUnit->dwFlags1);
			fprintf(fp," flags2=%x",pUnit->dwFlags2);
			fprintf(fp," flags=%x",pUnit->pItemData->dwItemFlags);
		}
		switch (pItemTxt->nType) {
			case 2:type="Shield盾";needSocket=1;break;//shield
			case 3:type="Armor盔甲";needSocket=1;break;//armor
			case 4:type="Gold";break;//gold
			case 5:type="Arrow";break;//arrow
			case 10:type="Ring戒指";break;
			case 12:type="Amulet项链";break;
			case 15:type="Boot靴";break;
			case 16:type="Glove手套";break;
			case 18:type="Book";break;
			case 19:type="Belt腰带";break;
			case 25:type="Wand法杖";needSocket=1;break;
			case 27:type="Bow弓";needSocket=1;break;
			case 30:type="Sword剑";needSocket=1;break;
			case 36:type="Maces钉头锤";break;
			case 37:type="Helm头盔";needSocket=1;break;
			case 39:type="Quest";break;
			case 43:type="Throwing投掷武器";break;
			case 44:type="Javelins标枪";needSocket=1;break;
			case 58:type="Jewel珠宝";break;
			case 69:type="NecHeads男巫盾牌";needSocket=1;break;
			case 70:type="PalShield游侠盾牌";needSocket=1;break;
			case 74:type="Rune符文";break;//rune int bktnum = (pItemTxt->szCode[1]-'0')*10+(pItemTxt->szCode[2]-'0');
			case 75:type="Circlet头环";needSocket=1;break;
			case 76:type="healing potions";break;
			case 77:type="mana potions";break;
			case 78:type="Rejuvenation Potion回复活力药剂";break;
			case 79:type="Stamina Potion体力药剂";break;
			case 82:type="Charm Small小护身符";break;
			case 83:type="Charm Large大型护身符";break;
			case 84:type="Charm Grand超大型护身符";break;
			case 96:type="Amethyst紫宝石";break;
			case 97:type="Diamond钻石";break;
			case 98:type="Emerald绿宝石";break;
			case 99:type="Ruby红宝石";break;
			case 100:type="Sapphire蓝宝石";break;
			case 101:type="Topaz黄宝石";break;
			case 102:type="Skull骷髅";break;
			default:
				fprintf(fp," type%d",pItemTxt->nType);
				break;
		}
		fprintf(fp," %s(%d,%s)",type,index,code);
		switch (location) {
			case 255: break;//fprintf(fp,"@body");break;
			case 0: fprintf(fp,"@bag");break;
			case 3: fprintf(fp,"@cube");break;
			case 4: fprintf(fp,"@box");break;
			default: fprintf(fp,"@%d",location);break;
		}
		if (ethereal) fprintf(fp," ethereal无形的");
		if (itemNames[index]) fprintf(fp," %s(",itemNames[index]);
		if (pItemTxt->dwSpeed) fprintf(fp," speed%d",pItemTxt->dwSpeed);
		if (pItemTxt->nLevelReq) fprintf(fp," 需要等级%d",pItemTxt->nLevelReq);
		if (pItemTxt->nMindam||pItemTxt->nMaxdam) 
			fprintf(fp," 单手伤害%d-%d",
				ethereal?pItemTxt->nMindam*150/100:pItemTxt->nMindam,
				ethereal?pItemTxt->nMaxdam*150/100:pItemTxt->nMaxdam);
		if (pItemTxt->nMindam2||pItemTxt->nMaxdam2) 
			fprintf(fp," 双手伤害%d-%d",
				ethereal?pItemTxt->nMindam2*150/100:pItemTxt->nMindam2,
				ethereal?pItemTxt->nMaxdam2*150/100:pItemTxt->nMaxdam2);
		if (pItemTxt->wReqstr) fprintf(fp," 需要力量%d",pItemTxt->wReqstr+(ethereal?-10:0));
		if (pItemTxt->wReqDex) fprintf(fp," 需要敏捷%d",pItemTxt->wReqDex+(ethereal?-10:0));
		fputc(')',fp);
		if (debug) fprintf(fp," pItem=%x pTxt=%x",pUnit,pItemTxt);
		fprintf(fp," %d*%d",pItemTxt->nInvwidth,pItemTxt->nInvheight);
		if (debug) {
			fprintf(fp,"@(%d,%d)",pUnit->pItemPath->dwPosX,pUnit->pItemPath->dwPosY);
		}
		//if (pItemTxt->nSocket) fprintf(fp," 最多%d孔",pItemTxt->nSocket);
		if (pUnit->pItemData->dwFingerPrint) fprintf(fp," finger=%x",pUnit->pItemData->dwFingerPrint);
		int fileindex;
		switch (dwQuality) {
			case ITEM_QUALITY_RARE:fprintf(fp," 亮金");break;
			case ITEM_QUALITY_MAGIC:fprintf(fp," 魔法");break;
			case ITEM_QUALITY_UNIQUE: {
				fileindex = D2GetItemFileIndex(pUnit);
				fprintf(fp," 暗金U%03d %s",fileindex,uniqueNames[fileindex]);
				UniqueItemTxt *pUniqueTxt = D2GetUniqueItemTxt(fileindex);
				if (pUniqueTxt) {
					if (pUniqueTxt->wReqlvl) fprintf(fp," 需要等级%d",pUniqueTxt->wReqlvl);
					fprintf(fp," %s",pUniqueTxt->szDesc);
				}
				break;
			}
			case ITEM_QUALITY_SET: {
				fileindex = D2GetItemFileIndex(pUnit);
				fprintf(fp," 套装SET%03d %s",fileindex,setItemNames[fileindex]);
				SetItemTxt *pSetTxt = D2GetSetItemTxt(fileindex);
				if (pSetTxt) {
					if (pSetTxt->wReqlvl) fprintf(fp," 需要等级%d",pSetTxt->wReqlvl);
					fprintf(fp," %s",pSetTxt->szDesc);
				}
				break;
			}
			default:
				if ( D2CheckItemFlag(pUnit, ITEMFLAG_RUNEWORD, 0, "?") ) {
					fileindex = D2GetRuneWordTxtIndex( pUnit->pItemData->wMagicPrefix[0] );
					fprintf(fp," 神符之语RW%03d %s",fileindex,runeWordNames[fileindex]);
					needSocket=0;
					RuneWordTxt *pRuneWordTxt = D2GetRuneWordTxt(fileindex);
					if (pRuneWordTxt) {
						fprintf(fp," %s",pRuneWordTxt->szName);
					}
				}
		}
		if (pUnit->pInventory&&pUnit->pInventory->dwFilledSockets) {
			needSocket=0;
			fprintf(fp," 已用%d孔",pUnit->pInventory->dwFilledSockets);
			//dumpItems(fp,pUnit->pInventory->pFirstItem,level+1);
		}
		if (pUnit->pStatList) {
			StatList *plist=pUnit->pStatList;
			dumpStatList(fp,plist,0);
		}
		fprintf(fp,"\n");
}
struct myItem {
	int sort;
	int location;
	UnitAny *p;
};
static int compareItem(const void *arg1,const void *arg2) {
	return ((struct myItem *)arg1)->sort - ((struct myItem *)arg2)->sort;
}
static void dumpItems(FILE *fp,UnitAny *pUnit,int level) {
	static struct myItem items[512];
	if (!pUnit) return;
	int n=0;
	nLoc[0]=0;nLoc[1]=0;nLoc[2]=0;nLoc[3]=0;
	while (pUnit) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) goto next;
		int index = GetItemIndex(pUnit->dwTxtFileNo)+1;//all config arrays are based 1
		ItemTxt *pItemTxt = D2GetItemTxt( pUnit->dwTxtFileNo );
		int location= pUnit->pItemData->nItemLocation;
		int sort=10,loc=10;
		switch (location) {
			case 255: loc=0;break;//body
			case 0: loc=1;break;//bag
			case 3: loc=2;break;//cube
			case 4: loc=3;break;//box
		}
		nLoc[loc]+=pItemTxt->nInvwidth*pItemTxt->nInvheight;
		if ( index >2999 ) index= 2999;
		if (!debug) {
			if (2080<=index&&index<=2089) goto next; //healing and mana potion
			if (!debug&&(index==2019||index==2021)) goto next; //arrow
			if (index==2022||index==2023) goto next; //scroll
			if (2006<=index&&index<=2012) goto next; //potions and book
			if (level==0&&2103<=index&&index<=2135) goto next;//rune
			if (2050<=index&&index<=2079) goto next;//gem
			if (2090<=index&&index<=2094) goto next;//skull
		}
		DWORD dwQuality = pUnit->pItemData->dwQuality;
		if (pUnit->pItemData->nItemLocation==255) { //equipped
			sort=pUnit->pItemData->nBodyLocation&0xFF;
			if (!sort) sort=100;
		} else {
			switch (dwQuality) {
				case ITEM_QUALITY_UNIQUE:sort=1;break;
				case ITEM_QUALITY_SET:sort=2;break;
				case ITEM_QUALITY_RARE:sort=3;break;
				case ITEM_QUALITY_MAGIC:sort=4;break;
				default:
					if ( D2CheckItemFlag(pUnit, ITEMFLAG_RUNEWORD, 0, "?")) sort=0;
			}
		}
		items[n].sort=(loc<<24)|(sort<<16)|index;
		items[n].location=loc;
		items[n].p=pUnit;
		n++;
		if (n>=512) break;
next:
		pUnit = D2GetNextItemInInv(pUnit);
	}
	qsort(items,n,sizeof(struct myItem),compareItem);
	for (int i=0;i<n;i++)
		dumpItem(fp,items[i].p,level,items[i].location);
}
struct Q {char q,pos;unsigned short mask;const char *name;};
static u16 *questData;
static void dumpTodo(FILE *fp,int d,char *name,struct Q *qs) {
	int has=0;
	for (struct Q *p=qs;p->q;p++) {
		BOOL f = questData[p->pos]&p->mask;
		if (!f) {has=1;break;}
	}
	if (!has) return;
	fprintf(fp,"%s%d:",name,d);
	for (struct Q *p=qs;p->q;p++) {
		BOOL f = questData[p->pos]&p->mask;
		if (!f) fprintf(fp," Q%d%s",p->q,p->name);
	}
	fprintf(fp,"\n");
}
static void dumpQuests(FILE *fp) {
	static struct Q todos[]={{11,1,1,"+1skill"},
	{21,9,0x8001,"+1skill"}, //after done 0x8012, after talk 0x9011
	{31,20,1,"+20HP"},{34,17,1,"+5stat"},{41,25,1,"+2skill"},{53,37,1<<8,"+10resist"},{0,0,0}};
	static struct Q rewards[]={
		{42,27,3,"gem"}, //finish:8002
		{51,35,1,"socket"},//not socket:8022  socketed:9021
		{52,36,1,"rune789"},{0,0,0}};
	char ext[16];
	int hasD[3]={0};
	u16 qdata[3][64];
	for (int d=0;d<=2;d++) {
		sprintf(ext,"quest%d.bin",d);
		FILE *fp2=openSnapshotFile("dat",ext,"rb",'_',"quest",0);
		if (fp2) {
			hasD[d]=1;
			int pos=readFully(fp2,(char *)qdata[d],84);
			fclose(fp2);
		}
	}
	for (int d=0;d<=2;d++) {
		if (!hasD[d]) continue;
		questData=qdata[d];
		fprintf(fp,"Quest%d: ",d);
		fputc((questData[1]&1)?'X':'-',fp);
		fputc((questData[2]&1)?'X':'-',fp);
		fputc((questData[4]&1)?'X':'-',fp);
		fputc((questData[5]&1)?'X':'-',fp);
		fputc((questData[3]&1)?'X':'-',fp);
		fputc((questData[6]&1)?'X':'-',fp);
		if (!(questData[7]&1)) goto end;
		fputc('|',fp);
		fputc((questData[9]&1)?'X':'-',fp);
		fputc((questData[10]&1)?'X':'-',fp);
		fputc((questData[11]&1)?'X':'-',fp);
		fputc((questData[12]&1)?'X':'-',fp);
		fputc((questData[13]&1)?'X':'-',fp);
		fputc((questData[14]&1)?'X':'-',fp);
		if (!(questData[15]&1)) goto end;
		fputc('|',fp);
		fputc((questData[20]&1)?'X':'-',fp);
		fputc((questData[19]&1)?'X':'-',fp);
		fputc((questData[18]&1)?'X':'-',fp);
		fputc((questData[17]&1)?'X':'-',fp);
		fputc((questData[21]&1)?'X':'-',fp);
		fputc((questData[22]&1)?'X':'-',fp);
		if (!(questData[23]&1)) goto end;
		fputc('|',fp);
		fputc((questData[25]&1)?'X':'-',fp);
		fputc((questData[27]&3)?'X':'-',fp);
		fputc((questData[26]&1)?'X':'-',fp);
		if (!(questData[28]&1)) goto end;
		fputc('|',fp);
		fputc((questData[35]&1)?'X':'-',fp);
		fputc((questData[36]&1)?'X':'-',fp);
		fputc((questData[37]&1)?'X':'-',fp);
		fputc((questData[38]&1)?'X':'-',fp);
		fputc((questData[39]&1)?'X':'-',fp);
		fputc((questData[40]&1)?'X':'-',fp);
		if (!(questData[39]&1)) goto end;
		fputc('|',fp);
		fputc((questData[40]&1)?'P':'-',fp);
end:
		fprintf(fp,"\n");
	}
	for (int d=0;d<=2;d++) {
		if (!hasD[d]) continue;
		questData=qdata[d];
		dumpTodo(fp,d,"TODO",todos);
	}
	for (int d=0;d<=2;d++) {
		if (!hasD[d]) continue;
		questData=qdata[d];
		dumpTodo(fp,d,"REWARDS",rewards);
	}
	fprintf(fp,"RESET:");
	for (int d=0;d<=2;d++) if (hasD[d]&&!(qdata[d][41]&1)) fprintf(fp," %d",d);
	fprintf(fp,"\n");
	fprintf(fp,"COW:");
	for (int d=0;d<=2;d++) if (hasD[d]&&!(qdata[d][4]&(1<<10))) fprintf(fp," %d",d);
	fprintf(fp,"\n");
	fprintf(fp,"A2BugMF:");
	for (int d=0;d<=2;d++) if (hasD[d]&&!(qdata[d][14]&(1<<5))) fprintf(fp," %d",d);
	fprintf(fp,"\n");
}
static void dumpWaypoints(FILE *fp) {
	char ext[16],buf[21];
	for (int d=0;d<=2;d++) {
		sprintf(ext,"wp%d.bin",d);
		FILE *fp2=openSnapshotFile("dat",ext,"rb",'_',"waypoint",0);
		if (!fp2) continue;
		int pos=readFully(fp2,buf,21);
		fclose(fp2);
		LONGLONG wp=*(LONGLONG *)(buf+7);
		fprintf(fp,"Waypoint%d: ",d);
		for (int i=0;i<5;i++) {
			if (i) fprintf(fp,"|");
			int n=i==3?3:9;
			for (int j=0;j<n;j++) {fputc((wp&1)?'X':'_',fp);wp>>=1;}
		}
		fputc('\n',fp);
	}
}
static void dumpSkill(FILE *fp,struct Skill *skill) {
	if (!skill) return;
	int id=skill->pSkillInfo->wSkillId;
	fprintf(fp," %d:%s",id,skillNames[id]);
	fprintf(fp,"(%d",skill->dwSkillLevel);
	if (skill->dwLevelBonus) fprintf(fp,"+%d",skill->dwLevelBonus);
	fputc(')',fp);
	if (skill->dwQuality) fprintf(fp,"(%d)",skill->dwQuality);
}
static void dumpSkills(FILE *fp,UnitAny *pUnit) {
	if (!pUnit||!pUnit->pSkill||!pUnit->pSkill->pFirstSkill) return;	
	if (pUnit->pSkill->pLeftSkill) {fputs(" 左键技能:",fp);dumpSkill(fp,pUnit->pSkill->pLeftSkill);fputc('\n',fp);}
	if (pUnit->pSkill->pRightSkill) {fputs(" 右键技能:",fp);dumpSkill(fp,pUnit->pSkill->pRightSkill);fputc('\n',fp);}
	fprintf(fp," 技能:");
	for (struct Skill *p=pUnit->pSkill->pFirstSkill;p;p=p->pNextSkill) dumpSkill(fp,p);
	fputc('\n',fp);
}
static void dumpObject(FILE *fp,UnitAny *pUnit) {
	if (!pUnit) return;
	if (pUnit->pObjectData) {
		//119 waypoints
		//59 blue portal
		//60 red portal
		if (pUnit->dwTxtFileNo==0x3b //blue portal
			||pUnit->dwTxtFileNo==0x3c //red portal
			) {
			fprintf(fp," Portal to area %d",pUnit->pObjectData->nShrineNo);
		}
	}
	fputc('\n',fp);
	//hex(fp,0,pUnit->pObjectData,64);
}
static void dumpRoomtile(FILE *fp,UnitAny *pUnit) {
	if (!pUnit) return;
	for(RoomTile *pRoomtile = pUnit->pItemPath->pRoom1->pRoom2->pRoomTiles ;pRoomtile ; pRoomtile = pRoomtile->pNext ){
		if (*(pRoomtile->pNum)==pUnit->dwTxtFileNo) {
			int lvl=pRoomtile->pRoom2->pDrlgLevel->dwLevelNo;
			fprintf(fp," connect to level=%d\n",lvl);
		}
	}
}
static void dumpUnit(FILE *fp,UnitAny *pUnit) {
	if (!pUnit) return;
	int unitType=pUnit->dwUnitType;
	DWORD dwUnitId = pUnit->dwUnitId; 
	if (pUnit->dwUnitType==UNITNO_PLAYER) {
		fprintf(fp,"Lv %d.%04d %s\n",D2GetUnitStat(PLAYER, STAT_LEVEL, 0), getLvPercent(pUnit),
			pUnit->pPlayerData->szName);
		if (dwUnitId == dwPlayerId) {
			if (*p_D2MercData16 != 0xFFFF)
				fprintf(fp,"Merc is dead: name=%x %x\n",*p_D2MercData16,*p_D2MercData32);
			dumpQuests(fp);
			dumpWaypoints(fp);
		}
		if (pUnit->pStatList) {
			StatList *plist=pUnit->pStatList;
			dumpStatList(fp,plist,1);
		}
		fprintf(fp,"\n");
		if (debug&&fSinglePlayer) {
			UnitAny *real=getSinglePlayerUnit(dwUnitId,unitType);
			if (real) {
				fprintf(fp,"  BACKEND %x",real);fflush(fp);
				StatList *plist=real->pStatList;
				if (plist&&!IsBadReadPtr(plist,sizeof(StatList)))
					dumpStatList(fp,plist,1);
			}
			fprintf(fp,"\n");
		}
		dumpSkills(fp,pUnit);
		fprintf(fp,"\n");
		UnitAny *pItem = pUnit->pInventory->pCursorItem;
		if (pItem) {
			fprintf(fp,"Cursor Item:\n");
			dumpItem(fp,pItem,1,-1);
		}
		pItem = D2GetFirstItemInInv(pUnit->pInventory);
		countGemRunes(fp,pItem);
		lastLocation=-1;
		dumpItems(fp,pItem,0);
	} else if ( pUnit->dwUnitType==UNITNO_MONSTER ){
		char name[64],def='?';BOOL used=FALSE;
		MonsterTxt *pMonsterTxt = pUnit->pMonsterData->pMonsterTxt;
		WideCharToMultiByte(CP_ACP,0,pUnit->pMonsterData->wszMonName,-1,name,64,&def,&used);
		fprintf(fp,"TXT%d ",pUnit->dwTxtFileNo);
		if ( pMonsterTxt->fNpc ) fprintf(fp,"NPC ");
		fprintf(fp,"%s ",name);
		if (pUnit->pStatList) {
			StatList *plist=pUnit->pStatList;
			dumpStatList(fp,plist,1);
		}
		fflush(fp);
		if (debug&&fSinglePlayer) {
			UnitAny *real=getSinglePlayerUnit(dwUnitId,unitType);
			if (real) {
				fprintf(fp,"  BACKEND %x",real);fflush(fp);
				StatList *plist=real->pStatList;
				if (plist&&!IsBadReadPtr(plist,sizeof(StatList)))
					dumpStatList(fp,plist,1);
			}
		} else {
			fprintf(fp,"\n");
		}
		dumpSkills(fp,pUnit);
		if (pUnit->pInventory) dumpItems(fp,D2GetFirstItemInInv(pUnit->pInventory),0);
		if (debug) fprintf(fp,"\n");
		//hex(fp,0,pUnit,0x100);
		//hex(fp,0,pUnit->pMonsterData,0x80);
	} else if ( pUnit->dwUnitType==UNITNO_OBJECT ){
		dumpObject(fp,pUnit);
	} else if ( pUnit->dwUnitType==UNITNO_ITEM ){
		dumpItem(fp,pUnit,0,-1);
	} else if ( pUnit->dwUnitType==UNITNO_ROOMTILE ){
		dumpRoomtile(fp,pUnit);
	}
}
void __fastcall saveWaypoints(char *packet,int len) {
	//from offset 5, length 16
	char ext[16];sprintf(ext,"wp%d.bin",DIFFICULTY);
	FILE *fp=openSnapshotFile("dat",ext,"wb+",'_',"waypoint",1);
	if (fp) {fwrite(packet,len,1,fp);fclose(fp);}
}
int d2s_generate(FILE *fp, UnitAny *pUnit,UnitAny *m,u16 *quest,char *wp,UnitAny *ironGolem);
static void dumpInventory() {
	FILE *fp;
	char ext[16];
	sprintf(ext,"quest%d.bin",DIFFICULTY);
	if (QUESTDATA) {
		u8 *ptr=(u8 *)(*(int **)QUESTDATA);
		fp=openSnapshotFile("dat",ext,"wb+",'_',"quest",1);
		if (fp) {fwrite(ptr,84,1,fp);fclose(fp);}
	}
	fp=openSnapshotFile(NULL,"txt","w+",'_',"inventory",1);if (!fp) return;
	dumpUnit(fp,PLAYER);
	UnitAny *m=NULL,*golem=NULL;
	PetUnit *pGolem = GetPetByType(PLAYER,3);
	if (pGolem&&pGolem->dwTxtFileNo==291) {
		golem=GetUnitFromIdSafe(pGolem->dwUnitId , UNITNO_MONSTER ) ;
		if (golem) {
			fprintf(fp,"------- Iron Golem -------\n");
			lastLocation=0;
			dumpUnit(fp,golem);
		}
	}
	fclose(fp);
	PetUnit *pMerc = GetPetByType(PLAYER,7);
	if (pMerc) {
		m=GetUnitFromIdSafe(pMerc->dwUnitId , UNITNO_MONSTER ) ;
		if (m) {
			fp=openSnapshotFile(NULL,"m.txt","w+",'_',"mercenary",1);if (!fp) return;
			lastLocation=0;
			dumpUnit(fp,m);
			fclose(fp);
		}
	}
	u16 quest[3][48]={0};
	char wp[3][24]={0};
	for (int d=0;d<=2;d++) {
		sprintf(ext,"quest%d.bin",d);
		FILE *fp2=openSnapshotFile("dat",ext,"rb",'_',"quest",0);
		if (fp2) {
			int pos=readFully(fp2,(char *)quest[d],96);
			fclose(fp2);
		}
		sprintf(ext,"wp%d.bin",d);
		fp2=openSnapshotFile("dat",ext,"rb",'_',"waypoint",0);
		if (fp2) {
			fseek(fp2,5,0);
			int pos=readFully(fp2,(char *)wp[d],24);
			fclose(fp2);
		} else {
			static unsigned char wpDef[7]={2,1,0xFF,0xFF,0xFF,0xFF,0x77};
			memcpy((char *)wp[d],wpDef,7);
		}
	}
	if (tSnapshotSaveD2s.isOn) {
		UnitAny *ironGolem=NULL;
		PetUnit *pPetUnit = GetPetByType(PLAYER,3);
		if (pPetUnit&&pPetUnit->dwTxtFileNo==291) {
			ironGolem=GetUnitFromIdSafe(pPetUnit->dwUnitId , UNITNO_MONSTER );
		}
		fp=openSnapshotFile(NULL,"d2s","wb+",'\\',"d2s",1);if (!fp) return;
		d2s_generate(fp,PLAYER,m,(u16 *)&quest,(char *)&wp,ironGolem);
		fclose(fp);
	}
}

FILE *dbgFp=NULL,*dumpPlayerFp=NULL,*dumpMonsterFp=NULL,*dumpObjectFp=NULL;
void dumpUnit2(FILE *fp,UnitAny *pUnit) {
	int unitType=pUnit->dwUnitType;
	DWORD dwUnitId = pUnit->dwUnitId; 
	fprintf(fp,">>> type=%d id=%d pUnit=0x%x mode=0x%x txt=%d",
		unitType,dwUnitId,pUnit,pUnit->dwMode,pUnit->dwTxtFileNo);
	if ((unitType==UNITNO_PLAYER||unitType==UNITNO_MONSTER)&&pUnit->pMonPath) {
		//fprintf(fp," offset=(%d,%d)",pUnit->pMonPath->wOffsetX,pUnit->pMonPath->wOffsetY);
		fprintf(fp," pos=(%d,%d)",pUnit->pMonPath->wPosX,pUnit->pMonPath->wPosY);
		//fprintf(fp," map=(%d,%d)",pUnit->pMonPath->dwMapPosX,pUnit->pMonPath->dwMapPosY);
		//fprintf(fp," target=(%d,%d)",pUnit->pMonPath->wTargetX,pUnit->pMonPath->wTargetY);
	} else if ((unitType==UNITNO_OBJECT||unitType==UNITNO_ITEM||unitType==UNITNO_ROOMTILE)&&pUnit->pItemPath) {
		fprintf(fp," pos=(%d,%d)",pUnit->pItemPath->dwPosX,pUnit->pItemPath->dwPosY);
		//fprintf(fp," map=(%d,%d)",pUnit->pItemPath->dwMapPosX,pUnit->pItemPath->dwMapPosY);
	}
	if (pUnit->dwUnitType==UNITNO_MONSTER) {
		int owner=D2GetMonsterOwner(pUnit->dwUnitId);
		if (owner!=-1) fprintf(fp," owner=%d",owner);
	}
	fprintf(fp,"\n");
	dumpUnit(fp,pUnit);
	if (0&&pUnit->pStatList&&pUnit->pStatList->dwListFlag&0x80000000) {
		fprintf(fp,"\tstate:\n");
		//pUnit->pStatList->StatFlags
	}
	fprintf(fp,"\n");
}
void dumpScanUnit(UnitAny *pUnit) {
	int unitType=pUnit->dwUnitType;
	DWORD dwUnitId = pUnit->dwUnitId; 
	int owner=-1;
	FILE *fp=dbgFp;
	if (pUnit->dwUnitType==UNITNO_MONSTER) owner=D2GetMonsterOwner(pUnit->dwUnitId);
	if (pUnit->dwUnitType==UNITNO_PLAYER&&dwUnitId==dwPlayerId) {
		fp=openDbgFile("_self.txt");
		dumpUnit2(fp,pUnit);
		fclose(fp);return;
	} else if (pUnit->dwUnitType==UNITNO_MONSTER&&owner==dwPlayerId) {
		fp=openDbgFile("_merc.txt");
		dumpUnit2(fp,pUnit);
		fclose(fp);return;
	} else if (unitType==UNITNO_PLAYER||owner!=-1) fp=dumpPlayerFp;
	else if (unitType==UNITNO_MONSTER) fp=dumpMonsterFp;
	else if (unitType==UNITNO_OBJECT) fp=dumpObjectFp;
	if (!fp) return;
	//if (pUnit->dwUnitType==UNITNO_ROOMTILE||pUnit->dwUnitType==UNITNO_OBJECT)
	dumpUnit2(fp,pUnit);
}
void dumpScanUnitEnd() {
	if (!dbgFp) return;
	fclose(dbgFp);dbgFp=NULL;enDumpUnit=0;debug=0;
	if (dumpPlayerFp) {fclose(dumpPlayerFp);dumpPlayerFp=NULL;}
	if (dumpMonsterFp) {fclose(dumpMonsterFp);dumpMonsterFp=NULL;}
	if (dumpObjectFp) {fclose(dumpObjectFp);dumpObjectFp=NULL;}
}
void SnapshotRunLoop() {
	if (enDumpUnit) { //Log.cpp dump surrounding units
		enDumpUnit++;
		if (enDumpUnit>=3) {
			dumpScanUnitEnd();enDumpUnit=0;
		}
	}
}
void dumpLayers() {
	FILE *fp=openDbgFile("_map.txt");if (!fp) return;
	fprintf(fp,"layer: %d\n",LAYER->dwLayerNo);
	fprintf(fp,"floor:\n");
	for (AutomapCell *p=LAYER->pFloors;p;p=p->pMore) {
		fprintf(fp,"%x\t%d\t%d\t%d\t%d\n",p,p->xPixel,p->yPixel,p->wCellNo,p->wWeight);
		int n=0;
		for (AutomapCell *p2=p->pLess;p2;p2=p2->pLess) {
			fprintf(fp,"\t%x\t%d\t%d\t%d\t%d\n",p2,p2->xPixel,p2->yPixel,p2->wCellNo,p2->wWeight);
			if (n++>100) break;
		}
	}
	fprintf(fp,"wall:\n");
	for (AutomapCell *p=LAYER->pWalls;p;p=p->pMore) {
		fprintf(fp,"%d\t%d\t%d\t%d\n",p->xPixel,p->yPixel,p->wCellNo,p->wWeight);
	}
	fclose(fp);
}
int DoSnapshot() {
	if (!fInGame) return 0;
	debug=0;
	wchar_t temp[256];
	wsprintfW(temp,  L"d2hackmp snapshot %hs",szSnapshotPath);
	D2ShowGameMessage(temp, 0);
	dumpInventory();
	
	FILE *fp=openDbgFile("_quest.txt");
	if (GAMEQUESTDATA) {
		u16 *s=(u16 *)(*(int **)GAMEQUESTDATA);
		fprintf(fp,"Game:\n");
		for (int i=0;i<=40;i++) {
			fprintf(fp,"%d %s: %04x",i,questNames[i],s[i]);
			fputc(i%2==0?'\t':'\n',fp);
		}
		fputc('\n',fp);
	}
	if (QUESTDATA) {
		u16 *s=(u16 *)(*(int **)QUESTDATA);
		fprintf(fp,"Player:\n");
		for (int i=0;i<=40;i++) {
			fprintf(fp,"%d %s: %04x",i,questNames[i],s[i]);
			fputc(i%2==0?'\t':'\n',fp);
		}
		fputc('\n',fp);
	}
	fclose(fp);
	fp=openDbgFile("_pet.txt");
	for (PetUnit *pPetUnit = *p_D2PetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) {
		fprintf(fp,"%x txt=%d type=%d",pPetUnit,pPetUnit->dwTxtFileNo,pPetUnit->dwPetType);
		fprintf(fp," owner(type=%d,id=%d)\n",pPetUnit->dwOwerType,pPetUnit->dwOwnerId);
		hex(fp,0,pPetUnit,0x40);
	}
	fclose(fp);
	dbgFp=openDbgFile("_debug.txt");
	if (!dbgFp) return 0;
	dumpPlayerFp=openDbgFile("_players.txt");
	dumpMonsterFp=openDbgFile("_monsters.txt");
	dumpObjectFp=openDbgFile("_objects.txt");
	fprintf(dbgFp,"DoSnapshot=%x\n",&DoSnapshot);
	debug=1;
	UnitAny *pUnit = D2GetSelectedUnit();
	if (pUnit) {
		fprintf(dbgFp,"============ Selected Unit =============\n");
		dumpUnit2(dbgFp,pUnit);
	}
	fprintf(dbgFp,"============ Scan Units =============\n");
	enDumpUnit=1;
	if (fSinglePlayer) singlePlayerSaveGame("d:\\git\\diablo2\\t\\test.d2s");
	dumpLayers();

	return 0;
}
void __declspec(naked) __fastcall GetMercInfo(
	int expansion, UnitAny *player, int seed,int arg,int diff,void *buf) {
	__asm {
		push ebp
		mov ebp, esp
		mov eax, buf
		push eax
		mov eax, diff
		push eax
		mov eax, arg
		push eax
		mov eax, seed
		push eax
		call D2GetMercInfo
		pop ebp
		ret 0x10
	}
}
extern int dwCheckMercMs;
void __fastcall saveMercInfo(char *packet) {
	int ownerId=*(int *)(packet+4);
	int mercId=*(int *)(packet+8);
	if (ownerId==dwPlayerId) {
		dwMercSeed=*(int *)(packet+0xc);
		int pettype=packet[1]&0xFF;
		int txt=*(WORD *)(packet+2);
		dwMercNameId=*(WORD *)(packet+0x10);
		int mercAct=2;
		if (0xd53<=dwMercNameId&&dwMercNameId<=0xd7b) {
			mercAct=1;
		} else if (0x3fb<=dwMercNameId&&dwMercNameId<=0x40f) {
			mercAct=2;
		} else if (0x410<=dwMercNameId&&dwMercNameId<=0x423) {
			mercAct=3;
		} else if (0x2a53<=dwMercNameId&&dwMercNameId<=0x2a95) {
			mercAct=5;
		}
		dwCheckMercMs=dwCurMs+500;
		if (logfp) {
			//char buf[256];
			//memset(buf,0,256);
			//GetMercInfo(*p_D2Expansion,PLAYER,dwMercSeed,mercAct-1,*p_D2Difficulty, buf);
			//fprintf(logfp,"merc type: %x\n",*(int *)buf);
			//hex(logfp,0,buf,64);
			/*for (int i=0;i<3;i++) {
				memset(buf,0,256);
				GetMercInfo(*p_D2Expansion,PLAYER,dwMercSeed,mercAct-1,i, buf);
				fprintf(logfp,"merc diff:%d type: %x\n",i,*(int *)buf);
				hex(logfp,0,buf,64);
			}
			*/
			//fprintf(logfp,"receive merc packet:\n"); 
			//hex(logfp,0,packet,20);
			fprintf(logfp,"Merc txt=%x name=%x seed=%x\n",txt,dwMercNameId,dwMercSeed);
			fflush(logfp);
		}
	}
}
/*
ret=6FB5CFB5
can modify: eax,edx
ecx,edi=buf edx=len
6FB5F8B0 - 83 EC 18              - sub esp,18 { 24 }
6FB5F8B3 - 8B 01                 - mov eax,[ecx]
6FB5F8B5 - 8B 51 04              - mov edx,[ecx+04]
*/
void __declspec(naked) RecvCommand_63_Patch_ASM() {
	__asm {
		pushad
		call saveWaypoints
		popad
		mov eax,[ecx]
		mov edx,[ecx+4]
		ret
	}
}
void __declspec(naked) RecvCommand_81_Patch_ASM() {
	__asm {
		pushad
		call saveMercInfo
		popad
		mov eax,[ecx+0xc]
		mov edx,[ecx+0x10]
		ret
	}
}
