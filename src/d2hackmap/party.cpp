#include "stdafx.h"
#include "header.h"

int dwCheckRelationMs=0,hasHostilePlayer=0;
int dwPartyInvideMs;
void ResponseInvite();
int fCanInvite=0,mAutoPartyDelays=500,dwPartyResponseMs=0;
ToggleVar tAutoParty={TOGGLEVAR_ONOFF,	0,	-1, 1,	"Auto Party Toggle"};
ToggleVar tAutoInvite={TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Invite Toggle"};
ToggleVar tAutoPermit={TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Loot Permit Toggle"};
ToggleVar tAutoLocalPermit={TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Local Loot Permit Toggle"};
ToggleVar tAutoPartyBugMF={TOGGLEVAR_ONOFF,	0,	-1, 1,	"Auto Party BugMF Toggle"};
ToggleVar tHardCoreBackTownIfAnyoneDie={TOGGLEVAR_ONOFF,	0,	-1, 1,	"HardCoreBackTownIfAnyoneDie"};
char autoPartyLevelRange[3][2]={{1,99},{1,99},{1,99}};
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY,"HardCoreBackTownIfAnyoneDie",&tHardCoreBackTownIfAnyoneDie},
  {CONFIG_VAR_TYPE_KEY,"AutoPartyBugMFToggle",&tAutoPartyBugMF},
  {CONFIG_VAR_TYPE_KEY,"AutoPartyToggle",&tAutoParty},
  {CONFIG_VAR_TYPE_KEY,"AutoInviteToggle",&tAutoInvite},
  {CONFIG_VAR_TYPE_KEY,"AutoLootPermit",&tAutoPermit},
  {CONFIG_VAR_TYPE_KEY,"AutoLocalLootPermit",&tAutoLocalPermit},
  {CONFIG_VAR_TYPE_INT,"AutoPartyDelays",&mAutoPartyDelays, 4},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0,"AutoPartyLevelRange",&autoPartyLevelRange,2,{3}},
};
void PartyHelp_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
struct AutoPartyUid {int uid,lv;char name[20];struct AutoPartyUid *next;};
static AutoPartyUid *autoInvites=NULL,*acceptInvites=NULL,*recycleAutoParty=NULL;
void PartyHelp_initConfigVars() {
}
extern int dwBugFlag;
static int autoPartyBugMF() {
	if (DIFFICULTY==2&&dwBugFlag&&!tAutoPartyBugMF.isOn) {
		if (dwBugFlag&(1<<dwCurrentAct)) {
			int qid=0;
			switch (dwCurrentAct) {
				case 0:qid=5;break;
				case 2:qid=22;break;
				case 3:qid=26;break;
				case 4:qid=40;break;
			}
			if (!qid) return 1;
			if (GAMEQUESTDATA->quests[qid]&0x8000) return 1;
			return 0;
		}
	}
	return 1;
}
static int CheckInvite() {
	if (dwPlayerId==1) return 1;
	for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext) {
		if (pUnit->dwUnitId<dwPlayerId) return 0; //自己不是最小编号，放弃邀请权利
	}
	return 1;
}
static void invitePlayerLater(int uid,char *name,int lv) {
	AutoPartyUid *p;
	if (recycleAutoParty) {p=recycleAutoParty;recycleAutoParty=p->next;}
	else p=(AutoPartyUid *)HeapAlloc(dllHeap,0,sizeof(struct AutoPartyUid));//gameHeap may not be valid yet
	p->uid=uid;p->lv=lv;memcpy(p->name,name,16);p->name[16]=0;p->next=autoInvites;autoInvites=p;
	dwPartyInvideMs=dwCurMs+mAutoPartyDelays;
}
static void setLootPermit(int uid,char *name) {
	if (!dwPlayerId&&PLAYER) dwPlayerId=PLAYER->dwUnitId;
	if (uid==dwPlayerId) return;
	int isLocal=isLocalPlayerName(name);
	int permit=tAutoPermit.isOn||tAutoLocalPermit.isOn&&isLocal;
	int old=0;
	for (RosterUnit *pUnit=PLAYERLIST;pUnit;pUnit=pUnit->pNext) {
		if (pUnit->dwUnitId!=dwPlayerId) continue;
		if (!pUnit->pPvPInfo) break;
		for (PvPInfo *pPvPInfo=*(pUnit->pPvPInfo);pPvPInfo;pPvPInfo=pPvPInfo->pNext) {
			if (pPvPInfo->dwUnitId==uid) {
				old=(pPvPInfo->dwFlag&1)?1:0;
				break;
			}
		}
		break;
	}
	if (permit!=old) {
		LOG("LootPermit %s local=%d permit=%d->%d\n",name,isLocal,old,permit);
		if (dwGameLng) gameMessageWColor(2,L"%s%s%hs搜括尸体",permit?L"允许":L"禁止",
			isLocal?L"本机号":L"陌生人",name);
		else gameMessageWColor(2,L"%hs %hs %hs to loot",permit?"Allow":"Forbid",isLocal?"local":"alien",name);
		BYTE packet[7];packet[0]=0x5D;packet[1]=1;packet[2]=permit?1:0;*(int *)(packet+3)=uid;
		SendPacket(packet,sizeof(packet));
	}
}
void PartyHelpNewGame() {
	while (autoInvites) {
		AutoPartyUid *p=autoInvites;autoInvites=p->next;p->next=recycleAutoParty;recycleAutoParty=p;
	}
	if (CheckInvite()) fCanInvite=1;
	for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext) {
		if (pUnit->dwUnitId==dwPlayerId) continue;
		LOG("invite %s later\n",pUnit->szName);
		invitePlayerLater(pUnit->dwUnitId,pUnit->szName,pUnit->wLevel);
	}
}
void PartyHelpEndGame() {fCanInvite=0;}
void ResponseInvite() {
	if (!tAutoParty.isOn||!acceptInvites||dwCurMs<dwPartyResponseMs) return;
	if (!autoPartyBugMF()) return;
	refresh_clients();
	while (acceptInvites) {
		AutoPartyUid *p=acceptInvites;acceptInvites=p->next;p->next=recycleAutoParty;recycleAutoParty=p;
		int uid=p->uid;
		RosterUnit *runit=getRosterUnit(uid);
		if (!runit) continue;
		static int print=0,printMs=0;
		if (!printMs||dwCurMs>printMs) {
			print=0;printMs=dwCurMs+10000;
		}
		if (print++ < 10)
			LOG("party request from uid=%d lv=%d name=%s difficulty=%d range=(%d-%d)\n",
				uid,runit->wLevel,runit->szName,DIFFICULTY,
				autoPartyLevelRange[DIFFICULTY][0],autoPartyLevelRange[DIFFICULTY][1]);
		if (runit->wLevel<autoPartyLevelRange[DIFFICULTY][0]||autoPartyLevelRange[DIFFICULTY][1]<runit->wLevel) {
			if (!isLocalPlayerName(runit->szName)) {
				if (print<10) LOG("%d %s is not local player\n",uid,runit->szName);
				continue;
			}
		}
		LOG("accept invite from %d\n",uid);
		BYTE packet[6]={0x5E,0x08};*(int *)&packet[2]=uid;
		SendPacket(packet,sizeof(packet));
	}
	dwPartyResponseMs=0;
}
extern int fPartyListValid;
static void __stdcall recv5A(char *packet) {
	fPartyListValid=0;
	switch (packet[1]) {
		case 0:case 3://somebody left
			if(CheckInvite()) fCanInvite=1; 
			break;
		case 6: 
			if (packet[2]==4) {//someone is dead
				int killerId=*(int *)(packet+3);
				int killerType=packet[7];
				char *playerName=packet+8;
				char *killerName=packet+0x18;
				if (fIsHardCoreGame&&tHardCoreBackTownIfAnyoneDie.isOn) {
					if (!fPlayerInTown&&PLAYER&&PLAYER->dwMode!=PlayerMode_Death&&PLAYER->dwMode!=PlayerMode_Dead)
						BackToTown();
				}
			}
			break;
		case 7:
			if (tAutoParty.isOn&&packet[2]==0x02&&packet[7]==0x05) {
				if (!autoPartyBugMF()) return;
				// Player Is Asking To Party With You
				// Player ID = packet[3~6]
				// Send packet back: 5E 08 xx xx xx xx
				int uid=*(int *)&packet[3];
				LOG("accept invite %d later\n",uid);
				AutoPartyUid *p;
				if (recycleAutoParty) {p=recycleAutoParty;recycleAutoParty=p->next;}
				else p=(AutoPartyUid *)HeapAlloc(dllHeap,0,sizeof(struct AutoPartyUid));//gameHeap may not be valid yet
				p->uid=uid;p->name[0]=0;p->next=acceptInvites;acceptInvites=p;
				dwPartyResponseMs=dwCurMs+mAutoPartyDelays;
			}
			break;
	}
}
void __declspec(naked) RecvCommand_5A_Patch_ASM() {
	__asm {
		pop eax  //ret code
		sub esp, 0x28
		push esi
		push edi
		push eax //ret code
		push ecx
		push ecx;
		call recv5A;
		pop ecx
		ret
	}
}
/*
Can't rob equipment after dead
[1.467] SEND Squelch/Hostile cmd_5d 1 len=7 ret=0x6FAC44A6
	00: 5d 01 00 02 - 00 00 00    -             -             |]               
[1.514] RECV cmd_8c len=11 RelationshipUpdate
	00: 8c 01 00 00 - 00 02 00 00 - 00 00 00    -             |                
Can rob equipment after dead
[3.245] SEND Squelch/Hostile cmd_5d 1 len=7 ret=0x6FAC44A6
	00: 5d 01 01 02 - 00 00 00    -             -             |]               
[3.323] RECV cmd_8c len=11 RelationshipUpdate
	00: 8c 01 00 00 - 00 02 00 00 - 00 01 00    -             |                
Don't send message to player(mouth button)
	[1.544] SEND Squelch/Hostile cmd_5d 1 len=7 ret=0x6FAC44A6
		00: 5d 02 01 02 - 00 00 00    -             -             |]               
	[1.654] RECV cmd_8c len=11 RelationshipUpdate
		00: 8c 01 00 00 - 00 02 00 00 - 00 02 00    -             |                
Send message to player(mouth button)
	[2.402] SEND Squelch/Hostile cmd_5d 1 len=7 ret=0x6FAC44A6
		00: 5d 02 00 02 - 00 00 00    -             -             |]               
	[2.480] RECV cmd_8c len=11 RelationshipUpdate
		00: 8c 01 00 00 - 00 02 00 00 - 00 00 00    -             |                
Don't show message from player(ear button)
	[3.510] SEND Squelch/Hostile cmd_5d 1 len=7 ret=0x6FAC44A6
		00: 5d 03 01 02 - 00 00 00    -             -             |]               
	[3.650] RECV cmd_8c len=11 RelationshipUpdate
		00: 8c 01 00 00 - 00 02 00 00 - 00 04 00    -             |                
Show message from player(ear button)
	[4.196] SEND Squelch/Hostile cmd_5d 1 len=7 ret=0x6FAC44A6
		00: 5d 03 00 02 - 00 00 00    -             -             |]               
	[4.290] RECV cmd_8c len=11 RelationshipUpdate
		00: 8c 01 00 00 - 00 02 00 00 - 00 00 00    -             |                
*/
void AutoInvite() {
	if (!autoInvites||dwCurMs<dwPartyInvideMs) return;
	if (!dwPlayerId&&PLAYER) dwPlayerId=PLAYER->dwUnitId;
	refresh_clients();
	while (autoInvites) {
		AutoPartyUid *p=autoInvites;autoInvites=p->next;p->next=recycleAutoParty;recycleAutoParty=p;
		int local=isLocalPlayerName(p->name);
		int uid=p->uid;
		int lv=p->lv;
		if (PLAYER&&uid==PLAYER->dwUnitId) continue;
		RosterUnit *runit=getRosterUnit(uid);
		LOG("AutoInvite uid=%d lv=%d name=%s local=%d\n",p->uid,p->lv,p->name,local);
		if (tAutoInvite.isOn&&fCanInvite&&autoPartyBugMF()&&runit&&!runit->dwPartyFlags) {
			int doInvite=0;
			if (local) {
				doInvite=1;
			} else if (autoPartyLevelRange[DIFFICULTY][0]<=lv&&lv<=autoPartyLevelRange[DIFFICULTY][1]) {
				doInvite=1;
			} else {
				doInvite=0;
			}
			LOG("%s %s: lv%d range(%d-%d) invite=%d\n",local?"local":"alien",p->name,p->lv,
				autoPartyLevelRange[DIFFICULTY][0],autoPartyLevelRange[DIFFICULTY][1],doInvite);
			if (doInvite) {
				LOG("send invite to %d\n",uid);
				//自动邀请
				BYTE InvitePlayer[6]={0x5E,0x06};*(int *)&InvitePlayer[2] = uid;
				SendPacket(InvitePlayer,sizeof(InvitePlayer));		
			} else {
				gameMessage("Didn't invite %s %s: lv%d range(%d-%d)",local?"local":"alien",p->name,p->lv,
					autoPartyLevelRange[DIFFICULTY][0],autoPartyLevelRange[DIFFICULTY][1]);
			}
		}
		if (fInGame&&fIsHardCoreGame) {
			setLootPermit(p->uid,p->name);
		}
	}
	dwPartyInvideMs=0;
}
//Player In Game 5b [WORD Packet Length] [DWORD Player Id] [BYTE Char Type] [NULLSTRING[16] Char Name] [WORD Char Lvl] [WORD Party Id] 00 00 00 00 00 00 00 00 
static void __fastcall recvCommand5B(BYTE *packet) {
	int uid=*(int *)&packet[3];
	if (!dwPlayerId&&PLAYER) dwPlayerId=PLAYER->dwUnitId;
	if (uid==dwPlayerId) return;
	char *name=(char *)packet+8;
	short lv=*(short *)(packet+0x18);
	LOG("playerJoin uid=%d lv=%d name=%s\n",uid,lv,name);
	//check later, don't know whether it is local player or not at this point
	if (fInGame) {
		invitePlayerLater(uid,name,lv);
	}
}
void __declspec(naked) RecvCommand_5B_Patch_ASM() {
	__asm {
		//ecx = cmdbuf
		pop eax
		push esi
		push eax //ret code
		push ecx
		call recvCommand5B
		pop ecx
		lea esi, dword ptr [ecx+22]
		mov eax, esi
		ret
	}
}

int saveRuntimeInfo1(HWND hwnd,int uid);
static void __fastcall assignPlayer(char *packet) {
	int uid=*(int *)(packet+1);
	char *name=packet+6;
	LOG("assign player id=%d name=%s\n",uid,name);
	GameStructInfo *pinfo=*d2client_pGameInfo;
	if (!pinfo) return;
	if (strcmp(pinfo->szCharName,name)==0) {
		saveRuntimeInfo1(d2gfx_GetHwnd(),uid);
		if (!dwPlayerId&&PLAYER) dwPlayerId=PLAYER->dwUnitId;
	}
}
/*
[2.637] RECV cmd_59 len=26 AssignPlayer
	00: 59 01 00 00 - 00 01 <char name>
	10: 00 00 00 00 - 00 00 00 00 - 00 00       -             |                
	d2client_ADF20: 0F B7 51 18        movzx edx, word ptr [ecx+0x18]
	d2client_ADF24: 8D 41 06           lea eax, [ecx+0x6]
*/
void __declspec(naked) RecvCommand_59_Patch_ASM() {
	__asm {
		//ecx=packet
		pushad
		call assignPlayer
		popad
		movzx edx, word ptr [ecx+0x18]
		lea eax, [ecx+0x6]
		ret
	}
}
static void __fastcall recv8C(char *packet) {
	int id1=*(int *)(packet+1); //operator
	int id2=*(int *)(packet+5); //target
	int flag=*(short *)(packet+9); //1:Allow Loot 2:Can't hear 4:block message 8:hostile
	if (tShowTestInfo.isOn) gameMessage("Relation operator=%d target=%d flag=%X",id1,id2,flag);
}
//d2client_AD7BA: 8B 79 01           mov edi, [ecx+0x1]
//d2client_AD7BD: 8B D7              mov edx, edi
void __declspec(naked) RecvCommand_8C_Patch_ASM() {
	__asm {
		mov edi, dwCurMs
		add edi, 300
		mov dwCheckRelationMs, edi
		mov fPartyListValid,0
		//pushad
		//call recv8C
		//popad
		mov edi, [ecx+1]
		mov edx,edi
		ret
	}
}
