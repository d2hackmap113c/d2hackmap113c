#ifndef CONFIGSTRUCT_H
#define CONFIGSTRUCT_H

enum ToggleVarType {
	TOGGLEVAR_ONOFF=1,
	TOGGLEVAR_DOWN=2,
	TOGGLEVAR_DOWNPARAM=3,
	TOGGLEVAR_DOWNUPPARAM=4,
};
enum ConfigVarType {
	CONFIG_VAR_TYPE_KEY	= 1,
	CONFIG_VAR_TYPE_INT	= 2,
	CONFIG_VAR_TYPE_STR	= 3,
	CONFIG_VAR_TYPE_D2STR	= 4,
	CONFIG_VAR_TYPE_ARRAY	= 5,
	CONFIG_VAR_TYPE_MINIMAPPOINT = 6,
	CONFIG_VAR_TYPE_GAMEMONITOR = 7,
	CONFIG_VAR_TYPE_INTARRAY	= 10,
	CONFIG_VAR_TYPE_STRARRAY	= 11,
	CONFIG_VAR_TYPE_KEYARRAY	= 12,
	CONFIG_VAR_TYPE_WSTR	= 20
};

struct ToggleVar {
	int  type ;    // 1  按键切换开关(可触发事件)  2 按键触发事件，不带参数 3或0  按键触发事件 带参数 4:带参数且松开时有响应函数
	int  isOn;	    //是否开启
	int  key;	    //触发按键
	int  value32;	//命令值 set to >=1 if isOn
	char* desc;     //事件描述
	union{
		void *func; //调用的函数 return 0 if handled
		int (*funcVoid)();
		int (*funcParam)(int);
	};
	int param;     //func2函数参数
	int isLoad;  //是否在配置中加载的
	int keyType; //0在游戏内起作用,1在游戏外起作用,2一直起作用
	int (*funcUp)(int);//
	struct ToggleVar *next; //Same key value
};

struct ConfigVar {
	int		type ;			//配置类型
	char	*szCmdName;	    //命令名
	void	*pVar;			//保存命令的对象
	int	size32;			//对象值的长度,这里用int会有问题
	short	anArrayMax[4];	//对应数组的各维长度
	int keyType;//0在游戏内起作用,1在游戏外起作用,2一直起作用
	char *keyDesc;
	int (*keyFunc)(int);
	int base; //数组下标起始值，在LoadConig.cpp InitValues里设置
};
    
struct Patch_t{
	void (*func)(DWORD, DWORD, DWORD);
	DWORD addr;
	DWORD param;
	DWORD len;
	BOOL *fEnable;
	BYTE *oldcode;
	BOOL fInit ;
};

//地图指向配置文件
struct MinimapConfig{
	int  nPoints  ;		//指向数量
	struct {
		WORD wPointNo ;	//指向编号
		BYTE fFlag;		//是否生效
		BYTE nColour;	//颜色
	} aPoint[30];
};

struct D2ScreenHookStruct
{
	BYTE *show;
	int xpos;
	int ypos;
	wchar_t wszMsg[512];
	BYTE color;
};

struct InventoryType {
	BYTE invType;
	short left;
	short bottom;
	BYTE nGridXs;
	BYTE nGridYs;
	BYTE nGridWidth;
	BYTE nGridHeight;
};

struct BugQuestInfo{
	DWORD nQuestNo; //任务编号
	DWORD nStatus ; //任务状态
	DWORD nType ;   //保护模式
	BYTE *fEnable ; //是否开始
	char szMsg[10]; //名称
} ;

struct MonTC
{
	WORD wGroup;
	WORD wLevel;
	int nTc;
	int nRtc;
	BOOL fInit;	
};

struct StatMonitor {
	wchar_t wszDesc[2][30];
	DWORD	dwStatNo;
	DWORD	dwTimer;
	DWORD   dwColor;
	BOOL	fEnable;
};

struct D2Window {
	HWND hwnd;
	int uid;
	char ip[32];
	char game[32];
	char name[32];
};
	


#endif
