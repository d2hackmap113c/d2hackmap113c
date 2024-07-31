#ifndef D2_PTR_H
  enum DllBase { //为查找反汇编方便, 以全地址方式定义函数地址，所以需要注意此处基值与反汇编文件中对应
    DLLBASE_D2CLIENT = 0x6FAB0000,
    DLLBASE_D2COMMON = 0x6FD50000,
    DLLBASE_D2GFX = 0x6FA80000,
    DLLBASE_D2WIN = 0x6F8E0000,
    DLLBASE_D2LANG = 0x6FC00000,
    DLLBASE_D2CMP = 0x6FE10000,
    DLLBASE_D2MULTI = 0x6F9D0000,
    DLLBASE_BNCLIENT = 0x6FF20000,
    DLLBASE_D2NET = 0x6FBF0000, 
    DLLBASE_STORM = 0x6FFB0000,//6FBF0000  , 与D2NET冲突，修改值
    DLLBASE_FOG = 0x6FF50000,
    DLLBASE_D2GAME = 0x6FC20000,
    DLLBASE_D2LAUNCH = 0x6FA40000,
	DLLBASE_D2MCPCLIENT = 0x6FA20000
  };

  enum DllNo {DLLNO_D2CLIENT, DLLNO_D2COMMON, DLLNO_D2GFX, DLLNO_D2WIN, DLLNO_D2LANG, DLLNO_D2CMP, DLLNO_D2MULTI, DLLNO_BNCLIENT, DLLNO_D2NET, DLLNO_STORM, DLLNO_FOG, DLLNO_D2GAME , DLLNO_D2LAUNCH,DLLNO_D2MCPCLIENT};
  #define DLLOFFSET(a1,b1) ((DLLNO_##a1)|(( ((b1)<0)?(b1):((b1)-DLLBASE_##a1) )<<8))

  #define D2FUNCPTR(d1,o1,v1,t1,t2) typedef t1 D2##v1##_t t2; extern D2##v1##_t *D2##v1;
  #define D2VARPTR(d1,o1,v1,t1)     typedef t1 p_D2##v1##_t;    extern p_D2##v1##_t *p_D2##v1;
  #define D2ASMPTR(d1,o1,v1)        extern DWORD vD2##v1;
  
  #define D2_PTR_H

#else

  #define D2FUNCPTR(d1,o1,v1,t1,t2) D2##v1##_t *D2##v1 = (D2##v1##_t *)DLLOFFSET(d1,o1);
  #define D2VARPTR(d1,o1,v1,t1)     p_D2##v1##_t *p_D2##v1 = (p_D2##v1##_t *)DLLOFFSET(d1,o1);
  #define D2ASMPTR(d1,o1,v1)        DWORD vD2##v1 = DLLOFFSET(d1,o1);
	
#endif

//此文件需要在任意cpp中重复引用一次

D2ASMPTR(D2CLIENT,  0xEEEEEE00, ERR)
D2ASMPTR(D2CLIENT,  0xEEEEEE00, StartOfPtr)

D2FUNCPTR(D2CLIENT, 0x6FB2D850, ShowGameMessage,    void __stdcall, (wchar_t* text, int nColor) )
D2FUNCPTR(D2CLIENT, 0x6FB2D610, ShowPartyMessage,   void __stdcall, (wchar_t* text, int nColor))

D2FUNCPTR(D2CLIENT, 0x6FAEB8B0, ShowMap,            void __fastcall, ())
D2FUNCPTR(D2CLIENT, 0x6FB12580, RevealAutomapRoom,  void __stdcall, (DrlgRoom1 *room1, DWORD clipflag, AutomapLayer *layer))
D2FUNCPTR(D2CLIENT, 0x6FB5C440, RecvCommand07,      void __fastcall, (BYTE *cmdbuf))
D2FUNCPTR(D2CLIENT, 0x6FB5C3D0, RecvCommand08,      void __fastcall, (BYTE *cmdbuf))
D2FUNCPTR(D2CLIENT, 0x6FB0F6B0, NewAutomapCell,     AutomapCell * __fastcall, ())
D2FUNCPTR(D2CLIENT, 0x6FB11320, AddAutomapCell,     void __fastcall, (AutomapCell *cell, AutomapCell **node))
D2FUNCPTR(D2CLIENT, 0x6FB5CE20, PacketHandler,      void __stdcall, (int len)) //eax=buf


D2FUNCPTR(D2CLIENT, 0x6FAEF6C0, GetPlayerXOffset,   long __stdcall, ())
D2FUNCPTR(D2CLIENT, 0x6FAEF6D0, GetPlayerYOffset,   long __stdcall, ())
D2FUNCPTR(D2CLIENT, 0x6FACBDE0, LeftClickBDE0,           int __stdcall, () ) //esi=buf
D2FUNCPTR(D2CLIENT, 0x6FACBC40, RightClickBC40,           int __stdcall, () ) //esi=buf
D2FUNCPTR(D2CLIENT, 0x6FAC85E0, ShouldClick85E0,           int __stdcall, () ) //ecx=buf
D2FUNCPTR(D2CLIENT, 0x6FB554A0, CanUseSkillNow,           int __stdcall, (int skillId) ) //eax=pUnit
D2FUNCPTR(D2CLIENT, 0x6FAF6150, Unknown6150,           int __stdcall, ())
D2FUNCPTR(D2CLIENT, 0x6FAD3F20, PlaySound,           int __fastcall, (int unitId,int unitType,int sndId))
D2VARPTR(D2CLIENT,  0x6FBCC2F4, HasSelectedObject,             BOOL ) 
D2VARPTR(D2CLIENT,  0x6FBCC2F8, FlagUnknownC2F8,             BOOL ) 
D2VARPTR(D2CLIENT,  0x6FBC9D30, SkillHotKeys,             int ) 
D2VARPTR(D2CLIENT,  0x6FBC9E20, SkillHotKeyLeft,          int ) 
D2VARPTR(D2CLIENT,  0x6FBCA2E0, KeyboardSettingStart, struct KeyboardSetting)
D2VARPTR(D2CLIENT,  0x6FBCA754, KeyboardSettingEnd, struct KeyboardSetting)

D2FUNCPTR(D2CLIENT, 0x6FB72790, SetUiStatus,        DWORD __fastcall, (DWORD dwUiNo, DWORD howset, DWORD unknown1))

D2FUNCPTR(D2CLIENT, 0x6FB3AFD0, CalcShake,          void __stdcall, (DWORD *pPosX, DWORD *pPosY))
D2FUNCPTR(D2CLIENT, 0x6FAD16A0, GetMonsterOwner,    DWORD __fastcall, (DWORD dwUnitId))
D2FUNCPTR(D2CLIENT, 0x6FB55B40, GetUnitFromId,      UnitAny * __fastcall, (DWORD dwUnitId, DWORD dwUnitType)) //根据编号和类型获取对象,monster and palyer
D2FUNCPTR(D2CLIENT, 0x6FB01A80, GetSelectedUnit,    UnitAny * __stdcall, ())


D2FUNCPTR(D2CLIENT, 0x6FB12710, InitAutomapLayerStub,   AutomapLayer* __fastcall, (DWORD nLayerNo)) //nLayerNo==>eax 设置当前Layer指针,如果对应nLayerNo未生成,则分配一个，存于客户端
D2FUNCPTR(D2CLIENT, 0x6FB6E400, CheckUiStatusStub,      DWORD __fastcall, (DWORD dwUiNo) )//dwUiNo ==> eax
D2FUNCPTR(D2CLIENT, 0x6FB0F8D0, SetAutomapPartyStub	,   void __fastcall, (DWORD flag) )    //parm = flag  ==> esi 
D2FUNCPTR(D2CLIENT, 0x6FB0F8A0, SetAutomapNamesStub ,   void __fastcall, (DWORD flag) )    //parm = flag  ==> esi 
D2FUNCPTR(D2CLIENT, 0x6FB20C45, ChatInputStub ,         void __stdcall, (D2MSG *pmsg) )  //与入口偏移了5个长度,使用自定义的eax


D2FUNCPTR(D2CLIENT, 0x6FAD3200, ItemProtect,		void __stdcall, (UnitAny *pUnit, DWORD dwType))
D2FUNCPTR(D2CLIENT, 0x6FAD9250, DrawClient,         void __fastcall, ())
D2FUNCPTR(D2CLIENT, 0x6FABBE84, Storm511,           void __stdcall, (D2MSG *pMsg) )


D2VARPTR(D2CLIENT,  0x6FBCC1C0, AutomapLayerList,   AutomapLayer *) //所有AutomapLayer列表
D2VARPTR(D2CLIENT,  0x6FBCC1C4, AutomapLayer,       AutomapLayer *) //当前所在的AutomapLayer

D2VARPTR(D2CLIENT,  0x6FBCBBFC, PlayerUnit,         UnitAny *)
D2VARPTR(D2CLIENT,  0x6FBCBC14, RosterUnitList,     RosterUnit *) //玩家列表
D2VARPTR(D2CLIENT,  0x6FBCC4D4, PetUnitList,        PetUnit * ) //玩家召唤物列表

D2VARPTR(D2CLIENT,  0x6FBCC3B8, DrlgAct,            DrlgAct *)
D2VARPTR(D2CLIENT,  0x6FBC9854, Expansion,          DWORD ) // 1资料片 0 非资料片
D2VARPTR(D2CLIENT,  0x6FBCC390, Difficulty,         BYTE) //当前难度 0 1 2
D2VARPTR(D2CLIENT,  0x6FBCB980, GameInfo,           GameStructInfo *)
D2VARPTR(D2CLIENT,  0x6FBCC2AC, Fps,                DWORD )
D2VARPTR(D2CLIENT,  0x6FBC9804, Ping,               DWORD )
D2VARPTR(D2CLIENT,  0x6FBA8C9C, ExitAppFlag,        DWORD)
D2VARPTR(D2CLIENT,  0x6FBCC3A0, InGame,             BOOL ) //是否在游戏中?
D2VARPTR(D2CLIENT,  0x6FBCC3F0, StandStill,         BOOL ) //是否站立不动
D2VARPTR(D2CLIENT,  0x6FBA4F10, GameControlStart,  int) //游戏控制函数
D2VARPTR(D2CLIENT,  0x6FBA4F14, GameControlEnd,  int) //游戏控制函数
D2VARPTR(D2CLIENT,  0x6FBCC3A4, notAcceptKeys,         int ) //not accept input?
D2VARPTR(D2CLIENT,  0x6FBCC484, BeltNotEmpty,  char) 
D2FUNCPTR(D2CLIENT, 0x6FAD9990, UseBelt,         void __fastcall, (UnitInventory *pInventory,UnitAny *player,int id)) //edx=PLAYER ecx=PLAYER->pInventory eax=0x8000(shift)or0
//D2VARPTR(D2CLIENT,  0x6FB7F0B4, SetCursorPos,         int ) 
D2VARPTR(D2CLIENT,  0x6FBBA608, UnitTable,         UnitAny * ) //type*128+(id&0x7f)
D2VARPTR(D2CLIENT,  0x6FBA23E8, MercData16,         int ) 
D2VARPTR(D2CLIENT,  0x6FBCC334, MercData32,         int ) 
D2VARPTR(D2CLIENT,  0x6FBACDD6, WaypointMenuCurTab, int ) //classic(0-3) expansion(0-4)
D2VARPTR(D2CLIENT,  0x6FBACDDA, WaypointMenuCount, int ) //act1:9 act4:3
D2VARPTR(D2CLIENT,  0x6FBACD8C, WaypointMenuItems, struct WaypointMenuItem) 

D2VARPTR(D2CLIENT,  0x6FBAADA8, AutomapOn,          DWORD) //小地图是否打开
D2VARPTR(D2CLIENT,  0x6FBA16B0, Divisor,            long)
D2VARPTR(D2CLIENT,  0x6FBCC1F8, Offset,             POINT)
D2VARPTR(D2CLIENT,  0x6FBCC1E8, AutomapPos,         POINT)
D2VARPTR(D2CLIENT,  0x6FBCC230, AutoMapSize,        POINT) //地图大小
D2VARPTR(D2CLIENT,  0x6FBCC1B0, MinmapType,         DWORD) // 1小地图 0 大地图
D2VARPTR(D2CLIENT,  0x6FBCC228, MinimapOffset,      POINT) //小地图边缘地址 ，大地图为 -16*-16

D2VARPTR(D2CLIENT,  0x6FBCBEFC, IsMapShakeOn,       BOOL)
D2VARPTR(D2CLIENT,  0x6FBBB9DC, MapShakeY,          long)
D2VARPTR(D2CLIENT,  0x6FBCBF00, MapShakeX,          long)

D2VARPTR(D2CLIENT,  0x6FB8BC48, ScreenSizeX,        int ) //屏幕宽度X 800*600   640*480
D2VARPTR(D2CLIENT,  0x6FB8BC4C, ScreenSizeY,        int ) //屏幕宽度Y 800*600   640*480
D2VARPTR(D2CLIENT,  0x6FB8BC48, ScreenSize,         POINT)  //屏幕宽度X 800*600   640*480
D2VARPTR(D2CLIENT,  0x6FBCB9A0, DrawOffset,         POINT )//显示框偏移量
D2VARPTR(D2CLIENT,  0x6FBA9E14, InfoPositionX,      int) //游戏信息显示X坐标
D2VARPTR(D2CLIENT,  0x6FBCC21C, InfoPositionY,      int) //游戏信息显示Y坐标

D2VARPTR(D2CLIENT,  0x6FBC973B, QuestData,          void *) //玩家任务信息表
D2VARPTR(D2CLIENT,  0x6FBC973F, GameQuestData,      void *) //房间任务信息表
D2VARPTR(D2CLIENT,  0x6FBD3395, QuestPage,          int)
D2VARPTR(D2CLIENT,  0x6FBC97AC, TradeRelated,      int *) 

D2VARPTR(D2CLIENT,  0x6FBCC3A0, MButton,            BOOL )
D2VARPTR(D2CLIENT,  0x6FBCA76C, XButton1,           BOOL )
D2VARPTR(D2CLIENT,  0x6FBCA768, XButton2,           BOOL )
D2VARPTR(D2CLIENT,  0x6FBCEC80, LastChatMessage,    wchar_t)  
D2VARPTR(D2CLIENT,  0x6FBCC028, ChatTextLength,         int)

D2VARPTR(D2CLIENT,  0x6FBCB824, MousePos,           POINT )//鼠标位置
D2VARPTR(D2CLIENT,  0x6FB8BC54, LastMousePos,		POINT )//最后的鼠标位置
D2VARPTR(D2CLIENT,  0x6FB90EB8, CursorInvGridX,     DWORD)//最后的物品栏位置
D2VARPTR(D2CLIENT,  0x6FB90EBC, CursorInvGridY,     DWORD)//最后的物品栏位置
D2VARPTR(D2CLIENT,  0x6FBCBC38, CurrentViewItem,    UnitAny*) //选择显示的物品
D2VARPTR(D2CLIENT,  0x6FBCBC94, ActiveWeapon,    int) 

D2VARPTR(D2CLIENT,  0x6FBCBBB0, GoldInTranBox,         int)
D2VARPTR(D2CLIENT,  0x6FBCC4B0, ShowLifeStr,         BOOL)
D2VARPTR(D2CLIENT,  0x6FBCC4B4, ShowManaStr,         BOOL)
D2VARPTR(D2CLIENT,  0x6FBCC414, ScreenBlocked,       int) //bit0 right half blocked, bit 1 left half blocked

D2ASMPTR(D2CLIENT,  0x6FBC3880, AutomapCellListBaseAddr)
D2ASMPTR(D2CLIENT,  0x6FADB420, LoadUiImageFunc)

D2FUNCPTR(D2COMMON, -0x29C0,   GetObjectTxt,         ObjectTxt * __stdcall, (DWORD objno))
D2FUNCPTR(D2COMMON, -0x29FD,   GetLevelDefTxt,       LevelDefTxt* __fastcall, (DWORD levelno))
D2FUNCPTR(D2COMMON, -0x271E,   GetLevelTxt,          LevelTxt * __stdcall, (DWORD levelno))
D2FUNCPTR(D2COMMON, -0x29C7,   GetItemTxt,           ItemTxt * __stdcall, (DWORD itemno))
D2FUNCPTR(D2COMMON, -0x298A,   GetTreasureClasses,   TreasureClass * __stdcall, (WORD tcno, DWORD lvlno))
D2FUNCPTR(D2COMMON, -0x2736,   GetSuperUniqueTxt,    SuperUniqueTxt * __stdcall, (DWORD dwUniqueNo))

D2FUNCPTR(D2COMMON, -0x297C,   GetItemFileIndex,     DWORD __stdcall, (UnitAny *pItem) ) //pItem->pItemData->dwFileIndex

D2FUNCPTR(D2COMMON, -0x2B66,   GetStatFromStatList,  BOOL __stdcall, (StatList *pStatList, DWORD dwStatNo, StatEx *pStatVal ,DWORD dwMask ) ) //是否存在属性,有则得到属性值
D2FUNCPTR(D2COMMON, -0x2AB2,   GetStatListWithFlag,  StatList *  __stdcall, (UnitAny *pUnit, DWORD dwStatNo, DWORD dwListFlag) )//
D2FUNCPTR(D2COMMON, -0x2ADD,   GetUnitStat,          int __stdcall, (UnitAny *pUnit, DWORD dwStatNo, WORD dwLayerNo))
D2FUNCPTR(D2COMMON, -0x295B,   GetUnitBaseStat,      int __stdcall, (UnitAny *pUnit, DWORD dwStatNo, WORD dwLayerNo))//只会查询第一条statlist,对玩家大部分属性能查到
D2FUNCPTR(D2COMMON, -0x2B13,   GetMonsterHpPercent,  int __stdcall, (UnitAny *pUnit) )//对玩家无效

D2FUNCPTR(D2COMMON, -10494,   CheckUnitState,       BOOL __stdcall, (UnitAny *pUnit, DWORD dwStateNo))
D2FUNCPTR(D2COMMON, -0x27BE,   CheckQuestFlag,       BOOL __stdcall, ( void *pQuestData, DWORD dwQuestNo, DWORD dwCheckFlag ))
D2FUNCPTR(D2COMMON, -0x27DA,   CheckItemFlag,        BOOL __stdcall, (UnitAny *pItem, DWORD dwFlagMask, DWORD dwLineno, char *filename) )
D2FUNCPTR(D2COMMON, -0x28CC,   CheckCorpseCanUse,    BOOL __stdcall, (UnitAny *pMon,DWORD flag))

D2FUNCPTR(D2COMMON, -0x277B,   GetItemValue,         int __stdcall, (UnitAny * pPlayer, UnitAny * pItem, DWORD dwDifficulty, void* questinfo, int value, DWORD dwFlag))
D2FUNCPTR(D2COMMON, -0x2B09,   GetCursorItem,        UnitAny * __stdcall, (UnitInventory * ptInventory))//pUnit->pInventory->pCursorItem
D2FUNCPTR(D2COMMON, -0x28DC,   GetFirstItemInInv,    UnitAny * __stdcall, (UnitInventory* ptInventory)) // pUnit->pInventory->pFirstItem
D2FUNCPTR(D2COMMON, -0x28E0,   GetNextItemInInv,     UnitAny * __stdcall, (UnitAny *pItem))
D2FUNCPTR(D2COMMON, -0x2958,   GetBoxType,           DWORD __stdcall, (UnitAny * pUnit, BYTE invType, DWORD dwExpansion));//箱子类型
D2FUNCPTR(D2COMMON, -0x2B8C,   CheckInvLocation,     int __stdcall, (UnitInventory* pInventory, UnitAny *pItem, DWORD dwCursorGridX, DWORD dwCursorGridY, DWORD dwBoxType, int* GridCount, int* ItemCount, BYTE invType))

D2FUNCPTR(D2COMMON, -0x27DF,   GetDrlgLevel,         DrlgLevel * __fastcall, (DrlgMisc *drlgmisc, DWORD levelno))//分配一个新的DrlgLevel结构,存于服务端
D2FUNCPTR(D2COMMON, -0x2852,   InitDrlgLevel,        void __stdcall, (DrlgLevel *drlglevel))
D2FUNCPTR(D2COMMON, -0x2A4A,   GetLevelIdFromRoom,   DWORD __stdcall, (DrlgRoom1 *pDrlgRoom1))

D2FUNCPTR(D2COMMON, -0x29FE,   GetUnitPosX,          DWORD __stdcall, (DynamicPath *pPath) ) //(Players, Monsters, Missiles) 
D2FUNCPTR(D2COMMON, -0x2A73,   GetUnitPosY,          DWORD __stdcall, (DynamicPath *pPath) ) //(Players, Monsters, Missiles) 

D2FUNCPTR(D2COMMON, -0x289F,   GetMinExpToLevel,     int __stdcall, (DWORD dwCharClass, DWORD dwLevel))
D2FUNCPTR(D2COMMON, -0x2AEB,   EncodeItem, DWORD __stdcall, (UnitAny *pUnit,char *buf,int size,int a,int b,int c) ) 
D2FUNCPTR(D2COMMON, -10458,   GetMercInfo, DWORD __stdcall, (int seed,int arg,int diff,void *buf)) //ecx=EXPANISION edx=PLAYER 

D2FUNCPTR(D2COMMON, -10839,   UnitVisionBlocked, DWORD __stdcall, (UnitAny *playerUnit,UnitAny *pUnit,int mode)) //mode=2

D2VARPTR(D2COMMON,  0x6FDEFBA0, WeaponsTxts,         int)
D2VARPTR(D2COMMON,  0x6FDEFBA8, ArmorTxts,           int)
D2VARPTR(D2COMMON,  0x6FDE9E1C, DataTables,        DWORD)
D2VARPTR(D2COMMON,  0x6FDEFBD4, RuneWords,         DWORD)
D2VARPTR(D2COMMON,  0x6FDEFBD8, RuneWordTxt,	   DWORD)

D2FUNCPTR(D2GFX,  -0x2740,   GetHwnd,                HWND __stdcall, ())
D2FUNCPTR(D2GFX,  -0x271E,   DrawRectangle,          void __stdcall, (long dwXstart, long dwYstart, long dwXend, long dwYend, DWORD dwPal, DWORD dwTransLvl))
D2FUNCPTR(D2GFX,  -0x271A,   DrawLine,               void __stdcall, (long x1, long y1, long x2, long y2, DWORD dwColor, DWORD dwTransLvl))
D2FUNCPTR(D2GFX,  -10041,   DrawCellFile,           void __stdcall, (CellContext *context, DWORD xPos, DWORD yPos, DWORD dw1, DWORD dwTransLvl, BYTE *coltab))
D2FUNCPTR(D2GFX,  -10079,   DrawAutomapCell,        void __stdcall, (CellContext *context, DWORD xpos, DWORD ypos, RECT *cliprect, DWORD dwTransLvl))
D2FUNCPTR(D2GFX,  -0x274B,   MinimizeWindows,        void __stdcall, (DWORD dwFlag))

D2VARPTR(D2GFX,  0x6FA9D66C, WinState,			DWORD)

D2FUNCPTR(D2WIN,  -10150,   DrawText,               void __fastcall, (wchar_t *wStr, long xPos, long yPos, DWORD dwColor, DWORD dwAlign))
D2FUNCPTR(D2WIN,  -0x2765,   DrawHoverText,          void __fastcall, (wchar_t *wStr, long xPos, long yPos, DWORD dwTran, DWORD dwColor))
D2FUNCPTR(D2WIN,  -0x2756,   DrawUnitLifeBar,        DWORD __fastcall, (wchar_t *wStr ,long xPos, long yPos, DWORD dwColor, DWORD dwUnk1 ,DWORD dwUnk2) )
D2FUNCPTR(D2WIN,  -0x277E,   DrawHover,              void __fastcall, ()) //真正显示字符框的地方，上面3个只是设置了变量,显示优先级非常高

D2FUNCPTR(D2WIN,  -0x27C1,   GetTextAreaSize,        DWORD __fastcall, (wchar_t *wStr, DWORD* pWidth, DWORD* pHeight)) 
D2FUNCPTR(D2WIN,  -0x27C8,   SetTextFont,            DWORD __fastcall, (DWORD dwFont))
D2FUNCPTR(D2WIN,  -0x272C,   GetTextPixelLen,        DWORD __fastcall, (wchar_t *wStr) )//这个函数不能分辨颜色代码，只能用于无色字符串

D2FUNCPTR(D2WIN,  -0x27B6,   GetEditBoxText,         wchar_t* __fastcall, (D2EditBox* box))
D2FUNCPTR(D2WIN,  -0x2725,   SelectEditBoxText,      void __fastcall,  (D2EditBox* box))
D2FUNCPTR(D2WIN,  -0x2750,   AddEditBoxChar,         DWORD __fastcall, (D2EditBox* box, BYTE keycode))
D2FUNCPTR(D2WIN,  -0x2740,   DestroyEditBox,         DWORD __fastcall, (D2EditBox* box))
D2FUNCPTR(D2WIN,  -0x2742,   DestroyListBox,         void __stdcall,   (D2EditBox* box))

D2FUNCPTR(D2WIN,  -0x2749,   CreateEditBox,          D2EditBox* __fastcall, (DWORD dwPosX, DWORD dwPosY, DWORD dwSizeX, DWORD dwSizeY, DWORD dwOffsetX, DWORD dwOffsetY, CellFile *pCellFile, DWORD dwReturnHandler, DWORD arg9, DWORD dwIsCloaked, void* buf))
D2FUNCPTR(D2WIN,  -10042,   SetEditBoxText,         void* __fastcall, (D2EditBox* box, wchar_t* wcszTxt))
D2FUNCPTR(D2WIN,  -10170,   SetEditBoxProc,         void __fastcall, (D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,DWORD,char*)))

D2ASMPTR(D2WIN,  0x6F8E7720, ToggleSound)

D2VARPTR(D2WIN,   0x6F9014B0, FocusedControl,        D2EditBox*)
D2VARPTR(D2WIN,   0x6F9014C0, RunningFlag,        int)

D2FUNCPTR(D2CMP,  -0x2751,   DeleteCellFile,         void __stdcall, (CellFile *cellfile))
D2FUNCPTR(D2CMP,  -0x2716,   InitCellFile,           void __stdcall, (void *cellfile, CellFile **outptr, char *srcfile, DWORD lineno, DWORD filever, char *filename))

D2FUNCPTR(D2LANG, -0x2713,   GetLocaleText,          wchar_t* __fastcall, (DWORD dwLocaleTxtNo))

D2VARPTR(D2NET,  0x6FBFB244, UnkNetFlag,	   DWORD)
D2FUNCPTR(D2NET,  -0x2728,   SendPacket,             void __stdcall, (size_t len, DWORD arg1, BYTE* buf))

D2VARPTR(BNCLIENT,  0x6FF3F64C, BnChatMessage,     LPDWORD )

//Single player data:
//DLLBASE_D2GAME = 0x6FC20000,
//D2Game.dll+E03D9 - 8B 04 85 14B0AB00     - mov eax,[eax*4+D2Game.dll+FB014]
D2VARPTR(D2GAME,0x6FD1B014, SPUnitOffsets,		int)
D2FUNCPTR(D2GAME,0x6FC2A364, GetSinglePlayerContext1,  int __stdcall, (int))
D2FUNCPTR(D2GAME,0x6FC4AAE0, GetSinglePlayerContext2,  int __stdcall, (int))
D2FUNCPTR(D2GAME,0x6FD003A0, GetSinglePlayerUnit,  int __fastcall, (int,int))
D2FUNCPTR(D2GAME,0x6FD0C7F0, MakeD2S,  int __fastcall, (UnitAny *pUnit,int context,char *buf,int *size,int cap,int zero,int one))


D2ASMPTR(D2CLIENT,  0xEEEEEEEE, EndOfPtr)

D2ASMPTR(D2CLIENT,  0xEEEEEE00, StartOfPtr2)

D2VARPTR(D2MULTI,  0x6FA09CC0, GameListControl,       D2EditBox *)
D2VARPTR(D2MULTI,  0x6F9E9C60, EditboxPreferences,    void *)  //6F9E9C5C+4

D2ASMPTR(D2CLIENT,  0xEEEEEEEE, EndOfPtr2)


#undef D2FUNCPTR
#undef D2VARPTR
#undef D2ASMPTR



