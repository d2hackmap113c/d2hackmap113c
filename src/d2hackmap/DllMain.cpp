#include "stdafx.h"
#include "header.h"

HMODULE hInstDLL=NULL;
HANDLE dllHeap=NULL,confHeap=NULL,gameHeap=NULL;
int dwGameWindowId=1;
int fIsSinglePlayer=0,fIsTcpIp=0,fIsRealmServer=0,fIsRealmClient=0;
char szDllPath[MAX_PATH]={0};
char szPluginPath[MAX_PATH]={0};

void patchServer();
void unpatchServer();
void saveDistanceMap();
void UnLoadExtInfo();
void saveMpqTable();
void ReleaseMem() {
	saveDistanceMap();
	saveMpqTable();
	UnLoadExtInfo();
}

static int patchedClient=0,patchedServer=0;
BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call,LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			hInstDLL = hModule;
			DisableThreadLibraryCalls(hInstDLL);
			dllHeap=HeapCreate(0,0,0);
			GetModuleFileName(hInstDLL,szDllPath,MAX_PATH);
			GetModuleFileName(hInstDLL,szPluginPath,MAX_PATH);
			char *end=strrchr(szPluginPath,'\\');
			if (!end) end=strrchr(szPluginPath,'/');
			if (end) {
				end[1]=0;
				if (end-szPluginPath>=3&&_memicmp(end-3,"dll",3)==0) end[-3]=0;
			}
			return 1;
		}
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH: {
			ReleaseMem();
			if (!lpReserved) {
				if (patchedClient) {Uninstall();patchedClient=0;}
				if (patchedServer) {unpatchServer();patchedServer=0;}
				if (gameHeap) {HeapDestroy(gameHeap);gameHeap=NULL;}
				if (confHeap) {HeapDestroy(confHeap);confHeap=NULL;}
				if (dllHeap) {HeapDestroy(dllHeap);dllHeap=NULL;}
			}
			if (logfp&&logfp!=stdout) {fclose(logfp);logfp=NULL;}
			break;
		}
	}
	return TRUE;
}


#define D2LOADER_PLUGIN

#ifdef D2LOADER_PLUGIN

extern "C" __declspec(dllexport) PVOID __stdcall QueryInterface() {//NOTE :- needs to be exported
	//char filename[MAX_PATH];
	//GetModuleFileName(hInstDLL, filename, MAX_PATH);
	//LoadLibrary(filename);
	MessageBox(NULL,"d2hackmap doesn't support QueryInterface","d2hackmap error",MB_OK);
	return NULL;
}

char *reloadStorage=NULL;
int reloadStorageSize=0;
extern "C" __declspec(dllexport) void __cdecl PatchClient(char *path,char *reloadStore,int storeSize) {
	reloadStorage=reloadStore;
	reloadStorageSize=storeSize;
	if (path) strncpy(szPluginPath,path,MAX_PATH);
	if (!patchedClient) {patchedClient=1;Install();}
}
extern int nCurCharTags;
void drawCharTags();
extern "C" __declspec(dllexport) void __stdcall MainMenuFlushFramebuf() {
	if (nCurCharTags) drawCharTags();
}
extern "C" __declspec(dllexport) void __stdcall PatchServer(char *path) {
	if (path) strncpy(szPluginPath,path,MAX_PATH);
	if (!patchedServer) {patchedServer=1;patchServer();}
}

#endif
