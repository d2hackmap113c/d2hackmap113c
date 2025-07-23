#ifdef INIT_D2PTR_ADDR
  #define DLL_VAR(dll,off1,name1,t1)     dll##_p##name1=(t1 *)(off1+(DWORD)addr_##dll);
  #define DLL_FUN(dll,off1,name1,t1,t2,t3) dll##_##name1=(t1 (t2 *) t3)(off1+(DWORD)addr_##dll);
  #define DLL_ORD(dll,off1,idx1,name1,t1,t2,t3) dll##_##name1=(t1 (t2 *) t3)GetProcAddress(addr_##dll,(LPCSTR)idx1);
#else
	#ifdef DECLARE_D2PTR_VARIBLE
		#define DLL_VAR(dll,off1,name1,t1)     t1 *dll##_p##name1;
		#define DLL_FUN(dll,off1,name1,t1,t2,t3) t1 (t2 * dll##_##name1) t3;
		#define DLL_ORD(dll,off1,idx1,name1,t1,t2,t3) t1 (t2 * dll##_##name1) t3;
	#else
		#ifdef DUMP_D2PTR
			#define DLL_VAR(dll,off1,name1,t1) dumpPtr(fp,#dll,(int)addr_##dll,off1,#name1,(int)dll##_p##name1);
			#define DLL_FUN(dll,off1,name1,t1,t2,t3) dumpPtr(fp,#dll,(int)addr_##dll,off1,#name1,(int)dll##_##name1);
			#define DLL_ORD(dll,off1,idx1,name1,t1,t2,t3) dumpPtr(fp,#dll,(int)addr_##dll,off1,#name1,(int)dll##_##name1);
		#else
			#define DLL_VAR(dll,off1,name1,t1)     extern t1 *dll##_p##name1;
			#define DLL_FUN(dll,off1,name1,t1,t2,t3) extern t1 (t2 * dll##_##name1) t3;
			#define DLL_ORD(dll,off1,idx1,name1,t1,t2,t3) extern t1 (t2 * dll##_##name1) t3;
			#define DEFINE_D2PTR
		#endif
	#endif
#endif

#ifdef DEFINE_D2PTR
struct FogBits {
	char *outptr; //+00
	int max_bits; //+04
	int n_bytes; //+08
	int n_bits; //+0C 0-7
	int off10; //+10 error
};
struct InvLayout {char cols,rows,uk[2];int x0,x1,y0,y1;char gridW,gridH,uk2[2];};
struct EquipmentLayout20 {int x0,x1,y0,y1;char w,h,uk[2];};
struct EquipmentLayout16 {int x0,x1,y0,y1;char w,h,uk[2];};
typedef struct {
	int off0,off4;
	char path[256]; //+08
	int off108,off10c; //+108
} mpq_block;
#endif
DLL_VAR(bnclient,0x1F64C,BnChatMessage,LPDWORD )
DLL_FUN(d2client,0x17F0,getSkillDescTxt,void *,__fastcall,()) //eax:int skillId
DLL_FUN(d2client,0x7E60,subEsp,int,__stdcall,()) //eax:int size
DLL_FUN(d2client,0xBE84,Storm511,void,__stdcall, (D2MSG *pMsg))
DLL_FUN(d2client,0xC3DC,getNearbyRectData,void,__stdcall, (AreaRectData *pAreaRectData,AreaRectData ***pDataNear,int *n))
DLL_FUN(d2client,0x10DF0,SetResolutionMode,void,__stdcall, ())
DLL_FUN(d2client,0x13CE0,drawGroundPiece,void,__stdcall,(int arg1,int arg2,int arg3,int arg4,int arg5,int arg6))
DLL_FUN(d2client,0x14050,drawGround,int,__stdcall,(void *arg1))
DLL_FUN(d2client,0x143E0,SendPacket43E0,void,__stdcall,(char *packet)) //ebx=len
DLL_FUN(d2client,0x148E0,sendPacketLen13,void,__fastcall,(int arg1,int arg2,int arg3)) //eax:char cmd
DLL_FUN(d2client,0x14A10,SendPacket3,void,__fastcall,(short param)) //al=cmd
DLL_FUN(d2client,0x185E0,ShouldClick85E0,int,__stdcall, ()) //ecx=buf
DLL_FUN(d2client,0x1BC40,RightClickBC40,int,__stdcall, ()) //esi=buf
DLL_FUN(d2client,0x1BDE0,LeftClickBDE0,int,__stdcall, ()) //esi=buf
DLL_FUN(d2client,0x216A0,GetMonsterOwner,int,__fastcall, (int dwUnitId))
DLL_FUN(d2client,0x23200,ItemProtect,void,__stdcall, (UnitAny *pUnit, DWORD dwType))
DLL_FUN(d2client,0x23F20,PlaySound,int,__fastcall, (int unitId,int unitType,int sndId))
DLL_FUN(d2client,0x27590,drawHpManaText,int,__stdcall,())
DLL_FUN(d2client,0x27BD0,fillLeftOrb,int,__stdcall,())
DLL_FUN(d2client,0x28BC0,DrawSkillButton,void,__stdcall, (Skill *pSkill,int x,int y,int isLeft))
DLL_FUN(d2client,0x29250,DrawClient,void,__fastcall, ())
DLL_FUN(d2client,0x29990,UseBelt,void,__fastcall,(UnitInventory *pInventory,UnitAny *player,int id)) //edx:PLAYER ecx:PLAYER->pInventory eax:0x8000(shift)or0
DLL_FUN(d2client,0x2A750,setState_2A750,void,__fastcall,(int,int,int,int))
DLL_FUN(d2client,0x2B420,LoadUiImageFunc,CellFile*,__stdcall,(int)) //eax:path
DLL_FUN(d2client,0x3B8B0,ShowMap,void,__fastcall, ())
DLL_FUN(d2client,0x3CB00,processMessages,void,__fastcall, ()) //ebx:int (*processGame)()
DLL_FUN(d2client,0x3CDC0,processWinMessage,int,__stdcall,(HWND hwnd,int msg,int wParam,int lParam))
DLL_FUN(d2client,0x3DA60,SkillUnusable,void,__fastcall, ()) //eax(pSkill),esi(playerUnit)
DLL_FUN(d2client,0x3E5E0,DrawSkillButtonE5E0,void,__stdcall,(int a,int b,int c,int d,int e,int isLeft))
DLL_FUN(d2client,0x3F6C0,GetScreenDrawX,long,__stdcall, ())
DLL_FUN(d2client,0x3F6D0,GetScreenDrawY,long,__stdcall, ())
DLL_FUN(d2client,0x46150,Unknown6150,int,__stdcall, ())
DLL_FUN(d2client,0x483B0,drawNpcInteractMenu,int,__stdcall,())
DLL_FUN(d2client,0x48870,clickGotoAct3MenuItem,int,__stdcall,())
DLL_FUN(d2client,0x488D0,clickGotoAct2MenuItem,int,__stdcall,())
DLL_FUN(d2client,0x49440,clickHireMercMenu,int,__stdcall,())
DLL_FUN(d2client,0x4B8A0,fun_4B8A0,int,__stdcall,(int off2,short *quest,char off6))
DLL_FUN(d2client,0x51A80,GetSelectedUnit,UnitAny *,__stdcall, ())
DLL_FUN(d2client,0x59980,clickAcceptTradeMenuItem,int,__stdcall,())
DLL_FUN(d2client,0x59ED0,recvCmd77ButtonAction,void,__stdcall, ()) //eax:packet
DLL_FUN(d2client,0x5C7F0,drawAllHeads,int,__stdcall,())
DLL_FUN(d2client,0x5F6B0,NewAutomapNode,AutomapNode *,__fastcall, ())
DLL_FUN(d2client,0x5F8A0,SetAutomapNamesStub ,void,__fastcall, (DWORD flag))    //parm = flag  ==> esi 
DLL_FUN(d2client,0x5F8D0,SetAutomapPartyStub	,void,__fastcall, (DWORD flag))    //parm = flag  ==> esi 
DLL_FUN(d2client,0x61320,AddAutomapNode,void,__fastcall, (AutomapNode *cell, AutomapNode **node))
DLL_FUN(d2client,0x62580,AddAreaRectToMinimap,void,__stdcall, (AreaRectData *pData, DWORD clipflag, AutomapLayer *layer))
DLL_FUN(d2client,0x62710,InitAutomapLayerStub,AutomapLayer*,__fastcall, (DWORD nLayerNo)) //nLayerNo==>eax 设置当前Layer指针,如果对应nLayerNo未生成,则分配一个，存于客户端
DLL_FUN(d2client,0x66620,shouldDrawUnit,int,__stdcall,()) //esi:UnitAny *pUnit
DLL_FUN(d2client,0x666D0,drawUnit,int,__stdcall,(void *arg1)) //ecx:UnitAny *pUnit
DLL_FUN(d2client,0x6B5A0,drawInvItem,int,__stdcall,(int x,int y)) //ebx:UnitAny *pItem
DLL_FUN(d2client,0x70C45,ChatInputStub ,void,__stdcall, (D2MSG *pmsg))  //与入口偏移了5个长度,使用自定义的eax
DLL_FUN(d2client,0x77C50,getSkillTreeButtonX,int,__fastcall,()) //al=col 1-3
DLL_FUN(d2client,0x77CA0,getSkillTreeButtonY,int,__fastcall,()) //al=row 1-7
DLL_FUN(d2client,0x7D610,ShowPartyMessage,void,__stdcall, (wchar_t* text, int nColor))
DLL_FUN(d2client,0x7D850,ShowGameMessage,void,__stdcall, (wchar_t* text, int nColor))
DLL_FUN(d2client,0x7E970,isNpcConversitionEnd,int,__fastcall,(void* arg))
DLL_FUN(d2client,0x7EA90,drawNpcConversition,int,__fastcall,()) //eax:void *npcConv
DLL_FUN(d2client,0x7FC70,drawEnvironment,void,__stdcall,(void *setting,void *b))
DLL_FUN(d2client,0x84750,packet9C_4,void,__fastcall,()) //eax:char *packet
DLL_FUN(d2client,0x86A40,useStackableItem,void,__stdcall,(char *packet))
DLL_FUN(d2client,0x8AFD0,CalcShake,void,__stdcall, (int *pPosX, int *pPosY))
DLL_FUN(d2client,0x8B110,DrawAllUnits,int,__stdcall, ())
DLL_FUN(d2client,0x958A0,func_958A0,int,__stdcall,(UnitAny *pUnit))
DLL_FUN(d2client,0x99440,drawInventory,int,__stdcall,())
DLL_FUN(d2client,0x9EE90,drawUnits9EE90,int,__stdcall,(void *arg1))
DLL_FUN(d2client,0xA3840,drawFloatingWindow,int,__stdcall,()) //esi:D2FloatingWindow *win
DLL_FUN(d2client,0xA3C10,processMessageA3C10,int,__stdcall,(D2MSG *msg))
DLL_FUN(d2client,0xA54A0,CanUseSkillNow,int,__stdcall, (int skillId)) //eax:pUnit
DLL_FUN(d2client,0xA5B40,GetUnitFromId,UnitAny *,__fastcall,(int dwUnitId,int dwUnitType))
DLL_FUN(d2client,0xA6500,addUnitToHash,int,__stdcall,())//esi:UnitAny *pUnit
DLL_FUN(d2client,0xA6EF0,removeUnitFromHash,int,__fastcall,(UnitAny **hash,int hashpos,int type,int id))
DLL_FUN(d2client,0xAC3D0,RecvCommand08,void,__fastcall, (BYTE *cmdbuf))
DLL_FUN(d2client,0xAC440,RecvCommand07,void,__fastcall, (BYTE *cmdbuf))
DLL_FUN(d2client,0xACE20,PacketHandler,void,__stdcall, (int len)) //eax:char *buf
DLL_FUN(d2client,0xAE080,RecvPacket9C,void,__fastcall,(char *packet))
DLL_FUN(d2client,0xAE870,RecvPacket9D,void,__fastcall,(char *packet))
DLL_FUN(d2client,0xAF560,RecvPacket0E,void,__fastcall,(int arg,char *packet))
DLL_FUN(d2client,0xAF910,RecvPacket3F,void,__fastcall,(char *packet))
DLL_FUN(d2client,0xBE400,CheckUiStatusStub,int,__fastcall, (int dwUiNo))//dwUiNo ==> eax
DLL_FUN(d2client,0xBF6C0,LoadUiImageSubpath,CellFile*,__fastcall,()) //esi=subpath data/global/ui/%s
DLL_FUN(d2client,0xC2790,SetUiStatus,int,__fastcall, (int dwUiNo, int howset, DWORD unknown1))
DLL_FUN(d2client,0xC39E0,DrawControls,int,__stdcall, (int unknown1))
DLL_VAR(d2client,0xCE474,sgptDataTables,D2DataTables *) 
DLL_VAR(d2client,0xDBC48,ScreenWidth,int ) //屏幕宽度X 800*600   640*480
DLL_VAR(d2client,0xDBC48,ScreenSize,POINT)  //屏幕宽度X 800*600   640*480
DLL_VAR(d2client,0xDBC4C,ScreenHeight,int ) //屏幕宽度Y 800*600   640*480
DLL_VAR(d2client,0xDBC54,LastMousePos,POINT )//最后的鼠标位置
DLL_VAR(d2client,0xE0EB8,CursorInvGridX,int)//最后的物品栏位置
DLL_VAR(d2client,0xE0EBC,CursorInvGridY,int)//最后的物品栏位置
DLL_VAR(d2client,0xE24F8,CurSkillClass,int ) 
DLL_VAR(d2client,0xF16B0,MinimapZoom,int)
DLL_VAR(d2client,0xF23E8,MercData16,int ) 
DLL_VAR(d2client,0xF4F10,GameControlStart,int) //游戏控制函数
DLL_VAR(d2client,0xF4F14,GameControlEnd,int) //游戏控制函数
DLL_VAR(d2client,0xF8C9C,ExitAppFlag,int)
DLL_VAR(d2client,0xF9E14,InfoPositionX,int) //游戏信息显示X坐标
DLL_VAR(d2client,0xFAD60,UiDropGoldWindow,void *)  //打开丢下金币窗口时此值不为零
DLL_VAR(d2client,0xFAD84,UiInventoryOn,int) 
DLL_VAR(d2client,0xFAD88,UiCharacterOn,int) 
DLL_VAR(d2client,0xFAD8C,UiSelectSkillOn,int) 
DLL_VAR(d2client,0xFAD90,UiSkillTreeOn,int) 
DLL_VAR(d2client,0xFAD94,UiChatOn,int) 
DLL_VAR(d2client,0xFAD98,UiNewStatOn,int) 
DLL_VAR(d2client,0xFAD9C,UiNewSkillOn,int) 
DLL_VAR(d2client,0xFADA0,UiInteractOn,int) 
DLL_VAR(d2client,0xFADA4,UiGameMenuOn,int) 
DLL_VAR(d2client,0xFADA8,AutomapOn,int) //小地图是否打开
DLL_VAR(d2client,0xFADB0,UiNpcTradeOn,int) 
DLL_VAR(d2client,0xFADB4,UiShowItemsOn,int) 
DLL_VAR(d2client,0xFADB8,UiModItemOn,int) 
DLL_VAR(d2client,0xFADBC,UiQuestOn,int) 
DLL_VAR(d2client,0xFADC4,UiNewQuestOn,int) 
DLL_VAR(d2client,0xFADD0,UiWaypointOn,int) 
DLL_VAR(d2client,0xFADD8,UiPartyOn,int) 
DLL_VAR(d2client,0xFADDC,UiPPLTradeOn,int) 
DLL_VAR(d2client,0xFADE0,UiMessageLogOn,int) 
DLL_VAR(d2client,0xFADE4,UiStashOn,int) 
DLL_VAR(d2client,0xFADE8,UiCubeOn,int) 
DLL_VAR(d2client,0xFADFC,UiBeltOn,int) 
DLL_VAR(d2client,0xFAE04,UiHelpOn,int) 
DLL_VAR(d2client,0xFCD8C,WaypointMenuItems,struct WaypointMenuItem) 
DLL_VAR(d2client,0xFCDD6,WaypointMenuCurTab,int ) //classic(0-3) expansion(0-4)
DLL_VAR(d2client,0xFCDDA,WaypointMenuCount,int ) //act1:9 act4:3
DLL_VAR(d2client,0x10A608,UnitTable,UnitAny * ) //type*128+(id&0x7f)
DLL_VAR(d2client,0x10B9DC,MapShakeY,long)
DLL_VAR(d2client,0x113880,AutomapNodeListBaseAddr,void *)
DLL_VAR(d2client,0x11973B,QuestData,QuestInfo *) 
DLL_VAR(d2client,0x11973F,GameQuestData,QuestInfo *)
DLL_VAR(d2client,0x1197AC,TradeRelated,int *) 
DLL_VAR(d2client,0x1197F8,CurFrame,int)
DLL_VAR(d2client,0x119804,Ping,int )
DLL_VAR(d2client,0x119854,Expansion,int ) // 1资料片 0 非资料片
DLL_VAR(d2client,0x11985C,CastingDelayEndFrame,int)
DLL_VAR(d2client,0x11995C,screenDrawY,int)
DLL_VAR(d2client,0x119960,screenDrawX,int)
DLL_VAR(d2client,0x119D30,SkillHotKeys,int ) 
DLL_VAR(d2client,0x119E20,SkillHotKeyLeft,int ) 
DLL_VAR(d2client,0x11A2E0,KeyboardSettingStart,struct KeyboardSetting)
DLL_VAR(d2client,0x11A754,KeyboardSettingEnd,struct KeyboardSetting)
DLL_VAR(d2client,0x11A768,XButton2,BOOL )
DLL_VAR(d2client,0x11A76C,XButton1,BOOL )
DLL_VAR(d2client,0x11A7C8,PanelInvChar,CellFile *)
DLL_VAR(d2client,0x11A7E0,PanelBtn,CellFile *)
DLL_VAR(d2client,0x11B824,MouseY,int )//鼠标位置
DLL_VAR(d2client,0x11B828,MouseX,int )//鼠标位置
DLL_VAR(d2client,0x11B980,GameInfo,GameStructInfo *)
DLL_VAR(d2client,0x11B99C,Is800x600,int)
DLL_VAR(d2client,0x11B9A0,DrawOffset,POINT )//显示框偏移量
DLL_VAR(d2client,0x11B9A0,DrawOffsetX,int)
DLL_VAR(d2client,0x11B9A4,DrawOffsetY,int)
DLL_VAR(d2client,0x11BBB0,GoldInTranBox,int)
DLL_VAR(d2client,0x11BBFC,PlayerUnit,UnitAny *)
DLL_VAR(d2client,0x11BC00,npcInteract,void *)
DLL_VAR(d2client,0x11BC14,RosterUnitList,RosterUnit *) //玩家列表
DLL_VAR(d2client,0x11BC38,CurrentViewItem,UnitAny*) //选择显示的物品
DLL_VAR(d2client,0x11BC48,NpcTradeCurrentTab,int) //Npc trade current tab
DLL_VAR(d2client,0x11BC74,PanelBank,CellFile *)
DLL_VAR(d2client,0x11BC78,PanelCube,CellFile *)
DLL_VAR(d2client,0x11BC84,PanelTradeStash,CellFile *)
DLL_VAR(d2client,0x11BC84,StachImage,CellFile *) 
DLL_VAR(d2client,0x11BC94,ActiveWeapon,int) 
DLL_VAR(d2client,0x11BEFC,IsMapShakeOn,BOOL)
DLL_VAR(d2client,0x11BF00,MapShakeX,long)
DLL_VAR(d2client,0x11C028,ChatTextLength,int)
DLL_VAR(d2client,0x11C1B0,MinimapType,int) // 1小地图 0 大地图
DLL_VAR(d2client,0x11C1C0,AutomapLayerList,AutomapLayer *) //所有AutomapLayer列表
DLL_VAR(d2client,0x11C1C4,AutomapLayer,AutomapLayer *) //当前所在的AutomapLayer
DLL_VAR(d2client,0x11C1C8,MinimapCellFile,struct CellFile *) 
DLL_VAR(d2client,0x11C1E8,AutomapPos,POINT)
DLL_VAR(d2client,0x11C1F8,ScreenMinimapX,int)
DLL_VAR(d2client,0x11C1FC,ScreenMinimapY,int)
DLL_VAR(d2client,0x11C21C,InfoPositionY,int) //游戏信息显示Y坐标
DLL_VAR(d2client,0x11C228,MinimapOffset,POINT) //小地图边缘地址 ，大地图为 -16*-16
DLL_VAR(d2client,0x11C230,AutoMapSize,POINT) //地图大小
DLL_VAR(d2client,0x11C2AC,Fps,int )
DLL_VAR(d2client,0x11C2F4,HasSelectedObject,BOOL ) 
DLL_VAR(d2client,0x11C2F8,FlagUnknownC2F8,BOOL ) 
DLL_VAR(d2client,0x11C334,MercData32,int ) 
DLL_VAR(d2client,0x11C390,Difficulty,BYTE) //当前难度 0 1 2
DLL_VAR(d2client,0x11C3A0,InGame,BOOL ) //是否在游戏中?
DLL_VAR(d2client,0x11C3A0,MButton,BOOL )
DLL_VAR(d2client,0x11C3A4,notAcceptKeys,int ) //not accept input?
DLL_VAR(d2client,0x11C3B8,DrlgAct,DrlgAct *) //current act
DLL_VAR(d2client,0x11C3C4,NpcConvData,void *)
DLL_VAR(d2client,0x11C3F0,StandStill,BOOL ) //是否站立不动
DLL_VAR(d2client,0x11C414,ScreenBlocked,int) //bit0 right half blocked, bit 1 left half blocked
DLL_VAR(d2client,0x11C418,screenDrawXsub,int)
DLL_VAR(d2client,0x11C484,BeltNotEmpty,char) 
DLL_VAR(d2client,0x11C4B0,ShowLifeStr,BOOL)
DLL_VAR(d2client,0x11C4B4,ShowManaStr,BOOL)
DLL_VAR(d2client,0x11C4D4,PetUnitList,PetUnit * ) //玩家召唤物列表
DLL_VAR(d2client,0x11EC80,LastChatMessage,wchar_t)  
DLL_VAR(d2client,0x122360,worldHash,WorldHash *)
DLL_VAR(d2client,0x1223B0,lpCritialSection,CriticalSection *)
DLL_VAR(d2client,0x123395,QuestPage,int)
DLL_VAR(d2client,0x123424,IsEquipment800x600,int)
DLL_ORD(d2cmp,0x11520,10065,DeleteCellFile,void,__stdcall, (CellFile *cellfile))
DLL_ORD(d2cmp,0x11AC0,10006,InitCellFile,void,__stdcall, (void *cellfile, CellFile **outptr, char *srcfile, int lineno, int filever, char *filename))
DLL_FUN(d2cmp,0x13010,mpq_path_3010,int,__fastcall, (char *path))
DLL_FUN(d2common,0x12D0,GetCharStatsTxt,void *,__stdcall,()) //eax=charClass
DLL_FUN(d2common,0x1330,GetNearRectDataWithPos,AreaRectData *,__fastcall, ())//eax=pData,ebx=x,edi=y
DLL_FUN(d2common,0x13B0,GetMonStats2,void *,__fastcall, ())//eax=dwTxtFileNo
DLL_FUN(d2common,0x1B0D,exit,int,__stdcall,(int exitcode))
DLL_FUN(d2common,0x5AD0,subEsp,int,__stdcall,()) //eax=size
DLL_ORD(d2common,0x1DE90,10508,getFilledSockets,int,__stdcall,(UnitInventory * pInventory))
DLL_ORD(d2common,0x1DFB0,11017,GetCursorItem,UnitAny *,__stdcall, (UnitInventory * ptInventory))//pUnit->pInventory->pCursorItem
DLL_ORD(d2common,0x1E190,10460,GetFirstItemInInv,UnitAny *,__stdcall, (UnitInventory* ptInventory)) // pUnit->pInventory->pFirstItem
DLL_FUN(d2common,0x1E1B0,createEquippedItem,EquippedGears *,__stdcall,()) //eax:int infoId,ebx:UnitInventory * pInv
DLL_ORD(d2common,0x1E670,11139,getEquippedItemInfo,int,__stdcall,(UnitInventory * pInventory,int bodyLoc)) //1<=bodyLoc<=12
DLL_ORD(d2common,0x1E710,10387,createInventory,UnitInventory *,__stdcall,(void *pMemPool,UnitAny *pUnit))
DLL_FUN(d2common,0x1E790,getOwnerInventory,UnitInventory *,__stdcall,()) //eax:UnitAny *pItem, return pItem->pItemData->pOwnerInventory
DLL_ORD(d2common,0x1E8F0,10464,GetNextItemInInv,UnitAny *,__stdcall, (UnitAny *pItem))
DLL_ORD(d2common,0x1F1F0,11148,CheckInvLocation,int,__stdcall,(UnitInventory *pInventory,UnitAny *pItem,int dwCursorGridX,int dwCursorGridY,int dwBoxType,int *GridCount,int *ItemCount,BYTE invType))
DLL_ORD(d2common,0x1F6E0,10391,isItemInside,int,__stdcall,(UnitInventory* pInventory,UnitAny *pItem))
DLL_ORD(d2common,0x21790,10466,setCursorItem,int,__stdcall,(UnitInventory * pInventory,UnitAny *pItem))
DLL_ORD(d2common,0x217F0,10405,putItemIntoSocket10405,int,__stdcall,(UnitInventory *pInv,UnitAny *pItemToPutInSocket,int one)) //return 1 if ok
DLL_ORD(d2common,0x21FE0,10197,isBitSet,int,__stdcall,(int value,int bitmask))
DLL_ORD(d2common,0x22170,10503,setItemFormat,void,__stdcall,(UnitAny *pItem,short wItemFormat))
DLL_ORD(d2common,0x221C0,10718,getItemFormat,int,__stdcall,(UnitAny *pItem))
DLL_FUN(d2common,0x222B0,isCompactItem,char,__fastcall,()) //eax=pItem getItemTxt143
DLL_ORD(d2common,0x22510,10386,setOwnerId,void,__stdcall,(UnitAny * pItem,int ownerId))
DLL_ORD(d2common,0x22590,10572,getItemData1C_20,void,__stdcall,(UnitAny * pItem,void *off1c,void *off20))
DLL_ORD(d2common,0x225F0,10536,setItemFileIndex,int,__stdcall,(UnitAny *pItem,int index))
DLL_ORD(d2common,0x22640,10620,GetItemFileIndex,int,__stdcall, (UnitAny *pItem)) //pItem->pItemData->dwFileIndex
DLL_ORD(d2common,0x22BC0,10176,isQuestItem,int,__stdcall,(UnitAny *pItem)) //*(char *)(ItemTxt+12A)
DLL_ORD(d2common,0x22F80,10108,getItemTypeTxt10,char,__stdcall,(int typeId))
DLL_ORD(d2common,0x22FF0,10059,getItemCode1,char *,__stdcall, (UnitAny * pItem))//txt+84
DLL_ORD(d2common,0x230A0,10082,getItemTypeByTxt,int,__stdcall,(int txt)) //*(short *)(ItemTxt+11E)
DLL_ORD(d2common,0x230F0,11088,getItemType,int,__stdcall,(UnitAny *pItem)) //*(short *)(ItemTxt+11E)
DLL_ORD(d2common,0x231A0,10114,getItemWidthHeight,int,__stdcall,(UnitAny *pItem,char *pWidth,char *pHeight))
DLL_ORD(d2common,0x23290,10802,getItemCode,char *,__stdcall, (UnitAny * pItem))
DLL_ORD(d2common,0x233B0,10710,getItemTypeB14,int,__stdcall,(UnitAny *pItem)) //*(char *)(ItemTypeTxt+14)
DLL_ORD(d2common,0x237A0,10012,setItemLocation,int,__stdcall,(UnitAny *pItem,char location)) //-1 equiped 0 Inventory, 3 cube,4 stash  
DLL_ORD(d2common,0x237E0,10377,setItemLevel,int,__stdcall,(UnitAny *pItem,int itemLvl))
DLL_ORD(d2common,0x238E0,10838,getItemFlag,void,__stdcall,(UnitAny *pItem))
DLL_ORD(d2common,0x23900,10977,setItemFlag,void,__stdcall,(UnitAny *pItem,int flag,int set))
DLL_ORD(d2common,0x23940,10202,CheckItemFlag,BOOL,__stdcall, (UnitAny *pItem, DWORD dwFlagMask, int dwLineno, char *filename))
DLL_ORD(d2common,0x23990,10433,getRareSuffix,short,__stdcall,(UnitAny * pItem))
DLL_ORD(d2common,0x239E0,10380,getRarePrefix,short,__stdcall,(UnitAny * pItem))
DLL_ORD(d2common,0x23A40,11028,getItemMagicSuffix,int,__stdcall,(UnitAny *pItem,int id)) //pItem->pItemData->wMagicSuffix[id]
DLL_ORD(d2common,0x23AF0,10257,getItemMagicPrefix,int,__stdcall,(UnitAny *pItem,int id)) //pItem->pItemData->wMagicPrefix[id]
DLL_ORD(d2common,0x23B20,10875,setItemQuality,void,__stdcall,(UnitAny *pItem,int quality))
DLL_ORD(d2common,0x23B40,10106,getItemQuality,int,__stdcall,(UnitAny *pItem))
DLL_ORD(d2common,0x23B70,10414,setItemFingerPrint,void,__stdcall,(UnitAny *pItem,int fingerPrint))
DLL_ORD(d2common,0x23BC0,10193,getItemFingerPrint,int,__stdcall,(UnitAny *pItem))
DLL_ORD(d2common,0x23C10,10411,getItemSeed,D2Seed *,__stdcall,(UnitAny *pItem))
DLL_FUN(d2common,0x23DD0,writeBitStream,void,__stdcall,(FogBits *bits))//esi:nbits,edx:value(set to [0,2^nbits-1])
DLL_ORD(d2common,0x24210,10516,isMagicItem,int,__stdcall,(UnitAny *pItem)) //quality>=4
DLL_ORD(d2common,0x24430,10744,checkItemTypeFlags,int,__stdcall,(UnitAny *pItem,int id)) //id=[0,127] 45:weapon 50:armor 0xD:charm 0x28:type40 7:ear 16:glove 12:amulet 4:gold
DLL_ORD(d2common,0x24610,11151,getItemMaxSocket,int,__stdcall,(UnitAny *pItem))
DLL_FUN(d2common,0x24F90,countUsedSocketed,int,__stdcall,()) //eax:UnitAny *pItem
DLL_FUN(d2common,0x25C10,EncodeCompactItem,int,__stdcall,(UnitAny *pItem,int header)) //ebx,ebp=FogBits *bits
DLL_ORD(d2common,0x26080,10366,getSocketCount,int,__stdcall, (UnitAny * pItem))
DLL_ORD(d2common,0x26BA0,10918,searchRunewordTxt,RuneWordTxt*,__stdcall, (UnitAny * pItem))
DLL_FUN(d2common,0x27180,EncodeComplexItem,int,__stdcall,(UnitAny *pItem,FogBits *bits,int complete,int onlyClass))
DLL_FUN(d2common,0x28D70,EncodeItemBits,int,__fastcall,(UnitAny *pItem,FogBits *bits,int a,int b,int c)) 
DLL_ORD(d2common,0x29D00,10987,EncodeItem,int,__stdcall, (UnitAny *pItem,char *buf,int size,int complete,int subItems,int classOnly)) 
DLL_ORD(d2common,0x29D60,10107,GetItemPrice,int,__stdcall,(UnitAny * pPlayer,UnitAny *pItem,int dwDifficulty,QuestInfo *questinfo, int value, DWORD dwFlag))
DLL_ORD(d2common,0x2A480,10992,setItemSocket,int,__stdcall,(UnitAny *pItem,int count))
DLL_ORD(d2common,0x2CCC0,10458,GetMercInfo,int,__stdcall, (int seed,int arg,int diff,void *buf)) //ecx=EXPANISION edx=PLAYER 
DLL_ORD(d2common,0x2CF40,10579,copyQuestData,QuestInfo *,__stdcall,(QuestInfo *info,short *dst,int size,int notUsed)) //size must = 96(0x60)
DLL_ORD(d2common,0x2D070,10593,clearQuestFlag,void,__stdcall,(QuestInfo *questData,int questId,int pos))
DLL_ORD(d2common,0x2D0B0,10003,setQuestFlag,void,__stdcall,(QuestInfo *questData,int questId,int pos))
DLL_ORD(d2common,0x2D0F0,10174,CheckQuestFlag,BOOL,__stdcall,(QuestInfo *pQuestData,int qid,int bit))
DLL_ORD(d2common,0x2D9B0,10207,GetDrlgLevel,DrlgLevel *,__fastcall, (DrlgMisc *drlgmisc, int levelno))//Allcate a new DrlgLevel server side
DLL_ORD(d2common,0x2E360,10322,InitDrlgLevel,void,__stdcall, (DrlgLevel *drlglevel))
DLL_FUN(d2common,0x2E5C0,revealRect2E5C0,int,__stdcall,(int tileX,int tileY,DrlgMisc *pDrlgMisc,DrlgLevel *drlglevel)) //eax:AreaRectInfo *playerRectInfo
DLL_ORD(d2common,0x2EA30,10111,setObjectFlags,int,__stdcall,(UnitAny* pUnit, int flags))
DLL_ORD(d2common,0x2EA70,10258,getObjectFlags,int,__stdcall,(UnitAny* pUnit))
DLL_ORD(d2common,0x2EB70,10920,GetPlayerData,PlayerData *,__stdcall,(UnitAny *pUnit))
DLL_ORD(d2common,0x2F270,11118,setStaticUnitY,void,__stdcall,(UnitAny * pUnit,int y))
DLL_ORD(d2common,0x2F2C0,10882,setStaticUnitX,void,__stdcall,(UnitAny * pUnit,int x))
DLL_ORD(d2common,0x2F370,11153,allocUnit,UnitAny *,__stdcall,(int arg0,int unitType))
DLL_ORD(d2common,0x2FB60,10584,GetBoxType,int,__stdcall, (UnitAny * pUnit, BYTE invType, int dwExpansion))//箱子类型
DLL_ORD(d2common,0x2FE10,10331,getRectData,AreaRectData *,__stdcall, (UnitAny *pUnit))
DLL_ORD(d2common,0x30050,10141,getUnitPos,void,__stdcall,(UnitAny *pUnit,POINT *p)) 
DLL_ORD(d2common,0x303A0,10511,getCurrentSkill,Skill *,__stdcall,(UnitAny *pUnit))
DLL_ORD(d2common,0x30460,10828,getLeftSkill,Skill *,__stdcall, (UnitAny *pUnit)) //pUnit->pSkill->pLeftSkill
DLL_ORD(d2common,0x30F80,10589,getUnitSize,int,__stdcall,(UnitAny *pUnit))
DLL_ORD(d2common,0x314A0,10839,IsUnitBlocked,int,__stdcall, (UnitAny *playerUnit,UnitAny *pUnit,int bitmask)) 
DLL_ORD(d2common,0x32330,11027,GetMonsterHpPercent,int,__stdcall,(UnitAny *pMon))//only monster
DLL_ORD(d2common,0x33920,11090,setUnitMode,int,__stdcall,(UnitAny *pUnit, int mode))
DLL_ORD(d2common,0x33CD0,10494,CheckUnitState,BOOL,__stdcall, (UnitAny *pUnit, int dwStateNo))
DLL_ORD(d2common,0x34B80,10750,GetUnitPosY,int,__stdcall, (DynamicPath *pPath)) //(Players, Monsters, Missiles) 
DLL_ORD(d2common,0x34BB0,10867,GetUnitPosX,int,__stdcall, (DynamicPath *pPath)) //(Players, Monsters, Missiles) 
DLL_ORD(d2common,0x37EC0,10930,GetStatListWithFlag,StatList * ,__stdcall, (UnitAny *pUnit, int dwStatNo, DWORD dwListFlag))
DLL_FUN(d2common,0x381A0,removeStat,int,__stdcall,(int)) //esi=(Stat *)pStat,edx=(StatEx *)remove
DLL_FUN(d2common,0x382B0,bsearchStat,int,__stdcall,()) //esi=(Stat *)pStat,edi=(statId<<16)|param,ret=index
DLL_ORD(d2common,0x38710,11110,GetStatFromStatList,BOOL,__stdcall, (StatList *pStatList, DWORD dwStatNo, StatEx *pStatVal ,DWORD dwMask)) 
DLL_ORD(d2common,0x38850,10984,sendStat10984,int,__stdcall,(UnitAny *pUnit1,UnitAny *pUnit2,void *func))
DLL_FUN(d2common,0x38A80,getStatValueWithMinChecked,int,__stdcall,(ItemStatCostTxt *txt))//ebx:StatList *pStatList,edi:(statId<<16)|param,return txt->fmin&&v<txt->minaccr?txt->minacr<<txt->valshift:v;
DLL_FUN(d2common,0x38B00,getUnitStatValueB00,int,__stdcall,(ItemStatCostTxt *pTxt)) //ebx:StatList *list,edi:(statId<<16)|param,str&dex&hpmax>=1
DLL_ORD(d2common,0x38B70,10973,GetUnitStat,int,__stdcall, (UnitAny *pUnit, int dwStatNo, WORD dwLayerNo))
DLL_ORD(d2common,0x38C20,10587,GetUnitBaseStat,int,__stdcall, (UnitAny *pUnit, int dwStatNo, WORD dwLayerNo))
DLL_ORD(d2common,0x39050,10578,getUnitMaxStamina,int,__stdcall,(UnitAny *pUnit))
DLL_ORD(d2common,0x390A0,10186,getUnitMaxMana,int,__stdcall,(UnitAny *pUnit))
DLL_ORD(d2common,0x390F0,10913,getUnitMaxHP,int,__stdcall,(UnitAny *pUnit))
DLL_FUN(d2common,0x39380,getStatValue39380,int,__stdcall,(int statParam))//eax:StatList *pStatList,esi:int statId,ret-4:int statValue
DLL_FUN(d2common,0x39F70,fun39F70,int,__stdcall,(int statValue,UnitAny *pUnit)) //eax:StatList *pStatList,edi:int statId
DLL_ORD(d2common,0x3A280,10261,changeStatList,int,__stdcall,(StatList *pStatList,int statId,int value,int param))
DLL_ORD(d2common,0x3A330,10245,addStatList,int,__stdcall,(StatList *pStatList,int id,int add,int param))
DLL_ORD(d2common,0x3A710,10551,AddPlayerStat,void,__stdcall,(UnitAny *pUnit,int stat,int add,int idx))
DLL_ORD(d2common,0x3A740,10887,changeUnitStat,int,__stdcall,(UnitAny *pUnit,int statId,int value,int param))
DLL_ORD(d2common,0x3A7D0,10807,stat_10807,int,__stdcall,(UnitAny *pUnit,StatList *pStatList,int arg2))
DLL_ORD(d2common,0x3AE60,10196,Ord10196,int,__stdcall,(UnitAny *pUnit))
DLL_ORD(d2common,0x3AF00,11166,createStatListEx,int,__stdcall,(UnitAny *pUnit,int listFlag,void *fCallback,int arg2,void *pGame))
DLL_ORD(d2common,0x3BC10,10346,Common11084,int,__stdcall,(AreaRectData* pData, int zero))
DLL_ORD(d2common,0x3C000,10826,GetLevelIdFromRectData,int,__stdcall, (AreaRectData *pData))
DLL_ORD(d2common,0x3C390,10057,isInTown,int,__stdcall, (AreaRectData *pAreaRectData))
DLL_ORD(d2common,0x3CCA0,10401,revealRect,int,__stdcall,(DrlgAct *act,int levelNo,int tileX,int tileY,AreaRectData *playerRect))
DLL_ORD(d2common,0x3E980,10688,GetObjectTxt,ObjectTxt *,__stdcall, (int objno))
DLL_ORD(d2common,0x46A40,10890,setItemEthereal,int,__stdcall,(UnitAny *pItem))
DLL_ORD(d2common,0x4AD20,11076,IsLineBlocked,int,__stdcall, (AreaRectData *pData,POINT *p1,POINT *p2,int bitmask))
DLL_ORD(d2common,0x4D7E0,10654,GetDropPos,AreaRectData *,__stdcall,(AreaRectData* pData, POINT*, POINT*, int, int, int, int))
DLL_ORD(d2common,0x4DD30,10252,setSkillCount,void,__stdcall,(Skill *pSkill,int count))
DLL_ORD(d2common,0x4E520,11120,getPlayerSkillCount,int,__stdcall,(int playerClass)) //0-6
DLL_ORD(d2common,0x4E550,10044,getSkillId,int,__stdcall,(int playerClass,int skillPos)) //0-6,0-29
DLL_FUN(d2common,0x4FCB0,getSkillBonus,int,__fastcall, ())//esi:PLAYER eax:pSkill
DLL_ORD(d2common,0x50150,10444,CheckCorpseCanUse,BOOL,__stdcall, (UnitAny *pMon,DWORD flag))
DLL_ORD(d2common,0x505C0,10306,GetSkillLevel,int,__stdcall,(UnitAny *pUnit,Skill *pSkill,int includingBonus))
DLL_ORD(d2common,0x51B80,10435,GetSkillCost,int,__stdcall,(UnitAny *pUnit,int skpoints,int skillId,int curSkillLevel))
DLL_ORD(d2common,0x51BF0,10786,getMaxSkeletons,int,__stdcall,(UnitAny* pUnit,int arg1,int arg2,int skillLv))
DLL_ORD(d2common,0x52340,10787,getSkillStatus,int,__stdcall,(UnitAny *pUnit,Skill *pSkill))
DLL_ORD(d2common,0x52BC0,10302,SetSkillBaseLevel,void,__stdcall,(UnitAny *pUnit,int skillId,int slvl,int bRemove,char *,int))
DLL_ORD(d2common,0x53370,10038,GetSuperUniqueTxt,SuperUniqueTxt *,__stdcall, (int dwUniqueNo))
DLL_ORD(d2common,0x53420,10634,GetTreasureClasses,TreasureClass *,__stdcall, (WORD tcno, int lvlno))
DLL_FUN(d2common,0x56A10,compileMonTypeTxt,void *,__stdcall,(int arg))
DLL_ORD(d2common,0x5E7E0,10066,getPlayerMaxLvl,int,__stdcall,(int playerClass))
DLL_ORD(d2common,0x5E810,10399,GetMinExpToLevel,int,__stdcall, (int dwCharClass, int dwLevel))
DLL_ORD(d2common,0x5E870,11023,uninit,int,__stdcall,())
DLL_ORD(d2common,0x5EF40,10849,CompileTxtFile,void *,__stdcall,(int unused,char *filename,TxtField *fields,int *ptRecordCount,int recordLength))
DLL_ORD(d2common,0x608D0,10670,getLvlOfExp,int,__stdcall,(int charClass,int experience))
DLL_ORD(d2common,0x66160,10943,initGameDataTable,int,__stdcall,(int zero,int fPreCache,int one))
DLL_ORD(d2common,0x665C0,10158,addToRectData1C,int,__stdcall,(UnitAny *pUnit))
DLL_ORD(d2common,0x6CB20,10749,GetLevelDefTxt,LevelDefTxt*,__fastcall, (int levelno))
DLL_ORD(d2common,0x6CC30,10499,GetLevelTxt06,char ,__stdcall, (int levelno))
DLL_ORD(d2common,0x6CC60,10884,GetLevelTxt07,char ,__stdcall, (int levelno))
DLL_ORD(d2common,0x6CC90,10215,GetLevelWeather,char ,__stdcall, (int levelno)) //txt05
DLL_ORD(d2common,0x6CCC0,10014,GetLevelTxt,LevelTxt *,__stdcall, (int levelno))
DLL_ORD(d2common,0x6E0C0,10894,getAreaLevel,int,__stdcall,(int areaId,int difficulty,int expansion))
DLL_ORD(d2common,0x71940,10814,getTxtNoByCode,int,__stdcall,(int code)) //return -1 if not find
DLL_ORD(d2common,0x719A0,10695,GetItemTxt,ItemTxt *,__stdcall, (int itemno)) //6fdc19a0
DLL_FUN(d2common,0x72510,compileItemTypeTxt,void *,__stdcall,(int arg))
DLL_FUN(d2common,0x73330,compileItemStatCostTxt,void *,__stdcall,(int arg))
DLL_FUN(d2common,0x7C300,revealRect7C300,int,__fastcall,(int levelNo,DrlgAct *act,int tileX,int tileY)) //eax:AreaRectInfo *playerRectInfo,esi:DrlgMisc *pDrlgMisc
DLL_ORD(d2common,0x81F20,10701,getEquipmentPos,void,__stdcall,(int invId,int resId,EquipmentLayout20 *dst,int equipId)) //copy equipId*0x14+0x28
DLL_ORD(d2common,0x81FB0,10760,getInventoryLayout,void,__stdcall,(int invId,int resId,InvLayout *dst)) 
DLL_ORD(d2common,0x82030,11012,getInventoryPos16,void,__stdcall,(int invId,int resId,EquipmentLayout16 *dst))
DLL_FUN(d2common,0x820A0,compileInventoryTxt,InventoryBIN *,__stdcall,(int arg))
DLL_VAR(d2common,0x99E1C,DataTables,D2DataTables *)
DLL_VAR(d2common,0x9FA58,InventoryTxtCount,int) 
DLL_VAR(d2common,0x9FA5C,InventoryTxt,InventoryBIN *) 
DLL_VAR(d2common,0x9FB94,ItemTxtCount,int)
DLL_VAR(d2common,0x9FB98,ItemTxts,ItemTxt *) //size 0x1A8
DLL_VAR(d2common,0x9FBA0,WeaponsTxts,int)
DLL_VAR(d2common,0x9FBA8,ArmorTxts,int)
DLL_VAR(d2common,0x9FBD4,RuneWords,int)
DLL_VAR(d2common,0x9FBD8,RuneWordTxt,RuneWordTxt *)
DLL_VAR(d2common,0x9FED8,pPlayerClassTxt ,void *)
DLL_VAR(d2common,0x9FEDC,pPlayerClassLink ,void *)
DLL_VAR(d2common,0x9FEE0,pBodyLocsTxt ,void *)
DLL_VAR(d2common,0x9FEE4,pBodyLocsLink ,void *)
DLL_VAR(d2common,0x9FEE8,pStorePageTxt ,void *)
DLL_VAR(d2common,0x9FEEC,pStorePageLink ,void *)
DLL_VAR(d2common,0x9FEF0,pElemTypesTxt ,void *)
DLL_VAR(d2common,0x9FEF4,pElemTypesLink ,void *)
DLL_VAR(d2common,0x9FEF8,pHitClassTxt ,void *)
DLL_VAR(d2common,0x9FEFC,pHitClassLink ,void *)
DLL_VAR(d2common,0x9FF00,pMonModeTxt ,void *)
DLL_VAR(d2common,0x9FF04,pMonModeLink ,void *)
DLL_VAR(d2common,0x9FF08,pPlrModeTxt ,void *)
DLL_VAR(d2common,0x9FF0C,pPlrModeLink ,void *)
DLL_VAR(d2common,0x9FF10,pSkillCalcTxt ,void *)
DLL_VAR(d2common,0x9FF14,pSkillCalcLink ,void *)
DLL_VAR(d2common,0x9FF18,pSkillCalcCache ,void *)
DLL_VAR(d2common,0x9FF1C,nSkillCalcCache ,int)
DLL_VAR(d2common,0x9FF20,nSkillCalcCacheDelta ,int)
DLL_VAR(d2common,0x9FF24,pSkillDescCalcCache ,void *)
DLL_VAR(d2common,0x9FF28,nSkillDescCalcCache ,int)
DLL_VAR(d2common,0x9FF2C,nSkillDescCalcCacheDelta ,int)
DLL_VAR(d2common,0x9FF30,pMissCalcTxt ,void *)
DLL_VAR(d2common,0x9FF34,pMissCalcLink ,void *)
DLL_VAR(d2common,0x9FF38,pMissCalcCache ,void *)
DLL_VAR(d2common,0x9FF3C,nMissCalcCache ,int)
DLL_VAR(d2common,0x9FF40,nMissCalcCacheDelta ,int)
DLL_VAR(d2common,0x9FF44,pSkillCodesBin,void *)
DLL_VAR(d2common,0x9FF48,pSkillCodesLink ,void *)
DLL_VAR(d2common,0x9FF4C,pEventsTxt ,void *)
DLL_VAR(d2common,0x9FF50,pEventsLink ,void *)
DLL_VAR(d2common,0x9FF54,pCompCodesTxt ,void *)
DLL_VAR(d2common,0x9FF58,pCompCodesLink ,void *)
DLL_VAR(d2common,0x9FF5C,nCompCodes ,int)
DLL_VAR(d2common,0x9FF60,pMonAiTxt ,void *)
DLL_VAR(d2common,0x9FF64,pMonAiLink ,void *)
DLL_VAR(d2common,0x9FF68,nMonAi ,int)
DLL_VAR(d2common,0x9FF6C,pItemLink,ItemCodeTxtNo *)
DLL_VAR(d2common,0x9FF70,pItemCalcCache ,void *)
DLL_VAR(d2common,0x9FF74,nItemCalcCache ,int)
DLL_VAR(d2common,0x9FF78,nItemCalcCacheDelta ,int)
DLL_VAR(d2common,0x9FF7C,pPropertiesTxt ,void *)
DLL_VAR(d2common,0x9FF80,pPropertiesLink ,void *)
DLL_VAR(d2common,0x9FF84,nProperties ,int)
DLL_VAR(d2common,0x9FF88,pRunesLink ,void *)
DLL_VAR(d2common,0x9FF8C,pHireDescTxt ,void *)
DLL_VAR(d2common,0x9FF90,pHireDescLink ,void *)
DLL_VAR(d2common,0x9FF94,pStatesTxt ,void *)
DLL_VAR(d2common,0x9FF98,pStatesLink ,void *)
DLL_VAR(d2common,0x9FF9C,nStates ,int)
DLL_VAR(d2common,0x9FFA0,pStateMaskFirst ,void *)
DLL_VAR(d2common,0x9FFA4,pStateMaskArray,void *)
DLL_VAR(d2common,0xA0044,pProgressiveStates,void *)
DLL_VAR(d2common,0xA0048,nProgressiveStates,int)
DLL_VAR(d2common,0xA004C,pCurseStates,void *)
DLL_VAR(d2common,0xA0050,nCurseStates ,int)
DLL_VAR(d2common,0xA0054,pTransformStates,void *)
DLL_VAR(d2common,0xA0058,nTransformStates ,int)
DLL_VAR(d2common,0xA005C,pActionStates,void *)
DLL_VAR(d2common,0xA0060,nActionStates ,int)
DLL_VAR(d2common,0xA0064,pColorStates,void *)
DLL_VAR(d2common,0xA0068,nColorStates ,int)
DLL_VAR(d2common,0xA006C,pSoundCodesBin,void *)
DLL_VAR(d2common,0xA0070,pSoundCodesLink ,void *)
DLL_VAR(d2common,0xA0074,nSoundCodes ,int)
DLL_VAR(d2common,0xA0078,pHirelingsTxt ,void *)
DLL_VAR(d2common,0xA007C,nHirelings ,int)
DLL_VAR(d2common,0xA0080,HireNameListStart,void *)
DLL_VAR(d2common,0xA0480,HireNameListEnd,void *)
DLL_VAR(d2common,0xA0880,pNpcsTxt ,void *)
DLL_VAR(d2common,0xA0884,nNpcs ,int)
DLL_VAR(d2common,0xA0888,pColorsTxt ,void *)
DLL_VAR(d2common,0xA088C,pColorsLink ,void *)
DLL_VAR(d2common,0xA0890,pTreasureClassExLink ,void *)
DLL_VAR(d2common,0xA0894,pTreasureClassExTxt,TreasureClass *)
DLL_VAR(d2common,0xA0898,nTreasureClassEx,int) //=0x3F5 1013
DLL_VAR(d2common,0xA089C,pChestTreasureClassList,void *)
DLL_VAR(d2common,0xA0950,pMonStatsTxt ,void *)
DLL_VAR(d2common,0xA0954,pMonStatsLink ,void *)
DLL_VAR(d2common,0xA0958,nMonStats ,int)
DLL_VAR(d2common,0xA095C,pMonSoundsTxt ,void *)
DLL_VAR(d2common,0xA0960,pMonSoundsLink ,void *)
DLL_VAR(d2common,0xA0964,nMonSounds ,int)
DLL_VAR(d2common,0xA0968,pMonStats2Txt ,void *)
DLL_VAR(d2common,0xA096C,pMonStats2Link ,void *)
DLL_VAR(d2common,0xA0970,nMonStats2 ,int)
DLL_VAR(d2common,0xA0974,pMonPlaceTxt ,void *)
DLL_VAR(d2common,0xA0978,pMonPlaceLink ,void *)
DLL_VAR(d2common,0xA097C,nMonPlace ,int)
DLL_VAR(d2common,0xA0980,pMonPresetTxt ,void *)
DLL_VAR(d2common,0xA0984,pMonPresetActList,void *)
DLL_VAR(d2common,0xA0998,nMonPresetEachAct,int)
DLL_VAR(d2common,0xA09AC,pSuperUniquesTxt ,void *)
DLL_VAR(d2common,0xA09B0,pSuperUniquesLink ,void *)
DLL_VAR(d2common,0xA09B4,nSuperUniques ,int)
DLL_VAR(d2common,0xA09B8,SuperUniqueHcIdxList,void *)
DLL_VAR(d2common,0xA0A3C,pMissilesTxt ,void *)
DLL_VAR(d2common,0xA0A40,pMissilesLink ,void *)
DLL_VAR(d2common,0xA0A44,nMissiles ,int)
DLL_VAR(d2common,0xA0A48,pMonLvlTxt ,void *)
DLL_VAR(d2common,0xA0A4C,nMonLvl ,int)
DLL_VAR(d2common,0xA0A50,pMonSeqTxt ,void *)
DLL_VAR(d2common,0xA0A54,pMonSeqLink ,void *)
DLL_VAR(d2common,0xA0A58,nMonSeq ,int)
DLL_VAR(d2common,0xA0A5C,pMonSequences,void *)
DLL_VAR(d2common,0xA0A60,nMonSequences,int)
DLL_VAR(d2common,0xA0A64,pSkillDescTxt,void *)
DLL_VAR(d2common,0xA0A68,pSkillDescLink ,void *)
DLL_VAR(d2common,0xA0A6C,nSkillDesc ,int)
DLL_VAR(d2common,0xA0A70,pSkillsTxt ,void *)
DLL_VAR(d2common,0xA0A74,pSkillsLink ,void *)
DLL_VAR(d2common,0xA0A78,nSkills ,int)
DLL_VAR(d2common,0xA0A7C,pPlayerSkillCount,void *)
DLL_VAR(d2common,0xA0A80,nPlayerSkillCount ,int)
DLL_VAR(d2common,0xA0A84,pPlayerSkillList,void *)
DLL_VAR(d2common,0xA0A88,nPassiveSkillCount ,int)
DLL_VAR(d2common,0xA0A8C,pPassiveSkillList,void *)
DLL_VAR(d2common,0xA0A90,pOverlayTxt ,void *)
DLL_VAR(d2common,0xA0A94,pOverlayLink ,void *)
DLL_VAR(d2common,0xA0A98,nOverlay ,int)
DLL_VAR(d2common,0xA0A9C,pCharStatsTxt ,void *)
DLL_VAR(d2common,0xA0AA0,nCharStats ,int)
DLL_VAR(d2common,0xA0AA4,pItemStatCostTxt ,ItemStatCostTxt *)
DLL_VAR(d2common,0xA0AA8,pItemStatCostLink ,void *)
DLL_VAR(d2common,0xA0AAC,nItemStatCost ,int)
DLL_VAR(d2common,0xA0AB0,pOpStatNesting,void *)
DLL_VAR(d2common,0xA0AB4,nOpStatNesting ,int)
DLL_VAR(d2common,0xA0AB8,pMonEquipTxt ,void *)
DLL_VAR(d2common,0xA0ABC,nMonEquip ,int)
DLL_VAR(d2common,0xA0AC0,pPetTypesTxt ,void *)
DLL_VAR(d2common,0xA0AC4,pPetTypesLink ,void *)
DLL_VAR(d2common,0xA0AC8,nPetTypes ,int)
DLL_VAR(d2common,0xA0ACC,pItemTypesLink ,void *)
DLL_VAR(d2common,0xA0AD0,pItemTypesTxt,ItemTypeTxt *)
DLL_VAR(d2common,0xA0AD4,nItemTypes,int) //=0x67
DLL_VAR(d2common,0xA0AD8,nItemTypeNesting ,int) //=4
DLL_VAR(d2common,0xA0ADC,pItemTypeNesting,void *)
DLL_VAR(d2common,0xA0AE0,pSetsLink ,void *)
DLL_VAR(d2common,0xA0AE4,pSetsTxt ,void *)
DLL_VAR(d2common,0xA0AE8,nSets ,int)
DLL_VAR(d2common,0xA0AEC,pSetItemsLink ,void *)
DLL_VAR(d2common,0xA0AF0,pSetItemsTxt ,void *)
DLL_VAR(d2common,0xA0AF4,nSetItems ,int)
DLL_VAR(d2common,0xA0AF8,pUniqueItemsLink ,void *)
DLL_VAR(d2common,0xA0AFC,pUniqueItemsTxt ,void *)
DLL_VAR(d2common,0xA0B00,nUniqueItems ,int)
DLL_VAR(d2common,0xA0B04,pMonPropLink ,void *)
DLL_VAR(d2common,0xA0B08,pMonPropTxt ,void *)
DLL_VAR(d2common,0xA0B0C,nMonProp ,int)
DLL_VAR(d2common,0xA0B10,pMonTypeLink ,void *)
DLL_VAR(d2common,0xA0B14,pMonTypeTxt ,void *)
DLL_VAR(d2common,0xA0B18,nMonType ,int)
DLL_VAR(d2common,0xA0B1C,pMonTypeNesting,void *)
DLL_VAR(d2common,0xA0B20,nMonTypeNesting ,int)
DLL_VAR(d2common,0xA0B24,pMonUModLink ,void *)
DLL_VAR(d2common,0xA0B28,pMonUModTxt ,void *)
DLL_VAR(d2common,0xA0B2C,nMonUMod ,int)
DLL_VAR(d2common,0xA0B30,pLevelTxt,LevelTxt *)
DLL_VAR(d2common,0xA0B34,nLevels ,int)
DLL_VAR(d2common,0xA0B38,pLvlDefsBin ,void *)
DLL_VAR(d2common,0xA0B3C,pLvlPrestTxt ,void *)
DLL_VAR(d2common,0xA0B40,nLvlPrest ,int)
DLL_VAR(d2common,0xA0B44,ItemStatCostStuff,void *)
DLL_VAR(d2common,0xA0B4C,pAnimTables,void *)
DLL_VAR(d2common,0xA0B50,pExperienceTxt ,void *)
DLL_VAR(d2common,0xA0B54,pDifficultyLevelsTxt ,void *)
DLL_VAR(d2common,0xA0B58,nDifficultyLevels ,int)
DLL_VAR(d2common,0xA0B5C,pExpFieldD2,void *)
DLL_VAR(d2common,0xA0B60,ExpFieldRuntimeData,void *)
DLL_VAR(d2common,0xA0B88,pLvlSubExtraStuff,void *)
DLL_VAR(d2common,0xA0B8C,nLvlSubExtraStuff ,int)
DLL_VAR(d2common,0xA0B90,pCharTemplateTxt ,void *)
DLL_VAR(d2common,0xA0B94,nCharTemplate ,int)
DLL_VAR(d2common,0xA0B98,pArenaTxt ,void *)
DLL_VAR(d2common,0xA0B9C,pLvlTypesTxt ,void *)
DLL_VAR(d2common,0xA0BA0,pWaypoints,void *)
DLL_VAR(d2common,0xA0BA4,nWaypoints ,int)
DLL_VAR(d2common,0xA0BA8,nLvlTypes ,int)
DLL_VAR(d2common,0xA0BAC,pLvlWarpTxt ,void *)
DLL_VAR(d2common,0xA0BB0,nLvlWarp ,int)
DLL_VAR(d2common,0xA0BB4,pLvlMazeTxt ,void *)
DLL_VAR(d2common,0xA0BB8,nLvlMaze ,int)
DLL_VAR(d2common,0xA0BBC,pLvlSubTxt ,void *)
DLL_VAR(d2common,0xA0BC0,nLvlSub ,int)
DLL_VAR(d2common,0xA0BC4,pLvlSubCache,void *)
DLL_VAR(d2common,0xA0BD4,pMapCache,void *)
DLL_VAR(d2common,0xA0BD8,nMapCache ,int)
DLL_VAR(d2common,0xA0BDC,pCubeMainTxt ,void *)
DLL_VAR(d2common,0xA0BE0,nCubeMain ,int)
DLL_FUN(d2game,0x1180,random,int,__fastcall,(D2Seed *seed)) //eax:number
DLL_FUN(d2game,0x1280,TestPosition,AreaRectData *,__fastcall,()) //eax:pData,ebx:x,edi:y
DLL_FUN(d2game,0x2130,getWorld_2130,World *,__stdcall,()) //edx:WorldHash *worlds,esi:int worldId
DLL_FUN(d2game,0x2A3C,exit,int,__stdcall,(int exitcode))
DLL_FUN(d2game,0xE250,hasDroppedUnique,int,__fastcall,(int txt)) //eax:World *world
DLL_FUN(d2game,0xE5D0,GetUnitLevel,int,__stdcall,(int zero)) //eax=pUnit
DLL_FUN(d2game,0xE930,GetFreeXY,AreaRectData *,__stdcall,(AreaRectData *pData,POINT *out,int size)) //esi:POINT *in
DLL_FUN(d2game,0x11490,DropItem1490,UnitAny *,__stdcall,(World *world,DropParam *param,int))
DLL_FUN(d2game,0x11880,DropItem1880,void,__stdcall,(World *world,int txt,int quality,int,int))
DLL_FUN(d2game,0x200E0,Game235C0,void,__stdcall,(World* game,AreaRectData *pData))
DLL_FUN(d2game,0x24950,SpawnSuperUnique,int,__fastcall,(World* game,AreaRectData *pData,int zero1,int x,int y,int minusOne,int txt,int zero2)) //wrong param
DLL_ORD(d2game,0x2A330,10048,initCallbackTable,int,__stdcall,(void *table))
DLL_ORD(d2game,0x2A370,10037,setWorldHash,int,__stdcall,(WorldHash *worldhash,int *arg1)) //arg1 not used, can't be null
DLL_ORD(d2game,0x2A9A0,10047,uninit,int,__stdcall,())
DLL_FUN(d2game,0x2AAE0,getWorldByID,World *,__stdcall,(int worldId))
DLL_FUN(d2game,0x2AB70,getWorld,World *,__stdcall,()) //esi:clientId
DLL_ORD(d2game,0x2C570,10044,NewEmptyGame,int,__stdcall,(char *name,char *pass,char *desc, int flag,char bTemplate,char bReserved1,char bReserved2, short *pwGameId))
DLL_FUN(d2game,0x2C830,SaveGame,void,__stdcall,(World *game))
DLL_ORD(d2game,0x2CFD0,10003,removeClientFromGame,int,__stdcall,(int clientId))
DLL_ORD(d2game,0x2D3B0,10035,SendDatabaseCharacter,int,__stdcall,(int clientId, void *saveData, int size, int totalSize, int bLock, int reserved1, PlayerInfo *playerInfo, int reserved2))
DLL_ORD(d2game,0x2D4B0,10038,Ord10038,int,__stdcall,())
DLL_ORD(d2game,0x2D820,10006,endAllGames,int,__cdecl,(int arg0))
DLL_ORD(d2game,0x2DF10,10024,Ord10024,int,__stdcall,(int isExiting,int zero))
DLL_ORD(d2game,0x2E1E0,10008,isBusy,int,__stdcall,(int zero))
DLL_ORD(d2game,0x2E460,10040,processGame,int,__stdcall,())
DLL_FUN(d2game,0x34C00,sendQuestToClient,int,__stdcall,(World *world)) //eax:UnitAny *pUnit
DLL_FUN(d2game,0x35640,processClientQuestReq,int,__stdcall,(World *world,UnitAny *pUnit))
DLL_FUN(d2game,0x49FE0,getNetClient,NetClient *,__fastcall,(int clientId)) //eax:World *context
DLL_FUN(d2game,0x4AB70,sendHandshake,int,__stdcall,(NetClient* client)) //eax:UnitAny *player
DLL_FUN(d2game,0x4E2A0,fun_4E2A0,void,__fastcall,(World *game,UnitAny *pMon,int skillId)) //eax=pUnit ecx=0
DLL_FUN(d2game,0x58BE0,passExpansion58BE0,void,__fastcall,(UnitAny *player)) //eax:QuestInfo *pQuest,ebx:World *world
DLL_FUN(d2game,0x599E0,passExpansion599E0,void,__fastcall,(World *game,UnitAny *player,int arg)) //player must be in worldstone chamber
DLL_FUN(d2game,0x637E0,completeImbueQuest,void,__stdcall,()) //eax:UnitAny *player,edi:World *game
DLL_FUN(d2game,0x70170,OpenPandFinalPortal,void,__fastcall,(World *game,UnitAny *pUnit))
DLL_FUN(d2game,0x70180,OpenPandPortal,void,__fastcall,(World *game,UnitAny *pUnit))
DLL_FUN(d2game,0x769A0,fun_769A0,int,__stdcall,(World *world,int arg2,int arg3))//edi:UnitAny *pUnit,esi:mode?
DLL_FUN(d2game,0x7B4E0,UberMephAI,void,__fastcall,(World* game,UnitAny *pMon,void *param))
DLL_FUN(d2game,0x85B60,MephAI,void,__fastcall,(World* game,UnitAny *pMon,void *param))
DLL_FUN(d2game,0x8A3E0,sendPacket,int,__stdcall,(char *packet,int size)) //eax:NetClient* client
DLL_FUN(d2game,0x8BD00,sendBaseAttr,int,__fastcall,(int statId,int value)) //eax:NetClient* client, statId<255
DLL_FUN(d2game,0x8D3D0,sendItemToClient8D3D0,void,__stdcall,(int packet1,int itemFlags,int classOnly))//ebx:NetClient *client,esi:UnitAny *pItem
DLL_FUN(d2game,0x8D5F0,SetSkillBaseLevelOnClient,void,__fastcall,(void *ptClient,UnitAny *pUnit,int skillId,int skillLvl,int bRemove))//by EAX,ESI,EBX
DLL_FUN(d2game,0x8DF70,sendItemToClient,void,__stdcall,(int itemFlags))//eax:UnitAny *player,ecx:NetClient *client,esi:UnitAny *pItem
DLL_FUN(d2game,0x8EBC0,fun_8EBC0,int,__stdcall,(World *world,void *arg2))
DLL_FUN(d2game,0xA22E0,LinkPortal,UnitAny *,__stdcall,(World* game,UnitAny *pUnit,int dstArea,int srcArea))
DLL_FUN(d2game,0xA3610,NecSummon,UnitAny *,__fastcall,()) //eax=param
DLL_FUN(d2game,0xA39D0,UberDiabloAI,void,__fastcall,(World* game,UnitAny *pMon,void *param))
DLL_FUN(d2game,0xA9610,DiabloAI,void,__fastcall,(World* game,UnitAny *pMon,void *param))
DLL_FUN(d2game,0xAFDB0,setupMonsterStat,void,__stdcall,(World* game,int arg1,int arg2)) //ebx:UnitAny *pMon
DLL_FUN(d2game,0xB0320,setupMonster,void,__fastcall,(World* game,AreaRectData * pData,UnitAny *pMon,int id))
DLL_FUN(d2game,0xB6C80,UberBaalAI,void,__fastcall,(World* game,UnitAny *pMon,void *param))
DLL_FUN(d2game,0xB8610,BaalAI,void,__fastcall,(World* game,UnitAny *pMon,void *param))
DLL_FUN(d2game,0xB9A30,passClassicB9A30,int,__fastcall,()) //eax:QuestInfo *questData,ecx:UnitAny *player,ebx:World* world
DLL_FUN(d2game,0xB9F20,passClassicB9F20,int,__fastcall,(World* game,UnitAny *player,int arg))
DLL_FUN(d2game,0xC00C0,imbueItem,int,__stdcall,(World* game,int arg1,int arg2,int arg3)) //ecx:UnitAny *player
DLL_FUN(d2game,0xCBBB0,send0D_unitXyHp,int,__stdcall,()) //eax:UnitAny *pUnit
DLL_FUN(d2game,0xCBD30,recv40_quest,int,__fastcall,(World *world,UnitAny *pUnit,char *packet,int len))
DLL_FUN(d2game,0xCC550,processPacket,int,__stdcall,(World *world,int len))//esi:UnitAny *pUnit,ebx:char *packet
DLL_FUN(d2game,0xCCF00,itemNotInInv,int,__fastcall,(int itemId,int notUsed,UnitAny *pUnit)) //eax:World *world
DLL_FUN(d2game,0xCEBD0,recv28_socket,int,__fastcall,(World *world,UnitAny *pUnit,char *packet,int len))
DLL_FUN(d2game,0xCEE60,recv1A_handToBody,int,__fastcall,(World *world,UnitAny *pUnit,char *packet,int len))
DLL_FUN(d2game,0xCF510,recv60_swapWeapon,int,__fastcall,(World *world,UnitAny *pUnit,char *packet,int len))
DLL_FUN(d2game,0xCF730,recv20_activeBufferItem,int,__fastcall,(World *world,UnitAny *pUnit,char *packet,int len))
DLL_FUN(d2game,0xD01D0,pickup,int,__fastcall,(World *world,int toHand,UnitAny *player,int id)) //eax:int type
DLL_FUN(d2game,0xD08B0,recv16_pickup,int,__fastcall,(World *world,UnitAny *pUnit,char *packet,int len))
DLL_FUN(d2game,0xD1010,recv1F_swapItem,int,__fastcall,(World *world,UnitAny *pUnit,char *packet,int len))
DLL_FUN(d2game,0xD1370,recv19_invToHand,int,__fastcall,(World *world,UnitAny *pUnit,char *packet,int len))
DLL_FUN(d2game,0xD4A70,putItemInSocketD4A70,int,__stdcall,(World *world,UnitAny *pUnit,int equipmentItemId,int *result,int one4,int one5,int one6,int one7)) //eax:inSocketItemId
DLL_FUN(d2game,0xD7160,pickupItem,int,__stdcall,(int itemId,int *result))//eax:UnitAny *player,ebx:World *world
DLL_FUN(d2game,0xD8EA0,swapWeapon,int,__stdcall,(World *world,int *result)) //eax:UnitAny *pUnit
DLL_FUN(d2game,0xDFA90,isUnitDead,int,__stdcall,()) //eax:UnitAny *pUnit
DLL_FUN(d2game,0xE03A0,GetUnit,int,__fastcall,(World *world,int unitId)) //eax:int unitType
DLL_FUN(d2game,0xE1D90,CreateUnit,UnitAny *,__fastcall,(int type,int txt,int x,int y,World* game, AreaRectData* pData,int uk1, int uk2, int uk3))
DLL_FUN(d2game,0xEB090,playerUpgrade,int,__stdcall,(World *world)) //esi:UnitAny *pUnit
DLL_FUN(d2game,0xEC7F0,MakeD2S,int,__fastcall,(UnitAny *pUnit,World *world,char *buf,int *size,int cap,int zero,int one))
DLL_FUN(d2game,0xEEF10,SpawnMonsterEF10,UnitAny *,__fastcall,(SpawnMon *mon)) 
DLL_FUN(d2game,0xEF650,SpawnMonster,UnitAny *,__fastcall,(int id,int mode,World* game,AreaRectData *pData,int x,int y,int one,int zero)) 
DLL_FUN(d2game,0xF2380,MonsterUseSkill,void,__fastcall,(World* game,UnitAny *pMon,int arg,int skill,UnitAny *target,int x,int y)) 
DLL_FUN(d2game,0xF2540,MonsterMove,void,__fastcall,(World* game,UnitAny *pMon,UnitAny *target,int two,int x,int y,int one,int zero)) 
DLL_VAR(d2game,0xF829C,sgptDataTables,D2DataTables *) 
DLL_VAR(d2game,0xFB014,SPUnitOffsets,int)
DLL_VAR(d2game,0x110BE8,criticalSection,CriticalSection) 
DLL_VAR(d2game,0x111C24,worldHash,WorldHash *) 
DLL_VAR(d2game,0x111F98,DropChioces,int) 
DLL_VAR(d2game,0x111FA0,DropChioceProb,int) 
DLL_FUN(d2gdi,0x6E40,initCriticalSection,int,__fastcall,(void *a,void *b))
DLL_VAR(d2gdi,0xC150,ColorMap,int)
DLL_VAR(d2gdi,0xC678,hwnd,HWND)
DLL_VAR(d2gdi,0xCA80,FrameBufWidth,int)
DLL_VAR(d2gdi,0xCA84,palette,HPALETTE)
DLL_VAR(d2gdi,0xCA8C,gdiobj,HGDIOBJ)
DLL_VAR(d2gdi,0xCA90,FrameBufHeight,int)
DLL_VAR(d2gdi,0xCA9C,DisplayRight,int)
DLL_VAR(d2gdi,0xCAA8,FrameBuf,void *)
DLL_VAR(d2gdi,0xCAB0,criticalSection,CriticalSection *)
DLL_ORD(d2gfx,0x7C80,10076,drawGroundPiece,void,__stdcall, (void *buf0,void *buf1,int arg2,int arg3,int arg4,int arg5,int arg6,int arg7,int arg8)) 
DLL_ORD(d2gfx,0x7FB0,10048,GetHwnd,HWND,__stdcall, ()) 
DLL_ORD(d2gfx,0x7FC0,10059,MinimizeWindows,void,__stdcall, (DWORD dwFlag))
DLL_ORD(d2gfx,0x88B0,10043,Ordinal10043,void,__stdcall,())
DLL_ORD(d2gfx,0xADD0,10040,flushFramebuf,void,__stdcall, ())
DLL_ORD(d2gfx,0xADF0,10085,clearFramebuf,void,__stdcall, (int a,int b,int c,int d))
DLL_ORD(d2gfx,0xAFD0,10011,drawShadow,void,__stdcall, (void *ptr,int x,int y))
DLL_ORD(d2gfx,0xAFF0,10079,DrawAutomapNode,void,__stdcall, (CellContext *context, int xpos, int ypos, RECT *cliprect, DWORD dwTransLvl)) 
DLL_ORD(d2gfx,0xB020,10074,FillOrb,void,__stdcall, (int arg0,int x,int y,int arg3,int arg4,int arg5)) 
DLL_ORD(d2gfx,0xB050,10019,drawSomething,void,__stdcall, (void *ptr,int x,int y,int d,int e,int f)) //6fa8b050 d=-1 e=(4:char,5:skill button) f=(0:normal 1:inactive)
DLL_ORD(d2gfx,0xB080,10041,DrawCellFile,int,__stdcall, (CellContext *context, int xPos, int yPos, DWORD dw1, int dwTransLvl, BYTE *coltab)) 
DLL_ORD(d2gfx,0xB320,10031,getResolutionMode,int,__stdcall, ()) 
DLL_ORD(d2gfx,0xB9C0,10010,DrawLine,void,__stdcall, (int x1, int y1, int x2, int y2, int dwColor, int dwTransLvl)) 
DLL_ORD(d2gfx,0xBA30,10014,DrawRectangle,void,__stdcall, (long dwXstart, long dwYstart, long dwXend, long dwYend, int dwPal, int dwTransLvl)) //filled
DLL_VAR(d2gfx,0x11260,ResolutionMode,int)
DLL_VAR(d2gfx,0x11268,callbackTable,void *)
DLL_VAR(d2gfx,0x148CC,DrawGoundH2,int)
DLL_VAR(d2gfx,0x1D468,DrawGoundH1,int)
DLL_VAR(d2gfx,0x1D66C,WinState,int)
DLL_FUN(d2lang,0x9050,getLocalText9050,wchar_t *,__stdcall,(wchar_t **strs)) //eax:d2lang_map *d2str,edx:int txtNo
DLL_ORD(d2lang,0x9450,10003,GetLocaleText,wchar_t*,__fastcall, (int dwLocaleTxtNo))
DLL_ORD(d2lang,0x9E20,10005,uninit,int,__stdcall,())
DLL_ORD(d2lang,0xA130,10008,initLanguage,int,__fastcall,(int zero,char *language,int one)) //ret 0 if failed
DLL_FUN(d2lang,0xAFE0,strcat,wchar_t *,__fastcall,(wchar_t *dst,wchar_t *src)) //ret dst
DLL_VAR(d2lang,0x10A64,localeTxtMap0,d2lang_map *) //0-9999
DLL_VAR(d2lang,0x10A68,localeTxt0,wchar_t **) //0-9999
DLL_VAR(d2lang,0x10A6C,localeTxt10000,wchar_t **) //10000-19999
DLL_VAR(d2lang,0x10A70,localeTxt20000,wchar_t **) //20000-29999
DLL_VAR(d2lang,0x10A80,localeTxtMap10000,d2lang_map *) //20000-29999
DLL_VAR(d2lang,0x10A84,localeTxtMap20000,d2lang_map *) //20000-29999
DLL_FUN(d2launch,0x101B,sprintf,int,__cdecl,(char *buf,char *fmt,...))
DLL_FUN(d2launch,0xDB60,DeleteSelectedCharacter,int,__stdcall,(int arg)) //return 0:failed 1:deleted
DLL_FUN(d2launch,0xF3A0,DeleteCharacter,void,__fastcall,(char *name)) //eax:region
DLL_VAR(d2launch,0x211F0,createTextBoxParam11F0,DWORD *)
DLL_VAR(d2launch,0x24DB0,SelectedCharIndex,int)
DLL_VAR(d2launch,0x252B8,launchChildren,void *)
DLL_VAR(d2launch,0x25E30,launchNumChildren,int)
DLL_VAR(d2launch,0x25EC8,D2Characters,D2Character *)
DLL_VAR(d2launch,0x25ED0,charCount,int)
DLL_FUN(d2mcpclient,0x63E0,mcpPacket05,int,__fastcall,(MCPPacket05 *packet,int packetLen))
DLL_ORD(d2mcpclient,0x67F0,10000,getMcp05Buf,Mcp05Buf *,__stdcall,()) //return d2mcpclient_B030
DLL_ORD(d2mcpclient,0x6990,10050,processOnePacket,int,__stdcall,())
DLL_VAR(d2mcpclient,0xAEB4,mpc05_seqId,short) //mcp05->seqId need to be same to this
DLL_VAR(d2mcpclient,0xAEE0,AccountInfo,D2AccountInfo)
DLL_VAR(d2mcpclient,0xB028,mcpSrc,void *) //used to read mcp packets
DLL_VAR(d2mcpclient,0xB030,mcp05_buf,Mcp05Buf) //size=0x63(99)
DLL_VAR(d2mcpclient,0xB034,mcp05_seqId,int) //=mcp05->seqId
DLL_VAR(d2mcpclient,0xB038,mcp05_idx,int) //=mcp05->idx
DLL_VAR(d2mcpclient,0xB044,mcp05_nPlayers,int) //=mcp05->nPlayers
DLL_VAR(d2mcpclient,0xB048,mcp05_off08,int) //=mcp05->off08
DLL_VAR(d2mcpclient,0xB04C,mcp05_gameName,char) 
DLL_VAR(d2mcpclient,0xB05C,mcp05_gameName2,char) 
DLL_VAR(d2mcpclient,0xB1C8,mcp05_type,int) //mcp05->off08=-2?0x33,-1?0x32
DLL_FUN(d2multi,0x1058,num2str1058,int,__stdcall,(int base,int neg)) //eax:num,ecx:char *buf
DLL_FUN(d2multi,0x1096,num2str,int,__cdecl,(int num,char *buf,int base))
DLL_FUN(d2multi,0x11F80,show_games,int,__stdcall,()) //maybe
DLL_FUN(d2multi,0x12120,add_game,int,__stdcall,()) //edi:Mcp05Buf *mcpbuf
DLL_FUN(d2multi,0x14850,reset_games,int,__stdcall,()) //maybe
DLL_VAR(d2multi,0x1BF20,mcp05_games,Mcp05Game)//size=0x3B 59 each
DLL_VAR(d2multi,0x19C60,EditboxPreferences,void *)  //6F9E9C5C+4
DLL_VAR(d2multi,0x2A598,mcp05_games2,Mcp05Game)//size=0x3B 59 each
DLL_VAR(d2multi,0x39CC0,GameListControl,D2EditBox *)
DLL_VAR(d2multi,0x39D2C,GameName,D2EditBox *)
DLL_VAR(d2multi,0x39D30,GamePassword,D2EditBox *)
DLL_VAR(d2multi,0x39D34,GameDescription,D2EditBox *)
DLL_VAR(d2multi,0x39DCC,nGames,int)
DLL_VAR(d2multi,0x3A104,lastRefreshMs1,int)
DLL_VAR(d2multi,0x3A108,lastRefreshMs2,int)
DLL_ORD(d2net,0x6000,10015,getWorldId,int,__stdcall,(int clientId))
DLL_ORD(d2net,0x61F0,10028,uninit,int,__stdcall,())
DLL_ORD(d2net,0x63E0,10002,SendToClient,int,__stdcall, (int zero,int clientId,void *data,int size))
DLL_ORD(d2net,0x6590,10005,initNetwork,int,__stdcall,(int zero,int notNT)) //ret 0 if error
DLL_ORD(d2net,0x6750,10033,getPacketSize,void,__fastcall,(char *pkg,int len,int *plen))
DLL_ORD(d2net,0x7650,10024,SendPacket,void,__stdcall,(int len,int not_used,BYTE* buf)) //len<=516
DLL_VAR(d2net,0xA900,packetSizes,int)
DLL_VAR(d2net,0xB244,UnkNetFlag,DWORD)
DLL_VAR(d2net,0xB424,clientInfos,ClientInfos *)
DLL_ORD(d2sound,0x9AA0,10004,set_music_volumn,int,__fastcall,(int volumn))
DLL_ORD(d2sound,0x9AC0,10011,get_music_volumn,int,,())
DLL_VAR(d2sound,0x164A8,music_volumn,int) //0-100
DLL_FUN(d2win,0x7720,ToggleSound,void,__stdcall,())
DLL_ORD(d2win,0x7C6E,10186,setLineEndText,void,__fastcall,(D2EditBox *list,char *buf,void *line,int one))
DLL_ORD(d2win,0x9740,10022,PrintLineOnTextBox,void,__fastcall, (void *textbox, char *buf, int color))
DLL_ORD(d2win,0x97C0,10167,addLineToList,void,__fastcall,(D2EditBox *list,char *buf))
DLL_ORD(d2win,0xA340,10088,uninit,int,__stdcall,())
DLL_ORD(d2win,0xAA20,10086,initMPQ,int, __fastcall, ()) //return 0 if failed
DLL_ORD(d2win,0x118F0,10085,DrawHoverText,void,__fastcall, (wchar_t *wStr, long xPos, long yPos, int dwTran, int dwColor))
DLL_ORD(d2win,0x12700,10177,GetTextAreaSize,int,__fastcall, (wchar_t *wStr, int* pWidth, int* pHeight)) 
DLL_ORD(d2win,0x12730,10070,DrawUnitLifeBar,int,__fastcall, (wchar_t *wStr ,long xPos, long yPos, int dwColor, DWORD dwUnk1 ,DWORD dwUnk2))
DLL_ORD(d2win,0x12B20,10028,GetTextPixelLen,int,__fastcall, (wchar_t *wStr))//doesn't ignore color codes
DLL_ORD(d2win,0x12FA0,10150,DrawText,void, __fastcall, (wchar_t *wStr, long xPos, long yPos, int dwColor, int dwAlign))
DLL_ORD(d2win,0x12FE0,10184,SetTextFont,int,__fastcall, (int dwFont))
DLL_ORD(d2win,0x133A0,10110,DrawHover,void, __fastcall, ()) 
DLL_ORD(d2win,0x136A0,10166,GetEditBoxText,wchar_t*,__fastcall, (D2EditBox* box))
DLL_ORD(d2win,0x13720,10021,SelectEditBoxText,void,__fastcall,  (D2EditBox* box))
DLL_ORD(d2win,0x13970,10170,SetEditBoxProc,void, __fastcall, (D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,int,char*)))
DLL_ORD(d2win,0x14DF0,10042,SetEditBoxText,void*,__fastcall, (D2EditBox* box, wchar_t* wcszTxt))
DLL_ORD(d2win,0x15450,10064,AddEditBoxChar,DWORD,__fastcall, (D2EditBox* box, BYTE keycode))
DLL_ORD(d2win,0x159E0,10048,DestroyEditBox,int,__fastcall, (D2EditBox* box))
DLL_ORD(d2win,0x161B0,10057,CreateEditBox,D2EditBox*,__fastcall, (int dwPosX, int dwPosY, int dwSizeX, int dwSizeY, int dwOffsetX, int dwOffsetY, CellFile *pCellFile, DWORD dwReturnHandler, DWORD arg9, DWORD dwIsCloaked, void* buf))
DLL_FUN(d2win,0x17740,processMessage,int,__stdcall,(HWND hwnd,int msg,int wParam,int lParam))
DLL_ORD(d2win,0x18490,10050,DestroyListBox,void,__stdcall,   (D2EditBox* box))
DLL_ORD(d2win,0x18560,10098,CreateTextBox,void *,__stdcall, (DWORD* param))
DLL_ORD(d2win,0x187E0,10024,DrawMainMenu,void*,__fastcall, ())
DLL_VAR(d2win,0x214A0,var_214A0,void *)
DLL_VAR(d2win,0x214B0,FocusedControl,D2EditBox*)
DLL_VAR(d2win,0x214BC,clientProcessMessageFunc,void *)
DLL_VAR(d2win,0x214C0,RunningFlag,int)
DLL_VAR(d2win,0xC9CF8,music_volumn,int) //0-100
DLL_VAR(d2win,0xCA65C,DrawHoverY,int)
DLL_FUN(fog,0x1BA9,exit,void,__stdcall,(int exitCode))
DLL_ORD(fog,0xD070,10185,enableNTFeature,int,__stdcall,(int *p100))
DLL_FUN(fog,0xE4E0,getClientInfo,ClientInfo *,__stdcall,(ClientInfos *infos,int id))
DLL_ORD(fog,0xE9F0,10173,getClientID,int,__stdcall,(ClientInfos *infos,int id))
DLL_ORD(fog,0xFC20,10265,getEIP,int,__stdcall,())
DLL_ORD(fog,0x10810,10021,setLogFilePrefix,int,__fastcall,(char *prefix))
DLL_ORD(fog,0x11100,10024,reportError,void,__cdecl,(void *ptr,int addr,int errNo))
DLL_ORD(fog,0x11690,10251,FogValidateCriticalSection,int,__fastcall,(CriticalSection *lpCriticalSection,int arg))
DLL_ORD(fog,0x11860,10019,Ord10019,int,__fastcall,(char *serverName,void *func,char *libVersion,int one))
DLL_ORD(fog,0x11F00,10213,binarySearchTxtNo,int,__stdcall,(ItemCodeTxtNo *pItemLink,int itemCode,int zero))
DLL_ORD(fog,0x13CA0,10131,alignBitStreamToByte,int,__stdcall,(FogBits *bits))
DLL_ORD(fog,0x13D80,10127,getBitStreamBytes,int,__stdcall,(FogBits *bits))
DLL_ORD(fog,0x14040,10120,clearBit,void,__stdcall,(void *bitmap,int pos))
DLL_ORD(fog,0x14090,10118,setBit,void,__stdcall,(void *bitmap,int pos))
DLL_ORD(fog,0x14120,10128,writeBitStream,void,__stdcall,(FogBits *bits,int value,int nbit))
DLL_ORD(fog,0x141E0,10126,initBitStream,FogBits *,__stdcall,(FogBits *bits,char *buf,int size))
DLL_ORD(fog,0x17E40,10102,mpq_open_block,int,__fastcall,(char *path,mpq_block *ptr))
DLL_ORD(fog,0x17EF0,10101,Ord10101,int,__fastcall,(int zero1,int zero2))
DLL_ORD(fog,0x1CCC0,10046,freeMemory,int,__fastcall,(int arg0,void *ptr,char *source,int line,int arg4))
DLL_ORD(fog,0x1CCF0,10043,setCriticalSection,int,__fastcall,(CriticalSection *,char *source,int arg3,int zero))
DLL_ORD(fog,0x1CD10,10042,getCriticalSection,CriticalSection *,__fastcall,(int arg,char *source,int arg3,int zero))
DLL_ORD(fog,0x1CD50,10045,allocMemory,void *,__fastcall,(int arg0,int size,char *source,int line,int arg4))
DLL_ORD(fog,0x1CDA0,10047,realloc,void *,__fastcall,(int pMemPool,void *ptr,int size,char *source,int line,int arg4))
DLL_ORD(fog,0x1D7F0,10090,uninit,int,__stdcall,())
DLL_ORD(fog,0x1DF00,10089,Ord10089,int,__fastcall,(int one))
DLL_ORD(fog,0x1E7D0,10072,getMCPPacket,int,__stdcall,(void *ptr,char *buf,int size)) //return packet len
DLL_ORD(fog,0x1F550,10218,Ord10218,int,__fastcall,())
DLL_ORD(storm,0x22700,552,mpq_uncompress,int,__stdcall,(void *dst, int *psize, void *src,int src_size))
DLL_FUN(storm,0x25ED0,mpq_hashstring,int,__stdcall,(int group)) //eax:char *path
DLL_FUN(storm,0x26BD0,mk_abs_path,char *,__stdcall,(char *buf,void *arg1)) //eax:mpq_path
DLL_FUN(storm,0x28450,mpq_read_block,int,__stdcall,(mpq_block *b,int offset,char *outbuf,int size))//return size
DLL_ORD(storm,0x28960,268,mpq_open_block_from,int,__stdcall,(void *mpq, char *path, int flags,mpq_block *b)) 
DLL_ORD(storm,0x28DA0,267,mpq_open_block,int,__stdcall,(char *path, mpq_block *b))
DLL_ORD(storm,0x2A0B0,266,openMpq,int,__stdcall,(LPCSTR lpFileName,int dwPriority,int dwFlags,void *mpq))
DLL_ORD(storm,0x341D0,412,processWinMessage,int,__stdcall,(HWND hwnd,int msg,int wParam,int lParam,void *ptr1,void *ptr2))
DLL_ORD(storm,0x35DD0,372,processWinMessage372,int,__stdcall,(int x534D534A,HWND hwnd,int wParam,D2MSG *msg))
DLL_ORD(storm,0x3BA80,578,snprintf,int,__cdecl,(char *buf,int size,int fmt,...))
DLL_ORD(storm,0x3C2B0,422,regQueryValue,int,__stdcall,(char *keyName,char *valueName,int arg,char *buf,int len))
DLL_VAR(storm,0x53120,mpq_crypt_table,int) //size 0x500*4
DLL_VAR(storm,0x53130,mpq_load_flag,int) //bit0,1:loadExternal?
#undef DLL_VAR
#undef DLL_FUN
#undef DLL_ORD
#ifdef DEFINE_D2PTR
	#undef DEFINE_D2PTR
#endif
