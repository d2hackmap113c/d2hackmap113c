#include "stdafx.h"

/*
cls: 0:ama 1:sor 2:nec 3:pal 4:bar 5:dru 6:asn
type: 
ama: 0=bow弓 1=passive被动 2=javelin标枪
sor: 0=fire火 1=light闪电 2=cold冰冻
nec: 0=curses诅咒 1=poision毒素 2=summon召唤
pal: 0=combat作战 1=offensive攻击灵气 2=defensive防御灵气
bar: 0=combat战门 1=combatMasteries战门专家 2=warcries呐喊
dru: 0=summon召唤 1=shape变形 2=elementals元素
asn: 0=traps陷阱 1=shadow影子 2=martial武学
*/
static int clsSkillId[7]={6,36,66,96,126,221,251,};
int skillTypes[7][30]={
	{
		0,0,1,1,2, //L1: 6:Magic Arrow, 7:Fire Arrow | 8:Inner Sight, 9:Critical Strike | 10:Jab,
		0,0,1,2,2, //L6: 11:Cold Arrow, 12:Multiple Shot | 13:Dodge | 14:Power Strike, 15:Poison Javelin,
		0,1,1,2,2, //L12: 16:Exploding Arrow | 17:Slow Missiles, 18:Avoid | 19:Impale, 20:Lightning Bolt,
		0,0,1,2,2, //L18: 21:Ice Arrow, 22:Guided Arrow | 23:Penetrate | 24:Charged Strike, 25:Plague Javelin,
		0,0,1,1,2, //L24: 26:Strafe, 27:Immolation Arrow | 28:Decoy, 29:Evade | 30:Fend,
		0,1,1,2,2, //L30: 31:Freezing Arrow | 32:Valkyrie, 33:Pierce | 34:Lightning Strike, 35:Lightning Fury,
	}, //ama
	{
		0,0,1,2,2, //L1: 36:Fire Bolt火弹, 37:Warmth温暖 | 38:Charged Bolt闪电弹 | 39:Ice Bolt冰弹, 40:Frozen Armor冰封装甲,
		0,1,1,2,2, //L6: 41:Inferno地狱火 | 42:Static Field静电力场, 43:Telekinesis心灵传动 | 44:Frost Nova霜之新星, 45:Ice Blast冰风暴,
		0,0,1,1,2, //L12: 46:Blaze烈焰之径, 47:Fire Ball火球 | 48:Nova新星, 49:Lightning闪电 | 50:Shiver Armor碎冰甲,
		0,0,1,1,2, //L18: 51:Fire Wall火墙, 52:Enchant火焰强化 | 53:Chain Lightning连锁闪电, 54:Teleport传送 | 55:Glacial Spike冰尖柱,
		0,1,1,2,2, //L24: 56:Meteor陨石 | 57:Thunder Storm雷暴, 58:Energy Shield能量护盾 | 59:Blizzard暴风雪, 60:Chilling Armor寒冰装甲,
		0,0,1,2,2, //L30: 61:Fire Mastery火焰支配, 62:Hydra九头海蛇 | 63:Lightning Mastery闪电支配 | 64:Frozen Orb冰封球, 65:Cold Mastery冰冷支配,
	}, //sor
	{
		0,1,1,2,2, //L1: 66:Amplify Damage伤害加深 | 67:Teeth牙, 68:Bone Armor骨盾 | 69:Skeleton Mastery骷髅支配, 70:Raise Skeleton骷髅复生
		0,0,1,1,2, //L6: 71:Dim Vision微暗灵视, 72:Weaken削弱 | 73:Poison Dagger淬毒匕首, 74:Corpse Explosion尸体爆炸 | 75:Clay Golem粘土石魔,
		0,0,1,2,2, //L12: 76:Iron Maiden攻击反噬, 77:Terror恐惧 | 78:Bone Wall骨墙 | 79:Golem Mastery石魔支配, 80:Raise Skeletal Mage骷髅法师,
		0,0,1,1,2, //L18: 81:Confuse迷乱, 82:Life Tap生命偷取 | 83:Poison Explosion毒爆, 84:Bone Spear骨矛 | 85:BloodGolem鲜血石魔,
		0,0,1,2,2, //L24: 86:Attract吸引, 87:Decrepify衰老 | 88:Bone Prison骨牢 | 89:Summon Resist召唤抵抗, 90:IronGolem钢铁石魔,
		0,1,1,2,2, //L30 91:Lower Resist降低抵抗 | 92:Poison Nova剧毒新星, 93:Bone Spirit骨魂 | 94:FireGolem火焰石魔, 95:Revive重生,
	},//nec
	{
		0,0,1,2,2, //L1: 96:Sacrifice, 97:Smite | 98:Might | 99:Prayer, 100:Resist Fire
		0,1,1,2,2, //L6: 101:Holy Bolt | 102:Holy Fire, 103:Thorns | 104:Defiance, 105:Resist Cold
		0,0,1,2,2, //L12: 106:Zeal, 107:Charge | 108:Blessed Aim | 109:Cleansing, 110:Resist Lightning
		0,0,1,1,2, //L18: 111:Vengeance, 112:Blessed Hammer | 113:Concentration, 114:Holy Freeze | 115:Vigor
		0,0,1,1,2, //L24: 116:Conversion, 117:Holy Shield | 118:Holy Shock, 119:Sanctuary | 120:Meditation
		0,1,1,2,2, //L30: 121:Fist of the Heavens | 122:Fanaticism, 123:Conviction | 124:Redemption, 125:Salvation
	},//pal
	{
		0,1,1,1,2,2, //L1: 126:Bash | 127:Sword Mastery, 128:Axe Mastery, 129:Mace Mastery | 130:Howl, 131:Find Potion
		0,0,1,1,1,2,2, //L6: 132:Leap, 133:Double Swing | 134:Pole Arm Mastery, 135:Throwing Mastery, 136:Spear Mastery | 137:Taunt, 138:Shout
		0,0,1,2, //L12: 139:Stun, 140:Double Throw | 141:Increased Stamina | 142:Find Item
		0,0,1,2, //L18: 143:Leap Attack, 144:Concentrate | 145:Iron Skin | 146:Battle Cry
		0,1,2,2, //L24: 147:Frenzy | 148:Increased Speed | 149:Battle Orders, 150:Grim Ward
		0,0,1,2,2, //L30: 151:Whirlwind, 152:Berserk | 153:Natural Resistance | 154:War Cry, 155:Battle Command
	},//bar
	{
		0,0,1,1,2, //L1: 221:Raven, 222:Poison Creeper | 223:Werewolf, 224:Lycanthropy | 225:Firestorm
		0,0,1,2,2, //L6: 226:Oak Sage, 227:Summon Spirit Wolf | 228:Werebear | 229:Molten Boulder, 230:Arctic Blast
		0,1,1,2,2, //L12: 231:Carrion Vine | 232:Feral Rage, 233:Maul撞鎚 | 234:Fissure, 235:Cyclone Armor
		0,0,1,1,2, //L18: 236:Heart of Wolverine, 237:Summon Dire Wolf | 238:Rabies, 239:Fire Claws | 240:Twister
		0,1,1,2,2, //L24: 241:Solar Creeper | 242:Hunger, 243:Shock Wave | 244:Volcano, 245:Tornado
		0,0,1,2,2, //L30: 246:Spirit of Barbs, 247:Summon Grizzly | 248:Fury | 249:Armageddon, 250:Hurricane
	},//dru
	{
		0,1,1,2,2, //L1: 251:Fire Blast | 252:Claw Mastery, 253:Psychic Hammer | 254:Tiger Strike, 255:Dragon Talon
		0,0,1,2,2, //L6: 256:Shock Web, 257:Blade Sentinel | 258:Burst of Speed | 259:Fists of Fire, 260:Dragon Claw
		0,0,1,1,2, //L12: 261:Charged Bolt Sentry, 262:Wake of Fire | 263:Weapon Block, 264:Cloak of Shadows | 265:Cobra Strike,
		0,1,1,2,2, //L18: 266:Blade Fury | 267:Fade, 268:Shadow Warrior | 269:Claws of Thunder, 270:Dragon Tail
		0,0,1,2,2, //L24: 271:Lightning Sentry, 272:Wake of Inferno | 273:Mind Blast | 274:Blades of Ice, 275:Dragon Flight
		0,0,1,1,2, //L30: 276:Death Sentry, 277:Blade Shield | 278:Venom, 279:Shadow Master | 280:Phoenix Strike,
	},//asn
};
int getSkillClass(int skillId) {
	for (int i=0;i<7;i++) if (clsSkillId[i]<=skillId&&skillId<clsSkillId[i]+30) return i;
	return -1;
}
int getSkillType(int cls,int skillId) {
	if (cls<0||cls>6) return -1;
	int idx=skillId-clsSkillId[cls];
	if (idx<0||idx>=30) return -1;
	return skillTypes[cls][idx];
}
int getSkillLevel(UnitAny *pUnit,int skillId) {
	int cls=getSkillClass(skillId);if (cls<0) return 0;
	int type=getSkillType(cls,skillId);if (type<0) return 0;
	int lv=0;
	for (struct Skill *p=pUnit->pSkill->pFirstSkill;p;p=p->pNextSkill) {
		if (p->pSkillInfo->wSkillId==skillId) {lv=p->dwSkillLevel;break;}
	}
	int clsLv=D2GetUnitStat(pUnit, 107, skillId); //class only
	int singleSkill=D2GetUnitStat(pUnit, 97, skillId); 
	if (clsLv||singleSkill) {
		int pcls=getPlayerClass(pUnit->dwUnitId);
		if (pcls<0) pcls=pUnit->dwTxtFileNo;
		if (pcls==cls) {
			lv+=clsLv;
			if (singleSkill>3) singleSkill=3;
		}
	}
	lv+=singleSkill;
	if (!lv) return 0;
	lv+=D2GetUnitStat(pUnit, 188, cls*8+type); //type skill
	lv+=D2GetUnitStat(pUnit, 83, cls); //class skill
	lv+=D2GetUnitStat(pUnit, 127, 0); //all skill
	return lv;
}
RosterUnit *getRosterUnit(int id) {
	for (RosterUnit *pRU = PLAYERLIST; pRU; pRU = pRU->pNext ) if (pRU->dwUnitId==id) return pRU;
	return NULL;
}
UnitAny *getRoomtileByTxt(int txt) {
	for (int i=0;i<128;i++) {
		UnitAny *pUnit=p_D2UnitTable[5*128+i];
		while (pUnit) {
			if (pUnit->dwTxtFileNo==txt) return pUnit;
			pUnit=pUnit->pHashNext;
		}
	}
	return NULL;
}
//level 0:town -1:any
UnitAny *getPortalToArea(int level,char *owner) {
	for (int i=0;i<128;i++) {
		UnitAny *pUnit=p_D2UnitTable[2*128+i];
		while (pUnit) {
			//blue portal 59 red portal 60
			if ((pUnit->dwTxtFileNo==0x3b||pUnit->dwTxtFileNo==0x3c)&&pUnit->pObjectData) {
				int lvl=pUnit->pObjectData->nShrineNo;
				if (level!=0&&level!=-1&&lvl!=level) goto next;
				if (level==0&&lvl!=1&&lvl!=40&&lvl!=75&&lvl!=103&&lvl!=109) goto next;
				char *name=(char *)pUnit->pObjectData;name+=0x28;
				if (!owner||strcmp(owner,name)==0) return pUnit;
			}
next:
			pUnit=pUnit->pHashNext;
		}
	}
	return NULL;
}
int getRoomtileTxtToLevel(int toLevel) {
	//only appear if distance < 20
	for(RoomTile *pRoomtile = PLAYER->pMonPath->pRoom1->pRoom2->pRoomTiles ;pRoomtile ; pRoomtile = pRoomtile->pNext ){
		int txt=*(pRoomtile->pNum);
		int lvl=pRoomtile->pRoom2->pDrlgLevel->dwLevelNo;
		if (lvl==toLevel) return txt;
	}
	return 0;
}
int singlePlayerSaveGame(char *path) {
	char dumpbuf[8192];
	int size;
	int r=D2GetSinglePlayerContext1(0);
	int context0=D2GetSinglePlayerContext2(r);
	if (!context0) return 0;
	int playerInfo=*(int *)(context0+0x88);
	int unitId=*(int *)(playerInfo+0x170);
	int context1=*(int *)(playerInfo+0x1A8);
	int unitType=*(int *)(playerInfo+0x16C);
	UnitAny *pUnit=getSinglePlayerUnit(unitId,unitType);
	if (!pUnit) return 0;
	D2MakeD2S(pUnit,context0,dumpbuf,&size,8192,0,1);
	FILE *fp=fopen(path,"wb+");
	if (!fp) return 0;
	fwrite(dumpbuf,size,1,fp);
	fclose(fp);
	return 0;
}
int encodeItem(UnitAny *pUnit,char *buf,int size) {
//D2Game.dll+53470 - 51                    - push ecx
		if ((pUnit->dwFlags2>>15)&1) return 0;
		return D2EncodeItem(pUnit,buf,size,1,1,0);
}
int hasSkill(int id) {
	if (PLAYER->pSkill){
		for (Skill *pSkill = PLAYER->pSkill->pFirstSkill;pSkill;pSkill= pSkill->pNextSkill) {
			if(pSkill->pSkillInfo->wSkillId != id) continue;
			if ((id==219||id==220)&&pSkill->dwQuality==0) continue;//town portal
			return 1;
		}
	}
	return 0;
}
void selectSkill(int right,int skill) {
	//LOG("select skill %d %d\r\n",right,skill);
	BYTE packet[9] = {0x3C};
	*(WORD*)&packet[1] = skill;
	packet[3]=0;
	packet[4]=right?0:0x80;
	*(DWORD*)&packet[5] = 0xFFFFFFFF;
	SendPacket(packet,sizeof(packet));
}
//Only in single player mode, get the unit on the backend, which has real HP and experience.
UnitAny *getSinglePlayerUnit(int id,int unitType) {
	if (!fSinglePlayer) return NULL;
	if (unitType<0||unitType>=5) return NULL;
	if (!dwSinglePlayerContext) {
		int r=D2GetSinglePlayerContext1(0);
		dwSinglePlayerContext=D2GetSinglePlayerContext2(r);//Crash if called repeatly
	}
	if (!dwSinglePlayerContext) return 0;
	int *offsets=(int *)p_D2SPUnitOffsets;
	int offset=offsets[unitType];
	offset+=(id&0x7f)*4;
	UnitAny *head=*((UnitAny **)(dwSinglePlayerContext+offset));
	for (UnitAny *p=head;p;p=p->pHashNext) {
		if (IsBadReadPtr(p,sizeof(UnitAny))) return NULL;
		if (p->dwUnitType!=unitType) return NULL;
		if (p->dwUnitId==id) return p;
	}
	return NULL;
}
void SendPacket(BYTE *buf,int size) {
	if (0&&logfp) {
		fprintf(logfp,"send %d:",size);
		for (int i=0;i<size;i++) fprintf(logfp," %02x",buf[i]);
		fprintf(logfp,"\r\n");
		fflush(logfp);
	}
	D2SendPacket(size, 0, buf);		
}
/*
//left click on map
args: 0 mouseX mouseY
6FACBF20 - A1 A4ADBA6F           - mov eax,[6FBAADA4] { (1) } esp=0012FC98
...
6FACC020 - E8 BBFDFFFF           - call 6FACBDE0 <-------------click npc
...
6FACC02F - 83 CF 05              - or edi,05 { 5 } <--- edi=0x45
6FACC032 - 8D 74 24 10           - lea esi,[esp+10]
6FACC036 - 89 2D F8C2BC6F        - mov [6FBCC2F8],ebp { (1) } <----0:normal 1:ESC menu 
6FACC03C - 89 2D F4C2BC6F        - mov [6FBCC2F4],ebp { (0) } <----0:normal 1:cursor select item
6FACC042 - 89 7C 24 10           - mov [esp+10],edi               esp=0012FC68 edi=45
6FACC046 - E8 95FDFFFF           - call 6FACBDE0 <---------------- click map esi=0012FC78
*/
int RunToPos(int x,int y) {
	int buf[8];
	buf[0]=0x45;
	buf[1]=(int)PLAYER;
	buf[2]=0; //object
	buf[3]=x;
	buf[4]=y;
	buf[5]=0;buf[6]=0;buf[7]=0;
	*p_D2HasSelectedObject=0;
	*p_D2FlagUnknownC2F8=0;
	__asm {
		lea esi,buf
		call D2LeftClickBDE0
	}
}
int LeftClickObject(UnitAny *pUnit) {
	int buf[8];
	buf[0]=0x45;
	buf[1]=(int)PLAYER;
	buf[2]=(int)pUnit;
	buf[3]=pUnit->pItemPath->dwPosX;
	buf[4]=pUnit->pItemPath->dwPosY;
	buf[5]=0;buf[6]=0;buf[7]=0;
	*p_D2HasSelectedObject=0;
	*p_D2FlagUnknownC2F8=0;
	__asm {
		lea esi,buf
		call D2LeftClickBDE0
	}
}
int RightSkill(UnitAny *pUnit,int x,int y) {
	int buf[8];
	buf[0]=0x46;
	buf[1]=(int)PLAYER;
	buf[2]=(int)pUnit;
	buf[3]=x;
	buf[4]=y;
	buf[5]=0;buf[6]=0;buf[7]=0;
	__asm {
		lea esi,buf
		call D2RightClickBC40
	}
}
/*
dwMode
0,4,7,8,9,10,11,12,17,19 //can't use skill, return 0
1,2,3,5,6 //can use skill, return 1
13: if (dwTxtFileNo==3) return 0;else return 1
14: if (dwTxtFileNo==6) return 0;else return 1
15,16: if (dwTxtFileNo==4,6,5) return 0;else return 1
18: complicated
*/
int canUseSkillNow(UnitAny *pUnit,int skillId) {
	__asm {
		mov eax, pUnit
		push skillId
		call D2CanUseSkillNow
	}
}

/*
Game Controls process function
6FAE8C88 - FF 14 8D 104FBA6F     - call dword ptr [ecx*4+6FBA4F10] 
ecx=cmd*3
cmd	function
0   Character Screen 1   Inventory Screen 2   Party Screen 3   Message Log 4   Quest Log
5   Chat 6   Help Screen
7   Automap (Show/Hide) 8   Center Automap 9   Fade Automap 10  Party on Automap 11  Names on Automap
12  Skill Tree 13  Skill Speed Bar
14  Skill1 15  Skill2 16  Skill3 17  Skill4 18  Skill5 19  Skill6 20  Skill7 21  Skill8
22  Show Belt 23  Use Belt 1 24  Use Belt 2 25  Use Belt 3 26  Use Belt 4
27  Say Help 28  Say Follow me 29  Say This is for you 30  Say Thanks 31  Say Sorry
32  Say Bye 33  Say Now you die
34  Run (up/down) CONTROL 35  Toggle Run/Walk 36  Stand Still (up/down) SHIFT
37  Show Items (up/down) ALT 38  Clear Screen  39  Select Previous Skill 40  Select Next Skill
41  Clear Nessage 43  Show Portraits 44  Swap Weapons 45  Toggle MiniMap (Left/Right)
46  Skill9 47  Skill10 48  Skill11 49  Skill12 50  Skill13 51  Skill14 52  Skill15 53  Skill16
54  Hiring Screen 55  Say Retreat 56  ESC
 0:6FAEC5B0 1:6FAEC2A0 2:6FAEBFC0 3:6FAEBEB0
 4:6FAEC8C0 5:6FAEBC10 6:6FAEBA70 7:6FAEB8B0
 8:6FAEA690 9:6FAE9E20 10:6FAE9DF0 11:6FAE9DC0
 12:6FAEB5B0 13:6FAEB470 14:6FAEADB0 15:6FAEAD70
 16:6FAEAD30 17:6FAEACF0 18:6FAEACB0 19:6FAEAC70
 20:6FAEAC30 21:6FAEABF0 22:6FAEB300 23:6FAEC9A0
 24:6FAEC960 25:6FAEC920 26:6FAEC8E0 27:6FAE9D60
 28:6FAE9D40 29:6FAE9D20 30:6FAE9D00 31:6FAE9CE0
 32:6FAE9CC0 33:6FAE9CA0 34:6FAEAE40 35:6FAE9C60
 36:6FAE9C50 37:6FAEB230 38:6FAEA900 39:6FAEA8E0
 40:6FAEA8C0 41:6FAE9C30 42:0 43:6FB0C3D0
 44:6FAEA9C0 45:6FAE9D80 46:6FAEABB0 47:6FAEAB70
 48:6FAEAB30 49:6FAEAAF0 50:6FAEAAB0 51:6FAEAA70
 52:6FAEAA30 53:6FAEA9F0 54:6FAEAEE0 55:6FAE9C80
 56:6FAEC880
*/
void useBelt(int id,int shift) {
	if (id<0||id>3) return;
// 6FAEC9A0 - A1 A4C3BC6F           - mov eax,[6FBCC3A4] { (0) }
	if (*p_D2notAcceptKeys) return;
	if (!PLAYER) return;
	if (PLAYER->dwMode==0x11) return;
	if (!p_D2BeltNotEmpty[id]) return;
	if (shift) shift=0x8000;
	__asm {
		mov eax,shift;
	}
	D2UseBelt(PLAYER->pInventory,PLAYER,id);
}
int GameControlStart(int cmd) {
	//wchar_t temp[256];wsprintfW(temp,  L"game control %d", cmd);D2ShowGameMessage(temp, 0);
	if (60<=cmd&&cmd<=63) {useBelt(cmd-60,1);return 0;}
	if (!fInGame) return -1;
	if (cmd<0||cmd>56) return -1;
	if (cmd==36) forceStandStill=1;
	int addr=p_D2GameControlStart[cmd*3];
	if (addr&&!IsBadCodePtr((FARPROC)addr)) ((void (*)())addr)();
	return 0;
}
int GameControlEnd(int cmd) {
	if (60<=cmd&&cmd<=63) return -1;
	if (!fInGame) return -1;
	if (cmd<0||cmd>56) return -1;
	if (cmd==36) forceStandStill=0;
	int addr=p_D2GameControlEnd[cmd*3];
	if (addr&&!IsBadCodePtr((FARPROC)addr)) ((void (*)())addr)();
	return 0;
}
void GameControl(int cmd) {GameControlStart(cmd);GameControlEnd(cmd);}
int getGameControlKey(int cmd) {
	for (KeyboardSetting *p=p_D2KeyboardSettingStart;p<p_D2KeyboardSettingEnd;p++) 
		if (p->cmd==cmd&&p->key!=-1) 
			return p->key;
	return 0;
}
//6FAF33D1 - C7 05 F0C3BC6F 00000000 - mov [6FBCC3F0],00000000 { (0),0 } <----------------------- keep running
void __declspec(naked) StandStillPatch_ASM() {
	__asm {
		mov eax,p_D2StandStill
		cmp forceStandStill,0
		je end
		mov dword ptr [eax], 1
		ret
//original code
end:
		mov dword ptr [eax], 0
		ret
	}
}
static wchar_t wszBAlertMsg1[512] = {L'\0'} ;
static wchar_t wszBAlertMsg2[512] = {L'\0'} ;
static wchar_t wszBAlertMsg3[512] = {L'\0'} ;
static int dwBAlertMsgMs1,fBAlertBlink1;
static int dwBAlertMsgColor1=0,dwBAlertMsgColorMs1=0;
static int dwBAlertMsgMs2,fBAlertBlink2;
static int dwBAlertMsgColor2=0,dwBAlertMsgColorMs2=0;
static int dwBAlertMsgMs3,fBAlertBlink3;
static int dwBAlertMsgColor3=0,dwBAlertMsgColorMs3=0;
void SetBottomAlertMsg1(wchar_t *wszMsg, int ms, int color,int blink) {	
	wcscpy(wszBAlertMsg1,wszMsg);dwBAlertMsgMs1=dwCurMs+ms;
	if (!blink||!fBAlertBlink1) {dwBAlertMsgColor1=color;dwBAlertMsgColorMs1=(dwCurMs+256)&0xFFFFFF00;}
	fBAlertBlink1=blink;
}
void SetBottomAlertMsg2(wchar_t *wszMsg, int ms, int color,int blink) {	
	wcscpy(wszBAlertMsg2,wszMsg);dwBAlertMsgMs2=dwCurMs+ms;
	if (!blink||!fBAlertBlink2) {dwBAlertMsgColor2=color;dwBAlertMsgColorMs2=(dwCurMs+256)&0xFFFFFF00;}
	fBAlertBlink2=blink;
}
void SetBottomAlertMsg3(wchar_t *wszMsg, int ms, int color,int blink) {	
	wcscpy(wszBAlertMsg3,wszMsg);dwBAlertMsgMs3=dwCurMs+ms;
	if (!blink||!fBAlertBlink3) {dwBAlertMsgColor3=color;dwBAlertMsgColorMs3=(dwCurMs+256)&0xFFFFFF00;}
	fBAlertBlink3=blink;
}
void DrawCenterAlertMsg(){
  //需要DrawClientPatch支持
	if (dwBAlertMsgMs1) {
		if (dwCurMs>dwBAlertMsgMs1) {dwBAlertMsgMs1=0;fBAlertBlink1=0;}
		if (fBAlertBlink1&&dwCurMs>dwBAlertMsgColorMs1) {
			dwBAlertMsgColor1=dwBAlertMsgColor1==1?2:1;
			dwBAlertMsgColorMs1=(dwCurMs+256)&0xFFFFFF00;
		}
		DrawCenterText(3, wszBAlertMsg1 , SCREENSIZE.x/2, SCREENSIZE.y/2+120, dwBAlertMsgColor1, 1,0);
	}
	if (dwBAlertMsgMs2) {
		if (dwCurMs>dwBAlertMsgMs2) {dwBAlertMsgMs2=0;fBAlertBlink2=0;}
		if (fBAlertBlink2&&dwCurMs>dwBAlertMsgColorMs2) {
			dwBAlertMsgColor2=dwBAlertMsgColor2==1?2:1;
			dwBAlertMsgColorMs2=(dwCurMs+256)&0xFFFFFF00;
		}
		DrawCenterText(3, wszBAlertMsg2 , SCREENSIZE.x/2, SCREENSIZE.y/2+150, dwBAlertMsgColor2, 1,0);
	}
	if (dwBAlertMsgMs3) {
		if (dwCurMs>dwBAlertMsgMs3) {dwBAlertMsgMs3=0;fBAlertBlink3=0;}
		if (fBAlertBlink3&&dwCurMs>dwBAlertMsgColorMs3) {
			dwBAlertMsgColor3=dwBAlertMsgColor3==1?2:1;
			dwBAlertMsgColorMs3=(dwCurMs+256)&0xFFFFFF00;
		}
		DrawCenterText(3, wszBAlertMsg3 , SCREENSIZE.x/2, SCREENSIZE.y/2+150, dwBAlertMsgColor3, 1,0);
	}
}

void ShowGameMessage(char *Msg)
{
	wchar_t temp[512];
	wsprintfW2(temp, "<Hackmap>: %s ", Msg);
	D2ShowGameMessage(temp, 0);
}
void ShowGameMessage(DWORD val)
{
	wchar_t temp[512];
	wsprintfW(temp, L"<Hackmap>: %d ", val);
	D2ShowGameMessage(temp, 0);
}


WORD GetAreaLevel()
{	
	static DWORD dwLastLevelNo = 0 ;
	static WORD  wAreaLevel = 0 ;
	if( dwLastLevelNo != LEVELNO){
		dwLastLevelNo = LEVELNO;
		LevelTxt *pLvlTxt=D2GetLevelTxt(LEVELNO);
		wAreaLevel = pLvlTxt->nMonLv[EXPANSION][DIFFICULTY];	
	}
	return wAreaLevel;
} 

PetUnit * GetPetByType(UnitAny *pUnit,int type) {
	for (PetUnit *pPetUnit = *p_D2PetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) 
		if( pPetUnit->dwPetType==type && pPetUnit->dwOwnerId == pUnit->dwUnitId && pPetUnit->dwOwerType==UNITNO_PLAYER ) 
			return pPetUnit;
	return NULL;
}
UnitAny *GetUnitFromIdSafe(DWORD id, DWORD type) {
	for (UnitAny *pUnit=p_D2UnitTable[(type<<7)+(id&0x7F)];pUnit;pUnit=pUnit->pHashNext) {
		if (IsBadReadPtr(pUnit,sizeof(UnitAny))) return NULL;
		if (pUnit->dwUnitType!=type) return NULL;
		if (pUnit->dwUnitId==id) return pUnit;
	}
	return NULL;
}
/*获得玩家的雇佣兵*/
UnitAny * GetUnitPet(UnitAny *pUnit) {
	PetUnit *pPetUnit = GetPetByType(pUnit,7);
	if (pPetUnit) return GetUnitFromIdSafe(pPetUnit->dwUnitId , UNITNO_MONSTER ) ;
	return NULL;
}

int getPlayerClass(DWORD id) {
	for (RosterUnit *pUnit = PLAYERLIST ; pUnit; pUnit=pUnit->pNext){
		if (pUnit->dwUnitId== id) return pUnit->dwClassId;
	}
	return -1;
}
int sameParty(DWORD dwUnitId , DWORD dwTargetUnitId){
	if (dwUnitId==dwTargetUnitId) return 1;
	WORD wPartyId = (WORD)-1 , wPartyIdTarget = (WORD)-1 ;
	for (RosterUnit *pUnit = PLAYERLIST ; pUnit; pUnit=pUnit->pNext){
		if ( pUnit->dwUnitId== dwUnitId){
			wPartyId=pUnit->wPartyId;
		}else if ( pUnit->dwUnitId==dwTargetUnitId){
			wPartyIdTarget=pUnit->wPartyId;
			if( wPartyId!=(WORD)-1 )break;
		}
	}
	if( wPartyId!=(WORD)-1 && wPartyId==wPartyIdTarget)return 1;
	return 0 ;
}
BYTE TestPvpFlag(DWORD dwUnitId , DWORD dwTargetUnitId){
	// 0 敌对  1 中立 2 队友 3 自身
	if (dwUnitId==dwTargetUnitId) return 3;
	WORD wPartyId = (WORD)-1 , wPartyIdTarget = (WORD)-1 ;
	for (RosterUnit *pUnit = PLAYERLIST ; pUnit; pUnit=pUnit->pNext){
		if ( pUnit->dwUnitId== dwUnitId){
			wPartyId=pUnit->wPartyId;
			if ( pUnit->pPvPInfo ){
				PvPInfo *pPvPInfo =  *(pUnit->pPvPInfo);
				while( pPvPInfo ){
					if (pPvPInfo->dwUnitId==dwTargetUnitId){
						if (pPvPInfo->dwFlag & 8){
							return 0 ;
						}else{
							break;	
						}
					}
					pPvPInfo = pPvPInfo->pNext;
				}
			}
		}else if ( pUnit->dwUnitId==dwTargetUnitId){
			wPartyIdTarget=pUnit->wPartyId;
			if( wPartyId!=(WORD)-1 )break;
		}
	}
	if( wPartyId!=(WORD)-1 && wPartyId==wPartyIdTarget)return 2;
	return 1 ;
}

//获取玩家距离，单位码数
double __fastcall GetUnitDistanceInSubtiles(UnitAny *pUnit1 , UnitAny *pUnit2){

	DWORD xpos1 ,xpos2 , ypos1 , ypos2 ;
	if ( pUnit1->dwUnitType==UNITNO_PLAYER || pUnit1->dwUnitType==UNITNO_MONSTER || pUnit1->dwUnitType==UNITNO_MISSILE){
		xpos1 = D2GetUnitPosX(pUnit1->pMonPath);
		ypos1 = D2GetUnitPosY(pUnit1->pMonPath);
	}else{
		xpos1 = pUnit1->pItemPath->dwPosX;
		ypos1 = pUnit1->pItemPath->dwPosY;
	}
	if ( pUnit2->dwUnitType==UNITNO_PLAYER || pUnit2->dwUnitType==UNITNO_MONSTER || pUnit2->dwUnitType==UNITNO_MISSILE){
		xpos2 = D2GetUnitPosX(pUnit2->pMonPath); 
		ypos2 = D2GetUnitPosY(pUnit2->pMonPath); 
	}else{
		xpos2 = pUnit2->pItemPath->dwPosX;
		ypos2 = pUnit2->pItemPath->dwPosY;
	}
	
	return 2*sqrt( double((xpos1-xpos2)*(xpos1-xpos2) + (ypos1-ypos2)*(ypos1-ypos2)))/3;
}

//获得魔法属性,只查询第一层
DWORD GetUnitMagicStat(UnitAny *pUnit , DWORD dwStatNo) {
	StatList *pStatList = D2GetStatListWithFlag( pUnit,	0,	0x40 );
	StatEx stat ;
	if ( pStatList && D2GetStatFromStatList(pStatList,dwStatNo , &stat , 0x1ff) ) return stat.dwStatValue;
	return 0;
}

//抗性惩罚
int GetUnitResist( UnitAny *pUnit , DWORD dwStatNo ) {
	static int resistoffset[2][3]={{0,-20,-50},{0,-40,-100}};
	int resist = D2GetUnitStat(pUnit, dwStatNo, 0) + resistoffset[EXPANSION][DIFFICULTY];
	if ( resist<-100 ) resist = -100 ;
	return resist;
}

//目标是否能选择(有装备)
BOOL CheckUnitSelect( UnitAny *pUnit ){
	if ( pUnit && pUnit->pInventory ){
		if ( pUnit->dwUnitType == UNITNO_PLAYER ){
			return pUnit->dwMode && pUnit->dwMode != 0x11;
		}
		if ( pUnit->dwUnitType == UNITNO_MONSTER ){

			if ( pUnit->dwMode && pUnit->dwMode != 0x0C ) {

				switch ( pUnit->dwTxtFileNo ){
					case 357:
					case 291:
					case 417:
					case 418:
						return TRUE ;
					default:
						return  FALSE;
				}

			}

		}
	}
	return FALSE;
}

int GetItemILvlAfterCube(int icubeType,int Ilvl , int Clvl)
{
	int NewLvl;
	if(icubeType==1)//craft
	{
		NewLvl=(Ilvl>>1)+(Clvl>>1);
		return NewLvl>99?99:(NewLvl<1?1:NewLvl);
	}
	else if(icubeType==2)//skull cube
	{
		NewLvl=(int)(Ilvl*0.4)+(int)(Clvl*0.4);
		return NewLvl>99?99:(NewLvl<1?1:NewLvl);
	}
	return 0;
}
int GetItemAlvl(int Ilvl,int Qlvl,int magic_lvl)
{
	int Alvl;
	if (Ilvl>99) Ilvl=99 ;
	if (Qlvl>Ilvl) Ilvl=Qlvl;
	if (magic_lvl>0) 
		Alvl=Ilvl+magic_lvl;
	else
	{
		if ( Ilvl<( 99-(Qlvl>>1) ) )
			Alvl=Ilvl-(Qlvl>>1) ;
		else 
			Alvl=(Ilvl<<1)-99;
	}
	return Alvl>99?99:Alvl;
}


//是否屏幕中
BOOL TestMonsterOnScreen(UnitAny *mon, UnitAny *pla)
{
	return abs((int)mon->pMonPath->dwMapPosX-(int)pla->pMonPath->dwMapPosX) < SCREENSIZE.x/2
		&& abs((int)mon->pMonPath->dwMapPosY-(int)pla->pMonPath->dwMapPosY) < (SCREENSIZE.y-48)/2;
}
void PreparePosForDraw(POINT* pos, int x, int y)
{
	pos->x = x/DIVISOR-OFFSET.x+8;
	pos->y = y/DIVISOR-OFFSET.y-8;
	if ( MINMAPTYPE ) {
		--pos->x;
		pos->y += 5;
	}
}

void MapToAbsScreen(POINT* pos, int x, int y)
{
	pos->x = 5*(x-y)*16;
	pos->y = 5*(x+y)*8;
}



CellFile *LoadBmpCellFile(BYTE *buf1, int width, int height)
{
	BYTE *buf2 = new BYTE[(width*height*2)+height], *dest = buf2;

	for (int i = 0; i < height; i++) {
		BYTE *src = buf1+(i*((width+3)&-4)), *limit = src+width;
		while (src < limit) {
			BYTE *start = src, *limit2 = min(limit, src+0x7f), trans = !*src;
			do src++; while ((trans == (BYTE)!*src) && (src < limit2));
			if (!trans || (src < limit)) *dest++ = (trans?0x80:0)+(src-start);
			if (!trans) while (start < src) *dest++ = *start++;
		}
		*dest++ = 0x80;
	}

	static DWORD dc6head[] = {6, 1, 0, 0xeeeeeeee, 1, 1, 0x1c,  0, -1, -1, 0, 0, 0, -1, -1};
	dc6head[8] = width;
	dc6head[9] = height;
	BYTE *ret = new BYTE[dc6head[13] = sizeof(dc6head)+(dc6head[14] = dest-buf2)+3];
	memset(memcpy2(memcpy2(ret, dc6head, sizeof(dc6head)), buf2, dc6head[14]), 0xee, 3);
	delete buf2;

	return (CellFile *)ret;
}

CellFile *LoadBmpCellFile(char *filename)
{
	BYTE *ret = 0;
		
	BYTE *buf1 = AllocReadFile(filename);
	BITMAPFILEHEADER *bmphead1 = (BITMAPFILEHEADER *)buf1;
	BITMAPINFOHEADER *bmphead2 = (BITMAPINFOHEADER *)(buf1+sizeof(BITMAPFILEHEADER));
	if (buf1 && (bmphead1->bfType == 'MB') && (bmphead2->biBitCount == 8) && (bmphead2->biCompression == BI_RGB)) {
		ret = (BYTE *)LoadBmpCellFile(buf1+bmphead1->bfOffBits, bmphead2->biWidth, bmphead2->biHeight);
	}
	delete buf1;

	return (CellFile *)ret;
}

CellFile *InitCellFile(CellFile *pCellFile)
{
	if (pCellFile) D2InitCellFile(pCellFile, &pCellFile, "?", 0, -1, "?");
	return pCellFile;
}

void DeleteCellFile(CellFile *pCellFile)
{
	if (pCellFile) {
		D2DeleteCellFile(pCellFile);
		delete pCellFile;
	}
}


DWORD GetTextHeight(wchar_t *wStr)
{
	DWORD dwWidth, dwHeight;
	D2GetTextAreaSize(wStr, &dwWidth, &dwHeight);
	return dwHeight;
}

DWORD GetTextWidth(wchar_t *wStr)
{
	DWORD dwWidth, dwHeight;
	D2GetTextAreaSize(wStr, &dwWidth, &dwHeight);
	return dwWidth;
}


void DrawD2Text(DWORD dwFont, wchar_t *wStr, int xpos, int ypos, DWORD dwColor,DWORD dwAlign)
{
	DWORD dwOldFone = D2SetTextFont(dwFont);
	D2DrawText(wStr, xpos, ypos, dwColor, dwAlign);
	D2SetTextFont(dwOldFone);
}

void DrawCenterText(DWORD dwFont, wchar_t *wStr, int xpos, int ypos, DWORD dwColor, int div , DWORD dwAlign)
{
	DWORD dwOldFone = D2SetTextFont(dwFont);
	D2DrawText(wStr, xpos-(GetTextWidth(wStr)>>div), ypos, dwColor, dwAlign);//dwAlign:多行时对齐有用 1居中 0 靠左
	D2SetTextFont(dwOldFone);
}

void DrawDefaultFontText(wchar_t *wStr, int xpos, int ypos, DWORD dwColor, int div , DWORD dwAlign)
{
	D2DrawText(wStr, xpos-(D2GetTextPixelLen(wStr)>>div), ypos, dwColor, dwAlign);//dwAlign:多行时对齐有用 1居中 0 靠左
}


extern int fSkipPainting;
void ExitGame() {
	fSkipPainting=0;
	*p_D2ExitAppFlag = 0;
	SendMessage(D2GetHwnd(), WM_CLOSE, 0, 0);
}


void DrawAutomapCell(CellFile *pCellfile, int xpos, int ypos, BYTE col)
{
	if ( !pCellfile )return ;
	
	CellContext ct;
	memset(&ct, 0, sizeof(ct));
	ct.pCellFile = pCellfile ;

	xpos -= (pCellfile->pCells[0]->dwWidth/2);
	ypos += (pCellfile->pCells[0]->dwHeight/2);

	int xpos2 = xpos - pCellfile->pCells[0]->dwXoffs ;
	int ypos2 = ypos - pCellfile->pCells[0]->dwYoffs ;

	if ( ( xpos2 >= SCREENSIZE.x ) || ( (xpos2 + (int)pCellfile->pCells[0]->dwWidth) <= 0 ) || (ypos2 >= SCREENSIZE.y) || ((ypos2 + (int)pCellfile->pCells[0]->dwHeight) <= 0) ) return;

	static BYTE coltab[2][256];//, tabno = 0, lastcol = 0;
	if (!coltab[0][1]) for (int k = 0; k < 255; k++) coltab[0][k] = coltab[1][k] = k;
	pCellfile->mylastcol = coltab[pCellfile->mytabno ^= (col != pCellfile->mylastcol)][255] = col;

	D2DrawCellFile(&ct, xpos, ypos, -1, 5, coltab[pCellfile->mytabno]);
}


int GetGameLanguage(){

	wchar_t wszTemp[512];
	wcscpy(wszTemp,D2GetLocaleText(0xFDA));
	if( wszTemp[0] == L'E'){
		return 0;
	}else{
		if ( wszTemp[0] == 32147 ){
			return 2;
		}
		return 1;
	}

}
int getFcrFrame(int type,UnitAny *pUnit) {
	int fcr = D2GetUnitStat(pUnit, STAT_FCR, 0);
	switch (type) {
		case 0: //ama
			if (fcr>=152) return 11;
			if (fcr>=99) return 12;
			if (fcr>=68) return 13;
			if (fcr>=48) return 14;
			if (fcr>=32) return 15;
			if (fcr>=22) return 16;
			if (fcr>=14) return 17;
			if (fcr>=7) return 18;
			return 19;
		case 1: //sor
		case 4: //bar
			if (fcr>=200) return 7;
			if (fcr>=105) return 8;
			if (fcr>=63) return 9;
			if (fcr>=37) return 10;
			if (fcr>=20) return 11;
			if (fcr>=9) return 12;
			return 13;
			/*//lighting
			if (fcr>=194) return 11;
			if (fcr>=117) return 12;
			if (fcr>=78) return 13;
			if (fcr>=52) return 14;
			if (fcr>=35) return 15;
			if (fcr>=23) return 16;
			if (fcr>=15) return 17;
			if (fcr>=7) return 18;
			return 19;
			*/
		case 2: //nec
		case 3: //pal
			if (fcr>=125) return 9;
			if (fcr>=75) return 10;
			if (fcr>=48) return 11;
			if (fcr>=30) return 12;
			if (fcr>=18) return 13;
			if (fcr>=9) return 14;
			return 15;
			/*//nec vampire
			if (fcr>=180) return 13;
			if (fcr>=120) return 14;
			if (fcr>=86) return 15;
			if (fcr>=65) return 16;
			if (fcr>=48) return 17;
			if (fcr>=35) return 18;
			if (fcr>=24) return 19;
			if (fcr>=18) return 20;
			if (fcr>=11) return 21;
			if (fcr>=6) return 22;
			return 23;
			*/
		case 5: //dru
			if (fcr>=292) return 9;
			if (fcr>=163) return 10;
			if (fcr>=99) return 11;
			if (fcr>=62) return 12;
			if (fcr>=45) return 13;
			if (fcr>=30) return 14;
			if (fcr>=19) return 15;
			if (fcr>=10) return 16;
			if (fcr>=4) return 17;
			return 18;
			/*//dru wolf
			if (fcr>=157) return 9;
			if (fcr>=95) return 10;
			if (fcr>=60) return 11;
			if (fcr>=40) return 12;
			if (fcr>=26) return 13;
			if (fcr>=14) return 14;
			if (fcr>=6) return 15;
			return 16;
			//dru bear
			if (fcr>=330) return 9;
			if (fcr>=163) return 10;
			if (fcr>=99) return 11;
			if (fcr>=63) return 12;
			if (fcr>=40) return 13;
			if (fcr>=26) return 14;
			if (fcr>=15) return 15;
			if (fcr>=7) return 16;
			return 17;
			*/
		case 6: //asn
			if (fcr>=174) return 9;
			if (fcr>=102) return 10;
			if (fcr>=65) return 11;
			if (fcr>=42) return 12;
			if (fcr>=27) return 13;
			if (fcr>=16) return 14;
			if (fcr>=8) return 15;
			return 16;
	}
	return 23;
}
