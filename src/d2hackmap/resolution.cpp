#include "stdafx.h"
#include "header.h"

/* the following code check if equipment location need to be reloaded
6FB408C0 - A1 9CB9BC6F           - mov eax,[6FBCB99C] { (0) }
6FB408C5 - 3B 05 2434BD6F        - cmp eax,[6FBD3424] { (1) }
*/


extern int fNoBackgroundImage;
int fHighRes;
static int fWg800;
static int resW,resH,resH1;
static int resDx,resDy,resDy2;

extern ToggleVar tResizeWindowBar;
ToggleVar tHighResolution={TOGGLEVAR_ONOFF,1,-1,1,"Change Resolution"};
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY, "HighResolutionToggle",&tHighResolution},
};
void Resolution_AddConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}

void changeResolution(int res) {
	__asm {
		mov esi, res
		call d2client_SetResolutionMode
	}
}
void resolution_init() {fHighRes=0;resW=800;resH=600;resH1=resH-1;resDx=0;resDy=0;resDy2=0;}
void resolution_uninit() {
	if (resW!=800||resH!=600) {
		resolution_init();
		if (*d2gfx_pResolutionMode==2) *d2gfx_pResolutionMode=1;
		changeResolution(2);
	}
}

void fitWindowToResolution() {
  RECT rect,drect,crect;
	HWND hwnd=d2gfx_GetHwnd();if (!hwnd) return;
	GetClientRect(hwnd, &crect); 
	HWND dhwnd=GetDesktopWindow();GetClientRect(dhwnd, &drect); 
	int w=crect.right-crect.left;
	int h=crect.bottom-crect.top;
	int dw=drect.right-drect.left;
	int dh=drect.bottom-drect.top;
	if (*d2client_pScreenWidth==crect.right&&*d2client_pScreenHeight==crect.bottom) return;
	int r=AdjustWindowRectEx(&rect,GetWindowLongA(hwnd,GWL_STYLE),0,WS_EX_APPWINDOW);
	int nw=rect.right-rect.left;
	int nh=rect.bottom-rect.top;
	if (nw>dw) nw=dw;if (nh>dh) nh=dh;
	SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,nw,nh,
		SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE);
	if (tShowTestInfo.isOn) LOG("fitWindow %d %d\n",rect.right-rect.left,rect.bottom-rect.top);
}
void fitResolution() {
  RECT rect,crect;
	if (!fInGame) return;
	HWND hwnd=d2gfx_GetHwnd();if (!hwnd) return;
	GetWindowRect(hwnd, &rect); 
	GetClientRect(hwnd, &crect); 
	int w=crect.right-crect.left;
	int h=crect.bottom-crect.top;
	//WINDOWPLACEMENT wp;if (GetWindowPlacement(hwnd,&wp)&&wp.showCmd==SW_SHOWMAXIMIZED) {w=1280;h=1924;}
	w&=0xFFFFFFFC;
	if (w<800) w=800;if (h<480) h=480;
	int change=d2gfx_pResolutionMode&&*d2gfx_pResolutionMode==2&&(*d2client_pScreenWidth!=w||*d2client_pScreenHeight!=h);
	if (tShowTestInfo.isOn)
		LOG("WindowRect(%d,%d,%d,%d) client(%d,%d,%d,%d) resolution(%d,%d) change=%d\n",
			rect.left,rect.top,rect.right,rect.bottom,
			crect.left,crect.top,crect.right,crect.bottom,w,h,change);
	if (!change) {
		if (w!=crect.right||h!=crect.bottom) fitWindowToResolution();
		return;
	}
	resW=w;resH=h;resH1=resH-1;
	resDx=(resW-800)>>1;resDy=(resH-600)>>1;resDy2=resH-600;
	fWg800=resW>800;
	fHighRes=resW!=800||resH!=600;
	if (*d2gfx_pResolutionMode==2) *d2gfx_pResolutionMode=1;
	changeResolution(2);
	*d2client_pIsEquipment800x600=!(*d2client_pIs800x600);
	wchar_t wszbuf[256];
	GetClientRect(hwnd, &crect); 
	wsprintfW(wszbuf, L"framebuf: %d*%d client: %d*%d",*d2client_pScreenWidth,*d2client_pScreenHeight,
		crect.right-crect.left,crect.bottom-crect.top);
	setBottomAlertMsg(2,wszbuf,3000,0,2,0);
}
//D2Gdi 6F876D3D - BE 20030000           - mov esi,00000320 { 800 }  eax!=0&&eax!=3 800*600
//6F876D42 - BA 58020000           - mov edx,00000258 { 600 }
void __declspec(naked) SetBitBlockWidthAndHeightPatch_ASM() {
	__asm {
		mov esi, resW
		mov edx, resH
		ret
	}
}
//D2Gdi 6F877051 - C7 05 9CCA876F 20030000 - mov [6F87CA9C],00000320 { (800),800 }
void __declspec(naked) SetCelDisplayLeftAndRightPatch_ASM() {
	__asm {
		push eax
		push ecx
		mov ecx, resW
		mov eax, d2gdi_pDisplayRight
		mov dword ptr [eax], ecx
		pop ecx
		pop eax
		ret
	}
}
//D2GFX 6FA883F0 - 3D 20030000           - cmp eax,00000320 { 800 }
void __declspec(naked) IsNeedResizeWindowPatchW_ASM() {
	__asm {
		push ecx
		mov ecx, resW
		cmp eax, ecx
		pop ecx
		ret
	}
}
//D2GFX 6FA88403 - 81 F9 58020000        - cmp ecx,00000258 { 600 }
void __declspec(naked) IsNeedResizeWindowPatchH_ASM() {
	__asm {
		push eax
		mov eax, resH
		cmp ecx, eax
		pop eax
		ret
	}
}
//D2gfx 6FA88141 - 3D 20030000           - cmp eax,00000320 { 800 }
void __declspec(naked) IsNeedRestoreDownWindowPatchW_ASM() {
	__asm {
		push ecx
		mov ecx, resW
		cmp eax, ecx
		pop ecx
		ret
	}
}
//D2GFX 6FA88154 - 81 F9 58020000        - cmp ecx,00000258 { 600 }
void __declspec(naked) IsNeedRestoreDownWindowPatchH_ASM() {
	__asm {
		push eax
		mov eax, resH
		cmp ecx, eax
		pop eax
		ret
	}
}
//D2GFX 6FA87FFF - C7 02 20030000        - mov [edx],00000320 { 800 }
//D2GFX 6FA88005 - C7 00 58020000        - mov [eax],00000258 { 600 }
void __declspec(naked) SetDisplayWidthAndHeightPatch_ASM() {
	__asm {
		push ecx
		mov ecx, resW
		mov [edx], ecx
		mov ecx, resH
		mov [eax], ecx
		pop ecx
		ret
	}
}

/* D2CLIENT
6FAC0E09 - B8 20030000           - mov eax,00000320 { 800 } <--- mode 2
6FAC0E0E - A3 48BCB86F           - mov [6FB8BC48],eax  //ScreenWidth
6FAC0E13 - C7 05 4CBCB86F 58020000 - mov [6FB8BC4C],00000258 {600 } <--- ScreenHeight install here
6FAC0E1D - C7 05 9CB9BC6F 01000000 - mov [6FBCB99C],00000001 { (0),1 }
6FAC0E27 - EB 25                 - jmp 6FAC0E4E
...
6FAC0E4E - 8B 0D 4CBCB86F        - mov ecx,[6FB8BC4C] { (480) }
6FAC0E54 - 83 C1 D8              - add ecx,-28 { 216 }
6FAC0E57 - 56                    - push esi
6FAC0E58 - A3 149EBA6F           - mov [6FBA9E14],eax { (640) }
6FAC0E5D - 89 0D 189EBA6F        - mov [6FBA9E18],ecx { (440) }
6FAC0E63 - A3 50BCB86F           - mov [6FB8BC50],eax { (640) }
6FAC0E68 - E8 BFC5FFFF           - call 6FABD42C { ->->6F8F7B60 }
eax need to be width
ecx can be used
*/
void __declspec(naked) SetGeneralDisplayWidthAndHeightPatch_ASM() {
	__asm {
		mov ecx, d2client_pScreenHeight
		mov eax, resH
		mov [ecx], eax
		mov ecx, d2client_pScreenWidth
		mov eax, resW
		mov [ecx], eax
		ret
	}
}
static void __fastcall getGlobalBeltSlotPosition(int *src, int *dst) {
	if (!fHighRes) return;
	if (src[0]<0||src[1]<0||src[2]<0||src[3]<0) return;
	dst[0]+=resDx;
	dst[1]+=resDx;
	dst[2]+=resDy2;
	dst[3]+=resDy2;
}
/*
4 times each frame
6FD63397 - 8B 08                 - mov ecx,[eax]
6FD63399 - 8B 54 24 0C           - mov edx,[esp+0C]
6FD6339D - 89 0A                 - mov [edx],ecx
6FD6339F - 8B 48 04              - mov ecx,[eax+04]
6FD633A2 - 89 4A 04              - mov [edx+04],ecx
6FD633A5 - 8B 48 08              - mov ecx,[eax+08]
6FD633A8 - 89 4A 08              - mov [edx+08],ecx
6FD633AB - 8B 40 0C              - mov eax,[eax+0C] <--- install here
6FD633AE - 89 42 0C              - mov [edx+0C],eax
6FD633B1 - C2 1000               - ret 0010 { 16 }
values: 423,452,562,591
*/
void __declspec(naked) GetGlobalBeltSlotPositionPatch_ASM() {
	__asm {
		pushad
		mov ecx, [eax+0xC]
		mov [edx+0xC], ecx
		mov ecx,eax
		call getGlobalBeltSlotPosition
		popad
		mov eax,[edx+0xC]
		ret
	}
}
static void __fastcall getGlobalBeltRecord(int *dst) {
	if (!fHighRes) return;
	int n=dst[1];
	int *end=dst+2+n*4;
	for (int *p=dst+2;p<end;p+=4) {
		if (!p[2]) break;
		p[0]+=resDx;p[1]+=resDx;p[2]+=resDy2;p[3]+=resDy2;
	}
}
/*
call once each frame
6fd632a0 D2Common.dll/Ordinal10991 160
...
6FD632EC - B9 42000000           - mov ecx,00000042 { 66 } <---
6FD632F1 - F3 A5                 - repe movsd 
6FD632F3 - 5F                    - pop edi
6FD632F4 - 5E                    - pop esi
6FD632F5 - C2 0C00               - ret 000C { 12 }
*/
void __declspec(naked) GetGlobalBeltRecordPatch_ASM() {
	__asm {
		push ebx
		mov edx,edi
		mov ecx, 0x42
		repe movsd
		mov ecx, edx
		call getGlobalBeltRecord
		pop ebx
		ret
	}
}
/*
6FA8842C - 68 00000400           - push 00040000 //WS_EX_APPWINDOW
6FA88431 - 56                    - push esi <--- menu
6FA88432 - 68 0000CB00           - push 00CB0000  <---style
	WS_CAPTION:00C00000 WS_SYSMENU:00080000 WS_GROUP:00020000 WS_MAXIMIZEBOX:00010000 WS_THICKFRAME:00040000
6FA88437 - 8D 54 24 1C           - lea edx,[esp+1C]
6FA8843B - 52                    - push edx <--- pRect
6FA8843C - 89 0D 80D5A96F        - mov [6FA9D580],ecx { (600) }
6FA88442 - FF 15 5CD1A86F        - call dword ptr [6FA8D15C] { ->77D2E7EA USER32.AdjustWindowRectEx}
6FA88442 - E8 29F059A0           - call 10027470 d2hackmap.AdjustWindowRectEx_Patch
6FA88447 - 90                    - nop 
...
6FA88490 - 5F                    - pop edi <--- jump to here
	BOOL AdjustWindowRectEx([in, out] LPRECT lpRect,[in]DWORD  dwStyle,[in]BOOL bMenu,[in]DWORD dwExStyle);
*/
static int __stdcall adjWinRectEx(int retAddr,RECT *pRect,int style,int menu,int exStyle) {
	RECT crect;
	HWND hwnd=d2gfx_GetHwnd();if (!hwnd) return retAddr+0x49;
	if (tResizeWindowBar.isOn) {
		int style=GetWindowLongA(hwnd,GWL_STYLE);
		if (!(style&WS_THICKFRAME)) {
			style|=WS_THICKFRAME;
			SetWindowLongA(hwnd,GWL_STYLE,style);
		}
	}
	WINDOWPLACEMENT wp;
	if (GetWindowPlacement(hwnd,&wp)&&wp.showCmd==SW_SHOWMAXIMIZED) goto noresize;
	GetClientRect(hwnd, &crect); 
	int w=crect.right-crect.left;
	int h=crect.bottom-crect.top;
	if (w==pRect->right&&h==pRect->bottom) goto noresize;
	int r=AdjustWindowRectEx(pRect,GetWindowLongA(hwnd,GWL_STYLE),menu,exStyle);
	if (tShowTestInfo.isOn) LOG("AdjustWindow %d %d %d %d\n",pRect->left,pRect->top,pRect->right,pRect->bottom);
	return retAddr;
noresize:
	return retAddr+0x49;
}
void __declspec(naked) AdjustWindowRectExPatch_ASM() {
	__asm {
		call adjWinRectEx
		jmp eax
	}
}
/* D2gdi
	6F8771C5 - 8B 0D 90CA876F        - mov ecx,[6F87CA90] { (600) }
	6F8771CB - 83 C1 D1              - add ecx,-2F { 209 }
*/
void __declspec(naked) ClearFramebufPatch_ASM() {
	__asm {
		mov ecx,d2gdi_pFrameBufHeight
		mov ecx,[ecx]
		cmp fWg800,0
		jne clearall
		cmp fNoBackgroundImage, 0
		jne clearall
		sub ecx, 47
clearall:
		ret
	}
}
//6FAD7459 - E8 385CFEFF           - call 6FABD096 { ->->6FA8B080 D2gfx.Ordinal10041} <---draw left orb
//6FAD7D45 - E8 4C53FEFF           - call 6FABD096 { ->->D2gfx.Ordinal10041 } //draw left orb circle
//(CellContext *context, DWORD xPos, DWORD yPos, DWORD dw1, DWORD dwTransLvl, BYTE *coltab))
void __declspec(naked) DrawLeftOrbImagePatch_ASM() {
	__asm {
		cmp fNoBackgroundImage, 0
		jne skip
		cmp fWg800, 0
		je original
		mov eax, [esp+8]
		add eax, resDx
		mov [esp+8], eax
original:
		jmp d2gfx_DrawCellFile
		ret
skip:
		ret 24
	}
}
void __declspec(naked) DrawLeftOrbPatch_ASM() {
	__asm {
		cmp fWg800, 0
		je original
		mov eax, [esp+8]
		add eax, resDx
		mov [esp+8], eax
original:
		jmp d2gfx_DrawCellFile
		ret
	}
}
//6FAD7536 - E8 5B5BFEFF           - call 6FABD096 { ->->6FA8B080 D2gfx.Ordinal10041} //draw right orb
//6FAD7BBE - E8 D354FEFF           - call 6FABD096 { ->->D2gfx.Ordinal10041 } //draw right orb circle
void __declspec(naked) DrawRightOrbImagePatch_ASM() {
	__asm {
		cmp fNoBackgroundImage, 0
		jne skip
		cmp fWg800, 0
		je original
		mov eax, [esp+8]
		sub eax, resDx
		mov [esp+8], eax
original:
		jmp d2gfx_DrawCellFile
		ret
skip:
		ret 24
	}
}
void __declspec(naked) DrawRightOrbPatch_ASM() {
	__asm {
		cmp fWg800, 0
		je original
		mov eax, [esp+8]
		sub eax, resDx
		mov [esp+8], eax
original:
		jmp d2gfx_DrawCellFile
		ret
	}
}
//args: 1244596 x:29 y:613 0 80 5 0 569
//6FAD7D18 - E8 9954FEFF           - call 6FABD1B6 { ->->D2gfx.Ordinal10074 } <--- filled orb 6 args
void __declspec(naked) FillLeftOrbPatch_ASM() {
	__asm {
		cmp fWg800, 0
		je original
		mov eax, [esp+8]
		add eax, resDx
		mov [esp+8], eax
original:
		jmp d2gfx_FillOrb
		ret
	}
}
//6FAD7B8D - E8 2456FEFF           - call 6FABD1B6 { ->->D2gfx.Ordinal10074 }
void __declspec(naked) FillRightOrbPatch_ASM() {
	__asm {
		cmp fWg800, 0
		je original
		mov eax, [esp+8]
		sub eax, resDx
		mov [esp+8], eax
original:
		jmp d2gfx_FillOrb
		ret
	}
}
//6FAD8D49 - E8 72FEFFFF           - call 6FAD8BC0 //draw left skill button
void __declspec(naked) DrawLeftSkillButtonPatch_ASM() {
	__asm {
		cmp fWg800, 0
		je original
		mov ecx, [esp+8]
		add ecx, resDx
		mov [esp+8], ecx
original:
		jmp d2client_DrawSkillButton
	}
}
//6FAD8DB3 - E8 08FEFFFF           - call 6FAD8BC0 //draw right skill button
void __declspec(naked) DrawRightSkillButtonPatch_ASM() {
	__asm {
		cmp fWg800, 0
		je original
		mov ecx, [esp+8]
		sub ecx, resDx
		mov [esp+8], ecx
original:
		jmp d2client_DrawSkillButton
	}
}
//6FAD7703 - E8 765CFEFF           - call 6FABD37E { ->->D2Win.Ordinal10150 } //display HP text
void __declspec(naked) DisplayHPTextPatch_ASM() {
	__asm {
		cmp fWg800, 0
		je original
		add edx, resDx
original:
		jmp d2win_DrawText
	}
}
//6FAD77C2 - E8 B75BFEFF           - call 6FABD37E { ->->D2Win.Ordinal10150 } //display mana text
void __declspec(naked) DisplayManaTextPatch_ASM() {
	__asm {
		cmp fWg800, 0
		je original
		sub edx, resDx
original:
		jmp d2win_DrawText
	}
}

/*
6FA889CD - 89 35 68D4A96F        - mov [6FA9D468],esi { (553) }
6FA889D3 - 89 35 B848A96F        - mov [6FA948B8],esi { (553) } <-can't modify
6FA889D9 - 89 35 CC48A96F        - mov [6FA948CC],esi { (553) } <-install here
*/
void __declspec(naked) DrawGroundHeightPatch_ASM() {
	__asm {
		cmp fWg800, 0
		jne drawall
		cmp fNoBackgroundImage, 0
		jne drawall
		push eax
		mov eax, d2gfx_pDrawGoundH2
		mov [eax], esi
		pop eax
		ret
drawall:
		push eax
		push ecx
		mov ecx, resH1
		//mov eax, d2gfx_pDrawGoundH1
		//mov [eax], ecx
		mov eax, d2gfx_pDrawGoundH2
		mov [eax], ecx
		pop ecx
		pop eax
		ret
	}
}
//6FA8A9EF - 3B 35 68D4A96F        - cmp esi,[6FA9D468] { (553) } //draw a piece of ground or not
/*void __declspec(naked) DrawBottomGroundPiecePatch_ASM() {
	__asm {
		cmp esi, resH1
		ret
	}
}
*/
//6FB3A4F8 - A1 48BCB86F           - mov eax,[6FB8BC48] { (640) } //ScreenWidth
//6FB3A4FD - 83 C2 D1              - add edx,-2F { 209 } //ScreenHeight-47
void __declspec(naked) BottomBoundPatch_ASM() {
	__asm {
		mov eax,d2client_pScreenWidth
		mov eax,[eax]
		cmp fWg800, 0
		jne drawall
		cmp fNoBackgroundImage, 0
		jne drawall
		sub edx,47
		ret
drawall:
		dec edx
		ret
	}
}
int installResolutionPatches() {
//d2gdi_6D42: BA 58 02 00 00     mov edx, 0x258 (600)
PatchCall(d2gdi,0x6D42,SetBitBlockWidthAndHeightPatch_ASM,5,"BA 58 02 00 00");
//d2gdi_7051: C7 05 9C CA 87 6F 20 03 00 00  mov dword ptr [d2gdi_CA9C](->320), 0x320 (800)
PatchCall(d2gdi,0x7051,SetCelDisplayLeftAndRightPatch_ASM,10,"C7 05 $(+CA9C) 20 03 00 00");
//d2gdi_71C5: 8B 0D 90 CA 87 6F  mov ecx, [d2gdi_CA90](->258)
//d2gdi_71CB: 83 C1 D1           add ecx, -0x2F
PatchCall(d2gdi,0x71C5,ClearFramebufPatch_ASM,9,"8B 0D $(+CA90) 83 C1 D1");
//d2gfx_8141: 3D 20 03 00 00     cmp eax, 0x320 (800)
PatchCall(d2gfx,0x8141,IsNeedRestoreDownWindowPatchW_ASM,5,"3D 20 03 00 00");
//d2gfx_8154: 81 F9 58 02 00 00  cmp ecx, 0x258 (600)
PatchCall(d2gfx,0x8154,IsNeedRestoreDownWindowPatchH_ASM,6,"81 F9 58 02 00 00");
//d2gfx_83F0: 3D 20 03 00 00     cmp eax, 0x320 (800)
PatchCall(d2gfx,0x83F0,IsNeedResizeWindowPatchW_ASM,5,"3D 20 03 00 00");
//d2gfx_8403: 81 F9 58 02 00 00  cmp ecx, 0x258 (600)
PatchCall(d2gfx,0x8403,IsNeedResizeWindowPatchH_ASM,6,"81 F9 58 02 00 00");
//d2gfx_8442: FF 15 5C D1 A8 6F  call dword ptr [d2gfx_D15C](->user32_1E7EA user32/AdjustWindowRectEx)
PatchCall(d2gfx,0x8442,AdjustWindowRectExPatch_ASM,6,"FF 15 $(+D15C)");
//d2gfx_8005: C7 00 58 02 00 00  mov dword ptr [eax], 0x258 (600)
PatchCall(d2gfx,0x8005,SetDisplayWidthAndHeightPatch_ASM,6,"C7 00 58 02 00 00");
//patchCall(PATCH_ADDR(d2gfx,0xA9EF),(DWORD)DrawBottomGroundPiecePatch_ASM,6,0xD468353B,0x8D0F6FA9);
//d2gfx_A9EF: 3B 35 68 D4 A9 6F  cmp esi, [d2gfx_1D468 int d2gfx_DrawGoundH1](->229)
patchValue("draw ground y",PATCH_ADDR(d2gfx,0xA9F1),(DWORD)d2client_pScreenHeight,4,"-2 3B 35 $(+1D468)");
//d2gfx_89D9: 89 35 CC 48 A9 6F  mov [d2gfx_148CC](->229), esi
PatchCall(d2gfx,0x89D9,DrawGroundHeightPatch_ASM,6,"89 35 $(+148CC)");
//d2client_10E13: C7 05 4C BC B8 6F 58 02 00 00  mov dword ptr [d2client_DBC4C](->258), 0x258 (600)
PatchCall(d2client,0x10E13,SetGeneralDisplayWidthAndHeightPatch_ASM,10,"C7 05 $(+DBC4C) 58 02 00 00");
//d2client_27459: E8 38 5C FE FF     call d2client_D096->d2gfx_B080 d2gfx/Ordinal10041
PatchCall(d2client,0x27459,DrawLeftOrbImagePatch_ASM,5,"E8 38 5C FE FF");
//d2client_27D45: E8 4C 53 FE FF     call d2client_D096->d2gfx_B080 d2gfx/Ordinal10041
PatchCall(d2client,0x27D45,DrawLeftOrbPatch_ASM,5,"E8 4C 53 FE FF");
//d2client_27536: E8 5B 5B FE FF     call d2client_D096->d2gfx_B080 d2gfx/Ordinal10041
PatchCall(d2client,0x27536,DrawRightOrbImagePatch_ASM,5,"E8 5B 5B FE FF");
//d2client_27BBE: E8 D3 54 FE FF     call d2client_D096->d2gfx_B080 d2gfx/Ordinal10041
PatchCall(d2client,0x27BBE,DrawRightOrbPatch_ASM,5,"E8 D3 54 FE FF");
//d2client_27703: E8 76 5C FE FF     call d2client_D37E->d2win_12FA0 d2win/Ordinal10150
PatchCall(d2client,0x27703,DisplayHPTextPatch_ASM,5,"E8 76 5C FE FF");
//d2client_277C2: E8 B7 5B FE FF     call d2client_D37E->d2win_12FA0 d2win/Ordinal10150
PatchCall(d2client,0x277C2,DisplayManaTextPatch_ASM,5,"E8 B7 5B FE FF");
//d2client_27D18: E8 99 54 FE FF     call d2client_D1B6->d2gfx_B020 d2gfx/Ordinal10074
PatchCall(d2client,0x27D18,FillLeftOrbPatch_ASM,5,"E8 99 54 FE FF");
//d2client_27B8D: E8 24 56 FE FF     call d2client_D1B6->d2gfx_B020 d2gfx/Ordinal10074
PatchCall(d2client,0x27B8D,FillRightOrbPatch_ASM,5,"E8 24 56 FE FF");
//d2client_28D49: E8 72 FE FF FF     call d2client_28BC0
PatchCall(d2client,0x28D49,DrawLeftSkillButtonPatch_ASM,5,"E8 72 FE FF FF");
//d2client_28DB3: E8 08 FE FF FF     call d2client_28BC0
PatchCall(d2client,0x28DB3,DrawRightSkillButtonPatch_ASM,5,"E8 08 FE FF FF");
//d2client_8A4F8: A1 48 BC B8 6F     mov eax, [d2client_DBC48](->320)
//d2client_8A4FD: 83 C2 D1           add edx, -0x2F
PatchCall(d2client,0x8A4F8,BottomBoundPatch_ASM,8,"A1 $(+DBC48) 83 C2 D1");
//patchFill(PATCH_ADDR(d2client,0xBCB30),INST_NOP,10,0xF50579E8,0x2F883FF);//clickifmode!=2
//patchFill(PATCH_ADDR(d2client,0x8F880),INST_NOP,10,0xF7D829E8,0x2F883FF);
//d2common_133AB: 8B 40 0C           mov eax, [eax+0xC]
//d2common_133AE: 89 42 0C           mov [edx+0xC], eax
PatchCall(d2common,0x133AB,GetGlobalBeltSlotPositionPatch_ASM,6,"8B 40 0C 89 42 0C");
//d2common_132EC: B9 42 00 00 00     mov ecx, 0x42 (66)
//d2common_132F1: F3 A5              rep movs d
PatchCall(d2common,0x132EC,GetGlobalBeltRecordPatch_ASM,7,"B9 42 00 00 00 F3 A5");
  return 1;
}
