#include "stdafx.h"

#ifdef MODULE_ENVIRONMENT


//天气
void __declspec(naked) WeatherPatch_ASM()
{
	__asm {
		cmp [tWeather.isOn],0
		je rainold
		xor al,al
rainold:
		ret 0x04
	}
}

//光线
void __declspec(naked) LightingPatch_ASM()
{
	__asm {
//[esp+4+0] = ptr red
//[esp+4+1] = ptr green
//[esp+4+2] = ptr blue
//return eax = intensity
		cmp [tLightRadiu.isOn],0
		je lightold

		mov eax,0xff
		mov byte ptr [esp+4+0], al
		mov byte ptr [esp+4+1], al
		mov byte ptr [esp+4+2], al
		add dword ptr [esp], 0x72;
		ret
lightold:
//original code
		push esi
		call D2GetLevelIdFromRoom
		ret
	}
}

//透视



//震动
void __stdcall ShakeScreenPatch(DWORD *pPosX, DWORD *pPosY)
{
	D2CalcShake(pPosX, pPosY);

	if (tScreenshake.isOn) {
		MAPSHAKEX = 0;
		MAPSHAKEY = 0;
	}

	#ifdef MODULE_SCROLLLOCK
		if(tScreenshake.isOn || *p_D2IsMapShakeOn==0){
			SetMapShake();
		}
	#endif

	*pPosX += MAPSHAKEX;
	*pPosY += MAPSHAKEY;

}

DWORD __fastcall InfravisionPatch(UnitAny *pUnit){

	//此处物件是否隐藏，但可以做很多事~~
	//返回1表示隐藏
	//返回0表示显示
	//返回2表示进行游戏默认判定
	
	if (enDumpUnit==2) dumpScanUnit(pUnit);
	if (dwMultiClientClickUnitId&&dwMultiClientClickUnitId==pUnit->dwUnitId
		&&dwMultiClientClickUnitType==pUnit->dwUnitType) {
		dwMultiClientClickUnitId=0;
		if (dwMultiClientClickRight) 
			RightSkill(pUnit,pUnit->pMonPath->wPosX,pUnit->pMonPath->wPosY);
		else 
			LeftClickObject(pUnit);
	}
	switch( pUnit->dwUnitType ){
		case UNITNO_PLAYER:
			#ifdef MODULE_CHECKDANGEROUS
				if (tChickenHostileNearby.isOn) CheckDangerousPlayer(pUnit);
			#endif

			#ifdef MODULE_AUTOENCHANT
				if (dwAutoEnchantGroupId) AutoEnchantUnit(pUnit);
			#endif
			break;
		case UNITNO_MONSTER:
			#ifdef MODULE_CHECKDANGEROUS
				if (tChickenDangerousMonster.isOn) CheckDangerousMonster(pUnit);
			#endif
			#ifdef MODULE_AUTOENCHANT
				if (dwAutoEnchantGroupId) AutoEnchantUnit(pUnit);
			#endif
			if (dwAutoSummonMode) AutoSummonUnit(pUnit);
			if (tHiddenCorpse.isOn && pUnit->dwMode == 0x0C && afMonsterCorpses[pUnit->dwTxtFileNo] == 0) return 1;
			break;
		case UNITNO_MISSILE:
			if (tHiddenCorpse.isOn && pUnit->dwMode!=3 && afMissileCorpses[pUnit->dwTxtFileNo] == 0) return 1;
			break;
		case UNITNO_ITEM:
			#ifdef MODULE_ITEMINGROUND
			if ( CheckItemVisible(pUnit) == FALSE ){
				return 1;
			}
			#endif
			break;
		default:
			break;
	}
	return tInfravision.isOn ?  0: 2;
}
void __declspec(naked) InfravisionPatch_ASM() {
	__asm {
		mov ecx, esi
		push esi
		call InfravisionPatch
		pop esi
		cmp eax ,2 
		je org
		add dword ptr [esp], 0x70
		ret
org:
		mov eax ,dword ptr [esi]
		cmp eax, 4
		ret 
	}
}

#endif
