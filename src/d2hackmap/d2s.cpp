#include "stdafx.h"
#include "header.h"
#include <string.h>
#include <time.h>
#include "bitstream.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
static char *attrNames[16]={"Strength","Energy","Dexterity","Vitality","stat points","skill points","HP","Max HP",
"Mana","Max mana","stamina","Max stamina","Level","Experience","Gold","Stashed",};
static int attrBits[16]={10,10,10,10,10,8,21,21,21,21,21,21,7,32,25,25,};
static int clsSkillId[7]={6,36,66,96,126,221,251,};
#pragma pack(1)
//struct __attribute__((packed)) d2s_header {
struct d2s_header {
	int magic; //0xaa55aa55
	int version; //0x60
	int filesize;
	int checksum;
	int active_weapon; //0x10 value=0 or 1
	char charName[16]; //0x14
	char charStatus; //0x24 value=0x20 Expansion
	char charProgression; //0x25 06 nightmare act2, 7 n3, 8 n4/n5, a h1, b h2, c h3, d h4/h5, f passed
	char off26[2];//zero
	char charClass;//0x28
	char off29[2];//value=[0x10 0x1e]
	char charLevel;//0x2B
	int off2C; //value=0
	int time; //0x30
	int off34; //value=-1
	int hotKeys[16]; //0x38 skillId or 0xFFFF
	int leftSkill; //0x78
	int rightSkill; //0x7C
	int leftSkill2; //0x80
	int rightSkill2; //0x84
	char menuApperance[32]; //0x88 
	char difficulty[3]; //0xa8 active:0x80 bit0-2: actNo
	int mapSeed; //0xab [1c 99 8f 5f]
	char offAF; //value=0
	char offB0; //value=0
	short mercDead; //0xB1 value=0
	int mercSeed; //0xB3 value=[5d 6d 48 fa] [b6 58 b9 eb]
	short mercNameId; //0xB7 value=[00 00] unknown value
/*
nightmare 
act2 09:combat 0a:defensive, 0b:offensive
mercType:
0 normal fire,1	normal ice,2 nightmare fire,3 nightmare ice,4 hell fire,5 hell ice
6 normal combat,7 normal defense,8 normal offense
9 nightmare combat,a nightmare defense,b nightmare offense
c hell combat,d hell defense,e hell offense
f normal A3 fire,10	normal A3 ice,11	normal A3 light
12 	nightmare A3 fire,13	nightmare A3 ice,14	nightmare A3 light
15 	hell A3 fire,16	hell A3 ice,17	hell A3 light
18 	normal bar,19 	normal bar
1a 	nightmare bar,1b 	nightmare bar
1c 	hell bar,1d 	hell bar
*/
	short mercType; //0xB9 value=[0b 00] 
	int mercExp; //0xbb 
	char offBF[144]; //value=0
	int questMagic; //0x14F value=0x216f6f57 Woo!
	int questVersion; //0x153 value=6
	short questSize; //0x157 value=298
	short quests[3][48]; //0x159
	short waypointMagic; //0x279 value=0x5357 WS
	int waypointVersion; //0x27b value=1
	short waypointSize; //0x27f value=80
	char waypoints[3][24]; //0x281
	short npcMagic; //0x2c9 value=0x7701 
	short npcSize; //0x2cb value=52
	char npcIntro[3][8]; //0x2cd
	char npcCongratulations[3][8]; //0x2e5
	//size=0x2fd 765
	short attrMagic; //0x6667 gf
};
struct d2s_file {
	struct d2s_header header;
	int attrs[16];
	char skills[30];//preceed by "if"
};
void hex(int addr,u8 *buf,int n) {
	for (int i=0;i<n;i+=16) {
		printf("%06x: ",addr+i);
		for (int j=0;j<16;j++) {
			if (i+j>=n) printf("   ");
			else printf("%02x ",buf[i+j]);
			if (j==3||j==7||j==11) printf("- ");
		}
		printf("|");
		for (int j=0;j<16;j++) {
			if (i+j>=n) {printf(" ");continue;}
			u8 c=buf[i+j];
			if (c<' '||c>127) printf(" ");
			else printf("%c",c);
		}
		printf("\n");
	}
	printf("\n");
}
void d2s_init(struct d2s_file *pfile) {
	static unsigned char wp[7]={2,1,0xFF,0xFF,0xFF,0xFF,0x77};
	memset(pfile,0,sizeof(struct d2s_file));
	struct d2s_header *h=&pfile->header;
	h->magic=0xaa55aa55;
	h->version=0x60;
	strcpy(h->charName,"test");
	h->charStatus=0x20;
	h->charProgression=0;
	h->charClass=2;
	h->off29[0]=0x10;h->off29[1]=0x1e;
	h->charLevel=1;
	h->time=(int)time(0);
	h->off34=-1;
	for (int i=0;i<16;i++) h->hotKeys[i]=0xFFFF;
	memset(h->menuApperance,0xFF,32);
	memset(h->difficulty,0x80,3);
	h->mercSeed=0xebb958b6;
	h->mercNameId=6;
	h->mercType=0xb;
	h->mercExp=0;
	h->questMagic=0x216f6f57;
	h->questVersion=6;
	h->questSize=298;
	h->waypointMagic=0x5357;
	h->waypointVersion=1;
	h->waypointSize=80;
	for (int i=0;i<3;i++) memcpy(h->waypoints[i],wp,7);
	h->npcMagic=0x7701;
	h->npcSize=52;
	h->attrMagic=0x6667;
	for (int i=0;i<16;i++) pfile->attrs[i]=0;
	memset(pfile->skills,0,30);
}
int d2s_write(struct d2s_file *pfile,char *outbuf,int size) {
	struct bitstream bs={0};
	unsigned char *p=(unsigned char *)outbuf;
	memcpy(outbuf,(unsigned char *)&pfile->header,767);p+=767;
	bitstream_init(&bs,p,outbuf+size);
	for (int i=0;i<16;i++) {
		bitstream_wbit(&bs,i,9);
		bitstream_wbit(&bs,pfile->attrs[i],attrBits[i]);
	}
	bitstream_wbit(&bs,511,9);
	bitstream_wbit_done(&bs);p=bs.buf;
	*p++='i';*p++='f';memcpy(p,pfile->skills,30);p+=30;
	return p-(unsigned char *)outbuf;
}
void d2s_checksum(char *buf,int size) {
	struct d2s_header *p=(struct d2s_header *)buf;
	p->checksum=0;
	p->filesize=size;
	int sum=0;
	for (int i=0;i<size;i++) {unsigned int c=buf[i]&0xFF;if (sum<0) c++;sum=(sum<<1)+c;}
	p->checksum=sum;
}
extern int dwMercSeed,dwMercNameId;
int d2s_generate(FILE *fp, UnitAny *pUnit,UnitAny *m,u16 *quest,char *wp,UnitAny *ironGolem) {
	if (sizeof(struct d2s_header)!=767) {
		LOG("ERROR d2s not packed\n");
		return -1;
	}
	int bufsize=32768,size=0;
	char *buf=(char *)malloc(bufsize);if (!buf) return -1;
	struct d2s_file f;
	d2s_init(&f);
	if (pUnit&&pUnit->dwUnitType==0) {
		int dwPlayerId = pUnit->dwUnitId;
		int dwPlayerClass = getPlayerClass(dwPlayerId);
		f.header.charClass=dwPlayerClass;
		f.header.charLevel=d2common_GetUnitStat(pUnit, STAT_LEVEL, 0);
		f.header.charStatus=EXPANSION?0x20:0;
		for (int i=0;i<16;i++) {
			int hotkey=d2client_pSkillHotKeys[i];
			int left=d2client_pSkillHotKeyLeft[i];
			if (hotkey==-1) hotkey=0xFFFF;
			else hotkey=(left?0x8000:0)|(hotkey&0x7fff);
			f.header.hotKeys[i]=hotkey;
		}
		f.header.leftSkill=f.header.leftSkill2=pUnit->pSkill->pLeftSkill->pSkillInfo->wSkillId;
		f.header.rightSkill=f.header.rightSkill2=pUnit->pSkill->pRightSkill->pSkillInfo->wSkillId;
		if (dwMercSeed) {
			f.header.mercSeed=dwMercSeed;
			int typeN=0,typeAdd=0;
			if (0xd53<=dwMercNameId&&dwMercNameId<=0xd7b) {
				f.header.mercNameId=dwMercNameId-0xd53;
				typeN=2;typeAdd=0; //unknown difficulty, assume normal
			} else if (0x3fb<=dwMercNameId&&dwMercNameId<=0x40f) {
				f.header.mercNameId=dwMercNameId-0x3fb;
				typeN=3;typeAdd=9; //unknown difficulty, assume nightmare
			} else if (0x410<=dwMercNameId&&dwMercNameId<=0x423) {
				f.header.mercNameId=dwMercNameId-0x410;
				typeN=3;typeAdd=0xF; //unknown difficulty, assume normal
			} else if (0x2a53<=dwMercNameId&&dwMercNameId<=0x2a95) {
				f.header.mercNameId=dwMercNameId-0x2a53;
				typeN=2;typeAdd=0x18; //unknown difficulty, assume normal
			} else
				f.header.mercNameId=0;
			if (typeN) {
				unsigned int seed=(unsigned int)dwMercSeed;
				seed=seed*0x6AC690C5+666;
				int typeIndex=seed%typeN;
				f.header.mercType=typeAdd+typeIndex;
			}
		}
		if (m) {
			if (*d2client_pMercData16 != 0xFFFF) f.header.mercDead=1;
			/*switch (m->dwTxtFileNo) {
				case 271:f.header.mercType=1;break; //10f A1 unknown merc, set to normal ice
				case 338:f.header.mercType=0xa;break; //152 A2 unknown merc, set to nightmare defense
				case 359:f.header.mercType=0x11;break; //167 A3 unknown merc, set to normal lightning
				case 560:f.header.mercType=0x18;break; //230 A5 unknown merc, set to normal
				case 561:f.header.mercType=0x19;break; //231 A5 unknown merc, set to normal
			}*/
			if (m->pSkill) {
				for (struct Skill *p=m->pSkill->pFirstSkill;p;p=p->pNextSkill) {
					if (!p->pSkillInfo) break;
					int id=p->pSkillInfo->wSkillId;
					if (id==104) f.header.mercType=0xa;
				}
			}
		}
		if (quest[96*2+40]&1) f.header.charProgression=0xf; //passed
		else if (quest[96*2+23]&1) f.header.charProgression=0xd; //hell act4/5
		else if (quest[96*2+15]&1) f.header.charProgression=0xc; //hell act3
		else if (quest[96*2+7]&1) f.header.charProgression=0xb; //hell act2
		else if (quest[96+40]&1) f.header.charProgression=0xa; //hell act1
		else {
			f.header.difficulty[2]=0;
			if (quest[96+23]&1) f.header.charProgression=8; //nightmare act4/5
			else if (quest[96+15]&1) f.header.charProgression=7; //nightmare act3
			else if (quest[96+7]&1) f.header.charProgression=6; //nightmare act2
			else if (quest[40]&1) f.header.charProgression=5; //nightmare act1
			else {
				f.header.difficulty[1]=0;
				if (quest[23]&1) f.header.charProgression=3; //act4/5
				else if (quest[15]&1) f.header.charProgression=2; //act3
				else if (quest[7]&1) f.header.charProgression=1; //act2
				else f.header.charProgression=0; //act1
			}
		}
		memcpy(f.header.charName,pUnit->pPlayerData->szName,16);
		int skillId0=clsSkillId[dwPlayerClass];
		StatList *plist=pUnit->pStatList;
		Stat *stat=&plist->sBaseStat;
		for (int i=0;i<stat->wStats;i++) {
			StatEx *se=&stat->pStats[i];
			if (se->wStatId<0) continue;
			if (se->wStatId>15) break;
			f.attrs[se->wStatId]=se->dwStatValue;
		}
		if (m) f.header.mercExp=d2common_GetUnitStat(m, STAT_EXP, 0);
		for (struct Skill *p=pUnit->pSkill->pFirstSkill;p;p=p->pNextSkill) {
			int id=p->pSkillInfo->wSkillId;
			if (skillId0<=id&&id<skillId0+30) f.skills[id-skillId0]=(char)p->dwSkillLevel;
		}
		if (quest) memcpy(f.header.quests,quest,3*96);
		if (wp) memcpy(f.header.waypoints,wp,3*24);
		size=d2s_write(&f,buf,bufsize);
		buf[size++]='J';buf[size++]='M';
		short *pItemCount=(short *)&buf[size];*pItemCount=0;size+=2;
		for (UnitAny *pItem = d2common_GetFirstItemInInv(pUnit->pInventory);pItem;pItem = d2common_GetNextItemInInv(pItem)) {
			if (size>bufsize-8192) {bufsize*=2;buf=(char *)realloc(buf,bufsize);if (!buf) return -1;}
			int written=encodeItem(pItem,buf+size,bufsize-size);
			size+=written;
			pItemCount[0]++;
		}
	} else {
		size=d2s_write(&f,buf,bufsize);
		buf[size++]='J';buf[size++]='M';buf[size++]=0;buf[size++]=0;
	}
	buf[size++]='J';buf[size++]='M';buf[size++]=0;buf[size++]=0;
	buf[size++]='j';buf[size++]='f';buf[size++]='J';buf[size++]='M';
	short *pItemCount=(short *)&buf[size];*pItemCount=0;size+=2;
	if (m) {
		for (UnitAny *pItem = d2common_GetFirstItemInInv(m->pInventory);pItem;pItem = d2common_GetNextItemInInv(pItem)) {
			if (size>bufsize-8192) {bufsize*=2;buf=(char *)realloc(buf,bufsize);if (!buf) return -1;}
			int written=encodeItem(pItem,buf+size,bufsize-size);
			size+=written;
			pItemCount[0]++;
		}
	}
	buf[size++]='k';buf[size++]='f';
	UnitAny *ironGolemItem = ironGolem?d2common_GetFirstItemInInv(ironGolem->pInventory):NULL;
	if (ironGolemItem) {
		buf[size++]=1;
		int written=encodeItem(ironGolemItem,buf+size,bufsize-size);
		size+=written;
	} else {
		buf[size++]=0;
	}
	d2s_checksum(buf,size);
	fwrite(buf,size,1,fp);
	free(buf);
	return 0;
}
/*
char *load(char *path,int *psize) {
	FILE *fp=fopen(path,"rb");assert(fp);
	fseek(fp,0,2);int size=ftell(fp);*psize=size;
	char *p=malloc(size+1);p[size]=0;fseek(fp,0,0);
	int n=fread(p,1,size,fp);assert(n==size);fclose(fp);
	return p;
}
void save(char *path,char *buf,int size) {
	FILE *fp=fopen(path,"wb+");assert(fp);
	int n=fwrite(buf,1,size,fp);assert(n==size);fclose(fp);
}
void d2s_read(struct d2s_file *pfile,char *dat,int size) {
	memcpy(pfile,dat,767);
	struct bitstream bs={0};
	bitstream_init(&bs,dat+767,dat+size);
	int n=0;
	while (1) {
		int id=bitstream_rbit(&bs,9);if (id==511) break;
		if (n++ > 20) assert(0);
		int value=bitstream_rbit(&bs,attrBits[id]);
		if (id<0||id>=16) continue;
		pfile->attrs[id]=value;
	}
	char *p=bs.buf;assert(p[0]=='i'&&p[1]=='f');
	p+=2;
	memcpy(pfile->skills,p,30);
}
void d2s_dump(char *dat,int size) {
	struct d2s_file d2sfile;
	d2s_read(&d2sfile,dat,size);
	for (int i=0;i<16;i++) {
		if (6<=i&&i<12) {
			printf("%s: %d/%d\n",attrNames[i],d2sfile.attrs[i]>>8,d2sfile.attrs[i+1]>>8);
			i++;
			continue;
		}
		printf("%s: %d\n",attrNames[i],d2sfile.attrs[i]);
	}
	printf("skills: ");
	for (int i=0;i<30;i++) printf("%d,",d2sfile.skills[i]);
	printf("\n");
	hex(0,d2sfile.header.waypoints,24);
}
int main() {
	assert(sizeof(struct d2s_header)==767);
	char buf[32768];
	struct d2s_file f;
	//int len;char *dat=load("t/nec.d2s",&len);d2s_dump(dat,len);return 0;
	d2s_init(&f);
	int size=d2s_write(&f,buf,32768);
	buf[size++]='J';buf[size++]='M';buf[size++]=0;buf[size++]=0;
	buf[size++]='J';buf[size++]='M';buf[size++]=0;buf[size++]=0;
	buf[size++]='j';buf[size++]='f';buf[size++]='J';buf[size++]='M';buf[size++]=0;buf[size++]=0;
	buf[size++]='k';buf[size++]='f';buf[size++]=0;
	//d2s_dump(buf,size);
	d2s_checksum(buf,size);
	//save("t/test.d2s",buf,size);
	save("D:\\game\\diablo2\\save\\test.d2s",buf,size);
	return 0;
}
*/
