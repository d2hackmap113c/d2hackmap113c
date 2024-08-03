#include "stdafx.h"

#include "d2ptrs.h"

void ShowVersion_Patch_ASM();
void NpcTradePatch_ASM();
void DrawCellFile_Patch_ASM();
void DrawAutomapCell_Patch_ASM();
void initSendPacketCheckTable();
void DropItemSendCmd17Patch_ASM();
void RecvCommand_22_Patch_ASM();
void RecvCommand_4D_Patch_ASM();
void RecvCommand_81_Patch_ASM();
void RecvCommand_9C_Patch_ASM();
void RecvCommand_9D_Patch_ASM();
void RecvCommand_A4_Patch_ASM();
void ResetSkillPatch_ASM();
void ReceiveSwapWeapon_ASM();
void ClickHireMercMenuItemPatch_ASM();
void LeftClick_Patch_ASM();
void RightClick_Patch_ASM();
int fLoadedLib=0;
BOOL fInstallPacketPatch=0,fInstallPatch2 = 0;
void StandStillPatch_ASM();
void *__fastcall SelectGamePatch(D2EditBox* box, wchar_t* wcszTxt);
static Patch_t packetPatchs[] = {
//--- m_PacketHandler.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF39A8),      (DWORD)GamePacketReceivedInterceptPatch_ASM, 5 , &fDefault},
};
static Patch_t aD2Patchs[] = {
//--- m_PacketHandler.h ---
	{PatchCALL,   DLLOFFSET(D2NET, 0x6FBF7650),      (DWORD)GamePacketSendInterceptPatch_ASM, 5 , &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAFA2A1),      (DWORD)ResetSkillPatch_ASM, 5 , &fDefault},
//--- m_DropProtection.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF9440),      (DWORD)ClickHireMercMenuItemPatch_ASM, 6 , &fDefault},
//--- m_WinMessage.h ---
	{PatchCALL,   DLLOFFSET(D2LAUNCH, 0x6FA5805B),    (DWORD)ShowVersion_Patch_ASM,           5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2WIN, 0x6F8F7740),    (DWORD)WinMessagePatch_ASM,           5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF33D1),    (DWORD)StandStillPatch_ASM,           10 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB5BF86),    (DWORD)ReceiveSwapWeapon_ASM,           6 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2GFX, 0x6FA8B0A1),    (DWORD)DrawCellFile_Patch_ASM,           6 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2GFX, 0x6FA8B00C),    (DWORD)DrawAutomapCell_Patch_ASM,           6 ,   &fDefault},
//--- m_pub.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF4230),    (DWORD)GameLoopPatch_ASM,               7 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF5153),    (DWORD)GameEndPatch_ASM,                5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAE8BB5),    (DWORD)KeydownPatch_ASM,                7 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAE8AE1),    (DWORD)KeyupPatch_ASM,                  6 ,   &fDefault},
//	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF25A0),    (DWORD)MButtonDownPatch_ASM,            5 ,   &fDefault},
//	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAE9120),    (DWORD)XButton1DownPatch_ASM,           5 ,   &fDefault},
//	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAE90A0),    (DWORD)XButton2DownPatch_ASM,           5 ,   &fDefault},

	{PatchVALUE,  DLLOFFSET(D2CLIENT, 0x6FACAB88),    0xEB,									  1 ,   &fDefault}, //取消进出传送门时间限制

	{PatchCALL,   DLLOFFSET(D2WIN,    0x6F8F881F),    (DWORD)InstallPatchAfterLoad_ASM,       5 ,   &fDefault},
//--- m_AutoMapCell.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB11354),    (DWORD)OverrideShrinePatch_ASM,         7 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB11D83),    (DWORD)AddShrinePatch_ASM,              6 ,   &fDefault},
  {PatchFILL,   DLLOFFSET(D2CLIENT, 0x6FB11D31),    INST_NOP,                               6 ,   &fDefault}, //shrine distance
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB104EA),    (DWORD)DrawAutomapCellPatch,            5 ,   &fDefault},
//--- m_RoomArea.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB10E16),    (DWORD)RoomPatch_ASM,                   6 ,   &fDefault},
//--- m_AutoMapBlob.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB11AC4),    (DWORD)ForceDrawBlob_ASM,           6 ,   &fDefault},
	{PatchJMP,    DLLOFFSET(D2CLIENT, 0x6FB0F1C0),    (DWORD)DrawBlobPath_ASM,            6 ,   &fDefault},
	{PatchJMP,    DLLOFFSET(D2CLIENT, 0x6FB11688),    (DWORD)MixedBlobColPatch_ASM,       6 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB10BD8),    (DWORD)DrawPartyPlayerBlobPatch_ASM,5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB1175D),    (DWORD)HostilePlayerColor_ASM,      6 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB11CD3),    (DWORD)DrawPlayerTextPatch_ASM,     11,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB11B67),    (DWORD)MonsterBlobDescPatch_ASM,    5,    &fDefault},
//--- m_Environment.h ---
  {PatchFILL,   DLLOFFSET(D2CLIENT, 0x6FB125C4),    INST_NOP,                               2 ,   &fDefault}, //floors inside
  {PatchFILL,   DLLOFFSET(D2CLIENT, 0x6FB1260A),    INST_NOP,                               2 ,   &fDefault}, //walls inside

  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB59A37),    (DWORD)LightingPatch_ASM,               6 ,   &fDefault},
  {PatchJMP,    DLLOFFSET(D2COMMON, 0x6FDBCCB6),    (DWORD)WeatherPatch_ASM,                5 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF42A2),    (DWORD)ShakeScreenPatch,                5 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB16625),    (DWORD)InfravisionPatch_ASM,            5 ,   &fDefault}, 
//--- m_MonsterDeath.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAD0FBE),    (DWORD)MonsterDeathPatch_ASM,           5 ,   &fDefault},
//--- m_ScrollLock.h ---
  {PatchFILL,   DLLOFFSET(D2CLIENT, 0x6FB3B16F),    INST_NOP,                               2 ,   &fDefault}, //force perspective shake
  {PatchFILL,   DLLOFFSET(D2CLIENT, 0x6FAEF8DF),    INST_NOP,                               2 ,   &fDefault}, //force get shake
  {PatchFILL,   DLLOFFSET(D2CLIENT, 0x6FB3B096),    INST_NOP,                               15,   &fDefault}, //kill add shake
  {PatchFILL,   DLLOFFSET(D2CLIENT, 0x6FB3B0A5),    INST_NOP,                               2 ,   &fDefault}, //kill add shake, part 2
  {PatchFILL,   DLLOFFSET(D2CLIENT, 0x6FB0FB10),    INST_NOP,                               0x37, &fDefault}, //kill automap scroll, wipe out 4 adjustments
//--- m_QuickBackTown.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5D2F0),   (DWORD)RecvCommand_60_Patch_ASM,        5 ,   &fDefault},
//--- m_PartyHelp.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5D6B0),   (DWORD)RecvCommand_5A_Patch_ASM,        5 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5D8F0),   (DWORD)RecvCommand_5B_Patch_ASM,        6 ,   &fDefault},
//--- m_OutTownSelect.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB56941),    (DWORD)OutTownSelectPatch_ASM1,         6 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FACBA6B),    (DWORD)OutTownSelectPatch_ASM2,         6 ,   &fDefault},
//--- m_AutoEnchant.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5D740),   (DWORD)RecvCommand_2C_Patch_ASM,        5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB56505),    (DWORD)AddUnitPatch_ASM,           6 ,   &fDefault},
//--- m_ScreenHook.h ---
 {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB73B96),    (DWORD)DrawClientPatch_ASM,             5 ,   &fDefault},//无视系统菜单显示
 {PatchJMP,    DLLOFFSET(D2CLIENT, 0x6FB5EEB1),    (DWORD)UpdateExp_ASM,                   5 ,   &fDefault},// just got a update player state packet
//--- m_MessageLog.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB2D8AD),    (DWORD)MessageLogPatch1_ASM,            6 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB2D673),    (DWORD)MessageLogPatch2_ASM,            5 ,   &fDefault},
//--- m_QuickSwapItem.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF4630),    (DWORD)RButtonUpHandlerPatch_ASM,       5 ,   &fDefault}, 
  {PatchJMP,    DLLOFFSET(D2CLIENT, 0x6FB3B976),    (DWORD)RButtonInTradePatch_ASM,         5 ,   &fDefault}, 
	
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB46CCE),    (DWORD)CtrlClickHandlerPatch_ASM,       6 ,   &fDefault}, 
  //{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB4B7F9),    (DWORD)LButtonUpHandlerPatch_ASM,       5 ,   &fDefault}, //家里箱子和身上箱子之间
  //{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB4B2D4),    (DWORD)LButtonUpHandlerPatch_ASM,       5 ,   &fDefault}, //cube和身上箱子之间
  //{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB4C81F),    (DWORD)LButtonUpHandlerPatch_ASM,       5 ,   &fDefault}, //左边空地和身上箱子之间
  //{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB4BC16),    (DWORD)LButtonUpHandlerPatch_ASM,       5 ,   &fDefault}, //交易界面和身上箱子之间
//--- m_SkipNpcMessage.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAFBB07),    (DWORD)NPCQuestMessageStartPatch_ASM,           6 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF8BD6),    (DWORD)NPCQuestMessageEndPatch1_ASM,            8 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF819F),    (DWORD)NPCQuestMessageEndPatch2_ASM,            5 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB2E9B7),    (DWORD)NPCMessageLoopPatch_ASM,					6 ,   &fDefault},
//--- m_TransGoldHelp.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB6FDF6),      (DWORD)SetGoldPatch_ASM,		6 , &fDefault},//设置转移的金额值
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB734DB),      (DWORD)SetDefaultGoldPatch_ASM, 5 , &fDefault},//设置默认值
//--- m_ULCPath.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB5F134),    (DWORD)ULCPatch_ASM,                    6 ,   &fDefault},
//--- m_NsPassBugFix.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAF5564),    (DWORD)NsPassingActBugFixPatch_ASM,       9 ,   &fDefault},//-ns bugfix for d2loader  
//--- m_RoomInfo.h ---
	{PatchJMP,    DLLOFFSET(D2CLIENT, 0x6FB126FE),    (DWORD)DrawGameInfoPatch,               5 ,   &fDefault},//游戏右上角信息显示
//--- m_GameFilter.h ---
	{PatchCALL,   DLLOFFSET(D2WIN ,		 0x6F8F8911),   (DWORD)DrawGameListPatch_ASM,           5,   &fGameFilter},
//--- m_PartyInfo.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB0B582),    (DWORD)DrawPetHeadPath_ASM,            7 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB0BBE0),    (DWORD)DrawPartyHeadPath_ASM,          6 ,   &fDefault},
//--- m_GameCount.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB5D8A1),    (DWORD)KillCountPatch_ASM,               6 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2COMMON, 0x6FD83942),    (DWORD)UnitModeChangePatch_ASM,          6 ,   &fDefault},
//--- m_GameMonitor.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5DEB1),   (DWORD)RecvCommand_A7_Patch_ASM,        9 ,   &fDefault},//设置状态1
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5DD74),   (DWORD)RecvCommand_A8_Patch_ASM,        9 ,   &fDefault},//设置状态2
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5DD31),   (DWORD)RecvCommand_A9_Patch_ASM,        9 ,   &fDefault},//结束状态
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5E090),   (DWORD)RecvCommand_9C_Patch_ASM,        5 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5E880),   (DWORD)RecvCommand_9D_Patch_ASM,        5 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5C801),   (DWORD)RecvCommand_22_Patch_ASM,        5 ,   &fDefault},
//--- m_ShowOrbs.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAD76B5),    (DWORD)ShowLifePatch_ASM,           5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FAD7767),    (DWORD)ShowManaPatch_ASM,           5 ,   &fDefault},
//--- m_ItemName.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB42366),    (DWORD)ItemNamePatch_ASM,               6 ,   &fDefault}, 
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB444F1),    (DWORD)OpenCubeStringPatch_ASM,         5 ,   &fDefault}, 
  {PatchVALUE,  DLLOFFSET(D2CLIENT, 0x6FB70017),    0xe990,                                 2 ,   &fDefault},  //kill ground gold name
//--- m_PermShowItem.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB73D4E),    (DWORD)PermShowItemsPatch1_ASM,         6 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB70E9A),    (DWORD)PermShowItemsPatch1_ASM,         6 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB09483),    (DWORD)PermShowItemsPatch2_ASM,         6 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB0908A),    (DWORD)PermShowItemsPatch3_ASM,         6 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB56BA3),    (DWORD)PermShowItemsPatch4_ASM,         6 ,   &fDefault},
//--- m_ItemDesc.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB05C1D),    (DWORD)ItemBasicStatPatch_ASM,          5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB05BF7),    (DWORD)GetItemExtStatListPatch,         5 ,   &fDefault},
	
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB43450),    (DWORD)ItemSocketViewPatch_ASM,         5 ,   &fDefault},
    {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB433E2),    (DWORD)SocketItemStatViewPatch_ASM,     5 ,   &fDefault},//镶嵌的物品只显示有效属性
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB43BE2),    (DWORD)ItemDescPath,                    5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB43C0A),    (DWORD)ItemDescPath,                    5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB433C5),    (DWORD)ItemDescPath,                    5 ,   &fDefault},
//--- m_ItemVariableProp.h ---
    {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB05DAA),    (DWORD)GetItemPropStringPatch_ASM,      5 ,   &fDefault},//普通属性
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB04658),    (DWORD)StrcatItemPropStringPatch_ASM,   6 ,   &fDefault},//增强伤害等特殊属性
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB3E2C5),    (DWORD)StrcatSocketStringPatch_ASM,     6 ,   &fDefault},//拼孔数，ETH等底部字符串，孔数可能为变量
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB3FDAD),    (DWORD)StrcatDefenseStringPatch_ASM,    6 ,   &fDefault},//拼防御值字符串
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB44367),    (DWORD)StrcatDamageStringPatch_ASM1,    6 ,   &fDefault},//拼攻击字符串,一般武器
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB42EE4),    (DWORD)StrcatDamageStringPatch_ASM2,    9 ,   &fDefault},//拼攻击字符串,绿色武器
//--- m_ItemExtInfo.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB73D91),    (DWORD)DrawHoverPatch_ASM,             5 ,   &fDefault},//单独找个地方显示，保持独立
//--- m_ViewUnitStat.h ---
{PatchJMP,    DLLOFFSET(D2CLIENT, 0x6FB6DC16),    (DWORD)ViewUnitStatPatch_ASM,         10 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB4AFCC),    (DWORD)ViewPetStatPatch_ASM,          6 ,   &fDefault},//pet显示
//--- m_ViewTargetUnitInfo.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB6CEA6),    (DWORD)GetViewStatUnit,                 5 ,   &fDefault}, //draw stat
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB6D743),    (DWORD)GetViewStatUnit,                 5 ,   &fDefault}, //draw stat
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB6CBA3),    (DWORD)GetViewStatUnit,                 5 ,   &fDefault}, //draw stat
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FADFE63),    (DWORD)GetViewingStatsPlayerPatch1_ASM, 6 ,   &fDefault}, //drawstat
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB6C524),    (DWORD)GetViewingStatsPlayerPatch2_ASM, 6 ,   &fDefault}, //draw stat

  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB453E2),    (DWORD)GetUnitDrawEquipItemPatch_ASM,   6 ,   &fDefault}, //draw equip items
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB44AB4),    (DWORD)GetUnitDrawItemDescPath_ASM,     6 ,   &fDefault}, //desc item
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB43A6F),    (DWORD)GetUnitTestHasItem_ASM,          5 ,   &fDefault}, //test has item for desc item
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB4A695),    (DWORD)GetUnitDrawInvItemPatch_ASM,     5 ,   &fDefault}, //draw inv items

  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB49120),    (DWORD)GetUnitProtectClickEquipPatch_ASM, 5 ,   &fDefault}, //protect click equip
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB4908B),    (DWORD)GetUnitProtectClickInvPatch_ASM, 7 ,   &fDefault}, //protect click inv
//--- m_MonsterLifeBar.h ---
{PatchCALL,   DLLOFFSET(D2WIN,    0x6F8F34D3),    (DWORD)LifeBarPatch_ASM,                6 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FACFE3B),    (DWORD)MonsterDescCommaPatch1_ASM,      5 ,   &fDefault},//给怪物的增强属性增加分隔符
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FACFF25),    (DWORD)MonsterDescCommaPatch2_ASM,      5 ,   &fDefault},//给怪物的增强属性增加分隔符
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB70C01),    (DWORD)MonsterDescCommaPatch3_ASM,      6 ,   &fDefault},//给怪物的增强属性增加分隔符
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB70B5F),    (DWORD)MonsterNamePatch_ASM,			  5 ,   &fDefault},
//--- m_PlayerLifeBar.h ---
 {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB70E88),    (DWORD)PlayerNamePatch_ASM,			  5 ,   &fDefault},
//--- m_QuestProtect.h ---
{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FACF7C9),    (DWORD)DrawQuestPagePatch_ASM,          6 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5F8F0),   (DWORD)RecvCommand_5D_Patch_ASM,        5 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB023B0),   (DWORD)RecvCommand_89_Patch_ASM,        5 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5D766),   (DWORD)RecvCommand_A4_Patch_ASM,        6 ,   &fDefault},
{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FB5F1A9),   (DWORD)RecvCommand_4D_Patch_ASM,        5 ,   &fDefault},
//--- m_ItemSocketProtect.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB49174),    (DWORD)SocketProtectPatch1_ASM,         6 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB46FAB),    (DWORD)SocketProtectPatch2_ASM,         6 ,   &fDefault},
//--- m_GameChat.h ---
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB1FEC7),    (DWORD)WisperPatch_ASM,                 5 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB20C6C),    (DWORD)InputLinePatch1_ASM,             5 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB2037B),    (DWORD)InputLinePatch2_ASM,             5 ,   &fDefault},
  {PatchVALUE,  DLLOFFSET(D2CLIENT, 0x6FB54745),     0x2EB58,                               3 ,   &fDefault}, 
  {PatchVALUE,  DLLOFFSET(D2CLIENT, 0x6FB546C5),     0x2EB58,                               3 ,   &fDefault},
  {PatchCALL,   DLLOFFSET(D2WIN ,   0x6F8F5F61),     (DWORD)SelectTextPatch1_ASM,			8 ,   &fDefault}, //聊天框选中中文文字
  {PatchCALL,   DLLOFFSET(D2WIN ,   0x6F8F5F02),     (DWORD)SelectTextPatch2_ASM,			8 ,   &fDefault}, //聊天框选中中文文字
//--- m_UnicodeSupport.h ---
  {PatchJMP,    DLLOFFSET(D2LANG, 0x6FC082F0),      (DWORD)D2Lang_Unicode2WinPatch,          5 ,   &fLocalizationSupport},
  {PatchJMP,    DLLOFFSET(D2LANG, 0x6FC08320),      (DWORD)D2Lang_Win2UnicodePatch,          5 ,   &fLocalizationSupport},
  {PatchCALL,   DLLOFFSET(D2WIN,  0x6F8F83A0),      (DWORD)ChannelEnterCharPatch,            5 ,   &fLocalizationSupport}, //注意InputLinePatch1_ASM必须结合这个，不然会引起部分堆栈错误(虽然不影响使用)
  {PatchCALL,   DLLOFFSET(BNCLIENT,0x6FF2FF5C),     (DWORD)MultiByteFixPatch_ASM,            6 ,   &fLocalizationSupport}, // for /w *acc msg text
  {PatchCALL,   DLLOFFSET(D2WIN , 0x6F8F1880),      (DWORD)IsPritableCharacterPatch_ASM,     8 ,   &fLocalizationSupport}, //繁体

  {PatchFILL,   DLLOFFSET(D2WIN,  0x6F8F30E9),		INST_NOP,							     2,	   &fLocalizationSupport},//强制tbl解析方式
  {PatchFILL,   DLLOFFSET(D2WIN,  0x6F8F30F8),		INST_JMPR,							     1,	   &fLocalizationSupport},//强制tbl解析方式
//--- m_Socket.h ---
	{PatchCALL,   DLLOFFSET(D2NET,    0x6FBF72D2),    (DWORD)FixSocketTimeOut_ASM,				  8 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2NET,    0x6FBF7458),    (DWORD)FixSocketErrRet_ASM,				  10 ,   &fFixSocketError},
//--- m_GameWindow.h ---
	{PatchCALL,   DLLOFFSET(D2WIN, 0x6F8F73F6),    (DWORD)NoHidePatch_ASM,               9 ,   &fDefault},
//--- m_MultiClient.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FACBDE5),   (DWORD)LeftClick_Patch_ASM,        5 ,   &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FACBC42),   (DWORD)RightClick_Patch_ASM,        5 ,   &fDefault},
//--- m_NpcTrade.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT , 0x6FAFAEE0),   (DWORD)NpcTradePatch_ASM,        5 ,   &fDefault},
//--- m_Snapshot.h ---
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB5F8B3),      (DWORD)RecvCommand_63_Patch_ASM, 5 , &fDefault},
	{PatchCALL,   DLLOFFSET(D2CLIENT, 0x6FB5EBA3),      (DWORD)RecvCommand_81_Patch_ASM, 6 , &fDefault},
};

static Patch_t aD2Patch2s[] = {
//--- m_pub.h ---
	//分一个给patch2占位置
	{PatchVALUE,  DLLOFFSET(D2CLIENT, 0x6FB2CB25),    0x3AEB,                                 2 ,   &fDefault}, //hover text bugfix,set old font
//--- m_QuickGame.h ---
	{PatchCALL,   DLLOFFSET(D2MULTI, 0x6F9E4A0B),   (DWORD)NextGameNamePatch,           5 ,   &fDefault},
    {PatchCALL,   DLLOFFSET(D2MULTI, 0x6F9E4A46),   (DWORD)NextGamePasswordPatch,       5 ,   &fDefault},
    {PatchCALL,   DLLOFFSET(D2MULTI, 0x6F9E4D29),   (DWORD)NextGameNamePatch,           5 ,   &fDefault},
    {PatchCALL,   DLLOFFSET(D2MULTI, 0x6F9E4D64),   (DWORD)NextGamePasswordPatch,       5 ,   &fDefault},
    {PatchCALL,   DLLOFFSET(D2MULTI, 0x6F9E4D8F),   (DWORD)NextGameDecriptionPatch,       5 ,   &fDefault},
    {PatchCALL,   DLLOFFSET(D2MULTI, 0x6F9E4808),   (DWORD)SelectGamePatch,       5 ,   &fDefault},
//--- m_GameFilter.h ---
    {PatchCALL,   DLLOFFSET(D2MCPCLIENT, 0x6FA263E0),   (DWORD)D2MCPPacketHandler_05_Patch_ASM,     5 ,   &fGameFilter},
	{PatchCALL,   DLLOFFSET(D2MULTI,	 0x6F9E49EF),   (DWORD)CreateGameBoxPatch_ASM,				5 ,   &fGameFilter},
	{PatchCALL,   DLLOFFSET(D2MULTI,	 0x6F9E1DC3),   (DWORD)DestroyGamelistPatch,				5 ,   &fGameFilter},
	{PatchCALL,   DLLOFFSET(D2MULTI,	 0x6F9DDF4E),   (DWORD)GameListRefreshTimePatch_ASM,		7 ,   &fGameFilter},
};

BOOL RelocD2Ptrs()
{
	DWORD *pPtrsStart = (DWORD *)&vD2StartOfPtr;
	DWORD *pPtrsEnd = (DWORD *)&vD2EndOfPtr;
	vD2ERR = 1;//防止这个值被优化
	DWORD *p = pPtrsStart, ret = TRUE;
	while (++p < pPtrsEnd){
		if (!(*p = GetDllOffset(*p))) ret = FALSE;
	};
	return ret;
}


BOOL InstallD2Patchs(Patch_t* pPatchStart, Patch_t* pPatchEnd)
{
  Patch_t* p = pPatchStart;
  while( p<pPatchEnd ){
    if ( !p->fInit){
      p->addr = GetDllOffset(p->addr);
      p->fInit = 1 ;
    }
    if (p->func && *(p->fEnable)){
      p->oldcode = new BYTE[p->len];
      ReadLocalBYTES(p->addr, p->oldcode, p->len);
      p->func(p->addr, p->param, p->len);
			if (0&&logfp) {
				fprintf(logfp,"Install Patch 0x%x oldcode[%d]={",p->addr,p->len);
				for (unsigned int i=0;i<p->len;i++) fprintf(logfp,"0x%02x,",p->oldcode[i]);
				fprintf(logfp,"};\r\n");
				fflush(logfp);
			}
    }
    p++;
  }
  return TRUE;
}

void RemoveD2Patchs(Patch_t* pPatchStart, Patch_t* pPatchEnd)
{
  Patch_t* p = pPatchStart;
  while( p<pPatchEnd) {
    if (p->oldcode && *(p->fEnable) ){
      WriteLocalBYTES(p->addr, p->oldcode, p->len);
      delete p->oldcode;
    }
    p++;
  }
}

//D2loader 存在bug，部分dll需要迟后加装

BOOL RelocD2Ptrs2()
{
	DWORD *pPtrsStart = (DWORD *)&vD2StartOfPtr2;
	DWORD *pPtrsEnd = (DWORD *)&vD2EndOfPtr2;
	DWORD *p = pPtrsStart, ret = TRUE;
	while (++p < pPtrsEnd){
		if (!(*p = GetDllOffset(*p))) ret = FALSE;
	};
	return ret;
}
void InstallPatchAfterLoad(){
	if ( !fInstallPatch2 ){
		RelocD2Ptrs2();
		InstallD2Patchs(aD2Patch2s, aD2Patch2s+_ARRAYSIZE(aD2Patch2s));
		fInstallPatch2 = 1;
	}
}
void __declspec(naked) InstallPatchAfterLoad_ASM()
{
	__asm{
		cmp [fInstallPatch2],1
		je org
		push edi
		call InstallPatchAfterLoad
		pop edi
org:
		pop ecx
		lea eax, dword ptr [esp+0x4]
		push eax
		jmp ecx
	}
}

BOOL InstallPatchs(){
	if (fMonitorPacket) {
		if(!InstallD2Patchs(packetPatchs, packetPatchs+_ARRAYSIZE(packetPatchs))){
			return FALSE;
		};
		fInstallPacketPatch=1;
	}
  if(!InstallD2Patchs(aD2Patchs, aD2Patchs+_ARRAYSIZE(aD2Patchs))){
    return FALSE;
  };

  #ifdef MODULE_GAMEWINDOW
	KeepWindowPatch();
  #endif

  return TRUE;
}

void RemovePatchs(){
	if (fInstallPacketPatch) {
		RemoveD2Patchs(packetPatchs, packetPatchs+_ARRAYSIZE(packetPatchs));
		fInstallPacketPatch=0;
	}
	RemoveD2Patchs(aD2Patchs, aD2Patchs+_ARRAYSIZE(aD2Patchs));
	if ( fInstallPatch2 ){
		RemoveD2Patchs(aD2Patch2s, aD2Patch2s+_ARRAYSIZE(aD2Patch2s));
		fInstallPatch2 = 0;
	}
	#ifdef MODULE_GAMEWINDOW
		RemoveKeepWindowPatch();
	#endif

}

void InitCellFiles()
{
	char filename[MAX_PATH];
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUMOF_SHRINES; j++) {
			sprintf(filename,"%sBMP\\%s%.2d.BMP",szPluginPath,"MINISHRINE"+(i?0:4) ,j);
			apMyShrineCells[i][j] = InitCellFile(LoadBmpCellFile(filename));
		}
	}
	sprintf(filename,"%sBMP\\%s",szPluginPath,"WAYPOINT.BMP");
	pWaypointCell = InitCellFile(LoadBmpCellFile(filename));
	sprintf(filename,"%sBMP\\%s",szPluginPath,"BLOBCELL.BMP");
	pMinimapObjectCell = InitCellFile(LoadBmpCellFile(filename));
	if(!pMinimapObjectCell) {
		sprintf(filename,"%sBMP\\%s",szPluginPath,"BLOBDOT.BMP");
		pMinimapObjectCell = InitCellFile(LoadBmpCellFile(filename));
	}

	for (int i = 0; i < _ARRAYSIZE(pUnitBlobCells); i++) {
		if (aszUnitBlobFiles[i]) {
			sprintf(filename,"%sBMP\\%s.BMP",szPluginPath, aszUnitBlobFiles[i]);
			pUnitBlobCells[i] = InitCellFile(LoadBmpCellFile(filename));
		}
	}

}

void DeleteCellFiles()
{
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < NUMOF_SHRINES; j++) {
			DeleteCellFile(apMyShrineCells[i][j]);
		}
	}
	DeleteCellFile(pWaypointCell);
	DeleteCellFile(pMinimapObjectCell);

	memset( apMyShrineCells ,			0 ,			sizeof(apMyShrineCells) );
	pWaypointCell = NULL;
	pMinimapObjectCell = NULL;

	for (int i = 0; i < _ARRAYSIZE(pUnitBlobCells); i++) {
		DeleteCellFile(pUnitBlobCells[i]);
	}
	memset( pUnitBlobCells ,			0 ,			sizeof(pUnitBlobCells) );
}



BOOL Install(){

	//重定位函数
	if(!RelocD2Ptrs()){
		return FALSE;
	}

	//加载配置文件
	if(!LoadConfig()){
		return FALSE;
	}

	//加载旁路点
	if(!InstallPatchs()){
		return FALSE;
	}
	
	//InitCellFiles();

	if (fAutomapPartyDefault != (BYTE)-1) D2SetAutomapParty(fAutomapPartyDefault==0?0:1);
	if (fAutomapNamesDefault != (BYTE)-1) D2SetAutomapNames(fAutomapNamesDefault==0?0:1);
	
	if (szLibraryPath[0]) {
		LPSTR cmdLine = GetCommandLine();
		int load=0;
		if (szLoadLibraryCmd[0]&&cmdLine) {
			int len=strlen(szLoadLibraryCmd);
			for (char *p=cmdLine;p;p=strchr(p+1,' ')) {
				while (*p==' ') p++;
				if (memcmp(szLoadLibraryCmd,p,len)==0) {load=1;break;}
			}
		}
		if (dwLoadLibraryKey>0&&GetKeyState(dwLoadLibraryKey&0xFF)&0x8000) load=1;
		if (load) {
			char path[512];
			if (szLibraryPath[0]&&szLibraryPath[1]==':') //absolute path
				sprintf(path,"%s",szLibraryPath);
			else
				sprintf(path,"%s%s",szPluginPath,szLibraryPath);
			LOG("Load custom library : %s\r\n", path);
			DWORD offset = (DWORD)LoadLibrary(path);
			if (!offset) {
				LOG("Load custom library %s failed\r\n", path);
			} else {
				HMODULE module = GetModuleHandle(path);
				FARPROC initFct = GetProcAddress(module,(LPCSTR)"_Init@0");
				if (!initFct) initFct = GetProcAddress(module,(LPCSTR)10000);
				if (initFct) initFct();
				fLoadedLib=1;
			}
		}
	}
	initSendPacketCheckTable();
	return TRUE;
}


void Uninstall(){
	
	#ifdef MODULE_AUTOMAPCELL
		RemoveMyAutomapCells();
	#endif
	
	DeleteCellFiles();
	RemovePatchs();

}

int ReloadConfig(){
	DeleteCellFiles();
	if (LoadConfig()){
		InitCellFiles();
		#ifdef MODULE_MINIMAPPOINT
		BuildMiniMapPoint(5);
		#endif

		ShowGameMessage("Config Reload Successful");
		ShowWarningMessage();
	}
	return 0;
}
