#include "stdafx.h"

#ifdef MODULE_GAMEWINDOW

BOOL fKeepWindowPatch = FALSE;
static Patch_t aKeepGameWindowPatchs[] = {
    {PatchFILL,   DLLOFFSET(D2GFX, 0x6FA88530),     4,    1,    &fDefault}, // SW_MINIMIZE
    {PatchFILL,   DLLOFFSET(D2GFX, 0x6FA87D87),     4,    1,    &fDefault}, // SW_HIDE
};


BOOL IsFullWindow()
{
  WINDOWINFO wi;
  HWND hwnd = D2GetHwnd();
  if (hwnd && GetWindowInfo(hwnd, &wi)) {
    if ( (wi.dwStyle & WS_CAPTION) && (wi.dwExStyle& WS_EX_WINDOWEDGE) ) {
      return FALSE;
    }
  }
  else {
    LPSTR pszCmdLine = GetCommandLine();
    LPSTR p;
    for ( p = pszCmdLine; (p=strchr(p, ' ')) != NULL; p++) {
      if ((p[1] == '-') &&
        (p[2] == 'w' || p[2] == 'W') &&
        (p[3] == ' ' || p[3] == '\0')) {
        return FALSE;
      }
    }
  }
  return TRUE;
}


int KeepWindowPatch() {
  if( fKeepWindowPatch ){
    RemoveD2Patchs(aKeepGameWindowPatchs, aKeepGameWindowPatchs+_ARRAYSIZE(aKeepGameWindowPatchs));
    fKeepWindowPatch = 0;
  }
  if (tKeepGameWindow.isOn && !fFullWindows){
    InstallD2Patchs(aKeepGameWindowPatchs, aKeepGameWindowPatchs+_ARRAYSIZE(aKeepGameWindowPatchs));
    fKeepWindowPatch = 1;
  }
	return 0;
}


void RemoveKeepWindowPatch(){

	if( fKeepWindowPatch ){
		RemoveD2Patchs(aKeepGameWindowPatchs, aKeepGameWindowPatchs+_ARRAYSIZE(aKeepGameWindowPatchs));
		fKeepWindowPatch = 0;
	  }
  
}

void __declspec(naked) NoHidePatch_ASM()
{
	__asm{
		jne hidewin
		cmp [fFullWindows] , 0
		jne justret
		cmp [tNoHide.isOn] , 1
		je justret
hidewin: 
		xor ecx, ecx
		call vD2ToggleSound
		push 1
		call D2MinimizeWindows
justret:
		ret 
	}
}

#endif
