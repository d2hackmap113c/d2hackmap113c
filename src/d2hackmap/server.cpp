﻿#include "stdafx.h"
#include "header.h"
#include "d2BinFile.h"

void modifyStashGrid();

enum {UNASSIGN_STR=1,UNASSIGN_DEX,UNASSIGN_VIT,UNASSIGN_ENE,};
extern int dwAddStatMaxCount,hasPlugY;
static int dwBatchUndoStatPoints=5;
static int dwBatchUndoSkillPoints=5;
int RepeatAdminCmd();
int DoServerTest();
static void serverRecvTest(World *world, UnitAny* pUnit,int param);
ToggleVar tServerTest={TOGGLEVAR_DOWN,0,-1,0,"ServerTest",&DoServerTest};
ToggleVar tRepeatAdminCmd={TOGGLEVAR_DOWN,0,-1,0,"RepeatAdminCmd",&RepeatAdminCmd};
ToggleVar tEnable2025Patch={TOGGLEVAR_ONOFF,0,-1,1,"Enable2025Patch"};
static int fServerEnableSlainCowKing=1,fServerEnableNihlathakPortal=1;
static int fServerUndoStat=1,fServerUndoSkill=1,fServerUndoQuest=1;
static int fServerInfiniteSocket=1,fServerInfiniteImbue=1,fServerInfiniteIdentify=1,fServerInfinitePortal=1;
static int fServerInfiniteArrow=1,fServerInfiniteMana=0,fServerInfiniteLife=0,fServerInstantKill=0;
int fEnableAdminCmd=1;
static int fServerInfiniteDurability=1;
static int fServerNoMonsterDrop=0;
char adminCmdPrefix[32]=",";
static char undoPrefix[32]=",,,undo";
static int maxPlayerResist=127,maxMonsterResist=0;
static int addPlayerStat[12],addPlayerRep;
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_INT_ARRAY0, "ServerAddPlayerStat",&addPlayerStat,1,{12}},
	{CONFIG_VAR_TYPE_INT,"ServerPlayerREP",&addPlayerRep,4},
	{CONFIG_VAR_TYPE_INT,"MaxPlayerResist",&maxPlayerResist,4},
	{CONFIG_VAR_TYPE_INT,"MaxMonsterResist",&maxMonsterResist,4},
	{CONFIG_VAR_TYPE_STR,"AdminCmdPrefix",adminCmdPrefix,1,{31}},
	{CONFIG_VAR_TYPE_STR,"UndoPrefix",undoPrefix,1,{31}},
	{CONFIG_VAR_TYPE_INT,"ServerEnableSlainCowKing",  &fServerEnableSlainCowKing,4},
	{CONFIG_VAR_TYPE_INT,"ServerEnableNihlathakPortal",  &fServerEnableNihlathakPortal,4},
	{CONFIG_VAR_TYPE_INT,"ServerUndoStat",  &fServerUndoStat,4},
	{CONFIG_VAR_TYPE_INT,"ServerUndoSkill",  &fServerUndoSkill,4},
	{CONFIG_VAR_TYPE_INT,"ServerUndoQuest",  &fServerUndoQuest,4},
	{CONFIG_VAR_TYPE_INT,"ServerInfiniteSocket",  &fServerInfiniteSocket,4},
	{CONFIG_VAR_TYPE_INT,"ServerInfiniteImbue",  &fServerInfiniteImbue,4},
	{CONFIG_VAR_TYPE_INT,"ServerInfiniteIdentify",  &fServerInfiniteIdentify,4},
	{CONFIG_VAR_TYPE_INT,"ServerInfinitePortal",  &fServerInfinitePortal,4},
	{CONFIG_VAR_TYPE_INT,"ServerInfiniteArrow",  &fServerInfiniteArrow,4},
	{CONFIG_VAR_TYPE_INT,"ServerInfiniteMana",  &fServerInfiniteMana,4},
	{CONFIG_VAR_TYPE_INT,"ServerInfiniteLife",  &fServerInfiniteLife,4},
	{CONFIG_VAR_TYPE_INT,"ServerInstantKill",  &fServerInstantKill,4},
	{CONFIG_VAR_TYPE_INT,"EnableAdminCmd",  &fEnableAdminCmd,4},
	{CONFIG_VAR_TYPE_INT,"ServerInfiniteDurability",  &fServerInfiniteDurability,4},
	{CONFIG_VAR_TYPE_INT,"ServerNoMonsterDrop",  &fServerNoMonsterDrop,4},
	{CONFIG_VAR_TYPE_INT,"BatchUndoStatPoints",  &dwBatchUndoStatPoints,4},
	{CONFIG_VAR_TYPE_INT,"BatchUndoSkillPoints",  &dwBatchUndoSkillPoints,4},
	{CONFIG_VAR_TYPE_KEY,"ServerTestKey",&tServerTest},
	{CONFIG_VAR_TYPE_KEY,"RepeatAdminCmdKey",&tRepeatAdminCmd},
	{CONFIG_VAR_TYPE_KEY,"Enable2025Patch",&tEnable2025Patch},
};
void server_initConfigVars() {
	memset(addPlayerStat,0,sizeof(addPlayerStat));
	addPlayerRep=0;
	maxMonsterResist=0;
}
void server_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
/*
server recv packet:
d2game_CC658: push ecx(arg1:len)
d2game_CC659: push ebx(packet)     ; esp-12
d2game_CC65A: mov edx, esi(UnitAny pUnit) ; esp-16
d2game_CC65C: mov ecx, ebp(arg0:World world)
d2game_CC65E: FF 14 FD 78 A0 B4 00  call dword ptr [edi*8+d2game_FA078] <------------- edi=packet[0]
stacktrace:
	d2game_CC550
	d2game_CAE80
	d2game_2E460 d2game_processGame
	d2client_44B50
	d2client_3CB00 d2client_processMessages
return 3 if size error
00:0 01:d2game_CE0D0 02:d2game_CE090 03:d2game_CE040 
04:d2game_CE000 05:d2game_CE5A0 06:d2game_CE9D0 07:d2game_CE950 
08:d2game_CE8F0 09:d2game_CEB70 0A:d2game_CEB10 0B:d2game_CB5B0 
0C:d2game_CE520 0D:d2game_CE870 0E:d2game_CE7F0 0F:d2game_CE790 
10:d2game_CEAB0 11:d2game_CEA50 12:d2game_CB590 13:d2game_D0910 
14:d2game_D00A0 15:d2game_CDBB0 16:d2game_D08B0 17:d2game_CF380 
18:d2game_CF0E0 19:d2game_D1370 1A:d2game_CEE60 1B:d2game_CF2F0 
1C:d2game_D12E0 1D:d2game_D1200 1E:d2game_D1140 1F:d2game_D1010 
20:d2game_CF730 21:d2game_CE4B0 22:d2game_CD4F0 23:d2game_CEC70 
24:d2game_D0F80 25:d2game_CEDE0 26:d2game_CF5B0 27:d2game_CF6B0 
28:d2game_CEBD0 29:d2game_CDB30 2A:d2game_CF270 2B:0 
2C:d2game_CB1E0 2D:d2game_CBDA0 2E:d2game_CB1D0 2F:d2game_CE440 
30:d2game_CD480 31:d2game_CC9D0 32:d2game_CF220 33:d2game_CF570 
34:d2game_CF070 35:d2game_CF0A0 36:d2game_D0060 37:d2game_CEFE0 
38:d2game_CFB30 39:d2game_CB560 3A:d2game_CC960 3B:d2game_CDA20 
3C:d2game_CF480 3D:d2game_CD440 3E:d2game_CD8D0 3F:d2game_CBD50 
40:d2game_CBD30 41:d2game_D14B0 42:d2game_CB1C0 43:d2game_CB1B0 
44:d2game_D0E90 45:d2game_CB550 46:d2game_CD3D0 47:d2game_CD360 
48:d2game_CB4E0 49:d2game_D0D30 4A:0 4B:d2game_CC6E0 
4C:d2game_CF400 4D:d2game_CB4A0 4E:0 4F:d2game_D1470 
50:d2game_CEF40 51:d2game_CBC70 52:d2game_CB490 53:d2game_CB450 
54:d2game_CB420 55:0 56:0 57:0 
58:d2game_CB3D0 59:d2game_CFF30 5A:0 5B:0 
5C:0 5D:d2game_CC8E0 5E:d2game_CC860 5F:d2game_D0790 
60:d2game_CF510 61:d2game_D0BC0 62:d2game_CFB90 63:d2game_D0950 
64:0 65:0 66:d2game_CB190 67:0 
68:d2game_F0EE0 69:d2game_F1130 6A:d2game_F0870 6B:d2game_F0D50 
6C:BF 6D:1 6E:2 6F:FFFFFFFF 
70:0 71:d2game_782F0 72:d2game_78710 73:d2game_78710 
74:d2game_78150 75:d2game_782F0 76:d2game_78710 77:d2game_78840 
78:d2game_78840 79:d2game_781C0 7A:d2game_78840 7B:d2game_78840 
7C:d2game_78840 7D:d2game_78840 7E:d2game_78840 7F:d2game_78840 
80:d2game_78840 81:d2game_79730 82:d2game_78840 83:d2game_78710 
84:d2game_76270 85:d2game_76040 86:d2game_74E60 87:0 
88:d2game_74C60 89:0 8A:d2game_75210 8B:d2game_76BF0 
8C:d2game_75510 8D:d2game_76BF0 8E:d2game_75510 8F:d2game_76BF0 
90:d2game_74C10 91:d2game_76BF0 92:d2game_75510 93:d2game_76BF0 
94:d2game_75510 95:d2game_76BF0 96:d2game_75510 97:d2game_76BF0 
98:d2game_74C90 99:d2game_74890 9A:d2game_76700 9B:0 
9C:d2game_75460 9D:0 9E:d2game_75160 9F:d2game_765E0 
A0:d2game_754E0 A1:d2game_76BF0 A2:d2game_74C30 A3:0 
A4:d2game_75510 A5:d2game_76BF0 A6:d2game_75510 A7:d2game_76BF0 
A8:d2game_75510 A9:d2game_76B20 AA:d2game_75510 AB:d2game_76A50 
AC:d2game_75510 AD:d2game_76BF0 AE:0 AF:d2game_70180 
B0:0 B1:0 B2:FFFFFFFF B3:d2game_698A0 
B4:0 B5:FFFFFFFF B6:d2game_69800 B7:0 
B8:FFFFFFFF B9:d2game_6ABB0 BA:80 BB:FFFFFFFF 
BC:d2game_6ABB0 BD:83 BE:2B BF:d2game_6ABB0 
C0:82 C1:2D C2:d2game_6AB60 C3:86 
C4:1B C5:d2game_6AC30 C6:88 C7:55 
C8:d2game_69770 C9:0 CA:FFFFFFFF CB:d2game_6B430 
CC:0 CD:FFFFFFFF CE:d2game_6B8A0 CF:0 
D0:FFFFFFFF D1:d2game_6C200 D2:0 D3:FFFFFFFF 
D4:0 D5:0 D6:8 D7:0 
D8:0 D9:7 DA:1 DB:1 
DC:1 DD:0 DE:0 DF:6 
E0:1 E1:0 E2:B E3:1 
E4:0 E5:11 E6:1 E7:0 
E8:12 E9:1 EA:0 EB:4 
EC:1 ED:0 EE:C EF:1 
F0:0 F1:E F2:1 F3:0 
F4:1A F5:1 F6:0 F7:1C 
F8:0 F9:0 FA:9 FB:0 
FC:1 FD:14 FE:1 FF:0 
*/
int client_unassign_stat(int type,int batch) {
	if (!*d2client_pUiCharacterOn) return 0;
	int count=batch?dwBatchUndoStatPoints:1;
	switch (type) {
		case UNASSIGN_STR:
			sendChatMessageToServer("%sstr:%d,%d",undoPrefix,0,count);break;
		case UNASSIGN_DEX:
			sendChatMessageToServer("%sdex:%d,%d",undoPrefix,0,count);break;
		case UNASSIGN_VIT:
			sendChatMessageToServer("%svit:%d,%d",undoPrefix,0,count);break;
		case UNASSIGN_ENE:
			sendChatMessageToServer("%sene:%d,%d",undoPrefix,0,count);break;
		default:return 0;
	}
	return 1;
}
int client_mouse_stat(int mx,int my) {
	if (!*d2client_pUiCharacterOn) return 0;
	mx-=(SCREENSIZE.x-800)>>1;
	my-=(SCREENSIZE.y-600)>>1;
	if (mx<205||mx>235) return 0;
	if (140<=my&&my<170) return UNASSIGN_STR;
	if (200<=my&&my<230) return UNASSIGN_DEX;
	if (286<=my&&my<316) return UNASSIGN_VIT;
	if (350<=my&&my<380) return UNASSIGN_ENE;
	return 0;
}
void *getSkillDescTxt(int skillId) {
	__asm {
		mov eax,skillId
		call d2client_getSkillDescTxt
	}
}
int getSkillTreeButtonY(int row) {
	__asm {
		mov eax,row
		call d2client_getSkillTreeButtonY
	}
}
int getSkillTreeButtonX(int col) {
	__asm {
		mov eax,col
		call d2client_getSkillTreeButtonX
	}
}
struct SkillDescTXT {char off00,off01,cls,row,col;};
int client_mouse_skillBtn(int mx,int my) {
	if (!*d2client_pUiSkillTreeOn) return 0;
	int skillCount=d2common_getPlayerSkillCount(PLAYER->dwTxtFileNo);
	//LOG("skills=%d %d %d\n",skillCount,mx,my);
	for (int i=0;i<skillCount;i++) {
		int skillId=d2common_getSkillId(PLAYER->dwTxtFileNo,i);
		SkillDescTXT *skillDesc=(SkillDescTXT *)getSkillDescTxt(skillId);
		if (!skillDesc) continue;
		int cls=skillDesc->cls;//*(char *)((int)skillDesc+2);
		if (cls!=*d2client_pCurSkillClass) continue;
		int row=skillDesc->row;//*(char *)((int)skillDesc+3);
		int col=skillDesc->col;//*(char *)((int)skillDesc+4);
		int y=getSkillTreeButtonY(row);
		int x=getSkillTreeButtonX(col);
		//LOG("skill=%d cls=%d (%d,%d) (%d,%d)\n",skillId,cls,row,col,x,y);
		if (x<=mx&&mx<x+50&&y-50<=my&&my<y) return skillId;
	}
	return 0;
}
int client_unassign_skill(int skillId,int batch) {
	Skill *skill=NULL;
	for (Skill *p=PLAYER->pSkill->pFirstSkill;p;p=p->pNextSkill) {
		if ((int)p->dwItemId!=-1) continue;
		int id=p->pSkillInfo->wSkillId;
		if (id==skillId) {skill=p;break;}
	}
	if (!skill) return 0;
	int curLvl = d2common_GetSkillLevel(PLAYER,skill, 0);
	if (curLvl<=0) return 0;
	int count=batch?dwBatchUndoSkillPoints:1;
	if (count>31) count=31;
	sendChatMessageToServer("%sskill:%d,%d",undoPrefix,skillId,count);
	return 1;
}
int client_reset_quest(int questId) {
	if (!*d2client_pUiQuestOn) return 0;
	sendChatMessageToServer("%squest:%d",undoPrefix,questId);
	return 0;
}
void UndoStr(UnitAny* pUnit, int count) {
	CharStatsTxt* charStats = getCharStatsTxt(pUnit->dwTxtFileNo);if (!charStats) return;
	int cur=d2common_GetUnitBaseStat(pUnit,STAT_STRENGTH,0);
	//LOG("str=%d base=%d\n",cur,charStats->baseSTR);
	if (cur<=charStats->baseSTR) return;
	int remove=cur-charStats->baseSTR;if (remove>count) remove=count;
	if (cur-remove<1) remove=cur-1;
	d2common_AddPlayerStat(pUnit,STAT_STRENGTH,-remove,0);
	d2common_AddPlayerStat(pUnit,STAT_STATPOINTSLEFT,remove,0);
}
void UndoDex(UnitAny* pUnit, int count) {
	CharStatsTxt* charStats = getCharStatsTxt(pUnit->dwTxtFileNo);if (!charStats) return;
	int cur=d2common_GetUnitBaseStat(pUnit,STAT_DEXTERITY,0);
	//LOG("str=%d base=%d\n",cur,charStats->baseDEX);
	if (cur<=charStats->baseSTR) return;
	int remove=cur-charStats->baseSTR;if (remove>count) remove=count;
	if (cur-remove<1) remove=cur-1;
	d2common_AddPlayerStat(pUnit,STAT_DEXTERITY,-remove,0);
	d2common_AddPlayerStat(pUnit,STAT_STATPOINTSLEFT,remove,0);
}
void UndoVit(UnitAny* pUnit, int count) {
	CharStatsTxt* charStats = getCharStatsTxt(pUnit->dwTxtFileNo);if (!charStats) return;
	int cur = d2common_GetUnitBaseStat( pUnit, STAT_VITALITY, 0 );
	if (cur <= charStats->baseVIT) return;
	int statPoint = cur - charStats->baseVIT >= count ? count : cur - charStats->baseVIT;
	if (cur - statPoint < 1) statPoint = cur - 1;
	int rmHP = statPoint * (charStats->lifePerVitality << 6);
	int rmStamina = statPoint * (charStats->staminaPerVitality << 6);
	d2common_AddPlayerStat( pUnit, STAT_VITALITY,	-statPoint ,0 );
	d2common_AddPlayerStat( pUnit, STAT_MAXHP,		-rmHP ,0 );
	d2common_AddPlayerStat( pUnit, STAT_MAXSTAMINA,	-rmStamina ,0 );
	d2common_AddPlayerStat( pUnit, STAT_STATPOINTSLEFT,		statPoint ,0 );
}
void UndoEne(UnitAny* pUnit, int count) {
	CharStatsTxt* charStats = getCharStatsTxt(pUnit->dwTxtFileNo);
	int cur = d2common_GetUnitBaseStat( pUnit, STAT_ENERGY, 0);
	if (cur <= charStats->baseENE) return;
	int statPoint = cur - charStats->baseENE >= count ? count : cur - charStats->baseENE;
	if (cur - statPoint < 1) statPoint = cur - 1;
	int rmMana = statPoint * (charStats->manaPerMagic << 6);
	d2common_AddPlayerStat( pUnit, STAT_ENERGY,		-statPoint ,0 );
	d2common_AddPlayerStat( pUnit, STAT_MAXMANA,		-rmMana ,0 );
	d2common_AddPlayerStat( pUnit, STAT_STATPOINTSLEFT,		statPoint ,0 );
}
void *getClient(UnitAny *pUnit) {
	if (pUnit->dwUnitType==0) {
		void *playerData=d2common_GetPlayerData(pUnit);
		if (playerData) return (void *)*(int *)((int)playerData+0x9C);
	}
	return NULL;
}
static void SetSkillBaseLevelOnClient(void *ptClient,UnitAny *pUnit,int skillId,int skillLvl,int bRemove) {
	__asm {
		mov eax,bRemove
		push eax
		mov eax,skillLvl
		push eax
		mov eax,ptClient
		mov esi,pUnit
		mov ebx,skillId
		call d2game_SetSkillBaseLevelOnClient
	}
}
void UndoSkill(UnitAny *pUnit,int skillId,int count) {
	Skill *skill=NULL;
	for (Skill *p=pUnit->pSkill->pFirstSkill;p;p=p->pNextSkill) {
		if ((int)p->dwItemId!=-1) continue;
		int id=p->pSkillInfo->wSkillId;
		if (id==skillId) {skill=p;break;}
	}
	if (!skill) return;
	SkillsBIN *txt=(SkillsBIN *)skill->pSkillInfo;
	if (txt->charclass!=pUnit->dwTxtFileNo) {
		LOG("Can't unassign skill, class mismatch\n");
		return;
	}
	int skpoints=txt->skpoints;
	int curLvl = d2common_GetSkillLevel(pUnit, skill, 0);
	if (curLvl<=0) return;
	int newLvl=curLvl-count;
	if (newLvl<0) newLvl=0;
	int addSkillPoint=curLvl-newLvl;
	//LOG("skill=%d lvl=%d->%d add=%d\n",skillId,curLvl,newLvl,addSkillPoint);
	d2common_AddPlayerStat(pUnit, STAT_NEWSKILLS, addSkillPoint, 0 );
	if (1||skpoints==-1) {
		d2common_SetSkillBaseLevel(pUnit, skillId,newLvl, 0, __FILE__, __LINE__);
		addSkillPoint+=newLvl;
	} else {
		for (int i=0;i<count;i++) {
			curLvl--;if (curLvl<0) break;
			d2common_SetSkillBaseLevel(pUnit, skillId, curLvl, 0, __FILE__, __LINE__);
			addSkillPoint += d2common_GetSkillCost(pUnit, skpoints, skillId, curLvl);
		}
	}
	SetSkillBaseLevelOnClient(getClient(pUnit), pUnit, skillId, newLvl, 0);
}
void hex(FILE *fp,int addr,void *buf1,int n);
unsigned short *getQuest(World *world,UnitAny *pUnit) {
	int difficulty=world->difficulty;
	PlayerData *playerData=d2common_GetPlayerData(pUnit);
	QuestInfo *info=playerData->quests[difficulty];
	unsigned short *quest=info->quests;
	//LOG("difficulty=%d info=%X\n",difficulty,info);
	//LOG("quest=%X\n",quest);
	return quest;
}
static void resetQuest(World *world, UnitAny* pUnit,int questId) {
	char packet[1]={0x40};
	unsigned short *qs=getQuest(world,pUnit);
	unsigned short q=qs[questId];
	int removeSkill=0,removeHP=0,removeStat=0;
	switch (questId) {
		case 1:if (q&1) removeSkill=1;break;
		case 9:if ((q&3)&&(!(q&0x20))) removeSkill=1;break;
		case 20:if ((q&1)&&(!(q&0x20))) removeHP=20;break;
		case 17:if (q&1) removeStat=5;break;
		case 25:if (q&1) removeSkill=2;break;
	}
	if (removeSkill&&d2common_GetUnitStat(pUnit,STAT_NEWSKILLS,0)<removeSkill) return;
	if (removeStat&&d2common_GetUnitStat(pUnit,STAT_STATPOINTSLEFT,0)<removeStat) return;
	if (removeHP&&(d2common_GetUnitStat(pUnit,STAT_MAXHP,0)>>8)<=removeHP) return;
	qs[questId]=0;
	if (removeSkill) d2common_AddPlayerStat(pUnit,STAT_NEWSKILLS,-removeSkill,0);
	if (removeStat) d2common_AddPlayerStat(pUnit,STAT_STATPOINTSLEFT,-removeStat,0);
	if (removeHP) d2common_AddPlayerStat(pUnit,STAT_MAXHP,-removeHP<<8,0);
	d2game_recv40_quest(world,pUnit,packet,1);
}
int serverUndo(World *world,UnitAny *player,char *msg,char *end) {
	int n=strlen(undoPrefix);
	if (memcmp(msg,undoPrefix,n)!=0) return 0;
	msg+=n;
	//packet[1]: 0:strength 1:energy 2:dexterity 3:vitality
	char *cmd=msg;int param=0,count=0;
	char *v=strchr(msg,':');
	if (v) {
		*v=0;v++;param=strtol(v,&v,0);
		if (*v==',') {
			v++;count=strtol(v,&v,0);
		}
	}
	LOG("undo %s %d %d\n",cmd,param,count);
	if (strcmp("skill",cmd)==0&&fServerUndoSkill) {UndoSkill(player,param,count);return 1;}
	if (strcmp("str",cmd)==0&&fServerUndoStat) {UndoStr(player,count);return 1;}
	if (strcmp("dex",cmd)==0&&fServerUndoStat) {UndoDex(player,count);return 1;}
	if (strcmp("vit",cmd)==0&&fServerUndoStat) {UndoVit(player,count);return 1;}
	if (strcmp("ene",cmd)==0&&fServerUndoStat) {UndoEne(player,count);return 1;}
	if (strcmp("quest",cmd)==0&&fServerUndoQuest) {resetQuest(world,player,param);return 1;}
	if (strcmp("test",cmd)==0) {serverRecvTest(world,player,param);return 1;}
	return 0;
}


/*
...
6FDD2BD2 - 68 D4A3DD6F           - push 6FDDA3D4 { ("inventory") }
6FDD2BD7 - 51                    - push ecx
...
6FDD2CA4 - E8 97C2FDFF           - call 6FDAEF40 d2common_CompileTxtFile
6FDD2CA9 - A3 5CFADE6F           - mov [6FDEFA5C d2common_pInventoryTxt],eax { (028AF610) } <---
*/
void __declspec(naked) ModifyStashGridPatch_ASM() {
	__asm {
		pushad
		mov ecx,d2common_pInventoryTxt
		mov dword ptr [ecx], eax
		call modifyStashGrid
		popad
		ret
	}
}

void* __stdcall compileRunesTxt(int unused, char* filename, TxtField* ptFields,
	int* ptRecordCount, int recordLength) {
	RunesBIN* ptRunesBin=(RunesBIN*)d2common_CompileTxtFile(unused,filename,ptFields,ptRecordCount,recordLength);
	//enabled the ladder only runewords
	for (int i=0; i < *ptRecordCount; i++) ptRunesBin[i].Server=0;
	return ptRunesBin;
}

extern char *szVersion;
static char d2hackmapMsg[256];
static int d2hackmapMsgLen=0;
char *printPanelConf(int isServer,char *s,char *end);
static void createHackmapMsg() {
	char *s=d2hackmapMsg,*end=d2hackmapMsg+255;
	s+=_snprintf(s,end-s,"d2hackmap server patch %s:",szVersion);
	s=printPanelConf(1,s,end);
	if (fServerUndoStat) s+=_snprintf(s,end-s," undoStat");
	if (fServerUndoSkill) s+=_snprintf(s,end-s," undoSkill");
	if (fServerUndoQuest) s+=_snprintf(s,end-s," undoQuest");
	if (fServerEnableSlainCowKing) s+=_snprintf(s,end-s," cowKing");
	if (fServerEnableNihlathakPortal) s+=_snprintf(s,end-s," nihPortal");
	*s++=0;d2hackmapMsgLen=s-d2hackmapMsg;
}

static int __fastcall skipDropTxt(int txt) {
	if (!fServerNoMonsterDrop) return 0;
	int idx=GetItemIndex(txt)+1;
	switch (idx) {
		case 2014:case 2017:case 2018:return 0;
		case 2036:return 1;
		default:
			if (2024<=idx&&idx<=2049) return 0;
			if (idx>=2137) return 0;
	}
	return 1;
}
/*
009D2A03 - 52                    - push edx
009D2A04 - 8B 55 08              - mov edx,[ebp+08]
009D2A07 - 57                    - push edi
009D2A08 - 50                    - push eax
009D2A09 - 51                    - push ecx //txt
009D2A0A - 52                    - push edx
009D2A0B - E8 70EEFFFF           - call 9D1880 d2game_DropItem1880
*/
void __declspec(naked) NoMonsterDropPatch_ASM() {
	__asm {
		pushad
		call skipDropTxt
		test eax,eax
		popad
		jne skip
		jmp d2game_DropItem1880
skip:
		xor eax,eax
		ret 0x14
	}
}
void resetUberQuest();
static void __fastcall gameStart(NetClient *client) {
	resetUberQuest();
	if (!d2hackmapMsgLen||!d2hackmapMsg[0]) createHackmapMsg();
	sendMessageToClient(client,4,0,"<d2hackmap113c>",d2hackmapMsg);
	LOG("gameStart player=%X\n",client->player);
}
//d2game_2AC77: E8 64 F7 05 00     call d2game_8A3E0
void __declspec(naked) GameStartPatch_ASM() {
	__asm {
		pushad
		mov ecx,eax
		call gameStart
		popad
		jmp d2game_sendPacket
	}
}
static void __fastcall serverSendHandshake(NetClient *client,UnitAny *pUnit) {
	if (pUnit) {
		for (int i=0;i<12;i++) {
			if (addPlayerStat[i]>0) {
				int t=addPlayerStat[i];
				if (STAT_HP<=i&&i<=STAT_MAXSTAMINA) t<<=8;
				d2common_AddPlayerStat(pUnit,i,t,0);
			}
		}
		if (addPlayerRep>0) {
			d2common_AddPlayerStat(pUnit,STAT_HPREGEN,addPlayerRep,0);
		}
	}
}
//d2game_4BE00: E8 6B ED FF FF     call d2game_4AB70 int __stdcall d2game_sendHandshake(NetClient* client)//eax:UnitAny *player(1 args)
void __declspec(naked) ServerSendHandShakePatch_ASM() {
	__asm {
		pushad
		mov ecx,[esp+0x24]
		mov edx,eax
		call serverSendHandshake
		popad
		jmp d2game_sendHandshake
	}
}
static void __fastcall serverLoadComplete(NetClient *client) {
}
//d2game_2DB0F: call d2game_8A3E0 int __stdcall d2game_sendPacket(char *packet,int size)//eax:NetClient* client(2 args)
void __declspec(naked) ServerSendLoadCompletePatch_ASM() {
	__asm {
		pushad
		mov ecx,eax
		call serverLoadComplete
		popad
		jmp d2game_sendPacket
	}
}
static void __fastcall serverSendPacket(int *esp) {
	if (!logfp) return;
	int ret=esp[0];unsigned char *packet=(unsigned char *)esp[1];int size=esp[2];
	fprintf(logfp,"ServerSend ret=%X len=%d: ",ret,size);
	for (int i=0;i<size;i++) fprintf(logfp," %02X",packet[i]);
	fputc('\n',logfp);
	return;
	switch (packet[0]) {
		case 0x0B:
		dumpStackFrom(esp,128);
		break;
	}
}
/*
function int __stdcall d2game_sendPacket(char *packet,int size)//eax:NetClient* client {
	d2game_8A3E0: 53                 push ebx
	d2game_8A3E1: 55                 push ebp
	d2game_8A3E2: 8B 6C 24 10        mov ebp, [esp+0x10]
	d2game_8A3E6: 56                 push esi
	d2game_8A3E7: 8B F0              mov esi, eax <--
	d2game_8A3E9: 33 DB              xor ebx, ebx
	d2game_8A3EB: 85 F6              test esi, esi
*/
void __declspec(naked) ServerSendPacketPatch_ASM() {
	__asm {
		pushad
		lea ecx,[esp+0x30]
		call serverSendPacket
		popad
		mov esi,eax
		xor ebx,ebx
		test esi,esi
		ret
	}
}
static void __fastcall serverActiveBufferItem(World *world,UnitAny *player,int id) {
	//LOG("server active %d\n",id);
	UnitAny *pItem=getUnitFromWorld(world,UNITNO_ITEM,id);if (!pItem) return;
	int txt=pItem->dwTxtFileNo,area=0,n=1;
	if (txt==simpleItemStackTxt) {
		int txt=0;int n=getSimpleItemStackContent(pItem,&txt);
		if (!n) return;
		if (!txt) return;
		pickupItem(world,player,dropItemTxt(world,player,txt,0,pItem->pItemData->dwItemLevel,0),0);
		return;
	}

	if (679<=txt&&txt<=690) area=157+txt-679;
	else if (694<=txt&&txt<=713) area=137+txt-694;
	else {
		switch (txt) {
			case 691:area=169;n=2;break;//海皇宮
			case 692:area=171;n=3;break;//冥王宮
			case 693:area=174;n=2;break;//女神宮
			case 715:area=176;n=2;break;//天堂
		}
	}
	if (!area) return;
	while (n>1) {
		AreaRectData* pData=d2common_getRectData(player);
		if (!hasPortalNearby(pData,area)) break;
		n--;area++;
	}
	portalToLevel(world,player,area);
}
/*
	d2game_CF75B: push edi(UnitAny pUnit)
	d2game_CF75C: mov ecx, esi <-- id
	d2game_CF75E: mov eax, edx(World world)
	d2game_CF760: mov [esp+0x1C(arg0:*packet)], ebx
	d2game_CF764: E8 97 D7 FF FF     call d2game_CCF00 int __fastcall d2game_itemNotInInv(int itemId,int notUsed,UnitAny *pUnit)//eax:World *world
*/
void __declspec(naked) ServerRecvPacket20Patch_ASM() {
	__asm {
		pushad
		push esi
		mov ecx, eax
		mov edx, edi
		call serverActiveBufferItem
		popad
		jmp d2game_itemNotInInv
		ret
	}
}
static int __fastcall skipSwapItem(World *world,UnitAny *player,char *packet) {
	int cursorId=*(int *)(packet+1);int bufferId=*(int *)(packet+5);
	int x=*(int *)(packet+9);int y=*(int *)(packet+13);
	UnitAny *pItem=getUnitFromWorld(world,UNITNO_ITEM,bufferId);if (!pItem) return 0;
	if (pItem->dwTxtFileNo==simpleItemStackTxt) {
		int txt=0;int n=getSimpleItemStackContent(pItem,&txt);
		if (n||txt) return 0;
		UnitAny *item2=getUnitFromWorld(world,UNITNO_ITEM,cursorId);if (!item2) return 0;
		d2common_setItemFlag(pItem,ITEMFLAG_COMPACTSAVE,0);
		d2common_changeUnitStat(pItem,simpleItemStackStatId,1,item2->dwTxtFileNo);
		NetClient *client=player->pPlayerData->client;
		sendItemToClient(client,player,pItem);
		return 1;
	}
	return 0;
}
/*
	d2game_D102D: 8B 48 01           mov ecx, [eax+0x1]
	d2game_D1030: 8B 50 09           mov edx, [eax+0x9]
*/
void __declspec(naked) ServerRecvPacket1FPatch_ASM() {
	__asm {
		pushad
		mov ecx,ebx //world
		mov edx,esi //player
		push eax //packet
		call skipSwapItem
		test eax,eax
		popad
		jnz skip
		mov ecx,[eax+1]
		mov edx,[eax+9]
		ret
skip:
		pop eax
		xor eax,eax
		pop esi
		pop ebx
		pop ecx
		ret 8
	}
}
static int __fastcall skipServerChangeHp(UnitAny *pUnit,int hp) {
	switch (pUnit->dwUnitType) {
		case UNITNO_PLAYER: {
			if (fServerInfiniteLife) {
				int curhp=d2common_GetUnitStat(pUnit,STAT_HP,0);
				if (hp<curhp) return 1;
			}
			break;
		}
		case UNITNO_MONSTER:
			switch (pUnit->dwTxtFileNo) {
				case Mon_Act1Hireling:case Mon_Act2Hireling:case Mon_Act3Hireling:
				case Mon_Act5Hireling1hs:case Mon_Act5Hireling:
				case Mon_Necroskeleton:case Mon_Necromage:
					if (fServerInfiniteLife) {
						int curhp=d2common_GetUnitStat(pUnit,STAT_HP,0);
						if (hp<curhp) return 1;
					}
					break;
				default:
					if (fServerInstantKill) d2common_changeUnitStat(pUnit,STAT_HP,0,0);
					break;
			}
			return 1;
	}
	return 0;
}
/*
	d2game_DE25E: 6A 00              push 0
	d2game_DE260: 50                 push eax
	d2game_DE261: 6A 06              push 6
	d2game_DE263: 56                 push esi
  d2game_DE264: E8 09 C2 F2 FF     call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args)
*/
void __declspec(naked) ServerChangeHpPatch_ASM() {
	__asm {
		pushad
		mov ecx,esi
		mov edx,eax
		call skipServerChangeHp
		test eax,eax
		popad
		jnz skip
		jmp d2common_changeUnitStat
skip:
		ret 16
	}
}
/*
	d2game_D95F4: 6A 00              push 0
	d2game_D95F6: 50                 push eax
	d2game_D95F7: 6A 06              push 6
	d2game_D95F9: 55                 push ebp
	d2game_D95FA: E8 73 0E F3 FF     call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args)
*/
void __declspec(naked) ServerChangeHp2Patch_ASM() {
	__asm {
		pushad
		mov ecx,ebp
		mov edx,eax
		call skipServerChangeHp
		test eax,eax
		popad
		jnz skip
		jmp d2common_changeUnitStat
skip:
		ret 16
	}
}
/*
	d2game_77D0C: 6A 00              push 0
	d2game_77D0E: 57                 push edi
	d2game_77D0F: 6A 06              push 6
	d2game_77D11: 56                 push esi
	d2game_77D12: E8 5B 27 F9 FF     call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args)
	d2game_76844: push 0
	d2game_76846: push edi             ; esp-32
	d2game_76847: push 6               ; esp-36
	d2game_76849: push esi(input_edx)  ; esp-40
	d2game_7684A: E8 23 3C F9 FF     call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args)
*/
void __declspec(naked) ServerChangeHp3Patch_ASM() {
	__asm {
		pushad
		mov ecx,esi
		mov edx,edi
		call skipServerChangeHp
		test eax,eax
		popad
		jnz skip
		jmp d2common_changeUnitStat
skip:
		ret 16
	}
}
static void __fastcall checkSpawnedMonster(UnitAny *pUnit) {
	if (!pUnit||pUnit->dwUnitType!=1) return;
	int pet=0;
	switch (pUnit->dwTxtFileNo) {
		case Mon_Act1Hireling:case Mon_Act2Hireling:case Mon_Act3Hireling:
		case Mon_Act5Hireling1hs:case Mon_Act5Hireling:
		case Mon_Necroskeleton:case Mon_Necromage:
			pet=1;
			break;
	}
	static int stats[]={36,37,39,41,43,45,0};
	for (int *p=stats;*p;p++) {
		int resist=d2common_GetUnitStat(pUnit,*p,0);
		if (pet) {
			if (resist<maxPlayerResist) d2common_changeUnitStat(pUnit,*p,maxPlayerResist,0);
		} else {
			if (resist>maxMonsterResist) d2common_changeUnitStat(pUnit,*p,maxMonsterResist,0);
		}
	}
}
//d2game_B045A: 8B 46 14           mov eax,  [esi(arg0:UnitAny pMon)+0x14(->ItemData *pItemData)]
//d2game_B045D: 85 C0              test eax, eax
void __declspec(naked) ServerSpawnMonsterPatch_ASM() {
	__asm {
		pushad
		mov ecx,esi
		call checkSpawnedMonster
		popad
		mov eax,[esi+0x14]
		test eax,eax
		ret
	}
}
/*
	d2game_B0116: push ebx(UnitAny pMon) ; esp-116
	d2game_B0117: call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args) ; esp-120
	d2game_B011C: 8B 74 24 6C        mov esi, [esp+0x6C(arg0:World * game)] 
	d2game_B0120: 8B 4E 70           mov ecx, [esi(arg0:World  game)+0x70(->int expansion)] <--
	d2game_B0123: 33 C0              xor eax, eax
*/
void __declspec(naked) ServerSpawnMonster2Patch_ASM() {
	__asm {
		pushad
		mov ecx,ebx
		call checkSpawnedMonster
		popad
		mov ecx,[esi+0x70]
		xor eax,eax
		ret
	}
}


void server_test(int param) {
	sendChatMessageToServer("%stest:%d,%d",undoPrefix,param,0);
}
int DoServerTest() {
	//server_test(0);
	return 0;
}
AreaRectData * testPosition(AreaRectData *pData,int x,int y);
struct NecSummon {
	int mode;UnitAny *pUnit;int txt,offc;
	int off10,x,y,off1c;
	int maxCount;
};
static UnitAny *necSummon(struct NecSummon *ns) {
	__asm {
		mov eax, ns
		call d2game_NecSummon
	}
}
static void necSummon2(UnitAny *pMon,World *world,UnitAny *pUnit,int skillId) {
	__asm {
		/*push skillId
		push pMon
		push world
		xor ecx,ecx
		mov eax,pUnit
		call d2game_fun_4E2A0
		*/
	}
}
static void serverRecvTest(World *world, UnitAny* pUnit,int param) {
	AreaRectData *pData=d2common_getRectData(pUnit);
	POINT pos;d2common_getUnitPos(pUnit,&pos);
	if (!d2common_GetDropPos(pData, &pos, &pos, 3, 0x3E01, 0xC01, 0)) return;
	pData = testPosition(pData, pos.x, pos.y);if (!pData) return;
	/*
	struct SpawnMon sm;
	sm.world=world;sm.pData=pData;sm.off08=0;sm.txt=Mon_Necroskeleton;//Mon_UberDiablo;
	sm.mode=8;sm.uid=0;sm.x=pos.x;sm.y=pos.y;
	sm.off20=-1;sm.off24=0x42;
	d2game_SpawnMonsterEF10(&sm);
	*/
	int skillId=Skill_RaiseSkeleton;
	/*
	int dataTable=*d2game_psgptDataTables;
	dataTable=*(int *)dataTable;
	int skillTxts=*(int *)(dataTable+0xB98);
	int skillTxt=skillTxts+skillId*0x23C;
	int sparam=*(int *)(skillTxt+0xC0);
	LOG("sparam=%x\n",sparam);
	*/
	struct NecSummon ns;
	ns.mode=1;ns.pUnit=pUnit;ns.txt=Mon_Necroskeleton;ns.offc=0;
	ns.off10=8;ns.x=pos.x;ns.y=pos.y;ns.off1c=4;
	ns.maxCount=16;
	UnitAny *pMon=necSummon(&ns);
	//necSummon2(pMon,world,pUnit,skillId);
/*
	char packet[256];*(int *)packet=0;*(int *)(packet+4)=0;*(int *)(packet+8)=0;
	int type=4,color=0;
	char *name="admin",*msg="test msg";
	packet[0]=0x26;packet[1]=type;packet[3]=2;packet[8]=color;packet[9]=0;
	char *s=packet+10;
	int len=strlen(name);memcpy(s,name,len+1);s+=len+1;
	len=strlen(msg);memcpy(s,msg,len+1);s+=len+1;
	int playerInfo=*(int *)((char *)world+0x88);
	int packetLen=s-packet;
	__asm {
		push packetLen
		lea eax, packet
		push eax
		mov eax, playerInfo
		call d2game_sendPacket
	}
*/
}
int installUberQuestPatches();
void ServerRecvPacket15Patch_ASM();
void SelectUniquePatch_ASM();
void SelectSetPatch_ASM();
void ClientChatPatch_ASM();
void MonsterSleepPatch_ASM();
extern int fEnableBigStash;
int installServerPatches() {
//d2game_CDBC5: B9 44 00 00 00     mov ecx, 0x44 (68)
PatchCall(d2game,0xCDBC5,ServerRecvPacket15Patch_ASM,5,"B9 44 00 00 00");
//d2client_70803: B9 40 00 00 00     mov ecx, 0x40 (64)
PatchCall(d2client,0x70803,ClientChatPatch_ASM,5,"B9 40 00 00 00");
	if (fServerEnableSlainCowKing) {
//d2game_65AEC: 6A 0A              push 0xA
//d2game_65AEE: 6A 04              push 4
//d2game_65AF0: 50                 push eax
//d2game_65AF1: E8 84 4A FA FF     call d2game_A57A->d2common_2D0B0 d2common/Ordinal10003
patchFill("Do not set 4-10 after cow king killed",PATCH_ADDR(d2game,0x65AEC),INST_NOP,10,"6A 0A 6A 04 50 E8 84 4A FA FF");
//d2game_61529: 6A 0A              push 0xA
//d2game_6152B: 6A 04              push 4
//d2game_6152D: 50                 push eax
//d2game_6152E: E8 47 90 FA FF     call d2game_A57A->d2common_2D0B0 d2common/Ordinal10003
patchFill("Do not set 4-10 after cow king killed",PATCH_ADDR(d2game,0x61529),INST_NOP,10,"6A 0A 6A 04 50 E8 47 90 FA FF");
//d2game_67508: 85 C0              test eax, eax
if (*(char *)DLL_ADDR(d2game,0x67508)!=0x33)
	patchValue("enable cow level after cow king killed",PATCH_ADDR(d2game,0x67508),0x33,1,"85 C0");
	}
//d2game_5AE67: 0F 85 BF 00 00 00  jnz d2game_5AF2C ->+191 B5AF2C
patchFill("Do not close Nihlathak's portal",PATCH_ADDR(d2game,0x5AE67),INST_NOP,6,"0F 85 BF 00 00 00");
	if (!hasPlugY)
//d2common_724B1: E8 8A CA FE FF     call d2common_5EF40 d2common/Ordinal10849
PatchCall(d2common,0x724B1,compileRunesTxt,5,"E8 8A CA FE FF"); //Conflict with PlugY
	if (fEnableBigStash)
//d2common_82CA9: A3 5C FA DE 6F     mov [d2common_9FA5C](->28A2460(->"@")), eax
PatchCall(d2common,0x82CA9,ModifyStashGridPatch_ASM,5,"A3 $(+9FA5C)");
	if (fServerInfiniteSocket) {
		//d2game_585C0: 56                 push esi
		patchValue("Infinite socket quest",PATCH_ADDR(d2game,0x585C0),0xC3,1,"56");
	}
	if (fServerInfiniteImbue) {
		/*
			d2game_637EF: 6A 00              push 0
			d2game_637F1: 6A 03              push 3
			d2game_637F3: 56                 push esi
			d2game_637F4: E8 81 6D FA FF     call d2game_A57A->d2common_2D0B0 void __stdcall d2common_setQuestFlag(QuestInfo *questData,int questId,int pos)(3 args)
			d2game_637F9: 6A 01              push 1
			d2game_637FB: 6A 03              push 3
			d2game_637FD: 56                 push esi
			d2game_637FE: E8 83 6D FA FF     call d2game_A586->d2common_2D070 void __stdcall d2common_clearQuestFlag(QuestInfo *questData,int questId,int pos)(3 args)
		*/
		patchFill("Infinite imbue",PATCH_ADDR(d2game,0x637EF),INST_NOP,20,"6A 00 6A 03 56 E8 81 6D FA FF 6A 01 6A 03 56 E8 83 6D FA FF");
	}
	if (fServerInfiniteIdentify) {
		//d2game_DA280: 6A FF                  push -1
		patchValue("Infinite book of identify",PATCH_ADDR(d2game,0xDA281),0,1,"-1 6A FF");
		//d2game_DA2B8: 55                 push ebp
		//d2game_DA2B9: 8B C7              mov eax, edi
		//d2game_DA2BB: E8 20 EB FF FF     call d2game_D8DE0
		patchFill("Infinite book of identify skill",PATCH_ADDR(d2game,0xDA2B8),INST_NOP,8,"55 8B C7 E8 20 EB FF FF");
	}
	if (fServerInfinitePortal) {
		//d2game_D9D22: 6A FF                  push -1
		patchValue("Infinite book of town portal",PATCH_ADDR(d2game,0xD9D23),0,1,"-1 6A FF");
		//d2game_D9D87: 56                 push esi
		//d2game_D9D88: 8B C7              mov eax, edi
		//d2game_D9D8A: E8 51 F0 FF FF     call d2game_D8DE0
		patchFill("Infinite book of town portal skill",PATCH_ADDR(d2game,0xD9D87),INST_NOP,8,"56 8B C7 E8 51 F0 FF FF");
	}
	if (fServerInfiniteArrow) {
		//d2game_A16BE: 4D                 dec ebp
		patchValue("Infinite arrow",PATCH_ADDR(d2game,0xA16BE),0x90,1,"4D");
	}
	if (fServerInfiniteMana) {
		//d2game_9F955: F7 DF              neg edi
		//d2game_9F957: 57                 push edi
		patchValue("Infinite mana",PATCH_ADDR(d2game,0x9F955),0x90006A,3,"F7 DF 57");
		//d2game_DA984: 2B F7              sub esi, edi
		patchFill("Infinite mana",PATCH_ADDR(d2game,0xDA984),INST_NOP,2,"2B F7");
	}
	if (fServerInfiniteDurability) {
		//d2game_12F6F: 4D                 dec ebp
		patchFill("InfiniteDurability",PATCH_ADDR(d2game,0x12F6F),INST_NOP,1,"4D");
	}
	//d2game_F6BD: 6A 00                  push 0x00
	patchValue("identified unique",PATCH_ADDR(d2game,0xF6BE),1,1,"00");//identified unique
	//d2game_F651: 8B 34 CD 98 1F B6 00  mov esi, [ecx*8+d2game_111F98]
	PatchCall(d2game,0xF651,SelectUniquePatch_ASM,7,"8B 34 CD $(+111F98)");
	//d2game_13DA4: E8 01 74 FF FF     call d2game_B1AA->d2common_225F0 d2common/Ordinal10536
	PatchCall(d2game,0x13DA4,SelectSetPatch_ASM,5,"E8 01 74 FF FF");
	//d2game_13DA9: 6A 00                  push 0x00
	patchValue("identified set",PATCH_ADDR(d2game,0x13DAA),1,1,"00");//identified set
	//d2common_48B1F: 75 04              jnz d2common_48B25 ->+4 B48B25
	//d2common_48B21: 8B C7              mov eax, edi
	//patchFill("MaximizeRangedProp",PATCH_ADDR(d2common,0x48B1F),0x90,4,"75 04 8B C7");
	//d2game_2AC77: E8 64 F7 05 00     call d2game_8A3E0
	PatchCall(d2game,0x2AC77,GameStartPatch_ASM,5,"E8 64 F7 05 00");
	//d2game_4BE00: E8 6B ED FF FF     call d2game_4AB70 int __stdcall d2game_sendHandshake(NetClient* client)//eax:UnitAny *player
	PatchCall(d2game,0x4BE00,ServerSendHandShakePatch_ASM,5,"E8 6B ED FF FF");
	//d2game_2DB0F: E8 CC C8 05 00     call d2game_8A3E0 int __stdcall d2game_sendPacket(char *packet,int size)//eax:NetClient* client(2 args)
	PatchCall(d2game,0x2DB0F,ServerSendLoadCompletePatch_ASM,5,"E8 CC C8 05 00");
		if (!hasPlugY) installUberQuestPatches();
	//d2game_12A0B: E8 70 EE FF FF     call d2game_11880
	//PatchCall(d2game,0x12A0B,NoMonsterDropPatch_ASM,5,"E8 70 EE FF FF");
	//d2game_9D4E7: 8B CD              mov ecx, ebp
	//d2game_9D4E9: FF 50 04           call dword ptr [eax+0x4]
	PatchCall(d2game,0x9D4E7,MonsterSleepPatch_ASM,5,"8B CD FF 50 04");
/*
	d2game_DB466: 83 F8 5F           cmp eax, 0x5F (95) ('_')
	d2game_DB469: 7C 05              jl d2game_DB470 ->+5 BDB470
	d2game_DB46B: B8 5F 00 00 00     mov eax, 0x5F (95) ('_')
*/
	if (maxPlayerResist>=100) {
		patchFill("max player resist",PATCH_ADDR(d2game,0xDB46B),INST_NOP,5,"B8 5F 00 00 00");
	} else if (maxPlayerResist>95) {
		patchValue("max player resist",PATCH_ADDR(d2game,0xDB468),maxPlayerResist,1,"5F");
		patchValue("max player resist",PATCH_ADDR(d2game,0xDB46C),maxPlayerResist,1,"5F");
	}
	if (tEnable2025Patch.isOn) {
		//d2game_CF764: E8 97 D7 FF FF     call d2game_CCF00 int __fastcall d2game_itemNotInInv(int itemId,int notUsed,UnitAny *pUnit)//eax:World *world
		PatchCall(d2game,0xCF764,ServerRecvPacket20Patch_ASM,5,"E8 97 D7 FF FF");
		//d2game_D102D: 8B 48 01           mov ecx, [eax+0x1]
		//d2game_D1030: 8B 50 09           mov edx, [eax+0x9]
		PatchCall(d2game,0xD102D,ServerRecvPacket1FPatch_ASM,6,"8B 48 01 8B 50 09");
	}
  //d2game_DE264: E8 09 C2 F2 FF     call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args)
	PatchCall(d2game,0xDE264,ServerChangeHpPatch_ASM,5,"E8 09 C2 F2 FF");
	//d2game_D95FA: E8 73 0E F3 FF     call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args)
	PatchCall(d2game,0xD95FA,ServerChangeHp2Patch_ASM,5,"E8 73 0E F3 FF");
	//d2game_77D12: E8 5B 27 F9 FF     call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args)
	PatchCall(d2game,0x77D12,ServerChangeHp3Patch_ASM,5,"E8 5B 27 F9 FF");
	//d2game_7684A: E8 23 3C F9 FF     call d2game_A472->d2common_3A740 int __stdcall d2common_changeUnitStat(UnitAny *pUnit,int statId,int value,int param)(4 args)
	PatchCall(d2game,0x7684A,ServerChangeHp3Patch_ASM,5,"E8 23 3C F9 FF");
	if (maxMonsterResist) {
		//d2game_B045A: 8B 46 14           mov eax,  [esi(arg0:UnitAny pMon)+0x14(->ItemData *pItemData)]
		//d2game_B045D: 85 C0              test eax, eax
		PatchCall(d2game,0xB045A,ServerSpawnMonsterPatch_ASM,5,"8B 46 14 85 C0");
		//d2game_B0120: 8B 4E 70           mov ecx, [esi(arg0:World  game)+0x70(->int expansion)] 
		//d2game_B0123: 33 C0              xor eax, eax
		//PatchCall(d2game,0xB0120,ServerSpawnMonster2Patch_ASM,5,"8B 4E 70 33 C0");
		if (0) {
			//d2game_AFF66: 0F BF 94 75 50 01 00 00  movsx edx, word ptr [ebp+esi*2+0x150]
			//d2game_AFF6E: 6A 00              push 0
			//d2game_AFF70: 52                 push edx
			patchFill("monster fr=0",PATCH_ADDR(d2game,0xAFF66),INST_NOP,7,"0F BF 94 75 50 01 00");
			patchValue("monster fr=0",PATCH_ADDR(d2game,0xAFF6D),0x006A006A,4,"00 6A 00 52");
			//d2game_AFF79: 0F BF 84 75 56 01 00 00  movsx eax, word ptr [ebp+esi*2+0x156]
			//d2game_AFF81: 6A 00              push 0
			//d2game_AFF83: 50                 push eax
			patchFill("monster lr=0",PATCH_ADDR(d2game,0xAFF79),INST_NOP,7,"0F BF 84 75 56 01 00");
			patchValue("monster lr=0",PATCH_ADDR(d2game,0xAFF80),0x006A006A,4,"00 6A 00 50");
			//d2game_AFF8C: 0F BF 8C 75 5C 01 00 00  movsx ecx, word ptr [ebp+esi*2+0x15C]
			//d2game_AFF94: 6A 00              push 0
			//d2game_AFF96: 51                 push ecx
			patchFill("monster cr=0",PATCH_ADDR(d2game,0xAFF8C),INST_NOP,7,"0F BF 8C 75 5C 01 00");
			patchValue("monster cr=0",PATCH_ADDR(d2game,0xAFF93),0x006A006A,4,"00 6A 00 51");
		}
	}
	return 1;
}
