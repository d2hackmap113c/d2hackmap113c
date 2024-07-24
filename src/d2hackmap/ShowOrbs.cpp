#include "stdafx.h"

#ifdef MODULE_SHOWORBS

char *szOrbPattern = "%d/%d (%d%%)";
DWORD __stdcall ShowLifeWithMyPattern(DWORD callBack , int min ,int max){
	
	wchar_t wszTemp[64];
	int iPercent =  100*min/max ;
	wsprintfW2(wszTemp	,szOrbPattern , min,	max,	iPercent);
	DrawDefaultFontText(wszTemp ,	75 ,  SCREENSIZE.y-95 , 0  );
	return callBack;

}

DWORD __stdcall ShowManaWithMyPattern(DWORD callBack , int min ,int max){
	
	wchar_t wszTemp[64];
	int iPercent =  100*min/max ;
	wsprintfW2(wszTemp	,szOrbPattern , min,	max,	iPercent);
	DrawDefaultFontText(wszTemp,	SCREENSIZE.x-65 ,	SCREENSIZE.y-95,	0);
	return callBack;

}

void __declspec(naked) ShowLifePatch_ASM()
{
	__asm {
		cmp [tPermShowOrb.value32],2
		je showme
		mov ecx, 0x00001045
		ret
showme:
		call ShowLifeWithMyPattern
		push eax
		add dword ptr [esp], 0x4E
		ret
	}
}

void __declspec(naked) ShowManaPatch_ASM()
{
	__asm {
		cmp [tPermShowOrb.value32],2
		je showme
		mov ecx, 0x00001046
		ret
showme:
		call ShowManaWithMyPattern
		push eax
		add dword ptr [esp], 0x5B
		ret
	}
}


int SetLifeManaShow(){
	if ( tPermShowOrb.isOn ){
		*p_D2ShowLifeStr = 1;
		*p_D2ShowManaStr = 1;
	}else{
		*p_D2ShowLifeStr = 0;
		*p_D2ShowManaStr = 0;
	}
	return 0;
}
#endif
