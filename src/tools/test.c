#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
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
typedef void *(__fastcall* AllocMem)(int arg0,int size,char *source,int line,int arg4);
typedef int (__fastcall* FreeMem)(int arg0,void *ptr,char *source,int line,int arg4);
AllocMem allocMem = 0;
FreeMem freeMem = 0;
int main(int argc, char* argv[]) {
	HINSTANCE storm = LoadLibrary("d:\\game\\diablo2\\storm.dll");assert(storm);
	HINSTANCE fog = LoadLibrary("d:\\game\\diablo2\\fog.dll");assert(fog);
	allocMem = (AllocMem)GetProcAddress(fog,(LPCSTR)10045);
	freeMem = (FreeMem)GetProcAddress(fog,(LPCSTR)10046);
	printf("%x\n",fog);
	printf("%x\n",allocMem);
	printf("%x\n",freeMem);
#define N 4
	void *ptr[N];
	for (int i=0;i<N;i++) {
		int size=32;
		ptr[i]=allocMem(0,size,"aaa",0,0);
		printf("%d %x %d\n",i,ptr[i],size);
		memset(ptr[i],0,size);
	}
	for (int i=0;i<N;i++) {
		int r1=freeMem(0,ptr[i],"aaa",0,0);
		//printf("%x\n",r1);
	}
	for (int i=0;i<N;i++) {
		ptr[i]=allocMem(0,32,"aaa",0,0);
		printf("%d %x\n",i,ptr[i]);
	}
	
	return;
}
