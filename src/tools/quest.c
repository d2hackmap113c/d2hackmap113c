#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
char *snapshotPath="d:\\snapshot";
struct __attribute__((packed)) d2s_header {
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
char *load(char *path,int *psize) {
	FILE *fp=fopen(path,"rb");if (!fp) return NULL;
	fseek(fp,0,2);int size=ftell(fp);if (psize) *psize=size;
	char *p=malloc(size+1);p[size]=0;fseek(fp,0,0);
	int n=fread(p,1,size,fp);assert(n==size);fclose(fp);
	return p;
}
void save(char *path,char *buf,int size) {
	FILE *fp=fopen(path,"wb+");assert(fp);
	int n=fwrite(buf,1,size,fp);assert(n==size);fclose(fp);
}
struct bitstream {unsigned char *buf,*bufend;unsigned int v;int n;};
int rbit(struct bitstream *bs,int n) {
	if (n<=0) return 0;
	if (n>16) {int low=rbit(bs,16);int high=rbit(bs,n-16);return (high<<16)|low;}
	while (bs->n < n) {
		if (bs->buf>=bs->bufend) return 0;
		bs->v|=((*bs->buf++)&0xFF)<<bs->n;
		bs->n+=8;
	}
	int r=bs->v & ((1<<n)-1);bs->v >>= n;bs->n -= n;return r;
}
struct CharTag {char *name,*tag;};
static struct CharTag *charTags=NULL;
static struct CharTag curCharTags[8];
static int nCharTag=0,capCharTag=0;
int nCurCharTags=0;
static int compareCharTag(const void *a,const void *b) {return strcmp(((struct CharTag *)a)->name,((struct CharTag *)b)->name);}
void clearCharacterTag() {
	charTags=NULL;nCharTag=0;capCharTag=0;
}
void addCharacterTag(char *name,char *tag) {
	if (!charTags) {nCharTag=0;capCharTag=32;charTags=(struct CharTag *)malloc(sizeof(struct CharTag)*capCharTag);}
	for (int i=0;i<nCharTag;i++) {
		struct CharTag *p=&charTags[i];
		if (strcmp(p->name,name)==0) {p->tag=tag;return;}
	}
	if (nCharTag>=capCharTag) {
		capCharTag*=2;charTags=(struct CharTag *)realloc(charTags,sizeof(struct CharTag)*capCharTag);
	}
	struct CharTag *p=&charTags[nCharTag++];
	p->name=name;p->tag=tag;
}
void doneCharacterTag() {qsort(charTags,nCharTag,sizeof(struct CharTag),compareCharTag);}
char isquote(char c) {return ((c) == '"' || (c) == '`') ? c : 0;}
void filterSpace(char *s) {
	char *src=s,*dst=s;int quote = 0;
	while(1){
		char c=*src++;if (!c) break;
		if((c==' '||c=='\t')&&!quote) continue;
		if(c=='/'&&*src=='/') break;
		if(isquote(c)) quote = !quote;
		*dst++=c;
	}
	*dst=0;
}
void splitLine(char *line,char **pleft,char **pright){
	filterSpace(line);
	char *e = strchr(line,':');
	if (e) {*e=0;*pleft=line;*pright=e+1;return;}
	if (line[0]) {
		e = strrchr(line,']');
		if (e) {*e=0;*pleft=line;*pright=e+1;return;}
	}
	*pleft=NULL;*pright=NULL;
}
static void loadTagFile(char *path) {
	char *charname,*tag;
	int size=0;char *p=load(path,&size);if (!p) return;
	char *end=p+size;
	int n=0;
	while (p<end) {
		char *e=strchr(p,'\n');if (!e) e=end;
		char *line=p;p=e+1;
		*e=0;if (e[-1]=='\r') {e--;*e=0;}
		int len=e-line;
		if (line[0]=='/'&&line[1]=='/') continue;
		if (line[0]=='#') continue;
		splitLine(line,&charname,&tag);
		if (charname&&charname[0]) {addCharacterTag(charname,tag);n++;}
	}
	doneCharacterTag();
}
static int compareNames(const void *a,const void *b) {return strcmp(*(char **)a,*(char **)b);}
typedef struct {
	int accountId;
	int lv;char *acc,*name,*line;
} Char;
Char chars[128];int nchar=0;
int total_soj=0;
void check(int accountId,char *realm,char *account,char *name) {
	static int attrBits[16]={10,10,10,10,10,8,21,21,21,21,21,21,7,32,25,25,};
	static char *diffName[3]={"pt","em","dy"};
	struct d2s_header header;
	char path[512];
	short quest[48];
	char buf[1024],buf2[32];int pos=0;
	sprintf(path,"%s\\%s\\%s\\%s.d2s",snapshotPath,realm,account,name);
	int size;char *dat=load(path,&size);
#define PRT(...) pos+=_snprintf(buf+pos,1024-pos,__VA_ARGS__)
	if (!dat) {PRT("can't find %s\n",path);return;}
	memcpy(&header,dat,sizeof(struct d2s_header));
	struct bitstream bs={0};
	bs.buf=dat+767;
	bs.bufend=dat+size;
	int n=0,gold=0,goldstash=0;
	while (1) {
		int id=rbit(&bs,9);if (id==511) break;
		if (n++ > 20) assert(0);
		int value=rbit(&bs,attrBits[id]);
		if (id<0||id>=16) {
			printf("ERROR: id=%d\n",id);
			continue;
		}
		switch (id) {
			case 14: gold=value;break;
			case 15: goldstash=value;break;
		}
	}
	free(dat);
	int dL=0,act=0;
	int exp=header.charStatus&0x20;
	PRT("F%d ",accountId);
	sprintf(path,"%s\\%s\\dat\\%s_%s.soj.txt",snapshotPath,realm,account,name);
	FILE *fp2=fopen(path,"rb");
	if (fp2) {
		int n=fread(buf2,32,1,fp2);fclose(fp2);
		int soj=strtol(buf2,0,10);
		PRT(" %4dsoj",soj);
		total_soj+=soj;
	}
	for (int d=0;d<=2;d++) {
		sprintf(path,"%s\\%s\\dat\\%s_%s.quest%d.bin",snapshotPath,realm,account,name,d);
		FILE *fp2=fopen(path,"rb");
		if (fp2) {fread((char *)quest,96,1,fp2);fclose(fp2);}
		else {
			PRT(d<2?"  --------":"  -----");
			continue;
		}
		PRT("  ");
		if (quest[27]&3) PRT("  ");
		else PRT("g%d",d);
		if (d<2) {
			PRT(" ");
			if (quest[35]&1) PRT("  ");
			else if (quest[35]&2) PRT("S%d",d);
			else PRT("s%d",d);
			PRT(" ");
			if (quest[36]&1) PRT("  ");
			else PRT("r%d",d);
		}
		if (header.charLevel>=40) {
			PRT(" ");if (!exp||quest[1]&1) PRT("  ");else PRT("A%d",d,quest[1]);//Den of Evil
			PRT(" ");if (!exp||quest[9]&0x8001) PRT("  ");else PRT("B%d",d,quest[9]);//Radament's Lair
			PRT(" ");if (quest[20]&1) PRT("  ");else PRT("G%d",d);//The Golden Bird
			PRT(" ");if (quest[17]&1) PRT("  ");else PRT("a%d",d);//+5 stat
			PRT(" ");if (!exp||quest[25]&1) PRT("  ");else PRT("C%d",d,quest[25]);//The Fallen Angel
			PRT(" ");if (!exp||quest[37]&0x100) PRT("  ");else PRT("R%d",d,quest[37]);//Prison of Ice
		}
		dL=d;act=1;
		if (quest[7]&1) act=2;
		if (quest[15]&1) act=3;
		if (quest[23]&1) act=4;
		if (quest[28]&1) act=5;
		if (quest[40]&1) act=6;
	}
	PRT(" %cL%d",header.charStatus&0x20?' ':'c',header.charLevel);
	if (header.charLevel<10) PRT(" ");
	
	PRT(" %s%d",diffName[dL],act);
	PRT(" %s/%s",account,name);
	struct CharTag ct;ct.name=name;
	struct CharTag *pct=(struct CharTag *)bsearch(&ct,charTags,nCharTag,sizeof(struct CharTag),compareCharTag);
	PRT(" %dW",(gold+goldstash)/10000);
	if (pct) PRT("  %s",pct->tag);
	Char *p=&chars[nchar++];
	p->accountId=accountId;
	p->lv=header.charLevel;
	p->acc=account;
	p->name=name;
	p->line=strdup(buf);
}
int compareChar(const void *a,const void *b) {
	Char *c1=(Char *)a;
	Char *c2=(Char *)b;
	if (c1->lv<5&&c2->lv<5) {
		int t=c1->accountId-c2->accountId;if (t) return t;
		return strcmp(c1->name,c2->name);
	} else if (c1->lv>5&&c2->lv>5) {
		int t=c1->accountId-c2->accountId;if (t) return t;
	}
	int t=c1->lv-c2->lv;if (t) return -t;
	//t=strcmp(c1->acc,c2->acc);if (t) return t;
	return strcmp(c1->name,c2->name);
}
void scan_realm(char *realm) {
	WIN32_FIND_DATAA fd;
	char path[256],buf[256];
	char *accounts[32],*names[32];
	int accountN=0,nameN=0;
	sprintf(path,"%s\\%s\\account.txt",snapshotPath,realm);
	FILE *fp=fopen(path,"r");
	if (fp) {
		while (1) {
			char *line=fgets(buf,256,fp);if (!line) break;
			int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
			if (accountN<32) accounts[accountN++]=strdup(line);
		}
		fclose(fp);
	} else {
		sprintf(path,"%s\\%s\\account\\*.txt",snapshotPath,realm);
		printf("%s\n",path);
		int hasMore=1;
		for (HANDLE h=FindFirstFile(path,&fd);h!=INVALID_HANDLE_VALUE&&hasMore;hasMore=FindNextFile(h,&fd)) {
			if (fd.nFileSizeLow==0) continue;
			char *e=strrchr(fd.cFileName,'.');if (e) *e=0;
			if (accountN<32) accounts[accountN++]=strdup(fd.cFileName);
		}
		qsort(accounts,accountN,sizeof(char *),compareNames);
	}
	if (accountN==0) return;
	for (int i=0;i<accountN;i++) {
		sprintf(path,"%s\\%s\\account\\%s.txt",snapshotPath,realm,accounts[i]);
		FILE *fp=fopen(path,"r");assert(fp);
		nameN=0;
		while (1) {
			char *line=fgets(buf,256,fp);if (!line) break;
			int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
			names[nameN++]=strdup(line);
		}
		qsort(names,nameN,sizeof(char *),compareNames);
		for (int j=0;j<nameN;j++) {
			check(i+1,realm,accounts[i],names[j]);
		}
	}
	sprintf(path,"%s\\%s\\z_report.txt",snapshotPath,realm);
	FILE *reportFp=fopen(path,"w+");assert(reportFp);
	printf("output %s\n",path);
	qsort(chars,nchar,sizeof(Char),compareChar);
	for (int i=0;i<nchar;i++) {
		fputs(chars[i].line,reportFp);fputc('\n',reportFp);
	}
	fprintf(reportFp,"total soj: %d\n",total_soj);
	fclose(reportFp);
}
int main(int argc, char* argv[]) {
	char path[256];
	sprintf(path,"%s\\*",snapshotPath);
	printf("%s\n",path);
	//loadTagFile("d:/git/diablo2/tag.txt");
	WIN32_FIND_DATAA fd;
	int hasMore=1;
	for (HANDLE h=FindFirstFile(path,&fd);h!=INVALID_HANDLE_VALUE&&hasMore;hasMore=FindNextFile(h,&fd)) {
		int isdir=fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
		if (!isdir) continue;
		if (stricmp(fd.cFileName,".")==0) continue;
		if (stricmp(fd.cFileName,"..")==0) continue;
		if (stricmp(fd.cFileName,"dat")==0) continue;
		if (stricmp(fd.cFileName,"debug")==0) continue;
		if (stricmp(fd.cFileName,"log")==0) continue;
		scan_realm(fd.cFileName);
	}
	return 0;
}
