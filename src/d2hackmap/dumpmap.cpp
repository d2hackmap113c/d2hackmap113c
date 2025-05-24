#include "stdafx.h"
#include <stdio.h>
#include <assert.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
void dumpMap(char *path,char *outpath,char *dll) {
	FILE *fp=fopen(path,"r");if (!fp) return;
	FILE *outfp=fopen(outpath,"w+");if (!outfp) return;
	int start=0;
	int loadAddr=0x66000000;
	while (1) {
		char buf[256];char *line=fgets(buf,256,fp);if (!line) break;
		char *s=line;
		while (*s==' '||*s=='\t') s++;
		if (!start) {
			if (memcmp(s,"Preferred load address is",25)==0) {
				s+=25;while (*s==' ') s++;
				loadAddr=strtol(s,0,16);
				printf("dll base: %X\n",loadAddr);
			}
			if (memcmp(s,"Address",7)==0) start=1;
			continue;
		}
		if (*s!='0') continue;
		s=strchr(s,' ');if (!s) continue;
		while (*s==' '||*s=='\t') s++;
		char *name=s;
		s=strchr(s,' ');if (!s) continue;
		*s=0;s++;while (*s==' '||*s=='\t') s++;
		int addr=strtol(s,&s,16);
		assert(*s==' ');s++;
		int isFun=*s=='f';
		if (!addr) continue;
		if (name[0]=='_') name++;
		fprintf(outfp,"%c\t%X\t%s_%s\n",isFun?'f':'0',addr-loadAddr,dll,name);
	}
	fclose(outfp);
	fclose(fp);
}

int main(int argc, char *argv[]) {
	if (argc<3) {printf("%s <exe.map> <output> [dll_name]\n",argv[0]);return 0;}
	char *dll="";
	if (argc>=3) dll=argv[3];
	dumpMap(argv[1],argv[2],dll);
	return 0;
}

