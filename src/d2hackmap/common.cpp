#include "stdafx.h"
#include "header.h"
#include <sys/stat.h>

//cls: 0:ama 1:sor 2:nec 3:pal 4:bar 5:dru 6:asn
World *singlePlayerWorld=NULL;
int forceStandStill=0;

static int resistoffset[2][3]={{0,-20,-50},{0,-40,-100}};
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_INT_ARRAY1, "ResistOffsetClassic",resistoffset[0],3,{0}},
  {CONFIG_VAR_TYPE_INT_ARRAY1, "ResistOffsetExpansion",resistoffset[1],3,{0}},
};
void common_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}

/*IAS:frame
ama(Hunter Bow): 0:12 10:11 25:10 45:9 75:8 145:7
ama(Composite-Bow): 0:12 10:11 25:10 45:9 75:8 145:7
ama(Long Bow): 0:13 10:12 20:11 40:10 65:9 105:8 200:7
ama(Repeating-Crossbow): 0:14 5:13 20:12 35:11

bar(Long Bow): 0:14 10:13 20:12 35:11 55:10 90:9 155:8 
pal(Long Bow): 0:15 10:14 20:13 30:12 50:11 75:10 125:9 
dru(Long Bow): 0:15 10:14 20:13 30:12 50:11 75:10 125:9 
sor(Long Bow): 0:16 10:15 20:14 30:13 45:12 65:11 105:10 175:9 
asn(Long Bow): 0:16 10:15 20:14 30:13 45:12 65:11 105:10 175:9 
nec(Long Bow): 0:17 10:16 15:15 30:14 40:13 60:12 90:11 140:10
pal(Smite Caduceus+L20 Fanatiscism) 0:8 10:7 35:6
pal(Smite Caduceus ) 0:10 15:9 30:8 60:7 130:6
pal(Smite Might Scepter+L20 Fanatiscism) 0:8 20:7 55:6
pal(Smite Might Scepter ) 0:11 15:10 30:9 50:8 90:7 180:6
FHR
Faster Hit Recovery Table（简单版本）
=============================================================================
FHR(%)-----|speed=15--14--13--12--11--10---9---8---7---6---5---4---3---2(frame)
-----------------------------------------------------------------------------
Sor--------|-------0---5---9--14--20--30--42--60--86--142-280-1480-----------
Dru/Nec----|---------------0---5--10--16--26--39--56---86-152-377------------
Ama--------|-----------------------0---6--13--20--32---52--86-174-600--------
Pal/Asn/Bar|-------------------------------0---7--15---27--48--86-200-4680---
=============================================================================
*/

void UnicodeFix2(LPWSTR lpText) {
	if (lpText) {
		size_t LEN = wcslen(lpText);
		for (size_t i = 0; i < LEN; i++) {
			if (lpText[i] == 0xf8f5) // Unicode 'y'
				lpText[i] = 0xff; // Ansi 'y'
		}
	}
}
int MyMultiByteToWideChar(
						UINT CodePage,         // code page
						DWORD dwFlags,         // character-type options
						LPCSTR lpMultiByteStr, // string to map
						int cbMultiByte,       // number of bytes in string
						LPWSTR lpWideCharStr,  // wide-character buffer
						int cchWideChar        // size of buffer
						) {
	int r = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	UnicodeFix2(lpWideCharStr);
	return r;
}

// GB2312 => GBK
void GB2GBK(char *szBuf) {
	if(!strcmp(szBuf, ""))return;
	int nStrLen = strlen(szBuf);
	DWORD wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
	int nReturn = LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nStrLen, NULL, 0);
	if(!nReturn)return;
	char *pcBuf = new char[nReturn + 1];
	__try{
		wLCID = MAKELCID(MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), SORT_CHINESE_PRC);
		LCMapString(wLCID, LCMAP_TRADITIONAL_CHINESE, szBuf, nReturn, pcBuf, nReturn + 1);
		strncpy(szBuf, pcBuf, nReturn);
	} __finally {
		delete[] pcBuf;
	}
}
int readFully(FILE *fp,char *buf,int size) {
	int total=0;
	while (size>0) {int n=fread(buf,1,size,fp);if (n<=0) break;buf+=n;size-=n;total+=n;}
	return total;
}
int getFileSize(const char *path) {
	struct stat st;
	if (stat(path,&st)!=0) return -1;
	return st.st_size;
}
int fileExist(const char *path) {
	return getFileSize(path)!=-1;
}
FILE *openFile(char *path,char *mode) {
	char abspath[MAX_PATH];
	if (path[0]&&path[1]==':') //absolute path
		_snprintf(abspath,MAX_PATH,"%s",path);
	else
		_snprintf(abspath,MAX_PATH,"%s%s",szPluginPath,path);
	FILE *fp=fopen(abspath,mode);
	if (!fp) {
		if (fInGame) {
			wchar_t temp[256];
			wsprintfW(temp,  L"can't open %hs", abspath);
			d2client_ShowGameMessage(temp, 0);
		}
		LOG("open %s %s failed\n",abspath,mode);
		return NULL;
	} 
	return fp;
}
char *heap_strdup(HANDLE heap,char *s) {
	int len=strlen(s)+1;
	char *p=(char *)HeapAlloc(heap,0,len);if (!p) return NULL;
	memcpy(p,s,len);
	return p;
}
char *loadFile(HANDLE heap,FILE *fp,int *psize) {
	*psize=0;
	fseek(fp,0,2);int size=ftell(fp);if (psize) *psize=size;
	char *p;
	if (heap) p=(char *)HeapAlloc(heap,0,size+1);
	else p=(char *)malloc(size+1);
	if (!p) return NULL;
	p[size]=0;fseek(fp,0,0);
	int pos=readFully(fp,p,size);
	if (pos<size) {
		LOG("ERROR: incomplete read %d/%d\r\n",pos,size);
		if (fInGame) {
			wchar_t temp[256];
			wsprintfW(temp,  L"incomplete read %d/%d", pos,size);
			d2client_ShowGameMessage(temp, 0);
		}
		if (psize) *psize=pos;
		return p;
	} 
	return p;
}
static void (__stdcall *loadhackmap_msg)(wchar_t *s,int color)=NULL;
extern int fStartingGame;
static void d2msg(int party,wchar_t *s,int color) {
	if (fInGame||fStartingGame) {
		if (!party) d2client_ShowGameMessage(s, color);
		else d2client_ShowPartyMessage(s, color);
	} else {
		if (!loadhackmap_msg) {
			HMODULE loadhackmap = GetModuleHandle("loadhackmap.dll");
			if (!loadhackmap) {
				LOG("Can't find loadhackmap\n");
				return;
			}
			loadhackmap_msg=(void (__stdcall *)(wchar_t *,int))GetProcAddress(loadhackmap,"MainMenuMessage");
			if (!loadhackmap_msg) {
				LOG("Can't find MainMenuMessage\n");
				return;
			}
		}
		//LOG("MainMenuMessage=%X\n",loadhackmap_msg);
		if (loadhackmap_msg) loadhackmap_msg(s,color);
	}
}
void __cdecl gameMessage(char *fmt, ...) {
	va_list va;
	wchar_t wszbuf[256];
	char buf[256];
	va_start(va, fmt);
	int len = wvsprintf(buf, fmt, va);
	if (len>255) buf[255]=0;
	wsprintfW(wszbuf, L"%hs",buf);
	LOG("%s\n",buf);
	d2msg(0,wszbuf,0);
}
void __cdecl partyMessage(char *fmt, ...) {
	va_list va;
	wchar_t wszbuf[256];
	char buf[256];
	va_start(va, fmt);
	int len = wvsprintf(buf, fmt, va);
	if (len>255) buf[255]=0;
	wsprintfW(wszbuf, L"%hs",buf);
	LOG("%s\n",buf);
	d2msg(1,wszbuf,0);
}
void __cdecl gameMessageW(wchar_t *fmt, ...) {
	va_list va;
	wchar_t wszbuf[256];
	va_start(va, fmt);
	wvsprintfW(wszbuf, fmt,va);
	d2msg(0,wszbuf,0);
}
void __cdecl partyMessageW(wchar_t *fmt, ...) {
	va_list va;
	wchar_t wszbuf[256];
	va_start(va, fmt);
	wvsprintfW(wszbuf, fmt,va);
	d2msg(1,wszbuf,0);
}
void __cdecl partyMessageWcolor(int color,wchar_t *fmt, ...) {
	va_list va;
	wchar_t wszbuf[256];
	va_start(va, fmt);
	wvsprintfW(wszbuf, fmt,va);
	d2msg(1,wszbuf,color);
}

//static int get_processor_number() {SYSTEM_INFO info;GetSystemInfo(&info);return (int)info.dwNumberOfProcessors;}
int get_cpu_usage(int *ucpu,int *kcpu) {
  //static int ncpu = 0;if (!ncpu) ncpu = get_processor_number();
  static int ltime = 0;
  static int lutime,lktime = 0;
  FILETIME now,creation_time,exit_time,utime,ktime;
  GetSystemTimeAsFileTime(&now);
  HANDLE hProcess = GetCurrentProcess(); //OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
  if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &ktime, &utime)) return -1;
  if (!ltime) {
		ltime = now.dwLowDateTime;lutime = utime.dwLowDateTime;lktime = ktime.dwLowDateTime;
    return 0;
  }
	int dktime=ktime.dwLowDateTime-lktime;
	int dutime=utime.dwLowDateTime-lutime;
  int dtime = now.dwLowDateTime - ltime;
	if (dtime) {
		*ucpu = (int)((dutime*100+dtime/2)/dtime);
		*kcpu = (int)((dktime*100+dtime/2)/dtime);
	}
  ltime = now.dwLowDateTime;lutime = utime.dwLowDateTime;lktime = ktime.dwLowDateTime;
  return 0;
}
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
	int clsLv=d2common_GetUnitStat(pUnit, 107, skillId); //class only
	int singleSkill=d2common_GetUnitStat(pUnit, 97, skillId); 
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
	lv+=d2common_GetUnitStat(pUnit, 188, cls*8+type); //type skill
	lv+=d2common_GetUnitStat(pUnit, 83, cls); //class skill
	lv+=d2common_GetUnitStat(pUnit, 127, 0); //all skill
	return lv;
}
int getUnitRawDistance2(UnitAny *pUnit1,UnitAny *pUnit2) {
	int x1,y1,x2,y2;
	switch (pUnit1->dwUnitType) {
		case UNITNO_PLAYER:case UNITNO_MONSTER:case UNITNO_MISSILE:
			x1=pUnit1->pMonPath->wUnitX;y1=pUnit1->pMonPath->wUnitY;break;
		case UNITNO_OBJECT:case UNITNO_ITEM:case UNITNO_AREA_ENTRANCE:
			x1=pUnit1->pItemPath->unitX;y1=pUnit1->pItemPath->unitY;break;
		default:return 1000;
	}
	switch (pUnit2->dwUnitType) {
		case UNITNO_PLAYER:case UNITNO_MONSTER:case UNITNO_MISSILE:
			x2=pUnit2->pMonPath->wUnitX;y2=pUnit2->pMonPath->wUnitY;break;
		case UNITNO_OBJECT:case UNITNO_ITEM:case UNITNO_AREA_ENTRANCE:
			x2=pUnit2->pItemPath->unitX;y2=pUnit2->pItemPath->unitY;break;
		default:return 1000;
	}
	int dx=x1-x2,dy=y1-y2;
	return dx*dx+dy*dy;
}
float getUnitYardDistance2(UnitAny *pUnit1,UnitAny *pUnit2) {
	return (float)getUnitRawDistance2(pUnit1,pUnit2)*4.0f/9;
}
float getUnitYardDistance(UnitAny *pUnit1,UnitAny *pUnit2) {
	return sqrt(getUnitYardDistance2(pUnit1,pUnit2));
}
static int *disM256=NULL,disM256_modified=0;
void saveDistanceMap() {
	if (!disM256||!disM256_modified) return;
	FILE *fp=openFile("runtime\\dis256.cache.bin","wb+");
	if (fp) {fwrite(disM256,1,128*128*sizeof(int),fp);fclose(fp);}
}
int getDistanceM256(int dx,int dy) {
	if (dx<0) dx=-dx;if (dy<0) dy=-dy;
	if (dx>dy) {int t=dy;dy=dx;dx=t;}
	if (dx==0) return dy<<8;
	if (dy>=128) return dy<<8;
	if (!disM256) {
		disM256=(int *)HeapAlloc(dllHeap,HEAP_ZERO_MEMORY,128*128*sizeof(int));
		FILE *fp=openFile("runtime\\dis256.cache.bin","rb");
		if (fp) {fread(disM256,1,128*128*sizeof(int),fp);fclose(fp);}
	}
	int *p=disM256+(dy<<7)+dx;
	if (!p[0]) {p[0]=(int)(sqrt((float)(dx*dx+dy*dy))*256);disM256_modified=1;}
	return p[0];
}
int getPlayerDistanceM256(UnitAny *pUnit) {
	int x,y;
	switch (pUnit->dwUnitType) {
		case UNITNO_PLAYER:case UNITNO_MONSTER:case UNITNO_MISSILE:
			x=pUnit->pMonPath->wUnitX;y=pUnit->pMonPath->wUnitY;break;
		case UNITNO_OBJECT:case UNITNO_ITEM:case UNITNO_AREA_ENTRANCE:
			x=pUnit->pItemPath->unitX;y=pUnit->pItemPath->unitY;break;
	}
	return getDistanceM256(x-dwPlayerX,y-dwPlayerY);
}
float getPlayerDistanceRaw(UnitAny *pUnit) {
	int x,y;
	switch (pUnit->dwUnitType) {
		case UNITNO_PLAYER:case UNITNO_MONSTER:case UNITNO_MISSILE:
			x=pUnit->pMonPath->wUnitX;y=pUnit->pMonPath->wUnitY;break;
		case UNITNO_OBJECT:case UNITNO_ITEM:case UNITNO_AREA_ENTRANCE:
			x=pUnit->pItemPath->unitX;y=pUnit->pItemPath->unitY;break;
	}
	return (float)getDistanceM256(x-dwPlayerX,y-dwPlayerY)/256.0f;
}
float getPlayerDistanceYard(UnitAny *pUnit) {return getPlayerDistanceRaw(pUnit)*2.0f/3.0f;}

RosterUnit *getRosterUnit(int id) {
	for (RosterUnit *pRU=PLAYERLIST;pRU;pRU=pRU->pNext) if (pRU->dwUnitId==id) return pRU;
	return NULL;
}
UnitAny *getAreatileByTxt(int txt) {
	for (int i=0;i<128;i++) {
		UnitAny *pUnit=d2client_pUnitTable[5*128+i];
		while (pUnit) {
			if (pUnit->dwTxtFileNo==txt) return pUnit;
			pUnit=pUnit->pHashNext;
		}
	}
	return NULL;
}
UnitAny *findObjectByTxt(int from,int to,int mode) {
	UnitAny *pMinUnit=NULL;int minDis=0;
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[2*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (from<=pUnit->dwTxtFileNo&&pUnit->dwTxtFileNo<=to&&pUnit->dwMode==mode) {
				int dis=getPlayerDistanceM256(pUnit);
				if (!pMinUnit||dis<minDis) {pMinUnit=pUnit;minDis=dis;}
			}
		}
	}
	return pMinUnit;
}
//level 0:town -1:any
UnitAny *getPortalToArea(int level,char *owner) {
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[2*128+i];pUnit;pUnit=pUnit->pHashNext) {
			//blue portal 59 red portal 60
			if ((pUnit->dwTxtFileNo==59||pUnit->dwTxtFileNo==60)&&pUnit->pObjectData) {
				int lvl=pUnit->pObjectData->nShrineNo;
				if (level!=0&&level!=-1&&lvl!=level) continue;
				if (level==0&&lvl!=1&&lvl!=40&&lvl!=75&&lvl!=103&&lvl!=109) continue;
				if (!owner||strcmp(owner,pUnit->pObjectData->owner)==0) return pUnit;
			}
		}
	}
	return NULL;
}
int getAreatileTxtToLevel(int toLevel) {
	//only appear if distance < 20
	for(AreaTile *pTile = PLAYER->pMonPath->pAreaRectData->pAreaRectInfo->pAreaTiles ;pTile ; pTile = pTile->pNext ){
		int txt=*(pTile->pNum);
		int lvl=pTile->pAreaRectInfo->pDrlgLevel->dwLevelNo;
		if (lvl==toLevel) return txt;
	}
	return 0;
}
World *getSinglePlayerWorld() {
	if (!singlePlayerWorld) {
		int worldId=d2net_getWorldId(0);
		singlePlayerWorld=d2game_getWorldByID(worldId);//Crash if called repeatly
	}
	return singlePlayerWorld;
}
int singlePlayerSaveGame(char *path) {
	if (!fIsSinglePlayer) return 0;
	char dumpbuf[8192];
	int size;
	getSinglePlayerWorld();
	if (!singlePlayerWorld) return 0;
	NetClient *client=singlePlayerWorld->client;
	int unitId=client->unitId;
	int unitType=client->unitType;
	//client->ptGame;
	UnitAny *pUnit=getSinglePlayerUnit(unitId,unitType);
	if (!pUnit) return 0;
	d2game_MakeD2S(pUnit,singlePlayerWorld,dumpbuf,&size,8192,0,1);
	FILE *fp=fopen(path,"wb+");
	if (!fp) return 0;
	fwrite(dumpbuf,size,1,fp);
	fclose(fp);
	return 0;
}
int encodeItem(UnitAny *pUnit,char *buf,int size) {
//D2Game.dll+53470 - 51                    - push ecx
		if ((pUnit->dwFlags2>>15)&1) return 0;
		return d2common_EncodeItem(pUnit,buf,size,1,1,0);
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
	BYTE packet[9];packet[0]=0x3C;
	if (!right) skill|=0x80000000;
	*(int *)&packet[1]=skill;*(int *)&packet[5]=-1;
	SendPacket(packet,sizeof(packet));
}
void switchLeftSkill(int skill) {selectSkill(0,skill);}
void switchRightSkill(int skill) {selectSkill(1,skill);}

//Only in single player mode, get the unit on the backend, which has real HP and experience.
UnitAny *getSinglePlayerUnit(int id,int unitType) {
	if (!fIsSinglePlayer) return NULL;
	if (unitType<0||unitType>=5) return NULL;
	getSinglePlayerWorld();
	if (!singlePlayerWorld) return 0;
	int *offsets=(int *)d2game_pSPUnitOffsets;
//d2game_FB014: 00001120 00001320 00001520 00001920 | ... ... ... ...|
//d2game_FB024: 00001720 00000000 535C2E2E 6372756F | .........\Sourc|
	int offset=offsets[unitType];
	offset+=(id&0x7f)*4;
	UnitAny *head=*((UnitAny **)((int)singlePlayerWorld+offset));
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
	//d2net_SendPacket(size, 0, buf);		
	d2net_SendPacket(size, 1, buf);		
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
int LeftClickType(int type,UnitAny *pUnit,int x,int y) {
	int buf[8];
	buf[0]=type;buf[1]=(int)PLAYER;buf[2]=(int)pUnit;
	if (pUnit&&x==0&&y==0) {
		switch (pUnit->dwUnitType) {
			case UNITNO_PLAYER:case UNITNO_MONSTER:
				x=pUnit->pMonPath->wUnitX;y=pUnit->pMonPath->wUnitY;break;
			case UNITNO_OBJECT:case UNITNO_ITEM:case UNITNO_AREA_ENTRANCE:
				x=pUnit->pItemPath->unitX;y=pUnit->pItemPath->unitY;break;
		}
	}
	buf[3]=x;buf[4]=y;
	buf[5]=0;buf[6]=0;buf[7]=0;
	*d2client_pHasSelectedObject=0;
	*d2client_pFlagUnknownC2F8=0;
	__asm {
		lea esi,buf
		call d2client_LeftClickBDE0
	}
}
int LeftClickPos(int x,int y) {
	return LeftClickType(0x45,NULL,x,y);
}
int LeftClickUnit(UnitAny *pUnit) {
	return LeftClickType(0x45,pUnit,0,0);
}
int ShiftLeftClickUnit(UnitAny *pUnit) {
	GameControlStart(36);forceStandStill=1;
	int r=LeftClickType(0x65,pUnit,0,0);
	forceStandStill=0;GameControlEnd(36);
	return r;
}
int ShiftLeftClickPos(int x,int y) {
	GameControlStart(36);forceStandStill=1;
	int r=LeftClickType(0x65,NULL,x,y);
	forceStandStill=0;GameControlEnd(36);
	return r;
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
		call d2client_RightClickBC40
	}
}
int RightSkillPos(int x,int y) {return RightSkill(NULL,x,y);}
int RightSkillUnit(UnitAny *pUnit) {return RightSkill(pUnit,pUnit->pMonPath->wUnitX,pUnit->pMonPath->wUnitY);}
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
		call d2client_CanUseSkillNow
	}
}
//return 1,4 if unusable
int skillUnusableAsm(UnitAny *pUnit,Skill *pSkill) {
	__asm {
		mov esi, pUnit
		mov eax, pSkill
		call d2client_SkillUnusable
	}
}
int skillUnusable(UnitAny *pUnit,Skill *pSkill) {
	int ret=skillUnusableAsm(pUnit,pSkill);if (ret) return ret;
	if (pSkill->pSkillInfo->flags4&1) return 0; //can use in town
	return fPlayerInTown;
}

/*
Game Controls process function
d2client_38C88: FF 14 8D 10 4F BA 6F  call dword ptr [ecx*4+d2client_F4F10]
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
0:d2client_3C5B0 1:d2client_3C2A0 2:d2client_3BFC0 3:d2client_3BEB0 
4:d2client_3C8C0 5:d2client_3BC10 6:d2client_3BA70 7:d2client_3B8B0 
8:d2client_3A690 9:d2client_39E20 10:d2client_39DF0 11:d2client_39DC0 
12:d2client_3B5B0 13:d2client_3B470 14:d2client_3ADB0 15:d2client_3AD70 
16:d2client_3AD30 17:d2client_3ACF0 18:d2client_3ACB0 19:d2client_3AC70 
20:d2client_3AC30 21:d2client_3ABF0 22:d2client_3B300 23:d2client_3C9A0 
24:d2client_3C960 25:d2client_3C920 26:d2client_3C8E0 27:d2client_39D60 
28:d2client_39D40 29:d2client_39D20 30:d2client_39D00 31:d2client_39CE0 
32:d2client_39CC0 33:d2client_39CA0 34:d2client_3AE40 35:d2client_39C60 
36:d2client_39C50 37:d2client_3B230 38:d2client_3A900 39:d2client_3A8E0 
40:d2client_3A8C0 41:d2client_39C30 42:0 43:d2client_5C3D0 
44:d2client_3A9C0 45:d2client_39D80 46:d2client_3ABB0 47:d2client_3AB70 
48:d2client_3AB30 49:d2client_3AAF0 50:d2client_3AAB0 51:d2client_3AA70 
52:d2client_3AA30 53:d2client_3A9F0 54:d2client_3AEE0 55:d2client_39C80 
56:d2client_3C880 
*/
void useBelt(int id,int shift) {
	if (id<0||id>3) return;
//d2client_3C9A0: A1 A4 C3 BC 6F     mov eax, [d2client_11C3A4 int d2client_notAcceptKeys]
	if (*d2client_pnotAcceptKeys) return;
	if (!PLAYER) return;
	if (PLAYER->dwMode==0x11) return;
	if (!d2client_pBeltNotEmpty[id]) return;
	if (shift) shift=0x8000;
	__asm {
		mov eax,shift;
	}
	d2client_UseBelt(PLAYER->pInventory,PLAYER,id);
}
int GameControlStart(int cmd) {
	if (60<=cmd&&cmd<=63) {useBelt(cmd-60,1);return 0;}
	if (!fInGame) return -1;
	if (cmd<0||cmd>56) return -1;
	if (cmd==36) forceStandStill=1;
	int addr=d2client_pGameControlStart[cmd*3];
	if (addr&&!IsBadCodePtr((FARPROC)addr)) ((void (*)())addr)();
	return 0;
}
int GameControlEnd(int cmd) {
	if (60<=cmd&&cmd<=63) return -1;
	if (!fInGame) return -1;
	if (cmd<0||cmd>56) return -1;
	if (cmd==36) forceStandStill=0;
	int addr=d2client_pGameControlEnd[cmd*3];
	if (addr&&!IsBadCodePtr((FARPROC)addr)) ((void (*)())addr)();
	return 0;
}
void GameControl(int cmd) {GameControlStart(cmd);GameControlEnd(cmd);}
int getGameControlKey(int cmd) {
	for (KeyboardSetting *p=d2client_pKeyboardSettingStart;p<d2client_pKeyboardSettingEnd;p++) 
		if (p->cmd==cmd&&p->key!=-1) 
			return p->key;
	return 0;
}
//6FAF33D1 - C7 05 F0C3BC6F 00000000 - mov [6FBCC3F0],00000000 { (0),0 } <----------------------- keep running
void __declspec(naked) StandStillPatch_ASM() {
	__asm {
		mov eax,d2client_pStandStill
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
		DrawCenterText(3, wszBAlertMsg3 , SCREENSIZE.x/2, SCREENSIZE.y/2+180, dwBAlertMsgColor3, 1,0);
	}
}

int GetAreaLevel() {	
	static int dwLastLevelNo = 0 ;
	static int  wAreaLevel = 0 ;
	if (dwLastLevelNo!=LEVELNO) {
		dwLastLevelNo = LEVELNO;
		LevelTxt *pLvlTxt=d2common_GetLevelTxt(LEVELNO);
		wAreaLevel = pLvlTxt->nMonLv[EXPANSION][DIFFICULTY];	
	}
	return wAreaLevel;
} 

PetUnit * GetPetByType(UnitAny *pUnit,int type) {
	for (PetUnit *pPetUnit = *d2client_pPetUnitList;pPetUnit;pPetUnit = pPetUnit->pNext) 
		if( pPetUnit->dwPetType==type && pPetUnit->dwOwnerId == pUnit->dwUnitId && pPetUnit->dwOwerType==UNITNO_PLAYER ) 
			return pPetUnit;
	return NULL;
}
UnitAny *GetUnitFromIdSafe(DWORD id, DWORD type) {
	//int lastId=-1;
	for (UnitAny *pUnit=d2client_pUnitTable[(type<<7)+(id&0x7F)];pUnit;pUnit=pUnit->pHashNext) {
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

int getPlayerClass(int id) {
	for (RosterUnit *pUnit = PLAYERLIST ; pUnit; pUnit=pUnit->pNext){
		if (pUnit->dwUnitId== id) return pUnit->dwClassId;
	}
	return -1;
}

//获得魔法属性,只查询第一层
DWORD GetUnitMagicStat(UnitAny *pUnit , DWORD dwStatNo) {
	StatList *pStatList = d2common_GetStatListWithFlag( pUnit,	0,	0x40 );
	StatEx stat ;
	if ( pStatList && d2common_GetStatFromStatList(pStatList,dwStatNo , &stat , 0x1ff) ) return stat.dwStatValue;
	return 0;
}

//抗性惩罚
int GetUnitResist( UnitAny *pUnit , DWORD dwStatNo ) {
	int resist = d2common_GetUnitStat(pUnit, dwStatNo, 0) + resistoffset[EXPANSION][DIFFICULTY];
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


DWORD GetTextHeight(wchar_t *wStr) {
	int dwWidth, dwHeight;
	d2win_GetTextAreaSize(wStr, &dwWidth, &dwHeight);
	return dwHeight;
}
DWORD GetTextWidth(wchar_t *wStr) {
	int dwWidth, dwHeight;
	d2win_GetTextAreaSize(wStr, &dwWidth, &dwHeight);
	return dwWidth;
}
void DrawD2Text(DWORD dwFont, wchar_t *wStr, int xpos, int ypos, DWORD dwColor,DWORD dwAlign) {
	DWORD dwOldFone = d2win_SetTextFont(dwFont);
	d2win_DrawText(wStr, xpos, ypos, dwColor, dwAlign);
	d2win_SetTextFont(dwOldFone);
}
void DrawCenterText(DWORD dwFont, wchar_t *wStr, int xpos, int ypos, DWORD dwColor, int div , DWORD dwAlign) {
	DWORD dwOldFone = d2win_SetTextFont(dwFont);
	d2win_DrawText(wStr, xpos-(GetTextWidth(wStr)>>div), ypos, dwColor, dwAlign);//dwAlign:多行时对齐有用 1居中 0 靠左
	d2win_SetTextFont(dwOldFone);
}
int drawBgText(wchar_t *s,int x,int y,int color,int bgColor) {
	if (x<0||x>SCREENWIDTH||y<0||y>SCREENHEIGHT) return x;
	int w,h;d2win_GetTextAreaSize(s,&w,&h);
	d2gfx_DrawRectangle(x,y-12,x+w,y,bgColor,5);d2win_DrawText(s,x,y,color,0);
	return w;
}
int drawBgTextMiddle(wchar_t *s,int x,int y,int color,int bgColor) {
	int w,h;d2win_GetTextAreaSize(s,&w,&h);
	d2gfx_DrawRectangle(x-(w>>1),y-12,x+(w>>1),y,bgColor,5);d2win_DrawText(s,x-(w>>1),y,color,0);
	return w;
}
int drawBgTextLeft(wchar_t *s,int x,int y,int color,int bgColor) {
	int w,h;d2win_GetTextAreaSize(s,&w,&h);
	d2gfx_DrawRectangle(x-w,y-12,x,y,bgColor,5);d2win_DrawText(s,x-w,y,color,0);
	return w;
}
extern int fSkipPainting;
void ExitGame() {
	fSkipPainting=0;
	*d2client_pExitAppFlag = 0;
	SendMessage(d2gfx_GetHwnd(), WM_CLOSE, 0, 0);
}
int GetGameLanguage(){
	wchar_t wszTemp[512];
	wcscpy(wszTemp,d2lang_GetLocaleText(0xFDA));
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
	int fcr = d2common_GetUnitStat(pUnit, STAT_FCR, 0);
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

BOOL IsFullWindow() {
  WINDOWINFO wi;
  HWND hwnd = d2gfx_GetHwnd();
  if (hwnd && GetWindowInfo(hwnd, &wi)) {
    if ( (wi.dwStyle & WS_CAPTION) && (wi.dwExStyle& WS_EX_WINDOWEDGE) ) {
      return FALSE;
    }
  } else {
    LPSTR pszCmdLine = GetCommandLine();
    LPSTR p;
    for ( p = pszCmdLine; (p=strchr(p, ' ')) != NULL; p++) {
      if ((p[1] == '-') &&
        (p[2] == 'w' || p[2] == 'W') &&
        (p[3] == ' ' || p[3] == '\0')) {
        return FALSE;
      }
    }
  }
  return TRUE;
}

struct Pet {int id,owner;};
static Pet *pets=NULL;
int nPet=0,petCap=0;
int dwPetListChangeCount=1;
int dwPetListChangeVerify=0;
struct UnitExtra {int changeCount,owner;};
static int comparePet(const void *a,const void *b) {return ((Pet *)a)->id-((Pet *)b)->id;}
int getUnitOwnerId(UnitAny *pUnit) {
	static int refreshMs=0;
	if (!pUnit) return -1;
	if (dwCurMs>refreshMs) {
		refreshMs=dwCurMs+10000;
		dwPetListChangeCount++;
	}
	UnitExtra *extra=(UnitExtra *)(((char *)pUnit)+0xF4);
	if (extra->changeCount==dwPetListChangeCount) return extra->owner;
	if (dwPetListChangeVerify!=dwPetListChangeCount) {
		if (!pets) {petCap=64;pets=(Pet *)HeapAlloc(dllHeap,0,petCap*sizeof(Pet));}
		nPet=0;
		for (PetUnit *pPetUnit=*d2client_pPetUnitList;pPetUnit;pPetUnit=pPetUnit->pNext) {
			if (nPet>=petCap) {petCap*=2;pets=(Pet *)HeapReAlloc(dllHeap,0,pets,petCap*sizeof(Pet));}
			Pet *p=&pets[nPet++];
			p->id=pPetUnit->dwUnitId;
			p->owner=pPetUnit->dwOwnerId;
		}
		qsort(pets,nPet,sizeof(Pet),comparePet);
		dwPetListChangeVerify=dwPetListChangeCount;
	}
	Pet pet;pet.id=pUnit->dwUnitId;
	int owner=-1;
	Pet *p=(Pet *)bsearch(&pet,pets,nPet,sizeof(Pet),comparePet);
	if (p) owner=p->owner;
	extra->changeCount=dwPetListChangeCount;
	extra->owner=owner;
	return owner;
}
struct Pvp {int id,pvp;};
static Pvp pvps[16];
static int nPvp=0;
int fPartyListValid=0;
static int comparePvp(const void *a,const void *b) {return ((Pvp *)a)->id-((Pvp *)b)->id;}
// 0 Hostile敌对  1 Neutral 中立 2 Party 队友 3 Self 自身
int testPvpFlag(int dwUnitId) {
	static int refreshMs=0;
	if (dwUnitId==dwPlayerId) return 3;
	if (dwCurMs>refreshMs) {
		refreshMs=dwCurMs+10000;
		fPartyListValid=0;
	}
	if (!fPartyListValid) {
		nPvp=0;
		for (RosterUnit *pUnit=PLAYERLIST;pUnit;pUnit=pUnit->pNext) {
			if (pUnit->dwUnitId==dwPlayerId) continue;
			Pvp *p=&pvps[nPvp++];
			p->id=pUnit->dwUnitId;
			p->pvp=1;
			if (pUnit->wPartyId!=(WORD)-1&&pUnit->wPartyId==dwPlayerPartyId) p->pvp=2;
			if (pUnit->pPvPInfo) {
				for (PvPInfo *pPvPInfo=*(pUnit->pPvPInfo);pPvPInfo;pPvPInfo=pPvPInfo->pNext) {
					if (pPvPInfo->dwUnitId==dwPlayerId) {
						if (pPvPInfo->dwFlag&8) p->pvp=0;
						break;	
					}
				}
			}
			if (nPvp>=16) break;
		}
		qsort(pvps,nPvp,sizeof(Pvp),comparePvp);
		fPartyListValid=1;
	}
	if (nPvp<=4) {
		Pvp *p=&pvps[0];
		for (int i=0;i<nPvp;i++,p++) if (p->id==dwUnitId) return p->pvp;
	}
	Pvp pvp;pvp.id=dwUnitId;
	Pvp *p=(Pvp *)bsearch(&pvp,pvps,nPvp,sizeof(Pvp),comparePvp);
	if (!p) return 1;
	return p->pvp;
}
int sameParty(int dwUnitId) {return testPvpFlag(dwUnitId)>=2;}
bool isInRect(AreaRectData* pData, int ux, int uy) {
	int x=pData->unitX;int y=pData->unitY;int w=pData->unitW;int h=pData->unitH;
	return x<=ux&&ux<=x+w&&y<=uy&&uy<=y+h;
}
AreaRectData *testPosition(AreaRectData *pData,int x,int y) {
	__asm {
		push edi
		push ebx
		mov edi,y
		mov eax,pData
		mov ebx,x
		call d2game_TestPosition
		pop ebx
		pop edi
	}
}
extern int actlvls[6];//{1, 40, 75, 103, 109, 137};
int portalToLevel(World* game, UnitAny* pUnit, int levelId) {
	if (levelId<Level_RogueEncampment||levelId>Level_Tristram6boss) return 0;
	AreaRectData *pData=d2common_getRectData(pUnit);
	int curArea=d2common_GetLevelIdFromRectData(pData);
	if (levelId==Level_TheSecretCowLevel) {
		if (curArea!=Level_RogueEncampment) return 0;
	} else if (levelId>=Level_FurnaceofPain) {
		if (curArea!=Level_Harrogath) return 0;
	} else {
		if (levelId<actlvls[ACTNO]||levelId>=actlvls[ACTNO+1]) {
			sendMessageToClient(pUnit->pPlayerData->client,4,0,"<hackmap>",
				"Can't open portal to a different act");
			return 0;
		}
	}
	//LOG("Open portal to level %d\n", levelId);
	POINT pos;d2common_getUnitPos(pUnit,&pos);
	if (!d2common_GetDropPos(pData, &pos, &pos, 3, 0x3E01, 0xC01, 0)) return 0;
	pData = testPosition(pData, pos.x, pos.y);if (!pData) return 0;
	UnitAny* portal=d2game_CreateUnit(UNITNO_OBJECT,60,pos.x,pos.y,game,pData,1,1,0);
	portal->pObjectData->nShrineNo = (BYTE)levelId;
	d2common_setUnitMode(portal, 1);
	d2game_LinkPortal(game, portal, levelId, Level_Harrogath);
	d2common_setObjectFlags(portal, d2common_getObjectFlags(portal) | 3);
	d2common_Common11084(pData, 0);
	d2common_Common11084(d2common_getRectData(portal), 0);
	return 1;
}
int hasPortalInRect(AreaRectData* pData, int dstLvl) {
	for (UnitAny* pUnit = pData->pUnit; pUnit; pUnit = pUnit->pRectNext) {
		if (pUnit->dwUnitType!=UNITNO_OBJECT) continue;
		if ((pUnit->dwTxtFileNo==59||pUnit->dwTxtFileNo==60)&&pUnit->pObjectData) {
			int lvl=pUnit->pObjectData->nShrineNo;
			if (lvl==dstLvl) return 1;
		}
	}
	return 0;
}
int hasPortalNearby(AreaRectData* pData, int dstLvl) {
	if (hasPortalInRect(pData,dstLvl)) return 1;
	for(int i = 0; i < pData->nearbyRectCount; i++) {
		AreaRectData* pData2 = pData->paDataNear[i];
		if (hasPortalInRect(pData2,dstLvl)) return 1;
	}
	return 0;
}
int hasPortalInLevel(DrlgLevel* level, int dstLvl) {
	for (AreaRectInfo* pInfo = level->pAreaRectInfo; pInfo; pInfo = pInfo->pNext) {
		AreaRectData *pData=pInfo->pAreaRectData;if (!pData) continue;
		if (hasPortalInRect(pData,dstLvl)) return 1;
	}
	return 0;
}
int unitHasItem(UnitAny *pUnit,int index) {
	for (UnitAny *pItem=d2common_GetFirstItemInInv(PLAYER->pInventory);pItem;pItem=d2common_GetNextItemInInv(pItem)) {
		if (pItem->dwUnitType!=UNITNO_ITEM) continue;
		int idx=GetItemIndex(pItem->dwTxtFileNo)+1;
		if (idx==index) return 1;
	}
	return 0;
}
World __declspec(naked) * getWorldByClientId(int clientId) {
	__asm {
		mov esi,clientId
		call d2game_getWorld
		ret
	}
}
NetClient __declspec(naked) *getNetClient(World *world,int clientId) {
	__asm {
		mov eax,world
		mov ecx,clientId
		call d2game_getNetClient
		ret
	}
}
void sendPacketToClient(NetClient *client,char *packet,int packetLen) {
	__asm {
		push packetLen
		mov eax, packet
		push eax
		mov eax, client
		call d2game_sendPacket
	}
}
void __cdecl sendChatMessageToServer(char *fmt,...) { //max 213 bytes
	va_list va;
	char pkg[256];
	pkg[0]=0x15;pkg[1]=1;pkg[2]=0;
	va_start(va, fmt);
	//int len=strlen(msg);
	//memcpy(pkg+3,msg,len);
	int len=_vsnprintf(pkg+3,213,fmt,va);
	if (len>213) len=213;
	pkg[3+len]=0;pkg[3+len+1]=0;pkg[3+len+2]=0;
	d2net_SendPacket(len+6,1,(BYTE *)pkg);		
}
void sendMessageToClient(NetClient *client,int type,int color,char *name,char *msg) {
	char packet[256];*(int *)packet=0;*(int *)(packet+4)=0;*(int *)(packet+8)=0;
	packet[0]=0x26;packet[1]=type;packet[3]=2;packet[8]=color;packet[9]=0;
	char *s=packet+10;int len=strlen(name);memcpy(s,name,len+1);s+=len+1;
	char *end=packet+255;len=strlen(msg);
	if (s+len>end) len=end-s;memcpy(s,msg,len);s+=len;*s++=0;
	int packetLen=s-packet;
	sendPacketToClient(client,packet,packetLen);
	/*
	__asm {
		push packetLen
		lea eax, packet
		push eax
		mov eax, client
		call d2game_sendPacket
	}
	*/
}
CharStatsTxt* getCharStatsTxt(int cls) {
	__asm {
		mov eax,cls
		call d2common_GetCharStatsTxt
	}
}
int getStatMaxValue(int id) {
	ItemStatCostTxt *txt=d2common_GetItemStatCostTxt(id);
	if (!txt) return 1;
	int maxvalue=(1<<txt->save_bits)-1-txt->save_add;
	if (txt->valshift) maxvalue<<=txt->valshift;
	return maxvalue;
}
//copy last line of s into dst
int cpLocaleName(wchar_t *dst,wchar_t *s,int max) {
	wchar_t *src=s;
	while (*s) {
		wchar_t c=*s++;
		if (c=='\n'&&*s) src=s;
	}
	int n=0;
	while (max>0) {
		wchar_t c=*src++;*dst++=c;if (!c) break;n++;max--;
	}
	return n;
}
int acpLocaleName(char *dst,wchar_t *s,int bufsize) {
	wchar_t *src=s;int n=0;
	while (*s) {
		wchar_t c=*s++;if (c) n++;
		if (c=='\n'&&*s) {src=s;n=0;}
	}
	n=WideCharToMultiByte(CP_ACP,0,src,n,dst,bufsize,NULL,NULL);
	if (n>=bufsize) n=bufsize-1;
	if (n>=0) dst[n]=0;
	return n;
}
FILE *mkbmp(char *outpath,int w,int h) {
	BITMAPFILEHEADER bf={0};
	BITMAPINFOHEADER bi={0};
	bf.bfType=0x4d42;
	bf.bfOffBits=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+1024;
	bf.bfSize=bf.bfOffBits+w*h;
	bi.biSize=sizeof(BITMAPINFOHEADER);bi.biWidth=w;bi.biHeight=h;bi.biPlanes=1;bi.biBitCount=8;
	bi.biSizeImage=w*h;bi.biXPelsPerMeter=bi.biYPelsPerMeter=0;
	FILE *fpout=fopen(outpath,"wb+");if (!fpout) return NULL;
	fwrite(&bf,1,sizeof(BITMAPFILEHEADER),fpout);
	fwrite(&bi,1,sizeof(BITMAPINFOHEADER),fpout);
	fwrite(d2gdi_pColorMap,1,1024,fpout);
	return fpout;
}
void dc6cell2bmp(GfxCell *pcell,char *outpath) {
	int x0=pcell->dwXoffs,y0=pcell->dwYoffs;
	int w=pcell->dwWidth,h=pcell->dwHeight;
	LOG("(%d,%d) %d*%d %d %s\n",x0,y0,w,h,pcell->dwLength,outpath);
	if (w&3) w=(w&~3)+4;
	char *buf=(char *)malloc(w*h);
	unsigned char *src=pcell->nCols;
	char *dst=buf,*dst_end=buf+w;
	for (int i=0;i<h;i++) {
		char *rowstart=dst;
		memset(dst,0,w);
		while (1) {
			int n=*src++;
			if (n==0x80) break;
			if (dst>=dst_end) {LOG("overflow %d/%d %x\n",dst-rowstart,w,*src);free(buf);return;}
			if (n&0x80) dst+=n&0x7f;
			else {memcpy(dst,src,n);src+=n;dst+=n;}
		}
		dst=rowstart+w;dst_end=dst+w;
	}
	FILE *fpout=mkbmp(outpath,w,h);
	if (fpout) {
		fwrite(buf,1,w*h,fpout);
		fclose(fpout);
	}
	free(buf);
}
int getSimpleItemStackContent(UnitAny *pUnit,int *ptxt) {
	if (!pUnit||!ptxt) return 0;
	*ptxt=0;
	StatList *plist=pUnit->pStatList;
	if (!(plist->dwListFlag&0x80000000)) return 0;
	if (!plist->sFullStat.pStats) return 0;
	Stat *stat=&plist->sFullStat;
	int n=stat->wStats;
	if (n>=511) return 0;
	StatEx *se=stat->pStats;
	for (int i=0;i<n;i++) {
		int id=se[i].wStatId;if (id!=simpleItemStackStatId) continue;
		*ptxt=se[i].wParam&0xFFFF;
		return se[i].dwStatValue;
	}
	return 0;
}
