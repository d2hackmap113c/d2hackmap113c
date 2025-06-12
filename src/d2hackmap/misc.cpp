#include "stdafx.h"
#include "header.h"

int	mSkipQuestMessage=0;
ToggleVar 	tOutTownSelect={				TOGGLEVAR_ONOFF,	1,	-1,	1,	"Out of town select"};
ToggleVar 	tULCMask={				TOGGLEVAR_ONOFF,	0,	-1,		1	,"ULC Mask Toggle" };

static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY, "OutTownSelectToggle",    &tOutTownSelect     },
	{CONFIG_VAR_TYPE_INT, "SkipQuestMessage",   &mSkipQuestMessage,   4},
	{CONFIG_VAR_TYPE_KEY, "ULCMaskToggle",	 &tULCMask       },
};

void misc_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void __declspec(naked) ULCPatch_ASM() {
		__asm {
			cmp [tULCMask.isOn],0
			je			org_code
			movzx		ecx,byte ptr [edi+6]
			cmp			ecx,197
			jne			org_code
			mov			byte ptr [edi+6],0	; modify the firestorm effect to normal attack effect
org_code:
			movzx   edx, byte ptr [edi+8]
			xor     ecx, ecx
			ret;
	}
}

int __fastcall OutTownSelectPatch(UnitAny *pUnit) {
	if ( tOutTownSelect.isOn==0 && fPlayerInTown==FALSE )return -1;
	if (CheckUnitSelect(pUnit)) return 1;
	return -1;
}

void __declspec(naked) OutTownSelectPatch_ASM1() {
	__asm {
		shr eax, 0x15
		and eax, 1
		jnz donothing
		mov ecx, ebp
		call OutTownSelectPatch;
		cmp al, 0xFF
		jz quitcode
		add dword ptr [esp], 0xA2 // adjust return address
		mov [esp+4+0x34], eax // save return result
		pop eax // return address to eax
		push esi // push esi and edi because the return address skips these two instructions
		push edi
		jmp eax
donothing:
		ret
quitcode:
		xor eax, eax
		ret
	}
}

//技能点击玩家会跑过去，这里修正

int getUnitOwnerId(UnitAny *pUnit);
BOOL __stdcall CheckOutTownSelected( UnitAny *pUnit )
{
	if ( tOutTownSelect.isOn ==0 ) return FALSE;
	if ( fPlayerInTown ) return FALSE;
	if ( pUnit ){
		if ( pUnit->dwUnitType == UNITNO_PLAYER ) return TRUE;
		if ( pUnit->dwUnitType==UNITNO_MONSTER && getUnitOwnerId(pUnit)!= (DWORD)-1 ) return TRUE;
	}
	return FALSE;
}

void __declspec(naked) OutTownSelectPatch_ASM2()
{
	__asm {
// ebx pUnit
		jnz orgcontinue
//check
		push esi
		push edi

		push ebx
		call CheckOutTownSelected;	
		test eax, eax

		pop edi
		pop esi
		jnz orgcontinue
		add dword ptr [esp], 0xA2
orgcontinue:
		ret
	}
}

static BOOL fSkipMessageReq = 0 ;
static DWORD mSkipMessageTimer = 0 ;

/*
	d2client_7E9B7: 0F 84 B8 00 00 00  jz d2client_7EA75 ->+184 B7EA75
*/
void  __declspec(naked) NPCMessageLoopPatch_ASM()
{
	__asm {
		test eax , eax
		jne noje
		mov eax ,[mSkipQuestMessage]
		cmp eax , 0				    //Not enabled 未开启功能，不处理
		je oldje
		cmp [fSkipMessageReq] , 0   //not quest 非任务信息，不处理
		je oldje
		add [mSkipMessageTimer],1
		cmp [mSkipMessageTimer],eax //less then timer 小于计时，不处理
		jle oldje
		mov [mSkipMessageTimer],0
		mov eax ,1
		ret
oldje:
		xor eax ,eax
		add dword ptr [esp], 0xB9  // 0F84B8000000
noje:
		ret
	}
}


//任务信息开始
void __declspec(naked) NPCQuestMessageStartPatch_ASM() {
	__asm {
		mov [fSkipMessageReq],1
		mov [mSkipMessageTimer],0
//oldcode:
		mov ecx, dword ptr [esi+0x0C]
		movzx edx, di
		ret
	}
}
//NPC菜单出现，表示信息结束了
void __declspec(naked) NPCQuestMessageEndPatch1_ASM() {
	__asm {
		mov [fSkipMessageReq],0
//oldcode:
		mov eax, dword ptr [esp+0x24]
		mov ecx, dword ptr [esp+0x20]
		ret
	}
}
//NPC交互结束，表示信息结束了
void __declspec(naked) NPCQuestMessageEndPatch2_ASM() {
	__asm {
		mov [fSkipMessageReq],0
//oldcode:
		mov edx, 1
		ret
	}
}
void __declspec(naked) NsPassingActBugFixPatch_ASM() {
		__asm {
		test edi, edi;
		jz retcode;
//oldcode
		rep stosd;
		mov ecx, edx;
		and ecx, 3;
		rep stosb;
retcode:
		ret;
	}
}
int __fastcall SetGoldPatch(int iType){
	int gold = *d2client_pGoldInTranBox;
	if (iType==4&&gold>0) {//从箱子往身上转移金钱，且金额大于0
		//计算是否超过能转移的最大值，如果超过，取最大值
		int goldstash = d2common_GetUnitStat(PLAYER, STAT_GOLD ,0);
		int lvl = d2common_GetUnitStat(PLAYER, STAT_LEVEL ,0);
		int maxgold = lvl*10000 - goldstash ;
		if ( gold>maxgold ) gold = maxgold ;
		*d2client_pGoldInTranBox = gold;
	}
	return gold;
}
void __declspec(naked) SetGoldPatch_ASM() {
	__asm{
		mov ecx ,eax
		call SetGoldPatch
		mov edx ,eax
		ret
	}
}
void __declspec(naked) SetDefaultGoldPatch_ASM() {
	__asm{
		cmp ebx , 4
		je nojmp
		cmp ebx , 6
		je nojmp
		add dword ptr [esp] , 0x30
nojmp:
		ret
	}
}

