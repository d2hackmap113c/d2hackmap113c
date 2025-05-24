#ifdef VARIABLE_DECLARE
	#define GVAR(d1,v1,v2) d1 v1 = v2 ;
	#define GVAR2(d1,v1,...) d1 v1 = { __VA_ARGS__ } ;
#else
	#define FUNCTION_DECLARE
	#define GVAR(d1,v1,v2) extern d1 v1 ;
	#define GVAR2(d1,v1,...) extern d1 v1 ;
#endif
#ifdef FUNCTION_DECLARE
#define VK_MULTICLIENT_INFO 0x85
#define VK_AUTOFOLLOW_CMD 0x84
#define VK_AUTOFOLLOW_CLICK_OBJECT 0x83
#define VK_MULTICLIENT_LEFT_XY 0x82
#define VK_MULTICLIENT_LEFT_UNIT 0x81
#define VK_MULTICLIENT_RIGHT_XY 0x80
#define VK_MULTICLIENT_RIGHT_UNIT 0x7F
//VK_F12: 0x7B
enum MulticlientCmd {
	MCC_StartFollow=1,
	MCC_StopFollow=2,
	MCC_BackToTown=3,
	MCC_EnterDoor=4,
	MCC_TalkToNpc=5,
};
enum MulticlientInfo {
	MCI_CpuUsage=0x10,
	MCI_WinActiveL=0x20,
	MCI_WinActiveH=0x30,
	MCI_HMRPotion=0x40,
	MCI_Quantity=0x60,
	MCI_Mana=0x70,
	MCI_MaxMana=0x80,
	MCI_Minions=0x90,
	MCI_StartFollow=0xA0,
	MCI_StopFollow=0xB0,
	MCI_AutoSkill=0xC0,
	MCI_Hp=0xD0,
	MCI_MaxHp=0xE0,
};
struct D2Window {
	HWND hwnd;
	int index,uid;
	int sameGame,isTeam,isLeader,isTransferClick;
	int hp,hpMax,mana,manaMax,hPotion,mPotion,rPotion,quantity,minions,enMinions,minionsHp,infoMs;
	int cpu,cpuMs;
	int autoSkillId,autoLeft;
	char ip[32];
	char game[32];
	char name[32];
	char switchkey[32];
};
HWND getGameWindowHwnd(int id);
extern D2Window d2wins[32];
#endif
#ifdef FUNCTION_DECLARE
	int saveRuntimeInfo(HWND hwnd);
	void MultiClientNewGame();
	void MultiClientEndGame();
	void MultiClientLoop();
	int MultiClientToggleFollow();
	int MultiClientStartFollow1();
	int MultiClientStartFollow2();
	int MultiClientStopFollow();
	int MultiClientEnterDoor();
	int leader_back_to_town();
	int MultiClientStartClick(int param);
	int MultiClientStopClick(int param);
	void follower_start_follow(int uid);
	void follower_stop_follow();
	void follower_enter_door(int uid,int area);
	void follower_back_to_town(int uid);
	void follower_left_click_xy(int x,int y);
	void follower_right_click_xy(int x,int y);
	void follower_left_click_unit(int type,int id);
	void follower_right_client_unit(int type,int id);
	void follower_click_object(int type,int id);
	void multiclient_send_info(int info);
	void multiclient_recv_info(int info);
	void leader_click_object(int type,int id);
#endif
	GVAR (int,			dwMultiClientMaxWindowId,					8)
	GVAR (int,			d2winLastId,					0)
	GVAR2 (wchar_t,			wszTransferClick[64],					0)
	GVAR (int,			fAutoFollowMoving,					0)
#ifdef FUNCTION_DECLARE
	#undef FUNCTION_DECLARE
#endif
#undef GVAR
#undef GVAR2
