//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
enum UnitStat {
	STAT_STRENGTH =				0	,		// str
	STAT_ENERGY =				1	,		// energy
	STAT_DEXTERITY =			2	,		// dexterity
	STAT_VITALITY =				3	,		// vitality
	STAT_STATPOINTSLEFT	=		4	,		// stats point left
	STAT_NEWSKILLS =			5	,		// skill point left
	STAT_HP	=					6	,		// life
	STAT_MAXHP =				7	,		// max life
	STAT_MANA =					8	,		// mana
	STAT_MAXMANA =				9	,		// max mana
	STAT_STAMINA =				10  ,		// stamina
	STAT_MAXSTAMINA	=			11	,		// max stamina
	STAT_LEVEL =				12	,		// level
	STAT_EXP =					13	,		// experience
	STAT_GOLD =					14	,		// gold
	STAT_GOLDBANK =				15	,		// stash gold
	STAT_TOBLOCK =				20	,		// to block
	STATS_MANARECOVERY =		26	,		//
	STAT_LASTEXP =				29	,		//
	STAT_NEXTEXP =				30	,		//
	STAT_MAGIC_DAMAGE_REDUCED = 35	,		// damage reduction
	STAT_DAMAGE_REDUCED =		36	,		// magic damage reduction
	STAT_MAGIC_RESIST =			37	,		// magic resist
	STAT_MAX_MAGIC_RESIST =		38	,		// max magic resist
	STAT_FIRE_RESIST =			39	,		// fire resist
	STAT_MAX_FIRE_RESIST =		40	,		// max fire resist
	STAT_LIGHTING_RESIST =		41	,		// lightning resist
	STAT_MAX_LIGHTING_RESIST =	42	,		// max lightning resist
	STAT_COLD_RESIST =			43	,		// cold resist
	STAT_MAX_COLD_RESIST =		44	,		// max cold resist
	STAT_POSION_RESIST =		45	,		// poison resist
	STAT_MAX_POSION_RESIST =	46	,		// max poison resist
	STAT_LIFE_LEECH =			60	,		// Life Leech
	STAT_MANA_LEECH	=			62	,		// Mana Leech
	STAT_VELOCITYPERCENT =		67	,		// effective run/walk
	STAT_AMMOQUANTITY =			70	,		// ammo quantity(arrow/bolt/throwing)
	STAT_DURABILITY	=			72	,		// item durability
	STAT_MAXDURABILITY =		73	,		// max item durability
	STAT_HPREGEN =				74 ,		// hp regen
	STAT_EXTRA_GOLD	=			79	,		// Gold find (GF)
	STAT_MAGIC_FIND =			80	,		// magic find (MF)
	STAT_IAS =					93	,		// ias
	STAT_FRW =					96	,		// faster run/walk
	STAT_FHR =					99	,		// faster hit recovery
	STAT_FBR =					102	,		// faster block rate
	STAT_FCR =					105	,		// faster cast rate
	STAT_PLR =					110	, 
	STAT_DTM =					114	,		// damage taken goes to mana
	STAT_OW =					135	, 
	STAT_CB =					136	,		// crushing blow
	STAT_EK =					138	,		// mana after each kill
	STAT_DS =					141	,		//deadly strike 
	STAT_FIRE_ABSORB =			143	,
	STAT_LIGHTING_ABSORB =		145	,
	STAT_COLD_ABSORB =			148	,
	STAT_NUMSOCKETS	=			194	,
	STATS_ITEM_HP_PERLEVEL =	216	,
	STAT_ITEMDIFFICULTY	=		356,
};
static unsigned char allWaypoints[7]={2,1,0xFF,0xFF,0xFF,0xFF,0x77};
static char *attrNames[16]={"Strength","Energy","Dexterity","Vitality","stat points","skill points","HP","Max HP",
"Mana","Max mana","stamina","Max stamina","Level","Experience","Gold","Stashed",};
static int attrBits[16]={10,10,10,10,10,8,21,21,21,21,21,21,7,32,25,25,};
static int clsSkillId[7]={6,36,66,96,126,221,251,};
#define D2CHARINFO_MAGICWORD		0x12345678
#define D2CHARINFO_VERSION			0x00010000
//#pragma pack(push, pack01, 1)
struct __attribute__((packed)) d2charinfo {
	int		magicword,version,create_time,last_time;
	int		checksum,total_play_time,reserved[6];
	unsigned char	charname[16],account[16],realmname[32];
	short header;	/* 0x84 0x80 */
	char gfx[11],char_class,color[11],level;
	char status,u1[3],ladder;
	/* client only check this bit */
	char u2[2],end,pad[30];
	int	experience,charstatus,charlevel,charclass;
};
//#pragma pack(pop, pack01)
struct __attribute__((packed)) d2s_header {
//#pragma pack(1)
//struct d2s_header {
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
	short mercNameId; //0xB7 value=[00 00]
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
struct bitstream {
	unsigned char *buf,*bufend;
	unsigned int v;
	int n;
};
int rbit(struct bitstream *bs,int n) {
	if (n<=0) return 0;
	if (n>16) {
		int low=rbit(bs,16);
		int high=rbit(bs,n-16);
		return (high<<16)|low;
	}
	while (bs->n < n) {
		if (bs->buf>=bs->bufend) return 0;
		bs->v|=((*bs->buf++)&0xFF)<<bs->n;
		bs->n+=8;
	}
	int r=bs->v & ((1<<n)-1);
	bs->v >>= n;
	bs->n -= n;
	return r;
}
int wbit(struct bitstream *bs,unsigned int value,int n) {
	if (n<=0) return 0;
	if (n>16) {
		wbit(bs,value&0xFFFF,16);
		return wbit(bs,value>>16,n-16);
	}
	while (bs->n>8) {
		if (bs->buf>=bs->bufend) return -1;
		*bs->buf++=bs->v&0xFF;
		bs->v>>=8;bs->n-=8;
	}
	bs->v |= value<<bs->n;
	bs->n+=n;
	return 0;
}
void wbit_done(struct bitstream *bs) {
	if (bs->n<=0) return;
	while (bs->n > 0) {
		if (bs->buf>=bs->bufend) return;
		*bs->buf++=bs->v&0xFF;
		bs->v>>=8;
		if (bs->n>=8) bs->n-=8;else bs->n=0;
	}
}
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
	h->mercType=11;
	h->mercExp=0;
	h->questMagic=0x216f6f57;
	h->questVersion=6;
	h->questSize=298;
	h->waypointMagic=0x5357;
	h->waypointVersion=1;
	h->waypointSize=80;
	for (int i=0;i<3;i++) memcpy(h->waypoints[i],allWaypoints,7);
	h->npcMagic=0x7701;
	h->npcSize=52;
	h->attrMagic=0x6667;
	pfile->attrs[0]=256;
	pfile->attrs[1]=256;
	pfile->attrs[2]=256;
	pfile->attrs[3]=256;
	pfile->attrs[6]=256<<8;
	pfile->attrs[7]=256<<8;
	pfile->attrs[8]=256<<8;
	pfile->attrs[9]=256<<8;
	pfile->attrs[10]=256<<8;
	pfile->attrs[11]=256<<8;
	pfile->attrs[12]=h->charLevel;
	pfile->attrs[13]=1000000000;
	pfile->attrs[14]=10000;
	pfile->attrs[15]=2500000;
	//memset(pfile->skills,20,30);
}
int d2s_write(struct d2s_file *pfile,char *outbuf,int size) {
	struct bitstream bs={0};
	unsigned char *p=(unsigned char *)outbuf;
	memcpy(outbuf,(unsigned char *)&pfile->header,767);p+=767;
	bs.buf=p;bs.bufend=(unsigned char *)outbuf+size;
	for (int i=0;i<16;i++) {
		wbit(&bs,i,9);
		wbit(&bs,pfile->attrs[i],attrBits[i]);
	}
	wbit(&bs,511,9);
	wbit_done(&bs);p=bs.buf;
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
char *load(char *path,int *psize) {
	FILE *fp=fopen(path,"rb");if (!fp) printf("can't open %s\n",path);assert(fp);
	fseek(fp,0,2);int size=ftell(fp);*psize=size;
	char *p=malloc(size+1);p[size]=0;fseek(fp,0,0);
	int n=fread(p,1,size,fp);assert(n==size);fclose(fp);
	return p;
}
void save(char *path,char *buf,int size) {
	FILE *fp=fopen(path,"wb+");assert(fp);
	int n=fwrite(buf,1,size,fp);assert(n==size);fclose(fp);
}
int d2s_read(struct d2s_file *pfile,char *dat,int size) {
	memcpy(pfile,dat,767);
	struct bitstream bs={0};
	bs.buf=dat+767;
	bs.bufend=dat+size;
	int n=0;
	while (1) {
		int id=rbit(&bs,9);if (id==511) break;
		if (n++ > 20) assert(0);
		int value=rbit(&bs,attrBits[id]);
		if (id<0||id>=16) {
			printf("ERROR: id=%d\n",id);
			continue;
		}
		pfile->attrs[id]=value;
	}
	char *p=bs.buf;assert(p[0]=='i'&&p[1]=='f');p+=2;
	memcpy(pfile->skills,p,30);p+=30;
	return p-dat;
}
void d2s_dump(char *dat,int size) {
	struct d2s_file f;
	d2s_read(&f,dat,size);
	for (int i=0;i<16;i++) {
		if (6<=i&&i<12) {
			printf("%s: %d/%d\n",attrNames[i],f.attrs[i]>>8,f.attrs[i+1]>>8);
			i++;
			continue;
		}
		printf("%s: %d\n",attrNames[i],f.attrs[i]);
	}
	printf("skills: ");
	for (int i=0;i<30;i++) printf("%d,",f.skills[i]);
	printf("\n");
	for (int d=0;d<3;d++) {
	//quest37:0x860a->0x8789 use resistance scroll
		printf("quest%d:",d);
		for (int i=0;i<48;i++) printf(" %d:%x",i,f.header.quests[d][i]&0xFFFF);
		printf("\n");
	}
	hex(0,f.header.waypoints,24);
}
void dump(char *path) {
	int len;char *dat=load(path,&len);
	d2s_dump(dat,len);
}
void empty(char *path) {
	char buf[32768];
	struct d2s_file f;
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
}
void mod(char *path) {
	char buf[32768];
	struct d2s_file f2;
	int len;char *dat=load(path,&len);
	struct d2s_file f;
	int size1=d2s_read(&f,dat,len);
	int remain=len-size1;
	f.attrs[STAT_GOLD]=900000;
	f.attrs[STAT_GOLDBANK]=2500000;
	f.header.quests[2][40]=0;
	if (1) {
		static short quest[48]={0x1,
			0x1001,0x1C,0x800A,0x9019,0x14,0x1019,0x1,0x1,
			0x901D,0x1801,0x1005,0x1181,0x1005,0x110D,0x1,0x1,0x1001,0x1001,0x0,0x1001,0x1009,0x1281,0x1,0x1,0x1001,0x1301,0x9001,0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x8022,0x1001,0x9789,0x801E,0x1339,0x175D,0x8002,0x0,0x0,0x0,0x0,0x0,0x0,};
		f.header.charProgression=0xF;
		for (int i=0;i<3;i++) {
			f.header.difficulty[i]=0x80;
			memcpy(f.header.quests[i],quest,96);
		}
		for (int i=0;i<3;i++) {
			f.header.quests[i][36]=0; //rescue
		}
	}
	f.header.charLevel=f.attrs[STAT_LEVEL]=90;//1764543065;
	f.attrs[STAT_EXP]=1764543065;
	f.attrs[STAT_STATPOINTSLEFT]=500;
	f.attrs[STAT_NEWSKILLS]=100;
	//f.attrs[STAT_HP]=f.attrs[STAT_MAXHP]=4096<<8;
	//f.attrs[STAT_MANA]=f.attrs[STAT_MAXMANA]=4096<<8;
	memset(f.skills,99,30);
	for (int i=0;i<3;i++) memcpy(f.header.waypoints[i],allWaypoints,7);
	//f.header.mercDead=1;
	//f.header.mercNameId=0x55;
	//f.header.mercType=0xa;
	//f.header.mercExp=0xa;
	//printf("name=%x\n",f.header.mercNameId);printf("type=%x\n",f.header.mercType);printf("seed=%x\n",f.header.mercSeed);
	int size=d2s_write(&f,buf,32768);
	memcpy(buf+size,dat+size1,remain);
	size+=remain;
	d2s_checksum(buf,size);
	printf("write %s\n",path);
	FILE *fp=fopen(path,"wb+");if (!fp) return;
	fwrite(buf,size,1,fp);
	fclose(fp);
}
void updateName(char *pvpgn,char *acc,char *name) {
	char path[256];
	char buf[32768];
	if (1) {
		_snprintf(path,256,"%s/charinfo/%s/%s",pvpgn,acc,name);
		int len;char *dat=load(path,&len);
		strcpy(dat+0x30,name);
		strcpy(dat+0x40,acc);
		save(path,dat,len);
	}
	_snprintf(path,256,"%s/charsave/%s",pvpgn,name);
	struct d2s_file f2;
	int len;char *dat=load(path,&len);
	struct d2s_file f;
	int size1=d2s_read(&f,dat,len);
	strcpy(f.header.charName,name);
	int remain=len-size1;
	int size=d2s_write(&f,buf,32768);
	memcpy(buf+size,dat+size1,remain);
	size+=remain;
	d2s_checksum(buf,size);
	printf("write %s\n",path);
	save(path,buf,size);
}
int main() {
	//updateName("D:/d2hackmap/d2server113c/pvpgn-1.99.r578/var","sor","csor");return 0;
	assert(sizeof(struct d2s_header)==767);
	char *path="D:\\game\\diablo2\\save\\nec.d2s";
	path="D:\\game\\diablo2\\save\\ttt.d2s";mod(path);return 0;
	path="D:\\game\\diablo2\\save\\csor.d2s";mod(path);
	path="D:\\game\\diablo2\\save\\sor.d2s";mod(path);
	path="D:\\game\\diablo2\\save\\nec.d2s";mod(path);
	return 0;
	path="D:\\game\\diablo2\\save\\bar.d2s";mod(path);
	path="D:\\game\\diablo2\\save\\pal.d2s";mod(path);
	path="D:\\game\\diablo2\\save\\ama.d2s";mod(path);
	//path="t/test.d2s";
	//dump(path);return 0;
	return 0;
}
