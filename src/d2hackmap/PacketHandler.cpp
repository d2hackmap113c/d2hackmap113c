#include "stdafx.h"
#include "bitstream.h"
//6FB8BC30 + 12*n 
#ifdef MODULE_PACKETHANDLER
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
void formatKey(char *buf,int vk);
extern int dwChangeLeftSkill;
int DropProtectionPacketBlock(BYTE *buf,int len);
static char sendPacketCheckTable[256]={0};
int __cdecl blockSendPacket(int regs);
void __cdecl GamePacketSendIntercept(int regs);
void __fastcall GamePacketReceivedIntercept(BYTE* buf, DWORD len);
static int dwPHTime;
void ResetPacketHandlerTimer() {dwPHTime=dwCurMs;}
static void hex(FILE *fp,int addr,void *vbuf,int n) {
	u8 *buf=(u8 *)vbuf;
	if (!fp) return;
	for (int i=0;i<n;i+=16) {
		fprintf(fp,"\t%02x: ",addr+i);
		for (int j=0;j<16;j++) {
			if (i+j>=n) fprintf(fp,"   ");
			else fprintf(fp,"%02x ",buf[i+j]);
			if (j==3||j==7||j==11) fprintf(fp,"- ");
		}
		fprintf(fp,"|");
		for (int j=0;j<16;j++) {
			if (i+j>=n) {fprintf(fp," ");continue;}
			u8 c=buf[i+j];
			if (c<' '||c>127) fprintf(fp," ");
			else fprintf(fp,"%c",c);
		}
		fprintf(fp,"\n");
	}
}
void dump9c(u8 *dat,int len) {
	if (!logfp) return;
	struct bitstream bs;
	int id=*(int *)(dat+4);
	if (dat[0]==0x9c) {//9c add belt
/*
byte1: 0f:hand 0e:belt
byte2: 14
byte3: 10
*/
		bitstream_init(&bs,dat+8,dat+len);
		fprintf(logfp,"	id=%d",id);
	} else { //9d use belt
		int unitType=dat[8];
		int unitId=*(int *)(dat+9);
		fprintf(logfp,"	id=%d unit=%d:%d",id,unitType,unitId);
		bitstream_init(&bs,dat+13,dat+len);
	}
	bitstream_seek(&bs,65-16);
	int x=bitstream_rbit(&bs, 4); //bit65-68
	int y=bitstream_rbit(&bs, 3); //bit69-71
	int bit72=bitstream_rbit(&bs, 1);//bit72
	int location=bitstream_rbit(&bs, 3);//bit73-75 0:belt bag:1 cube:4 stash:5
	int intcode=bitstream_rbit(&bs, 30);//bit76-105
	char *code=(char *)&intcode;code[3]=0;
	fprintf(logfp," code:%s %08x %d:(%d,%d)\n",code,intcode,location,x,y);
	fflush(logfp);
}
/*
6FAF39A3 - 56                    - push esi <-------------packet len
6FAF39A4 - 8D 44 24 0C           - lea eax,[esp+0C] <--------packet buf
6FAF39A8 - E8 73940600           - call 6FB5CE20 <---------------------
need to save ebp,ebx,edi,esi
packet handler [6FB8DE60+cmd*12]
buf:	ecx and edi
buf[0]: edx and esi
 00:6FB5C520 01:6FB5FA90 02:6FB5C4F0 03:6FB5DB50 04:6FB5DFE0 05:6FB5CB70 06:6FB5CAA0 07:6FB5C440
 08:6FB5C3D0 09:6FB5DF70 0a:6FB5CA40 0b:6FB5D6E0 0c:6FB5C520 0d:6FB5C520 0e:6FB5C520 0f:6FB5C520
 10:6FB5C520 11:6FB5DAA0 12:6FB5C370 13:6FB5C360 14:6FB5C350 15:6FB5F640 16:6FB5D9D0 17:6FB5C520
 18:6FB5ECA0 19:6FB5EDF0 1a:6FB5EDF0 1b:6FB5EDF0 1c:6FB5EDF0 1d:6FB5EDF0 1e:6FB5EDF0 1f:6FB5EDF0
 20:6FB5ED80 21:6FB5C910 22:6FB5C800 23:6FB5C780 24:6FB5C520 25:6FB5C520 26:6FB5F9F0 27:6FB5D680
 28:6FB5FA70 29:6FB5C330 2a:6FB5F9B0 2b:6FB5C520 2c:6FB5D740 2d:6FB5C520 2e:6FB5C520 2f:6FB5C520
 30:6FB5C520 31:6FB5C520 32:6FB5C520 33:6FB5C520 34:6FB5C520 35:6FB5C520 36:6FB5C520 37:6FB5C520
 38:6FB5C520 39:6FB5C520 3a:6FB5C520 3b:6FB5C520 3c:6FB5C520 3d:6FB5C520 3e:6FB5E300 3f:6FB5F910
 40:6FB5D340 41:6FB5C520 42:6FB5DB70 43:6FB5C520 44:6FB5C520 45:6FB5C130 46:6FB5C520 47:6FB5E2B0
 48:6FB5E260 49:6FB5C520 4a:6FB5C520 4b:6FB5C520 4c:6FB5C520 4d:6FB5C520 4e:6FB5C0B0 4f:6FB5C0E0
 50:6FB5D4B0 51:6FB5DF40 52:6FB5D520 53:6FB5E9C0 54:6FB5C120 55:6FB5C520 56:6FB5C520 57:6FB5D560
 58:6FB5F980 59:6FB5DF20 5a:6FB5D6B0 5b:6FB5D8F0 5c:6FB5D8E0 5d:6FB5F8F0 5e:6FB5C090 5f:6FB5C050
 60:6FB5D2F0 61:6FB5F940 62:6FB5DFD0 63:6FB5F8B0 64:6FB5C520 65:6FB5D890 66:6FB5C040 67:6FB5C520
 68:6FB5C520 69:6FB5C520 6a:6FB5C520 6b:6FB5C520 6c:6FB5C520 6d:6FB5C520 6e:6FB5C520 6f:6FB5C520
 70:6FB5C520 71:6FB5C520 72:6FB5C520 73:6FB5EEE0 74:6FB5DEF0 75:6FB5D870 76:6FB5D370 77:6FB5F930
 78:6FB5BFD0 79:6FB5BFB0 7a:6FB5C720 7b:6FB5C6D0 7c:6FB5D2A0 7d:6FB5E1F0 7e:6FB5D240 7f:6FB5D820
 80:0 81:6FB5EBA0 82:6FB5D160 83:97D1310 84:6FB5C520 85:6FB5C520 86:6FB5C520 87:6FB5C520
 88:6FB5C520 89:6FB5BFA0 8a:6FB5DB30 8b:6FB5D800 8c:6FB5D7B0 8d:6FB5D780 8e:6FB5D130 8f:6FB5BF40
 90:6FB5D220 91:6FB5C750 92:6FB5E230 93:6FB5D3D0 94:6FB5C880 95:6FB5EBE0 96:6FB5D950 97:6FB5BF50
 98:6FB5D4F0 99:6FB5F0F0 9a:6FB5F090 9b:6FB5E1E0 9c:6FB5E080 9d:6FB5E870 9e:6FB5EA40 9f:6FB5EA40
 a0:6FB5EA40 a1:6FB5EA40 a2:6FB5EA40 a3:6FB5EFB0 a4:6FB5D760 a5:6FB5F7F0 a6:6FB5BF20 a7:6FB5DEB0
 a8:6FB5DD70 a9:6FB5DD30 aa:6FB5DB90 ab:6FB5D0B0 ac:6FB5E420 ad:6FB5C520 ae:6FB5D630 af:6FBAAE28
[6FB8DE60+cmd*12+8]
00:0 01:0 02:0 03:0 04:0 05:0 06:0 07:0
08:0 09:0 0a:0 0b:0 0c:6fb5f5f0 0d:6fb5f590 0e:6fb5f560 0f:6fb5f510
10:6fb5f4c0 11:0 12:0 13:0 14:0 15:0 16:0 17:6fb5f240
18:0 19:0 1a:0 1b:0 1c:0 1d:0 1e:0 1f:0
20:0 21:0 22:0 23:0 24:0 25:0 26:0 27:0
28:0 29:0 2a:0 2b:0 2c:0 2d:0 2e:0 2f:0
30:0 31:0 32:0 33:0 34:0 35:0 36:0 37:0
38:0 39:0 3a:0 3b:0 3c:0 3d:0 3e:0 3f:0
40:0 41:0 42:0 43:0 44:0 45:0 46:0 47:0
48:0 49:0 4a:0 4b:0 4c:6fb5f1d0 4d:6fb5f160 4e:0 4f:0
50:0 51:0 52:0 53:0 54:0 55:0 56:0 57:0
58:0 59:0 5a:0 5b:0 5c:0 5d:0 5e:0 5f:0
60:0 61:0 62:0 63:0 64:0 65:0 66:0 67:6fb5f460
68:6fb5f3f0 69:6fb5f390 6a:6fb5f340 6b:6fb5f7c0 6c:6fb5f750 6d:6fb5f2b0 6e:6fb5c
3c0 6f:6fb5c3b0
70:6fb5c3a0 71:6fb5c390 72:6fb5c380 73:0 74:0 75:0 76:0 77:0
78:0 79:0 7a:0 7b:0 7c:0 7d:0 7e:0 7f:0
80:0 81:0 82:0 83:0 84:0 85:0 86:0 87:0
88:0 89:0 8a:0 8b:0 8c:0 8d:0 8e:0 8f:0
90:0 91:0 92:0 93:0 94:0 95:0 96:0 97:0
98:0 99:0 9a:0 9b:0 9c:0 9d:0 9e:0 9f:0
a0:0 a1:0 a2:0 a3:0 a4:0 a5:0 a6:0 a7:0
a8:0 a9:0 aa:0 ab:0 ac:0 ad:0 ae:0 af:2600
*/
void __declspec(naked) GamePacketReceivedInterceptPatch_ASM()
{
	__asm {
		mov ecx,tPacketHandler.isOn
		cmp ecx,0
		jne dump
		push esi
		call dword ptr [D2PacketHandler]
		ret 4
dump:
		push eax
		push ebx
		push esi
		push edi
		push ebp
		mov edx, esi;
		mov ecx, eax;
/*
		mov ebx, dword ptr [eax]
		cmp bl,0x2C
		jne next
		nop
next:
*/
		call GamePacketReceivedIntercept;
		pop ebp
		pop edi
		pop esi
		pop ebx
		pop eax
		push esi
		call dword ptr [D2PacketHandler]
		ret 4
	}
}
void __fastcall GamePacketReceivedIntercept(BYTE* buf, DWORD len) {
	if(!tPacketHandler.isOn) return;
	if (!buf||len<1||len>4096) return;
	int cmd=buf[0]&0xFF;
	if (!fMonitorRecvCmd[cmd]) return;
	//if (cmd<0xd) return;
	char *name="";
	if (!logfp) return;
/*
0d   13   Player Stop      0d [1:BYTE Unit Type] [2:DWORD Unit Id] [6:BYTE  Unknown] [7:WORD Unit X] [9:WORD Unit Y] [BYTE Unknown] [12:BYTE Life] 
	000000: 0d 00 02 00 - 00 00 07 cb - 18 24 15 00 - 64          |         $  d   
0f   16   Player Move      0f [1:BYTE Unit Type] [2:DWORD Unit Id] [6:BYTE  0x01 = Walk || 0x23 = Run || 0x20 = Knockback]
	[7:WORD Target X] [9:WORD Target Y] 00  [12:WORD Current X] [14:WORD Current Y] 
	000000: 0f 00 02 00 - 00 00 17 ce - 18 28 15 00 - cb 18 24 15 |         (    $ 
10   16   Player To Target   10 [1:BYTE Unit Type] [2:DWORD Unit Id] [6:BYTE  0x02 = Walk || 0x18 = Run] [7:BYTE Target Type] [8:DWORD Target Id] [12:WORD Current X]  [14:WORD Current Y] 
	000000: 10 00 02 00 - 00 00 18 01 - 09 00 00 00 - d2 18 2a 15 |              * 
*/
	switch (cmd) {
		case 0x00://   1   Game Loading      00 
			name="GameLoading";break;
		case 0x01://   8   Game Flags      01 [BYTE Difficulty] [WORD Unknown] [WORD  Hardcore?] [BYTE Expansion] [BYTE Ladder] 
			name="GameFlag";break;
		case 0x02://   1   Load Successful      02 
			name="LoadSuccess";break;
		case 0x03://   12  Load Act      03 [BYTE Act] [DWORD Map ID] [WORD Area  Id] [DWORD Unknown] 
			name="LoadAct";break;
		case 0x04://   1   Load Complete      04 
			name="LoadComplete";break;
		case 0x05://   1   Unload Complete   05 
			name="UnloadComplete";break;
		case 0x06://   1   Game Exit Sucessful   06 
			name="GameExitSuccess";break;
		case 0x07://   6   Map Reveal      07 [WORD Tile X] [WORD Tile Y] [BYTE Area Id]    
			name="MapReveal";break;
		case 0x08://   6   Map Hide      08 [WORD Tile X] [WORD Tile Y] [BYTE Area Id] 
			name="MapHide";break;
		case 0x09://   12   Assign Lvl Warp (Door)   09 [BYTE Unit Type] [DWORD Unit Id] [WORD X] [WORD Y] [WORD Warp Id] 
			name="AssignLvlWrap";break;
		case 0x0a://   6   Remove Object      0a [BYTE Unit Type] [DWORD Unit Id] 
			name="RemoveObject";break;
		case 0x0b://   6   Game Handshake   0b [BYTE Unit Type] [DWORD Unit Id] 
			name="GameHandshake";break;
		case 0x0c://   9   NPC Hit         0c [BYTE Unit Type] [DWORD Unit Id] [WORD  Animation Id] [BYTE Life] 
			name="NPCHit";break;
		case 0xd: {//player stop
			name="PlayerStop";
			break;
		}
		case 0x0e://12   Object State      0e [BYTE Unit Type] [DWORD Unit Id] [BYTE  Unknown (0x03)] [BYTE 0x00 = Can't Change Back || 0x01 = Can Change Back] [DWORD  Unit State] 
			name="ObjectState";break;
		case 0x0f: {//player move
			name="PlayerMove";
			break;
		}
		case 0x10: //player to target
//10   16   Player To Target   10 [1:BYTE Unit Type] [2:DWORD Unit Id] [6:BYTE  0x02 = Walk || 0x18 = Run] [7:BYTE Target Type] [8:DWORD Target Id] [12:WORD Current X]  [14:WORD Current Y] 
//	000000: 10 00 02 00 - 00 00 18 01 - 09 00 00 00 - d2 18 2a 15 |              * 
			name="PlayerToTarget";break;
		case 0x11://   8   Report Kill      11 [BYTE Unit Type] [DWORD Unit Id] [WORD  Unknown] 
			name="ReportKill";break;
		case 0x15://11   Reassign Player   15 [BYTE Unit Type] [DWORD Unit Id] [WORD  X] [WORD Y] [BYTE 0x01 = True || 0x00 = False] 
			name="ReassignPlayer";break;
		case 0x19://   2   (BYTE)Gold to Inv.   19 [BYTE Amount] 
			name="GoldToInv";break;
		case 0x1a://   2   (BYTE)Add Exp.      1a [BYTE Amount] 
			name="AddExp8";break;
		case 0x1b://   3   (WORD)Add Exp.   1b [WORD Amount]    
			name="AddExp16";break;
		case 0x1c://5   (DWORD)Add Exp.   1c [DWORD Amount] 
			name="AddExp32";break;
		case 0x1d://3   (BYTE)Base Attribute   1d [BYTE Attribute] [BYTE Amount] 
			name="BaseAttr8";break;
		case 0x1e://4   (WORD)Base Attribute   1e [BYTE Attribute] [WORD Amount] 
			name="BaseAttr16";break;
		case 0x1f://6   (DWORD)Base Attribute   1f [BYTE Attribute] [DWORD Amount] 
			name="BaseAttr32";break;
		case 0x20://   10   Attribute Update   20 [DWORD Unit Id] [BYTE Attribute] [DWORD  Amount] 
			name="AttrUpdate";break;
		case 0x21://   12   Update Item Oskill  21 [WORD Unknown] [DWORD Unit Id] [WORD Skill] [BYTE Base Level] [BYTE Bonus Amount] [BYTE Unknown] 
			name="UpdateItemOskill";break;
		case 0x22://12   Update Item Skill   22 [WORD Unknown (Unit Type?)] [DWORD Unit Id] [WORD Skill] [BYTE Amount] [WORD Unknown] 
	//00: 22 00 db 01 - 00 00 00 dc - 00 02 a9 00 -             |"               
			name="UpdateItemSkill";break;
		case 0x23://13   Set Skill      23 [BYTE Unit Type] [DWORD Unit Id] [Byte 0x00  = Right || Byte 0x01 = Left] [WORD Skill] FF FF FF FF 
			name="SetSkill";break;
		case 0x26://   *   Game Chat      26 [WORD Chat Kind] [WORD Unknown (0x02)]  00 00 00 00 [BYTE 0x05 = Normal Chat || 0x01 = Whisper] [Char Name] 00 [Message] 00 
			name="GameChat";break;
		case 0x27://40   NPC Info      27 [BYTE Unit Type] [DWORD Unit Id]  [BYTES[34] Unknown] 
			name="NPCInfo";break;
		case 0x28://103   Quest Info     28 06 00 00 00 00 00 [quest data 84 bytes ] [00 *12]
			name="QuestInfo";break;
		case 0x29://97   Game Quest Info   <Research Incomplete> 
			name="GameQuestInfo";break;
		case 0x2c://2c   8   Play Sound      2c [BYTE Unit Type] [DWORD Unit Id] [WORD  Sound] 
		//19:Help 1a:followme 1b:thisisforyou 1c:thanks 1d:sorry 1e:bye 1f:die 20:retreat
			name="PlaySound";
			fprintf(logfp,"RECV cmd_%02x len=%d %s type=%d unit=%d sound=%d\n",cmd,len,name,
				buf[1],*(int *)(buf+2),*(short *)(buf+6));
			return;
		case 0x3e://   *   Update Item Stats   <Research Incomplete> 
			name="UpdateItemStat";break;
		case 0x3f://   8   Use Stackable Item   ---------- 
			name="UseStackableItem";break;
		case 0x42://   6   Clear Cursor      42 [BYTE Unit Type] [DWORD Player Id] 
			name="ClearCursor";break;
		case 0x47://11   Relator 1      47 [WORD Param 1] [DWORD Unit Id] [DWORD  Param 2] 
			name="Relator1";break;
		case 0x48://11   Relator 2      48 [WORD Param 1] [DWORD Unit Id] [DWORD  Param 2] 
			name="Relator2";break;
		case 0x4c://   16   Unit Skill on Target   4c [BYTE Unit Type] [DWORD Unit Id] [WORD  Skill] [BYTE Unknown] [BYTE Unknown] [DWORD Target Id] 00 00 
			name="UnitSkillOnTarget";break;
		case 0x4d://   17   Unit Cast Skill      4d [BYTE Unit Type] [DWORD Unit Id] [DWORD  Skill] [BYTE Unknown] [WORD X] [WORD Y] 00 00 
			name="UnitCastSkill";break;
		case 0x4e://   7   Merc For Hire      4e [WORD Merc Id?] [DWORD Unknown] 
			name="MercForHire";break;
		case 0x4f://   1   Start Merc List      4f 
			name="StartMercList";break;
		case 0x51://14   Assign Object      51 [BYTE Object Type] [DWORD Object Id]  [WORD Object Code] [WORD X] [WORD Y] [BYTE State] [BYTE Interaction Type] 
			name="AssignObject";break;
		case 0x52://42   Quest Log Info      <Research Incomplete> 
			name="QuestLogInfo";break;
		case 0x53://10   Player Slot Refresh   53 [DWORD Slot #] [BYTE Unknown 0x00 || 0x80]  [DWORD Tickcount?] 
			name="PlayerSlotRefresh";break;
		case 0x59://26   Assign Player      59 [1:DWORD Unit Id] [5:BYTE Char Type]  [6:NULLSTRING[16] Char Name] [WORD X] [WORD Y] 
			name="AssignPlayer";break;
		case 0x5a://40 EventMessages
			name="EventMessage";break;
		case 0x5b://36   Player In Game      5b [WORD Packet Length] [DWORD Player Id] [BYTE Char Type] [NULLSTRING[16] Char Name] [WORD Char Lvl] [WORD Party Id] 00 00 00 00 00 00 00 00 
			name="PlayerInGame";break;
		case 0x5e://38   <Unknown>      ---------- 
			name="";break;
		case 0x5f://5   <Unknown>         ---------- 
			name="";break;
		case 0x60://   7   Townportal State      60 [BYTE State] [BYTE Area Id] [DWORD Unit Id] 
			name="TownportalState";break;
		case 0x63://*   Waypoint Menu      63 [DWORD Unit Id] [BYTES* Available Waypoints] 
			name="WaypointMenu";break;
		case 0x65://7  Player Kill Count      65 [DWORD Player Id] [DWORD Count] 
			name="PlayerKillCount";break;
		case 0x67://16 NPC move
			name="NPCMove";break;
		case 0x68://   21   NPC Move to Target   68 [DWORD Unit Id] [BYTE 0x00 = Walk || 0x18 = Run] [WORD X] [WORD Y] [BYTE Target Unit Type] [DWORD Target Id] [WORD Unknown] [BYTE Unknown] [WORD Unknown] [BYTE Unknown] 
			name="NPCMoveToTarget";break;
		case 0x69://   12   NPC State      69 [DWORD Unit Id] [BYTE State] [WORD X] [WORD Y] [BYTE Unit Life] [BYTE Unknown] 
			name="NPCState";break;
		case 0x6a://   12   <Unknown>         ---------- 
			name="";break;
		case 0x6b://16   NPC Action      6b [DWORD Unit Id] [BYTE Action] 00 00 00 00 00 00 [WORD X] [WORD Y] 
			name="NPCAction";break;
		case 0x6d://10   NPC Stop      6d [DWORD Unit Id] [WORD X] [WORD Y] [BYTE Unit Life] 
			name="NPCStop";break;
		case 0x74://   10   Player Corpse Assign   74 [BYTE Assign 0x00 = False || 0x01 True] [DWORD Owner Id] [DWORD Corpse Id] 
			name="PlayerCorpseAssign";break;
		case 0x75://   13   Player Party Info   75 [DWORD Unit Id] [WORD Party Id] [WORD Char Level] [WORD Relationship] [WORD In Your Party? 0x00 = False || 0x01 = True] 
			name="PlayerPartyInfo";break;
		case 0x76://6   Player In Proximity   76 [BYTE Unit Type] [BYTE Unit Id] 
			name="PlayerInProximity";break;
		case 0x77://   2   Button Actions      77 [BYTE Action] 
			name="ButtonActions";break;
		case 0x78://   21   Trade Accepted      78 [NULLSTRING[16] Char Name] [DWORD Unit Id] 
			name="TradeAccepted";break;
		case 0x79://   6    Gold in Trade      79 [BYTE Gold Owner] [DWORD Amount] 
			name="GoldInTrade";break;
		case 0x7a://13   Pet Action      7a [BYTE 0x00 = Unsummoned/Lost Sight || 0x01 = Summoned/Assign] [BYTE Skill] [WORD Pet Type] [DWORD Owner Id] [DWORD Pet Id] 
			name="PetAction";break;
		case 0x7b://8   Assign Skill Hotkey   7b [BYTE Slot] [BYTE Skill] [BYTE 0x00 = Right || 0x80 = Left] FF FF FF FF 
			name="AssignSkillHotKey";break;
		case 0x7c://   6   Use Scroll      7c [BYTE Type] [DWORD Item Id] 
			name="UseScroll";break;
		case 0x7d://   18   Set Item State      --------- 
			name="SetItemState";break;
		case 0x7e://5   <Unknown>                 --------- 
			name="";break;
		case 0x7f://10   Ally Party Info      7f  [BYTE Unit Type] [WORD Unit Life] [DWORD Unit Id] [DWORD Unit Area Id]    
			name="AllyPartyInfo";break;
		case 0x81://20   Assign Merc      81 [BYTE Unknown] [WORD Merc Kind?] [DWORD Owner Id] [DWORD Merc Id] [DWORD Unknown] [DWORD Unknown] 
			name="AssignMerc";break;
		case 0x82://   29   Portal Ownership   82 [DWORD Owner Id] [NULLSTRING[16] Owner Name] [DWORD Local Id] [DWORD Remote Id] 
			name="PortalOwnership";break;
		case 0x89://  2   Special Quest Event           89 [BYTE Event Id] 
			name="SpecialQuestEvent";break;
		case 0x8a://   6   NPC Wants to Interact   8a [BYTE Unit Type] [DWORD Unit Id] 
			name="NPCWantInteract";break;
		case 0x8b://   6   Player Relationship   8b [DWORD Unit Id] [BYTE 0x00 = No Party || 0x01 = In Party || 0x02 = Wants to Party] 
			name="PlayerRelationship";break;
		case 0x8c://   11   Relationship Update   8c [DWORD Player 1 Id] [DWORD Player 2 Id] [WORD Relation State] 
			name="RelationshipUpdate";break;
		case 0x8d://7  Assign Player To Party   8d [DWORD Player Id] [WORD Party Id] 
			name="AssignPlayerToParty";break;
		case 0x8e://   10   Corpse Assign      8e [BYTE 0x00 = Unassign || 0x01 = Assign] [DWORD Owner Id] [DWORD Corpse Id] 
			name="CorpseAssign";break;
		case 0x8f://33   Pong         8f [BYTES[32] 0x00] 
			name="Pong";break;
		case 0x90://   13   Party Automap Info   90 [DWORD Player Id] [DWORD Player X] [DWORD Player Y] 
			name="PartyAutomap";break;
		case 0x94://*   Base Skill Levels      94 [BYTE Amount of Skills] [DWORD Player Id] (for each skill->) [WORD Skill] [BYTE Level] 
			name="BaseSkillLevels";break;
		case 0x95://13   Life and Mana Update   95 [WORD Unit Life] [WORD Unit Mana] [WORD Unit Stamina] [WORD X] [WORD Y] [WORD Unknown] 
			name="LifeMana";break;
		case 0x96://9   Walk Verify      96 [WORD Stamina] [WORD X] [WORD Y] [WORD State] 
			name="WalkVerify";break;
		case 0x97://1   Weapon Switch      97 
			name="WeaponSwitch";break;
		case 0x99://   16   Skill Triggered      ---------- 
			name="SkillTrigger";break;
		case 0x9b://   7   (Merc Dead)         9b [WORD nameId] [DWORD 0xc350?] 
			name="Merc Dead";break;
		case 0x9c:
			name="Item Action (world)";break;
		case 0x9d:
			name="Item Action (owned)";break;
		case 0x9e://7   (BYTE)Merc Attribute   9e [BYTE Attribute] [DWORD Merc Id]  [BYTE Amount] 
			name="MercAttr8";break;
		case 0x9f://8   (WORD)Merc Attribute   9f [BYTE Attribute] [DWORD Merc Id] [WORD Amount] 
			name="MercAttr16";break;
		case 0xa0://10   (DWORD)Merc Attribute   a0 [BYTE Attribute] [DWORD Merc Id] [DWORD Amount] 
			name="MercAttr32";break;
		case 0xa1://   7   (BYTE)Merc Add Exp.   ---------- 
			name="MercExp";break;
		case 0xa2://   8  (WORD)Merc Add Exp.   ---------- 
			name="MercExp";break;
		case 0xa7://   7   Delayed State      a7 [BYTE Unit Type] [DWORD Unit Id] [BYTE State] 
	//00: a7 01 0f 00 - 00 00 17    -             -             |                
			name="DelayedState";
			if (logfp) {
				fprintf(logfp,"RECV cmd_%02x len=%d %s:",cmd,len,name);
				fprintf(logfp," type=%d id=%d state=%d 0x%x\n",buf[1],*(int *)(buf+2),buf[6],buf[6]);
				fflush(logfp);
			}
			return;
		case 0xa8://*   Set State      a8 [BYTE Unit Type] [DWORD Unit Id] [BYTE Packet Length] [BYTE State] [VOID State Effects] 
	//00: a8 01 0f 00 - 00 00 0c 09 - 24 38 ff 07 -             |        $8      
			name="SetState";
			if (logfp) {
				fprintf(logfp,"RECV cmd_%02x len=%d %s:",cmd,len,name);
				fprintf(logfp," type=%d id=%d len=%d state=%d 0x%x effects=[",buf[1],*(int *)(buf+2),buf[6],buf[7],buf[7]);
				for (int i=8;i<(int)len;i++) fprintf(logfp,"%02x ",buf[i]);
				fprintf(logfp,"]\n");
				fflush(logfp);
			}
			return;
		case 0xa9://   7   End State      a9 [BYTE Unit Type] [DWORD Unit Id] [BYTE State] 
	//00: a9 01 0f 00 - 00 00 09    -             -             |                
			name="EndState";
			if (logfp) {
				fprintf(logfp,"RECV cmd_%02x len=%d %s:",cmd,len,name);
				fprintf(logfp," type=%d id=%d state=%d 0x%x\n",buf[1],*(int *)(buf+2),buf[6],buf[6]);
				fflush(logfp);
			}
			return;
		case 0xaa://*   Add Unit         aa [BYTE Unit Type] [DWORD Unit Id] [BYTE Packet Length] [VOID State Info] 
			name="AddUnit";break;
		case 0xab://   7   NPC Heal      ab [BYTE Unit Type] [DWORD Unit Id] [BYTE Unit Life] 
			name="NPCHeal";break;
		case 0xac://*   Assign NPC      ac [DWORD Unit Id] [WORD Unit Code] [WORD X] [WORD Y] [BYTE Unit Life] [BYTE Packet Length] [VOID State Info] 
			name="AssignNPC";break;
		case 0xae://   *   Warden Request      ae [WORD Packet Length Excluding Header] [VOID Data] 
			name="WardenRequest";break;
		case 0xaf://   2   Connection Info      af [BYTE ProtocolVer] 
			name="ConnectInfo";break;
		case 0xb0://   1   Game Connection Terminated   b0 
			name="GameConnectionTerminated";break;
		case 0xb3://    5   IP Ban?                      b3 [DWORD Param?] 
			name="IPBan";break;
		default:
			break;
	}
	if (logfp) {
		int ms=dwCurMs-dwPHTime;
		fprintf(logfp,"[%d.%03d] RECV cmd_%02x len=%d %s\n",ms/1000,ms%1000,cmd,len,name);
		hex(logfp,0,buf,len);
		if (cmd==0x9c||cmd==0x9d) dump9c(buf,len);
		fflush(logfp);
	}
	return;
}
/*
DLLBASE_D2NET = 0x6FBF0000, 
D2VARPTR(D2NET,  0x6FBFB244, UnkNetFlag,	   DWORD)
D2FUNCPTR(D2NET,  -0x2728,   SendPacket,             void __stdcall, (size_t len, DWORD arg1, BYTE* buf))
D2Net.Ordinal10024
D2Net.dll+7650     - A1 44B2AF00           - mov eax,[D2Net.dll+B244] { (0B25D6AC) }
D2Net.dll+7655     - 85 C0                 - test eax,eax
D2Net.dll+7657     - 75 05                 - jne D2Net.dll+765E
D2Net.dll+7659     - 33 C0                 - xor eax,eax
D2Net.dll+765B     - C2 0C00               - ret 000C { 12 }
*/
void __declspec(naked) GamePacketSendInterceptPatch_ASM()
{
	__asm {
		mov eax, dword ptr [esp+0x10] //packet buf
		cmp eax,0
		je end
		movzx eax, byte ptr [eax]
		mov al,byte ptr [sendPacketCheckTable+eax]
		cmp al,0 
		jne blockCheck
		jmp checkNetworkMonitor
blockCheck:
		pushad
		call blockSendPacket
		cmp eax,0
		je notblock
		popad
		xor eax,eax
		ret
notblock:
		popad
checkNetworkMonitor:
		mov eax,tPacketHandler.isOn
		cmp eax,0
		jne dump
		mov eax, p_D2UnkNetFlag
		mov eax, [eax]
		ret
dump:
		pushad
		call GamePacketSendIntercept;
		popad
end:
		mov eax, p_D2UnkNetFlag
		mov eax, [eax]
		ret
	}
}
void __cdecl GamePacketSendIntercept(int regs) {
	int *esp=&regs;
	int len=esp[10];
	int arg=esp[11];
	BYTE *buf=(BYTE *)esp[12];
	if (!buf||len<1||len>4096) return;
	int cmd=buf[0]&0xFF;
	if (!fMonitorSendCmd[cmd]) return;
	//if (cmd==0x6d) return;//ping
	char *name="";
	switch (cmd) {
		case 0x01: //01   5   Walk            01 [WORD x] [WORD y] 
			name="Walk";break;
		case 0x02: //02   9   Walk to entity         02 [DWORD entity kind] [DWORD id] 
			name="Walk to entity";break;
		case 0x03: //03   5   Run            03 [WORD x] [WORD y] 
			name="Run";break;
		case 0x04: //04   9   Run to entity         04 [DWORD entity kind] [DWORD id] 
			name="Run to entity";break;
		case 0x05: //05   5   Shift Left Click Skill      05 [WORD x] [WORD y] 
			name="Shift Left Click Skill";break;
		case 0x06: //06   9   Left Skill on unit      06 [DWORD entity kind] [DWORD id] 
			name="Left Skill on unit";break;
		case 0x07: //07   9   Shift left skill on unit   07 [DWORD entity kind] [DWORD id] 
			name="Shift left skill on unit";break;
		case 0x08: //08   5   Shift Left skill (hold)      08 [WORD x] [WORD y] 
			name="Shift Left skill (hold)";break;
		case 0x09: //09   9   Left Skill on unit (hold)   09 [DWORD entity kind] [DWORD id] 
			name="Left Skill on unit (hold)";break;
		case 0x0a: //0a   9   Shift left skill unit (hold)   0a [DWORD entity kind] [DWORD id] 
			name="Shift left skill unit (hold)";break;
		case 0x0c: //0c   5   Right skill         0c [WORD x] [WORD y] 
			name="Right skill";break;
		case 0x0d: //0d   9   Right skill unit      0d [DWORD entity kind] [DWORD id] 
			name="Right skill unit";break;
		case 0x0e: //0e   9   Shift right skill unit      0e [DWORD entity kind] [DWORD id] 
			name="Shift right skill unit";break;
		case 0x0f: //0f   5   Right skill (hold)      0f [WORD x] [WORD y] 
			name="Right skill (hold)";break;
		case 0x10: //10   9   Right skill unit (hold)      10 [DWORD entity kind] [DWORD id] 
			name="Right skill unit (hold)";break;
		case 0x11: //11   9   Shift Right skill unit (hold)   11 [DWORD entity kind] [DWORD id] 
			name="Shift Right skill unit (hold)";break;
		case 0x13: //13   9   Interact (click) entity      13 [DWORD entity kind] [DWORD id] 
			name="Interact (click) entity";break;
		case 0x14: //14   [Varies]Overhead Chat         14 00 00 [*char message] 00 00 00 
			name="Overhead Chat";break;
		case 0x15: //15   [Varies]Chat            15 01 00 [*char message] 00 00 00 
			name="Chat";break;
		case 0x16: //16   13   Pick item (ground)      16 04 00 00 00 [DWORD id] [DWORD inventory(0) or cursor (1)] 
			name="Pick item (ground)";break;
		case 0x17: //17   5   Drop item (ground)      17 [DWORD id] 
			name="Drop item (ground)";break;
		case 0x18: //18   17   Insert item in buffer      18 [DWORD id] [DWORD xpos] [DWORD ypos] [DWORD buffer] 
			name="Insert item in buffer";break;
		case 0x19: //19   5   Remove item from buffer      19 [DWORD id] 
			name="Remove item from buffer";break;
		case 0x1a: //1a   9   Equip item         1a [DWORD id] [WORD position] 00 00 
			name="Equip item";break;
		case 0x1b: //1b   9   Swap 2 handed item      1b [DWORD id] [WORD position] 00 00 
			name="Swap 2 handed item";break;
		case 0x1c: //1c   3   Remove body item      1c [WORD position] 
			name="Remove body item";break;
		case 0x1d: //1d   9   Swap cursor item with body   1d [DWORD id] [WORD position] 00 00 
			name="Swap cursor item with body";break;
		case 0x1e: //1e   9   Swap 2 1-handed items with a 2-handed item   1e [DWORD unknown] [DWORD unknown] 
			name="Swap 2 1-handed items with a 2-handed item";break;
		case 0x1f: //1f   17   Swap cursor/buffer items   1f [DWORD cursor item id] [DWORD buffer item id] [DWORD xpos] [DWORD ypos] 
			name="Swap cursor/buffer items";break;
		case 0x20: //20   13   Activate buffer item      20 [DWORD id] [WORD x] 00 00 [WORD y] 00 00 
			name="Activate buffer item";break;
		case 0x21: //21   9   Stack items         21 [DWORD item to stack id] [DWORD item where it stacks id] 
			name="Stack items";break;
		case 0x22: //22   5   Unstack items         22 [DWORD unknown] 
			name="Unstack items";break;
		case 0x23: //23   9   Item to belt         23 [DWORD id] [DWORD position] 
			name="Item to belt";break;
		case 0x24: //24   5   Item from belt         24 [DWORD id] 
			name="Item from belt";break;
		case 0x25: //25   9   Switch belt item      25 [DWORD cursor item][DWORD belt item] 
			name="Switch belt item";break;
		case 0x26: //26   13   Use belt item         26 [DWORD id] [DWORD option] 00 00 00 00 
			name="Use belt item";break;
		case 0x27: //27   9   Identify item         27 [DWORD item id] [DWORD scroll id] 
			name="Identify item";break;
		case 0x28: //28   9   Socket item         28 [DWORD item to socket id] [DWORD socketable item id] 
			name="Socket item";break;
		case 0x29: //29   9   Scroll to book         29 [DWORD scroll id] [DWORD book id] 
			name="Scroll to book";break;
		case 0x2a: //2a   9   Item to cube (indirect)      2a [DWORD item id] [DWORD cube id] 
			name="Item to cube (indirect)";break;
		case 0x2f: //2f   9   Initiate entity chat      2f [DWORD entity kind] [DWORD id] 
			name="Initiate entity chat";break;
		case 0x30: //30   9   Terminate entity chat      30 [DWORD entity kind] [DWORD id] 
			name="Terminate entity chat";break;
		case 0x31: //31   9   Quest message         31 [DWORD id] [DWORD message] 
			name="Quest message";break;
		case 0x32: //32   17   Buy item from NPC buffer   32 [DWORD entity id] [DWORD item id] [DWORD tab] [DWORD cost] 
			name="Buy item from NPC buffer";break;
		case 0x33: //33   17   Sell item to NPC buffer      33 [DWORD entity id] [DWORD item id] [DWORD tab] [DWORD cost] 
			name="Sell item to NPC buffer";break;
		case 0x34: //34   5   Identify items with NPC      34 [DWORD entity id] 
			name="Identify items with NPC";break;
		case 0x35: //35   17   Repair            35 [DWORD entity id] [DWORD item id] [DWORD tab] [DWORD cost?] 
			name="Repair";break;
		case 0x36: //36   9   Hire merc         36 [DWORD entity id] [DWORD merc id] 
			name="Hire merc";break;
		case 0x37: //37   5   Identify (from gamble)      37 [DWORD item id] 
			name="Identify (from gamble)";break;
		case 0x38: //38   13   Entity action         38 [DWORD action] [DWORD entity id] [DWORD complement] 
			name="Entity action";break;
		case 0x39: //39   5   Purchase Life         39 [DWORD NPC id] 
			name="Purchase Life";break;
		case 0x3a: //3a   3   Add stat point         3a [WORD stat] 
			name="Add stat point";break;
		case 0x3b: //3b   3   Add skill point         3b [WORD skill] 
			name="Add skill point";break;
		case 0x3c: //3c   9   Select skill         3c [WORD skill] 00 [BYTE left (80) or right 00) skill] FF FF FF FF 
			name="Select skill";break;
		case 0x3d: //3d   5   Highlight a door      3d [DWORD object id]   (Logged as Mode (5) crash) 
			name="Highlight a door";break;
		case 0x3e: //3e   5   Activate Scroll of Inifuss   3e [DWORD scroll of infuss. id] 
			name="Activate Scroll of Inifuss";break;
		case 0x3f: //3f   3   Play audio         3f [WORD sound message] 
			name="Play audio";break;
		case 0x40: //40   1   Request Quest Data      40 
			name="Request Quest Data";break;
		case 0x41: //41   1   Resurrect         41 
			name="Resurrect";break;
		case 0x44: //44   17   Staff in orifice      44 [DWORD orifice entity kind] [DWORD orifice id] [DWORD staff id] [DWORD entity state] 
			name="Staff in orifice";break;
		case 0x45: //45   9   Change TP Location      45 [DWORD entity id] [WORD location] 00 00 (logged as hack) 
			name="Change TP Location";break;
		case 0x46: //46   13   Have Merc Interact      46 [DWORD merc id] [DWORD entity id][DWORD entity type] 
			name="Have Merc Interact";break;
		case 0x47: //47   13   Move Merc         47 [DWORD Merc ID] [WORD X] 00 00 [WORD Y] 00 00 
			name="Move Merc";break;
		case 0x48: //48   1   Turns off busy state      48 
			name="Turns off busy state";break;
		case 0x49: //49   9   Take WP/Close WP      49 [DWORD wp id] [BYTE destination] 00 00 00 
			name="Take WP/Close WP";break;
		case 0x4b: //4b   9   Request Entity Update      4b [DWORD entity type][DWORD entity id] 
			name="Request Entity Update";break;
		case 0x4c: //4c   5   Transmorgify   4c [Object ID] 
			name="Transmorgify";break;
		case 0x4d: //4d   3   Play NPC Message      4d [WORD NPC Sound Message] 
			name="Play NPC Message";break;
		case 0x4f: //4f   7   Click Button         4f [DWORD button id] [WORD complement] 
			name="Click Button";break;
		case 0x50: //50   9   Drop Gold         50 [DWORD player id] [DWORD gold pieces] 
			name="Drop Gold";break;
		case 0x51: //51   9   Bind hotkey to skill      51 [BYTE skill]  [BYTE left (80) or right 00) skill] [WORD hotkey] FF FF FF FF 
			name="Bind hotkey to skill";break;
		case 0x53: //53   1   Turn stamina on   (Not used)   53 
			name="Turn stamina on";break;
		case 0x54: //54   1   Turn stamina off (Not used)   54 
			name="Turn stamina off";break;
		case 0x58: //58   3   Quest Completed         58 [WORD Quest id] 
			name="Quest Completed";break;
		case 0x59: //59   17   Make entity move      59 [DWORD entity kind] [DWORD entity id] [WORD x] 00 00 [WORD y] 00 00 
			name="Make entity move";break;
		case 0x5d: //5d   7   Squelch/Hostile         5d [BYTE button] [BYTE toggle on/off] [DWORD player id] 
			name="Squelch/Hostile";break;
		case 0x5e: //5e   6   Invite Party         5e [BYTE button] [DWORD player id] 
			name="Invite Party";break;
		case 0x5f: //5f   5   Update Player Pos      5f [WORD x] [WORD y] 
			name="Update Player Pos";break;
		case 0x60: //60   1   Swap weapons         60 
			name="Swap weapons";break;
		case 0x61: //61   3   Drop/Pickup merc item      61 [WORD position (00 to drop)] 
			name="Drop/Pickup merc item";break;
		case 0x62: //62   5   Resurrect Merc         62 [DWORD npc id] 
			name="Resurrect Merc";break;
		case 0x63: //63   5   Shift left-click item (to belt)   63 [DWORD item] 
			name="Shift left-click item (to belt)";break;
		case 0x66: //66   *   Warden Response         66 [BYTE Data Length] 00 [VARIABLE Data] 
			name="Warden Response";break;
		case 0x68: //68   37   Game Join Request         68 [DWORD Server Hash] [WORD Server Token] [BYTE Char Type] [DWORD VerByte (0x0b)] [DWORD Unknown] [DWORD Unknown] 00 [Char Name[15]] 00 
			name="Game Join Request";break;
		case 0x69: //69   1   Leave Game            69 
			name="Leave Game";break;
		case 0x6b: //6b   1   Join Game            6b 
			name="Join Game";break;
		case 0x6d: //Ping           6d [DWORD Tickcount] [DWORD Delay] 00 00 00 00 
			name="Ping";break;
	}
	//if (cmd!=0xd) return;
	if (logfp) {
		int ms=dwCurMs-dwPHTime;
		fprintf(logfp,"[%d.%03d] SEND %s cmd_%02x %d len=%d\n",ms/1000,ms%1000,name,cmd,arg,len);
		hex(logfp,0,buf,len);
		fflush(logfp);
	}
}

void initSendPacketCheckTable() {
	sendPacketCheckTable[0x0c]=1; //right skill map
	sendPacketCheckTable[0x0d]=1; //right skill unit
	sendPacketCheckTable[0x13]=1; //Interact (click) entity
	sendPacketCheckTable[0x17]=1; //drop item
	sendPacketCheckTable[0x20]=1; //active item
	sendPacketCheckTable[0x33]=1; //sell item
	sendPacketCheckTable[0x36]=1; //hire merc
	sendPacketCheckTable[0x3C]=1; //select skill
	sendPacketCheckTable[0x49]=1; //Take WP/Close WP
	sendPacketCheckTable[0x61]=1; //change merc equipment
}
void onRightSkillMap(int x,int y);
void onRightSkillUnit(int type,int id);
void showResetProtectMsg() {
	wchar_t wszbuf[256];char keyname[256];wszbuf[0]=0;
	formatKey(keyname,tResetProtectionToggle.key);
	wsprintfW(wszbuf, L"Reset Protection, (use %hs to unlock)",keyname);
	D2ShowGameMessage(wszbuf, 0);
}
int ResetProtection(int unitId) {
	if (!tResetProtectionToggle.isOn) return 0;
	UnitAny *pUnit=GetUnitFromIdSafe(unitId,UNITNO_ITEM);
	if (!pUnit) return 0;
	if (pUnit->dwUnitType!=UNITNO_ITEM) return 0;
	int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
	if (index==2146) {
		showResetProtectMsg();
		return 1;
	}
	return 0;
}
/*
click reset menu
6FAFA2A0 - 51                    - push ecx
6FAFA2A1 - E8 AABEFFFF           - call 6FAF6150
*/
void __declspec(naked) ResetSkillPatch_ASM() {
	__asm {
		mov eax,tResetProtectionToggle.isOn
		cmp eax,0
		jne protect
		call dword ptr [D2Unknown6150]
		ret
protect:
		pushad
		call showResetProtectMsg
		popad
		xor eax,eax
		add esp,4
		pop ecx
		ret
	}
}
extern int fAutoSummonSwitchSkill,fAutoSummonSwitchSkillTmp;
int dwInteractEntityCount=0;
int __cdecl blockSendPacket(int regs) {
	int *esp=&regs;
	int len=esp[10];
	int arg=esp[11];
	BYTE *packet=(BYTE *)esp[12];
	int cmd=packet[0]&0xFF;
	switch (cmd) {
		case 0x0c: //right skill map
			onRightSkillMap(*(WORD *)(packet+1),*(WORD *)(packet+3));
			break;
		case 0x0d: //right skill unit
			onRightSkillUnit(*(int *)(packet+1),*(int *)(packet+5));
			break;
		case 0x13: {//Interact (click) entity
			int type=*(int *)&packet[1];
			int id=*(int *)&packet[5];
			dwInteractEntityCount++;
			if (dwMultiClientCount&&(type==2||type==5)) {
				leader_click_object(type,id);
			}
			break;
		}
		case 0x17:case 0x33:case 0x36: return DropProtectionPacketBlock(packet,len);
		case 0x20: { //active item
			int id=*(int *)&packet[1];
			if (ResetProtection(id)) return 1;
			break;
		}
		case 0x3c: {//select skill
			int skill=*(WORD*)&packet[1];
			int left=packet[4];
			fAutoSummonSwitchSkill=fAutoSummonSwitchSkillTmp;fAutoSummonSwitchSkillTmp=0;
			if (tNecTeleportAttract.isOn&&dwPlayerClass==2&&!left) {
				if (skill==54) dwChangeLeftSkill=86;
				else if (dwRightSkill==54) dwChangeLeftSkill=0;
			}
			break;
		}
		case 0x49: {//Take WP/Close WP
			int unitId=*(DWORD *)&packet[1];
			int dest=packet[5];
			if (dest) {
				//gameMessage("waypoint %d to %d mouse=(%d,%d)",unitId,dest,p_D2MousePos->y,p_D2MousePos->x);
				leader_click_object(8,unitId);
				leader_click_object(9,dest);
			}
			break;
		}	
		case 0x61: //change merc equipment
			//TODO save merc
			break;
	}
	return 0;
}
#endif
