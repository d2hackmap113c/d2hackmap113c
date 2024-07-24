#include "stdafx.h"

#ifdef MODULE_MONSTERLIFEBAR

//返回调整后的Xpos
long __fastcall MonsterNamePatch(wchar_t *wszName, long xPos , UnitAny *pMon ,long xDrawSize ,long xDrawOffset)
{
	int orilen = wcslen(wszName);
	int trimlen = orilen;
	while (trimlen > 0 && wszName[trimlen-1] == L' ') {
		trimlen--;
	}
	wszName[trimlen] = L'\0';

	if ( tMonsterLevel.isOn || tMonsterRuneTC.isOn ||  tMonsterTC.isOn){
		int lvl = 0;
		MonsterTxt *pMonTxt= pMon->pMonsterData->pMonsterTxt;
		if( pMonTxt->fBoss == 0 && DIFFICULTY && EXPANSION){
			lvl = GetAreaLevel();
		}
		else{
			lvl = D2GetUnitStat(pMon, STAT_LEVEL, 0);
		}

		if ( pMonTxt->fBoss == 1 && pMon->pMonsterData->wUniqueNo == 0 && !(pMonTxt->hcIdx>=546 && pMonTxt->hcIdx<=550 )) {
				//Putrid Defiler 不受场景等级影响但受+2 +3影响 
				//超级金怪受+3规则影响
		} else {
			if ( pMon->pMonsterData->fChamp ){
				lvl += 2;
			} else if ( pMon->pMonsterData->fBoss || pMon->pMonsterData->fMinion ) {
				lvl += 3; 
			}
		}
		if (tMonsterLevel.isOn) {
			wsprintfW(wszName+wcslen(wszName), L" (L%d)", lvl);
		}
		if (tMonsterRuneTC.isOn ||  tMonsterTC.isOn) {
			BYTE tcmode;
			WORD nTc;
			if (pMon->pMonsterData->fChamp )
				tcmode=1;
			else if ( pMon->pMonsterData->fBoss || pMon->pMonsterData->fMinion ) 
				tcmode=2; 
			else
				tcmode=0;

			if ( pMon->pMonsterData->fUnique ){
				SuperUniqueTxt *pSuperUniqueTxt = D2GetSuperUniqueTxt(pMon->pMonsterData->wUniqueNo);
				nTc = pSuperUniqueTxt->Tcs[DIFFICULTY];
			}else{
				nTc = pMonTxt->Tcs[DIFFICULTY][tcmode];
			}
			MonTC *pMonTC = GetTcLevel(nTc , lvl);
			if ( tMonsterTC.isOn )
				wsprintfW(wszName+wcslen(wszName), L" (tc%d)", pMonTC->nTc);
			if ( tMonsterRuneTC.isOn && EXPANSION )
				wsprintfW(wszName+wcslen(wszName), L" (rtc%d)", pMonTC->nRtc);

		}
	}

	if ( tMonsterHPPercent.isOn ){
		wsprintfW(wszName+wcslen(wszName), L" (%d%%)", D2GetMonsterHpPercent(pMon));
	}
	
	if (fSinglePlayer&&(tSPMonsterHP.isOn||tSPMonsterExp.isOn)){
		UnitAny *real=getSinglePlayerUnit(pMon->dwUnitId,pMon->dwUnitType);
		if (real) {
			char buf[64];int pos=0;
			pos+=sprintf(buf+pos," id%d",pMon->dwUnitId);
			if (tSPMonsterHP.isOn) {
				int hp = D2GetUnitStat(real, STAT_HP, 0)>>8;
				int maxhp = D2GetUnitStat(real, STAT_MAXHP, 0)>>8;
				buf[pos++]=' ';
				if (hp>1000) pos+=sprintf(buf+pos,"%d,%03d",hp/1000,hp%1000);
				else pos+=sprintf(buf+pos,"%d",hp);
				buf[pos++]='/';
				if (hp>1000) pos+=sprintf(buf+pos,"%d,%03d",maxhp/1000,maxhp%1000);
				else pos+=sprintf(buf+pos,"%d",maxhp);
			}
			if (tSPMonsterExp.isOn) {
				int exp = D2GetUnitStat(real, STAT_EXP, 0);
				pos+=sprintf(buf+pos," EXP");
				if (exp>1000) pos+=sprintf(buf+pos,"%d,%03d",exp/1000,exp%1000);
				else pos+=sprintf(buf+pos,"%d",exp);
			}
			wsprintfW(wszName+wcslen(wszName), L"%hs", buf);
		}
	}
	
	if ( tMonsterDistance.isOn ){
		char strVal [10] ;
		DblToStr( GetUnitDistanceInSubtiles(pMon,PLAYER) ,2 , strVal);
		wsprintfW(wszName+wcslen(wszName), L" (%hs yards)", strVal);
	}

	if ( tMonsterID.isOn ){
		wsprintfW(wszName+wcslen(wszName), L" (ID:%d)", pMon->dwTxtFileNo);
	}

	if(tMonsterResists.isOn){
		int dr = D2GetUnitStat(pMon, STAT_DAMAGE_REDUCED,	0);
		int mr = D2GetUnitStat(pMon, STAT_MAGIC_RESIST,	0);
		int fr = D2GetUnitStat(pMon, STAT_FIRE_RESIST,		0);
		int lr = D2GetUnitStat(pMon, STAT_LIGHTING_RESIST,	0);
		int cr = D2GetUnitStat(pMon, STAT_COLD_RESIST,		0);
		int pr = D2GetUnitStat(pMon, STAT_POSION_RESIST,	0);
		if (dr| mr | fr | lr | cr | pr) {
			wsprintfW(wszName+wcslen(wszName), L" (Res:");
			if (dr)	wsprintfW2Colour(wszName+wcslen(wszName), 7, " D%d", dr); // Golden
			if (mr)	wsprintfW2Colour(wszName+wcslen(wszName), 8, " M%d", mr); // Deep yellow
			if (fr)	wsprintfW2Colour(wszName+wcslen(wszName), 1, " F%d", fr); // Red
			if (lr)	wsprintfW2Colour(wszName+wcslen(wszName), 9, " L%d", lr); // Light yellow
			if (cr)	wsprintfW2Colour(wszName+wcslen(wszName), 3, " C%d", cr); // Blue
			if (pr)	wsprintfW2Colour(wszName+wcslen(wszName), 2, " P%d", pr); // Green
			wsprintfW(wszName+wcslen(wszName), L")");
		}
	}
	
	int l = orilen - trimlen;
	while (l-- > 0) {
		wcscat(wszName, L" ");
	}

	int newlen = GetTextWidth(wszName);
	xPos = xDrawSize + xDrawOffset ;
	if ( newlen > xPos) return 1;

	return  ( xPos - newlen ) /2;

}


void __declspec(naked) MonsterNamePatch_ASM()
{
	//ecx  wszName
	//edx  xpos
	//ebx  pUnit
	//[esp+0x38] xpos offset
	__asm {
		push ecx

		push [esp+0x3C]
		push [esp+0x48]
		push ebx
		call MonsterNamePatch

		pop ecx
		mov edx , eax
		jmp D2DrawUnitLifeBar
	}
}

static wchar_t szMonsterDescComma[] = L", ";
void __declspec(naked) MonsterDescCommaPatch1_ASM()
{
	__asm {
		mov eax,offset szMonsterDescComma
		ret
	}
}

void __declspec(naked) MonsterDescCommaPatch2_ASM()
{
	__asm {
		mov edx,offset szMonsterDescComma
		ret
	}
}

void __fastcall MonsterDescCommaPatch3(wchar_t *wszStr, MonsterTxt *pMonTxt)
{
	if (pMonTxt->wFlag == 0x1506) {
		wcscat(wszStr, szMonsterDescComma);
	}
}

void __declspec(naked) MonsterDescCommaPatch3_ASM()
{
	__asm {
//ecx = ptr string
//[esp+0x20] = ptr monstats.txt
		mov edx,[esp+0x20]
		call MonsterDescCommaPatch3
		mov ecx, [esp+0x20];
		ret
	}
}

void __declspec(naked) LifeBarPatch_ASM()
{
	__asm {
		pop ebp
//originalish code
		movzx eax,[nLifeBarTrans]
		cmp al,-1
		jne nodefault1
		mov al,1
nodefault1:
		push eax
		mov al,[nLifeBarColour]
		cmp al,-1
		jne nodefault2
		mov al,5
nodefault2:
		push eax
		sar ecx,7
		jmp ebp
	}
}




#endif
