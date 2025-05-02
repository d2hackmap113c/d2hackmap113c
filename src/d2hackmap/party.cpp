#include "stdafx.h"
#include "header.h"

int CheckInvite() ;
void ResponseInvite();
int fCanInvite=0,mAutoPartyDelays=500,dwPartyResponseMs=0;
ToggleVar tAutoParty={TOGGLEVAR_ONOFF,	0,	-1, 1,	"Auto Party Toggle"};
ToggleVar tAutoInvite={TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Invite Toggle"};
ToggleVar tAutoPermit={TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Loot Permit Toggle"};
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY, "AutoPartyToggle",      &tAutoParty         },
  {CONFIG_VAR_TYPE_KEY, "AutoInviteToggle",     &tAutoInvite        },
  {CONFIG_VAR_TYPE_KEY, "AutoLootPermit",       &tAutoPermit        },
  {CONFIG_VAR_TYPE_INT, "AutoPartyDelays",      &mAutoPartyDelays, 4},
};
void PartyHelp_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
struct AcceptInvite {int uid;struct AcceptInvite *next;};
static AcceptInvite *acceptInvites=NULL,*recycleInvite=NULL;
void PartyHelpNewGame() {
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
	while (acceptInvites) {
		AcceptInvite *p=acceptInvites;acceptInvites=p->next;p->next=recycleInvite;recycleInvite=p;
		BYTE packet[6]={0x5E,0x08};*(int *)&packet[2]=p->uid;
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
		// Player Is Asking To Party With You
		// Player ID = aPacket[3~6]
		// Send packet back: 5E 08 xx xx xx xx
		AcceptInvite *p;
		if (recycleInvite) {p=recycleInvite;recycleInvite=p->next;}
		else p=(AcceptInvite *)HeapAlloc(dllHeap,0,sizeof(struct AcceptInvite));//gameHeap may not be valid yet
		p->uid=*(int *)&aPacket[3];p->next=acceptInvites;acceptInvites=p;
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
//Player In Game 5b [WORD Packet Length] [DWORD Player Id] [BYTE Char Type] [NULLSTRING[16] Char Name] [WORD Char Lvl] [WORD Party Id] 00 00 00 00 00 00 00 00 
void __fastcall AutoInvite(BYTE *aPacket) {
	if (tAutoInvite.isOn && fCanInvite ) {
		//自动邀请
		int recPlayerId= *(int *)&aPacket[3];
		if (recPlayerId==dwPlayerId) return;
		BYTE InvitePlayer[6] = {0x5E,0x06};
		*(int *)&InvitePlayer[2] = recPlayerId;
		SendPacket(InvitePlayer,sizeof(InvitePlayer));		
	}
	if (tAutoPermit.isOn && fInGame ){
		int recPlayerId= *(int *)&aPacket[3];
		if (recPlayerId==dwPlayerId) return;
		BYTE PermitPlayer[7] = {0x5D,0x01,0x01};
		*(int *)&PermitPlayer[3] = recPlayerId;
		SendPacket(PermitPlayer,sizeof(PermitPlayer));
	}
}
void __declspec(naked) RecvCommand_5B_Patch_ASM() {
	__asm {
		//ecx = cmdbuf
		pop eax
		push esi
		push eax //ret code
		push ecx
		call AutoInvite
		pop ecx
		lea esi, dword ptr [ecx+22]
		mov eax, esi
		ret
	}
}

