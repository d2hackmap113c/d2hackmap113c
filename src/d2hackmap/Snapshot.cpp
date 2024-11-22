#include "stdafx.h"
#include "d2ptrs.h"

#define printf ERROR
static int debug=0;
struct ItemIdName {int id;const char *name;};
#define MAX_NAMES 512
static int hasValuableEquipment;
int dwMercSeed=0,dwMercNameId;
char *questNames[41];
static char *clsNames[8]={"Ama","Sor","Nec","Pal","Bar","Dru","Asn","???"};
static char *clsSkillNames[8]={"AmaSkill","SorSkill","NecSkill","PalSkill","BarSkill","DruSkill","AsnSkill","???"};
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

int DoSnapshot();
char szSnapshotPath[256]="snapshot";
char szSnapshotNamesPath[256]="names.txt";
ToggleVar 	tDoSnapshot={			  TOGGLEVAR_DOWN,	0,	-1,	 1, "Do Snapshot",			&DoSnapshot};
ToggleVar 	tSnapshotNewGame={		TOGGLEVAR_ONOFF,	1,	-1,	 1,	"Snapshot Enter Game"};
ToggleVar 	tSnapshotEndGame={		TOGGLEVAR_ONOFF,	1,	-1,	 1,	"Snapshot Exit Game"};
ToggleVar 	tSnapshotLog={				TOGGLEVAR_ONOFF,	1,	-1,	 1,	"Enable Snapshot Log"};
ToggleVar 	tSnapshotSaveD2s={				TOGGLEVAR_ONOFF,	0,	-1,	 1,	"Snapshot d2s file"};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_STR, "SnapshotPath",		szSnapshotPath, 1,  {255 }},
	{CONFIG_VAR_TYPE_STR, "SnapshotNamesPath",		szSnapshotNamesPath, 1,  {255 }},
	{CONFIG_VAR_TYPE_KEY, "SnapshotKey",			&tDoSnapshot     },
	{CONFIG_VAR_TYPE_KEY, "SnapshotNewGame",			&tSnapshotNewGame     },
	{CONFIG_VAR_TYPE_KEY, "SnapshotEndGame",			&tSnapshotEndGame     },
	{CONFIG_VAR_TYPE_KEY, "SnapshotLog",			&tSnapshotLog     },
	{CONFIG_VAR_TYPE_KEY, "SnapshotSaveD2s",			&tSnapshotSaveD2s     },
};
void Snapshot_AddConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}

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
void getSnapshotPath(char *buf,char *realm,char *account,char *name,
	char *subfolder,char *ext,char sep) {
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
			if (name) sprintf(buf+len,"\\%s_%s.%s",account,name,ext);
			else sprintf(buf+len,"\\%s.%s",account,ext);
		}
	} else {
		if (subfolder) {
			strcat(buf,"\\");strcat(buf,subfolder);CreateDirectoryA(buf,NULL);
		}
		int len=strlen(buf);
		sprintf(buf+len,"\\%s.%s",name,ext);
	}
}
static FILE *openSnapshotFile(char *subfolder,char *ext,char *mode,char sep,char *action,int msg) {
	char buf[1024];
	char *realm=(*d2client_pGameInfo)->szRealmName;
	char *account=(*d2client_pGameInfo)->szAccountName;
	char *name=(*d2client_pGameInfo)->szCharName;
	getSnapshotPath(buf,realm,account,name,subfolder,ext,sep);
	FILE *fp=fopen(buf,mode);
	if (!msg) return fp;
	if (!fp) {
		LOG("can't open %s for %s\n", buf,action);
	} else {
		LOG(" %s to %s\n", action,buf);
		/*wchar_t temp[256];
		wsprintfW(temp,  L"%hs to %hs", action,buf);
		d2common_ShowGameMessage(temp, 0);
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
	int exp = d2common_GetUnitStat(pUnit, STAT_EXP, 0);
	DWORD lvl = d2common_GetUnitStat(pUnit, STAT_LEVEL, 0);
	DWORD player_class = pUnit->dwTxtFileNo;
	int nextlvlexp = d2common_GetMinExpToLevel(player_class, lvl);
	int currentlvlexp = 0;
	if (lvl>1) currentlvlexp = d2common_GetMinExpToLevel(player_class, lvl-1);
	int totalexpneedtoupgrade = nextlvlexp - currentlvlexp;
	int currentlvlgainedexp = exp - currentlvlexp;
	if (totalexpneedtoupgrade>200000) {
		return (int)((double)currentlvlgainedexp*10000.0/(double)totalexpneedtoupgrade);
	} else
		return currentlvlgainedexp*10000/totalexpneedtoupgrade;
}
void SnapshotNewGame() {
	startGameMs=GetTickCount();
	startLv = d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0);
	startLvPercent=getLvPercent(PLAYER);
	dwMercSeed=0;dwMercNameId=0;
	if (tSnapshotNewGame.isOn) dumpInventory();
	if (logfp&&*d2client_pGameInfo) {
		char *game=(*d2client_pGameInfo)->szGameName;
		char *name=(*d2client_pGameInfo)->szCharName;
		if (game) fprintf(logfp,"Game:%s",game);
		if (name) fprintf(logfp," Character:%s",name);
		fprintf(logfp," %s Lv%d.%04d\n",clsNames[dwPlayerClass],startLv,startLvPercent);
	}
}
void SnapshotEndGame() {
	if (tSnapshotLog.isOn&&GetTickCount()-startGameMs>60000) {
		FILE *fp=openSnapshotFile("log","log","a+",'_',"log",1);
		if (fp) {
			char *ip=(*d2client_pGameInfo)->szGameServerIp;
			char *game=(*d2client_pGameInfo)->szGameName;
			char *password=(*d2client_pGameInfo)->szGamePassword;
			char *realm=(*d2client_pGameInfo)->szRealmName;
			DWORD lvl = d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0);
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
static void dumpStat(FILE *fp,int id,int param,int value) {
	int unknown=id<0||id>=MAX_NAMES||!propNames[id];
	if (unknown||debug) {
		fprintf(fp,"S(%d",id);
		if (value||param) {
			fprintf(fp,",%d",value);
			if (param) {
				if (param>10000) fprintf(fp,",0x%x)",param);
				else fprintf(fp,",%d)",param);
			}
		}
		fprintf(fp,")");
		if (unknown) return;
	}
	switch (id) {
		case 7:
		case 9:
		case 11:
			fprintf(fp,propNames[id],value>>8);break;
		case 83:fprintf(fp,propNames[id],value,clsSkillNames[param]);break;
		case 97: 
		case 107:
		case 151:
			fprintf(fp,propNames[id],value,skillNames[param],"");break;
		case 188:fprintf(fp,propNames[id],value,clsSkills[param>>3][param&3],clsNames[param>>3]);break;
		case 195:
		case 196:
		case 197:
		case 198:
		case 199:
		case 201:
			fprintf(fp,propNames[id],value,param&0x3f,skillNames[param>>6]);break;
		case 204:
			fprintf(fp,propNames[id],param&0x3f,skillNames[param>>6],value&0xFF,value>>8);
			break;
		case 216:case 217:
			fprintf(fp,propNames[id],value/2048.0f,param);break;
			break;
		default:
			if (214<=id&&id<=250) {
				fprintf(fp,propNames[id],value/8.0f,param);break;
			} else {
				fprintf(fp,propNames[id],value,param);break;
			}
	}
}
static int dumpStats(FILE *fp,Stat *stat) {
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
		int param=se[i].wParam&0xFFFF;
		fputc(' ',fp);
		switch (id) {
			case 6:
			case 8:
			case 10:
				if (i<n-1&&se[i+1].wStatId==id+1) {
					fprintf(fp,propNames[id],value>>8,se[i+1].dwStatValue>>8);
					i++;
				}
				break;
			case 39:fr+=value;break;
			case 41:lr+=value;break;
			case 43:cr+=value;break;
			case 45:pr+=value;break;
			case 194:
				sockets=value;
				fprintf(fp," 有%d孔",value);
				//if (debug) fprintf(fp," %x",&se[i].dwStatValue);
				break;
			default:
				dumpStat(fp,id,param,value);
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
BYTE GetItemColour(UnitAny *pItem,int isMinimap);
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
		pUnit = d2common_GetNextItemInInv(pUnit);
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
			fprintf(fp,"  BaseStat:");nsockets=dumpStats(fp,&plist->sBaseStat);
			fprintf(fp,"\n");
		}
		if (plist->dwListFlag&0x80000000) {
			if (plist->sModStat.pStats) {
				fprintf(fp,"  ModStat:");dumpStats(fp,&plist->sModStat);
				fprintf(fp,"\n");
			}
			if (plist->sFullStat.pStats) {
				fprintf(fp,"  FullStat:");nsockets=dumpStats(fp,&plist->sFullStat);
				fprintf(fp,"\n");
			}
		}
	} else {
		if (plist->dwListFlag&0x80000000) {
			if (dumpBaseStat) {
				if (plist->sBaseStat.pStats) {
					fprintf(fp,"  BaseStat:");dumpStats(fp,&plist->sBaseStat);
					fprintf(fp,"\n");
				}
				if (plist->sFullStat.pStats) {
					fprintf(fp,"  FullStat:");nsockets=dumpStats(fp,&plist->sFullStat);
					fprintf(fp,"\n");
				}
			} else {
				nsockets=dumpStats(fp,&plist->sFullStat);
			}
		} else {
			nsockets=dumpStats(fp,&plist->sBaseStat);
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
		ItemTxt *pItemTxt = d2common_GetItemTxt( pUnit->dwTxtFileNo );
		DWORD dwQuality = pUnit->pItemData->dwQuality;
		int ethereal=d2common_CheckItemFlag(pUnit, ITEMFLAG_ETHEREAL, 0, "?");
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
			fprintf(fp," id=%d pItem=%x next=%x owner=%d pItemData=%x pPath=%x",
				pUnit->dwUnitId,pUnit,pUnit->pListNext,
				pUnit->pItemData->dwOwnerId,
				pUnit->pItemData,pUnit->pItemPath);
			fprintf(fp," mode=%d",pUnit->dwMode);
			fprintf(fp," nLocation=%d",pUnit->pItemData->nLocation);
			fprintf(fp," nItemLocation=%d",pUnit->pItemData->nItemLocation);
			fprintf(fp," nBodyLocation=%d",pUnit->pItemData->nBodyLocation);
			fprintf(fp," flags1=%x",pUnit->dwFlags1);
			fprintf(fp," flags2=%x",pUnit->dwFlags2);
			fprintf(fp," flags=%x",pUnit->pItemData->dwItemFlags);
		}
		switch (pItemTxt->nType) {
			case 2:type="Shield盾";needSocket=1;break;
			case 3:type="Armor盔甲";needSocket=1;break;
			case 4:type="Gold";break;
			case 5:type="Arrow箭矢";break;
			case 6:type="Bolts十字弓弹";break;
			case 7:type="Ear";break;
			case 10:type="Ring戒指";break;
			case 12:type="Amulet项链";break;
			case 15:type="Boot靴";break;
			case 16:type="Glove手套";break;
			case 18:type="Book";break;
			case 19:type="Belt腰带";break;
			case 22:type="Scroll卷轴";break;
			case 24:type="Sceptre权杖";break;
			case 25:type="Wand法杖";needSocket=1;break;
			case 26:type="Staff棍";needSocket=1;break;
			case 27:type="Bow弓";needSocket=1;break;
			case 28:type="Axe斧";needSocket=1;break;
			case 29:type="Club棒";needSocket=1;break;
			case 30:type="Sword剑";needSocket=1;break;
			case 31:type="Maul/Hammer";needSocket=1;break;
			case 32:type="Dagger";needSocket=1;break;
			case 33:type="Spear";needSocket=1;break;
			case 34:type="Polearm";needSocket=1;break;
			case 35:type="CrossBow十字弓";break;
			case 36:type="Maces钉头锤";break;
			case 37:type="Helm头盔";needSocket=1;break;
			case 39:type="Quest";break;
			case 41:type="Skeleton Key 钥匙";break;
			case 42:type="ThrowingKnife飞刀";break;
			case 43:type="ThrowingAxe飞斧";break;
			case 44:type="Javelins标枪";needSocket=1;break;
			case 58:type="Jewel珠宝";break;
			case 67:type="Claws爪";break;
			case 68:type="Orb水晶";break;
			case 69:type="NecHeads男巫盾牌";needSocket=1;break;
			case 70:type="PalShield游侠盾牌";needSocket=1;break;
			case 71:type="Helm头盔";needSocket=1;break;
			case 72:type="DruHelm";needSocket=1;break;
			case 74:type="Rune符文";break;
			case 75:type="Circlet头环";needSocket=1;break;
			case 76:type="healing potions";break;
			case 77:type="mana potions";break;
			case 78:type="Rejuvenation Potion回复活力药剂";break;
			case 79:type="Stamina Potion体力药剂";break;
			case 80:type="Antidote Potion 解毒剂";break;
			case 81:type="Thawing Potion 解冻剂";break;
			case 82:type="Charm Small小护身符";break;
			case 83:type="Charm Large大型护身符";break;
			case 84:type="Charm Grand超大型护身符";break;
			case 85:type="AmaBow";break;
			case 86:type="AmaSpear";break;
			case 87:type="AmaJavelin";break;
			case 88:type="Claw";break;
			case 96:type="Amethyst紫宝石";break;
			case 97:type="Diamond钻石";break;
			case 98:type="Emerald绿宝石";break;
			case 99:type="Ruby红宝石";break;
			case 100:type="Sapphire蓝宝石";break;
			case 101:type="Topaz黄宝石";break;
			case 102:type="Skull骷髅";break;
			default:type="?";break;
		}
		fprintf(fp," %d,%s(%d,%s)",pItemTxt->nType,type,index,code);
		switch (location) {
			case 255: break;//fprintf(fp,"@body");break;
			case 0: fprintf(fp,"@bag");break;
			case 3: fprintf(fp,"@cube");break;
			case 4: fprintf(fp,"@box");break;
			default: fprintf(fp,"@%d",location);break;
		}
		fprintf(fp," x=%d y=%d",pUnit->pItemPath->unitX,pUnit->pItemPath->unitY);
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
			fprintf(fp,"@(%d,%d)",pUnit->pItemPath->unitX,pUnit->pItemPath->unitY);
		}
		//if (pItemTxt->nSocket) fprintf(fp," 最多%d孔",pItemTxt->nSocket);
		if (pUnit->pItemData->dwFingerPrint) fprintf(fp," finger=%x",pUnit->pItemData->dwFingerPrint);
		int fileindex;
		switch (dwQuality) {
			case ITEM_QUALITY_RARE:fprintf(fp," 亮金");break;
			case ITEM_QUALITY_MAGIC:fprintf(fp," 魔法");break;
			case ITEM_QUALITY_UNIQUE: {
				fileindex = d2common_GetItemFileIndex(pUnit);
				fprintf(fp," 暗金U%03d %s",fileindex,uniqueNames[fileindex]);
				UniqueItemTxt *pUniqueTxt = d2common_GetUniqueItemTxt(fileindex);
				if (pUniqueTxt) {
					if (pUniqueTxt->wReqlvl) fprintf(fp," 需要等级%d",pUniqueTxt->wReqlvl);
					fprintf(fp," %s",pUniqueTxt->szDesc);
				}
				break;
			}
			case ITEM_QUALITY_SET: {
				fileindex = d2common_GetItemFileIndex(pUnit);
				fprintf(fp," 套装SET%03d %s",fileindex,setItemNames[fileindex]);
				SetItemTxt *pSetTxt = d2common_GetSetItemTxt(fileindex);
				if (pSetTxt) {
					if (pSetTxt->wReqlvl) fprintf(fp," 需要等级%d",pSetTxt->wReqlvl);
					fprintf(fp," %s",pSetTxt->szDesc);
				}
				break;
			}
			default:
				if ( d2common_CheckItemFlag(pUnit, ITEMFLAG_RUNEWORD, 0, "?") ) {
					fileindex = d2common_GetRuneWordTxtIndex( pUnit->pItemData->wMagicPrefix[0] );
					fprintf(fp," 神符之语RW%03d %s",fileindex,runeWordNames[fileindex]);
					needSocket=0;
					RuneWordTxt *pRuneWordTxt = d2common_GetRuneWordTxt(fileindex);
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
static int isStartEq(UnitAny *pUnit) {
	if (pUnit->pItemData->dwQuality!=ITEM_QUALITY_NORMAL) return 0;
	int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
	switch (index) {
		case 11:case 26:case 48:case 64:case 1023:
			return 1;
	}
	return 0;
}
static void dumpItems(FILE *fp,UnitAny *pUnit,int level) {
	static struct myItem items[512];
	if (!pUnit) return;
	int n=0;
	nLoc[0]=0;nLoc[1]=0;nLoc[2]=0;nLoc[3]=0;
	while (pUnit) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) goto next;
		int index = GetItemIndex(pUnit->dwTxtFileNo)+1;//all config arrays are based 1
		DWORD dwQuality = pUnit->pItemData->dwQuality;
		if (2103<=index&&index<=2135) { //runes
			hasValuableEquipment++;
		} else if (2050<=index&&index<=2079) { //gems
			hasValuableEquipment++;
		} else if (2090<=index&&index<=2094) { //skull
			hasValuableEquipment++;
		} else if (2006<=index&&index<=2012) { //potions & books
		} else if (2017<=index&&index<=2049) { //arrows & quest
		} else if (2080<=index&&index<=2089) { //potions
		} else {
			BYTE color=GetItemColour(pUnit,0);
			if (color!=(BYTE)-2) {
				if (!isStartEq(pUnit)) hasValuableEquipment++;
			}
		}
		ItemTxt *pItemTxt = d2common_GetItemTxt( pUnit->dwTxtFileNo );
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
					if ( d2common_CheckItemFlag(pUnit, ITEMFLAG_RUNEWORD, 0, "?")) sort=0;
			}
		}
		items[n].sort=(loc<<24)|(sort<<16)|index;
		items[n].location=loc;
		items[n].p=pUnit;
		n++;
		if (n>=512) break;
next:
		pUnit = d2common_GetNextItemInInv(pUnit);
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
		if (!(EXPANSION)&&p->q>=50) continue;
		BOOL f = questData[p->pos]&p->mask;
		if (!f) {has=1;break;}
	}
	if (!has) return;
	fprintf(fp,"%s%d:",name,d);
	for (struct Q *p=qs;p->q;p++) {
		if (!(EXPANSION)&&p->q>=50) continue;
		int qbit=questData[p->pos];
		BOOL f = qbit&p->mask;
		if (!f) fprintf(fp," Q%d%s(%X)",p->q,p->name,qbit);
	}
	fprintf(fp,"\n");
}
static int questIds[5][8]={{1,2,4,5,3,6,0},{9,10,11,12,13,14,0},{20,19,18,17,21,22,0,},{25,27,26,0},{35,36,37,38,39,40,0}};
static int hasD[3]={0};
static u16 qdata[3][64];
static void loadQuests() {
	for (int d=0;d<=2;d++) {
		char ext[16];sprintf(ext,"quest%d.bin",d);hasD[d]=0;
		FILE *fp2=openSnapshotFile("dat",ext,"rb",'_',"quest",0);
		if (fp2) {
			hasD[d]=1;
			int pos=readFully(fp2,(char *)qdata[d],84);
			fclose(fp2);
		}
	}
}
static void dumpQuests(FILE *fp) {
	static struct Q todos[]={{11,1,1,"+1skill"},
	{21,9,0x8001,"+1skill"}, //after done 0x8012, after talk 0x9011
	{31,20,1,"+20HP"},{34,17,1,"+5stat"},{41,25,1,"+2skill"},{53,37,1<<8,"+10resist"},{0,0,0}};
	static struct Q rewards[]={
		{42,27,3,"gem"}, //finish:8002
		{51,35,1,"socket"},//not socket:8022  socketed:9021
		{52,36,1,"rune789"},{0,0,0}};
	loadQuests();
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
		fputc((questData[14]&1)?'X':'-',fp); //done 302D
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
		if (EXPANSION) {
			fputc('|',fp);
			fputc((questData[35]&1)?'X':'-',fp);
			fputc((questData[36]&1)?'X':'-',fp);
			fputc((questData[37]&1)?'X':'-',fp);
			fputc((questData[38]&1)?'X':'-',fp);
			fputc((questData[39]&1)?'X':'-',fp);
			fputc((questData[40]&1)?'X':'-',fp);
			if (!(questData[39]&1)) goto end;
		}
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
			if (!(EXPANSION)&&i==4) break;
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
	if (debug) {
		fprintf(fp,"%x %x(flags=%02x,%02x,%02x,%02x,item=%d)",skill,skill->pSkillInfo,
			(BYTE)skill->pSkillInfo->flags1,(BYTE)skill->pSkillInfo->flags2,
			(BYTE)skill->pSkillInfo->flags3,(BYTE)skill->pSkillInfo->flags4,skill->dwItemId);
	}
	fprintf(fp," %d:%s",id,skillNames[id]);
	fprintf(fp,"(%d",skill->dwSkillLevel);
	if (skill->dwLevelBonus) fprintf(fp,"+%d",skill->dwLevelBonus);
	fputc(')',fp);
	if (skill->dwQuality) fprintf(fp,"(%d)",skill->dwQuality);
	if (debug) fputc('\n',fp);
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
		ObjectTxt *pObjectTxt=pUnit->pObjectData->pObjectTxt;
		char name[128],def='?';BOOL used=FALSE;
		WideCharToMultiByte(CP_ACP,0,pObjectTxt->wszName,-1,name,64,&def,&used);
		fprintf(fp," name=%s %s",pObjectTxt->szName,name);
		fprintf(fp," isdoor=%d",pObjectTxt->nIsDoor);
		fprintf(fp," nSubClass=%d",pObjectTxt->nSubClass);
		fprintf(fp," automap=%d",pObjectTxt->nAutoMap);
	}
	fputc('\n',fp);
	hex(fp,0,pUnit->pObjectData,64);
}
static void dumpAreatile(FILE *fp,UnitAny *pUnit) {
	if (!pUnit) return;
	for(AreaTile *pAreatile = pUnit->pItemPath->pAreaRectData->pAreaRectInfo->pAreaTiles ;pAreatile ; pAreatile = pAreatile->pNext ){
		if (*(pAreatile->pNum)==pUnit->dwTxtFileNo) {
			int lvl=pAreatile->pAreaRectInfo->pDrlgLevel->dwLevelNo;
			fprintf(fp," connect to level=%d\n",lvl);
		}
	}
}
static void dumpUnit(FILE *fp,UnitAny *pUnit) {
	if (!pUnit) return;
	int unitType=pUnit->dwUnitType;
	DWORD dwUnitId = pUnit->dwUnitId; 
	if (pUnit->dwUnitType==UNITNO_PLAYER) {
		fprintf(fp,"Lv %d.%04d %s\n",d2common_GetUnitStat(PLAYER, STAT_LEVEL, 0), getLvPercent(pUnit),
			pUnit->pPlayerData->szName);
		if (dwUnitId == dwPlayerId) {
			if (*d2client_pMercData16 != 0xFFFF)
				fprintf(fp,"Merc is dead: name=%x %x\n",*d2client_pMercData16,*d2client_pMercData32);
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
		pItem = d2common_GetFirstItemInInv(pUnit->pInventory);
		countGemRunes(fp,pItem);
		lastLocation=-1;
		dumpItems(fp,pItem,0);
		if (dwUnitId == dwPlayerId) {
			for (int d=0;d<=2;d++) {
				if (!hasD[d]) continue;
				questData=qdata[d];
				for (int i=0;i<5;i++) {
					if (i==4&&!(EXPANSION)) break;
					fprintf(fp,"QuestCode%d-%d:",d,i+1);
					for (int j=0;questIds[i][j];j++) fprintf(fp,"%X|",questData[questIds[i][j]]);
					fputc('\n',fp);
				}
			}
		}
	} else if ( pUnit->dwUnitType==UNITNO_MONSTER ){
		char name[64],def='?';BOOL used=FALSE;
		MonsterTxt *pMonsterTxt = pUnit->pMonsterData->pMonsterTxt;
		WideCharToMultiByte(CP_ACP,0,pUnit->pMonsterData->wszMonName,-1,name,64,&def,&used);
		fprintf(fp," TXT%d",pUnit->dwTxtFileNo);
		fprintf(fp," flags=%02x,%02x,%02x,%02x",
			pMonsterTxt->nflag1,pMonsterTxt->nflag2,pMonsterTxt->flag3,pMonsterTxt->nflag4);
		if (pMonsterTxt->fNpc) fprintf(fp," NPC");
		fprintf(fp," %s",name);
		if (pUnit->pStatList) {
			StatList *plist=pUnit->pStatList;
			dumpStatList(fp,plist,1);
		}
		fflush(fp);
		if (debug) {
			MonsterData *pMonsterData=pUnit->pMonsterData; 
			if (pMonsterData->fUnique) fprintf(fp," unique");
			if (pMonsterData->fChamp) fprintf(fp," champ");
			if (pMonsterData->fBoss) fprintf(fp," boss");
			fprintf(fp," enchants:{");
			for (int i=0;i<9;i++) {
				if (pMonsterData->anEnchants[i]) fprintf(fp," %d:%d",i,pMonsterData->anEnchants[i]);
			}
			fprintf(fp,"}");
		}
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
		if (pUnit->pInventory) dumpItems(fp,d2common_GetFirstItemInInv(pUnit->pInventory),0);
		if (debug) fprintf(fp,"\n");
		//hex(fp,0,pUnit,0x100);
		//hex(fp,0,pUnit->pMonsterData,0x80);
	} else if ( pUnit->dwUnitType==UNITNO_OBJECT ){
		dumpObject(fp,pUnit);
	} else if ( pUnit->dwUnitType==UNITNO_ITEM ){
		dumpItem(fp,pUnit,0,-1);
	} else if ( pUnit->dwUnitType==UNITNO_AREA_ENTRANCE ){
		dumpAreatile(fp,pUnit);
	}
}
void loadWaypoints(char *packet,int len) {
	if (len<5) return;
	char ext[16];sprintf(ext,"wp%d.bin",DIFFICULTY);
	FILE *fp=openSnapshotFile("dat",ext,"rb",'_',"waypoint",1);
	if (fp) {fseek(fp,7,0);fread(packet,5,1,fp);fclose(fp);}
}
void saveWaypoints(char *packet,int len) {
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
	hasValuableEquipment=0;
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
			fp=openSnapshotFile(NULL,"m.txt","w+",'_',"mercenary",1);
			if (fp) {
				lastLocation=0;
				dumpUnit(fp,m);
				fclose(fp);
			}
		}
	}
	fp=openSnapshotFile("dat","CanDel.txt","w+",'_',"CanDelete",1);
	if (fp) {
		fprintf(fp,"HasValuableEquipment:%d\n",hasValuableEquipment);
		fprintf(fp,"MercDead:%d\n",*d2client_pMercData16!=0xFFFF?1:0);
		fclose(fp);
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

void dumpUnit2(FILE *fp,UnitAny *pUnit) {
	if (!fp) return;
	int unitType=pUnit->dwUnitType;
	DWORD dwUnitId = pUnit->dwUnitId; 
	fprintf(fp,">>> type=%d id=%d pUnit=%x mode=%d txt=%d",
		unitType,dwUnitId,pUnit,pUnit->dwMode,pUnit->dwTxtFileNo);
	fprintf(fp," owner=(%d:%d)", pUnit->dwOwnerType,pUnit->dwOwnerId);
	fprintf(fp," hashnext=%x next=%x", pUnit->pHashNext,pUnit->pListNext);
	if ((unitType==UNITNO_PLAYER||unitType==UNITNO_MONSTER)&&pUnit->pMonPath) {
		fprintf(fp," pos=(%d,%d)",pUnit->pMonPath->wUnitX,pUnit->pMonPath->wUnitY);
	} else if ((unitType==UNITNO_OBJECT||unitType==UNITNO_ITEM||unitType==UNITNO_AREA_ENTRANCE)&&pUnit->pItemPath) {
		fprintf(fp," pos=(%d,%d)",pUnit->pItemPath->unitX,pUnit->pItemPath->unitY);
	}
	//float d=getUnitYardDistance(PLAYER,pUnit);fprintf(fp," dis=%.2f",d);
	if (pUnit->dwUnitType==UNITNO_MONSTER) {
		//fprintf(fp,d2common_IsUnitBlocked(PLAYER,pUnit,2)?" blocked":" visible");
		int owner=d2client_GetMonsterOwner(pUnit->dwUnitId);
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
FILE *openMinimapCellFile(char *name) {
	char path[1024];
	if (!szSnapshotPath[0]) sprintf(path,"%ssnapshot",szPluginPath);
	else if (szSnapshotPath[0]&&szSnapshotPath[1]==':') //absolute path
		sprintf(path,"%s",szSnapshotPath);
	else sprintf(path,"%s%s",szPluginPath,szSnapshotPath);
	CreateDirectoryA(path,NULL);
	strcat(path,"\\");strcat(path,"debug");CreateDirectoryA(path,NULL);
	strcat(path,"\\");strcat(path,"minimap");CreateDirectoryA(path,NULL);
	strcat(path,"\\");strcat(path,name);
	return fopen(path,"wb+");
}
void dc6bmp(FILE *logfp,FILE *fpout,struct GfxCell *pcell) {
	int x0=pcell->dwXoffs,y0=pcell->dwYoffs;
	int w=pcell->dwWidth,h=pcell->dwHeight;
	fprintf(logfp,"(%d,%d) %d*%d %d\n",x0,y0,w,h,pcell->dwLength);
	if (w&3) w=(w&~3)+4;
	BITMAPFILEHEADER bf={0};
	BITMAPINFOHEADER bi={0};
	bf.bfType=0x4d42;
	bf.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+1024;
	bf.bfSize=bf.bfOffBits+w*h;
	bi.biSize=sizeof(BITMAPINFOHEADER);bi.biWidth=w;bi.biHeight=h;bi.biPlanes=1;bi.biBitCount=8;
	bi.biSizeImage=w*h;bi.biXPelsPerMeter=bi.biYPelsPerMeter=0;
	fwrite(&bf,1,sizeof(BITMAPFILEHEADER),fpout);
	fwrite(&bi,1,sizeof(BITMAPINFOHEADER),fpout);
	fwrite(d2gdi_pColorMap,1,1024,fpout);
	char *buf=(char *)malloc(w*8);
	unsigned char *src=pcell->nCols;
	for (int i=0;i<h;i++) {
		memset(buf,0,w);
		char *dst=buf,*dst_end=buf+w;
		while (1) {
			int n=*src++;
			if (n==0x80) break;
			if (dst>=dst_end) {fprintf(logfp,"overflow %d/%d %x\n",dst-buf,w,*src);goto end;}
			if (n&0x80) dst+=n&0x7f;
			else {memcpy(dst,src,n);src+=n;dst+=n;}
		}
		fwrite(buf,1,w,fpout);
	}
end:
	free(buf);
}
static int btreeDepth,btreeNodes;
static void scan_btree(AutomapNode *p,int level) {
	for (;p;p=p->pMore) {
		if (p->pLess) scan_btree(p->pLess,level+1);
		btreeNodes++;if (level>btreeDepth) btreeDepth=level;
		//fprintf(fp,"%d\t(%d,%d)\tw%d\n",p->wCellNo,(short)p->drawXDiv10,(short)p->drawYDiv10,(short)p->wWeight);
		//xstep 16, ystep 4
	}
}
static void dumpCells(FILE *fp,AutomapNode *p) {
	btreeDepth=0;btreeNodes=0;
	scan_btree(p,0);
	fprintf(fp," nodes:%d depth:%d\n",btreeNodes,btreeDepth);
}
void dumpMinimap() {
	FILE *fp=openDbgFile("_minimap.txt");if (!fp) return;
	AreaRectInfo *pAreaRectInfo = PLAYER->pMonPath->pAreaRectData->pAreaRectInfo;
	fprintf(fp,"Tile(%d,%d,%d,%d)\n",pAreaRectInfo->tileX,pAreaRectInfo->tileY,pAreaRectInfo->tileW,pAreaRectInfo->tileH);
	fprintf(fp,"MinimapZoom: %d\n",*d2client_pMinimapZoom);
	fprintf(fp,"ScreenOnMinimap: (%d,%d)\n",*d2client_pScreenMinimapX,*d2client_pScreenMinimapY);
	struct CellFile *pCellFile=*d2client_pMinimapCellFile;
	fprintf(fp,"cell dir %d count %d\n",pCellFile->dwNumdirs,pCellFile->dwNumCells);
	if (0) {
		//all (0,0) 16*32
		for (int i=0;i<(int)pCellFile->dwNumCells;i++) {
			char buf[32];sprintf(buf,"%04d.bmp",i);
			FILE *cfp=openMinimapCellFile(buf);if (!cfp) continue;
			dc6bmp(fp,cfp,pCellFile->pCells[i]);
			fclose(cfp);
		}
	}
	fprintf(fp,"units:\n");
	PresetUnit *pUnit = pAreaRectInfo->pPresetUnits;
	while (pUnit) {
		int unitX=pAreaRectInfo->tileX*5+pUnit->unitDx;
		int unitY=pAreaRectInfo->tileY*5+pUnit->unitDy;
		int drawXDiv10 = ((unitX-unitY)*16)/10 +1;
		int drawYDiv10 = ((unitX+unitY)*8)/10 -3;
		fprintf(fp,"%d:%d (%d,%d) (%d,%d) (%d,%d)\n",pUnit->dwUnitType,pUnit->dwTxtFileNo,
			pUnit->unitDx,pUnit->unitDy,unitX,unitY,drawXDiv10,drawYDiv10);
		pUnit = pUnit->pNext;
	}
	fprintf(fp,"layer: %d\n",LAYER->dwLayerNo);
	fprintf(fp,"floor:\n");dumpCells(fp,LAYER->pFloors);
	fprintf(fp,"wall:\n");dumpCells(fp,LAYER->pWalls);
	fprintf(fp,"object:\n");dumpCells(fp,LAYER->pObjects);
	fprintf(fp,"extra:\n");dumpCells(fp,LAYER->pExtras);
	fclose(fp);
}
void dumpMap() {
	DrlgLevel *pDrlgLevel = PLAYER->pMonPath->pAreaRectData->pAreaRectInfo->pDrlgLevel;
	if (!pDrlgLevel) return;
	FILE *fp=openDbgFile("_map.txt");if (!fp) return;
	int px=PLAYER->pMonPath->wUnitX,py=PLAYER->pMonPath->wUnitY;
	fprintf(fp,"player=(%d,%d) level=%d\n",px,py,pDrlgLevel->dwLevelNo);
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	AreaRectInfo *pInfo = pData->pAreaRectInfo;
	fprintf(fp,"CurrentRect: unit(%d,%d,%d,%d) tile(%d,%d,%d,%d)\n",
		pData->unitX,pData->unitY,pData->unitW,pData->unitH,
		pData->tileX,pData->tileY,pData->tileW,pData->tileH);
	fprintf(fp,"present units:");
	for (PresetUnit *pUnit=pInfo->pPresetUnits;pUnit;pUnit=pUnit->pNext) {
		int map=0;
		if (pUnit->dwUnitType==UNITNO_OBJECT) {
			ObjectTxt *pObj = d2common_GetObjectTxt(pUnit->dwTxtFileNo);
			if (pObj) map=pObj->nAutoMap;
		}
		fprintf(fp,"\t%d:%d(%d,%d) map=%d\n",pUnit->dwUnitType,pUnit->dwTxtFileNo,
			pUnit->unitDx,pUnit->unitDy,map);
	}
	fputc('\n',fp);fflush(fp);
	fprintf(fp," Info(%d,%d,%d,%d)\n",pInfo->tileX,pInfo->tileY,pInfo->tileW,pInfo->tileH);
	AreaRectData *pAreaRectData = PLAYER->pMonPath->pAreaRectData;
	for (int i=0;i<pAreaRectData->nearbyRectCount;i++) {
		AreaRectData *pNData=pAreaRectData->paDataNear[i];
		if (!pNData) continue;
		AreaRectInfo *pNInfo = pNData->pAreaRectInfo;
		int dstLvl=pNInfo->pDrlgLevel->dwLevelNo;
		fprintf(fp," NearInfo(%d,%d,%d,%d) lvl=%d\n",pNData->tileX,pNData->tileY,pNData->tileW,pNData->tileH,dstLvl);
	}
	AreaRectBitmap *gtypes=pData->bitmap;
	if (gtypes) {
		fprintf(fp," (%d,%d,%d,%d) (%d,%d,%d,%d)\n",gtypes->unitX,gtypes->unitY,gtypes->unitW,gtypes->unitH,
			gtypes->tileX,gtypes->tileY,gtypes->tileW,gtypes->tileH);
		for (int bitshift=0;bitshift<16;bitshift+=4) {
			fprintf(fp,"bitmask%d-%d\n",bitshift,bitshift+3);
			unsigned short *ptr=gtypes->bitmap;
			for (int y=0;y<gtypes->unitH;y++) {
				for (int x=0;x<gtypes->unitW;x++) {
					int b=((*ptr++)>>bitshift)&0xF;
					if (gtypes->unitX+x==px&&gtypes->unitY+y==py) fputc('#',fp);
					if (b) fprintf(fp,"%X",b);
					else fputc(' ',fp);
					if ((x%5)==4) fputc('|',fp);
				}
				fputc('\n',fp);
				int n=gtypes->unitW*6/5;
				if ((y%5)==4) {for (int x=0;x<n;x++) fputc('-',fp);fputc('\n',fp);}
			}
		}
	}
		//bit0: path block
		//bit1: vision block
		//bit2: attack block
		//bit4: waypoint
		//bit5: tree
		//bit6: poison?
		//bit7: around player
		//bit8: around monster
		//bit10: object
		//bit11: door?
		//bit12: player or monster
	for (AreaRectInfo *pInfo=pDrlgLevel->pAreaRectInfo;pInfo;pInfo=pInfo->pNext) {
		fprintf(fp," Info(%d,%d,%d,%d)",pInfo->tileX,pInfo->tileY,pInfo->tileW,pInfo->tileH);
		AreaRectData *pData=pInfo->pAreaRectData;
		if (pData) {
			fprintf(fp," Data(%d,%d,%d,%d) Data5(%d,%d,%d,%d)",pData->unitX,pData->unitY,pData->unitW,pData->unitH,
				pData->tileX,pData->tileY,pData->tileW,pData->tileH);
		}
		fputc('\n',fp);
		fprintf(fp,"\tunits:");
		for (PresetUnit *pUnit=pInfo->pPresetUnits;pUnit;pUnit=pUnit->pNext) {
			fprintf(fp,"%d:%d(%d,%d),",pUnit->dwUnitType,pUnit->dwTxtFileNo,pUnit->unitDx,pUnit->unitDy);
		}
		fputc('\n',fp);
	}
	fclose(fp);
}
void dumpTxt() {
	FILE *fp=openDbgFile("_uniqueTxt.txt");if (!fp) return;
	int ptable=*d2common_pDataTables;
	int nUnique=*(int *)(ptable+0xC28);
	UniqueItemTxt *pu=(UniqueItemTxt *)*(int *)(ptable+0xC24);
	for (int i=0;i<nUnique;i++,pu++) {
		char code[5];memcpy(code,pu->szCode,4);code[4]=0;
		fprintf(fp," %d:%s",pu->dwIndex,pu->szDesc);
		if (uniqueNames[pu->dwIndex]) fprintf(fp," %s",uniqueNames[pu->dwIndex]);
		fprintf(fp," %s",code);
		fprintf(fp," qLvl=%d reqLvl=%d",pu->wQlvl,pu->wReqlvl);
		fprintf(fp," cost=(*%d+%d)",pu->dwCostMult,pu->dwCostAdd);
		for (int i=0;i<12;i++) {
			ItemProp *p=&pu->Prop[i];
			if (p->PropId==-1) break;
			fputc(' ',fp);
			switch (p->PropId) {
				case 41:fprintf(fp,"所有抗性+(%d-%d)",p->MinVal,p->MaxVal);break;
				default:
					PropertiesTxt *pPropertiesTxt = d2common_GetPropertiesTxt(p->PropId);
					if ( !pPropertiesTxt ) continue;
					fprintf(fp," P%d",p->PropId);
					short *pStatNo = pPropertiesTxt->wStatNo;
					for (int i=0;i<7;i++) {
						short statId=*pStatNo++;if (statId<0) break;
						fputc(' ',fp);
						dumpStat(fp,statId,p->Param,p->MinVal);
						if (p->MinVal!=p->MaxVal) fprintf(fp,"(%d-%d)",p->MinVal,p->MaxVal);
					}
					break;
			}
		}
		fputc('\n',fp);
		fflush(fp);
	}
	fclose(fp);
}
int DoSnapshot() {
	if (!fInGame) return 0;
	debug=0;
	dumpInventory();
	FILE *fp=openDbgFile("_debug.txt");
	if (fp) {
		fprintf(fp,"DoSnapshot=%x\n",&DoSnapshot);
		debug=1;
		UnitAny *pUnit = d2client_GetSelectedUnit();
		if (pUnit) {
			fprintf(fp,"============ Selected Unit =============\n");
			dumpUnit2(fp,pUnit);
		}
		fclose(fp);
	}
	if (fSinglePlayer) singlePlayerSaveGame("d:\\git\\diablo2\\t\\test.d2s");
	fp=openDbgFile("_quest.txt");
	if (fp) {
		if (GAMEQUESTDATA) {
			u16 *s=*GAMEQUESTDATA;
			fprintf(fp,"Game:\n");
			for (int i=0;i<=40;i++) {
				fprintf(fp,"%d %s: %04x",i,questNames[i],s[i]);
				fputc(i%2==0?'\t':'\n',fp);
			}
			fputc('\n',fp);
		}
		if (QUESTDATA) {
			u16 *s=*QUESTDATA;
			fprintf(fp,"Player:\n");
			for (int i=0;i<=40;i++) {
				fprintf(fp,"%d %s: %04x",i,questNames[i],s[i]);
				fputc(i%2==0?'\t':'\n',fp);
			}
			fputc('\n',fp);
		}
		fclose(fp);
	}
	//dumpMinimap();
	dumpMap();
	dumpTxt();
	fp=openDbgFile("_self.txt");
	if (fp) {
		dumpUnit2(fp,PLAYER);
		fclose(fp);
	}
	fp=openDbgFile("_pet.txt");
	if (fp) {
		for (PetUnit *pPetUnit = *d2client_pPetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) {
			fprintf(fp,"%x txt=%d type=%d",pPetUnit,pPetUnit->dwTxtFileNo,pPetUnit->dwPetType);
			fprintf(fp," owner(type=%d,id=%d)\n",pPetUnit->dwOwerType,pPetUnit->dwOwnerId);
			hex(fp,0,pPetUnit,0x40);
		}
		fclose(fp);
	}
	FILE *playerfp=openDbgFile("_players.txt");
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_PLAYER*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (IsBadReadPtr(pUnit,sizeof(UnitAny))) break;
			if (pUnit->dwUnitType!=UNITNO_PLAYER) break;
			if (pUnit->dwUnitId==dwPlayerId) continue;
			dumpUnit2(playerfp,pUnit);
		}
	}
	fp=openDbgFile("_monsters.txt");
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_MONSTER*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (IsBadReadPtr(pUnit,sizeof(UnitAny))) break;
			if (pUnit->dwUnitType!=UNITNO_MONSTER) break;
			int owner=d2client_GetMonsterOwner(pUnit->dwUnitId);
			if (owner==dwPlayerId) {
				FILE *fp2=openDbgFile("_merc.txt");
				dumpUnit2(fp2,pUnit);
				fclose(fp2);
			} else if (owner!=-1) {
				dumpUnit2(playerfp,pUnit);
			} else {
				dumpUnit2(fp,pUnit);
			}
		}
	}
	fclose(playerfp);
	fclose(fp);
	fp=openDbgFile("_objects.txt");
	if (fp) {
		for (int i=0;i<128;i++) {
			for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_OBJECT*128+i];pUnit;pUnit=pUnit->pHashNext) {
				if (IsBadReadPtr(pUnit,sizeof(UnitAny))) break;
				if (pUnit->dwUnitType!=UNITNO_OBJECT) break;
				dumpUnit2(fp,pUnit);
			}
		}
		fclose(fp);
	}
	fp=openDbgFile("_items.txt");
	if (fp) {
		for (int i=0;i<128;i++) {
			for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_ITEM*128+i];pUnit;pUnit=pUnit->pHashNext) {
				if (IsBadReadPtr(pUnit,sizeof(UnitAny))) break;
				if (pUnit->dwUnitType!=UNITNO_ITEM) break;
				if ((pUnit->dwUnitId&0x7f)!=i) break;
				dumpUnit2(fp,pUnit);
			}
		}
		fclose(fp);
	}
	fp=openDbgFile("_areatile.txt");
	if (fp) {
		for (int i=0;i<128;i++) {
			for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_AREA_ENTRANCE*128+i];pUnit;pUnit=pUnit->pHashNext) {
				if (IsBadReadPtr(pUnit,sizeof(UnitAny))) break;
				if (pUnit->dwUnitType!=UNITNO_AREA_ENTRANCE) break;
				dumpUnit2(fp,pUnit);
			}
		}
		fclose(fp);
	}
	fp=openDbgFile("_itemtxt.txt");
	if (fp) {
		for (int i=0;i<*d2common_pItemTxtCount;i++) {
			int index = GetItemIndex(i)+1;
			ItemTxt *pItemTxt = d2common_GetItemTxt(i);
			if (itemNames[index]) fprintf(fp,"%d %d %s\n",i,pItemTxt->nType,itemNames[index]);
		}
		fclose(fp);
	}
	gameMessage("d2hackmp snapshot %s",szSnapshotPath);
	debug=0;return 1;
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
		call d2common_GetMercInfo
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
			//GetMercInfo(*d2client_pExpansion,PLAYER,dwMercSeed,mercAct-1,*d2client_pDifficulty, buf);
			//fprintf(logfp,"merc type: %x\n",*(int *)buf);
			//hex(logfp,0,buf,64);
			/*for (int i=0;i<3;i++) {
				memset(buf,0,256);
				GetMercInfo(*d2client_pExpansion,PLAYER,dwMercSeed,mercAct-1,i, buf);
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
static void doDeleteSnapshotFile(char *path) {
	LOG("delete %s\n",path);
	if (fileExist(path)) DeleteFileA(path);
}
void removeSnapshot(char *realm,char *account,char *charname) {
	char buf[1024],ext[16];
	getSnapshotPath(buf,realm,account,charname,NULL,"txt",'_');
	doDeleteSnapshotFile(buf);
	getSnapshotPath(buf,realm,account,charname,NULL,"m.txt",'_');
	doDeleteSnapshotFile(buf);
	getSnapshotPath(buf,realm,account,charname,NULL,"d2s",'\\');
	doDeleteSnapshotFile(buf);
	getSnapshotPath(buf,realm,account,charname,"log","log",'_');
	doDeleteSnapshotFile(buf);
	getSnapshotPath(buf,realm,account,charname,"dat","CanDel.txt",'_');
	doDeleteSnapshotFile(buf);
	for (int d=0;d<=2;d++) {
		sprintf(ext,"quest%d.bin",d);
		getSnapshotPath(buf,realm,account,charname,"dat",ext,'_');
		doDeleteSnapshotFile(buf);
		sprintf(ext,"wp%d.bin",d);
		getSnapshotPath(buf,realm,account,charname,"dat",ext,'_');
		doDeleteSnapshotFile(buf);
	}
}
