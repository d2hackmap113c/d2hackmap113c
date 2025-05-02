// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include "d2ptrs.h"

#define DECLARE_D2PTR_VARIBLE
#include "d2ptrs.h"
#undef DECLARE_D2PTR_VARIBLE

static char reloadStore[1024]={0};
char d2path[MAX_PATH];
char mpqpath[MAX_PATH];
char bnetip[MAX_PATH];
FILE *logfp;
#define LOG(fmt,...) {if (logfp) {fprintf(logfp,fmt,__VA_ARGS__);fflush(logfp);}}
#define INST_NOP 0x90
#define INST_CALL 0xe8
HMODULE hInstDLL=NULL;
char szPluginPath[MAX_PATH]="\0";
char d2hackmap_path0[MAX_PATH]="\0";
char d2hackmap_path[MAX_PATH]="\0";
HMODULE d2hackmap_addr=0;
FILETIME d2hackmap_modtime;
int d2hackmap_loaded=0;
int d2hackmap_reload_flag;
int d2hackmap_exit_flag;
int d2hackmap_exit_time;
int d2hackmap_id=0;
void (__stdcall *d2hackmap_mainmenuFlushFramebuf)();
#define d2game_ADDR 0x009C0000 
#define d2net_ADDR 0x00AF0000
#define d2gdi_ADDR 0x6F870000 
#define d2win_ADDR 0x6F8E0000 
#define d2sound_ADDR 0x6F9B0000 
#define d2multi_ADDR 0x6F9D0000 
#define d2mcpclient_ADDR 0x6FA20000 
#define d2launch_ADDR 0x6FA40000 
#define d2gfx_ADDR 0x6FA80000 
#define d2client_ADDR 0x6FAB0000 
#define storm_ADDR 0x6FBF0000 
#define d2lang_ADDR 0x6FC00000 
#define d2common_ADDR 0x6FD50000 
#define d2cmp_ADDR 0x6FE10000 
#define bnclient_ADDR 0x6FF20000 
#define fog_ADDR 0x6FF50000 
HMODULE addr_storm=(HMODULE)storm_ADDR,addr_d2client=(HMODULE)d2client_ADDR;
HMODULE addr_d2common=(HMODULE)d2common_ADDR,addr_d2gfx=(HMODULE)d2gfx_ADDR;
HMODULE addr_d2win=(HMODULE)d2win_ADDR,addr_d2lang=(HMODULE)d2lang_ADDR;
HMODULE addr_d2cmp=(HMODULE)d2cmp_ADDR,addr_bnclient=(HMODULE)bnclient_ADDR;
HMODULE addr_d2net=(HMODULE)d2net_ADDR,addr_fog=(HMODULE)fog_ADDR;
HMODULE addr_d2game=(HMODULE)d2game_ADDR,addr_d2launch=(HMODULE)d2launch_ADDR;
HMODULE addr_d2gdi=(HMODULE)d2gdi_ADDR,addr_d2sound=(HMODULE)d2sound_ADDR;
HMODULE addr_d2multi=(HMODULE)d2multi_ADDR,addr_d2mcpclient=(HMODULE)d2mcpclient_ADDR;
#define DLL_ADDR(dll,addr) (addr-##dll##_ADDR+(DWORD)addr_##dll)

extern "C" void __stdcall d2hackmap_reload() {d2hackmap_reload_flag=1;}
extern "C" void __stdcall d2hackmap_exit() {d2hackmap_exit_flag=1;}
int getFileSize(const char *path) {struct stat st;if (stat(path,&st)!=0) return -1;return st.st_size;}
int fileExist(const char *path) {return getFileSize(path)!=-1;}
void cp(FILE *in,FILE *out) {
	char buf[4096];
	while (1) {
		int len=fread(buf,1,4096,in);if (len<=0) break;
		fwrite(buf,1,len,out);
	}
}
void load_d2hackmap() {
	d2hackmap_reload_flag=0;
	d2hackmap_exit_flag=0;
	sprintf(d2hackmap_path,"%sruntime\\d2hackmap%d.dll",szPluginPath,d2hackmap_id);
	char *path=d2hackmap_path;
	int update=1;
	if (fileExist(d2hackmap_path)) {
		struct stat st1,st2;
		if (stat(d2hackmap_path0,&st1)==0&&stat(d2hackmap_path,&st2)==0) {
			//LOG("filetime %d %d\n",(int)st1.st_mtime,(int)st2.st_mtime);
			if (st1.st_mtime<=st2.st_mtime&&st1.st_size==st2.st_size) update=0;
		}
	}
	if (update) {
		FILE *fpout=fopen(d2hackmap_path,"wb+");
		if (!fpout) {
			LOG("open %s failed\n",d2hackmap_path);
			path=d2hackmap_path0;
		} else {
			LOG("copy %s to %s\n",d2hackmap_path0,d2hackmap_path);
			FILE *fpin=fopen(d2hackmap_path0,"rb");
			if (fpin) cp(fpin,fpout);
			fclose(fpin);fclose(fpout);
			path=d2hackmap_path;
		}
	}
	d2hackmap_addr=LoadLibrary(path);
	LOG("Load 0x%X %s\n",d2hackmap_addr,path);
	if (!d2hackmap_addr) {MessageBox(NULL,"Load d2hackmap failed","loadhackmap",MB_OK);return;}

	d2hackmap_mainmenuFlushFramebuf=(void (__stdcall *)())GetProcAddress(d2hackmap_addr,"MainMenuFlushFramebuf");

	void (__cdecl *PatchClient)(char *path,char *,int)=
		(void (__cdecl *)(char *path,char *,int))GetProcAddress(d2hackmap_addr,"PatchClient");
	//LOG("PatchClient=0x%X\n",(int)PatchClient);
	if (PatchClient) PatchClient(szPluginPath,reloadStore,1024);
	void (__fastcall *SetReloadFlag)(int *,int *)=
		(void (__fastcall *)(int *,int *))GetProcAddress(d2hackmap_addr,"SetReloadFlag");
	//LOG("SetReloadFlag=0x%X\n",(int)SetReloadFlag);
	if (SetReloadFlag) SetReloadFlag(&d2hackmap_reload_flag,&d2hackmap_exit_flag);
	return;
}
void reload_d2hackmap() {
	if (!d2hackmap_loaded) {d2hackmap_loaded=1;load_d2hackmap();return;}
	if (d2hackmap_exit_flag) {
		if (GetTickCount()-d2hackmap_exit_time>500)
			load_d2hackmap();
	} else {
		LOG("FreeLibrary 0x%x\n",d2hackmap_addr);
		for (int i=0;i<10;i++) {
			if (!FreeLibrary(d2hackmap_addr)) {
				MessageBox(NULL,"free library error","loadhackmap",MB_OK);
				return;
			}
			if (d2hackmap_exit_flag) {
				d2hackmap_mainmenuFlushFramebuf=NULL;
				d2hackmap_exit_time=GetTickCount();
				//LOG("d2hackmap exited at loop %d\n",i);
				//MessageBox(NULL,"d2hackmap exit","loadhackmap",MB_OK);
				return;
			}
		}
		MessageBox(NULL,"d2hackmap didn't exit","loadhackmap",MB_OK);
	}
}

//6F8F8A3A - E8 A1FDFFFF           - call 6F8F87E0 D2Win.Ordinal10024<-------
void __declspec(naked) MenuLoop_Patch() {
	__asm {
		cmp d2hackmap_loaded,0
		je reload
		cmp d2hackmap_reload_flag,0
		jne reload
		call d2win_drawMainMenu
		ret
reload:
		pushad
		call reload_d2hackmap
		popad
		call d2win_drawMainMenu
		ret
	}
}
//6FAF4237 - E8 B4D20700           - call 6FB714F0
void __declspec(naked) GameLoop_Patch() {
	__asm {
		cmp d2hackmap_loaded,0
		je reload
		cmp d2hackmap_reload_flag,0
		jne reload
		call d2client_14F0
		ret
reload:
		pushad
		call reload_d2hackmap
		popad
		call d2client_14F0
		ret
	}
}
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
static InstalledPatch *installedPatches=NULL;
int installPatch(void (*func)(DWORD, DWORD, int),DWORD addr,DWORD param,int len) {
	if (!func) return 0;
	InstalledPatch *p=(InstalledPatch *)malloc(sizeof(InstalledPatch)+len);
	p->addr=addr;p->len=len;p->next=installedPatches;
	memcpy(p->oldcode,(void *)addr,len);
	if (0&&logfp) {
		fprintf(logfp,"Install Patch 0x%x oldcode[%d]={",addr,len);
		for (int i=0;i<len;i++) fprintf(logfp,"0x%02x,",(unsigned char)p->oldcode[i]);
		fprintf(logfp,"};\n");
		fflush(logfp);
	}
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
	if (mpqpath[0]) {
		struct stat st;
		char *name=strrchr(lpFileName,'\\');
		if (name) name++;else name=lpFileName;
		sprintf(buf,"%s\\%s",mpqpath,name);
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
//STORM.Ordinal427+1EC - 8B 2D 1030C36F        - mov ebp,[6FC33010] { (77DA7AAB) ADVAPI32.RegQueryValueExA }
LONG __stdcall MyRegQueryValueExA(HKEY hKey,LPCSTR lpValueName,LPDWORD lpReserved,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData) {
	int ret=RegQueryValueExA(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
	if (d2path[0]&&strcmp(lpValueName,"InstallPath")==0) {
		int len=strlen(d2path);
		if (lpData) {
			memcpy(lpData,d2path,len+1);
			if (lpcbData) *lpcbData=len+1;
		}
	} else if (d2path[0]&&strcmp(lpValueName,"BNETIP")==0) {
		int len=strlen(bnetip);
		if (lpData) {
			memcpy(lpData,bnetip,len+1);
			if (lpcbData) *lpcbData=len+1;
		}
	}
	if (logfp) {
		fprintf(logfp,"RegQueryValueExA %s",lpValueName);
		if (lpType) {
			fprintf(logfp," type=%d",*lpType);
			switch (*lpType) {
			case 1:
				if (lpData) fprintf(logfp," \"%s\"",lpData);break;
			case 7: 
				if (lpData&&lpcbData) {
					char *s=(char *)lpData,*end=(char *)lpData + *lpcbData;
					while (s<end) {
						fprintf(logfp," \"%s\"",s);
						s+=strlen(s);
						while (!(*s)&&s<end) s++;
					}
				}
				break;
			case 4:
				fprintf(logfp," value=%d",*(int *)lpData);break;
			}
		}

		if (lpcbData) fprintf(logfp," len=%d",*lpcbData);
		fputc('\n',logfp);
		fflush(logfp);
	}
	return ret;
}
static wchar_t lastMsg[256];
static int lastMsgColor=0,lastMsgTimeoutMs=0;
void drawMainMenuMessages() {
	if (lastMsgTimeoutMs) {
		DWORD dwOldFone = d2win_SetTextFont(1);
		d2win_DrawText(lastMsg,10,30,lastMsgColor,-1);
		d2win_SetTextFont(dwOldFone);
		if ((int)GetTickCount()>lastMsgTimeoutMs) lastMsgTimeoutMs=0;
	}
}
static void mainmenuFlushFramebuf() {
	drawMainMenuMessages();
	if (d2hackmap_mainmenuFlushFramebuf) d2hackmap_mainmenuFlushFramebuf();
}
//6F8F896C - E8 51EEFEFF           - call 6F8E77C2 { ->->6FA8ADD0 d2gfx_flushFramebuf} //put to screen
void __declspec(naked) MainMenuFlushFramebufPatch_ASM() {
	__asm {
		pushad
		call mainmenuFlushFramebuf
		popad
		jmp d2gfx_flushFramebuf
	}
}
#define d2win_ADDR 0x6F8E0000
#define d2client_ADDR 0x6FAB0000
#define storm_ADDR 0x6FBF0000
void parseCmdLine() {
	char buf[1024];
	mpqpath[0]=0;d2path[0]=0;bnetip[0]=0;
	char *cmdLine = (char *)GetCommandLine();
	int len=strlen(cmdLine);
	if (len>=1024) len=1023;
	memcpy(buf,cmdLine,len);buf[len]=0;
	char *end=buf+len;
	char *e=strchr(buf,' ');if (!e) return;
	*e=0;strcpy(d2path,buf);
	char *s=e+1;
	e=strrchr(d2path,'\\');if (e) e[1]=0;
	while (s<end) {
		while (*s==' ') s++;
		if (memcmp(s,"-mpqpath=",9)==0) {
			s+=9;e=strchr(s,' ');if (e) *e=0;
			strcpy(mpqpath,s);
			if (!e) break;
			s=e+1;
		} else if (memcmp(s,"-bnetip=",8)==0) {
			s+=8;e=strchr(s,' ');if (e) *e=0;
			strcpy(bnetip,s);
			if (!e) break;
			s=e+1;
		} else {
			s=strchr(s,' ');if (!s) break;
		}
	}
	if (d2path[0]) LOG("Install path: %s\n",d2path);
	if (mpqpath[0]) LOG("Mpq path: %s\n",mpqpath);
	if (bnetip[0]) LOG("Bnet ip: %s\n",bnetip);
}
int Install() {
	char buf[MAX_PATH];
	sprintf(buf,"%sruntime",szPluginPath);CreateDirectoryA(buf,NULL);
	for (d2hackmap_id=1;d2hackmap_id<=20;d2hackmap_id++) {
		sprintf(buf,"%sruntime\\loadhackmap%d.log",szPluginPath,d2hackmap_id);
		if (!fileExist(buf)) break;
		if (DeleteFile(buf)) break;
	}
	logfp=fopen(buf,"w+");
  HMODULE addr_storm=get_dll_addr("STORM.DLL");if (!addr_storm) return 0;
	HMODULE addr_d2client=get_dll_addr("D2CLIENT.DLL");if (!addr_d2client) return 0;
  HMODULE addr_d2win=get_dll_addr("D2WIN.DLL");if (!addr_d2win) return 0;
  HMODULE addr_d2gfx=get_dll_addr("D2GFX.DLL");if (!addr_d2gfx) return 0;
	#define INIT_D2PTR_ADDR
	#include "d2ptrs.h"
	#undef INIT_D2PTR_ADDR
	installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F8A3A),(DWORD)MenuLoop_Patch,5);
	installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF4237),(DWORD)GameLoop_Patch,5);
	installPatch(PatchVALUE,DLL_ADDR(d2client,0x6FC33010),(DWORD)MyRegQueryValueExA,4);
	installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F896C),(DWORD)MainMenuFlushFramebufPatch_ASM,5);
	parseCmdLine();
	if (mpqpath[0]) installPatch(PatchCALL,DLL_ADDR(storm,0x6FC1A1F8),(DWORD)openMpq_patch,6);
	LOG("patch installed\n");
	return 1;
}
void Uninstall(){
	LOG("Uninstall patches\n");
	for (InstalledPatch *p=installedPatches;p;) {
    if (p->oldcode) WriteLocalBYTES(p->addr, p->oldcode, p->len);
		InstalledPatch *p2=p;p=p->next;free(p2);
	}
	installedPatches=NULL;
}
BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call,LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		d2hackmap_reload_flag=0;d2hackmap_loaded=0;
		hInstDLL = hModule;
		DisableThreadLibraryCalls(hInstDLL);
		GetModuleFileName(hInstDLL,szPluginPath,MAX_PATH);
		*(strrchr(szPluginPath,'\\')+1)=0;
		sprintf(d2hackmap_path0,"%sdll\\d2hackmap.dll",szPluginPath);
		return Install();
	case DLL_THREAD_ATTACH:break;
	case DLL_THREAD_DETACH:break;
	case DLL_PROCESS_DETACH:
		if (!lpReserved)
			Uninstall();
		if (logfp) {fclose(logfp);logfp=NULL;}
		break;
	}
	return TRUE;
}

extern "C" PVOID __stdcall QueryInterface() {
	char filename[MAX_PATH];
	GetModuleFileName(hInstDLL, filename, MAX_PATH);
	LoadLibrary(filename);
	//MessageBox(NULL,filename,"loadhackmap QueryInterface",MB_OK);
	return NULL;
}
extern "C" void __stdcall MainMenuMessage(wchar_t *s,int color) {
	lastMsgColor=color;
	lastMsgTimeoutMs=GetTickCount()+10000;
	wcscpy(lastMsg,s);
}
