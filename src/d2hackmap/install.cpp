#include "stdafx.h"
#include <sys/stat.h>
#include "header.h"

#define DECLARE_D2PTR_VARIBLE
#include "d2ptrs.h"
#undef DECLARE_D2PTR_VARIBLE

static int debug=0;
char *bnetIp=NULL;
int LoadConfigFile(char *path,int isServer);
void InitCellFiles();
void DeleteCellFiles();
void RemoveMyAutomapNodes();
void ShowWarningMessage();
int installServerPatches();
int installPanelPatches();
int uninstallBigstashPatches();

int dwGameLng=-1;
extern int dwDumpMpqPath;
extern ToggleVar tHighResolution;
static int ReloadDll();
static int ReloadConfig();
static int installPatchToggle();
static void InstallPatchAfterLoad();
void resolution_init();
void resolution_uninit();
static int patchCount=0;
int load_dll_error=0;
int fLocalizationSupport=0;

HMODULE addr_storm=(HMODULE)0,addr_d2client=(HMODULE)0;
HMODULE addr_d2common=(HMODULE)0,addr_d2gfx=(HMODULE)0;
HMODULE addr_d2win=(HMODULE)0,addr_d2lang=(HMODULE)0;
HMODULE addr_d2cmp=(HMODULE)0,addr_bnclient=(HMODULE)0;
HMODULE addr_d2net=(HMODULE)0,addr_fog=(HMODULE)0;
HMODULE addr_d2game=(HMODULE)0,addr_d2launch=(HMODULE)0;
HMODULE addr_d2gdi=(HMODULE)0,addr_d2sound=(HMODULE)0;
HMODULE addr_d2multi=(HMODULE)0,addr_d2mcpclient=(HMODULE)0;

int fGameFilter=0;
int	fFixSocketError=1;
ToggleVar tReloadDll={TOGGLEVAR_DOWN,0,-1,1,"ReloadDll",&ReloadDll};
ToggleVar tReloadConfig={TOGGLEVAR_DOWN,0,-1,1,"ReloadConfig",&ReloadConfig};
ToggleVar tInstallPatches={TOGGLEVAR_ONOFF,1,-1,1,"InstallPatches",&installPatchToggle};
int hasPlugY=0;
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY, "ReloadConfigKey",			&tReloadConfig     },
	{CONFIG_VAR_TYPE_KEY, "ReloadDllKey",			&tReloadDll     },
	{CONFIG_VAR_TYPE_KEY, "InstallPatchesToggle",			&tInstallPatches     },
	{CONFIG_VAR_TYPE_INT, "GameFilterSupport",     &fGameFilter,  4},
	{CONFIG_VAR_TYPE_INT, "FixSocketError",	   &fFixSocketError,4},
	{CONFIG_VAR_TYPE_INT, "LocalizationSupport",      &fLocalizationSupport,  4},
};
void Install_AddConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void Install_FixValues() {
	tReloadConfig.keyType=2;
	tReloadDll.keyType=2;
}

HMODULE get_dll_addr(char *name) {
	HMODULE hmod = GetModuleHandle(name);
	if (!hmod) hmod = LoadLibrary(name);
	if (!hmod) {
		char buf[256];sprintf(buf,"load %s error",name);
		MessageBox(NULL,buf,"d2hackmap",MB_OK);
		load_dll_error++;
	}
	return hmod;
}
DWORD VirtualProtect(DWORD pAddress, DWORD len, DWORD prot) {
	DWORD oldprot = 0;VirtualProtect((void *)pAddress, len, prot, &oldprot);return oldprot;
}
void WriteLocalBYTES(DWORD pAddress, void *buf, int len) {
	DWORD oldprot = VirtualProtect(pAddress, len, PAGE_EXECUTE_READWRITE);
	WriteProcessMemory(GetCurrentProcess(), (void *)pAddress, buf, len, 0);
	VirtualProtect(pAddress, len, oldprot);
}
void FillLocalBYTES(DWORD pAddress, BYTE ch, int len) {
	BYTE *buf1 = new BYTE[len];
	memset(buf1, ch, len);
	WriteLocalBYTES(pAddress, buf1, len);
	delete buf1;
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
static void doPatchCALL(int addr, int param, int len) {InterceptLocalCode(INST_CALL, addr, param, len);}
static void doPatchJMP(int addr, int param, int len) {InterceptLocalCode(INST_JMP, addr, param, len);}
static void doPatchFILL(int addr, int param, int len) {FillLocalBYTES(addr, (BYTE)param, len);}
static void doPatchVALUE(int addr, int param, int len) {WriteLocalBYTES(addr, &param, len);}
InstalledPatch **pPatchList=NULL;
static InstalledPatch *keyPatches=NULL,*installedPatches=NULL,*resolutionPatches=NULL;
struct InstalledPatch *networkPatch=NULL;
int patchCall(char *name,char *dll,int offset,int addr,int param,int len,
	char *original) {
	return installPatch(doPatchCALL,name,dll,offset,addr,param,len,original);
}
int patchJmp(char *name,char *dll,int offset,int addr,int param,int len,
	char *original) {
	return installPatch(doPatchJMP,name,dll,offset,addr,param,len,original);
}
int patchFill(char *name,char *dll,int offset,int addr,int param,int len,
	char *original) {
	return installPatch(doPatchFILL,name,dll,offset,addr,param,len,original);
}
int patchValue(char *name,char *dll,int offset,int addr,int param,int len,
	char *original) {
	return installPatch(doPatchVALUE,name,dll,offset,addr,param,len,original);
}
static int parseOriginalCode(char *buf,int size,char *s,int dllbase,int *pfix) {
	char *s0=s;int len=0;
	*pfix=0;
	if (*s=='-') *pfix=strtol(s,&s,0);
	while (size>0) {
		while (*s==' ') s++;if (!*s) break;
		if (*s=='$') {
			s++;if (*s!='(') goto bad_format;
			s++;if (*s!='+') goto bad_format;
			int off=strtol(s,&s,16);
			if (*s++!=')') goto bad_format;
			if (size<4) break;
			*(int *)buf=dllbase+off;buf+=4;size-=4;len+=4;
		} else {
			*buf=(char)strtol(s,&s,16);buf++;size--;len++;
		}
	}
	return len;
bad_format:	
	LOG("Bad original code format: %s\n",s0);return 0;
}
int installPatch(void (*func)(int, int, int),char *name,
	char *dll,int offset,int addr,int param,int len,char *original) {
	if (!func||!pPatchList) return 0;
	if (original&&len<=256) {
		char code[256];int fix;
		int n=parseOriginalCode(code,256,original,addr-offset,&fix);
		int caddr=addr+fix;
		if (n&&memcmp((void *)caddr,code,n)!=0) {
			LOG("Warning: patch at %s_%X %s len=%d mismatch\n",
				dll,offset,name,len);
			if (logfp) {
				hex(logfp,caddr,(void *)caddr,len);
				hex(logfp,0,code,n);
			}
			return 0;
		}
	}
	InstalledPatch *p=(InstalledPatch *)malloc(sizeof(InstalledPatch)+len);
	p->addr=addr;p->len=len;p->next=*pPatchList;
	memcpy(p->oldcode,(void *)addr,len);
	if (debug&&logfp) {
		LOG("Install Patch %s_%X %X %s %d\n",dll,offset,addr,name,len);
	}
	func(addr,param,len);
	*pPatchList=p;patchCount++;
	return 1;
}
int removePatches(InstalledPatch **phead) {
	int n=0;
	while (*phead) {
		InstalledPatch *p=*phead;if (p->oldcode) WriteLocalBYTES(p->addr,p->oldcode,p->len);*phead=p->next;free(p);
		n++;
	}
	return n;
}

#include "patch.h"
static int installNetworkRecvPatch() {
	pPatchList=&networkPatch;
	if (load_dll_error) return 0;
//d2client_439A8: E8 73 94 06 00     call d2client_ACE20
PatchCall(d2client,0x439A8,GamePacketReceivedInterceptPatch_ASM,5,"E8 73 94 06 00");
	return 1;
}
int installAlwaysDebugPatches();
static int installD2Patches() {
	if (load_dll_error) return 0;
	if (!keyPatches) {
		pPatchList=&keyPatches;
//d2win_17740: A1 C0 14 90 6F     mov eax, [d2win_214C0](->1)
PatchCall(d2win,0x17740,WinMessagePatch_ASM,5,"A1 $(+214C0)");
//d2client_38BB5: F7 47 0C 00 00 00 40  test dword ptr [edi+0xC], 0x40000000
PatchCall(d2client,0x38BB5,KeydownPatch_ASM,7,"F7 47 0C 00 00 00 40");
//d2client_38AE1: 0F B7 41 04        movzx eax, word ptr [ecx+0x4]
//d2client_38AE5: 3B C6              cmp eax, esi
PatchCall(d2client,0x38AE1,KeyupPatch_ASM,6,"0F B7 41 04 3B C6");
//d2win_187E3: E8 10 F0 FE FF     call d2win_77F8->d2gfx_88B0 d2gfx/Ordinal10043
PatchCall(d2win,0x187E3,SkipDrawMenuPatch_ASM,5,"E8 10 F0 FE FF");
//d2win_1893D: 8B 35 A0 14 90 6F  mov esi, [d2win_214A0]
PatchCall(d2win,0x1893D,SkipDrawMenuPatch2_ASM,6,"8B 35 $(+214A0)");
//d2client_44361: 83 3D 14 C4 BC 6F 03  cmp dword ptr [d2client_11C414 int d2client_ScreenBlocked](->0), 3<--- install here
PatchCall(d2client,0x44361,SkipDrawUnitAndControlPatch_ASM,7,"83 3D $(+11C414) 03");
//d2client_4423C: E8 61 8E FC FF     call d2client_D0A2->d2gfx_88B0 d2gfx/Ordinal10043
PatchCall(d2client,0x4423C,SkipAllDrawingPatch_ASM,5,"E8 61 8E FC FF");
//d2common_2F383: BA F4 00 00 00     mov edx, 0xF4 (244)
patchValue("unit size 0xF4->0xFC",PATCH_ADDR(d2common,0x2F384),0xFC,1,"-1 BA F4 00 00 00");
//d2common_2F393: B9 3D 00 00 00     mov ecx, 0x3D (61)
patchValue("zero unit 0xF4->0xFC",PATCH_ADDR(d2common,0x2F394),0x3F,1,"-1 B9 3D 00 00 00");
	}
	if (installedPatches) return 1;
	pPatchList=&installedPatches;
if (dwDumpMpqPath) 
//storm_28964: 81 EC 10 01 00 00  sub esp, 0x110 (272)
PatchCall(storm,0x28964,storm268_patch_ASM,6,"81 EC 10 01 00 00");
installAlwaysDebugPatches();
//--- m_pub.h ---
//d2client_44230: 83 EC 20           sub esp, 0x20 (32)
//d2client_44233: 89 4C 24 0C        mov [esp+0xC], ecx
PatchCall(d2client,0x44230,GameLoopPatch_ASM,7,"83 EC 20 89 4C 24 0C");
//d2client_45153: B9 B4 00 00 00     mov ecx, 0xB4 (180)
PatchCall(d2client,0x45153,GameEndPatch_ASM,5,"B9 B4 00 00 00");
//d2client_1AB88: 74 06              jz d2client_1AB90 ->+6 B1AB90
patchValue("Fast enter portal",PATCH_ADDR(d2client,0x1AB88),0xEB,1,"74 06");
	if (addr_d2multi&&addr_d2mcpclient) InstallPatchAfterLoad();
	else 
//d2win_1881F: 8D 44 24 04        lea eax, [esp+0x4]
//d2win_18823: 50                 push eax
PatchCall(d2win,0x1881F,InstallPatchAfterLoad_ASM,5,"8D 44 24 04 50");
//--- login ---
//d2launch_19539: E8 16 02 FF FF     call d2launch_9754->storm_3C2B0
PatchCall(d2launch,0x19539,LastBNet_Patch,5,"E8 16 02 FF FF");
//d2launch_E1B1: A3 B0 4D A6 6F     mov [d2launch_24DB0](->0), eax
PatchCall(d2launch,0xE1B1,SelectCharPatch_ASM,5,"A3 $(+24DB0)");
//d2launch_CBF6: 0F B7 57 01        movzx edx, word ptr [edi+0x1]
//d2launch_CBFA: 3B EA              cmp ebp, edx
PatchCall(d2launch,0xCBF6,OpenCharMenuPatch_ASM,6,"0F B7 57 01 3B EA");
//--- winmsg.h ---
//d2launch_1805B: 68 F0 11 A6 6F     push dword 0x6FA611F0(d2launch_211F0)(->6)
PatchCall(d2launch,0x1805B,ShowVersion_Patch_ASM,5,"68 $(+211F0)");
//d2client_433D1: C7 05 F0 C3 BC 6F 00 00 00 00  mov dword ptr [d2client_11C3F0](->0), 0
PatchCall(d2client,0x433D1,StandStillPatch_ASM,10,"C7 05 $(+11C3F0) 00 00 00 00");
//d2client_ABF86: 8B 0D 94 BC BC 6F  mov ecx, [d2client_11BC94](->0)
PatchCall(d2client,0xABF86,ReceiveSwapWeapon_ASM,6,"8B 0D $(+11BC94)");
//d2client_1621D: FF 15 B4 F0 B7 6F  call dword ptr [d2client_CF0B4](->user32_461B3 user32/SetCursorPos)
PatchCall(d2client,0x1621D,SetCursorPosPatch_ASM,6,"FF 15 $(+CF0B4)");
//d2client_271C0: 83 EC 48           sub esp, 0x48 (72)
//d2client_271C3: 57                 push edi
//d2client_271C4: 33 C0              xor eax, eax
PatchCall(d2client,0x271C0,DrawLeftBackImagePatch_ASM,6,"83 EC 48 57 33 C0");
//d2client_270D6: 83 EC 48           sub esp, 0x48 (72)
//d2client_270D9: 85 D2              test edx, edx
PatchCall(d2client,0x270D6,DrawRightBackImagePatch_ASM,5,"83 EC 48 85 D2");
//d2client_4436F: E8 9C 6D 04 00     call d2client_8B110
PatchCall(d2client,0x4436F,DrawGroundAndUnitPatch_ASM,5,"E8 9C 6D 04 00");
//d2client_4437B: E8 60 F6 07 00     call d2client_C39E0
PatchCall(d2client,0x4437B,DrawControlsPatch_ASM,5,"E8 60 F6 07 00");
//--- m_PacketHandler.h ---
//function d2net/Ordinal10024 {
//d2net_7650: A1 44 B2 96 00     mov eax, [d2net_B244](->2B5474C(->0))
PatchCall(d2net,0x7650,GamePacketSendInterceptPatch_ASM,5,"A1 $(+B244)");
//d2client_4A2A1: E8 AA BE FF FF     call d2client_46150
PatchCall(d2client,0x4A2A1,ResetSkillPatch_ASM,5,"E8 AA BE FF FF");
//--- m_DropProtection.h ---
//d2client_49440: 64 A1 00 00 00 00  mov eax, [fs:0]
PatchCall(d2client,0x49440,ClickHireMercMenuItemPatch_ASM,6,"64 A1 00 00 00 00");
//d2launch_F3D0: BA 04 01 00 00     mov edx, 0x104 (260)
PatchCall(d2launch,0xF3D0,DeleteCharacter_Patch_ASM,5,"BA 04 01 00 00");
//d2launch_DB9F: 8D 96 00 01 00 00  lea edx, [esi+0x100]
PatchCall(d2launch,0xDB9F,DeleteSelectedCharacter_Patch_ASM,6,"8D 96 00 01 00 00");
//d2win_F4E6: 8B 33              mov esi, [ebx]
//d2win_F4E8: 83 3E 06           cmp dword ptr [esi], 6
PatchCall(d2win,0xF4E6,ClickMenuItem_Patch_ASM,5,"8B 33 83 3E 06");
//--- map.cpp ---
//d2client_61354: 8B 14 85 B8 55 BC 6F  mov edx, [eax*4+d2client_1155B8]
PatchCall(d2client,0x61354,OverrideShrinePatch_ASM,7,"8B 14 85 $(+1155B8)");
//d2client_61D83: 8B 8E BC 01 00 00  mov ecx, [esi+0x1BC]
PatchCall(d2client,0x61D83,AddShrinePatch_ASM,6,"8B 8E BC 01 00 00");
//d2client_61D31: 0F 84 21 01 00 00  jz d2client_61E58 ->+289 B61E58
patchFill("shrinedistance",PATCH_ADDR(d2client,0x61D31),INST_NOP,6,"0F 84 21 01 00 00");
//d2client_604EA: E8 73 CC FA FF     call d2client_D162->d2gfx_AFF0 d2gfx/Ordinal10079
PatchCall(d2client,0x604EA,DrawAutomapNodePatch,5,"E8 73 CC FA FF");
//d2client_60E16: 8B 0D C4 C1 BC 6F  mov ecx, [d2client_11C1C4](->2689E20(->0))
PatchCall(d2client,0x60E16,MinimapPatch_ASM,6,"8B 0D $(+11C1C4)");
//d2client_AF580: E8 CB B1 F7 FF     call d2client_2A750(2 args)
PatchCall(d2client,0xAF580,RecvPacket0EPatch_ASM,5,"E8 CB B1 F7 FF");
//--- m_AutoMapBlob.h ---
//d2client_61AC4: 0F 85 20 02 00 00  jnz d2client_61CEA ->+544 B61CEA
PatchCall(d2client,0x61AC4,ForceDrawBlob_ASM,6,"0F 85 20 02 00 00");
//d2client_5F1C0: 53                 push ebx
//d2client_5F1C1: 55                 push ebp
//d2client_5F1C2: 8B 6C 24 0C        mov ebp, [esp+0xC]
PatchJmp(d2client,0x5F1C0,DrawBlobPath_ASM,6,"53 55 8B 6C 24 0C");
//d2client_61688: 33 C0              xor eax, eax
//d2client_6168A: 5E                 pop esi
//d2client_6168B: 83 C4 0C           add esp, 0xC (12)
PatchJmp(d2client,0x61688,MixedBlobColPatch_ASM,6,"33 C0 5E 83 C4 0C");
//d2client_60BD8: 68 FF 00 00 00     push dword 0xFF (255)
PatchCall(d2client,0x60BD8,DrawPartyPlayerBlobPatch_ASM,5,"68 FF 00 00 00");
//d2client_6175D: 8A 0D 11 C2 BC 6F  mov cl, [d2client_11C211](->CB849B62)
PatchCall(d2client,0x6175D,HostilePlayerColor_ASM,6,"8A 0D $(+11C211)");
//d2client_61CD3: 75 04              jnz d2client_61CD9 ->+4 B61CD9
//d2client_61CD5: 6A 02              push 2
//d2client_61CD7: EB 02              jmp d2client_61CDB ->+2 B61CDB
//d2client_61CD9: 6A 01              push 1
//d2client_61CDB: 53                 push ebx
//d2client_61CDC: 57                 push edi
//d2client_61CDD: 56                 push esi
PatchCall(d2client,0x61CD3,DrawPlayerTextPatch_ASM,11,"75 04 6A 02 EB 02 6A 01 53 57 56");
//d2client_61B67: BA 09 00 00 00     mov edx, 9
PatchCall(d2client,0x61B67,MonsterBlobDescPatch_ASM,5,"BA 09 00 00 00");
//--- env.cpp ---
//d2client_625C4: 75 08              jnz d2client_625CE ->+8 B625CE
patchFill("floorsinside",PATCH_ADDR(d2client,0x625C4),INST_NOP,2,"75 08");
//d2client_6260A: 75 28              jnz d2client_62634 ->+40 B62634
patchFill("wallsinside",PATCH_ADDR(d2client,0x6260A),INST_NOP,2,"75 28");
//d2client_A9A37: 56                 push esi
//d2client_A9A38: E8 A1 27 F6 FF     call d2client_C1DE->d2common_3C000 d2common/Ordinal10826
PatchCall(d2client,0xA9A37,LightingPatch_ASM,6,"56 E8 A1 27 F6 FF");
//d2common_6CCB6: C2 04 00           ret 4
//d2common_6CCB9: 32 C0              xor al, al
PatchJmp(d2common,0x6CCB6,WeatherPatch_ASM,5,"C2 04 00 32 C0");
//d2client_442A2: E8 29 6D 04 00     call d2client_8AFD0
PatchCall(d2client,0x442A2,ShakeScreenPatch,5,"E8 29 6D 04 00");
//d2client_66625: 8B 06              mov eax, [esi]
//d2client_66627: 83 F8 04           cmp eax, 4
PatchCall(d2client,0x66625,InfravisionPatch_ASM,5,"8B 06 83 F8 04");
//d2client_8B235: E8 16 8E F8 FF     call d2client_14050 int __stdcall d2client_drawGround(void *arg1)(1 args)
PatchCall(d2client,0x8B235,SkipDrawGroundPatch_ASM,5,"E8 16 8E F8 FF");
//--- m_MonsterDeath.h ---
//d2client_20FBE: 3D 00 01 00 00     cmp eax, 0x100 (256)
PatchCall(d2client,0x20FBE,MonsterDeathPatch_ASM,5,"3D 00 01 00 00");
//--- m_ScrollLock.h ---
//d2client_8B16F: 74 34              jz d2client_8B1A5 ->+52 B8B1A5
patchFill("forceperspectiveshake",PATCH_ADDR(d2client,0x8B16F),INST_NOP,2,"74 34");
//d2client_3F8DF: 74 1B              jz d2client_3F8FC ->+27 B3F8FC
patchFill("forcegetshake",PATCH_ADDR(d2client,0x3F8DF),INST_NOP,2,"74 1B");
//d2client_8B096: 89 31              mov [ecx], esi
//d2client_8B098: 8B 4C 24 18        mov ecx, [esp+0x18]
//d2client_8B09C: 8B 11              mov edx, [ecx]
//d2client_8B09E: 03 D0              add edx, eax
//d2client_8B0A0: A3 DC B9 BB 6F     mov [d2client_10B9DC](->0), eax
patchFill("killaddshake",PATCH_ADDR(d2client,0x8B096),INST_NOP,15,"89 31 8B 4C 24 18 8B 11 03 D0 A3 $(+10B9DC)");
//d2client_8B0A5: 89 11              mov [ecx], edx
patchFill("killaddshake,part2",PATCH_ADDR(d2client,0x8B0A5),INST_NOP,2,"89 11");
//d2client_5FB10: 83 05 E8 C1 BC 6F 10  add dword ptr [d2client_11C1E8](->0), 0x10 (16)
//d2client_5FB17: C2 04 00           ret 4
//d2client_5FB1A: CC                 int 3 
//d2client_5FB1B: CC                 int 3 
//d2client_5FB1C: CC                 int 3 
//d2client_5FB1D: CC                 int 3 
//d2client_5FB1E: CC                 int 3 
//d2client_5FB1F: CC                 int 3 
//d2client_5FB20: 83 2D E8 C1 BC 6F 10  sub dword ptr [d2client_11C1E8](->0), 0x10 (16)
//d2client_5FB27: C2 04 00           ret 4
//d2client_5FB2A: CC                 int 3 
//d2client_5FB2B: CC                 int 3 
//d2client_5FB2C: CC                 int 3 
//d2client_5FB2D: CC                 int 3 
//d2client_5FB2E: CC                 int 3 
//d2client_5FB2F: CC                 int 3 
//d2client_5FB30: 83 2D EC C1 BC 6F 10  sub dword ptr [d2client_11C1EC](->0), 0x10 (16)
//d2client_5FB37: C2 04 00           ret 4
//d2client_5FB3A: CC                 int 3 
//d2client_5FB3B: CC                 int 3 
//d2client_5FB3C: CC                 int 3 
//d2client_5FB3D: CC                 int 3 
//d2client_5FB3E: CC                 int 3 
//d2client_5FB3F: CC                 int 3 
//d2client_5FB40: 83 05 EC C1 BC 6F 10  add dword ptr [d2client_11C1EC](->0), 0x10 (16)
patchFill("killautomapscroll,wipeout4adjustments",PATCH_ADDR(d2client,0x5FB10),INST_NOP,55,"83 05 $(+11C1E8) 10 C2 04 00 CC CC CC CC CC CC 83 2D $(+11C1E8) 10 C2 04 00 CC CC CC CC CC CC 83 2D $(+11C1EC) 10 C2 04 00 CC CC CC CC CC CC 83 05 $(+11C1EC) 10");
//--- m_QuickBackTown.h ---
//d2client_AD2F0: 83 EC 08           sub esp, 8
//d2client_AD2F3: 8B 01              mov eax, [ecx]
PatchCall(d2client,0xAD2F0,RecvCommand_60_Patch_ASM,5,"83 EC 08 8B 01");
//--- m_PartyHelp.h ---
//d2client_AD6B0: 83 EC 28           sub esp, 0x28 (40)
//d2client_AD6B3: 56                 push esi
//d2client_AD6B4: 57                 push edi
PatchCall(d2client,0xAD6B0,RecvCommand_5A_Patch_ASM,5,"83 EC 28 56 57");
//d2client_AD8F0: 56                 push esi
//d2client_AD8F1: 8D 71 22           lea esi, [ecx+0x22]
//d2client_AD8F4: 8B C6              mov eax, esi
PatchCall(d2client,0xAD8F0,RecvCommand_5B_Patch_ASM,6,"56 8D 71 22 8B C6");
//--- m_OutTownSelect.h ---
//d2client_A6941: C1 E8 15           shr eax, 0x15 (21)
//d2client_A6944: 83 E0 01           and eax, 1
PatchCall(d2client,0xA6941,OutTownSelectPatch_ASM1,6,"C1 E8 15 83 E0 01");
//d2client_1BA6B: 0F 84 9F 00 00 00  jz d2client_1BB10 ->+159 B1BB10
PatchCall(d2client,0x1BA6B,OutTownSelectPatch_ASM2,6,"0F 84 9F 00 00 00");
//--- m_AutoSkill.h ---
//patchCall(PATCH_ADDR(d2client,0x28C97),(DWORD)DrawSkillButton_Patch_ASM,5,0x15944E8,0x74F68500);
//--- m_AutoEnchant.h ---
//d2client_AD740: 8A 51 01           mov dl, [ecx+0x1]
//d2client_AD743: 33 C0              xor eax, eax
PatchCall(d2client,0xAD740,RecvCommand_2C_Patch_ASM,5,"8A 51 01 33 C0");
//d2client_A6505: 8B 86 C4 00 00 00  mov eax, [esi+0xC4]
PatchCall(d2client,0xA6505,AddUnitPatch_ASM,6,"8B 86 C4 00 00 00");
//--- m_AutoSummon.h ---
//d2client_A6F25: 8B 81 E4 00 00 00  mov eax, [ecx+0xE4]
PatchCall(d2client,0xA6F25,RemoveUnitPatch1_ASM,6,"8B 81 E4 00 00 00");
//d2client_A6F30: 8B 91 E4 00 00 00  mov edx, [ecx+0xE4]
PatchCall(d2client,0xA6F30,RemoveUnitPatch2_ASM,6,"8B 91 E4 00 00 00");
//d2client_AF395: 8B D1              mov edx, ecx
//d2client_AF397: 0F B7 48 06        movzx ecx, word ptr [eax+0x6]
PatchCall(d2client,0xAF395,RecvCommand_69_Patch_ASM,6,"8B D1 0F B7 48 06");
//d2client_AC720: 8A 41 01           mov al, [ecx+0x1]
//d2client_AC723: 84 C0              test al, al
PatchCall(d2client,0xAC720,RecvCommand_7A_Patch_ASM,5,"8A 41 01 84 C0");
//d2client_AC911: 8B F1              mov esi, ecx
//d2client_AC913: 0F B6 56 01        movzx edx, byte ptr [esi+0x1]
PatchCall(d2client,0xAC911,RecvCommand_21_Patch_ASM,6,"8B F1 0F B6 56 01");
//--- m_ScreenHook.h ---
//d2client_C3B96: E8 B5 56 F6 FF     call d2client_29250
PatchCall(d2client,0xC3B96,DrawClientPatch_ASM,5,"E8 B5 56 F6 FF");//无视系统菜单显示
//d2client_AEEB1: 5F                 pop edi
//d2client_AEEB2: 5E                 pop esi
//d2client_AEEB3: 5D                 pop ebp
//d2client_AEEB4: 5B                 pop ebx
//d2client_AEEB5: C3                 ret 
PatchJmp(d2client,0xAEEB1,UpdateExp_ASM,5,"5F 5E 5D 5B C3");//justgotaupdateplayerstatepacket
//--- m_MessageLog.h ---
//d2client_7D8AD: 8B 15 48 BC B8 6F  mov edx, [d2client_DBC48](->320)
PatchCall(d2client,0x7D8AD,MessageLogPatch1_ASM,6,"8B 15 $(+DBC48)");
//d2client_7D673: 05 10 27 00 00     add eax, 0x2710 (10000)
PatchCall(d2client,0x7D673,MessageLogPatch2_ASM,5,"05 10 27 00 00");
//--- m_QuickSwapItem.h ---
//d2client_44630: A1 A0 C3 BC 6F     mov eax, [d2client_11C3A0](->1)
PatchCall(d2client,0x44630,RButtonUpHandlerPatch_ASM,5,"A1 $(+11C3A0)");
//d2client_8B976: E9 09 05 F8 FF     jmp d2client_BE84 ^-522999 BBE84->storm_343D0
PatchJmp(d2client,0x8B976,RButtonInTradePatch_ASM,5,"E9 09 05 F8 FF");

//d2client_96CCE: 0F 84 40 01 00 00  jz d2client_96E14 ->+320 B96E14
PatchCall(d2client,0x96CCE,CtrlClickHandlerPatch_ASM,6,"0F 84 40 01 00 00");
//patchCall(PATCH_ADDR(d2client,0x9B7F9),(DWORD)LButtonUpHandlerPatch_ASM,5,0xF70686E8,0x895E5FFF);//家里箱子和身上箱子之间
//patchCall(PATCH_ADDR(d2client,0x9B2D4),(DWORD)LButtonUpHandlerPatch_ASM,5,0xF70BABE8,0x5D5E5FFF);//cube和身上箱子之间
//patchCall(PATCH_ADDR(d2client,0x9C81F),(DWORD)LButtonUpHandlerPatch_ASM,5,0xF6F660E8,0x5D5E5FFF);//左边空地和身上箱子之间
//patchCall(PATCH_ADDR(d2client,0x9BC16),(DWORD)LButtonUpHandlerPatch_ASM,5,0xF70269E8,0x642D39FF);//交易界面和身上箱子之间
//--- m_SkipNpcMessage.h ---
//d2client_4BB07: 8B 4E 0C           mov ecx, [esi+0xC]
//d2client_4BB0A: 0F B7 D7           movzx edx, di
PatchCall(d2client,0x4BB07,NPCQuestMessageStartPatch_ASM,6,"8B 4E 0C 0F B7 D7");
//d2client_48BD6: 8B 44 24 20        mov eax, [esp+0x20]
//d2client_48BDA: 8B 4C 24 1C        mov ecx, [esp+0x1C]
PatchCall(d2client,0x48BD6,NPCQuestMessageEndPatch1_ASM,8,"8B 44 24 20 8B 4C 24 1C");
//d2client_4819F: BA 01 00 00 00     mov edx, 1
PatchCall(d2client,0x4819F,NPCQuestMessageEndPatch2_ASM,5,"BA 01 00 00 00");
//d2client_7E9B7: 0F 84 B8 00 00 00  jz d2client_7EA75 ->+184 B7EA75
PatchCall(d2client,0x7E9B7,NPCMessageLoopPatch_ASM,6,"0F 84 B8 00 00 00");
//--- m_TransGoldHelp.h ---
//d2client_BFDF6: 8B 15 B0 BB BC 6F  mov edx, [d2client_11BBB0](->0)
PatchCall(d2client,0xBFDF6,SetGoldPatch_ASM,6,"8B 15 $(+11BBB0)");//设置转移的金额值
//d2client_C34DB: 83 FB 06           cmp ebx, 6
//d2client_C34DE: 75 30              jnz d2client_C3510 ->+48 BC3510
PatchCall(d2client,0xC34DB,SetDefaultGoldPatch_ASM,5,"83 FB 06 75 30");//设置默认值
//--- m_ULCPath.h ---
//d2client_AF134: 0F B6 57 08        movzx edx, byte ptr [edi+0x8]
//d2client_AF138: 33 C9              xor ecx, ecx
PatchCall(d2client,0xAF134,ULCPatch_ASM,6,"0F B6 57 08 33 C9");
//--- m_NsPassBugFix.h ---
//d2client_45564: E9 12 56 91 90     jmp d2loader.exe_AB7B ^-1869523438 BAB7B
//d2client_45569: E1 03              loopz d2client_4556E ->+3 B4556E
//d2client_4556B: F3 AA              rep stos b
PatchCall(d2client,0x45564,NsPassingActBugFixPatch_ASM,9,"E9 12 56 91 90 E1 03 F3 AA");//-nsbugfixford2loader
//d2client_626FE: E9 A5 AC FA FF     jmp d2client_D3A8 ^-349019 BD3A8->d2win_12FE0 d2win/Ordinal10184
PatchJmp(d2client,0x626FE,DrawGameInfoPatch,5,"E9 A5 AC FA FF");//游戏右上角信息显示
//--- m_GameFilter.h ---
if (fGameFilter) 
//d2win_18911: 8B 76 3C           mov esi, [esi+0x3C]
//d2win_18914: 85 F6              test esi, esi
PatchCall(d2win,0x18911,DrawGameListPatch_ASM,5,"8B 76 3C 85 F6");
//--- m_PartyInfo.h ---
//d2client_5B582: 66 C7 44 24 52 00 00  mov word ptr [esp+0x52], 0
PatchCall(d2client,0x5B582,DrawPetHeadPatch_ASM,7,"66 C7 44 24 52 00 00");
//d2client_5BA9F: C7 44 24 14 00 00 00 00  mov dword ptr [esp+0x14], 0
PatchCall(d2client,0x5BA9F,StartDrawPartyHeadPatch_ASM,8,"C7 44 24 14 00 00 00 00");
//d2client_5BBE0: 8B 0B              mov ecx, [ebx]
//d2client_5BBE2: 8B 54 24 08        mov edx, [esp+0x8]
PatchCall(d2client,0x5BBE0,DrawPartyHeadPatch_ASM,6,"8B 0B 8B 54 24 08");
//d2client_5B86B: E8 26 18 FB FF     call d2client_D096->d2gfx_B080 d2gfx/Ordinal10041
PatchCall(d2client,0x5B86B,DrawSkeletonHeadPatch_ASM,5,"E8 26 18 FB FF");
//--- m_GameCount.h ---
//d2client_AD8A1: F7 C6 00 80 00 00  test esi, 0x8000 (32768)
PatchCall(d2client,0xAD8A1,KillCountPatch_ASM,6,"F7 C6 00 80 00 00");
//d2common_33942: 8B 96 C4 00 00 00  mov edx, [esi+0xC4]
PatchCall(d2common,0x33942,UnitModeChangePatch_ASM,6,"8B 96 C4 00 00 00");
//--- m_GameMonitor.h ---
//d2client_ADEB1: 8B F1              mov esi, ecx
//d2client_ADEB3: 0F B6 56 01        movzx edx, byte ptr [esi+0x1]
//d2client_ADEB7: 8B 4E 02           mov ecx, [esi+0x2]
PatchCall(d2client,0xADEB1,RecvCommand_A7_Patch_ASM,9,"8B F1 0F B6 56 01 8B 4E 02");//设置状态1
//d2client_ADD74: 8B F1              mov esi, ecx
//d2client_ADD76: 0F B6 56 01        movzx edx, byte ptr [esi+0x1]
//d2client_ADD7A: 8B 4E 02           mov ecx, [esi+0x2]
PatchCall(d2client,0xADD74,RecvCommand_A8_Patch_ASM,9,"8B F1 0F B6 56 01 8B 4E 02");//设置状态2
//d2client_ADD31: 8B F1              mov esi, ecx
//d2client_ADD33: 0F B6 56 01        movzx edx, byte ptr [esi+0x1]
//d2client_ADD37: 8B 4E 02           mov ecx, [esi+0x2]
PatchCall(d2client,0xADD31,RecvCommand_A9_Patch_ASM,9,"8B F1 0F B6 56 01 8B 4E 02");//结束状态
//d2client_AE090: B9 3F 00 00 00     mov ecx, 0x3F (63)
PatchCall(d2client,0xAE090,RecvCommand_9C_Patch_ASM,5,"B9 3F 00 00 00");
//d2client_AE880: B9 40 00 00 00     mov ecx, 0x40 (64)
PatchCall(d2client,0xAE880,RecvCommand_9D_Patch_ASM,5,"B9 40 00 00 00");
//d2client_AC801: 8B F1              mov esi, ecx
//d2client_AC803: 8A 46 0B           mov al, [esi+0xB]
PatchCall(d2client,0xAC801,RecvCommand_22_Patch_ASM,5,"8B F1 8A 46 0B");
//--- m_ShowOrbs.h ---
//d2client_276B5: B9 45 10 00 00     mov ecx, 0x1045 (4165)
PatchCall(d2client,0x276B5,ShowLifePatch_ASM,5,"B9 45 10 00 00");
//d2client_27767: B9 46 10 00 00     mov ecx, 0x1046 (4166)
PatchCall(d2client,0x27767,ShowManaPatch_ASM,5,"B9 46 10 00 00");
//--- m_ItemName.h ---
//d2client_92366: 8A 85 2A 01 00 00  mov al, [ebp+0x12A]
PatchCall(d2client,0x92366,ItemNamePatch_ASM,6,"8A 85 2A 01 00 00");
//d2client_944F1: E8 FA B0 02 00     call d2client_BF5F0
PatchCall(d2client,0x944F1,OpenCubeStringPatch_ASM,5,"E8 FA B0 02 00");
//d2client_C0017: 0F 85 E4 00 00 00  jnz d2client_C0101 ->+228 BC0101
patchValue("killgroundgoldname",PATCH_ADDR(d2client,0xC0017),0xe990,2,"0F 85 E4 00 00 00");
//--- m_PermShowItem.h ---
//d2client_C3D4E: 39 1D B4 AD BA 6F  cmp [d2client_FADB4](->0), ebx
PatchCall(d2client,0xC3D4E,PermShowItemsPatch1_ASM,6,"39 1D $(+FADB4)");
//d2client_C0E9A: 39 1D B4 AD BA 6F  cmp [d2client_FADB4](->0), ebx
PatchCall(d2client,0xC0E9A,PermShowItemsPatch1_ASM,6,"39 1D $(+FADB4)");
//d2client_59483: 8B 44 24 1C        mov eax, [esp+0x1C]
//d2client_59487: 85 C0              test eax, eax
PatchCall(d2client,0x59483,PermShowItemsPatch2_ASM,6,"8B 44 24 1C 85 C0");
//d2client_5908A: 0F 8D 89 03 00 00  jge d2client_59419 ->+905 B59419
PatchCall(d2client,0x5908A,PermShowItemsPatch3_ASM,6,"0F 8D 89 03 00 00");
//d2client_A6BA3: 8B 0D B4 AD BA 6F  mov ecx, [d2client_FADB4](->0)
PatchCall(d2client,0xA6BA3,PermShowItemsPatch4_ASM,6,"8B 0D $(+FADB4)");
//--- m_ItemDesc.h ---
//d2client_55C1D: E8 CE 68 FB FF     call d2client_C4F0->d2common_1E190 d2common/Ordinal10460
PatchCall(d2client,0x55C1D,ItemBasicStatPatch_ASM,5,"E8 CE 68 FB FF");
//d2client_55BF7: E8 E2 68 FB FF     call d2client_C4DE->d2common_37EC0 d2common/Ordinal10930
PatchCall(d2client,0x55BF7,GetItemExtStatListPatch,5,"E8 E2 68 FB FF");
//d2client_93450: B8 50 66 00 00     mov eax, 0x6650 (26192)
PatchCall(d2client,0x93450,ItemSocketViewPatch_ASM,5,"B8 50 66 00 00");
//d2client_933E2: B9 9E 0F 00 00     mov ecx, 0xF9E (3998)
PatchCall(d2client,0x933E2,SocketItemStatViewPatch_ASM,5,"B9 9E 0F 00 00");//镶嵌的物品只显示有效属性
//d2client_93BE2: E8 9D 97 F7 FF     call d2client_D384->d2win_118F0 d2win/Ordinal10085
PatchCall(d2client,0x93BE2,ItemDescPath,5,"E8 9D 97 F7 FF");
//d2client_93C0A: E8 75 97 F7 FF     call d2client_D384->d2win_118F0 d2win/Ordinal10085
PatchCall(d2client,0x93C0A,ItemDescPath,5,"E8 75 97 F7 FF");
//d2client_933C5: E8 BA 9F F7 FF     call d2client_D384->d2win_118F0 d2win/Ordinal10085
PatchCall(d2client,0x933C5,ItemDescPath,5,"E8 BA 9F F7 FF");
//--- m_ItemVariableProp.h ---
//d2client_55DAA: 8B C3              mov eax, ebx
//d2client_55DAC: 83 E8 17           sub eax, 0x17 (23)
PatchCall(d2client,0x55DAA,GetItemPropStringPatch_ASM,5,"8B C3 83 E8 17");//普通属性
//d2client_54658: FF 15 BC EA B7 6F  call dword ptr [d2client_CEABC](->d2lang_AFE0 d2lang/Unicode::strcat)
PatchCall(d2client,0x54658,StrcatItemPropStringPatch_ASM,6,"FF 15 $(+CEABC)");//增强伤害等特殊属性
//d2client_8E2C5: 8B 54 24 10        mov edx, [esp+0x10]
//d2client_8E2C9: 8B CE              mov ecx, esi
PatchCall(d2client,0x8E2C5,StrcatSocketStringPatch_ASM,6,"8B 54 24 10 8B CE");//拼孔数，ETH等底部字符串，孔数可能为变量
//d2client_8FDAD: 8B 54 24 18        mov edx, [esp+0x18]
//d2client_8FDB1: 8B CF              mov ecx, edi
PatchCall(d2client,0x8FDAD,StrcatDefenseStringPatch_ASM,6,"8B 54 24 18 8B CF");//拼防御值字符串
//d2client_94367: 8B D0              mov edx, eax
//d2client_94369: 8D 4C 24 60        lea ecx, [esp+0x60]
PatchCall(d2client,0x94367,StrcatDamageStringPatch_ASM1,6,"8B D0 8D 4C 24 60");//拼攻击字符串,一般武器
//d2client_92EE4: 8B D7              mov edx, edi
//d2client_92EE6: 8D 8C 24 48 19 00 00  lea ecx, [esp+0x1948]
PatchCall(d2client,0x92EE4,StrcatDamageStringPatch_ASM2,9,"8B D7 8D 8C 24 48 19 00 00");//拼攻击字符串,绿色武器
//--- m_ItemExtInfo.h ---
//d2client_C3D91: E8 D0 95 F4 FF     call d2client_D366->d2win_133A0 d2win/Ordinal10110
PatchCall(d2client,0xC3D91,DrawHoverPatch_ASM,5,"E8 D0 95 F4 FF");//单独找个地方显示，保持独立
//--- m_ViewUnitStat.h ---
//d2client_BDC16: 5F                 pop edi
//d2client_BDC17: 5E                 pop esi
//d2client_BDC18: 5D                 pop ebp
//d2client_BDC19: 5B                 pop ebx
//d2client_BDC1A: 81 C4 70 03 00 00  add esp, 0x370 (880)
PatchJmp(d2client,0xBDC16,ViewUnitStatPatch_ASM,10,"5F 5E 5D 5B 81 C4 70 03 00 00");
//d2client_9AFCC: 81 C1 10 01 00 00  add ecx, 0x110 (272)
PatchCall(d2client,0x9AFCC,ViewPetStatPatch_ASM,6,"81 C1 10 01 00 00");//pet显示
//--- m_ViewTargetUnitInfo.h ---
//d2client_BCEA6: A1 FC BB BC 6F     mov eax, [d2client_11BBFC](->2678600(->0))
PatchCall(d2client,0xBCEA6,GetViewStatUnit,5,"A1 $(+11BBFC)");//drawstat
//d2client_BD743: A1 FC BB BC 6F     mov eax, [d2client_11BBFC](->2678600(->0))
PatchCall(d2client,0xBD743,GetViewStatUnit,5,"A1 $(+11BBFC)");//drawstat
//d2client_BCBA3: A1 FC BB BC 6F     mov eax, [d2client_11BBFC](->2678600(->0))
PatchCall(d2client,0xBCBA3,GetViewStatUnit,5,"A1 $(+11BBFC)");//drawstat
//d2client_2FE63: 3B 1D FC BB BC 6F  cmp ebx, [d2client_11BBFC](->2678600(->0))
PatchCall(d2client,0x2FE63,GetViewingStatsPlayerPatch1_ASM,6,"3B 1D $(+11BBFC)");//drawstat
//d2client_BC524: 8B 35 FC BB BC 6F  mov esi, [d2client_11BBFC](->2678600(->0))
PatchCall(d2client,0xBC524,GetViewingStatsPlayerPatch2_ASM,6,"8B 35 $(+11BBFC)");//drawstat

//d2client_953E2: 8B 35 FC BB BC 6F  mov esi, [d2client_11BBFC](->2678600(->0))
PatchCall(d2client,0x953E2,GetUnitDrawEquipItemPatch_ASM,6,"8B 35 $(+11BBFC)");//drawequipitems
//d2client_94AB4: 8B 1D FC BB BC 6F  mov ebx, [d2client_11BBFC](->2678600(->0))
PatchCall(d2client,0x94AB4,GetUnitDrawItemDescPath_ASM,6,"8B 1D $(+11BBFC)");//descitem
//d2client_93A6F: 85 C0              test eax, eax
//d2client_93A71: 8B 4F 60           mov ecx, [edi+0x60]
PatchCall(d2client,0x93A6F,GetUnitTestHasItem_ASM,5,"85 C0 8B 4F 60");//testhasitemfordescitem
//d2client_9A695: A1 FC BB BC 6F     mov eax, [d2client_11BBFC](->2678600(->0))
PatchCall(d2client,0x9A695,GetUnitDrawInvItemPatch_ASM,5,"A1 $(+11BBFC)");//drawinvitems

//d2client_99120: E8 DD 30 F7 FF     call d2client_C202->d2common_1DFB0 d2common/Ordinal11017
PatchCall(d2client,0x99120,GetUnitProtectClickEquipPatch_ASM,5,"E8 DD 30 F7 FF");//protectclickequip
//d2client_9908B: 8B 4D 08           mov ecx, [ebp+0x8]
//d2client_9908E: 8B 54 24 2C        mov edx, [esp+0x2C]
PatchCall(d2client,0x9908B,GetUnitProtectClickInvPatch_ASM,7,"8B 4D 08 8B 54 24 2C");//protectclickinv
//--- m_MonsterLifeBar.h ---
//d2win_134D3: 6A 01              push 1
//d2win_134D5: C1 F9 07           sar ecx, 7
//d2win_134D8: 50                 push eax
PatchCall(d2win,0x134D3,LifeBarPatch_ASM,6,"6A 01 C1 F9 07 50");
//d2client_1FE3B: E8 AC D3 FE FF     call d2client_D1EC->d2lang_9450 d2lang/Ordinal10003
PatchCall(d2client,0x1FE3B,MonsterDescCommaPatch1_ASM,5,"E8 AC D3 FE FF");//给怪物的增强属性增加分隔符
//d2client_1FF25: BA D0 23 BD 6F     mov edx, 0x6FBD23D0(d2client_1223D0)(->0)
PatchCall(d2client,0x1FF25,MonsterDescCommaPatch2_ASM,5,"BA $(+1223D0)");//给怪物的增强属性增加分隔符
//d2client_C0C01: FF D5              call ebp
//d2client_C0C03: 8B 4C 24 1C        mov ecx, [esp+0x1C]
PatchCall(d2client,0xC0C01,MonsterDescCommaPatch3_ASM,6,"FF D5 8B 4C 24 1C");//给怪物的增强属性增加分隔符
//d2client_C0B5F: E8 FC C7 F4 FF     call d2client_D360->d2win_12730 d2win/Ordinal10070
PatchCall(d2client,0xC0B5F,MonsterNamePatch_ASM,5,"E8 FC C7 F4 FF");
//--- m_PlayerLifeBar.h ---
//d2client_C0E88: E8 D3 C4 F4 FF     call d2client_D360->d2win_12730 d2win/Ordinal10070
PatchCall(d2client,0xC0E88,PlayerNamePatch_ASM,5,"E8 D3 C4 F4 FF");
//--- m_QuestProtect.h ---
//d2client_1F7C9: 81 FE 16 01 00 00  cmp esi, 0x116 (278)
PatchCall(d2client,0x1F7C9,DrawQuestPagePatch_ASM,6,"81 FE 16 01 00 00");
//d2client_AF8F0: 83 EC 08           sub esp, 8
//d2client_AF8F3: 8B 01              mov eax, [ecx]
PatchCall(d2client,0xAF8F0,RecvCommand_5D_Patch_ASM,5,"83 EC 08 8B 01");
//d2client_523B0: 83 F9 20           cmp ecx, 0x20 (32)
//d2client_523B3: 7C 07              jl d2client_523BC ->+7 B523BC
PatchCall(d2client,0x523B0,RecvCommand_89_Patch_ASM,5,"83 F9 20 7C 07");
//d2client_AD766: 0F B7 59 01        movzx ebx, word ptr [ecx+0x1]
//d2client_AD76A: 8B 08              mov ecx, [eax]
PatchCall(d2client,0xAD766,RecvCommand_A4_Patch_ASM,6,"0F B7 59 01 8B 08");
//d2client_AF1A9: B9 15 00 00 00     mov ecx, 0x15 (21)
PatchCall(d2client,0xAF1A9,RecvCommand_4D_Patch_ASM,5,"B9 15 00 00 00");
//--- item.cpp ---
//d2client_99174: 8B 44 24 18        mov eax, [esp+0x18]
//d2client_99178: 85 C0              test eax, eax
PatchCall(d2client,0x99174,SocketProtectPatch1_ASM,6,"8B 44 24 18 85 C0");
//d2client_96FAB: 8B 44 24 44        mov eax, [esp+0x44]
//d2client_96FAF: 85 C0              test eax, eax
PatchCall(d2client,0x96FAB,SocketProtectPatch2_ASM,6,"8B 44 24 44 85 C0");
// d2client_6B6B2: E8 DF 19 FA FF     call d2client_D096->d2gfx_B080 void __stdcall d2gfx_DrawCellFile(CellContext *context, int xPos, int yPos, DWORD dw1, int dwTransLvl, BYTE *coltab)(6 args)
PatchCall(d2client,0x6B6B2,DrawInvItemPatch_ASM,5,"E8 DF 19 FA FF");
//d2client_98D2E: E8 AD BB F7 FF     call d2client_148E0 void __fastcall d2client_sendPacketLen13(int arg1,int arg2,int arg3)//eax:char cmd(1 args)
PatchCall(d2client,0x98D2E,ActiveBufferItemPatch_ASM,5,"E8 AD BB F7 FF");
//--- m_GameChat.h ---
//d2client_6FEC7: B9 D4 14 00 00     mov ecx, 0x14D4 (5332)
PatchCall(d2client,0x6FEC7,WisperPatch_ASM,5,"B9 D4 14 00 00");
//d2client_70C6C: 8B D8              mov ebx, eax
//d2client_70C6E: 8B 47 08           mov eax, [edi+0x8]
PatchCall(d2client,0x70C6C,InputLinePatch1_ASM,5,"8B D8 8B 47 08");
//d2client_7037B: B9 80 EC BC 6F     mov ecx, 0x6FBCEC80(d2client_11EC80)(->0)
PatchCall(d2client,0x7037B,InputLinePatch2_ASM,5,"B9 $(+11EC80)");
//d2client_A4745: E8 3A 77 F6 FF     call d2client_BE84 void __stdcall d2client_Storm511(D2MSG *pMsg)->storm_343D0
//->d2client_A4745: 58                 pop eax
//->d2client_A4746: EB 02              jmp d2client_A474A ->+2 BA474A
patchValue("",PATCH_ADDR(d2client,0xA4745),0x2EB58,3,"E8 3A 77 F6 FF");
//d2client_A46C5: E8 BA 77 F6 FF     call d2client_BE84 void __stdcall d2client_Storm511(D2MSG *pMsg)->storm_343D0
patchValue("",PATCH_ADDR(d2client,0xA46C5),0x2EB58,3,"E8 BA 77 F6 FF");
//d2win_15F61: C6 84 34 84 00 00 00 00  mov byte ptr [esp+esi+0x84], 0
PatchCall(d2win,0x15F61,SelectTextPatch1_ASM,8,"C6 84 34 84 00 00 00 00");//聊天框选中中文文字
//d2win_15F02: C6 84 2C 84 00 00 00 00  mov byte ptr [esp+ebp+0x84], 0
PatchCall(d2win,0x15F02,SelectTextPatch2_ASM,8,"C6 84 2C 84 00 00 00 00");//聊天框选中中文文字
//--- m_UnicodeSupport.h ---
if (fLocalizationSupport) {
//function d2lang/Unicode::unicode2Win {
//d2lang_82F0: 56                 push esi
//d2lang_82F1: 57                 push edi
//d2lang_82F2: 8B 7C 24 0C        mov edi, [esp+0xC]
PatchJmp(d2lang,0x82F0,D2Lang_Unicode2WinPatch,6,"56 57 8B 7C 24 0C"); 
//function d2lang/Unicode::win2Unicode {
//d2lang_8320: 8B C1              mov eax, ecx
//d2lang_8322: 8B 4C 24 04        mov ecx, [esp+0x4]
PatchJmp(d2lang,0x8320,D2Lang_Win2UnicodePatch,5,"8B C1 8B 4C 24");
//d2win_183A0: E8 AB D0 FF FF     call d2win_15450 d2win/Ordinal10064
PatchCall(d2win,0x183A0,ChannelEnterCharPatch,5,"E8 AB D0 FF FF");//注意InputLinePatch1_ASM必须结合这个，不然会引起部分堆栈错误(虽然不影响使用)
//bnclient_FF5C: 89 2D 4C F6 F3 6F  mov [bnclient_1F64C](->0), ebp
PatchCall(bnclient,0xFF5C,MultiByteFixPatch_ASM,6,"89 2D $(+1F64C)");//for/w*accmsgtext
//function d2win/Ordinal10159 {
//d2win_11880: 66 0F B6 C1        movzx ax, cl
//d2win_11884: 66 3D 00 01        cmp ax, 0x100 (256)
PatchCall(d2win,0x11880,IsPritableCharacterPatch_ASM,8,"66 0F B6 C1 66 3D 00 01");//繁体
//d2win_130E9: 74 19              jz d2win_13104 ->+25 B13104
patchFill("force tbl parse",PATCH_ADDR(d2win,0x130E9),INST_NOP,2,"74 19");
//d2win_130F8: 74 0A              jz d2win_13104 ->+10 B13104
patchFill("parse",PATCH_ADDR(d2win,0x130F8),INST_JMPR,1,"74 0A");
}
//--- m_Socket.h ---
//d2net_72D2: C7 44 24 2C A0 86 01 00  mov dword ptr [esp+0x2C], 0x186A0
PatchCall(d2net,0x72D2,FixSocketTimeOut_ASM,8,"C7 44 24 2C A0 86 01 00");
if (fFixSocketError)
//d2net_7458: 3B C6              cmp eax, esi
//d2net_745A: 7E 15              jle d2net_7471 ->+21 B7471
//d2net_745C: 8B 0D 44 B2 96 00  mov ecx, [d2net_B244](->2B5474C(->0))
PatchCall(d2net,0x7458,FixSocketErrRet_ASM,10,"3B C6 7E 15 8B 0D $(+B244)");
//--- m_GameWindow.h ---
//d2win_173F6: 74 07              jz d2win_173FF ->+7 B173FF
//d2win_173F8: 6A 01              push 1
//d2win_173FA: E8 93 03 FF FF     call d2win_7792->d2gfx_7FC0 d2gfx/Ordinal10059
PatchCall(d2win,0x173F6,NoHidePatch_ASM,9,"74 07 6A 01 E8 93 03 FF FF");
//--- m_MultiClient.h ---
//d2client_1BDE5: E8 F6 C7 FF FF     call d2client_185E0
PatchCall(d2client,0x1BDE5,LeftClick_Patch_ASM,5,"E8 F6 C7 FF FF");
//d2client_1BC42: E8 99 C9 FF FF     call d2client_185E0
PatchCall(d2client,0x1BC42,RightClick_Patch_ASM,5,"E8 99 C9 FF FF");
//--- m_NpcTrade.h ---
//d2client_4AEE0: A1 AC 97 BC 6F     mov eax, [d2client_1197AC](->0)
PatchCall(d2client,0x4AEE0,NpcTradePatch_ASM,5,"A1 $(+1197AC)");
//--- m_Snapshot.h ---
//d2client_AF8B3: 8B 01              mov eax, [ecx]
//d2client_AF8B5: 8B 51 04           mov edx, [ecx+0x4]
PatchCall(d2client,0xAF8B3,RecvCommand_63_Patch_ASM,5,"8B 01 8B 51 04");
//d2client_AEBA3: 8B 41 0C           mov eax, [ecx+0xC]
//d2client_AEBA6: 8B 51 10           mov edx, [ecx+0x10]
PatchCall(d2client,0xAEBA3,RecvCommand_81_Patch_ASM,6,"8B 41 0C 8B 51 10");
  /*if (0&&!fFullWindows) {
//d2gfx_8530: 6A 06                 push 6
patchFill("SW_MINIMIZE->SW_SHOWNOACTIVATE",PATCH_ADDR(d2gfx,0x8531),4,1,"-1 6A 06");
//d2gfx_7D86: 6A 00              push 0
patchFill("SW_HIDE",PATCH_ADDR(d2gfx,0x7D87),4,1,"-1 6A 00");
  }*/
return 1;
}
static int installD2PatchesAfterLoad() {
	if (load_dll_error) return 0;
	pPatchList=&installedPatches;
//--- m_pub.h ---
	//分一个给patch2占位置
//d2client_7CB25: 5F                 pop edi
//d2client_7CB26: 5E                 pop esi
patchValue("hovertextbugfix,setoldfont",PATCH_ADDR(d2client,0x7CB25),0x3AEB,2,"5F 5E");
//--- m_QuickGame.h ---
//d2multi_14A0B: E8 88 32 FF FF     call d2multi_7C98->d2win_13970 d2win/Ordinal10170
PatchCall(d2multi,0x14A0B,NextGameNamePatch,5,"E8 88 32 FF FF");
//d2multi_14A46: E8 4D 32 FF FF     call d2multi_7C98->d2win_13970 d2win/Ordinal10170
PatchCall(d2multi,0x14A46,NextGamePasswordPatch,5,"E8 4D 32 FF FF");
//d2multi_14D29: E8 6A 2F FF FF     call d2multi_7C98->d2win_13970 d2win/Ordinal10170
PatchCall(d2multi,0x14D29,NextGameNamePatch,5,"E8 6A 2F FF FF");
//d2multi_14D64: E8 2F 2F FF FF     call d2multi_7C98->d2win_13970 d2win/Ordinal10170
PatchCall(d2multi,0x14D64,NextGamePasswordPatch,5,"E8 2F 2F FF FF");
//d2multi_14D8F: E8 04 2F FF FF     call d2multi_7C98->d2win_13970 d2win/Ordinal10170
PatchCall(d2multi,0x14D8F,NextGameDecriptionPatch,5,"E8 04 2F FF FF");
//d2multi_14808: E8 91 34 FF FF     call d2multi_7C9E->d2win_14DF0 d2win/Ordinal10042
PatchCall(d2multi,0x14808,SelectGamePatch,5,"E8 91 34 FF FF");
//--- m_GameFilter.h ---
if (fGameFilter) {
//d2mcpclient_63E0: 51                 push ecx
//d2mcpclient_63E1: 8B 41 08           mov eax, [ecx+0x8]
//d2mcpclient_63E4: 56                 push esi
PatchCall(d2mcpclient,0x63E0,D2MCPPacketHandler_05_Patch_ASM,5,"51 8B 41 08 56");
//d2multi_149EF: BA 10 00 00 00     mov edx, 0x10 (16)
PatchCall(d2multi,0x149EF,CreateGameBoxPatch_ASM,5,"BA 10 00 00 00");
//d2multi_11DC3: E8 C4 5E FF FF     call d2multi_7C8C->d2win_18490 d2win/Ordinal10050
PatchCall(d2multi,0x11DC3,DestroyGamelistPatch,5,"E8 C4 5E FF FF");
//d2multi_DF4E: 3D 98 3A 00 00     cmp eax, 0x3A98 (15000)
//d2multi_DF53: 72 14              jb d2multi_DF69 ->+20 BDF69
PatchCall(d2multi,0xDF4E,GameListRefreshTimePatch_ASM,7,"3D 98 3A 00 00 72 14");
//d2multi_11FE2: A1 CC 9D A0 6F     mov eax, [d2multi_39DCC int d2multi_nGames]
PatchCall(d2multi,0x11FE2,SortGameList_Patch_ASM,5,"A1 $(+39DCC)");
//d2multi_1207F: E8 EA 5B FF FF     call d2multi_7C6E->d2win_8A50 d2win/Ordinal10186(2 args)
PatchCall(d2multi,0x1207F,ShowGameListDifficulty_Patch_ASM,5,"E8 EA 5B FF FF");
}
	return 1;
}
int init_d2_ptrs(int server) {
  addr_storm=get_dll_addr("STORM.DLL");if (!addr_storm) return 0;
  addr_d2client=get_dll_addr("D2CLIENT.DLL"); if (!addr_d2client) return 0;
  addr_d2common=get_dll_addr("D2COMMON.DLL");if (!addr_d2common) return 0;
  addr_d2gfx=get_dll_addr("D2GFX.DLL"); if (!addr_d2gfx) return 0;
  addr_d2win=get_dll_addr("D2WIN.DLL");if (!addr_d2win) return 0;
  addr_d2lang=get_dll_addr("D2LANG.DLL"); if (!addr_d2lang) return 0;
  addr_d2cmp=get_dll_addr("D2CMP.DLL"); if (!addr_d2cmp) return 0;
	if (server) addr_bnclient=(HMODULE)0;
  else {addr_bnclient=get_dll_addr("BNCLIENT.DLL"); if (!addr_bnclient) return 0;}
  addr_d2net=get_dll_addr("D2NET.DLL"); if (!addr_d2net) return 0;
  addr_fog=get_dll_addr("FOG.DLL"); if (!addr_fog) return 0;
  addr_d2game=get_dll_addr("D2GAME.DLL"); if (!addr_d2game) return 0;
	if (!server) {
		addr_d2launch=get_dll_addr("D2LAUNCH.DLL");if (!addr_d2launch) return 0;
		addr_d2gdi=get_dll_addr("D2GDI.DLL"); if (!addr_d2gdi) return 0;
	}
  addr_d2sound=get_dll_addr("D2SOUND.DLL");if (!addr_d2sound) return 0;
	if (!server) {
		addr_d2multi=GetModuleHandle("D2MULTI.DLL");
		addr_d2mcpclient=GetModuleHandle("D2MCPCLIENT.DLL");
	}
	if (load_dll_error) return 0;
	#define INIT_D2PTR_ADDR
	#include "d2ptrs.h"
	#undef INIT_D2PTR_ADDR
	return 1;
}
int init_d2_ptrs2() {
	if (!addr_d2multi) {addr_d2multi=get_dll_addr("D2MULTI.DLL");if (!addr_d2multi) return 0;}
  if (!addr_d2mcpclient) {addr_d2mcpclient=get_dll_addr("D2MCPCLIENT.DLL");if (!addr_d2mcpclient) return 0;}
	if (load_dll_error) return 0;
	#define INIT_D2PTR_ADDR
	#undef INIT_D2PTR_ADDR
	return 1;
}
static int fInstallPatch2=0;
static void InstallPatchAfterLoad() {
	if (!fInstallPatch2 ){
		//RelocD2Ptrs2();
		init_d2_ptrs2();
		installD2PatchesAfterLoad();
		fInstallPatch2 = 1;
	}
}
void __declspec(naked) InstallPatchAfterLoad_ASM() {
	__asm {
		cmp [fInstallPatch2],1
		je org
		push edi
		call InstallPatchAfterLoad
		pop edi
org:
		pop ecx
		lea eax, dword ptr [esp+0x4]
		push eax
		jmp ecx
	}
}
int installResolutionPatches();
void setResolutionPatch(int install) {
	if (install) {
		if (!resolutionPatches) {
			pPatchList=&resolutionPatches;
			if (!load_dll_error) installResolutionPatches();
		}
	} else {
		if (resolutionPatches) {
			removePatches(&resolutionPatches);
			resolution_uninit();
		}
	}
}
void ResetPacketHandlerTimer();
int installDebugPatches();
void MonitorPacket() {
	if (tPacketHandler.isOn) {
		if (!networkPatch) {
			installNetworkRecvPatch();
			pPatchList=&networkPatch;
			installDebugPatches();
		}
		ResetPacketHandlerTimer();
		if (fInGame) SetBottomAlertMsg3(L"Network Packet Patch Installed",3000,1,1);
	} else {
		if (networkPatch) removePatches(&networkPatch);
	}
}
static int installPatchToggle() {
	if (tInstallPatches.isOn) {
		int patchCount0=patchCount;
		installD2Patches();
		if (!fInstallPatch2) {fInstallPatch2=1;installD2PatchesAfterLoad();}
		if (fInGame) gameMessage("install %d patches",patchCount-patchCount0);
		fInGame=0;
	} else {
		if (installedPatches) {
			RemoveMyAutomapNodes();
			int n=removePatches(&installedPatches);
			if (fInGame) gameMessage("remove %d patches",n);
			fInstallPatch2=0;patchCount=0;
		}
	}
	return 1;
}
extern BYTE fAutomapPartyDefault,fAutomapNamesDefault;
void saveCharMenu();
static void dump() {
	#define DUMP(dll) LOG("%s: %X\n",#dll,addr_##dll)
	DUMP(d2game)
	DUMP(d2net)
	DUMP(d2gdi)
	DUMP(d2win)
	DUMP(d2sound)
	DUMP(d2multi)
	DUMP(d2mcpclient)
	DUMP(d2launch)
	DUMP(d2gfx)
	DUMP(d2client)
	DUMP(storm)
	DUMP(d2lang)
	DUMP(d2common)
	DUMP(d2cmp)
	DUMP(bnclient)
	DUMP(fog)
	#undef DUMP
}
static void dumpPtr(FILE *fp,char *dll,int dllbase,int off,char *name,int addr) {
	if (dllbase<=addr&&addr<dllbase+0x200000) return;
	if (!fp) return;
	fprintf(fp,"ERROR ptr: %X %s_%X %s %X\n",addr,dll,off,name,dllbase);
}
void check_d2ptrs() {
	if (!logfp) return;
	FILE *fp=logfp;
	#define DUMP_D2PTR
	#include "d2ptrs.h"
	#undef DUMP_D2PTR
}
BOOL Install(){
	if (!init_d2_ptrs(0)) return 0;
	if (!LoadConfigFile("config.txt",0)) return 0;
	resolution_init();
  if (!installD2Patches()) {
		MessageBox(NULL,"Error install patches","d2hackmap",MB_OK);
		return 0;
	}
	HMODULE plugYAddr=GetModuleHandle("PlugY.dll");
	hasPlugY=plugYAddr!=0;
	if (hasPlugY) {
		LOG("PlugY installed, panel and worldevent patches will be disabled\n");
	}
	pPatchList=&installedPatches;
	installPanelPatches();
	installServerPatches();
	if (tPacketHandler.isOn) MonitorPacket();
	//if (tHighResolution.isOn) HightResolution();
	LOG("Install %d patches done\n",patchCount);
	InitCellFiles();
	LOG("Loading minimap bitmaps done\n");
	if (fAutomapPartyDefault != (BYTE)-1) d2client_SetAutomapParty(fAutomapPartyDefault==0?0:1);
	if (fAutomapNamesDefault != (BYTE)-1) d2client_SetAutomapNames(fAutomapNamesDefault==0?0:1);
	//LPSTR cmdLine = GetCommandLine();
	//HMODULE module = GetModuleHandle(path);
	initSendPacketCheckTable();
	if (debug) MessageBox(NULL,"Press to continue","PauseAtStartup",MB_OK);
	if (dwGameLng<0) {
		dwGameLng=GetGameLanguage();
		if (dwGameLng) dwGameLng = 1;
	}
	if (*d2launch_pD2Characters) saveCharMenu();
	if (debug) dump();
	check_d2ptrs();
	if (bnetIp) LOG("Bnetip=%s\n",bnetIp);
	return TRUE;
}
void patchServer() {
	fIsRealmServer=1;
	logfp=stdout;
	//printf("d2hackmap patch server\n");
	if (!init_d2_ptrs(1)) return;
	init_d2_ptrs2();
	if (!LoadConfigFile("config_server.txt",1)) return;
	pPatchList=&installedPatches;
	installServerPatches();
	installAlwaysDebugPatches();
}
static int *pReloadFlag=NULL,*pExitFlag=NULL;
extern "C" __declspec(dllexport) void __fastcall SetReloadFlag(int *reload_flag,int *exit_flag) {
	pReloadFlag=reload_flag;pExitFlag=exit_flag;
}
extern "C" __declspec(dllexport) void __fastcall SetBnetIp(char *bnetip) {
	bnetIp=bnetip;
}
void unpatchServer() {
	removePatches(&installedPatches);
	installedPatches=NULL;
	if (pExitFlag&&!IsBadWritePtr(pExitFlag,4)) *pExitFlag=1;
}
void ToggleIMEInput(BOOL fChatInput);
void Uninstall() {
	ToggleIMEInput(1);
	RemoveMyAutomapNodes();
	DeleteCellFiles();
	removePatches(&keyPatches);
	removePatches(&installedPatches);
	removePatches(&networkPatch);
	removePatches(&resolutionPatches);
	uninstallBigstashPatches();
	installedPatches=NULL;
	LOG("pExitFlag=0x%X\n",pExitFlag);
	if (pExitFlag&&!IsBadWritePtr(pExitFlag,4)) *pExitFlag=1;
}
int ReloadDll() {
	LOG("pReloadFlag=0x%X\n",pReloadFlag);
	if (pReloadFlag&&!IsBadWritePtr(pReloadFlag,4)) *pReloadFlag=1;
	return 0;
}
extern char szDllPath[MAX_PATH];
void panel_reset();
int ReloadConfig() {
	char dllpath[MAX_PATH];
	sprintf(dllpath,"%sdll\\d2hackmap.dll",szPluginPath);
	if (fileExist(dllpath)) {
		struct stat st1,st2;
		if (stat(dllpath,&st1)==0&&stat(szDllPath,&st2)==0) {
			if (st1.st_mtime>st2.st_mtime||st1.st_size!=st2.st_size) {
				gameMessage("Dll modified, reload instead\n");
				ReloadDll();
				return 0;
			}
		}
	}
	DeleteCellFiles();
	if (LoadConfigFile("config.txt",0)) {
		InitCellFiles();
		gameMessage("Config Reload Successful");
		ShowWarningMessage();
	}
	//panel_reset();
	fInGame=0;
	return 0;
}
