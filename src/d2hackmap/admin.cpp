#include "stdafx.h"
#include "header.h"
#include "d2BinFile.h"

unsigned short *getQuest(World *context,UnitAny *pUnit);
void SendChatMessage(char *myMsg);
extern char adminCmdPrefix[32];
static char lastAdminCmd[256]={0};
int RepeatAdminCmd() {
	int plen=strlen(adminCmdPrefix);
	if (memcmp(adminCmdPrefix,lastAdminCmd,plen)!=0) return 0;
	SendChatMessage(lastAdminCmd);
	return 0;
}
int DoTest();
static void __fastcall clientChat(char *msg) {
	//LOG("client chat %s\n",msg);
	int plen=strlen(adminCmdPrefix);
	if (memcmp(adminCmdPrefix,msg,plen)!=0) return;
	if (memcmp(",test",msg,5)==0) {DoTest();}
	strncpy(lastAdminCmd,msg,255);lastAdminCmd[255]=0;
}
/*
6FB207F0 - 55                    - push ebp
6FB207F1 - 8B EC                 - mov ebp,esp
6FB207F3 - 83 E4 F8              - and esp,-08 { 248 }
6FB207F6 - 81 EC 1C010000        - sub esp,0000011C { 284 }
6FB207FC - 53                    - push ebx
6FB207FD - 56                    - push esi
6FB207FE - 57                    - push edi
6FB207FF - 8B D8                 - mov ebx,eax
6FB20801 - 33 C0                 - xor eax,eax
6FB20803 - B9 40000000           - mov ecx,00000040 { 64 } <---
*/
void __declspec(naked) ClientChatPatch_ASM() {
	__asm {
		pushad
		mov ecx,edx
		call clientChat 
		popad
		mov ecx,0x40
		ret
	}
}
void setCharLv(World *world,UnitAny *pUnit,int lv) {
	unsigned int nextexp = d2common_GetMinExpToLevel(pUnit->dwTxtFileNo, lv);
	unsigned int curexp = d2common_GetUnitStat(pUnit, STAT_EXP, 0);
	if (curexp<nextexp) 
		d2common_AddPlayerStat(pUnit,STAT_EXP,nextexp-curexp,0 );
	else {
		if (lv<=1) {lv=1;nextexp=0;}
		d2common_changeUnitStat(pUnit,STAT_LEVEL,lv,0);
		d2common_changeUnitStat(pUnit,STAT_EXP,nextexp,0);
	}
	__asm {
		mov esi,pUnit
		push world
		call d2game_playerUpgrade
	}
}
void maxCharStat(World *world,UnitAny *pUnit) {
	setCharLv(world,pUnit,99);
	for (int id=0;id<16;id++) {
		if (id==4||id==12||id==13) continue;
		ItemStatCostTxt *txt=d2common_GetItemStatCostTxt(id);if (!txt) continue;
		int v=(1<<txt->csvbits)-1;
		//if (txt->valshift) v<<=txt->valshift;
		if (id==14) v=990000;
		if (id==15) v=2500000;
		int cur=d2common_GetUnitBaseStat(pUnit,id,0);
		if (v!=cur) d2common_AddPlayerStat(pUnit,id,v-cur,0);
	}
}
void enableAllWaypoints(UnitAny *pUnit) {
	unsigned char allWaypoints[7]={2,1,0xFF,0xFF,0xFF,0xFF,0x77};
	for (int i=0;i<3;i++) 
		memcpy((char *)(pUnit->pPlayerData->waypoints[i]),allWaypoints,7);
}
int passGame(World *world,UnitAny *pUnit,int act) {
	enableAllWaypoints(pUnit);
	unsigned short *qs=(unsigned short *)getQuest(world,pUnit);
	if (act>=1) {
		qs[0]=1;//Travel to ACT1
		qs[4]=0x801a; //Search for Cain
		qs[6]=0x11; //Sisters to slaughter
		qs[7]=1;//Travel to ACT2
	}
	if (act>=2) {
		qs[8]=1;//Introduced to Jerhyn
		qs[10]=0x0801;// The Horadric Staff
		qs[11]=0x0001;// Tainted Sun
		qs[12]=0x0181;// Arcane Sanctuary
		qs[13]=0x0005;// The Summoner
		qs[14]=0x0025;// The Seven Tombs
		qs[15]=0x0001;// Travel to ACT3
	}
	if (act>=3) {
		qs[16]=0x0001;// Introduced to Hratli
		qs[18]=0x0001;// Khalim's Will
		//qs[20]=0x0040;// The Golden Bird
		qs[21]=0x0019;// The Blackened Temple
		qs[22]=0x0b01;// The Guardian
		qs[23]=0x0001; //Travel to ACT4
	}
	if (act>=4) {
		qs[24]=0x0001; //Introduced to ACT4
		if (world->expansion) {
			//qs[25]=0x0004; //The Fallen Angel
			qs[26]=0x201; // Terro's End
			qs[28]=1;//Travel to ACT5
		} else {
			int difficulty=world->difficulty;
			PlayerData *playerData=d2common_GetPlayerData(pUnit);
			QuestInfo *questInfo=playerData->quests[difficulty];
			__asm {
				mov eax, questInfo
				mov ecx, pUnit
				mov ebx, world
				call d2game_passClassicB9A30
			}
		}
	}
	if (world->expansion&&act>=5) {
		//qs[39]=0x19; //Rite of Passage
		//qs[40]=0x405; //Eve of Destruction
		int difficulty=world->difficulty;
		PlayerData *playerData=d2common_GetPlayerData(pUnit);
		QuestInfo *questInfo=playerData->quests[difficulty];
		if (0) {
			NetClient *client=playerData->client;
			int expansion=(client->flag&0x20)?1:0;
			int progress=(4+expansion)*difficulty+5;
			if (client->progress<progress) {
				client->progress=(client->progress&0xE0)|progress;
			}
		}
		//LOG("world=%X quest=%X\n",world,questInfo);
		__asm {
			mov eax, questInfo
			mov ebx, world
			mov ecx, pUnit
			call d2game_passExpansion58BE0
		}
	}
	__asm {
		mov eax, pUnit
		push world
		call d2game_sendQuestToClient
	}
	return 0;
}
static AreaRectData *getFreePos(AreaRectData *pData,POINT *src,POINT *dst,int unitsize) {
	__asm {
		push unitsize 
		push dst
		push pData
		mov esi,src
		call d2game_GetFreeXY
	}
}
int itemIndex2txt(int id) {
	if (1<=id&&id<1000) id=id-1;
	else if (1001<=id&&id<2000) id=*d2common_pWeaponsTxts+id-1001;
	else if (2001<=id&&id<3000) id=*d2common_pWeaponsTxts+*d2common_pArmorTxts+id-2001;
	else return -1;
	return id;
}
struct DropParam {
	UnitAny *pMon; //+00
	int off04; //+04 =0
	World *world; //+08
	int itemLevel; //+0C
	int off10; //+10
	int txt; //+14
	int off18; //+18 mon:3 npc:4
	int x,y; //+1C
	AreaRectData *pAreaData; //+24
	short off28; //+28 =1
	short itemFormat; //+2A =world[0x78]=0x65
	int notInStore; //+2C
	int quality; //+30
	int off34[3]; //+34
	int off40; //+40
	int itemFlag; //+44 0x8:socket
	int off48[2]; //+48
	int off50[4]; //+50
	int off60[4]; //+60
	int off70[4]; //+70
	int flags; //+80 bit0:HellBovine bit1:npc,notEthereal bit2:ethereal bit3:noSocket
};
static UnitAny *dropItem(World *world,UnitAny *pUnit,int itemIndex,int quality,int lv,int flags) {
	if (quality<0) {
		if (itemIndex<2000) quality=7;
		else quality=0;
	}
	int txt=itemIndex2txt(itemIndex);
	if (txt<0) {LOG("dropItem can't find txt for %d\n",itemIndex);return NULL;}
	//LOG("drop idx=%d txt=%d q=%d lv=%d\n",itemIndex,txt,quality,lv);
	/*	__asm {
			mov ebx, pUnit
			push 0
			push 0
			push quality
			push txt
			push world
			call d2game_DropItem
		}
	*/
	int arg3=0,arg4=0;
	DropParam param;if (sizeof(DropParam)!=0x84) LOG("FATAL ERROR DropItem size\n");
	memset(&param,0,sizeof(DropParam));
	POINT unitPos;d2common_getUnitPos(pUnit,&unitPos);
	AreaRectData *pData=d2common_getRectData(pUnit);
	POINT freePos;AreaRectData *pDataFree=getFreePos(pData,&unitPos,&freePos,1);
	if (!pDataFree) return NULL;
	//LOG("freePos %x(%d,%d)->%x(%d,%d)\n",pData,unitPos.x,unitPos.y,pDataFree,freePos.x,freePos.y);
	param.off04=0;
	param.itemFormat=world->itemFormat;
	param.txt=txt;
	param.pAreaData=pDataFree;
	param.x=freePos.x;
	param.pMon=pUnit;
	param.world=world;
	param.off18=3;
	param.y=freePos.y;
	param.off28=1;
	param.quality=quality;
	param.itemLevel=lv;
	param.off40=arg3;
	//flags|=4;//if HellBovine
	param.flags=flags;
	return d2game_DropItem1490(world,&param,0);
}
static int dropUniqueId=-1,dropSetId=-1;
#define MAX_NAMES 512
static int __fastcall selectUnique(int pos,int n) {
	int *cb=d2game_pDropChioceProb;
	if (0&&logfp) {
		int *p=cb;
		for (int i=0;i<n;i++,p+=2) {
			int uid=p[0],prob=p[1];
			//char *name=0<=uid&&uid<MAX_NAMES?uniqueNames[uid]:"";if (!name) name="";
			//fprintf(logfp,"DropUnique%d U%d prob=%d %s\n",i,uid,prob,name);
			//fflush(logfp);
		}
	}
	int *p=cb;
	for (int i=0;i<n;i++,p+=2) {
		int uid=p[0],prob=p[1];
		if (uid==dropUniqueId) return i+1;
	}
	return pos;
}
//009CF651 - 8B 34 CD 981FAD00     - mov esi,[ecx*8+00AD1F98] //ecx=1 to ebx
void __declspec(naked) SelectUniquePatch_ASM() {
	__asm {
		cmp dropUniqueId,0
		jl original
		push ebx
		push edx
		mov edx,ebx
		call selectUnique
		mov ecx,eax
		pop edx
		pop ebx
original:
		mov esi,d2game_pDropChioces
		mov esi,[esi+ecx*8]
		ret
	}
}
static int __fastcall selectSet(int *table,int n) {
	int *p=table;
	for (int i=0;i<n;i++,p+=2) {
		int id=p[0],prob=p[1];
		if (id==dropSetId) return id;
	}
	return -1;
}
/*
009D3D97 - 8B 44 C4 1C           - mov eax,[esp+eax*8+1C]
009D3D9B - 8B B4 24 20080000     - mov esi,[esp+00000820]
009D3DA2 - 50                    - push eax
009D3DA3 - 56                    - push esi
009D3DA4 - E8 0174FFFF           - call 009CB1AA { ->->6FD725F0 setItemFileIndex}
*/
void __declspec(naked) SelectSetPatch_ASM() {
	__asm {
		cmp dropSetId,0
		jl original
		lea ecx,[esp+0x28]
		mov edx,edi
		push esi
		call selectSet
		pop esi
		cmp eax,0
		jl original
		mov [esp+8],eax
original:
		jmp d2common_setItemFileIndex
		ret
	}
}
int getItemTxtByCode(int code) {
	int nTxt=*d2common_pItemTxtCount;
	ItemTxt *ptxt=*d2common_pItemTxts;
	for (int id=0;id<nTxt;ptxt=(ItemTxt *)((char *)ptxt+0x1a8),id++) {
		if (code==(int)ptxt->dwCode[0]) return id;
	}
	char buf[8];memcpy(buf,&code,4);buf[4]=0;
	LOG("Can't find code %s\n",buf);
	return -1;
}
void maximizeProps(UnitAny *pItem,ItemProp *props,int n) {
	for (int i=0;i<n;i++) {
		ItemProp *p=&props[i];if (p->PropId==-1) break;
		if (p->MinVal==p->MaxVal) continue;
		PropertiesTxt *ptxt=d2common_GetPropertiesTxt(p->PropId);if (!ptxt) continue;
		if (ptxt->wVal[0]) continue;
		short *pStatNo=ptxt->wStatNo;
		for (int i=0;i<7;i++) {
			short statId=*pStatNo++;if (statId<0) break;
			int v=d2common_GetUnitStat(pItem,statId,0);
			if (v<p->MinVal||v>p->MaxVal) {
				LOG("ERROR maximizeProps: not in range\n");continue;
			}
			d2common_changeStatList(pItem->pStatList,statId,p->MaxVal,0);
			if (pItem->pStatList->pNext)
				d2common_changeStatList(pItem->pStatList->pNext,statId,p->MaxVal,0);
		}
	}
}
RuneWordTxt *getRunewordTxt(int id) {
	if (id<0||id>=*d2common_pRuneWords) return NULL;
	RuneWordTxt *txt=*d2common_pRuneWordTxt;
	return &txt[id];
}
void pickupItem(World *world,UnitAny *player,UnitAny *pItem,int toHand) {
	if (!pItem) return;
	char packet[13];
	packet[0]=0x16;
	*(int *)&packet[1]=4;
	*(int *)&packet[5]=pItem->dwUnitId;
	*(int *)&packet[9]=toHand?1:0;
	d2game_recv16_pickup(world,player,packet,13);
}
void socketItem(World *world,UnitAny *player,UnitAny *pItem,UnitAny *pRune) {
	char packet[9];
	packet[0]=0x28;
	*(int *)&packet[1]=pItem->dwUnitId; //need check
	*(int *)&packet[5]=pRune->dwUnitId; //need check
	d2game_recv28_socket(world,player,packet,9);
}
void invToHand(World *world,UnitAny *player,UnitAny *pItem) {
	char packet[5];
	packet[0]=0x19;
	*(int *)&packet[1]=pItem->dwUnitId;
	d2game_recv19_invToHand(world,player,packet,13);
}
/*
	b<1-12>: put next item on body
	1:Helmet 2:Amulet 3:Armor 
	4:Weapon (Right) 5:Weapon (Left)
	6:Ring (Right) 7:Ring (Left)
	8:Belt 9:Boots 10:Gloves
	11:Alternate Weapon (Right) 12:Alternate Weapon (Left)
*/
void handToBody(World *world,UnitAny *player,UnitAny *pItem,int bodyLocation) {
	if (player->pInventory->pCursorItem!=pItem) return;
	char packet[9];
	packet[0]=0x1A;
	*(int *)&packet[1]=pItem->dwUnitId;
	*(int *)&packet[5]=bodyLocation;
	LOG("hand to body %d\n",bodyLocation);
	d2game_recv1A_handToBody(world,player,packet,9);
}
void putItemIntoSocket(World *world,UnitAny *pUnit,UnitAny *pEquipment,UnitAny *pRune) {
	int result;
	int eqId=pEquipment->dwUnitId;
	int runeId=pRune->dwUnitId;
	__asm {
		push 1
		push 1
		push 1
		push 1
		lea eax,result
		push eax
		push eqId
		push pUnit
		push world
		mov eax, runeId
		call d2game_putItemInSocketD4A70
	}
}
static UnitAny *dropRuneword(World *world,UnitAny *pUnit,int rwid,int idx,int flags) {
	RuneWordTxt *txt=getRunewordTxt(rwid);if (!txt) return NULL;
	UnitAny *pItem=dropItem(world,pUnit,idx,3,99,flags);
	int rtxt[6],n=0;
	for (int j=0;j<6;j++) {
		int rtxtid=txt->runeTxtId[j];if (rtxtid<=0) break;
		rtxt[n++]=rtxtid;
	}
	for (int i=0;i<3;i++) {
		int id=txt->wEType[i];if (!id) break;
		if (d2common_checkItemTypeFlags(pItem,id)) {
			sendMessageToClient(pUnit->pPlayerData->client,4,0,"<hackmap>","incompatible item");
			break;
			//return pItem;
		}
	}
	for (int i=0;i<6;i++) {
		int id=txt->wIType[i];if (!id) break;
		if (d2common_checkItemTypeFlags(pItem,id)) {
			sendMessageToClient(pUnit->pPlayerData->client,4,0,"<hackmap>","item ok");
			break;
			//return pItem;
		}
	}
	int maxSocket=d2common_getItemMaxSocket(pItem);if (maxSocket<n) return pItem;
	d2common_setItemFlag(pItem,ITEMFLAG_SOCKETED,1);
	d2common_setItemSocket(pItem,n);
	pickupItem(world,pUnit,pItem,0);
	for (int j=0;j<n;j++) {
		int ridx=GetItemIndex(rtxt[j])+1;
		UnitAny *rune=dropItem(world,pUnit,ridx,0,99,0);
		pickupItem(world,pUnit,rune,1);
		putItemIntoSocket(world,pUnit,pItem,rune);
	}
	if (d2common_CheckItemFlag(pItem,ITEMFLAG_RUNEWORD,0,"?")) {
		//maximizeProps(pItem,txt->Prop,7);
	}
	return pItem;
}
static UnitAny *dropUnique(World *world,UnitAny *pUnit,int uid,int lv,int flags) {
	//int ptable=*(int *)d2common_pDataTables;
	//int nUnique=*(int *)(ptable+0xC28);
	D2DataTables *table=*d2common_pDataTables;
	int nUnique=table->nUniqueItems;
	if (uid<0||uid>=nUnique) return NULL;
	//UniqueItemTxt *pu=(UniqueItemTxt *)table->pUniqueItemsTxt;//*(int *)(ptable+0xC24);
	//pu=(UniqueItemTxt *)((char *)pu+0x14C*uid);
	UniqueItemTxt *pu=&table->pUniqueItemsTxt[uid];
	if (lv<pu->wQlvl) lv=pu->wQlvl;
	int code=*(int *)pu->szCode;
	int txt=getItemTxtByCode(code);
	if (txt<0) return NULL;
	int idx=GetItemIndex(txt)+1;
	world->uniqueDroppedBitmap[uid>>3]=0;
	//char *flag=(char *)world+0x1B24+(uid>>3);*flag=0; //clear unique dropped flag
	LOG("drop unique world=%X %d/%d txt=%d idx=%d\n",world,uid,nUnique,txt,idx);
	dropUniqueId=uid;
	UnitAny *pItem=dropItem(world,pUnit,idx,7,lv,flags);
	dropUniqueId=-1;
	//maximizeProps(pItem,pu->Prop,12);
	return pItem;
}
static UnitAny *dropSet(World *world,UnitAny *pUnit,int sid,int lv,int flags) {
	//int ptable=*(int *)d2common_pDataTables;
	//int nSet=*(int *)(ptable+0xC1C);
	D2DataTables *table=*d2common_pDataTables;
	int nSet=table->nSetItems;
	if (sid>=nSet) return NULL;
	//int pSets=*(int *)(ptable+0xC18);
	//SetItemTxt *pu=(SetItemTxt *)(pSets+0x1B8*sid);
	SetItemTxt *pu=&table->pSetItemsTxt[sid];
	if (lv<pu->wQlvl) lv=pu->wQlvl;
	int code=*(int *)pu->szCode;
	int txt=getItemTxtByCode(code);
	if (txt<0) return NULL;
	int idx=GetItemIndex(txt)+1;
	//LOG("drop set %d/%d txt=%d idx=%d\n",sid,nSet,txt,idx);
	dropSetId=sid;
	UnitAny *pItem=dropItem(world,pUnit,idx,5,lv,flags);
	dropSetId=-1;
	//maximizeProps(pItem,pu->Prop,9);
	return pItem;
}
int autoPortal(World* world,UnitAny* pUnit) {
	unsigned short *qs=getQuest(world,pUnit);
	AreaRectData* pData = d2common_getRectData(pUnit);
	DrlgLevel* pLvl = pData->pAreaRectInfo->pDrlgLevel;
	switch (pLvl->dwLevelNo) {
		case Level_RogueEncampment:
			if (qs&&!(qs[6]&1)&&!hasPortalNearby(pData,Level_CatacombsLevel4)) return Level_CatacombsLevel4;
			if (!hasPortalNearby(pData,Level_TheSecretCowLevel)) return Level_TheSecretCowLevel;
			break;
		case Level_LutGholein:
			if (qs&&!(qs[13]&1)) {
				if (!unitHasItem(pUnit,HoradricStaff)) {
					if (!unitHasItem(pUnit,StaffOfKing)&&!hasPortalNearby(pData,Level_MaggotLairLevel3))
						return Level_MaggotLairLevel3;
					else if (!unitHasItem(pUnit,ViperAmulet)&&!hasPortalNearby(pData,Level_ClawViperTempleLevel2)) 
						return Level_ClawViperTempleLevel2;
				}
				if (!hasPortalNearby(pData,Level_ArcaneSanctuary)) return Level_ArcaneSanctuary;
			}
			if (!hasPortalNearby(pData,ACT->pDrlgMisc->dwStaffTombLvl)) return ACT->pDrlgMisc->dwStaffTombLvl;
			else if (!hasPortalNearby(pData,Level_SewersLevel3)) return Level_SewersLevel3;
			break;
		case Level_KurastDocks:
			if (qs&&!(qs[21]&1)) {
				if (!unitHasItem(pUnit,KhalimsWill)) {
					if (!unitHasItem(pUnit,KhalimsEye)&&!hasPortalNearby(pData,Level_SpiderCavern)) return Level_SpiderCavern;
					if (!unitHasItem(pUnit,KhalimsBrain)&&!hasPortalNearby(pData,Level_FlayerDungeonLevel3)) 
					if (!unitHasItem(pUnit,KhalimsHeart)&&!hasPortalNearby(pData,Level_KurastSewersLevel2)) 
						return Level_KurastSewersLevel2;
				}
				//!unitHasItem(pUnit,KhalimsFlail)
				if (!hasPortalNearby(pData,Level_Travincal)) return Level_Travincal;
			}
			if (!hasPortalNearby(pData,Level_DuranceofHateLevel3)) return Level_DuranceofHateLevel3;
			break;
		case Level_ThePandemoniumFortress:
			if (!hasPortalNearby(pData,Level_ChaosSanctuary)) return Level_ChaosSanctuary;
			break;
		case Level_Harrogath:
			if (qs&&!(qs[39]&1)&&!hasPortalNearby(pData,Level_ArreatSummit)) return Level_ArreatSummit;
			if (!hasPortalNearby(pData,Level_ThroneofDestruction)) return Level_ThroneofDestruction;
			if (world->difficulty>=2) {
				if (!hasPortalNearby(pData,Level_MatronDen)) return Level_MatronDen;
				else if (!hasPortalNearby(pData,Level_ForgottenSands)) return Level_ForgottenSands;
				else if (!hasPortalNearby(pData,Level_FurnaceofPain)) return Level_FurnaceofPain;
				else if (!hasPortalNearby(pData,Level_Tristram6boss)) return Level_Tristram6boss;
			}
			break;
	}
	return 0;
}
int portalToLevel(World* world, UnitAny* pUnit, int levelID);
static int monsterSleep=0;
/*
00A5D4DC - 8D 44 24 10           - lea eax,[esp+10]
00A5D4E0 - 50                    - push eax <-AIParam* ptAIArgs
00A5D4E1 - 8B 44 24 14           - mov eax,[esp+14] //
00A5D4E5 - 8B D7                 - mov edx,edi <-UnitAny* ptMonster
00A5D4E7 - 8B CD                 - mov ecx,ebp <-World* ptGame
00A5D4E9 - FF 50 04              - call dword ptr [eax+04] <---------
*/
void __declspec(naked) MonsterSleepPatch_ASM() {
	__asm {
		cmp monsterSleep,0
		jne skip
		mov ecx,ebp
		jmp dword ptr [eax+4]
skip:
		jmp d2game_UberDiabloAI
	}
}

struct d2stat {short id,param;int value;};
struct d2stats {
	d2stat stats[128];
	int n;
};
static void setItemStats(UnitAny *pItem,d2stats *pstats) {
	if (!pItem->pStatList||!(pItem->pStatList->dwListFlag&0x80000000)) return;
	if (pItem->pItemData->dwQuality!=5&&pItem->pItemData->dwQuality!=7) {
		pItem->pStatList->sBaseStat.wStats=0;
		pItem->pStatList->sFullStat.wStats=0;
		if (pItem->pStatList->pNext) {
			pItem->pStatList->pNext->sBaseStat.wStats=0;
			pItem->pStatList->pNext->sFullStat.wStats=0;
		}
	}
	for (int i=0;i<pstats->n;i++) {
		struct d2stat *p=&pstats->stats[i];
		int id=p->id;int value=p->value;int param=p->param;
		d2common_changeStatList(pItem->pStatList,id,value,param);
		if (pItem->pStatList->pNext)
			d2common_changeStatList(pItem->pStatList->pNext,id,value,param);
	}
}
static int setStat(struct d2stats *pstats,int id,int param,int value) {
	int maxvalue=getStatMaxValue(id);
	if (!value||value>maxvalue) value=maxvalue;
	if (pstats->n>=128) return 0;
	struct d2stat *p=&pstats->stats[pstats->n++];
	p->id=id;p->param=param;p->value=value;
	return 1;
}
static char *parseStats(char *s,struct d2stats *pstats) {
	pstats->n=0;
	char *end=strchr(s,'}');if (end) {*end=0;end++;}
	char *n=NULL;
	for (;s;s=n) {
		n=strchr(s,',');if (n) {*n=0;n++;}
		int id=-1,param=0;char *name=NULL,*nameend;
		if ('0'<=*s&&*s<='9') {
			id=strtol(s,&s,10);
		} else {
			name=s;
			while ('a'<=*s&&*s<='z'||'A'<=*s&&*s<='Z') s++;
			nameend=s;
		}
		if (*s=='-'||*s==':') {
			s++;param=strtol(s,&s,0);
			if (*s=='-'||*s==':') {
				s++;param=strtol(s,&s,0)&0x3f|(param<<6);
			}
		}
		int value=0;
		if (*s=='=') {s++;value=strtol(s,&s,10);}
		if (id>=0) {
			if (!setStat(pstats,id,param,value)) break;
		} else {
			*nameend=0;
			if (strcmp(name,"str")==0) {
				if (!setStat(pstats,STAT_STRENGTH,param,value)) break;
			} else if (strcmp(name,"eng")==0) {
				if (!setStat(pstats,STAT_ENERGY,param,value)) break;
			} else if (strcmp(name,"dex")==0) {
				if (!setStat(pstats,STAT_DEXTERITY,param,value)) break;
			} else if (strcmp(name,"vit")==0) {
				if (!setStat(pstats,STAT_VITALITY,param,value)) break;
			} else if (strcmp(name,"hp")==0) {
				if (!setStat(pstats,STAT_MAXHP,param,value)) break;
			} else if (strcmp(name,"mana")==0) {
				if (!setStat(pstats,STAT_MAXMANA,param,value)) break;
			} else if (strcmp(name,"dam")==0) {
				if (!setStat(pstats,STAT_EnhancedMinDamage,param,value)) break;
				if (!setStat(pstats,STAT_EnhancedDamage,param,value)) break;
				if (!setStat(pstats,STAT_AttackRating,param,value)) break;
				for (int i=21;i<=24;i++)
					if (!setStat(pstats,i,param,value)) break;
				for (int i=48;i<=55;i++)
					if (!setStat(pstats,i,param,value)) break;
				if (!setStat(pstats,111,param,value)) break;
				for (int i=305;i<=308;i++)
					if (!setStat(pstats,i,param,value)) break;
				for (int i=329;i<=336;i++)
					if (!setStat(pstats,i,param,value)) break;
			} else if (strcmp(name,"def")==0) {
				if (!setStat(pstats,STAT_Defense,param,value)) break;
				if (!setStat(pstats,STAT_EnhancedDefense,param,value)) break;
				if (!setStat(pstats,STAT_DAMAGE_REDUCED,param,value)) break;
			} else if (strcmp(name,"resist")==0) {
				if (!setStat(pstats,STAT_FIRE_RESIST,param,value)) break;
				if (!setStat(pstats,STAT_LIGHTING_RESIST,param,value)) break;
				if (!setStat(pstats,STAT_COLD_RESIST,param,value)) break;
				if (!setStat(pstats,STAT_POSION_RESIST,param,value)) break;
				if (!setStat(pstats,STAT_MAX_FIRE_RESIST,param,value)) break;
				if (!setStat(pstats,STAT_MAX_LIGHTING_RESIST,param,value)) break;
				if (!setStat(pstats,STAT_MAX_COLD_RESIST,param,value)) break;
				if (!setStat(pstats,STAT_MAX_POSION_RESIST,param,value)) break;
				if (!setStat(pstats,STAT_FIRE_ABSORB,param,value)) break;
				if (!setStat(pstats,STAT_LIGHTING_ABSORB,param,value)) break;
				if (!setStat(pstats,STAT_COLD_ABSORB,param,value)) break;
			} else if (strcmp(name,"LL")==0) {
				if (!setStat(pstats,STAT_LIFE_LEECH,param,value)) break;
			} else if (strcmp(name,"LM")==0) {
				if (!setStat(pstats,STAT_MANA_LEECH,param,value)) break;
			} else if (strcmp(name,"req")==0) {
				if (!value) value=-100;
				if (!setStat(pstats,STAT_Requirement,param,value)) break;
			} else if (strcmp(name,"IAS")==0) {
				if (!setStat(pstats,STAT_IAS,param,value)) break;
			} else if (strcmp(name,"FRW")==0) {
				if (!setStat(pstats,STAT_FRW,param,value)) break;
			} else if (strcmp(name,"FHR")==0) {
				if (!setStat(pstats,STAT_FHR,param,value)) break;
			} else if (strcmp(name,"FBR")==0) {
				if (!setStat(pstats,STAT_FBR,param,value)) break;
			} else if (strcmp(name,"FCR")==0) {
				if (!setStat(pstats,STAT_FCR,param,value)) break;
			} else if (strcmp(name,"ITD")==0) {
				if (!setStat(pstats,STAT_ITD,param,value)) break;
			} else if (strcmp(name,"PHM")==0) {
				if (!setStat(pstats,STAT_PHM,param,value)) break;
			} else if (strcmp(name,"skills")==0) {
				if (!setStat(pstats,STAT_AllSkills,param,value)) break;
			} else if (strcmp(name,"OW")==0) {
				if (!setStat(pstats,STAT_OW,param,value)) break;
			} else if (strcmp(name,"CB")==0) {
				if (!setStat(pstats,STAT_CB,param,value)) break;
			} else if (strcmp(name,"slow")==0) {
				if (!value) value=99;
				if (!setStat(pstats,150,param,value)) break;
			} else if (strcmp(name,"indest")==0) { //Indestructible
				if (!setStat(pstats,152,param,value)) break;
			} else if (strcmp(name,"notFrozen")==0) {
				if (!setStat(pstats,153,param,value)) break;
			} else if (strcmp(name,"aura")==0) {
				if (value<96||value>125) value=123;
				if (!setStat(pstats,151,param,value)) break;
			} else if (strcmp(name,"pierce")==0) {
				if (!setStat(pstats,156,param,value)) break;
			} else if (strcmp(name,"explode")==0) {
				if (!setStat(pstats,158,param,value)) break;
			} else {
				LOG("Can't find stat name %s\n",name);
			}
		}
	}
	return end;
}
int serverUndo(World *world,UnitAny *player,char *msg,char *end);
static void pickupItemTo(World *world,UnitAny *player,UnitAny *pItem,int bodyId) {
	if (!pItem) return;
	if (!bodyId) {pickupItem(world,player,pItem,0);return;}
	if (pItem->dwMode==ItemMode_OnGound) {
		pickupItem(world,player,pItem,1);
		handToBody(world,player,pItem,bodyId);
	} else if (pItem->dwMode==ItemMode_Inventory) {
		invToHand(world,player,pItem);
		handToBody(world,player,pItem,bodyId);
	}
}
extern int fEnableAdminCmd;
int __fastcall skipServerPacket15(World *world,UnitAny *player,char *packet,int len) {
	UnitAny *pItem=NULL;
	struct d2stats stats,socketStat;
	stats.n=0;socketStat.n=0;
	char *msg=packet+3;char *end=packet+len-3;
	if (0&&logfp) {
		LOG("server recv chat %x %x %d\n",world,player,len);
		hex(logfp,0,packet,len);fflush(logfp);
	}
	if (serverUndo(world,player,msg,end)) return 1;
	if (!fEnableAdminCmd) return 0;
	int plen=strlen(adminCmdPrefix);
	if (memcmp(adminCmdPrefix,msg,plen)!=0) return 0;
	char *s=msg+plen;
	int q=-1,socket=-1,lv=0,t,nItems=0,flags=0,err=0,bodyId=0;
	int levelId=d2common_GetLevelIdFromRectData(d2common_getRectData(player));
	lv=d2common_getAreaLevel(levelId,world->difficulty,world->expansion);
	if (!lv) lv=d2common_GetUnitStat(player,STAT_LEVEL,0);
	if (lv<1) lv=1;
	while (s<end) {
		while ((*s==' '||*s==',')&&s<end) s++;if (s>=end) break;
		if (memcmp(s,"ub",2)==0) {
			s+=2;portalToLevel(world,player,Level_Tristram6boss);
		} else if (memcmp(s,"pass",4)==0) {
			s+=4;int act=5;
			if ('0'<=s[0]&&s[0]<='9') act=strtol(s,&s,0);
			passGame(world,player,act);break;
		} else if (memcmp(s,"waypoints",9)==0) {
			s+=9;enableAllWaypoints(player);
		} else if (memcmp(s,"sleep",5)==0) {
			s+=3;monsterSleep=1;
			for (NetClient *client=world->client;client;client=client->ptNextClient)
				sendMessageToClient(client,4,0,"<admin>","monsters stop moving");
			break;
		} else if (memcmp(s,"move",4)==0) {
			s+=4;monsterSleep=0;
			for (NetClient *client=world->client;client;client=client->ptNextClient)
				sendMessageToClient(client,4,0,"<admin>","monsters start moving");
			break;
		} else if (memcmp(s,"up",2)==0) {
			s+=2;maxCharStat(world,player);
		} else if (memcmp(s,"stat{",5)==0) {
			s+=5;s=parseStats(s,&stats);if (!s) break;
		} else if (memcmp(s,"socket{",7)==0) {
			s+=7;s=parseStats(s,&socketStat);if (!s) break;
		} else if (memcmp(s,"lv",2)==0&&'0'<=s[2]&&s[2]<='9') {
			s+=2;int lv=strtol(s,&s,0);
			setCharLv(world,player,lv);
		} else {
			char cmd=*s++;
			switch (cmd) {
				case 'e':flags|=4;break;
				case 'q':q=strtol(s,&s,10);break;
				case 'l':
					if ('1'<=*s&&*s<='9') lv=strtol(s,&s,10);
					break;
				case 'o':socket=strtol(s,&s,10);break;
				case 'p':
					if ('0'<=*s&&*s<='9') t=strtol(s,&s,10);
					else t=autoPortal(world,player);
					if (t) portalToLevel(world,player,t);
					break;
				case 'r':
					if ('1'<=*s&&*s<='9') {
						t=strtol(s,&s,10);
						if (1<=t&&t<=33) {
							pickupItem(world,player,dropItem(world,player,2102+t,0,lv,0),0);
						}
					} else {
						pickupItem(world,player,dropItem(world,player,2009,0,lv,0),0);
					}
					break;
				case 'w': {
					int rwId=strtol(s,&s,10);
					if (*s!='i') break;
					s++;int idx=strtol(s,&s,10);
					pItem=dropRuneword(world,player,rwId,idx,flags);
					pickupItemTo(world,player,pItem,bodyId);bodyId=0;
					break;
				}
				case 'b':bodyId=strtol(s,&s,10);if (bodyId<0||bodyId>12) bodyId=0;break;
				case 'i':case 'u':case 'U':case 's':case 'S': {
					if (!('0'<=*s&&*s<='9')) break;
					t=strtol(s,&s,10);
					int sk=socket;
					if (pItem&&pItem->dwMode==ItemMode_OnGound) pickupItem(world,player,pItem,0);
					if (cmd=='u'||cmd=='U') {pItem=dropUnique(world,player,t,lv,flags);if (sk<0) sk=6;}
					else if (cmd=='s'||cmd=='S') {pItem=dropSet(world,player,t,lv,flags);if (sk<0) sk=6;}
					else {pItem=dropItem(world,player,t,q,lv,flags);if (q>3&&sk<0) sk=6;}
					if (!pItem) break;
					nItems++;
					pItem->pItemData->dwItemFlags|=ITEMFLAG_IDENTIFIED;
					if (stats.n) setItemStats(pItem,&stats);
					if (sk>=0) {
						int maxSocket=d2common_getItemMaxSocket(pItem);if (sk>maxSocket) sk=maxSocket;
						d2common_setItemFlag(pItem,ITEMFLAG_SOCKETED,sk>0);
						if (sk) d2common_setItemSocket(pItem,sk);
					}
					if (flags&4) d2common_setItemEthereal(pItem);
					if (socketStat.n) {
						int n=d2common_getSocketCount(pItem);
						if (n>6) n=6;
						pickupItem(world,player,pItem,0);
						for (int i=0;i<n;i++) {
							UnitAny *pJ=dropItem(world,player,2136,0,lv,0);if (!pJ) break;
							pJ->pItemData->dwItemFlags|=ITEMFLAG_IDENTIFIED;
							setItemStats(pJ,&socketStat);
							pickupItem(world,player,pJ,1);
							putItemIntoSocket(world,player,pItem,pJ);
						}
					}
					pickupItemTo(world,player,pItem,bodyId);bodyId=0;
					break;
				}
				default:err=1;break;
			}
		}
	}
	return !err;
}
/*
ecx:world
edx:pUnit
packet:[ebp+8]
len:[ebp+c]
00A8DBB0 - 55                    - push ebp
00A8DBB1 - 8B EC                 - mov ebp,esp
00A8DBB3 - 83 E4 F8              - and esp,-08 { 248 }
00A8DBB6 - 81 EC 9C030000        - sub esp,0000039C { 924 }
00A8DBBC - 53                    - push ebx
00A8DBBD - 56                    - push esi
00A8DBBE - 57                    - push edi
00A8DBBF - 89 4C 24 40           - mov [esp+40],ecx
00A8DBC3 - 33 C0                 - xor eax,eax
00A8DBC5 - B9 44000000           - mov ecx,00000044 { 68 } <---
*/
void __declspec(naked) ServerRecvPacket15Patch_ASM() {
	__asm {
		pushad
		mov eax,dword ptr [ebp+0xC]
		push eax
		mov eax,dword ptr [ebp+8]
		push eax
		call skipServerPacket15 
		test eax,eax
		popad
		jnz skip
		mov ecx,0x44
		ret
skip:
		pop ecx
		pop edi
		pop esi
		pop ebx
		mov esp,ebp
		pop ebp
		ret 8
	}
}

