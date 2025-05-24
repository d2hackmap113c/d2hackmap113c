#include "stdafx.h"
#include "header.h"
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

ToggleVar tShowTestInfo={TOGGLEVAR_ONOFF,0,-1,	1,"ShowTestInfo"};
World *getSinglePlayerWorld();
int SaveGame() {
	if (fIsRealmClient||!fInGame) return 0;
	World *game=getSinglePlayerWorld();
	if (!game) return 0;
	d2game_SaveGame(game);
	gameMessage("saved game");
	return 1;
}
int DoTest() {
	gameMessage("test");
	return 1;
}

static void __fastcall loadUiImage(char *path,int ret) {
	LOG("LoadUiImage ret=0x%X: %s\n",ret,path);
}
static void __fastcall loadUiImageEnd(int pCell) {
	LOG("LoadUiImage pCell=0x%X\n",pCell);
}
// 6FADB420 - 8D 94 24 FCFEFFFF     - lea edx,[esp-00000104]
static void __declspec(naked) LoadUiImagePatch_ASM() {
	__asm {
		pushad
		mov ecx,eax
		mov edx,dword ptr [esp+0x24]
		call loadUiImage
		popad
		lea edx, [esp-0x100]
		ret
	}
}
//6FADB4B0                          - 81 C4 08010000        - add esp,00000108 { 264 }
static void __declspec(naked) LoadUiImageEndPatch_ASM() {
	__asm {
		pushad
		mov ecx,eax
		call loadUiImageEnd
		popad
		pop ecx
		add esp,0x108
		jmp ecx
	}
}
static void __fastcall loadUiImageSubpath(char *subpath,int ret) {
	LOG("LoadUiImageSubpath ret=0x%X: %s\n",ret,subpath);
}
//6FB6F6C0 - 81 EC 04010000        - sub esp,00000104 { 260 }
static void __declspec(naked) LoadUiImageSubpathPatch_ASM() {
	__asm {
		pushad
		mov ecx,esi
		mov edx,dword ptr [esp+0x24]
		call loadUiImageSubpath
		popad
		pop eax
		sub esp,0x104
		jmp eax
	}
}
static void __fastcall allocMemDbg(int *esp,int size,int arg0) {
	int ret=esp[0];
	char *src=(char *)esp[1];
	int line=esp[2],arg4=esp[3];
	if (!src) src="(null)";
	LOG("allocMem ret=0x%X: 0x%X %d %s %d %d\n",ret,arg0,size,src,line,arg4);
}
/*
6FF6CD50 - 53                    - push ebx
6FF6CD51 - 8B 5C 24 08           - mov ebx,[esp+08]
6FF6CD55 - 56                    - push esi
6FF6CD56 - 8B C2                 - mov eax,edx
*/
static void __declspec(naked) AllocMemoryPatch_ASM() {
	__asm {
		pushad
		push ecx
		lea ecx,dword ptr [esp+0x28]
		call allocMemDbg
		popad
		mov eax, dword ptr [esp]
		mov dword ptr [esp], ebx
		mov ebx, dword ptr [esp+8]
		jmp eax
	}
}
static unsigned short *playerQuest=NULL;
unsigned short *getQuest(World *context,UnitAny *pUnit);
static void __fastcall recvPacket40(World *world, UnitAny* pUnit) {
	//LOG("unit client=%d world=%X\n",pUnit->clientId,pUnit->world);
	//LOG("world=%X net=%X id=%d\n",world,world->client,world->client->clientID);
	playerQuest=getQuest(world,pUnit);
	//LOG("playerQuest=0x%X\n",playerQuest);
}
// 00A8BD30 - 83 7C 24 08 01        - cmp dword ptr [esp+08],01 { 1 }
void __declspec(naked) ServerRecvPacket40Patch_ASM() {
	__asm {
		pushad
		call recvPacket40 
		popad
		cmp dword ptr [esp+0xC],1
		ret
	}
}
void __fastcall setQuestFlagPatch(unsigned short **quest,int id,int flag,int ret) {
	if (*quest!=playerQuest) return;
	gameMessage("setQuest %d %d ret=0x%X",id,flag,ret);
}
void __fastcall clearQuestFlagPatch(unsigned short **quest,int id,int flag,int ret) {
	if (*quest!=playerQuest) return;
	gameMessage("clearQuest %d %d ret=0x%X",id,flag,ret);
}
//D2FUNCNAME(d2common,0x6FD7D0B0,10003,clearQuestFlag,void,__stdcall,(short **questData,int questId,int pos))
//6FD7D0B0 - 8B 44 24 04           - mov eax,[esp+04]
//6FD7D0B4 - 85 C0                 - test eax,eax
void __declspec(naked) SetQuestFlagPatch_ASM() {
	__asm {
		pushad
		mov ecx,[esp+0x24]
		push ecx
		mov ecx,[esp+0x34]
		push ecx
		mov ecx,[esp+0x30]
		mov edx,[esp+0x34]
		call setQuestFlagPatch 
		popad
		mov eax,[esp+8]
		test eax,eax
		ret
	}
}
//6FD7D070 - 8B 44 24 04           - mov eax,[esp+04]
//6FD7D074 - 85 C0                 - test eax,eax
void __declspec(naked) ClearQuestFlagPatch_ASM() {
	__asm {
		pushad
		mov ecx,[esp+0x24]
		push ecx
		mov ecx,[esp+0x34]
		push ecx
		mov ecx,[esp+0x30]
		mov edx,[esp+0x34]
		call clearQuestFlagPatch 
		popad
		mov eax,[esp+8]
		test eax,eax
		ret
	}
}
static void __fastcall sendPacket43E0(char *packet,int len,int ret) {
	LOG("SendPacket43E0: ret=0x%X cmd=%02X len=%d\n",ret,packet[0],len);
}
/*
ebx=len arg0=packet
can use: eax,ecx,edx
6FAC43E0 - 55                    - push ebp
6FAC43E1 - 8B EC                 - mov ebp,esp
6FAC43E3 - 83 E4 F8              - and esp,-08 { 248 }
6FAC43E6 - 81 EC 08020000        - sub esp,00000208 { 520 }
6FAC43EC - 81 FB 00020000        - cmp ebx,00000200 { 512 } <----------
*/
static void __declspec(naked) sendPacket43E0Patch_ASM() {
	__asm {
		pushad
		mov ecx, [ebp+4]
		push ecx
		mov ecx, [ebp+8]
		mov edx, ebx
		call sendPacket43E0
		popad
		cmp ebx,0x200
		ret
	}
}
static void __fastcall sendPacket3(char cmd, short param,int ret) {
	LOG("SendPacket3: ret=%X cmd=%02X param=%04X\n",ret,cmd,param);
}
/*
6FAC4A10 - 51                    - push ecx
6FAC4A11 - 53                    - push ebx
6FAC4A12 - 8D 54 24 04           - lea edx,[esp+04]
6FAC4A16 - 52                    - push edx
6FAC4A17 - BB 03000000           - mov ebx,00000003 { 3 } 
*/
static void __declspec(naked) sendPacket3Patch_ASM() {
	__asm {
		pushad
		mov ebx, [esp+0x30]
		push ebx
		mov edx, ecx
		mov ecx, eax
		call sendPacket3
		popad
		mov ebx,3
		ret
	}
}
static void __fastcall clickMenuItem(int addr) {
	LOG("click menu item %X\n",addr);
}
/*
	d2client_A3CFB: 8B 84 32 70 01 00 00  mov eax, [edx+esi+0x170]
	d2client_A3D02: 85 C0              test eax, eax
	d2client_A3D04: 74 06              jz d2client_A3D0C ->+6 BA3D0C
	d2client_A3D06: FF D0              call eax
*/
static void __declspec(naked) clickMenuItemPatch_ASM() {
	__asm {
		mov eax, [edx+esi+0x170]
		pushad
		mov ecx,eax
		call clickMenuItem
		popad
		ret
	}
}
static int drawCellLoopId=0,cellId=0;
void hex(FILE *fp,int addr,void *buf1,int n);
static void __fastcall drawCellFilePatch(CellContext *context,int x,int y) {
	char path[128];
	if (!logfp) return;
	if (drawCellLoopId&&drawCellLoopId!=dwLoopCount) return;drawCellLoopId=dwLoopCount;
	int esp=(int)&y;
	esp+=0x38;
	int ret=*(int *)esp;
	if (ret!=0x6FB1B6B7) return;
	fprintf(logfp,"drawCellFile: context=%X (%d,%d)",context,x,y);
	if (context) {
		fprintf(logfp," cellNo=%d dir=%d,%d",context->dwCellNo,context->dwDirNo,context->dwDirection);
		fprintf(logfp," cellfile=%X",context->pCellFile);
		if (context->pCellFile) {
			fprintf(logfp," nDir=%d nCells=%d",context->pCellFile->dwNumdirs,context->pCellFile->dwNumCells);
			fprintf(logfp," cell[0]=%X",context->pCellFile->pCells[0]);
			if (context->pCellFile->pCells[0]) {
				_snprintf(path,128,"/t/dc6/%03d.bmp",cellId++);
				dc6cell2bmp(context->pCellFile->pCells[0],path);
			}
		}
	}
	fprintf(logfp," ret=%X",ret);
	for (int i=0;i<10;i++) {
		esp+=4;
		fprintf(logfp," %X",*(int *)esp);
	}
	fputc('\n',logfp);
	/*
	hex(logfp,0,context,0x48);
	if (context->uk_2C) hex(logfp,0,context->uk_2C,0x48);
	if (context->uk_3C) hex(logfp,0,context->uk_3C,0x48);
	*/
	fflush(logfp);
}
/*
function void __stdcall d2gfx_DrawCellFile(CellContext *context, int xPos, int yPos, DWORD dw1, int dwTransLvl, BYTE *coltab) {
	d2gfx_B080: mov eax, [esp+0x18(arg5:*coltab)] ; esp+0
	d2gfx_B084: mov ecx, [esp+0x14(arg4:dwTransLvl)]
	d2gfx_B088: mov edx, [esp+0x10(arg3:dw1)]
	d2gfx_B08C: push eax(arg5:coltab)
	d2gfx_B08D: mov eax, [esp+0x10(arg2:yPos)] ; esp-4
	d2gfx_B091: push ecx(arg4:dwTransLvl)
	d2gfx_B092: mov ecx, [esp+0xC(arg0:CellContext *context)] ; esp-8
	d2gfx_B096: push edx(arg3:dw1)
	d2gfx_B097: mov edx, [esp+0x14(arg1:xPos)] ; esp-12
	d2gfx_B09B: push eax(arg2:yPos)
	d2gfx_B09C: A1 68 12 A9 6F     mov eax, [d2gfx_11268 void *d2gfx_callbackTable](->0x6F87B908(d2gdi_B908)
	d2gfx_B0A1: call dword ptr [eax(d2gfx_callbackTable=0x6F87B908(d2gdi_B908)))+0x84 =6F87B98C d2gdi_B98C](=0x6F8765E0(d2gdi_65E0))
*/
static void __declspec(naked) drawCellFilePatch_ASM() {
	__asm {
		pushad
		mov eax, [esp+0x24]
		push eax
		call drawCellFilePatch
		popad
		mov eax,d2gfx_pcallbackTable
		mov eax,[eax]
		ret
	}
}
int installAlwaysDebugPatches() {
	return 1;
}
static void notused() {
	//function d2common/Ordinal10003 {
	//d2common_2D0B0: 8B 44 24 04        mov eax, [esp+0x4]
	//d2common_2D0B4: 85 C0              test eax, eax
	PatchCall(d2common,0x2D0B0,SetQuestFlagPatch_ASM,6,"8B 44 24 04 85 C0");
	//function d2common/Ordinal10593 {
	//d2common_2D070: 8B 44 24 04        mov eax, [esp+0x4]
	//d2common_2D074: 85 C0              test eax, eax
	PatchCall(d2common,0x2D070,ClearQuestFlagPatch_ASM,6,"8B 44 24 04 85 C0");
	//d2game_CBD30: 83 7C 24 08 01     cmp dword ptr [esp+0x8], 1
	PatchCall(d2game,0xCBD30,ServerRecvPacket40Patch_ASM,5,"83 7C 24 08 01");

	//d2client_2B420: 8D 94 24 FC FE FF FF  lea edx, [esp-0x104]
	PatchCall(d2client,0x2B420,LoadUiImagePatch_ASM,7,"8D 94 24 FC FE FF FF");
	//d2client_2B4B0: 81 C4 08 01 00 00  add esp, 0x108 (264)
	PatchCall(d2client,0x2B4B0,LoadUiImageEndPatch_ASM,6,"81 C4 08 01 00 00");
	//d2client_BF6C0: 81 EC 04 01 00 00  sub esp, 0x104 (260)
	PatchCall(d2client,0xBF6C0,LoadUiImageSubpathPatch_ASM,6,"81 EC 04 01 00 00");
		
	//function fog/Ordinal10045 {
	//fog_1CD50: 53                 push ebx
	//fog_1CD51: 8B 5C 24 08        mov ebx, [esp+0x8]
	PatchCall(fog,0x1CD50,AllocMemoryPatch_ASM,5,"53 8B 5C 24 08");
}
int installDebugPatches() {
	//d2client_14A17: BB 03 00 00 00     mov ebx, 3
	PatchCall(d2client,0x14A17,sendPacket3Patch_ASM,5,"BB 03 00 00 00");
	//d2client_143EC: 81 FB 00 02 00 00  cmp ebx, 0x200 (512)
	PatchCall(d2client,0x143EC,sendPacket43E0Patch_ASM,6,"81 FB 00 02 00 00");
	//d2client_A3CFB: 8B 84 32 70 01 00 00  mov eax, [edx+esi+0x170]
	PatchCall(d2client,0xA3CFB,clickMenuItemPatch_ASM,7,"8B 84 32 70 01 00 00");
	drawCellLoopId=0;cellId=0;
	//d2gfx_B09C: A1 68 12 A9 6F     mov eax, [d2gfx_11268 void *d2gfx_callbackTable](->0x6F87B908(d2gdi_B908)
	//PatchCall(d2gfx,0xB09C,drawCellFilePatch_ASM,5,"A1 $(+11268)");
	return 1;
}
