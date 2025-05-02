#include "stdafx.h"

#define LOG printf
#define INST_NOP 0x90
#define INST_CALL 0xe8

extern char mpq_path[256];

HMODULE get_dll_addr(char *name) {
	HMODULE hmod = GetModuleHandle(name);
	if (!hmod) hmod = LoadLibrary(name);
	if (!hmod) {
		char buf[256];sprintf(buf,"load %s error",name);
		MessageBox(NULL,buf,"d2hackmap",MB_OK);
	}
	return hmod;
}
struct InstalledPatch {int addr,len;struct InstalledPatch *next;char oldcode[4];};
static struct InstalledPatch *installedPatches=NULL;
int installPatch(void (*func)(DWORD, DWORD, int),DWORD addr,DWORD param,int len) {
	if (!func) return 0;
	struct InstalledPatch *p=(struct InstalledPatch *)malloc(sizeof(struct InstalledPatch)+len);
	p->addr=addr;p->len=len;p->next=installedPatches;
	memcpy(p->oldcode,(void *)addr,len);
	func(addr,param,len);
	installedPatches=p;
	return 1;
}
DWORD VirtualProtect(DWORD pAddress, DWORD len, DWORD prot) {
	DWORD oldprot = 0;VirtualProtect((void *)pAddress, len, prot, &oldprot);return oldprot;
}
void WriteLocalBYTES(DWORD pAddress, void *buf, int len) {
	DWORD oldprot = VirtualProtect(pAddress, len, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(GetCurrentProcess(), (void *)pAddress, buf, len, 0);
	VirtualProtect(pAddress, len, oldprot);
}
void InterceptLocalCode2(BYTE inst, DWORD pOldCode, DWORD pNewCode, int len) {
	BYTE *buf1 = new BYTE[len];
	buf1[0] = inst;
	*(DWORD *)(buf1+1) = pNewCode;
	memset(buf1+5, INST_NOP, len-5);
	WriteLocalBYTES(pOldCode, buf1, len);
	delete buf1;
}
void InterceptLocalCode(BYTE inst, DWORD pOldCode, DWORD pNewCode, int len) {
	pNewCode -= (pOldCode+5);
	InterceptLocalCode2(inst, pOldCode, pNewCode, len);
}
void PatchCALL(DWORD addr, DWORD param, int len) {
	InterceptLocalCode(INST_CALL, addr, param, len);
}
void PatchVALUE(DWORD addr, DWORD param, int len) {
	WriteLocalBYTES(addr, &param, len);
}
//6FC1A1F8 - FF 15 4432C36F        - call dword ptr [6FC33244] { ->7C801A28 kernel32.CreateFileA}
HANDLE __stdcall openMpq_patch(LPSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile) {
	char buf[MAX_PATH];
	if (mpq_path[0]) {
		struct stat st;
		char *name=strrchr(lpFileName,'\\');
		if (name) name++;else name=lpFileName;
		sprintf(buf,"%s\\%s",mpq_path,name);
		if (stat(buf,&st)==0) {
			LOG("Open mpq file: %s -> %s\n",lpFileName,buf);
			lpFileName=buf;
		} else {
			LOG("Open mpq file: %s\n",lpFileName);
		}
	} else {
		LOG("Open mpq file: %s\n",lpFileName);
	}
	return CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,
		dwFlagsAndAttributes,hTemplateFile);
}
void D2GSEventLog(char const * module, char const * fmt, ...);
void __fastcall dump_mpq_path(int esp0) {
	char *path=(char *)*(int *)(esp0+8);
	D2GSEventLog("MPQ", path);
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
void __declspec(naked) storm268_patch_ASM() {
	__asm {
		lea eax, [esp+4]
		pushad
		mov ecx, eax
		call dump_mpq_path
		popad
		pop eax
		sub esp,0x110
		jmp eax
	}
}
void init_crypt_table(unsigned int *mpq_crypt_table) {
	unsigned int seed=0x00100001;
	for(int i = 0; i < 0x100; i++) {
		for(int j = i; j < 0x500; j += 0x100) {
			seed=(seed*125+3)%0x2AAAAB;unsigned int high=(seed&0xFFFF)<<16;
			seed=(seed*125+3)%0x2AAAAB;mpq_crypt_table[j]=high|(seed&0xFFFF);
		}
	}
}
int getFileSize(const char *path) {struct stat st;if (stat(path,&st)!=0) return -1;return st.st_size;}
int fileExist(const char *path) {return getFileSize(path)!=-1;}
void cp(FILE *in,FILE *out) {
	char buf[4096];
	while (1) {int len=fread(buf,1,4096,in);if (len<=0) break;fwrite(buf,1,len,out);}
}
extern char conf_d2hackmap_path0[256],conf_d2hackmap_path[256],conf_d2hackmap_cfg_path[256];
static char d2hackmap_path0[256],d2hackmap_path[256],d2hackmap_cfg_path[256];
static HMODULE d2hackmap_addr=NULL;
static int d2hackmap_reload_flag=0,d2hackmap_exit_flag=0;
extern char server_root[256];
int is_d2hackmap_modified() {
	int modified=1;
	if (fileExist(d2hackmap_path)) {
		struct stat st1,st2;
		if (stat(d2hackmap_path0,&st1)==0&&stat(d2hackmap_path,&st2)==0) {
			if (st1.st_mtime<=st2.st_mtime&&st1.st_size==st2.st_size) modified=0;
		}
	}
	return modified;
}
void load_d2hackmap() {
	d2hackmap_reload_flag=0;
	d2hackmap_exit_flag=0;
	if (conf_d2hackmap_path0[1]!=':') _snprintf(d2hackmap_path0,256,"%s\\%s",server_root,conf_d2hackmap_path0);
	else strcpy(d2hackmap_path0,conf_d2hackmap_path0);
	if (conf_d2hackmap_path[1]!=':') _snprintf(d2hackmap_path,256,"%s\\%s",server_root,conf_d2hackmap_path);
	else strcpy(d2hackmap_path,conf_d2hackmap_path);
	if (conf_d2hackmap_cfg_path[1]!=':') _snprintf(d2hackmap_cfg_path,256,"%s\\%s",server_root,conf_d2hackmap_cfg_path);
	else strcpy(d2hackmap_cfg_path,conf_d2hackmap_cfg_path);
	if (!fileExist(d2hackmap_path0)) {printf("Can't find %s\n",d2hackmap_path0);return;}
	if (is_d2hackmap_modified()) {
		FILE *fpout=fopen(d2hackmap_path,"wb+");
		if (!fpout) {printf("open %s failed\n",d2hackmap_path);return;} 
		printf("copy %s to %s\n",d2hackmap_path0,d2hackmap_path);
		FILE *fpin=fopen(d2hackmap_path0,"rb");
		if (fpin) cp(fpin,fpout);
		fclose(fpin);fclose(fpout);
	}
	d2hackmap_addr=LoadLibrary(d2hackmap_path);
	printf("Load 0x%X %s\n",d2hackmap_addr,d2hackmap_path);
	if (!d2hackmap_addr) return;
	void (__stdcall *PatchServer)(char *path)=(void (__stdcall *)(char *))GetProcAddress(d2hackmap_addr,"PatchServer");
	//printf("PatchServer 0x%X\n",PatchServer);
	if (PatchServer) PatchServer(d2hackmap_cfg_path);
	void (__fastcall *SetReloadFlag)(int *,int *)=
		(void (__fastcall *)(int *,int *))GetProcAddress(d2hackmap_addr,"SetReloadFlag");
	//printf("SetReloadFlag=0x%X\n",(int)SetReloadFlag);
	if (SetReloadFlag) SetReloadFlag(&d2hackmap_reload_flag,&d2hackmap_exit_flag);
}
void check_reload_d2hackmap() {
	if (!d2hackmap_addr) return;
	if (!is_d2hackmap_modified()) return;
	for (int i=0;i<10;i++) {
		if (!FreeLibrary(d2hackmap_addr)) {
			printf("unload 0x%X %s error\n",d2hackmap_addr,d2hackmap_path);
			return;
		}
		if (d2hackmap_exit_flag) break;
	}
	load_d2hackmap();
}
#define DLL_ADDR(dll,addr) (addr-##dll##_ADDR+(DWORD)addr_##dll)
#define d2win_ADDR 0x6F8E0000
#define d2client_ADDR 0x6FAB0000
#define storm_ADDR 0x6FBF0000
int Install() {
  HMODULE addr_storm=get_dll_addr("STORM.DLL");if (!addr_storm) return 0;
	installPatch(PatchCALL,DLL_ADDR(storm,0x6FC1A1F8),(DWORD)openMpq_patch,6);
	//installPatch(PatchCALL,DLL_ADDR(storm,0x6FC18964),(DWORD)storm268_patch_ASM,6);
	{
		int *cryptTable=(int *)DLL_ADDR(storm,0x6FC43120);
		printf("cryptTable=%x\n",*cryptTable);
		*cryptTable=0;
		/*cryptTable=*(int *)cryptTable;
		printf("[cryptTable]=%x\n",cryptTable);
		init_crypt_table((unsigned int *)cryptTable);
		*/
	}
	//LOG("patch installed\n");
	load_d2hackmap();
	return 1;
}
void Uninstall(){
	LOG("Uninstall patches\n");
	for (struct InstalledPatch *p=installedPatches;p;) {
    if (p->oldcode) WriteLocalBYTES(p->addr, p->oldcode, p->len);
		struct InstalledPatch *p2=p;p=p->next;free(p2);
	}
	installedPatches=NULL;
}
