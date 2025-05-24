#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>

char *load(char *path, int addr,int len, int *rlen) {
	assert(addr>=0);
	if (len<=0) {
		struct stat st;if (stat(path,&st)!=0) {printf("can't stat %s\n",path);exit(1);}
		len=st.st_size;
	}
	FILE *fp=fopen(path,"rb");
	if (!fp) {fprintf(stderr, "can't open %s\n",path);*rlen=0;return NULL;}
	char *buf=(char *)malloc(len+1);buf[len]=0;
	if (addr>0) {
		fseek(fp,addr,0);
		if (ftell(fp)!=addr) {
			fprintf(stderr,"seek failed %d %d\n", addr,ftell(fp));
			*rlen=0; return NULL;
		}
	}
	int n=fread(buf,1,len,fp);fclose(fp);
	if (rlen) *rlen=n;
	if (len!=n) printf("need %d,read %d\n",len,n);
	assert(len==n);
	return buf;
}
void save(char *path, char *buf,int len) {
	FILE *fp=fopen(path,"wb+");
	if (!fp) {fprintf(stderr, "can't open %s\n",path);return;}
	fwrite(buf,len,1,fp);
	fclose(fp);
	printf("write to %s %d\n",path,len);
}
void hex(int addr,void *buf2,int len) {
	unsigned char *buf=(unsigned char *)buf2;
	int same=0;
	for (int i=0;i<len;i+=16) {
		if (i>0&&i+16<len&&memcmp(buf+i-16,buf+i,16)==0) {
			if (same) {
			} else {
				same=1;
				printf("*\n");
			}
			continue;
		}
		same=0;
		printf("%05x: ",addr+i);
		for (int j=0;j<16;j++) {
			if (j&&j%4==0) printf("- ");
			if (i+j<len) printf("%02x ",buf[i+j]);
			else printf("   ");
		}
		printf("|");
		for (int j=0;j<16&&i+j<len;j++) {
			char c=buf[i+j];
			if (0x20<=c&&c<0x7f) printf("%c",c);
			else printf(".");
		}
		printf("|");
		printf("\n");
	}
}
int main() {
	int start=0x400,end=0x3000,size=0x2a00;
	int len;char *buf;
	//buf=load("d2server.dll",0xFA00,0x4000,&len);save("marsgod.bin",buf,len);
	//int off=0x2a00;hex(off,buf+off,len-off);
	buf=load("d2server.dll",0,0,&len);
	int modlen;char *mod=load("obj/server.dll",start,size,&modlen);
	memcpy(buf+0xFA00,mod,modlen);
	save("obj/d2server.dll",buf,len);
	return 0;
	buf=load("pvp.dll",start,size,&len);save("pvp.bin",buf,len);
	return 0;
}
