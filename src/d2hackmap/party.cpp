#include "stdafx.h"
#include "header.h"

int CheckInvite() ;
void ResponseInvite();
int fCanInvite=0,mAutoPartyDelays=500,dwPartyResponseMs=0;
ToggleVar tAutoParty={TOGGLEVAR_ONOFF,	0,	-1, 1,	"Auto Party Toggle"};
ToggleVar tAutoInvite={TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Invite Toggle"};
ToggleVar tAutoPermit={TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Loot Permit Toggle"};
ToggleVar tAutoPartyBugMF={TOGGLEVAR_ONOFF,	0,	-1, 1,	"Auto Party BugMF Toggle"};
char autoPartyLevelRange[3][2]={{1,99},{1,99},{1,99}};
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY,"AutoPartyBugMFToggle",&tAutoPartyBugMF},
  {CONFIG_VAR_TYPE_KEY,"AutoPartyToggle",&tAutoParty},
  {CONFIG_VAR_TYPE_KEY,"AutoInviteToggle",&tAutoInvite},
  {CONFIG_VAR_TYPE_KEY,"AutoLootPermit",&tAutoPermit},
  {CONFIG_VAR_TYPE_INT,"AutoPartyDelays",&mAutoPartyDelays, 4},
  {CONFIG_VAR_TYPE_CHAR_ARRAY0,"AutoPartyLevelRange",&autoPartyLevelRange,2,{3}},
};
void PartyHelp_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
struct AutoPartyUid {int uid;char name[20];struct AutoPartyUid *next;};
static AutoPartyUid *autoInvites=NULL,*acceptInvites=NULL,*recycleAutoPartyUid=NULL;
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
void PartyHelpNewGame() {
	if (!autoPartyBugMF()) return;
	if (CheckInvite()) {
		if (tAutoInvite.isOn) {
			for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext) {
				if (pUnit->dwUnitId==dwPlayerId || pUnit->dwPartyFlags) continue;
				BYTE packet[6] = {0x5E,0x06};*(int *)&packet[2] = pUnit->dwUnitId;
				SendPacket(packet,sizeof(packet));
			}
		}
		fCanInvite=1;
	}
	if (tAutoPermit.isOn) {
		for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext) {
			if (pUnit->dwUnitId==dwPlayerId ) continue;
			BYTE packet[7] = {0x5D,0x01,0x01};*(int *)&packet[3] = pUnit->dwUnitId;
			SendPacket(packet,sizeof(packet));
		}
	}
}
void PartyHelpEndGame() {fCanInvite=0;}
void ResponseInvite() {
	if (!tAutoParty.isOn||!acceptInvites||dwCurMs<dwPartyResponseMs) return;
	if (!autoPartyBugMF()) return;
	refresh_clients();
	while (acceptInvites) {
		AutoPartyUid *p=acceptInvites;acceptInvites=p->next;p->next=recycleAutoPartyUid;recycleAutoPartyUid=p;
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
		BYTE packet[6]={0x5E,0x08};*(int *)&packet[2]=uid;
		SendPacket(packet,sizeof(packet));
	}
	dwPartyResponseMs=0;
}
int CheckInvite() {
	if (dwPlayerId==1) return 1;
	for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext) {
		if (pUnit->dwUnitId<dwPlayerId) return 0; //自己不是最小编号，放弃邀请权利
	}
	return 1;
}
extern int fPartyListValid;
void __stdcall AutoParty(BYTE *aPacket) {
	fPartyListValid=0;
	if ( tAutoParty.isOn && aPacket[1] == 0x07 && aPacket[2] == 0x02 && aPacket[7] == 0x05 ) {
		if (!autoPartyBugMF()) return;
		// Player Is Asking To Party With You
		// Player ID = aPacket[3~6]
		// Send packet back: 5E 08 xx xx xx xx
		int uid=*(int *)&aPacket[3];
		AutoPartyUid *p;
		if (recycleAutoPartyUid) {p=recycleAutoPartyUid;recycleAutoPartyUid=p->next;}
		else p=(AutoPartyUid *)HeapAlloc(dllHeap,0,sizeof(struct AutoPartyUid));//gameHeap may not be valid yet
		p->uid=uid;p->name[0]=0;p->next=acceptInvites;acceptInvites=p;
		dwPartyResponseMs=dwCurMs+mAutoPartyDelays;
	} else if(aPacket[1] == 0x03 || aPacket[1] == 0x00) { //somebody left
		if(CheckInvite()) fCanInvite=1; 
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
		call AutoParty;
		pop ecx
		ret
	}
}
static void doInvite(int uid) {
	if (!autoPartyBugMF()) return;
	if (tAutoInvite.isOn && fCanInvite ) {
		LOG("send invite to %d\n",uid);
		//自动邀请
		BYTE InvitePlayer[6] = {0x5E,0x06};
		*(int *)&InvitePlayer[2] = uid;
		SendPacket(InvitePlayer,sizeof(InvitePlayer));		
	}
	if (tAutoPermit.isOn && fInGame ){
		BYTE PermitPlayer[7] = {0x5D,0x01,0x01};
		*(int *)&PermitPlayer[3] = uid;
		SendPacket(PermitPlayer,sizeof(PermitPlayer));
	}
}
int dwPartyInvideMs;
void AutoInviteLocalPlayer() {
	if (!tAutoParty.isOn||!autoInvites||dwCurMs<dwPartyInvideMs) return;
	refresh_clients();
	while (autoInvites) {
		AutoPartyUid *p=autoInvites;autoInvites=p->next;p->next=recycleAutoPartyUid;recycleAutoPartyUid=p;
		int uid=p->uid;
		int local=isLocalPlayerName(p->name);
		LOG("AutoInviteLocalPlayer uid=%d name=%s local=%d\n",uid,p->name,local);
		if (local) {
			doInvite(uid);
		}
	}
	dwPartyInvideMs=0;
}
//Player In Game 5b [WORD Packet Length] [DWORD Player Id] [BYTE Char Type] [NULLSTRING[16] Char Name] [WORD Char Lvl] [WORD Party Id] 00 00 00 00 00 00 00 00 
static void __fastcall recvCommand5B(BYTE *aPacket) {
	int uid=*(int *)&aPacket[3];
	if (uid==dwPlayerId) return;
	char *name=(char *)aPacket+8;
	short lv=*(short *)(aPacket+0x18);
	LOG("playerJoin uid=%d lv=%d name=%s difficulty=%d (%d-%d)\n",uid,lv,name,DIFFICULTY,
		autoPartyLevelRange[DIFFICULTY][0],autoPartyLevelRange[DIFFICULTY][1]);
	if (lv<autoPartyLevelRange[DIFFICULTY][0]||autoPartyLevelRange[DIFFICULTY][1]<lv) {
		int local=isLocalPlayerName(name);
		if (local) {
			doInvite(uid);
		} else {
			//check again later if local player
			AutoPartyUid *p;
			if (recycleAutoPartyUid) {p=recycleAutoPartyUid;recycleAutoPartyUid=p->next;}
			else p=(AutoPartyUid *)HeapAlloc(dllHeap,0,sizeof(struct AutoPartyUid));//gameHeap may not be valid yet
			p->uid=uid;memcpy(p->name,name,16);p->name[16]=0;p->next=autoInvites;autoInvites=p;
			dwPartyInvideMs=dwCurMs+mAutoPartyDelays;
		}
	} else {
		doInvite(uid);
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
	if (strcmp(pinfo->szCharName,name)==0)
		saveRuntimeInfo1(d2gfx_GetHwnd(),uid);
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

