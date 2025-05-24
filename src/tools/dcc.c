#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>
#include <assert.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
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
void save(char *path,char *buf,int size) {
	FILE *fp=fopen(path,"wb+");assert(fp);
	int n=fwrite(buf,1,size,fp);assert(n==size);fclose(fp);
}
char *load(char *path,int *psize) {
	FILE *fp=fopen(path,"rb");if (!fp) printf("can't open %s\n",path);assert(fp);
	fseek(fp,0,2);int size=ftell(fp);*psize=size;
	char *p=malloc(size+1);p[size]=0;fseek(fp,0,0);
	int n=fread(p,1,size,fp);assert(n==size);fclose(fp);
	return p;
}
strcut dcc_header __attribute__((packed)) {
	u8 magic,version,nDirs;//0x74,6,0x10
	u32 nFrames; //0x10
	u32 magic2; //1
	u32 totalSizeCoded;
};

void dccbmp(char *inpath,char *outfmt) {
	assert(sizeof(struct dcc_header)==15);
	int size;char *dc6bin=load(inpath,&size);
	struct dcc_header *dcc=(struct dcc_header *)dc6bin;
	printf("%d*%d %s %s\n",dcc->nDirs,dcc->nFrames,inpath,outfmt);
	int *offsets=(int *)(dc6bin+15);
	for (int i=0;i<dcc->nDirs;i++) {
		int offset=*offsets++;
	}
}
