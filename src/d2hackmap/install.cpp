#include "stdafx.h"
#include <sys/stat.h>
#include "d2ptrs.h"
#include "d2ptrs2.h"

#define DECLARE_D2PTR_VARIBLE
#include "d2ptrs.h"
#include "d2ptrs2.h"
#undef DECLARE_D2PTR_VARIBLE

void InitCellFiles();
void DeleteCellFiles();
void RemoveMyAutomapNodes();

extern int dwDumpMpqPath;
extern ToggleVar tHighResolution;
#define INST_NOP 0x90
#define INST_CALL 0xe8
#define INST_JMP 0xe9
#define INST_JMPR 0xeb
#define INST_RET 0xc3
void PatchCALL(DWORD addr, DWORD param, int len);
void PatchJMP(DWORD addr, DWORD param, int len);
void PatchFILL(DWORD addr, DWORD param, int len);
void PatchVALUE(DWORD addr, DWORD param, int len);
static int ReloadDll();
static int ReloadConfig();
static int installPatchToggle();
static void InstallPatchAfterLoad();
void resolution_init();
void resolution_uninit();
static int patchCount=0;
int load_dll_error=0;

HMODULE addr_storm=NULL,addr_d2client=NULL,addr_d2common=NULL,addr_d2gfx=NULL,addr_d2win=NULL,addr_d2lang=NULL;
HMODULE addr_d2cmp=NULL,addr_bnclient=NULL,addr_d2net=NULL,addr_fog=NULL,addr_d2game=NULL,addr_d2launch=NULL;
HMODULE addr_d2gdi=NULL,addr_d2sound=NULL;
HMODULE addr_d2multi=NULL,addr_d2mcpclient=NULL;
int fGameFilter=0;
int	fFixSocketError=1;
ToggleVar tReloadDll={TOGGLEVAR_DOWN,0,-1,1,"ReloadDll",&ReloadDll};
ToggleVar tReloadConfig={TOGGLEVAR_DOWN,0,-1,1,"ReloadConfig",&ReloadConfig};
ToggleVar tInstallPatches={TOGGLEVAR_ONOFF,1,-1,1,"InstallPatches",&installPatchToggle};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY, "ReloadConfigKey",			&tReloadConfig     },
	{CONFIG_VAR_TYPE_KEY, "ReloadDllKey",			&tReloadDll     },
	{CONFIG_VAR_TYPE_KEY, "InstallPatchesToggle",			&tInstallPatches     },
	{CONFIG_VAR_TYPE_INT, "GameFilterSupport",     &fGameFilter,  4},
	{CONFIG_VAR_TYPE_INT, "FixSocketError",	   &fFixSocketError,4},
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
void PatchCALL(DWORD addr, DWORD param, int len) {
	InterceptLocalCode(INST_CALL, addr, param, len);
}
void PatchJMP(DWORD addr, DWORD param, int len) {
	InterceptLocalCode(INST_JMP, addr, param, len);
}
void PatchFILL(DWORD addr, DWORD param, int len) {
	FillLocalBYTES(addr, (BYTE)param, len);
}
void PatchVALUE(DWORD addr, DWORD param, int len) {
	WriteLocalBYTES(addr, &param, len);
}
struct InstalledPatch {int addr,len;struct InstalledPatch *next;char oldcode[4];};
static InstalledPatch **pPatchList=NULL;
static InstalledPatch *keyPatches=NULL,*installedPatches=NULL,*resolutionPatches=NULL;
struct InstalledPatch *networkPatch=NULL;
int installPatch(void (*func)(DWORD, DWORD, int),DWORD addr,DWORD param,int len) {
	if (!func||!pPatchList) return 0;
	InstalledPatch *p=(InstalledPatch *)malloc(sizeof(InstalledPatch)+len);
	p->addr=addr;p->len=len;p->next=*pPatchList;
	memcpy(p->oldcode,(void *)addr,len);
	if (0&&logfp) {
		fprintf(logfp,"Install Patch 0x%x oldcode[%d]={",addr,len);
		for (int i=0;i<len;i++) fprintf(logfp,"0x%02x,",(unsigned char)p->oldcode[i]);
		fprintf(logfp,"};\n");
		fflush(logfp);
	}
	func(addr,param,len);
	*pPatchList=p;patchCount++;
	return 1;
}
static int removePatches(InstalledPatch **phead) {
	int n=0;
	while (*phead) {
		InstalledPatch *p=*phead;if (p->oldcode) WriteLocalBYTES(p->addr,p->oldcode,p->len);*phead=p->next;free(p);
		n++;
	}
	return n;
}

#include "patch.h"
#define DLL_ADDR(dll,addr) (addr-##dll##_ADDR+(DWORD)addr_##dll)
#define d2client_ADDR 0x6FAB0000
#define d2common_ADDR 0x6FD50000
#define d2gfx_ADDR 0x6FA80000
#define d2win_ADDR 0x6F8E0000
#define d2lang_ADDR 0x6FC00000
#define d2cmp_ADDR 0x6FE10000
#define bnclient_ADDR 0x6FF20000
#define d2net_ADDR 0x6FBF0000
#define d2game_ADDR 0x6FC20000
#define d2launch_ADDR 0x6FA40000
#define d2gdi_ADDR 0x6F870000
#define d2multi_ADDR 0x6F9D0000
#define storm_ADDR 0x6FBF0000
#define fog_ADDR 0x6FF50000
#define d2sound_ADDR 0x6F9B0000
#define d2mcpclient_ADDR 0x6FA20000
static int installNetworkRecvPatch() {
	pPatchList=&networkPatch;
	if (load_dll_error) return 0;
	installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF39A8),(DWORD)GamePacketReceivedInterceptPatch_ASM,5);
	return 1;
}
int installResolutionPatches() {
	pPatchList=&resolutionPatches;
	if (load_dll_error) return 0;
installPatch(PatchCALL,DLL_ADDR(d2gdi,0x6F876D42),(DWORD)SetBitBlockWidthAndHeightPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2gdi,0x6F877051),(DWORD)SetCelDisplayLeftAndRightPatch_ASM,10);
installPatch(PatchCALL,DLL_ADDR(d2gdi,0x6F8771C5),(DWORD)ClearFramebufPatch_ASM,9);
installPatch(PatchCALL,DLL_ADDR(d2gfx,0x6FA88141),(DWORD)IsNeedRestoreDownWindowPatchW_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2gfx,0x6FA88154),(DWORD)IsNeedRestoreDownWindowPatchH_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2gfx,0x6FA883F0),(DWORD)IsNeedResizeWindowPatchW_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2gfx,0x6FA88403),(DWORD)IsNeedResizeWindowPatchH_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2gfx,0x6FA88442),(DWORD)AdjustWindowRectExPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2gfx,0x6FA88005),(DWORD)SetDisplayWidthAndHeightPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2gfx,0x6FA8A9EF),(DWORD)DrawBottomGroundPiecePatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2gfx,0x6FA889D9),(DWORD)DrawGroundHeightPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAC0E13),(DWORD)SetGeneralDisplayWidthAndHeightPatch_ASM,10);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD7459),(DWORD)DrawLeftOrbImagePatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD7D45),(DWORD)DrawLeftOrbPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD7536),(DWORD)DrawRightOrbImagePatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD7BBE),(DWORD)DrawRightOrbPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD7703),(DWORD)DisplayHPTextPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD77C2),(DWORD)DisplayManaTextPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD7D18),(DWORD)FillLeftOrbPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD7B8D),(DWORD)FillRightOrbPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD8D49),(DWORD)DrawLeftSkillButtonPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD8DB3),(DWORD)DrawRightSkillButtonPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB3A4F8),(DWORD)BottomBoundPatch_ASM,8);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB73A05),(DWORD)SetScreenShiftPatch_ASM,10);
//installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB6CB30),INST_NOP,10);//clickifmode!=2
//installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB3F880),INST_NOP,10);
installPatch(PatchCALL,DLL_ADDR(d2common,0x6FDD2090),(DWORD)GetGlobalInventoryPositionPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2common,0x6FD633AB),(DWORD)GetGlobalBeltSlotPositionPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2common,0x6FDD1F9C),(DWORD)GetGlobalEquipmentSlotLayoutPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2common,0x6FDD2020),(DWORD)GetGlobalInventoryGridLayoutPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2common,0x6FD632EC),(DWORD)GetGlobalBeltRecordPatch_ASM,7);
  return 1;
}
static int installD2Patches() {
	if (load_dll_error) return 0;
	if (!keyPatches) {
		pPatchList=&keyPatches;
		installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F7740),(DWORD)WinMessagePatch_ASM,5);
		installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAE8BB5),(DWORD)KeydownPatch_ASM,7);
		installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAE8AE1),(DWORD)KeyupPatch_ASM,6);
		installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F87E3),(DWORD)SkipDrawMenuPatch_ASM,5);
		installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF423C),(DWORD)SkipAllDrawingPatch_ASM,5);
installPatch(PatchVALUE,DLL_ADDR(d2common,0x6FD7F384),0xFC,1);//unit size 0xF4 -> 0xFC
installPatch(PatchVALUE,DLL_ADDR(d2common,0x6FD7F394),0x3F,1);//zero unit 0xF4 -> 0xFC
	}
	if (installedPatches) return 1;
	pPatchList=&installedPatches;
if (dwDumpMpqPath) installPatch(PatchCALL,DLL_ADDR(storm,0x6FC18964),(DWORD)storm268_patch_ASM,6);
//--- m_pub.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF4230),(DWORD)GameLoopPatch_ASM,7);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF5153),(DWORD)GameEndPatch_ASM,5);
installPatch(PatchVALUE,DLL_ADDR(d2client,0x6FACAB88),0xEB,1);//取消进出传送门时间限制
	if (addr_d2multi&&addr_d2mcpclient) InstallPatchAfterLoad();
	else installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F881F),(DWORD)InstallPatchAfterLoad_ASM,5);
//--- login ---
installPatch(PatchCALL,DLL_ADDR(d2launch,0x6FA59539),(DWORD)LastBNet_Patch,5);
installPatch(PatchCALL,DLL_ADDR(d2launch,0x6FA4E1B1),(DWORD)SelectCharPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2launch,0x6FA4CBF6),(DWORD)OpenCharMenuPatch_ASM,6);
//--- winmsg.h ---
installPatch(PatchCALL,DLL_ADDR(d2launch,0x6FA5805B),(DWORD)ShowVersion_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF33D1),(DWORD)StandStillPatch_ASM,10);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5BF86),(DWORD)ReceiveSwapWeapon_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAC621D),(DWORD)SetCursorPosPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD71C0),(DWORD)DrawLeftBackImagePatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD70D6),(DWORD)DrawRightBackImagePatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF436F),(DWORD)DrawGroundAndUnitPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF437B),(DWORD)DrawControlsPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F896C),(DWORD)MainMenuFlushFramebufPatch_ASM,5);
//--- m_PacketHandler.h ---
installPatch(PatchCALL,DLL_ADDR(d2net,0x6FBF7650),(DWORD)GamePacketSendInterceptPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAFA2A1),(DWORD)ResetSkillPatch_ASM,5);
//--- m_DropProtection.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF9440),(DWORD)ClickHireMercMenuItemPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2launch,0x6FA4F3D0),(DWORD)DeleteCharacter_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2launch,0x6FA4DB9F),(DWORD)DeleteSelectedCharacter_Patch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8EF4E6),(DWORD)ClickMenuItem_Patch_ASM,5);
//--- m_AutoMapCell.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB11354),(DWORD)OverrideShrinePatch_ASM,7);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB11D83),(DWORD)AddShrinePatch_ASM,6);
installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB11D31),INST_NOP,6);//shrinedistance
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB104EA),(DWORD)DrawAutomapNodePatch,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB10E16),(DWORD)MinimapPatch_ASM,6);
//--- m_AutoMapBlob.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB11AC4),(DWORD)ForceDrawBlob_ASM,6);
installPatch(PatchJMP,DLL_ADDR(d2client,0x6FB0F1C0),(DWORD)DrawBlobPath_ASM,6);
installPatch(PatchJMP,DLL_ADDR(d2client,0x6FB11688),(DWORD)MixedBlobColPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB10BD8),(DWORD)DrawPartyPlayerBlobPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB1175D),(DWORD)HostilePlayerColor_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB11CD3),(DWORD)DrawPlayerTextPatch_ASM,11);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB11B67),(DWORD)MonsterBlobDescPatch_ASM,5);
//--- m_Environment.h ---
installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB125C4),INST_NOP,2);//floorsinside
installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB1260A),INST_NOP,2);//wallsinside
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB59A37),(DWORD)LightingPatch_ASM,6);
installPatch(PatchJMP,DLL_ADDR(d2common,0x6FDBCCB6),(DWORD)WeatherPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF42A2),(DWORD)ShakeScreenPatch,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB16625),(DWORD)InfravisionPatch_ASM,5);
//--- m_MonsterDeath.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD0FBE),(DWORD)MonsterDeathPatch_ASM,5);
//--- m_ScrollLock.h ---
installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB3B16F),INST_NOP,2);//forceperspectiveshake
installPatch(PatchFILL,DLL_ADDR(d2client,0x6FAEF8DF),INST_NOP,2);//forcegetshake
installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB3B096),INST_NOP,15);//killaddshake
installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB3B0A5),INST_NOP,2);//killaddshake,part2
installPatch(PatchFILL,DLL_ADDR(d2client,0x6FB0FB10),INST_NOP,0x37);//killautomapscroll,wipeout4adjustments
//--- m_QuickBackTown.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5D2F0),(DWORD)RecvCommand_60_Patch_ASM,5);
//--- m_PartyHelp.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5D6B0),(DWORD)RecvCommand_5A_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5D8F0),(DWORD)RecvCommand_5B_Patch_ASM,6);
//--- m_OutTownSelect.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB56941),(DWORD)OutTownSelectPatch_ASM1,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FACBA6B),(DWORD)OutTownSelectPatch_ASM2,6);
//--- m_AutoSkill.h ---
//installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD8C97),(DWORD)DrawSkillButton_Patch_ASM,5);
//--- m_AutoEnchant.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5D740),(DWORD)RecvCommand_2C_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB56505),(DWORD)AddUnitPatch_ASM,6);
//--- m_AutoSummon.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB56F25),(DWORD)RemoveUnitPatch1_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB56F30),(DWORD)RemoveUnitPatch2_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5F395),(DWORD)RecvCommand_69_Patch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5C720),(DWORD)RecvCommand_7A_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5C911),(DWORD)RecvCommand_21_Patch_ASM,6);
//--- m_ScreenHook.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB73B96),(DWORD)DrawClientPatch_ASM,5);//无视系统菜单显示
installPatch(PatchJMP,DLL_ADDR(d2client,0x6FB5EEB1),(DWORD)UpdateExp_ASM,5);//justgotaupdateplayerstatepacket
//--- m_MessageLog.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB2D8AD),(DWORD)MessageLogPatch1_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB2D673),(DWORD)MessageLogPatch2_ASM,5);
//--- m_QuickSwapItem.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF4630),(DWORD)RButtonUpHandlerPatch_ASM,5);
installPatch(PatchJMP,DLL_ADDR(d2client,0x6FB3B976),(DWORD)RButtonInTradePatch_ASM,5);

installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB46CCE),(DWORD)CtrlClickHandlerPatch_ASM,6);
//installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB4B7F9),(DWORD)LButtonUpHandlerPatch_ASM,5);//家里箱子和身上箱子之间
//installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB4B2D4),(DWORD)LButtonUpHandlerPatch_ASM,5);//cube和身上箱子之间
//installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB4C81F),(DWORD)LButtonUpHandlerPatch_ASM,5);//左边空地和身上箱子之间
//installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB4BC16),(DWORD)LButtonUpHandlerPatch_ASM,5);//交易界面和身上箱子之间
//--- m_SkipNpcMessage.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAFBB07),(DWORD)NPCQuestMessageStartPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF8BD6),(DWORD)NPCQuestMessageEndPatch1_ASM,8);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF819F),(DWORD)NPCQuestMessageEndPatch2_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB2E9B7),(DWORD)NPCMessageLoopPatch_ASM,6);
//--- m_TransGoldHelp.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB6FDF6),(DWORD)SetGoldPatch_ASM,6);//设置转移的金额值
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB734DB),(DWORD)SetDefaultGoldPatch_ASM,5);//设置默认值
//--- m_ULCPath.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5F134),(DWORD)ULCPatch_ASM,6);
//--- m_NsPassBugFix.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAF5564),(DWORD)NsPassingActBugFixPatch_ASM,9);//-nsbugfixford2loader
installPatch(PatchJMP,DLL_ADDR(d2client,0x6FB126FE),(DWORD)DrawGameInfoPatch,5);//游戏右上角信息显示
//--- m_GameFilter.h ---
if (fGameFilter) 
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F8911),(DWORD)DrawGameListPatch_ASM,5);
//--- m_PartyInfo.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB0B582),(DWORD)DrawPetHeadPath_ASM,7);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB0BBE0),(DWORD)DrawPartyHeadPath_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB0B86B),(DWORD)DrawSkeletonHeadPatch_ASM,5);
//--- m_GameCount.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5D8A1),(DWORD)KillCountPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2common,0x6FD83942),(DWORD)UnitModeChangePatch_ASM,6);
//--- m_GameMonitor.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5DEB1),(DWORD)RecvCommand_A7_Patch_ASM,9);//设置状态1
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5DD74),(DWORD)RecvCommand_A8_Patch_ASM,9);//设置状态2
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5DD31),(DWORD)RecvCommand_A9_Patch_ASM,9);//结束状态
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5E090),(DWORD)RecvCommand_9C_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5E880),(DWORD)RecvCommand_9D_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5C801),(DWORD)RecvCommand_22_Patch_ASM,5);
//--- m_ShowOrbs.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD76B5),(DWORD)ShowLifePatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAD7767),(DWORD)ShowManaPatch_ASM,5);
//--- m_ItemName.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB42366),(DWORD)ItemNamePatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB444F1),(DWORD)OpenCubeStringPatch_ASM,5);
installPatch(PatchVALUE,DLL_ADDR(d2client,0x6FB70017),0xe990,2);//killgroundgoldname
//--- m_PermShowItem.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB73D4E),(DWORD)PermShowItemsPatch1_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB70E9A),(DWORD)PermShowItemsPatch1_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB09483),(DWORD)PermShowItemsPatch2_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB0908A),(DWORD)PermShowItemsPatch3_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB56BA3),(DWORD)PermShowItemsPatch4_ASM,6);
//--- m_ItemDesc.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB05C1D),(DWORD)ItemBasicStatPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB05BF7),(DWORD)GetItemExtStatListPatch,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB43450),(DWORD)ItemSocketViewPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB433E2),(DWORD)SocketItemStatViewPatch_ASM,5);//镶嵌的物品只显示有效属性
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB43BE2),(DWORD)ItemDescPath,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB43C0A),(DWORD)ItemDescPath,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB433C5),(DWORD)ItemDescPath,5);
//--- m_ItemVariableProp.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB05DAA),(DWORD)GetItemPropStringPatch_ASM,5);//普通属性
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB04658),(DWORD)StrcatItemPropStringPatch_ASM,6);//增强伤害等特殊属性
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB3E2C5),(DWORD)StrcatSocketStringPatch_ASM,6);//拼孔数，ETH等底部字符串，孔数可能为变量
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB3FDAD),(DWORD)StrcatDefenseStringPatch_ASM,6);//拼防御值字符串
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB44367),(DWORD)StrcatDamageStringPatch_ASM1,6);//拼攻击字符串,一般武器
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB42EE4),(DWORD)StrcatDamageStringPatch_ASM2,9);//拼攻击字符串,绿色武器
//--- m_ItemExtInfo.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB73D91),(DWORD)DrawHoverPatch_ASM,5);//单独找个地方显示，保持独立
//--- m_ViewUnitStat.h ---
installPatch(PatchJMP,DLL_ADDR(d2client,0x6FB6DC16),(DWORD)ViewUnitStatPatch_ASM,10);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB4AFCC),(DWORD)ViewPetStatPatch_ASM,6);//pet显示
//--- m_ViewTargetUnitInfo.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB6CEA6),(DWORD)GetViewStatUnit,5);//drawstat
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB6D743),(DWORD)GetViewStatUnit,5);//drawstat
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB6CBA3),(DWORD)GetViewStatUnit,5);//drawstat
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FADFE63),(DWORD)GetViewingStatsPlayerPatch1_ASM,6);//drawstat
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB6C524),(DWORD)GetViewingStatsPlayerPatch2_ASM,6);//drawstat

installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB453E2),(DWORD)GetUnitDrawEquipItemPatch_ASM,6);//drawequipitems
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB44AB4),(DWORD)GetUnitDrawItemDescPath_ASM,6);//descitem
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB43A6F),(DWORD)GetUnitTestHasItem_ASM,5);//testhasitemfordescitem
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB4A695),(DWORD)GetUnitDrawInvItemPatch_ASM,5);//drawinvitems

installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB49120),(DWORD)GetUnitProtectClickEquipPatch_ASM,5);//protectclickequip
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB4908B),(DWORD)GetUnitProtectClickInvPatch_ASM,7);//protectclickinv
//--- m_MonsterLifeBar.h ---
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F34D3),(DWORD)LifeBarPatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FACFE3B),(DWORD)MonsterDescCommaPatch1_ASM,5);//给怪物的增强属性增加分隔符
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FACFF25),(DWORD)MonsterDescCommaPatch2_ASM,5);//给怪物的增强属性增加分隔符
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB70C01),(DWORD)MonsterDescCommaPatch3_ASM,6);//给怪物的增强属性增加分隔符
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB70B5F),(DWORD)MonsterNamePatch_ASM,5);
//--- m_PlayerLifeBar.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB70E88),(DWORD)PlayerNamePatch_ASM,5);
//--- m_QuestProtect.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FACF7C9),(DWORD)DrawQuestPagePatch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5F8F0),(DWORD)RecvCommand_5D_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB023B0),(DWORD)RecvCommand_89_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5D766),(DWORD)RecvCommand_A4_Patch_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5F1A9),(DWORD)RecvCommand_4D_Patch_ASM,5);
//--- m_ItemSocketProtect.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB49174),(DWORD)SocketProtectPatch1_ASM,6);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB46FAB),(DWORD)SocketProtectPatch2_ASM,6);
//--- m_GameChat.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB1FEC7),(DWORD)WisperPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB20C6C),(DWORD)InputLinePatch1_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB2037B),(DWORD)InputLinePatch2_ASM,5);
installPatch(PatchVALUE,DLL_ADDR(d2client,0x6FB54745),0x2EB58,3);
installPatch(PatchVALUE,DLL_ADDR(d2client,0x6FB546C5),0x2EB58,3);
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F5F61),(DWORD)SelectTextPatch1_ASM,8);//聊天框选中中文文字
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F5F02),(DWORD)SelectTextPatch2_ASM,8);//聊天框选中中文文字
//--- m_UnicodeSupport.h ---
if (fLocalizationSupport) {
installPatch(PatchJMP,DLL_ADDR(d2lang,0x6FC082F0),(DWORD)D2Lang_Unicode2WinPatch,5);
installPatch(PatchJMP,DLL_ADDR(d2lang,0x6FC08320),(DWORD)D2Lang_Win2UnicodePatch,5);
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F83A0),(DWORD)ChannelEnterCharPatch,5);//注意InputLinePatch1_ASM必须结合这个，不然会引起部分堆栈错误(虽然不影响使用)
installPatch(PatchCALL,DLL_ADDR(bnclient,0x6FF2FF5C),(DWORD)MultiByteFixPatch_ASM,6);//for/w*accmsgtext
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F1880),(DWORD)IsPritableCharacterPatch_ASM,8);//繁体
installPatch(PatchFILL,DLL_ADDR(d2win,0x6F8F30E9),INST_NOP,2);//强制tbl解析方式
installPatch(PatchFILL,DLL_ADDR(d2win,0x6F8F30F8),INST_JMPR,1);//强制tbl解析方式
}
//--- m_Socket.h ---
installPatch(PatchCALL,DLL_ADDR(d2net,0x6FBF72D2),(DWORD)FixSocketTimeOut_ASM,8);
if (fFixSocketError)
installPatch(PatchCALL,DLL_ADDR(d2net,0x6FBF7458),(DWORD)FixSocketErrRet_ASM,10);
//--- m_GameWindow.h ---
installPatch(PatchCALL,DLL_ADDR(d2win,0x6F8F73F6),(DWORD)NoHidePatch_ASM,9);
//--- m_MultiClient.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FACBDE5),(DWORD)LeftClick_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FACBC42),(DWORD)RightClick_Patch_ASM,5);
//--- m_NpcTrade.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FAFAEE0),(DWORD)NpcTradePatch_ASM,5);
//--- m_Snapshot.h ---
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5F8B3),(DWORD)RecvCommand_63_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2client,0x6FB5EBA3),(DWORD)RecvCommand_81_Patch_ASM,6);
  /*if (0&&!fFullWindows) {
		//6FA88530 - 6A 06                 - push 06 { 6 } //SW_MINIMIZE
		installPatch(PatchFILL,DLL_ADDR(d2gfx,0x6FA88531),4,1);//SW_MINIMIZE->SW_SHOWNOACTIVATE
		//6FA87D86 - 6A 00                 - push 00 { 0 } //SW_HIDE
		installPatch(PatchFILL,DLL_ADDR(d2gfx,0x6FA87D87),4,1);//SW_HIDE
  }*/
return 1;
}

static int installD2PatchesAfterLoad() {
	if (load_dll_error) return 0;
	pPatchList=&installedPatches;
//--- m_pub.h ---
	//分一个给patch2占位置
installPatch(PatchVALUE,DLL_ADDR(d2client,0x6FB2CB25),0x3AEB,2);//hovertextbugfix,setoldfont
//--- m_QuickGame.h ---
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9E4A0B),(DWORD)NextGameNamePatch,5);
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9E4A46),(DWORD)NextGamePasswordPatch,5);
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9E4D29),(DWORD)NextGameNamePatch,5);
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9E4D64),(DWORD)NextGamePasswordPatch,5);
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9E4D8F),(DWORD)NextGameDecriptionPatch,5);
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9E4808),(DWORD)SelectGamePatch,5);
//--- m_GameFilter.h ---
if (fGameFilter) {
installPatch(PatchCALL,DLL_ADDR(d2mcpclient,0x6FA263E0),(DWORD)D2MCPPacketHandler_05_Patch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9E49EF),(DWORD)CreateGameBoxPatch_ASM,5);
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9E1DC3),(DWORD)DestroyGamelistPatch,5);
installPatch(PatchCALL,DLL_ADDR(d2multi,0x6F9DDF4E),(DWORD)GameListRefreshTimePatch_ASM,7);
}
	return 1;
}
#undef d2client_ADDR
#undef d2common_ADDR
#undef d2gfx_ADDR
#undef d2win_ADDR
#undef d2lang_ADDR
#undef d2cmp_ADDR
#undef bnclient_ADDR
#undef d2net_ADDR
#undef d2game_ADDR
#undef d2launch_ADDR
#undef d2gdi_ADDR
#undef d2multi_ADDR
#undef storm_ADDR
#undef fog_ADDR
#undef d2sound_ADDR
#undef d2mcpclient_ADDR
int init_d2_ptrs() {
  addr_storm=get_dll_addr("STORM.DLL");if (!addr_storm) return 0;
  addr_d2client=get_dll_addr("D2CLIENT.DLL"); if (!addr_d2client) return 0;
  addr_d2common=get_dll_addr("D2COMMON.DLL");if (!addr_d2common) return 0;
  addr_d2gfx=get_dll_addr("D2GFX.DLL"); if (!addr_d2gfx) return 0;
  addr_d2win=get_dll_addr("D2WIN.DLL");if (!addr_d2win) return 0;
  addr_d2lang=get_dll_addr("D2LANG.DLL"); if (!addr_d2lang) return 0;
  addr_d2cmp=get_dll_addr("D2CMP.DLL"); if (!addr_d2cmp) return 0;
  addr_bnclient=get_dll_addr("BNCLIENT.DLL"); if (!addr_bnclient) return 0;
  addr_d2net=get_dll_addr("D2NET.DLL"); if (!addr_d2net) return 0;
  addr_fog=get_dll_addr("FOG.DLL"); if (!addr_fog) return 0;
  addr_d2game=get_dll_addr("D2GAME.DLL"); if (!addr_d2game) return 0;
  addr_d2launch=get_dll_addr("D2LAUNCH.DLL");if (!addr_d2launch) return 0;
  addr_d2gdi=get_dll_addr("D2GDI.DLL"); if (!addr_d2gdi) return 0;
  addr_d2sound=get_dll_addr("D2SOUND.DLL");if (!addr_d2sound) return 0;
	if (load_dll_error) return 0;
	#define INIT_D2PTR_ADDR
	#include "d2ptrs.h"
	#undef INIT_D2PTR_ADDR
  addr_d2multi=GetModuleHandle("D2MULTI.DLL");
  addr_d2mcpclient=GetModuleHandle("D2MCPCLIENT.DLL");
	return 1;
}
int init_d2_ptrs2() {
	if (!addr_d2multi) {addr_d2multi=get_dll_addr("D2MULTI.DLL");if (!addr_d2multi) return 0;}
  if (!addr_d2mcpclient) {addr_d2mcpclient=get_dll_addr("D2MCPCLIENT.DLL");if (!addr_d2mcpclient) return 0;}
	if (load_dll_error) return 0;
	#define INIT_D2PTR_ADDR
	#include "d2ptrs2.h"
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
void HightResolution() {
	if (tHighResolution.isOn) {
		if (!resolutionPatches) installResolutionPatches();
	} else {
		if (resolutionPatches) {
			removePatches(&resolutionPatches);
			resolution_uninit();
		}
	}
}
void ResetPacketHandlerTimer();
void MonitorPacket() {
	if (tPacketHandler.isOn) {
		if (!networkPatch) installNetworkRecvPatch();
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
BOOL Install(){
	if (!init_d2_ptrs()) return 0;
	if(!LoadConfig()) return FALSE;
	resolution_init();
  if (!installD2Patches()) {
		MessageBox(NULL,"Error install patches","d2hackmap",MB_OK);
		return 0;
	}
	if (tPacketHandler.isOn) MonitorPacket();
	if (tHighResolution.isOn) HightResolution();
	LOG("Install %d patches done\n",patchCount);
	InitCellFiles();
	LOG("Loading minimap bitmaps done\n");
	if (fAutomapPartyDefault != (BYTE)-1) d2client_SetAutomapParty(fAutomapPartyDefault==0?0:1);
	if (fAutomapNamesDefault != (BYTE)-1) d2client_SetAutomapNames(fAutomapNamesDefault==0?0:1);
	//LPSTR cmdLine = GetCommandLine();
	//HMODULE module = GetModuleHandle(path);
	initSendPacketCheckTable();
	//MessageBox(NULL,"Press to continue","PauseAtStartup",MB_OK);
	if (dwGameLng<0) {
		dwGameLng=GetGameLanguage();
		if (dwGameLng==2) dwGameLng = 1;
	}
	if (*d2launch_pD2Characters) saveCharMenu();
	return TRUE;
}

static int *pReloadFlag=NULL,*pExitFlag=NULL;
extern "C" void __fastcall SetReloadFlag(int *reload_flag,int *exit_flag) {
	pReloadFlag=reload_flag;pExitFlag=exit_flag;
}
void Uninstall(){
	RemoveMyAutomapNodes();
	DeleteCellFiles();
	removePatches(&keyPatches);
	removePatches(&installedPatches);
	removePatches(&networkPatch);
	removePatches(&resolutionPatches);
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
int ReloadConfig() {
	char dllpath[MAX_PATH];
	sprintf(dllpath,"%sruntime\\d2hackmap.dll",szPluginPath);
	if (fileExist(dllpath)) {
		struct stat st1,st2;
		if (stat(dllpath,&st1)==0&&stat(szDllPath,&st2)==0) {
			if (st1.st_mtime>st2.st_mtime||st1.st_size!=st2.st_size) {
				if (fInGame) gameMessage("Dll modified, reload instead\n");
				ReloadDll();
				return 0;
			}
		}
	}
	DeleteCellFiles();
	if (LoadConfig()){
		InitCellFiles();
		if (fInGame) {
			gameMessage("Config Reload Successful");
			ShowWarningMessage();
		}
	}
	fInGame=0;
	return 0;
}
