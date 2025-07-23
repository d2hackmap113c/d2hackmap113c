#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
struct Dll {char *name;int addr,end;};
struct Dll dlls[]={
	{"d2game", 0x009C0000 ,0x00AE0000},
	{"d2net", 0x00AF0000, 0x00AFCFFF},
	{"d2gdi", 0x6F870000 ,0x6F87DFFF},
	{"d2win", 0x6F8E0000,0x6F9AEFFF},
	{"d2sound", 0x6F9B0000 ,0x6F9C8FFF},
	{"d2multi", 0x6F9D0000 ,0x6FA0EFFF},
	{"d2mcpclient", 0x6FA20000 ,0x6FA33FFF},
	{"d2launch", 0x6FA40000 ,0x6FA6CFFF},
	{"d2gfx", 0x6FA80000 ,0x6FAA0FFF},
	{"d2client", 0x6FAB0000 ,0x6FBE4FFF},
	{"storm", 0x6FBF0000 ,0x6FC4FFFF},
	{"d2lang", 0x6FC00000 ,0x6FC13FFF},
	{"d2common", 0x6FD50000 ,0x6FDF8FFF},
	{"d2cmp", 0x6FE10000 ,0x6FF17FFF},
	{"bnclient", 0x6FF20000 ,0x6FF43FFF},
	{"fog", 0x6FF50000 ,0x6FFABFFF},
	{0},
};
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
int dllOff(char *dll,int addr) {
	static struct Dll *last=NULL;
	if (last&&strcmp(last->name,dll)==0) {
		if (addr<last->addr||addr>last->end) printf("ERR: %s %X\n",dll,addr);
		return addr-last->addr;
	}
	//printf("search %s\n",dll);
	for (struct Dll *p=dlls;p->name;p++) {
		if (strcmp(p->name,dll)==0) {
			last=p;
			if (addr<last->addr||addr>last->end) printf("ERR: %s %X\n",dll,addr);
			return addr-last->addr;
		}
	}
	return addr;
}
*/
void readSymbols(char *path,FILE *fpout) {
	FILE *fp=fopen(path,"r");if (!fp) return;
	while (1) {
		char buf[256];char *line=fgets(buf,256,fp);if (!line) break;
		int len=strlen(line);
		if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[len-1]=='\r') {len--;line[len]=0;}
		char *s=line;
		while (*s==' '||*s=='\t') s++;
		int idx=0,isFun=0;
		if (memcmp(s,"DLL_VAR",7)==0) {
			idx=1;
		} else if (memcmp(s,"DLL_FUN",7)==0) {
			idx=1;isFun=1;
		} else if (memcmp(s,"DLL_ORD",7)==0) {
			idx=2;isFun=1;
		} else continue;
		if (!idx) continue;
		char *e;
		s=strchr(s,'(');if (!s) continue;s++;
		while (*s==' '||*s=='\t') s++;
		char *dll=s;
		s=strchr(s,',');if (!s) continue;*s=0;s++;
		while (*s==' '||*s=='\t') s++;
		char *addrS=s;
		int off=strtol(addrS,NULL,16);
		s=strchr(s,',');if (!s) continue;*s=0;s++;
		if (idx>1) {s=strchr(s,',');if (!s) continue;*s=0;s++;}
		while (*s==' '||*s=='\t') s++;
		char *name=s;s=strchr(s,',');if (!s) continue;*s=0;e=s-1;s++;
		while (*e==' '||*e=='\t') {*e=0;e--;}
		while (*s==' '||*s=='\t') s++;
		char *retType=s,*stackType="",*args="",*comment="";
		if (isFun) {
			s=strchr(s,',');if (!s) goto print;*s=0;s++;
			while (*s==' '||*s=='\t') s++;
			stackType=s;
			s=strchr(s,',');if (!s) goto print;*s=0;s++;
			while (*s==' '||*s=='\t') s++;if (*s=='(') s++;
			args=s;
			s=strchr(s,')');if (!s) goto print;*s=0;s++;
			while (*s&&*s!='/') s++;
			comment=s;
			s=strchr(s,'\r');if (s) *s=0;
		} else {
			s=strchr(s,')');if (s) *s=0;
		}
print:
		{
			char type=isFun?'f':'v';
			if (isFun) 
				fprintf(fpout,"%c\t%s_%X\t%s\t%s %s_%s(%s)%s\n",
					type,dll,off,retType,stackType,dll,name,args,comment);
			else
				fprintf(fpout,"%c\t%s_%X\t%s\t%s_%s\n",type,dll,off,retType,dll,name);
		}
	}
	fclose(fp);
}
void exportSymbols(char *path) {
	FILE *fp=fopen(path,"w+");
	if (!fp) printf("can't open %s\n",path);
	assert(fp);
	readSymbols("/d2hackmap/src/d2hackmap/d2ptrs.h",fp);
	fclose(fp);
}

int main(int argc, char *argv[]) {
	//oldcodes();return 0;
	char *path="/d2hackmap/src/symbols.txt";
	if (argc>=2) path=argv[1];
	exportSymbols(path);
	return 0;
}

