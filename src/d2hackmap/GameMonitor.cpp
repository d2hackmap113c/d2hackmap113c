
#include "stdafx.h"
#include "bitstream.h"

int guessEnchantLevel();
int guessBOLevel(int debug);

#ifdef MODULE_GAMEMONITOR

void hex(FILE *fp,int addr,void *buf,int n);
int fState100HP=0,fState106Mana=0;
unsigned char beltCount[4];
int beltLayers;
int dwOakSageLvl;
int dwGuessBOMs=0;
int dwGuessEnchantMs=0;
int dwRecheckBeltMs;
int dwTownPortalCount=0,dwIdentifyPortalCount=0;
struct CountDown {
	const char *name;
	int skill,cls,type;
	int addS,stepS,startMs,endMs;
	int active,lv,selfskill;
};
//werewolf: time 40s
//Lycanthropy: time+=20+lvl*20
int fWerewolf,fWerebear;
static struct CountDown cd_QH={"QH",52,1,0,120,24,0,0,0}; //sor fire enchant
static struct CountDown cd_BO={"BO",149,4,2,20,10,0,0,0}; //bar warcries battleorder
static struct CountDown cd_HS={"HS",117,3,0,5,25,0,0,0};//pal combat holyshield
static struct CountDown cd_werewolf={"Wolf",223,5,1,60,20,0,0,0};//dru shape werewolf
static struct CountDown cd_werebear={"Bear",228,5,1,60,20,0,0,0};//dru shape werebear
static struct CountDown cd_Fade={"Fade",267,6,1,108,12,0,0,0};//asn shadow fade
static struct CountDown cd_Burst={"Burst",258,6,1,108,12,0,0,0};//asn shadow burst of speed
static struct CountDown cd_Cloak={"Cloak",264,6,1,8,1,0,0,0};//asn shadow Cloak of Shadows
static struct CountDown cd_KB={"KB",0,0,0,0,0,0,0,0};//KB countdown
struct CountDown *countDowns[]={&cd_QH,&cd_BO,&cd_HS,
	&cd_werewolf,&cd_werebear,
	&cd_Fade,&cd_Burst,&cd_Cloak,&cd_KB};
void setKBCountDown(int team,int en) {
	cd_KB.active=en;
	cd_KB.lv=team;
	cd_KB.endMs=dwCurMs+11750;
}
void setCountDownEndTime(struct CountDown *cd) {
	int time=cd->addS+cd->lv*cd->stepS;
	if (cd->skill==223||cd->skill==228) { //werewolf werebear
		int lv=getSkillLevel(PLAYER,224,cd->type,cd->skill); //Lycanthropy
		if (lv==0) time=40;
		else time=cd->addS+lv*cd->stepS;
	}
	cd->endMs=cd->startMs+time*1000;
	if (tPacketHandler.isOn) {
		LOG("%s self=%d level=%d time=%d\n",cd->name,cd->selfskill,cd->lv,time);
	}
}
void setupCountdown(struct CountDown *cd) {
	cd->lv=getSkillLevel(PLAYER,cd->cls,cd->type,cd->skill); 
	cd->startMs=dwCurMs;
	cd->selfskill=1;
	setCountDownEndTime(cd);
}
void onRightSkillMap(int x,int y) {
	switch (dwRightSkill) {
		case 52: setupCountdown(&cd_QH);break;
		case 117: setupCountdown(&cd_HS);break;
		case 149: setupCountdown(&cd_BO);break;
		case 223: setupCountdown(&cd_werewolf);break;
		case 228: setupCountdown(&cd_werebear);break;
		case 258: setupCountdown(&cd_Burst);break;
		case 264: setupCountdown(&cd_Cloak);break;
		case 267: setupCountdown(&cd_Fade);break;
	}
}
void onRightSkillUnit(int type,int id) {
	switch (dwRightSkill) {
		//case 52: setupCountdown(&cd_QH);break;
		case 117: setupCountdown(&cd_HS);break;
		case 149: setupCountdown(&cd_BO);break;
		case 223: setupCountdown(&cd_werewolf);break;
		case 228: setupCountdown(&cd_werebear);break;
		case 258: setupCountdown(&cd_Burst);break;
		case 264: setupCountdown(&cd_Cloak);break;
		case 267: setupCountdown(&cd_Fade);break;
	}
}
UnitAny *leftWeapon,*rightWeapon;
void recheckBelt() {
	static int n=0;
	if (!fInGame) return;
	*(int *)beltCount=0;
	leftWeapon=NULL;rightWeapon=NULL;
	beltLayers=1;
	for (UnitAny *pUnit = D2GetFirstItemInInv(PLAYER->pInventory);pUnit;pUnit = D2GetNextItemInInv(pUnit)) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) continue ;
		if (pUnit->pItemData->nItemLocation==255) {
			switch (pUnit->pItemData->nBodyLocation) {
				case 4:rightWeapon=pUnit;break;
				case 5:leftWeapon=pUnit;break;
				case 8: //belt
					int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
					if (index==1039||index==1040) beltLayers=2;
					else if (index==1041||index==1042) beltLayers=3;
					else beltLayers=4;
					break;
			}
		}
		if (pUnit->pItemData->nLocation==2) {
			int x=pUnit->pItemPath->dwPosX;
			beltCount[x&3]++;
		}
	}
	//LOG("recheckBelt %d\n",n++);
	dwRecheckBeltMs=0;
}
void ResetMonitor(){
	dwGuessBOMs=0;dwOakSageLvl=0;
	dwGuessEnchantMs=0;
	fWerewolf=0;fWerebear=0;
	leftWeapon=NULL;rightWeapon=NULL;
	for (int i=0;i<_ARRAYSIZE(countDowns);i++) countDowns[i]->active=0;
	for ( int i = 0 ; i < 200 ; i ++ ){	
		if ( (int)(sMonitorStr[i].dwStatNo)<=0 )break;
		sMonitorStr[i].fEnable = 0 ;
	}
}
void DrawMonitorInfo(){
	if (dwGuessBOMs&&dwCurMs>dwGuessBOMs) {
		dwGuessBOMs=0;
		struct CountDown *cd=&cd_BO;
		cd->selfskill=0;
		cd->lv=guessBOLevel(0);
		setCountDownEndTime(cd);
	}
	if (dwGuessEnchantMs&&dwCurMs>dwGuessEnchantMs) {
		dwGuessEnchantMs=0;
		int lv=guessEnchantLevel();
		if (lv) {
			struct CountDown *cd=&cd_QH;
			cd->selfskill=0;cd->lv=lv;
			setCountDownEndTime(cd);
		}
	}
	if (fScreenSaverActive) return;
	wchar_t wszTemp[512];
	int xpos = SCREENSIZE.x+dwGameMonitorX;
	int ypos = SCREENSIZE.y+dwGameMonitorY;
	DWORD dwTimer = dwCurMs;
	DWORD dwOldFone = D2SetTextFont(8);
	if (fTransferClick&&wszTransferClick[0]) {
		int x=xpos-GetTextWidth(wszTransferClick);if (x<0) x=0;
		D2DrawText(wszTransferClick, x , ypos, 0, 0);
		ypos = ypos -15;
	}
	if (fMonitorQuantity) {
		D2SetTextFont(3);
		if (dwRightSkill==35) {
			int n=D2GetUnitStat(PLAYER, 70, 0); //quantity
			if (n) {
				wsprintfW(wszTemp, L"%d",  n);
				int x=xpos-GetTextWidth(wszTemp);
				D2DrawText(wszTemp, x , ypos, 0, 0);
				D2DrawText(wszTemp, x+1 , ypos+1, 6, 0);
				ypos = ypos-30;
			}
		}
		if (!D2CheckUiStatus(UIVAR_BELT)) {
			static wchar_t *num[4]={L"0",L"1",L"2",L"3"};
			int x=SCREENSIZE.x/2+33;
			int y=SCREENSIZE.y-40;
			for (int i=0;i<4;i++) {
				if (beltCount[i]<beltLayers) D2DrawText(num[beltCount[i]&7],x,y,0,0);
				x+=30;
			}
		}
		/*if (!fPlayerInTown) {
			wsprintfW(wszTemp, L"ID%d Town%d", dwIdentifyPortalCount,dwTownPortalCount);
			int x=xpos-GetTextWidth(wszTemp);
			D2DrawText(wszTemp, x , ypos, 0, 0);
			D2DrawText(wszTemp, x+1 , ypos+1, 6, 0);
			ypos = ypos -15;
		}
		*/
	}
	if (fMonitorDurability) {
		D2SetTextFont(8);
		for (int i=0;i<2;i++) {
			UnitAny *pUnit=i==0?rightWeapon:leftWeapon;
			if (!pUnit) continue;
			int max_durability=D2GetUnitStat(pUnit, 73, 0); 
			if (max_durability) {
				int durability=D2GetUnitStat(pUnit, 72, 0); 
				if (durability<max_durability) {
					wsprintfW(wszTemp, L"%s %d/%d",i==0?L"DurR":L"DurL",durability,max_durability);
					int x=xpos-GetTextWidth(wszTemp);
					D2DrawText(wszTemp, x , ypos, 0, 0);
					D2DrawText(wszTemp, x+1 , ypos+1, 6, 0);
					ypos = ypos -15;
				}
			}
		}
	}
	if (tCountDown.isOn) {
		D2SetTextFont(dwCountDownFontSize);
		for (int i=0;i<_ARRAYSIZE(countDowns);i++) {
			struct CountDown *cd=countDowns[i];
			if (!cd->active) continue;
			int color=6;
			int secs;
			if (cd->endMs) {
				int ms=cd->endMs-dwCurMs;
				secs = ms/1000;
				if (0<=secs&&secs<30) color=(ms%1000)<500?1:2;
				wsprintfW(wszTemp, L"%hs%d: %d", cd->name,cd->lv,secs);
			} else {
				secs=(dwCurMs-cd->startMs)/1000;
				wsprintfW(wszTemp, L"%hs: %d", cd->name,secs);
			}
			int x=xpos-GetTextWidth(wszTemp);
			D2DrawText(wszTemp, x , ypos, 0, 0);
			D2DrawText(wszTemp, x+1 , ypos+1, color, 0);
			ypos = ypos-dwCountDownGap;
		}
		if (dwOakSageLvl) {
			wsprintfW(wszTemp, L"OakSage%d", dwOakSageLvl);
			int x=xpos-GetTextWidth(wszTemp);
			D2DrawText(wszTemp, x , ypos, 0, 0);
			D2DrawText(wszTemp, x+1 , ypos+1, 6, 0);
			ypos = ypos-dwCountDownGap;
		}
	}
	D2SetTextFont(8);
	if (tStateMonitorToggle.isOn) {
		for ( int i = 0 ; i < 200 ; i ++ ){	
			if ( (int)(sMonitorStr[i].dwStatNo)<=0 )break;
			if ( sMonitorStr[i].fEnable ){
				DWORD secs = (dwTimer - sMonitorStr[i].dwTimer)/1000;
				//wsprintfW(wszTemp, L"%s: %.2d:%.2d:%.2d",  sMonitorStr[i].wszDesc[dwGameLng],secs/3600, (secs/60)%60, secs%60);
				wsprintfW(wszTemp, L"%s: %.2d:%.2d",  sMonitorStr[i].wszDesc[dwGameLng],secs/60, secs%60);
				int x=xpos-GetTextWidth(wszTemp);
				D2DrawText(wszTemp, x , ypos, 0, 0);
				D2DrawText(wszTemp, x+1 , ypos+1, sMonitorStr[i].dwColor, 0);
				ypos = ypos -15;
			}
		}
	}
	D2SetTextFont(dwOldFone);
}

void __stdcall SetState( DWORD dwStateNo , BOOL fSet ){
	if (tPacketHandler.isOn) {
		LOG("set state %d %d\n",dwStateNo,fSet);
	}
	struct CountDown *cd=NULL;
	switch (dwStateNo) {
		case 16: cd=&cd_QH;break;
		case 32: cd=&cd_BO;break;
		case 100: fState100HP=fSet;break;
		case 101: cd=&cd_HS;break;
		case 106: fState106Mana=fSet;break;
		case 139: fWerewolf=fSet;cd=&cd_werewolf;break;
		case 140: fWerebear=fSet;cd=&cd_werebear;break;
		case 149: dwGuessBOMs=dwCurMs+500;break; //Oak Sage
		case 153: cd=&cd_Cloak;break;
		case 157: cd=&cd_Burst;break;
		case 159: cd=&cd_Fade;break;
	}
	if (cd) {
		cd->active=fSet;
		if (fSet) {
			if (cd->startMs<=dwCurMs&&dwCurMs<cd->startMs+500) {//self skill
				if (dwStateNo==16) dwGuessEnchantMs=dwCurMs+500;
				else if (dwStateNo==32) dwGuessBOMs=dwCurMs+500;
			} else {
				cd->endMs=0;
				if (dwStateNo==16) dwGuessEnchantMs=dwCurMs+500;
				else if (dwStateNo==32) dwGuessBOMs=dwCurMs+500;
			}
			cd->startMs=dwCurMs;
		} else {
			cd->startMs=cd->endMs=0;
		}
	}
	if ( tStateMonitorToggle.isOn==0 ) return ;
	for ( int i = 0 ; i < 200 ; i ++ ){	
		if ( (int)(sMonitorStr[i].dwStatNo)<=0 )break;
		if ( sMonitorStr[i].dwStatNo==dwStateNo ){
			sMonitorStr[i].fEnable = fSet;
			sMonitorStr[i].dwTimer = dwCurMs ;
			break;
		}
	}
}

void __declspec(naked) RecvCommand_A7_Patch_ASM()
{
	__asm{

		mov esi, ecx
		movzx edx, byte ptr [esi+1]
		mov ecx, dword ptr [esi+2]

		cmp edx , 0
		jne  org
		cmp ecx ,[dwPlayerId]
		jne org
		movzx eax, byte ptr [esi+06]
		
		push ecx
		push edx
		push esi 

		push 1
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
org:
		ret

	}

}

void __declspec(naked) RecvCommand_A8_Patch_ASM() {
	__asm{

		mov esi, ecx
		movzx edx, byte ptr [esi+1]
		mov ecx, dword ptr [esi+2]

		cmp edx , 0
		jne  org
		cmp ecx ,[dwPlayerId]
		jne org
		movzx eax, byte ptr [esi+07]
		
		push ecx
		push edx
		push esi 

		push 1
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
org:
		ret
	}
}

void __declspec(naked) RecvCommand_A9_Patch_ASM() {
	__asm{
		mov esi, ecx
		movzx edx, byte ptr [esi+1]
		mov ecx, dword ptr [esi+2]

		cmp edx , 0
		jne  org
		cmp ecx ,[dwPlayerId]
		jne org
		movzx eax, byte ptr [esi+06]
		
		push ecx
		push edx
		push esi 

		push 0
		push eax
		call SetState

		pop esi
		pop edx
		pop ecx
org:
		ret
	}
}

void itemAction(struct bitstream *bs,char *buf) {
	int cmd=buf[0]&0xFF;
	bitstream_seek(bs,61-16);
	int equipped=bitstream_rbit(bs, 4);
	bitstream_seek(bs,73-16);
	int location=bitstream_rbit(bs, 3);//bit73-75 0:belt bag:1 cube:4 stash:5
	if (location==0 //belt changed
		||equipped==4||equipped==5||equipped==11||equipped==12 //weapon changed
		) {
		leftWeapon=NULL;rightWeapon=NULL;
		dwRecheckBeltMs=dwCurMs+300;
	}
	if (tPacketHandler.isOn&&logfp) {
		LOG("	RECV %02x_itemAction location=%d equipped=%d\n",buf[0]&0xFF,location,equipped);
		if ((buf[0]&0xFF)==0x9C) hex(logfp,0,buf,8);
		else hex(logfp,0,buf,13);
/*|  1    | Helmet                  |
|  2    | Amulet                  |
|  3    | Armor                   |
|  4    | Weapon (Right)          |
|  5    | Weapon (Left)           |
|  6    | Ring (Right)            |
|  7    | Ring (Left)             |
|  8    | Belt                    |
|  9    | Boots                   |
| 10    | Gloves                  |
| 11    | Alternate Weapon (Right)|
| 12    | Alternate Weapon (Left) |
*/
	}
}
void __fastcall itemAction9C(char *buf) {
	struct bitstream bs;
	bitstream_init(&bs,buf+8,buf+8+12);
	itemAction(&bs,buf);
}
void __fastcall itemAction9D(char *buf) {
	struct bitstream bs;
	bitstream_init(&bs,buf+13,buf+13+12);
	itemAction(&bs,buf);
}

void __fastcall updateItemSkill(char *buf) {
	//22 [WORD Unknown (Unit Type?)] [DWORD Unit Id] [WORD Skill] [BYTE Amount] [WORD Unknown] 
	//22 00 db 01 - 00 00 00 dc - 00 02 a9 00 -             |"               
	int skill=*(unsigned short *)(buf+7);
	int count=buf[9];
	if (skill==220) dwTownPortalCount=count;
	else if (skill==218) dwIdentifyPortalCount=count;
	if (tPacketHandler.isOn) 
		LOG("	RECV 22_updateItemSkill skill=%d count=%d\n", skill,count);
}

// 6FB5E090 - B9 3F000000           - mov ecx,0000003F { 63 }
void __declspec(naked) RecvCommand_9C_Patch_ASM() {
	__asm{
		pushad
		call itemAction9C
		popad
		mov ecx, 0x3F
		ret
	}
}
// 6FB5E880 - B9 40000000           - mov ecx,00000040 { 64 }
void __declspec(naked) RecvCommand_9D_Patch_ASM() {
	__asm{
		pushad
		call itemAction9D
		popad
		mov ecx, 0x40
		ret
	}
}
/*
6FB5C800 - 56                    - push esi
6FB5C801 - 8B F1                 - mov esi,ecx
6FB5C803 - 8A 46 0B              - mov al,[esi+0B]
6FB5C806 - 84 C0                 - test al,al
*/
void __declspec(naked) RecvCommand_22_Patch_ASM() {
	__asm{
		pushad
		call updateItemSkill
		popad
		mov esi,ecx
		mov al,byte ptr [esi+0xB]
		ret
	}
}
#endif
