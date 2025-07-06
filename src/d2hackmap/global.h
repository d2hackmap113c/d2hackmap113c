#define LEVELNO dwCurrentLevel
#define ACTNO dwCurrentAct
#define PLAYER (*d2client_pPlayerUnit)
#define PLAYERLIST (*d2client_pRosterUnitList)
#define GAMEINFO (*d2client_pGameInfo)
#define ACT (*d2client_pDrlgAct)
#define LAYER (*d2client_pAutomapLayer)
#define SCREENSIZE (*d2client_pScreenSize)
#define SCREENWIDTH (*d2client_pScreenWidth)
#define SCREENHEIGHT (*d2client_pScreenHeight)
#define DRAWOFFSET (*d2client_pDrawOffset)
#define DIFFICULTY (*d2client_pDifficulty)
#define EXPANSION (*d2client_pExpansion)
#define PING (*d2client_pPing)
#define FPS (*d2client_pFps)
#define QUESTDATA (*d2client_pQuestData)
#define QUESTPAGE (*d2client_pQuestPage)
#define GAMEQUESTDATA (*d2client_pGameQuestData)
#define FOCUSECONTROL (*d2win_pFocusedControl)
#define MONSTER_TYPE_NEUTRAL 0x80
#define MONSTER_TYPE_NORMAL 1
#define MONSTER_TYPE_DANGROUS 2
#define MONSTER_TYPE_REVIVE 4 
#define MONSTER_TYPE_MF 8 
#define MONSTER_TYPE_ACTBOSS 0x10
enum ToggleVarType {
	TOGGLEVAR_ONOFF=1,
	TOGGLEVAR_DOWN=2,
	TOGGLEVAR_DOWNPARAM=3,
	TOGGLEVAR_DOWNUPPARAM=4,
	TOGGLEVAR_DOWNPTR=5,
};
enum ToggleVarKeyType {
	ToggleVarKeyType_InGame=0,
	ToggleVarKeyType_OutGame=1,
	ToggleVarKeyType_Always=2,
};
struct ToggleVar {
	int type;//1:toggle switch 2:press callback without paramter 0,3:with parameter 4:release callback
	int isOn;//是否开启
	int key;//触发按键
	int value32;//命令值 set to >=1 if isOn
	char* desc;//事件描述
	union {
		void *func; //调用的函数 return 0 if handled
		int (*funcVoid)();
		int (*funcInt)(int);
		int (*funcPtr)(void *ptr);
	};
	union {
		int param;     //func2函数参数
		void *paramPtr;
	};
	int isLoad;  //是否在配置中加载的
	int keyType; //0在游戏内起作用,1在游戏外起作用,2一直起作用
	union {
		void *funcUp;
		int (*funcUpVoid)();
		int (*funcUpInt)(int);
		int (*funcUpPtr)(void *ptr);
	};
	wchar_t *wdesc;
	struct ToggleVar *next; //Same key value
};
enum ConfigVarType {
	CONFIG_VAR_TYPE_KEY=1,
	CONFIG_VAR_TYPE_INT=2,
	CONFIG_VAR_TYPE_STR=3,
	CONFIG_VAR_TYPE_WSTR=4,
	CONFIG_VAR_TYPE_D2STR=5,
	CONFIG_VAR_TYPE_CHAR_ARRAY0=6,
	CONFIG_VAR_TYPE_CHAR_ARRAY1=7,
	CONFIG_VAR_TYPE_INT_ARRAY0=8,
	CONFIG_VAR_TYPE_INT_ARRAY1=9,
	CONFIG_VAR_TYPE_STR_ARRAY0=10,
	CONFIG_VAR_TYPE_STR_ARRAY1=11,
	CONFIG_VAR_TYPE_KEY_ARRAY0=12,
	CONFIG_VAR_TYPE_KEY_ARRAY1=13,
	CONFIG_VAR_TYPE_LEVEL_TARGET=14,
	CONFIG_VAR_TYPE_SWITCH_SKILL=15,
	CONFIG_VAR_TYPE_SCREEN_SCROLL=16,
	CONFIG_VAR_TYPE_MINIMAP_SCROLL=17,
	CONFIG_VAR_TYPE_ACCOUNT=18,
	CONFIG_VAR_TYPE_SEND_MESSAGE=19,
	CONFIG_VAR_TYPE_QUICK_TO_LEVEL=20,
	CONFIG_VAR_TYPE_SET_TAG=21,
	CONFIG_VAR_TYPE_STRING_VALUE=22,
};
struct ConfigVar {
	int		type;
	char	*szCmdName;
	void	*pVar;
	int	size32;
	short	anArrayMax[4];
	int keyType;//0:inGame,1:outGame,2:Always
	char *keyDesc;
	int (*keyFunc)(int);
	int base; //First array index
	int useCount;
};
void addConfigVar(ConfigVar *var);
int IsKeyDown(int keyCode);
void formatKey(char *buf,int vk);
void setupKey(ToggleVar *p);
ToggleVar *addExtraKey(int key);
int GetItemIndex(int dwTxtFileNo);
BOOL Install();
void Uninstall();
BOOL LoadConfig();

#define INST_NOP 0x90
#define INST_CALL 0xe8
#define INST_JMP 0xe9
#define INST_JMPR 0xeb
#define INST_RET 0xc3
#define DLL_ADDR(dll,offset) (offset+(DWORD)addr_##dll)
#define PATCH_ADDR(dll,offset) #dll,offset,(offset+(DWORD)addr_##dll)
#define PatchCall(dll,offset,func,len,originalcode) patchCall(#func,#dll,offset,offset+(DWORD)addr_##dll,(DWORD)func,len,originalcode)
#define PatchJmp(dll,offset,func,len,originalcode) patchJmp(#func,#dll,offset,offset+(DWORD)addr_##dll,(DWORD)func,len,originalcode)

void hex(FILE *fp,int addr,void *buf1,int n);
HMODULE get_dll_addr(char *name);
struct InstalledPatch {int addr,len;struct InstalledPatch *next;char oldcode[4];};
int installPatch(void (*func)(int, int, int),char *name,char *dll,int offset,int addr,
	int param,int len,char *original);
int removePatches(InstalledPatch **phead);
int patchCall(char *name,char *dll,int offset,int addr,int param,int len,char *original);
int patchJmp(char *name,char *dll,int offset,int addr,int param,int len,char *original);
int patchFill(char *name,char *dll,int offset,int addr,int param,int len,char *original);
int patchValue(char *name,char *dll,int offset,int addr,int param,int len,char *original);

int waypointExist(int area);
int hasWaypoint(int area);
void refresh_clients();
int isLocalPlayer(int uid);
int isLocalPlayerName(char *name);
void check_d2ptrs();
void draw2map(POINT *minimap,int drawX,int drawY);
void __stdcall dumpStack(int n);
int ReloadConfig();
int BackToTown();
int QuickNextGame(int addnum);

extern HMODULE addr_storm,addr_d2client,addr_d2common,addr_d2gfx,addr_d2win,addr_d2lang;
extern HMODULE addr_d2cmp,addr_bnclient,addr_d2net,addr_fog,addr_d2game,addr_d2launch;
extern HMODULE addr_d2gdi,addr_d2sound;
extern HMODULE addr_d2multi,addr_d2mcpclient;
extern int dwLoopCount,dwDrawUnitCount;
extern char *szVersion;
extern HANDLE dllHeap,confHeap,gameHeap;
extern int dwGameWindowId;
extern char szRuntimePath[256];
extern int fIsSinglePlayer,fIsTcpIp,fIsRealmServer,fIsRealmClient;
extern int fStartingGame,fInGame,fPlayerInTown,dwGameLng;
extern int fIsHardCoreGame,fHasHardCoreConfig,fUsingHardCoreConfig,needReloadConfig,fIsHardCoreSafe;
extern volatile int	dwCurMs;
extern int actlvls[6];
extern int dwCurrentAct,dwCurrentLevel;
extern int dwPlayerX,dwPlayerY,dwPlayerId,dwPlayerPartyId,dwPlayerClass;
extern Skill *pLeftSkill,*pRightSkill;
extern int dwLeftSkill,dwRightSkill;
extern int dwLeftSkillStatus,dwRightSkillStatus;
extern int fCanUseLeftSkill,fCanUseRightSkill;
extern int dwPlayerLevel;
extern int dwPlayerHP,dwPlayerMaxHP;
extern int dwPlayerMana,dwPlayerMaxMana;
extern int dwPlayerFcrFrame,dwPlayerFcrMs;
extern int dwRecheckSelfItemMs;
extern UnitAny *leftWeapon,*rightWeapon;
extern int dwTownPortalCount,dwIdentifyPortalCount,fUsingBow,fUsingCrossBow,fUsingThrow;
extern int dwHPotionCount,dwMPotionCount,dwRPotionCount,dwArrowCount,dwCArrowCount,dwThrowCount;
extern int dwSkillChangeCount; //increase if skill level maybe changed
extern int fWinActive;
extern ToggleVar tShowTestInfo,tPacketHandler;
extern int simpleItemStackTxt,simpleItemStackIdx,simpleItemStackStatId;
extern int screenDrawX,screenDrawY;
extern int altDown,ctrlDown,shiftDown,fUserOperating,hydraLock,hydraUid,hydraX,hydraY;

#define MAX_STASH_PAGES 128
#define MAX_ITEMS_PER_PAGE 128
struct StashPage {
	int n;
	UnitAny *items[MAX_ITEMS_PER_PAGE];
};
extern StashPage *stashPages[MAX_STASH_PAGES];
extern int curStashPage,nStashPages;
