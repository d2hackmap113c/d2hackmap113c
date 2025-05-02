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
	if (n%16) printf("\n");
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
int needConvert(char *s) {
	while (*s) {
		if (*s&0x80) return 1;
		s++;
	}
	return 0;
}
void checkfile(char *path) {
	struct stat st;
	if (stat(path,&st)!=0) return;
	if (st.st_size==0) return;
	char *ext=strrchr(path,'.');if (!ext) return;ext++;
	int src=0;
	if (stricmp(ext,"cpp")==0
		||stricmp(ext,"c")==0
		||stricmp(ext,"h")==0) {
		src=1;
	} else if (stricmp(ext,"md")==0
		||stricmp(ext,"txt")==0) {
		src=0;
	} else return;
	FILE *fp=fopen(path,"rb");if (!fp) return;
	char header[3];fread(header,1,3,fp);
	if (header[0]==(char)0xef&&header[1]==(char)0xbb&&header[2]==(char)0xbf) {fclose(fp);return;}
	if (src) {printf("Not UTF8 %s\n",path);fclose(fp);return;}
	fseek(fp,0,0);
	int conv=0,n=0;
	while (1) {
		char buf[256];char *line=fgets(buf,256,fp);if (!line) break;
		n++;
		if (needConvert(line)) {
			conv=1;
			printf("%s:%d: %s\n",path,n,line);
			break;
		}
	}
	fclose(fp);
}
void scandir(char *path) {
	char ptn[256];
	char subpath[256];
	WIN32_FIND_DATAA fd;
	int hasMore=1;
	snprintf(ptn,256,"%s\\*",path);
	for (HANDLE h=FindFirstFile(ptn,&fd);h!=INVALID_HANDLE_VALUE&&hasMore;hasMore=FindNextFile(h,&fd)) {
		int isdir=fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY;
		if (stricmp(fd.cFileName,".")==0) continue;
		if (stricmp(fd.cFileName,"..")==0) continue;
		if (stricmp(fd.cFileName,".git")==0) continue;
		if (stricmp(fd.cFileName,"t")==0) continue;
		snprintf(subpath,256,"%s\\%s",path,fd.cFileName);
		if (isdir) scandir(subpath);
		else checkfile(subpath);
	}
}
void test() {
	wchar_t dst[32];
	char utf8_fire[4]={0xe7,0x81,0xab,0};
	char *src="»ð"; //706B
	int n=MultiByteToWideChar(CP_UTF8, 0, utf8_fire, -1, dst, 32);
	printf("%d %X %X\n",n,dst[0],dst[1]);
	n=MultiByteToWideChar(CP_ACP, 0, src, -1, dst, 32);
	printf("%d %X %X\n",n,dst[0],dst[1]);

}
int main(int argc, char* argv[]) {
	//test();return;
	//scandir("d:\\d2hackmap\\src\\d2hackmap");
	//scandir("d:\\d2hackmap\\src\\loadhackmap");
	scandir("d:\\d2hackmap\\src\\server");
	return 0;
}
