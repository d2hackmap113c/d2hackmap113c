#include "stdafx.h"
#include "header.h"

void __declspec(naked) __fastcall d2client_SetAutomapParty(DWORD flag) {
	__asm {
		push esi
		mov esi, ecx
		call d2client_SetAutomapPartyStub
		pop esi
		ret
	}
}
void __declspec(naked) __fastcall d2client_SetAutomapNames(DWORD flag) {
	__asm {
		push esi
		mov esi, ecx
		call d2client_SetAutomapNamesStub
		pop esi
		ret
	}
}
//聊天消息
DWORD __declspec(naked) __fastcall d2client_ChatInput(D2MSG *pmsg) {
	__asm {
		mov eax, 0xF
		push ecx
		call d2client_ChatInputStub
		ret
	}
}
int __declspec(naked) __fastcall d2client_CheckUiStatus(int dwUiNo) {
	__asm {
		mov eax, ecx
		call d2client_CheckUiStatusStub
		ret 
	}
}
__declspec(naked) AutomapLayer* __fastcall d2client_InitAutomapLayer(int nLayerNo) {
	__asm {
		mov eax, ecx
		call d2client_InitAutomapLayerStub
		ret 
	}
}
__declspec(naked) UniqueItemTxt*  __fastcall d2common_GetUniqueItemTxt(int dwIdx) {
	__asm {
		mov eax, ecx
		mov ecx, [d2common_pDataTables]
		mov ecx , [ecx]
		cmp eax, dword ptr [ecx+0xC28]
		jge unkret
		mov edx, dword ptr [ecx+0xC24]
		imul eax, 0x14C
		add eax, edx
		ret
unkret:
		xor eax , eax
		ret 
	}
}
__declspec(naked) ItemStatCostTxt* __fastcall d2common_GetItemStatCostTxt(int dwStatNo) {
	__asm {
		mov eax, ecx
		mov ecx, [d2common_pDataTables]
		mov ecx , [ecx]
		cmp eax, dword ptr [ecx+0xBD4]
		jge unkret
		mov edx, dword ptr [ecx+0xBCC]
		imul eax, 0x144
		add eax, edx
		ret
unkret:
		xor eax , eax
		ret 
	}
}
__declspec(naked) PropertiesTxt* __fastcall d2common_GetPropertiesTxt(int dwPropNo) {
	__asm {
		mov eax, ecx
		mov ecx, [d2common_pDataTables]
		mov ecx , [ecx]
		cmp eax, dword ptr [ecx+0xAC]
		jge unkret
		mov edx, dword ptr [ecx+0xA4]
		imul eax, 0x2E
		add eax, edx
		ret
unkret:
		xor eax , eax
		ret 
	}
}
__declspec(naked) SetItemTxt* __fastcall d2common_GetSetItemTxt(int dwIdx) {
	__asm {
			mov eax, ecx
			mov ecx, [d2common_pDataTables]
			mov ecx , [ecx]
			cmp eax, dword ptr [ecx+0xC1C]
			jge unkret
			mov edx, dword ptr [ecx+0xC18]
			imul eax, 0x1B8
			add eax, edx
			ret
	unkret:
			xor eax , eax
			ret 
	}
}
__declspec(naked) ItemTypeTxt* __fastcall d2common_GetItemTypeTxt(int dwIdx) {
	__asm {
			mov eax, ecx
			mov ecx, [d2common_pDataTables]
			mov ecx , [ecx]
			cmp eax, dword ptr [ecx+0xBFC]
			jge unkret
			mov edx, dword ptr [ecx+0xBF8]
			imul eax, 0xE4
			add eax, edx
			ret
	unkret:
			xor eax , eax
			ret 
	}
}
__declspec(naked) RuneWordTxt* __fastcall d2common_GetRuneWordTxt(int dwIdx) {
	__asm {
			mov eax, ecx

			mov ecx, [d2common_pRuneWords]
			mov ecx , [ecx]

			cmp eax, ecx
			jge unkret
			mov edx ,[d2common_pRuneWordTxt]
			mov edx ,[edx]
			imul eax, 0x120
			add eax, edx
			ret
	unkret:
			xor eax , eax
			ret 
	}
}
int __declspec(naked) __fastcall d2common_GetRuneWordTxtIndex(int dwFileNo) {
	__asm {
			push ebx
			push ebp
			mov ebx , ecx
			
			mov ecx, [d2common_pRuneWords]
			mov ecx , [ecx]

			mov edx ,[d2common_pRuneWordTxt]
			mov edx ,[edx]

			xor ebp ,ebp
myloop:		
			mov eax , ebp
			cmp eax, ecx
			jge unkret
			imul eax, 0x120
			add eax, edx
			
			inc ebp
			cmp bx , word ptr [eax+0x82]
			jne myloop

			mov eax , ebp
			dec eax
			pop ebp
			pop ebx
			ret
	unkret:
			xor eax , eax
			pop ebp
			pop ebx
			ret 
	}
}
__declspec(naked) CellFile* __fastcall d2client_LoadUiImage(char* szPath) {
	__asm {
		mov eax, ecx
		push 0
		call d2client_LoadUiImageFunc
		retn
	}
}


