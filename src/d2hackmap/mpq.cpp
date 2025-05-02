#include "stdafx.h"
#include "header.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

int dwDumpMpqPath;

static u32 mpq_crypt_table[0x500];
void init_crypt_table() {
	u32 seed=0x00100001;
	for(int i = 0; i < 0x100; i++) {
		for(int j = i; j < 0x500; j += 0x100) {
			seed=(seed*125+3)%0x2AAAAB;u32 high=(seed&0xFFFF)<<16;
			seed=(seed*125+3)%0x2AAAAB;mpq_crypt_table[j]=high|(seed&0xFFFF);
		}
	}
}
u32 mpq_hashstring(const char *s, u32 offset) {
	u32 a=0x7FED7FED,b=0xEEEEEEEE,c;
	if (s) while (*s) {c=toupper(*s++);a=mpq_crypt_table[offset+c]^(a+b);b=c+a+b+(b<<5)+3;}
	return a;
}
static char *mpq_table_path="runtime/mpqtable.bin";
static char *mpq_list_path="runtime/mpqlist.txt";
static int mpq_table_modified=0;
struct mpqhash {int a,b;};
static struct mpqhash *mpqhashs=NULL;
static int mpq_err=0;
static FILE *mpq_fp=NULL;
void saveMpqTable() {
	if (!mpqhashs||!mpq_table_modified) return;
	FILE *fp=openFile(mpq_table_path,"wb+");
	if (fp) {
		fwrite(mpqhashs,1,sizeof(struct mpqhash)*0x10000,fp);fclose(fp);
		LOG("Save %s %d\n",mpq_table_path,mpq_table_modified);
		mpq_table_modified=0;
	}
}
int __fastcall add_mpq_path(char *path) {
	int pos=mpq_hashstring(path,0)&0xFFFF;
	int a=mpq_hashstring(path,0x100),b=mpq_hashstring(path,0x200);
	for (int i=0;i<0x10000;i++) {
		struct mpqhash *p=&mpqhashs[pos];
		if (p->a==a&&p->b==b) return 0;
		if (p->a==0&&p->b==0) {
			p->a=a;p->b=b;mpq_table_modified++;
			if (mpq_fp) {
				fprintf(mpq_fp,"#%04X %08X_%08X\t%s\n",pos,a,b,path);fflush(mpq_fp);
			}
			return 1;
		}
		pos++;if (pos>=0x10000) pos=0;
	}
	return 0;
}
void __fastcall dump_mpq_path(int esp0) {
	if (mpq_err>100) return;
	if (!mpq_fp) {
		LOG("mpq list %s\n",mpq_list_path);
		if (!mpqhashs) {
			init_crypt_table();
			mpqhashs=(struct mpqhash *)HeapAlloc(dllHeap,HEAP_ZERO_MEMORY,0x10000*sizeof(struct mpqhash));
			FILE *fp=openFile(mpq_table_path,"rb");
			if (fp) {
				fread(mpqhashs,1,sizeof(struct mpqhash)*0x10000,fp);fclose(fp);
				int n=0;
				for (int i=0;i<0x10000;i++) {
					if (mpqhashs[i].a||mpqhashs[i].b) n++;
				}
				LOG("Load %s %d\n",mpq_table_path,n);
			}
		}
		/*
		FILE *fplist=openFile(mpq_list_path,"r");
		if (fplist) {
			while (1) {
				char buf[256];char *line=fgets(buf,256,fplist);if (!line) break;
				int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}
				if (line[0]) add_mpq_path(line);
			}
			fclose(fplist);
		}
		*/
		mpq_fp=openFile(mpq_list_path,"a+");
		if (!mpq_fp) mpq_err++;
	}
	char *path=(char *)*(int *)(esp0+8);
	if (mpq_fp&&add_mpq_path(path)) {
		/*
		int ret=*(int *)esp0;
		if (ret!=(int)storm_mpq_open_block+0x3F) {
			char *mpq="";
			int from=*(int *)(esp0+4);
			if (from) mpq=(char *)(from+8);
			int flags=*(int *)(esp0+12);
			fprintf(mpq_fp,"#storm267 %X %s F%x\t%s\n",ret,mpq,flags,path);fflush(mpq_fp);return;
		} 
		esp0+=20;ret=*(int *)esp0;
		if (ret!=(int)fog_mpq_open_block+7) {
			fprintf(mpq_fp,"#storm268 %X\t%s\n",ret,path);fflush(mpq_fp);return;
		} 
		esp0+=12;ret=*(int *)esp0;
		if (ret!=(int)d2cmp_mpq_path_3010+0xB) {
			fprintf(mpq_fp,"#fog10102 %X\t%s\n",ret,path);fflush(mpq_fp);return;
		}
		ret=*(int *)(esp0+4+24);
		fprintf(mpq_fp,"#d2cmp3010 %X\t%s\n",ret,path);fflush(mpq_fp);
		*/
	}
}
/* 
STORM.Ordinal268  args: mpq path flags pointer
6FC18960 - 8B 4C 24 10           - mov ecx,[esp+10]
6FC18964 - 81 EC 10010000        - sub esp,00000110 { 272 }
// push ebx,ebp,esi,edi,eax
// eax:path
*/
void __declspec(naked) storm268_patch_ASM() {
	__asm {
		cmp dwGameWindowId,1
		jne original
		cmp dwDumpMpqPath,0
		je original
		lea eax, [esp+4]
		pushad
		mov ecx, eax
		call dump_mpq_path
		popad
original:
		pop eax
		sub esp,0x110
		jmp eax
	}
}
