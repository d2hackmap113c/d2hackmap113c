#include "stdafx.h"
#include "header.h"

#define FontHalfH 6
extern int dwRescueBarSafeDistance;
extern ToggleVar 	tChickenHostileNearby;
extern ToggleVar 	tChickenDangerousMonster;
extern int dwSlowRestoreMs;

int CheckItemVisible(UnitAny *pItem);
int ChangeVisualEffect();
void checkBossMonster( UnitAny  *pUnit );
void CheckDangerousPlayer( UnitAny  *pUnit );
void CheckDangerousMonster( UnitAny  *pUnit );
int dwVisualEffectMode=0;
ToggleVar 	tChangeVisualEffect={TOGGLEVAR_DOWN,	0,	-1,		1,	"ChangeVisualEffect",&ChangeVisualEffect};
ToggleVar 	tWeather={				TOGGLEVAR_ONOFF,	0,	-1,		1,	"Weather"};
BYTE 			afMonsterCorpses[1000]	={0};
BYTE 			afMissileCorpses[1000]	={0};
ToggleVar     tHiddenCorpse={        TOGGLEVAR_ONOFF,  0,  -1,  1,	"HiddenCorpse"};
char skipMonsters[1024];
ToggleVar tSkipDrawToggle={TOGGLEVAR_ONOFF,0,-1,1,"SkipDrawToggle",0,0,0,2};

static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_INT, "VisualEffectMode",              &dwVisualEffectMode      , 4 },
  {CONFIG_VAR_TYPE_KEY, "VisualEffectKey",			&tChangeVisualEffect		},
  {CONFIG_VAR_TYPE_KEY, "WeatherToggle",			&tWeather		},
  {CONFIG_VAR_TYPE_CHAR_ARRAY1, "MissileCorpses",         &afMissileCorpses,    1,{1000}},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "MonsterCorpses",         &afMonsterCorpses,    1,{1000}},
  {CONFIG_VAR_TYPE_KEY, "HiddenCorpsesToggle",    &tHiddenCorpse      },
  {CONFIG_VAR_TYPE_CHAR_ARRAY0, "SkipDrawMonsters",skipMonsters,1,{1024}},
  {CONFIG_VAR_TYPE_KEY, "SkipDrawToggle",&tSkipDrawToggle},
};
void env_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void env_initConfigVars() {
  memset(afMonsterCorpses,1,sizeof(afMonsterCorpses));
  memset(afMissileCorpses,1,sizeof(afMissileCorpses));
  memset(skipMonsters,0,sizeof(skipMonsters));
}

int ChangeVisualEffect() {
	dwVisualEffectMode++;if (dwVisualEffectMode<0||dwVisualEffectMode>2) dwVisualEffectMode=0;
	if (fInGame) {
		switch (dwVisualEffectMode) {
			case 0:gameMessage("Full visual effects");break;
			case 1:gameMessage("Infravision on");break;
			case 2:gameMessage("Light effects off");break;
		}
	}
	return 1;
}
//天气
void __declspec(naked) WeatherPatch_ASM() {
	__asm {
		cmp [tWeather.isOn],0
		je rainold
		xor al,al
rainold:
		ret 0x04
	}
}
//光线
void __declspec(naked) LightingPatch_ASM() {
	__asm {
//[esp+4+0] = ptr red
//[esp+4+1] = ptr green
//[esp+4+2] = ptr blue
//return eax = intensity
		cmp [dwVisualEffectMode],2
		jne lightold
		mov eax,0xff
		mov byte ptr [esp+4+0], al
		mov byte ptr [esp+4+1], al
		mov byte ptr [esp+4+2], al
		add dword ptr [esp], 0x72;
		ret
lightold:
//original code
		push esi
		call d2common_GetLevelIdFromRectData
		ret
	}
}
static int prisonDistance=0;
//透视
DWORD __fastcall InfravisionPatch(UnitAny *pUnit){
	//此处物件是否隐藏，但可以做很多事~~
	//返回1表示隐藏 Hide
	//返回0表示显示 Show
	//返回2表示进行游戏默认判定 Default
	switch (pUnit->dwUnitType) {
		case UNITNO_PLAYER: {
			if (tChickenHostileNearby.isOn) CheckDangerousPlayer(pUnit);
			break;
		}
		case UNITNO_OBJECT:
			if (!fPlayerInTown) {
				if (pUnit->dwTxtFileNo==473&&LEVELNO==111) {
					prisonDistance=(getPlayerDistanceM256(pUnit)>>8)*2/3;
				}
			}
			break;
		case UNITNO_MONSTER: {
			if (tChickenDangerousMonster.isOn) CheckDangerousMonster(pUnit);
			if (tHiddenCorpse.isOn && pUnit->dwMode == 0x0C && afMonsterCorpses[pUnit->dwTxtFileNo] == 0) return 1;
			if (!fPlayerInTown) {
				if (pUnit->dwMode&&pUnit->dwMode!=12&&pUnit->pMonsterData->fBoss) checkBossMonster(pUnit);
				if (pUnit->dwTxtFileNo==434&&LEVELNO==111) {
					wchar_t wszbuf[128];int pos=0,color=2;
					int tooClose=prisonDistance<=dwRescueBarSafeDistance;
					pos+=wsprintfW(wszbuf+pos, L"Prison %d yard",prisonDistance);
					if (tooClose) pos+=wsprintfW(wszbuf+pos, L" TOO CLOSE!");
					int opened=pUnit->dwMode==0||pUnit->dwMode==12;
					if (opened) pos+=wsprintfW(wszbuf+pos, L" OPEN");
					else if (d2common_IsUnitBlocked(PLAYER,pUnit,4)) {color=1;pos+=wsprintfW(wszbuf+pos, L" blocked");}
					SetBottomAlertMsg2(wszbuf,300,color,tooClose||opened);
				}
			}
			int skip=tSkipDrawToggle.isOn&&skipMonsters[pUnit->dwTxtFileNo]<2
						||dwSlowRestoreMs&&skipMonsters[pUnit->dwTxtFileNo]==1;
			if (skip) {
				int dead=pUnit->dwMode==0||pUnit->dwMode==12;
				if (!dead) {
					wchar_t temp[16];
					wsprintfW(temp,L"%d:%d",pUnit->dwTxtFileNo,pUnit->dwMode);
					drawBgTextMiddle(temp,pUnit->pMonPath->drawX-screenDrawX,pUnit->pMonPath->drawY-screenDrawY-FontHalfH,0,0x10);
				}
				return 1;
			}
			break;
		}
		case UNITNO_MISSILE:
			if (tHiddenCorpse.isOn && pUnit->dwMode!=3 && afMissileCorpses[pUnit->dwTxtFileNo] == 0) return 1;
			break;
		case UNITNO_ITEM:
			if (!CheckItemVisible(pUnit)) return 1;
			return 0; //shouldn't be hide behide a wall
			break;
		default:
			break;
	}
	return dwVisualEffectMode?0:2;
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
static void drawGroundRect(AreaRectData *pData) {
	AreaRectBitmap *gtypes=pData->bitmap;if (!gtypes) return;
	unsigned short *ptr=gtypes->bitmap;if (!ptr) return;
	for (int y=0;y<gtypes->unitH;y++) {
		for (int x=0;x<gtypes->unitW;x++) {
			int b=*ptr++;if ((b&5)==0) continue;
			int unitX=gtypes->unitX+x,unitY=gtypes->unitY+y;
			int drawX=(unitX-unitY)<<4,drawY=(unitX+unitY)<<3;
			if ((b&1)==0) //path not block
				drawBgTextMiddle(L"-",drawX-screenDrawX,drawY-screenDrawY-FontHalfH,0,0x10);
			else //path block
				drawBgTextMiddle(L"X",drawX-screenDrawX,drawY-screenDrawY-FontHalfH,0,0x10);
		}
	}
}
void drawMyGround() {
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	drawGroundRect(pData);
	for (int i=0;i<pData->nearbyRectCount;i++) {
		AreaRectData *pNData=pData->paDataNear[i];if (!pNData) continue;
		drawGroundRect(pNData);
	}
}
//d2client_8B22F: push edi(input_eax)
//d2client_8B230: mov eax, 0x6FBD33B0(d2client_1233B0)(->1) ; esp-24?
//d2client_8B235: call d2client_14050(1 args) <--- draw ground
void __declspec(naked) SkipDrawGroundPatch_ASM() {
	__asm {
		cmp tSkipDrawToggle.isOn,0
		jz org
		pushad
		call drawMyGround
		popad
		ret 4
org:
		jmp dword ptr [d2client_drawGround]
	}
}

