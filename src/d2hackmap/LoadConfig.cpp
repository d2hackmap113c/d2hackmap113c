#include "stdafx.h"

void AddMinimapConfig( DWORD dwLevelNo , WORD wPointNo ,BYTE fFlag , BYTE nColour );
MinimapConfig aMinimapConfigs[140];//配置中设定的待指向坐标
HANDLE confHeap=NULL;
ToggleVar *tKeyEventMap[8*256] = {0};
FILE *logfp=NULL;
static char *cfgFileName=NULL;
static char WarningMsg[200][100];
static int nMsgCount = 0 ,lineId;
struct Symbol {char *name,*value;};
#define TMP_SYMBOLS 128
struct Symbol *pSymbols=NULL;
struct Symbol tmpSymbols[TMP_SYMBOLS];
int nSymbols=0,capSymbols=0,nTmpSymbols=0,fSymbolSorted=0;

static ConfigVar aConfigVars[] = {
	{0,     "JustInHere",    (void*)-1     , 0 , {0,0,0,0}  },
//--- m_pub.h ---
	{CONFIG_VAR_TYPE_KEY, "ReloadConfigKey",			&tReloadConfig     },
	{CONFIG_VAR_TYPE_INT, "EnterGameSound",			&fEnterGameSound  ,   4 },
	{CONFIG_VAR_TYPE_INT, "ShowCFGCheckInfo",			&fShowCheckInfo  ,   4 },
	{CONFIG_VAR_TYPE_STR, "CFGVersionEng",			&szCFGVersion[0], 1,  {255 }},
	{CONFIG_VAR_TYPE_STR, "CFGVersionChi",			&szCFGVersion[1], 1,  {255 }},
	{CONFIG_VAR_TYPE_STR, "RuntimePath",			szRuntimePath, 1,  {255 }},
	{CONFIG_VAR_TYPE_INT, "EnableLanguageCheck",		&fLanguageCheck  ,   4 },
	{CONFIG_VAR_TYPE_INT, "LocalizationSupport",      &fLocalizationSupport,  4},
	{CONFIG_VAR_TYPE_INT, "AutoSetCHISupport",        &fAutoSetCHISupport,  4},
	{CONFIG_VAR_TYPE_INT, "LoadItemColours",        &fLoadItemColours,  4},
	{CONFIG_VAR_TYPE_INT, "EnterGameShowDifficultyMs",&dwEnterGameShowDifficultyMs,  4},

//--- m_AutoMap.h ---
	{CONFIG_VAR_TYPE_KEY, "RevealActAutomapKey",      &tRevealAct       },
	{CONFIG_VAR_TYPE_KEY, "RevealLevelAutomapKey",    &tRevealLevel     },
	{CONFIG_VAR_TYPE_KEY, "AutoMapToggle",            &tAutoMap         },
	{CONFIG_VAR_TYPE_INT, "AutoRevealAct",            &fAutoRevealAct   ,  4 },
//--- m_AutoMapCell.h ---
	{CONFIG_VAR_TYPE_KEY, "AutomapLevelNamesToggle",  &tAutomapLevelNames		},
	{CONFIG_VAR_TYPE_KEY, "MiniShrinesToggle",        &tMiniShrine			},
	{CONFIG_VAR_TYPE_INT, "LevelNameColour",          &nCaveNameTextCol , 1	},
	{CONFIG_VAR_TYPE_D2STR, "StaffTombLevelDesc",       &wszStaffTombLvlDesc, 1,  {50 }},
//--- m_RoomArea.h ---
  {CONFIG_VAR_TYPE_KEY, "AutomapActiveRoomsToggle",		&tAutomapActiveRoom     },
  {CONFIG_VAR_TYPE_KEY, "AutomapReadyRoomsToggle",		&tAutomapReadyRoom      },
  {CONFIG_VAR_TYPE_KEY, "AutomapScreenAreaToggle",		&tAutomapScreenArea     },
  {CONFIG_VAR_TYPE_INT, "AutomapActiveRoomsColour",		&nAutomapActiveRoomColors,  1 },
  {CONFIG_VAR_TYPE_INT, "AutomapReadyRoomsColour",		&nAutomapReadyColors  , 1 },
  {CONFIG_VAR_TYPE_INT, "AutomapScreenAreaColour",		&nAutomapScreenColors , 1 },
//--- m_MinimapPoint.h ---
	{CONFIG_VAR_TYPE_MINIMAPPOINT, "MinimapCells",             0			},
	{CONFIG_VAR_TYPE_KEY, "MinimapToggle",            &tMinimap			},
	{CONFIG_VAR_TYPE_INT, "MinimapCellColour",        &nMinimapCellCol,   1}, 
	{CONFIG_VAR_TYPE_INT, "MinimapSize",              &nMinimapSize,      1},
//--- m_AutoMapBlob.h ---
	{CONFIG_VAR_TYPE_INT, "AutomapPartyDefault",      &fAutomapPartyDefault , 1 },
	{CONFIG_VAR_TYPE_INT, "AutomapNamesDefault",      &fAutomapNamesDefault , 1 },
	{CONFIG_VAR_TYPE_KEY, "BossNameToggle",			&tBossNameToggle    },
	{CONFIG_VAR_TYPE_STR, "UnitBlobFiles",            &aszUnitBlobFiles ,   5,  {50}},
	{CONFIG_VAR_TYPE_STR, "PlayerBlobFile",           &aszUnitBlobFiles[0], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "MonsterBlobFile",          &aszUnitBlobFiles[1], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "DimVisionBlobFile",          &aszUnitBlobFiles[10], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "ObjectBlobFile",           &aszUnitBlobFiles[2], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "MissileBlobFile",          &aszUnitBlobFiles[3], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "ItemBlobFile",             &aszUnitBlobFiles[4], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "BossBlobFile",             &aszUnitBlobFiles[6], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "NpcBlobFile",              &aszUnitBlobFiles[7], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "MyBlobFile",               &aszUnitBlobFiles[8], 1,  {50}},
	{CONFIG_VAR_TYPE_STR, "CorpseBlobFile",           &aszUnitBlobFiles[9], 1,  {50}},
	{CONFIG_VAR_TYPE_KEY, "AutomapCorpsesToggle",    &tAutomapCorpse     },
	{CONFIG_VAR_TYPE_KEY, "AutomapMonstersToggle",   &tAutomapMonsters   },
	{CONFIG_VAR_TYPE_KEY, "AutomapChestsToggle",     &tAutomapChests     },
	{CONFIG_VAR_TYPE_KEY, "AutomapMissilesToggle",   &tAutomapMissiles     },
	{CONFIG_VAR_TYPE_KEY, "AutomapItemsToggle",      &tAutomapItems      },
	{CONFIG_VAR_TYPE_ARRAY, "MonsterColours",          &anMonsterColours,    1,{1000}},
	{CONFIG_VAR_TYPE_ARRAY, "MonsterCorpseColours",    &anMonsterCorpseColors, 1,{1000}},
	{CONFIG_VAR_TYPE_ARRAY, "MissileColours",          &anMissileColours,    1,{1000}},
	{CONFIG_VAR_TYPE_INT, "HostileMissileColour",    &nHostileMissileColor,  1 },
	{CONFIG_VAR_TYPE_INT, "GuidedMissileColour",     &nGuidedMissileColor, 1 },
	{CONFIG_VAR_TYPE_INT, "TracerMissileColour",     &nTracerMissileColor, 1 },
	{CONFIG_VAR_TYPE_INT, "OtherMissileColour",      &nOtherMissileColor,  1 },
	{CONFIG_VAR_TYPE_INT, "ClosedChestColour",       &nClosedChestColor,   1 },
	{CONFIG_VAR_TYPE_INT, "LockedChestColour",       &nLockedChestColor,   1 },
	{CONFIG_VAR_TYPE_INT, "NormalCorpseColour",      &nNormalCorpseColor,  1 },
	{CONFIG_VAR_TYPE_INT, "BossCorpseColour",        &nBossCorpseColor , 1 },
	{CONFIG_VAR_TYPE_INT, "MinionCorpseColour",      &nMinionCorpseColor,  1 },
	{CONFIG_VAR_TYPE_INT, "ChampionCorpseColour",    &nChampionCorpseColor,  1 },
	{CONFIG_VAR_TYPE_INT, "NormalMonsterColour",     &nMonsterNormalColor, 1 },
	{CONFIG_VAR_TYPE_INT, "BossMonsterColour",       &nMonsterBossColor  , 1 },
	{CONFIG_VAR_TYPE_INT, "MinionMonsterColour",     &nMonsterMinionColor, 1 },
	{CONFIG_VAR_TYPE_INT, "ChampionMonsterColour",   &nMonsterChampColor,  1 },
	{CONFIG_VAR_TYPE_INT, "SuperUniqueColour",       &nSuperUniqueColor  , 1 },
	{CONFIG_VAR_TYPE_INT, "Act5BarbarianColour",     &anMonsterColours[522], 1 },
	{CONFIG_VAR_TYPE_INT, "NeutralPlayerColour",     &nNeutralPlayerColor, 1 },
	{CONFIG_VAR_TYPE_INT, "HostilePlayerColour",     &nHostilePlayerColor, 1 },
	{CONFIG_VAR_TYPE_INT, "FriendMinionColour",      &nFriendMinionColor,  1 },
	{CONFIG_VAR_TYPE_INT, "PlayerMinionColour",      &nPlayerMinionColor,  1 },
	{CONFIG_VAR_TYPE_INT, "PartyPlayerTextColour",   &nPartyTextColour,    1 },
	{CONFIG_VAR_TYPE_INT, "NeutralPlayerTextColour", &nNeutralTextColour,  1 },
	{CONFIG_VAR_TYPE_INT, "HostilePlayerTextColour", &nHostileTextColour,  1 },
	{CONFIG_VAR_TYPE_INT, "SuperUniqueDescFont",	 &nSuperUniqueDescFont,1 },
	{CONFIG_VAR_TYPE_INT, "MonsterDescColour",        &nMonsterTextCol  , 1 },
	{CONFIG_VAR_TYPE_D2STR, "EnchantmentDescs",         &awszEnchantDescs,    50, {20 }},
	{CONFIG_VAR_TYPE_D2STR, "AuraDescs",                &awszAuraDescs,     7,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ImmunityDescs",            &awszImmuneDescs,   6,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ExtraStrongDesc",          &awszEnchantDescs[5], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ExtraFastDesc",            &awszEnchantDescs[6], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "CursedDesc",               &awszEnchantDescs[7], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "MagicResistantDesc",       &awszEnchantDescs[8], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "FireEnchantedDesc",        &awszEnchantDescs[9], 1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ChampionDesc",             &awszEnchantDescs[16],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "LightningEnchantedDesc",   &awszEnchantDescs[17],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ColdEnchantedDesc",        &awszEnchantDescs[18],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ThiefDesc",                &awszEnchantDescs[24],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ManaBurnDesc",             &awszEnchantDescs[25],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "TeleportationDesc",        &awszEnchantDescs[26],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "SpectralHitDesc",          &awszEnchantDescs[27],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "StoneSkinDesc",            &awszEnchantDescs[28],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "MultipleShotsDesc",        &awszEnchantDescs[29],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "GhostlyDesc",              &awszEnchantDescs[36],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "FanaticDesc",              &awszEnchantDescs[37],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "PossessedDesc",            &awszEnchantDescs[38],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "BerserkerDesc",            &awszEnchantDescs[39],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "MightAuraDesc",            &awszAuraDescs[0],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "HolyFireAuraDesc",         &awszAuraDescs[1],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "BlessedAimAuraDesc",       &awszAuraDescs[2],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "HolyFreezeAuraDesc",       &awszAuraDescs[3],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "HolyShockAuraDesc",        &awszAuraDescs[4],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ConvictionAuraDesc",       &awszAuraDescs[5],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "FanaticismAuraDesc",       &awszAuraDescs[6],    1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "PhysicalImmunityDesc",     &awszImmuneDescs[0],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "MagicImmunityDesc",        &awszImmuneDescs[1],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "FireImmunityDesc",         &awszImmuneDescs[2],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "LightningImmunityDesc",    &awszImmuneDescs[3],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "ColdImmunityDesc",         &awszImmuneDescs[4],  1,  {20 }},
	{CONFIG_VAR_TYPE_D2STR, "PoisonImmunityDesc",       &awszImmuneDescs[5],  1,  {20 }},
//--- m_Environment.h ---
  {CONFIG_VAR_TYPE_KEY, "FullVisualsToggle",		&tFullVisuals   },
  {CONFIG_VAR_TYPE_KEY, "WeatherToggle",			&tWeather		},
  {CONFIG_VAR_TYPE_KEY, "LightRadiusToggle",		&tLightRadiu    },
  {CONFIG_VAR_TYPE_KEY, "InfravisionToggle",		&tInfravision   },
  {CONFIG_VAR_TYPE_KEY, "ScreenShakeToggle",		&tScreenshake   },
  {CONFIG_VAR_TYPE_ARRAY, "MissileCorpses",         &afMissileCorpses,    1,{1000}},
  {CONFIG_VAR_TYPE_ARRAY, "MonsterCorpses",         &afMonsterCorpses,    1,{1000}},
  {CONFIG_VAR_TYPE_KEY, "HiddenCorpsesToggle",    &tHiddenCorpse      },
//--- m_MonsterDeath.h ---
	{CONFIG_VAR_TYPE_ARRAY, "MonsterDeathAnims",      &afMonsterDeathAnims, 1,{1000}},
//--- m_ScrollLock.h ---
  {CONFIG_VAR_TYPE_KEY, "ScrollMapToggle",         &tScrollMap       },
  {CONFIG_VAR_TYPE_KEY, "DiagonalScrollToggle",    &tDiagonalScroll    },
  {CONFIG_VAR_TYPE_KEY, "CenterMapScrollKey",      &tCenterMap },
  {CONFIG_VAR_TYPE_INT, "ScrollMode",              &nScrollMode      , 4 },
  {CONFIG_VAR_TYPE_INTARRAY, "MiniMapScrollKeys",    miniMapScrollKeys, 2,{9}},
  {CONFIG_VAR_TYPE_INTARRAY, "MiniMapScrollOffset",  miniMapScrollOffset, 2,{9}},
  {CONFIG_VAR_TYPE_ARRAY, "MapScrollOffset",        &nMapScrollOffset, 2,{9,5}},
  {CONFIG_VAR_TYPE_KEYARRAY, "MapScrollKey",         tMapScrollKey,1,{9},0,"MapScrollKey[%d]", ScrollMap  },
//--- m_QuickBackTown.h ---
	{CONFIG_VAR_TYPE_KEY, "QuickBackToTownKey",   &tBackToTown		    	},
	{CONFIG_VAR_TYPE_INT, "TownportalNumsAlert",  &nTownportalAlertNums, 4    },
//--- m_PartyHelp.h ---
  {CONFIG_VAR_TYPE_KEY, "AutoPartyToggle",      &tAutoParty         },
  {CONFIG_VAR_TYPE_KEY, "AutoInviteToggle",     &tAutoInvite        },
  {CONFIG_VAR_TYPE_KEY, "AutoLootPermit",       &tAutoPermit        },
  {CONFIG_VAR_TYPE_INT, "AutoPartyDelays",      &mAutoPartyDelays, 4},
//--- m_OutTownSelect.h ---
	{CONFIG_VAR_TYPE_KEY, "OutTownSelectToggle",    &tOutTownSelect     },
//--- m_WinMessage.h ---
  {CONFIG_VAR_TYPE_INT, "AutoCallToArm",				&dwAutoCallToArm,     4},
  {CONFIG_VAR_TYPE_KEY, "LockMouseToggle",       &tLockMouseToggle        },
  {CONFIG_VAR_TYPE_KEY, "AutoHideMinimapToggle",       &tAutoHideMinimapToggle        },
  {CONFIG_VAR_TYPE_KEY, "AutoTeleportMinimapToggle",       &tAutoMinimapTeleportToggle        },
  {CONFIG_VAR_TYPE_INT, "AutoHideMinimapKey",				&dwAutoHideMinimapKey,     4},
  {CONFIG_VAR_TYPE_INT, "QuickSwapItemDelayMs",				&dwQuickSwapItemDelayMs,     4},
	{CONFIG_VAR_TYPE_KEY, "FullScreenKey",			&tFullScreen     },
  {CONFIG_VAR_TYPE_INT, "AutoMaximizeWidth",				&dwAutoMaximizeWidth,     4},
  {CONFIG_VAR_TYPE_INT, "AutoMaximizeHeight",				&dwAutoMaximizeHeight,     4},
  {CONFIG_VAR_TYPE_INT, "HideCaptionBorder",				&dwHideCaptionBorder,     4},
  {CONFIG_VAR_TYPE_KEY, "AutoClickLeft",       &tAutoClickLeft        },
  {CONFIG_VAR_TYPE_KEY, "AutoClickRight",       &tAutoClickRight        },
  {CONFIG_VAR_TYPE_INT, "AutoClickLeftDownFs",				&dwAutoClickLeftDownFs,     4},
  {CONFIG_VAR_TYPE_INT, "AutoClickLeftUpFs",				&dwAutoClickLeftUpFs,     4},
  {CONFIG_VAR_TYPE_INT, "AutoClickRightDownFs",				&dwAutoClickRightDownFs,     4},
  {CONFIG_VAR_TYPE_INT, "AutoClickRightUpFs",				&dwAutoClickRightUpFs,     4},
  {CONFIG_VAR_TYPE_INT, "EnableScreenSaver",				&dwEnableScreenSaver,     4},
	{CONFIG_VAR_TYPE_INT, "LoadLibraryKey",			&dwLoadLibraryKey     ,4},
	{CONFIG_VAR_TYPE_STR, "LibraryPath",		szLibraryPath, 1,  {256 }},
	{CONFIG_VAR_TYPE_STR, "LoadLibraryCommand",		szLoadLibraryCmd, 1,  {256 }},
  {CONFIG_VAR_TYPE_INTARRAY, "GameControlKeys",    &dwGameControlKeys,  4, {64} },
  {CONFIG_VAR_TYPE_INTARRAY, "GameSoundKeys",    &dwGameSoundKeys,  1, {128} },
  {CONFIG_VAR_TYPE_KEYARRAY, "SwitchWindowKeys", tSwitchWindowKeys,1,{8},2,"SwitchWindowKeys[%d]", SwitchWindow  },
  {CONFIG_VAR_TYPE_KEYARRAY, "AccountKey",       &tAccountKeys,     1, {16},1,"Account[%d]",inputUserPassword },
  {CONFIG_VAR_TYPE_STRARRAY, "AccountName",      &szAccountNames,    64, {16} },
  {CONFIG_VAR_TYPE_STRARRAY, "AccountPassword",  &szAccountPasswords,64, {16} },
  {CONFIG_VAR_TYPE_INTARRAY, "SwitchSkillKeys",    &dwSwitchSkillKeys,  1, {32} },
  {CONFIG_VAR_TYPE_INTARRAY, "SwitchSkillLeft",    &dwSwitchSkillLeft,  4, {32,8} },
  {CONFIG_VAR_TYPE_INTARRAY, "SwitchSkillRight",    &dwSwitchSkillRight,  4, {32,8} },
  {CONFIG_VAR_TYPE_INTARRAY, "SwitchSkillLeftUp",    &dwSwitchSkillLeftUp,  1, {32,8} },
  {CONFIG_VAR_TYPE_INTARRAY, "SwitchSkillRightUp",    &dwSwitchSkillRightUp,  1, {32,8} },
  {CONFIG_VAR_TYPE_INTARRAY, "SwitchSkillStandStill",    &dwSwitchSkillStandStill,  1, {32} },
//--- m_AutoEnchant.h ---
	{CONFIG_VAR_TYPE_KEY, "AutoEnchantToggle",   &tAutoEnchant     },
  {CONFIG_VAR_TYPE_INT, "EnchantMana",				&dwEnchantMana,     4},
  {CONFIG_VAR_TYPE_INT, "RenewEnchantSeconds",				&dwRenewEnchantSeconds,     4},
  {CONFIG_VAR_TYPE_INT, "AutoEnchantScreenSaverDelayMs",				&dwAutoEnchantScreenSaverDelayMs,     4},
  {CONFIG_VAR_TYPE_INT, "AutoEnchantGroup",				&dwAutoEnchantGroup,     4},
  {CONFIG_VAR_TYPE_ARRAY, "AutoEnchantMonsterGroup",dwAutoEnchantMonsterGroup,       1, {1024}},
//--- m_AutoSummon.h ---
	{CONFIG_VAR_TYPE_KEY, "AutoSummonToggle",   &tAutoSummon     },
  {CONFIG_VAR_TYPE_INT, "AutoSummonMana",				&dwAutoSummonMana,     4},
  {CONFIG_VAR_TYPE_INT, "AutoSummonSwitchSkillMs",				&dwAutoSummonSwitchSkillMs,     4},
  {CONFIG_VAR_TYPE_INT, "AutoSummonSkeletonLevelAdjust",				&dwAutoSummonSkeletonLevelAdjust,     4},
  {CONFIG_VAR_TYPE_INT, "AutoSummonSkeletonMageLevelAdjust",				&dwAutoSummonSkeletonMageLevelAdjust,     4},
//--- m_Snapshot.h ---
	{CONFIG_VAR_TYPE_STR, "SnapshotPath",		szSnapshotPath, 1,  {255 }},
	{CONFIG_VAR_TYPE_STR, "SnapshotNamesPath",		szSnapshotNamesPath, 1,  {255 }},
	{CONFIG_VAR_TYPE_KEY, "SnapshotKey",			&tDoSnapshot     },
	{CONFIG_VAR_TYPE_KEY, "SnapshotNewGame",			&tSnapshotNewGame     },
	{CONFIG_VAR_TYPE_KEY, "SnapshotEndGame",			&tSnapshotEndGame     },
	{CONFIG_VAR_TYPE_KEY, "SnapshotLog",			&tSnapshotLog     },
	{CONFIG_VAR_TYPE_KEY, "SnapshotSaveD2s",			&tSnapshotSaveD2s     },
//--- m_ScreenHook.h ---
	{CONFIG_VAR_TYPE_KEY, "ShowExpToggle",	&tShowExp   },
	{CONFIG_VAR_TYPE_KEY, "ShowPingToggle",	&tShowPing  }, 
//--- m_MessageLog.h ---
  {CONFIG_VAR_TYPE_KEY, "MessageLogToggle",    &tLogMessage},
  {CONFIG_VAR_TYPE_INT, "MessageLogFileSize",  &dwMessageFileSize,4},
//--- m_QuickSwapItem.h ---
  {CONFIG_VAR_TYPE_KEY, "RightClickSwapToggle",  &tRightClickSwap   },
  //{CONFIG_VAR_TYPE_KEY, "CtrlClickSwapToggle",   &tCtrlClickSwap    },
  {CONFIG_VAR_TYPE_KEY, "QuickDropToggle",       &tQuickDropToggle  },
//--- m_SkipNpcMessage.h ---
	{CONFIG_VAR_TYPE_INT, "SkipQuestMessage",   &mSkipQuestMessage,   4},
//--- m_ULCPath.h ---
	{CONFIG_VAR_TYPE_KEY, "ULCMaskToggle",	 &tULCMask       },
//--- m_RoomInfo.h ---
  {CONFIG_VAR_TYPE_KEY, "ServerIpToggle",			&tServerIp        },
  {CONFIG_VAR_TYPE_KEY, "GameTimeToggle",         &tGameTime        },
  {CONFIG_VAR_TYPE_KEY, "ClockToggle",            &tClock           },
  {CONFIG_VAR_TYPE_KEY, "LayerLevelNoToggle",     &tLayerLevelNo    },
  {CONFIG_VAR_TYPE_KEY, "AreaLevelToggle",        &tAreaLevel       },
//--- m_QuickGame.h ---
  {CONFIG_VAR_TYPE_KEY, "QuickExitGameKey",      &tExitGame              },
  {CONFIG_VAR_TYPE_KEY, "QuickNextGameKey",      &tQuickNextGame         },
  {CONFIG_VAR_TYPE_KEY, "QuickNextGameKey2",     &tQuickNextGame2        },

  {CONFIG_VAR_TYPE_INT, "AutoNextGameName",      &fAutoNextGameName  ,     4 },
  {CONFIG_VAR_TYPE_INT, "AutoNextGamePassword",  &fAutoNextGamePassword  , 4 },
  {CONFIG_VAR_TYPE_INT, "AutoNextGameDescription",  &fAutoNextGameDescription  , 4 },
  
  {CONFIG_VAR_TYPE_WSTR, "DefaultGameName",       &wszGameName,     1,  {32 }},
  {CONFIG_VAR_TYPE_WSTR, "DefaultGamePassword",   &wszGamePassword, 1,  {32 }},
  {CONFIG_VAR_TYPE_WSTR, "DefaultGameDescription",   &wszGameDescription, 1,  {32 }},
//--- m_GameFilter.h ---
	{CONFIG_VAR_TYPE_INT, "GameFilterSupport",     &fGameFilter,  4},
//--- m_PartyInfo.h ---
	{CONFIG_VAR_TYPE_KEY, "ShowPartyPositionToggle",		&tShowPartyPosition     },
	{CONFIG_VAR_TYPE_KEY, "ShowPartyLevelToggle",			&tShowPartyLevel		},
//--- m_GameCount.h ---
	{CONFIG_VAR_TYPE_KEY,     "KillCountToggle",		&tKillCount         },
	{CONFIG_VAR_TYPE_KEY,     "GetHitCountToggle",	&tGetHitCount         },
//--- m_GameMonitor.h ---
	{CONFIG_VAR_TYPE_GAMEMONITOR, "StateMonitor",          0			},
	{CONFIG_VAR_TYPE_KEY, "StateMonitorToggle",		&tStateMonitorToggle     },
	{CONFIG_VAR_TYPE_INT, "MonitorQuantity",     &fMonitorQuantity,  4},
	{CONFIG_VAR_TYPE_INT, "MonitorDurability",     &fMonitorDurability,  4},
	{CONFIG_VAR_TYPE_KEY, "CountDown",		&tCountDown},
	{CONFIG_VAR_TYPE_INT, "CountDownFontSize",		&dwCountDownFontSize,4},
	{CONFIG_VAR_TYPE_INT, "GameMonitorX",		&dwGameMonitorX,4},
	{CONFIG_VAR_TYPE_INT, "GameMonitorY",		&dwGameMonitorY,4},
	{CONFIG_VAR_TYPE_INT, "CountDownGap",		&dwCountDownGap,4},
	{CONFIG_VAR_TYPE_INT, "CountDownFlashSecond",		&dwCountDownFlashSecond,4},
//--- m_ShowOrbs.h ---
	{CONFIG_VAR_TYPE_KEY, "PermShowOrbsToggle",    &tPermShowOrb       },
//--- m_ItemDefine.h ---
  {CONFIG_VAR_TYPE_ARRAY, "ItemColours",       &anItemColours,       4, {3000 , 8 , 2 , 7}},
  {CONFIG_VAR_TYPE_ARRAY, "WeaponColours",     &anItemColours,       4, {1000 , 8 , 2 , 7}},
  {CONFIG_VAR_TYPE_ARRAY, "ArmorColours",      &anItemColours[1000], 4, {1000 , 8 , 2 , 7}},
  {CONFIG_VAR_TYPE_ARRAY, "MiscItemColours",   &anItemColours[2000], 4, {1000 , 8 , 2 , 7}},
  {CONFIG_VAR_TYPE_ARRAY, "RuneColours",       &anRuneColours,       4, {100} },
  {CONFIG_VAR_TYPE_ARRAY, "CharmColours",      &anItemColours[2095], 4, {3, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "RejuvPotColours",   &anItemColours[2007], 4, {2, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "HealingPotColours", &anItemColours[2079], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "ManaPotColours",    &anItemColours[2084], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "AmethystColours",   &anItemColours[2049], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "TopazColours",      &anItemColours[2054], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "SapphireColours",   &anItemColours[2059], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "EmeraldColours",    &anItemColours[2064], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "RubyColours",       &anItemColours[2069], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "DiamondColours",    &anItemColours[2074], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "SkullColours",      &anItemColours[2089], 4, {5, 8, 2, 7} },
  {CONFIG_VAR_TYPE_ARRAY, "ItemHideLvl",       &anItemHideLevel,     1, {3000 , 8} },
  {CONFIG_VAR_TYPE_ARRAY, "ItemShowCLvl",       &anItemShowCLevel,     1, {3000} },
  {CONFIG_VAR_TYPE_ARRAY, "GoodGoldColour",      &anGoldGoodCol,     4,{1}},
  {CONFIG_VAR_TYPE_ARRAY, "PoorGoldColour",      &anGoldPoorCol,     4,{1}},
  {CONFIG_VAR_TYPE_INT, "GoodGoldNumber",      &dwGoldGoodNum,     4 },
  {CONFIG_VAR_TYPE_INT, "DefaultItemShowMode", &nItemShowMode,     4 },
  {CONFIG_VAR_TYPE_KEY, "SwitchItemShowMode",  &tSwitchItemMode    },
//--- m_ItemName.h ---
  {CONFIG_VAR_TYPE_INT, "SimplifiedPotionName",        &fSimplifiedPotionName, 1 },
  {CONFIG_VAR_TYPE_INT, "QuestItemColour",        &nQuestItemColour, 1 },
  {CONFIG_VAR_TYPE_KEY, "RuneNumbersToggle",      &tRuneNumbers        },
  {CONFIG_VAR_TYPE_KEY, "SocketNumbersToggle",    &tSocketNumbers      },
  {CONFIG_VAR_TYPE_KEY, "ItemLevelsToggle",       &tItemLevel       },
  {CONFIG_VAR_TYPE_KEY, "ItemValueToggle",        &tItemValue       },
  {CONFIG_VAR_TYPE_KEY, "ItemIndexsToggle",       &tItemIndex       },
  {CONFIG_VAR_TYPE_KEY, "ItemFileIndexsToggle",   &tItemFileIndex   },
  {CONFIG_VAR_TYPE_KEY, "UnitNumbersToggle",      &tUnitNumber      },
  {CONFIG_VAR_TYPE_D2STR, "EtherealItemPrefix",     &wszEtherealItemPrefix, 1,  {30 }},
  {CONFIG_VAR_TYPE_D2STR, "EtherealItemPostfix",    &wszEtherealItemPostfix,1,  {30 }},
  
  {CONFIG_VAR_TYPE_KEY, "ItemQLevelToggle",       &tItemQLevel      },
  {CONFIG_VAR_TYPE_KEY, "ItemMLevelToggle",       &tItemMLevel      },
  {CONFIG_VAR_TYPE_KEY, "ItemALevelToggle",       &tItemALevel      },
  {CONFIG_VAR_TYPE_KEY, "ItemAfterCubeToggle",    &tAfterCube       },

  {CONFIG_VAR_TYPE_KEY, "ItemLifePerLevelToggle", &tLifePerLevel    },
  {CONFIG_VAR_TYPE_KEY, "ShowBaseEDToggle",           &tShowBaseED      },
//--- m_ItemInGround.h ---
  {CONFIG_VAR_TYPE_KEY, "ShowIdentifiedItem",    &tShowIdentifiedItem	 },
  {CONFIG_VAR_TYPE_KEY, "HiddenItemsToggle",     &tHiddenItems         },
//--- m_PermShowItem.h ---
	{CONFIG_VAR_TYPE_KEY, "PermShowItemsToggle",		&tPermShowItems     },
	{CONFIG_VAR_TYPE_KEY, "LockAltToggle",			&tLockAltToggle     },
//--- m_ItemDesc.h ---
  {CONFIG_VAR_TYPE_KEY, "ViewSocketablesToggle",		&tViewSocketable    },
  {CONFIG_VAR_TYPE_KEY, "ItemBasicStatsToggle",		&tItemBasicStat     },
  {CONFIG_VAR_TYPE_KEY, "ViewSocketBaseItemsToggle",  &tViewSocketBase    },
//--- m_ItemVariableProp.h ---
	{CONFIG_VAR_TYPE_KEY, "ShowItemVariablePropToggle",					&tShowItemVariableProp         },
//--- m_ItemExtInfo.h ---
	{CONFIG_VAR_TYPE_STR, "ItemExtInfoFileEng",		&szItemExtInfoCfgName[0], 1,  {255 }},
	{CONFIG_VAR_TYPE_STR, "ItemExtInfoFileChi",		&szItemExtInfoCfgName[1], 1,  {255 }},
	{CONFIG_VAR_TYPE_KEY, "ItemExtInfoToggle",		&tItemExtInfo         },
	{CONFIG_VAR_TYPE_INT, "DefaultExtInfoColour",     &nDefaultExtInfoColour,   1 },
//--- m_ViewUnitStat.h ---
  {CONFIG_VAR_TYPE_KEY, "FirstPlayerStatKey",     &tFirstPlayerStat   },
  {CONFIG_VAR_TYPE_KEY, "NextPlayerStatKey",      &tNextPlayerStat    },
  {CONFIG_VAR_TYPE_KEY, "PreviousPlayerStatKey",  &tPrevPlayerStat    },
  {CONFIG_VAR_TYPE_KEY, "ShowBaseStat",           &tShowBaseStat      },
//--- m_ViewTargetUnitInfo.h ---
  {CONFIG_VAR_TYPE_KEY, "ViewEquipmentKey",      &tViewEquipmentKey    },
  {CONFIG_VAR_TYPE_KEY, "ViewPlayerStatsKey",    &tViewPlayerStatsKey  },
//--- m_MonsterLifeBar.h ---
  {CONFIG_VAR_TYPE_KEY, "MonsterTCToggle",         &tMonsterTC         },
  {CONFIG_VAR_TYPE_KEY, "MonsterRuneTCToggle",     &tMonsterRuneTC     },
  {CONFIG_VAR_TYPE_KEY, "MonsterLevelToggle",      &tMonsterLevel      },
  {CONFIG_VAR_TYPE_KEY, "MonsterResistsToggle",    &tMonsterResists    },                                       
  {CONFIG_VAR_TYPE_KEY, "MonsterHPPercentToggle",  &tMonsterHPPercent  },
  {CONFIG_VAR_TYPE_KEY, "MonsterDistanceToggle",   &tMonsterDistance   },
  {CONFIG_VAR_TYPE_KEY, "MonsterIDToggle",		 &tMonsterID		 },
  {CONFIG_VAR_TYPE_KEY, "SinglePlayerMonsterHPToggle",  &tSPMonsterHP  },
  {CONFIG_VAR_TYPE_KEY, "SinglePlayerMonsterExpHPToggle",  &tSPMonsterExp  },
  {CONFIG_VAR_TYPE_INT, "LifeBarTransparency",     &nLifeBarTrans  ,   1 },
  {CONFIG_VAR_TYPE_INT, "LifeBarColour",           &nLifeBarColour ,   1 },
//--- m_PlayerLifeBar.h ---
  {CONFIG_VAR_TYPE_KEY, "PlayerLevelToggle",       &tPlayerLevel       },
  {CONFIG_VAR_TYPE_KEY, "PlayerDistanceToggle",    &tPlayerDistance    },
  {CONFIG_VAR_TYPE_KEY, "PlayerHPPercentToggle",   &tPlayerHPPercent   },
//--- m_CheckDangerous.h ---
  {CONFIG_VAR_TYPE_INT, "AutoPotionCheckMs",			&dwAutoPotionCheckMs,     4},
  {CONFIG_VAR_TYPE_INT, "IronGolemLifeAlertPercent",&dwIronGolemLifeAlertPercent,     4},
  {CONFIG_VAR_TYPE_INT, "IronGolemLifeTownPercent",&dwIronGolemLifeTownPercent,     4},
  {CONFIG_VAR_TYPE_INT, "IronGolemLifeExitPercent",&dwIronGolemLifeExitPercent,     4},
  {CONFIG_VAR_TYPE_INT, "HirePotionMinCLevel",	&dwHirePotionMinCLevel,     4},
  {CONFIG_VAR_TYPE_INT, "HirePotionLifePercent",	&dwHirePotionLifePercent,     4},
  {CONFIG_VAR_TYPE_INT, "HirePotionColumn",				&dwHirePotionColumn,     4},
  {CONFIG_VAR_TYPE_INT, "HirePotionDelayMs",			&dwHirePotionDelayMs,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionMax",				&dwManaPotionMax,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionValue",				&dwManaPotionValue,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionNTValue",				&dwManaPotionNTValue,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionColumn",				&dwManaPotionColumn,     4},
  {CONFIG_VAR_TYPE_INT, "ManaPotionDelayMs",			&dwManaPotionDelayMs,     4},
  {CONFIG_VAR_TYPE_KEY, "ChickenLifeToggle",          &tChickenLife     },
  {CONFIG_VAR_TYPE_KEY, "ChickenHostileToggle",		&tChickenHostile    },
  {CONFIG_VAR_TYPE_KEY, "ChickenHostileNearbyToggle", &tChickenHostileNearby  },
  {CONFIG_VAR_TYPE_INT, "ChickenMaxLife",				&dwChickenMaxLife,     4},
  {CONFIG_VAR_TYPE_INT, "ChickenLife",				&dwChickenLife,     4},
  {CONFIG_VAR_TYPE_INT, "ChickenHostileLife",			&dwChickenHostileLife,  4},
  {CONFIG_VAR_TYPE_INT, "ChickenHostileNearbyLife",   &dwChickenHostileNearbyLife,4},
  {CONFIG_VAR_TYPE_INT, "ChickenLifePercent",			&dwChickenLifePercent,  4},
  {CONFIG_VAR_TYPE_INT, "ChickenHostileLifePercent",  &dwChickenHostileLifePercent,4},
  {CONFIG_VAR_TYPE_INT, "ChickenHostileNearbyLifePercent", &dwChickenHostileNearbyLifePercent,4},
  {CONFIG_VAR_TYPE_INT, "ChickenLifeAction",          &nChickenLifeAction,  4},
  {CONFIG_VAR_TYPE_ARRAY, "DangerousMonster",           &anDangerousMonster,  2,{1000}},
  {CONFIG_VAR_TYPE_KEY, "DangerousMonsterChickenToggle", &tChickenDangerousMonster},
  {CONFIG_VAR_TYPE_KEY, "AutoPotionToggle", &tAutoPotion},
  {CONFIG_VAR_TYPE_INT, "DangerousMonsterAction",     &nDangerousMonsterAction, 4},
  {CONFIG_VAR_TYPE_INT, "ChickenLifeMinClevel",				&dwChickenLifeMinClevel,     4},
  {CONFIG_VAR_TYPE_INT, "ChickenLifeForcedClevel",				&dwChickenLifeForcedClevel,     4},
  {CONFIG_VAR_TYPE_INT, "AutoProtectionNewGame",				&dwChickenLifeEnterGame,     4},
//--- m_QuestProtect.h ---
  {CONFIG_VAR_TYPE_KEY, "BugKCountessToggle",          &tBugKCountess         },
  {CONFIG_VAR_TYPE_KEY, "BugKMToggle",          &tBugKM         },
  {CONFIG_VAR_TYPE_KEY, "BugKDToggle",          &tBugKD         },
  {CONFIG_VAR_TYPE_KEY, "BugKBToggle",          &tBugKB         },
  {CONFIG_VAR_TYPE_KEY, "BugAutoQuitToggle",    &tBugAutoQuit   },
  {CONFIG_VAR_TYPE_KEY, "BugAutoQuitHellToggle",    &tBugAutoQuitHell   },
  {CONFIG_VAR_TYPE_KEY, "BugAutoQuitHellAct5Toggle",    &tBugAutoQuitHellAct5   },
  {CONFIG_VAR_TYPE_KEY, "BugAllHellAlertToggle",    &tBugAllHellAlert   },
  {CONFIG_VAR_TYPE_INT, "BugAlertTimes",        &dwBugAlertTimes ,4 },
  {CONFIG_VAR_TYPE_KEY, "AlertNoBug",           &tAlertNoBug    },
  {CONFIG_VAR_TYPE_WSTR, "BugKBAlertMessage",       &wszBugKBAlertMessage,     1,  {256 }},
//--- m_IMEProtect.h ---
	{CONFIG_VAR_TYPE_KEY, "SocketProtectToggle",   &tSocketProtect   },
//--- m_GameChat.h ---
	{CONFIG_VAR_TYPE_KEY, "WisperNoticeToggle",     &tWisperNotice      },
	{CONFIG_VAR_TYPE_KEY, "InputLineToggle",        &tInputLine         },
	{CONFIG_VAR_TYPE_KEY, "UseCustomFontToggle",    &tUseCustomFont     },
//--- m_Socket.h ---
	{CONFIG_VAR_TYPE_INT, "SocketTimeOutValue",  &dwSocketTimeOutSec,4},
	{CONFIG_VAR_TYPE_INT, "FixSocketError",	   &fFixSocketError,4},
//--- m_GameWindow.h ---
	{CONFIG_VAR_TYPE_KEY, "KeepGameWindowToggle",   &tKeepGameWindow    },
	{CONFIG_VAR_TYPE_KEY, "NoHideToggle",			  &tNoHide     },		
//--- m_MultiClient.h ---
	{CONFIG_VAR_TYPE_KEY, "MultiClientToggle",          &tMultiClient         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientStartFollowKey",          &tMultiClientStartFollow         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientStopFollowKey",          &tMultiClientStopFollow         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientEnterDoorKey",          &tMultiClientEnterDoor         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientRetreatKey",          &tMultiClientRetreat         },
	{CONFIG_VAR_TYPE_KEY, "MultiClientTransferClickKey",          &tMultiClientClick         },
	{CONFIG_VAR_TYPE_INT, "MultiClientDistance",	   &dwMultiClientDistance,4},
	{CONFIG_VAR_TYPE_INT, "MultiClientMaxDistance",	   &dwMultiClientMaxDistance,4},
	{CONFIG_VAR_TYPE_INT, "MultiClientMoveDistance",	   &dwMultiClientMoveDistance,4},
	{CONFIG_VAR_TYPE_INT, "MultiClientOverAreaDistance",	   &dwMultiClientOverAreaDistance,4},
	{CONFIG_VAR_TYPE_KEY, "NecNoAttackInHellToggle",          &tNecNoAttackInHell         },
//--- m_DropProtection.h ---
	{CONFIG_VAR_TYPE_KEY, "DropProtectionToggle",          &tDropProtectionToggle         },
	{CONFIG_VAR_TYPE_KEY, "ResetProtectionToggle",          &tResetProtectionToggle         },
  {CONFIG_VAR_TYPE_ARRAY, "DropProtectionRune",       &aDropProtectRune,       1, {34}},
  {CONFIG_VAR_TYPE_ARRAY, "DropProtectionRuneword",       &aDropProtectRuneword,       1, {256}},
  {CONFIG_VAR_TYPE_ARRAY, "DropProtectionUnique",       &aDropProtectUnique,       1, {512}},
  {CONFIG_VAR_TYPE_ARRAY, "DropProtectionSet",       &aDropProtectSet,       1, {256}},
//--- m_PacketHandler.h ---
	{CONFIG_VAR_TYPE_INT, "MonitorPacket",	   &fMonitorPacket,4},
	{CONFIG_VAR_TYPE_KEY, "PacketHandlerToggle",          &tPacketHandler         },
  {CONFIG_VAR_TYPE_ARRAY, "MonitorSendCmd",       &fMonitorSendCmd,       1, {256}},
  {CONFIG_VAR_TYPE_ARRAY, "MonitorRecvCmd",       &fMonitorRecvCmd,       1, {256}},
	{CONFIG_VAR_TYPE_KEY, "TestKey",          &tTestKey         },
	{CONFIG_VAR_TYPE_KEY, "NecTeleportAttractToggle",          &tNecTeleportAttract         },
};



int compareConfigVar(const void *a,const void *b) {
	return _stricmp(((struct ConfigVar *)a)->szCmdName,((struct ConfigVar *)b)->szCmdName);
}
int compareSymbol(const void *a,const void *b) {
	return _stricmp(((struct Symbol *)a)->name,((struct Symbol *)b)->name);
}
ConfigVar *findConfigVar(char *name) {
	struct ConfigVar c;c.szCmdName=name;
	return (ConfigVar *)bsearch(&c,aConfigVars,_ARRAYSIZE(aConfigVars),sizeof(struct ConfigVar),compareConfigVar);
}

void InitValues(){
	static char *base0arrays[]={
		"DropProtectionRuneword","DropProtectionUnique", 
		"DropProtectionSet","MonitorSendCmd","MonitorRecvCmd"};
	qsort(aConfigVars,_ARRAYSIZE(aConfigVars),sizeof(struct ConfigVar),compareConfigVar);
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) {
		aConfigVars[i].base=1;
	}
	for (int i=0;i<_ARRAYSIZE(base0arrays);i++) {
		ConfigVar *pcv=findConfigVar(base0arrays[i]);
		if (pcv) pcv->base=0;
		else LOG("Can't find %s\n",base0arrays[i]);
	}
	if (!capSymbols) {
		capSymbols=8192;
		pSymbols=(struct Symbol *)malloc(capSymbols*sizeof(struct Symbol));
	}
	nSymbols=0;
	memset(tKeyEventMap ,       0 ,      sizeof(tKeyEventMap) );
	memset(tAccountKeys ,       0 ,      sizeof(tAccountKeys) );
	memset(tSwitchWindowKeys ,       0 ,      sizeof(tSwitchWindowKeys) );
	memset(szAccountNames ,       0 ,      sizeof(szAccountNames) );
	memset(szAccountPasswords ,       0 ,      sizeof(szAccountPasswords) );
	memset(dwGameControlKeys ,       0 ,      sizeof(dwGameControlKeys) );
	memset(dwGameSoundKeys ,       0 ,      sizeof(dwGameSoundKeys) );
	memset(dwSwitchSkillKeys ,       0 ,      sizeof(dwSwitchSkillKeys) );
	memset(dwSwitchSkillLeft ,       0xFF ,      sizeof(dwSwitchSkillLeft) );
	memset(dwSwitchSkillRight ,       0xFF ,      sizeof(dwSwitchSkillRight) );
	memset(dwSwitchSkillLeftUp ,       0xFF ,      sizeof(dwSwitchSkillLeftUp) );
	memset(dwSwitchSkillRightUp ,       0xFF ,      sizeof(dwSwitchSkillRightUp) );
	memset(dwSwitchSkillStandStill ,       0 ,      sizeof(dwSwitchSkillStandStill) );
	memset(miniMapScrollKeys ,       0 ,      sizeof(miniMapScrollKeys) );
	memset(miniMapScrollOffset ,       0 ,      sizeof(miniMapScrollOffset) );
	memset(WarningMsg ,        '\0' ,      sizeof(WarningMsg) );
	nMsgCount = 0 ;
	memset( aMinimapConfigs ,			0	,		sizeof(aMinimapConfigs) );
	//INIT_VALUE
//--- m_pub.h ---
	memset( szCFGVersion ,    '\0' ,      sizeof(szCFGVersion) );
//--- m_AutoMapCell.h ---
	memset( wszStaffTombLvlDesc ,      0 ,      sizeof(wszStaffTombLvlDesc) );
//--- m_AutoMapBlob.h ---
	memset( aszUnitBlobFiles ,    '\0' ,      sizeof(aszUnitBlobFiles) );
	memset(anMonsterCorpseColors, -1,       sizeof(anMonsterCorpseColors));
	memset(anMissileColours,    -1,       sizeof(anMissileColours));
	memset( awszAuraDescs ,            0 ,      sizeof(awszAuraDescs) );
	memset( awszEnchantDescs ,         0 ,      sizeof(awszEnchantDescs) );
	memset( awszImmuneDescs ,          0 ,      sizeof(awszImmuneDescs) );
	ResetMonsterColours();
//--- m_Environment.h ---
  memset(afMonsterCorpses,    1,        sizeof(afMonsterCorpses));
  memset(afMissileCorpses,    1,        sizeof(afMissileCorpses));
//--- m_MonsterDeath.h ---
	memset(afMonsterDeathAnims,   1 ,     sizeof(afMonsterDeathAnims));
//--- m_ScrollLock.h ---
	InitScrollLock(TRUE);
//--- m_GameMonitor.h ---
	memset( sMonitorStr ,  0 , sizeof(sMonitorStr));
	nMonitors = 0;
//--- m_ItemDefine.h ---
  memset(anGoldGoodCol,     -1,       sizeof(anGoldGoodCol));
  memset(anGoldPoorCol,     -1,       sizeof(anGoldPoorCol));
  memset(anItemColours,     -1,       sizeof(anItemColours));
  memset(anRuneColours,     -1,       sizeof(anRuneColours));
  memset(anItemHideLevel,   -1,       sizeof(anItemHideLevel));
  memset(anItemShowCLevel,   -1,       sizeof(anItemShowCLevel));
//--- m_ItemName.h ---
  memset( wszEtherealItemPrefix ,    0 ,      sizeof(wszEtherealItemPrefix) );
  memset( wszEtherealItemPostfix ,   0 ,      sizeof(wszEtherealItemPostfix) );
//--- m_CheckDangerous.h ---
	memset(anDangerousMonster,    0,        sizeof(anDangerousMonster));
//--- m_AutoEnchant.h ---
	memset(dwAutoEnchantMonsterGroup,    0,        sizeof(dwAutoEnchantMonsterGroup));
//--- m_DropProtection.h ---
  memset(aDropProtectRune ,    0 ,      sizeof(aDropProtectRune) );
  memset(aDropProtectRuneword ,    0 ,      sizeof(aDropProtectRuneword) );
  memset(aDropProtectUnique ,    0 ,      sizeof(aDropProtectUnique) );
  memset(aDropProtectSet ,    0 ,      sizeof(aDropProtectSet) );
//--- m_PacketHandler.h ---
  memset(fMonitorSendCmd ,    1 ,      sizeof(fMonitorSendCmd) );
  memset(fMonitorRecvCmd ,    1 ,      sizeof(fMonitorRecvCmd) );
	dwGameMonitorX=-10;
	dwGameMonitorY=-110;
	dwCountDownFontSize=3;
	dwCountDownGap=25;
	dwCountDownFlashSecond=30;
}

void FixValues(){
	//FIX_VALUE
//--- m_pub.h ---
	if( szCFGVersion[0][0] ){
		wsprintfW2(wszCFGVersion[0], "<Hackmap>: %s " , szCFGVersion[0]);
	}
	if( szCFGVersion[1][0] ){
		wchar_t temp[512];
		if( fLocalizationSupport>1 ) GB2GBK(szCFGVersion[1]);
		MyMultiByteToWideChar(CP_ACP, 0, szCFGVersion[1], -1,temp , 0x200);
		wsprintfW(wszCFGVersion[1], L"<Hackmap>: %s " , temp);
	}else if( szCFGVersion[0][0] ){
		wsprintfW2(wszCFGVersion[1], "<Hackmap>: %s " , szCFGVersion[0]);
	}
	SetCenterAlertMsg(FALSE , L"");
//--- m_AutoMapCell.h ---
	if (nCaveNameTextCol>12)nCaveNameTextCol=0;
//--- m_MinimapPoint.h ---
	if ( nMinimapSize>10 ) nMinimapSize = 10 ;
//--- m_AutoMapBlob.h ---
	if ( !aszUnitBlobFiles[6][0] ){
		strcpy(aszUnitBlobFiles[6],aszUnitBlobFiles[1]);
	}
	if ( !aszUnitBlobFiles[7][0] ){
		strcpy(aszUnitBlobFiles[7],aszUnitBlobFiles[1]);
	}
	if ( !aszUnitBlobFiles[8][0] ){
		strcpy(aszUnitBlobFiles[8],aszUnitBlobFiles[0]);
	}
	if ( !aszUnitBlobFiles[9][0] ){
		strcpy(aszUnitBlobFiles[9],aszUnitBlobFiles[1]);
	}
	if ( !aszUnitBlobFiles[10][0] ){
		strcpy(aszUnitBlobFiles[10],aszUnitBlobFiles[1]);
	}
	if ( nMonsterTextCol>12 ) nMonsterTextCol= 0 ;

	if ( nSuperUniqueDescFont>12 ) nSuperUniqueDescFont = 0;
//--- m_Environment.h ---
  if ( tWeather.isLoad==FALSE ){
	  tWeather.isOn = tFullVisuals.isOn;
	  tWeather.key = tFullVisuals.key;
  }
  if ( tLightRadiu.isLoad==FALSE ){
	  tLightRadiu.isOn = tFullVisuals.isOn;
	  tLightRadiu.key = tFullVisuals.key;
  }
  if ( tInfravision.isLoad==FALSE ){
	  tInfravision.isOn = tFullVisuals.isOn;
	  tInfravision.key = tFullVisuals.key;
  }
  if ( tScreenshake.isLoad==FALSE ){
	  tScreenshake.isOn = tFullVisuals.isOn;
	  tScreenshake.key = tFullVisuals.key;
  }
//--- m_ScrollLock.h ---
	InitScrollLock(FALSE);
	#ifdef MODULE_ROOMAREA
	if ( tAutomapScreenArea.key == tScrollMap.key ){
		tAutomapScreenArea.isOn = tScrollMap.isOn ;
	}
	#endif
//--- m_ScreenHook.h ---
	InitScreenHook();
//--- m_ItemDefine.h ---
	if (nItemShowMode>3) nItemShowMode = 3;
//--- m_QuestProtect.h ---
	InitBugInfo();
//--- m_Socket.h ---
	if ( dwSocketTimeOutSec>100) dwSocketTimeOutSec = 100;
	if ( dwSocketTimeOutSec<5) dwSocketTimeOutSec = 5;
	dwSocketTimeOut = dwSocketTimeOutSec*1000;
//--- m_GameWindow.h ---
	fFullWindows = IsFullWindow() ;
	//强制添加塔拉夏古墓的两个指向
	AddMinimapConfig(46 ,300 ,1 ,(BYTE)-1);
	for (DWORD i=66 ; i<=72 ; i++ ){
		AddMinimapConfig(i ,300 ,1 ,(BYTE)-1);
	}
	if (dwManaPotionColumn<0||dwManaPotionColumn>4) dwManaPotionColumn=0;
	if (dwHirePotionColumn<0||dwHirePotionColumn>4) dwHirePotionColumn=0;
//--- m_MultiClient.h ---
	tMultiClientClick.type=TOGGLEVAR_DOWNUPPARAM;
	tMultiClientClick.funcUp=MultiClientStopClick;
}

static const char *gamecontrol_names[64]={
"CharacterScreen"/*0*/, "InventoryScreen"/*1*/, "PartyScreen"/*2*/, "MessageLog"/*3*/, "QuestLog"/*4*/, "Chat"/*5*/,
"HelpScreen"/*6*/, "Automap"/*7*/, "CenterAutomap"/*8*/, "FadeAutomap"/*9*/, "PartyonAutomap"/*10*/,
"NamesonAutomap"/*11*/, "SkillTree"/*12*/, "SkillSpeedBar"/*13*/, "Skill1"/*14*/, "Skill2"/*15*/,
"Skill3"/*16*/, "Skill4"/*17*/, "Skill5"/*18*/, "Skill6"/*19*/, "Skill7"/*20*/,
"Skill8"/*21*/, "ShowBelt"/*22*/, "UseBelt1"/*23*/, "UseBelt2"/*24*/, "UseBelt3"/*25*/,
"UseBelt4"/*26*/, "SayHelp"/*27*/, "SayFollowme"/*28*/, "SayThisisforyou"/*29*/, "SayThanks"/*30*/,
"SaySorry"/*31*/, "SayBye"/*32*/, "SayNowyoudie"/*33*/, "Run"/*34*/, "ToggleRun/Walk"/*35*/,
"StandStill"/*36*/, "ShowItems"/*37*/, "ClearScreen"/*38*/, "SelectPreviousSkill"/*39*/, "SelectNextSkill"/*40*/,
"ClearNessage"/*41*/,0, "ShowPortraits"/*43*/, "SwapWeapons"/*44*/, "ToggleMiniMap"/*45*/,
"Skill9"/*46*/, "Skill10"/*47*/, "Skill11"/*48*/, "Skill12"/*49*/, "Skill13"/*50*/,
"Skill14"/*51*/, "Skill15"/*52*/, "Skill16"/*53*/, "HiringScreen"/*54*/, "SayRetreat"/*55*/,
"ESC"/*56*/,0,0,0, "HireUseBelt1"/*60*/,
"HireUseBelt2"/*61*/, "HireUseBelt3"/*62*/, "HireUseBelt4"/*63*/,
};
static const char *vk_names[256]={
0,"LBUTTON"/*01*/, "RBUTTON"/*02*/, "CANCEL"/*03*/, "MBUTTON"/*04*/, "XBUTTON1"/*05*/, "XBUTTON2"/*06*/,0,
"BACK"/*08*/, "TAB"/*09*/,0,0, "CLEAR"/*0C*/, "RETURN"/*0D*/,0,0,
"SHIFT"/*10*/, "CONTROL"/*11*/, "MENU"/*12*/, "PAUSE"/*13*/, "CAPITAL"/*14*/, "KANA"/*15*/,0, "JUNJA"/*17*/,
"FINAL"/*18*/, "HANJA"/*19*/,0, "ESCAPE"/*1B*/, "CONVERT"/*1C*/, "NONCONVERT"/*1D*/, "ACCEPT"/*1E*/, "MODECHANGE"/*1F*/,
"SPACE"/*20*/, "PRIOR"/*21*/, "NEXT"/*22*/, "END"/*23*/, "HOME"/*24*/, "LEFT"/*25*/, "UP"/*26*/, "RIGHT"/*27*/,
"DOWN"/*28*/, "SELECT"/*29*/, "PRINT"/*2A*/, "EXECUTE"/*2B*/, "SNAPSHOT"/*2C*/, "INSERT"/*2D*/, "DELETE"/*2E*/, "HELP"/*2F*/,
"0"/*30*/, "1"/*31*/, "2"/*32*/, "3"/*33*/, "4"/*34*/, "5"/*35*/, "6"/*36*/, "7"/*37*/,
"8"/*38*/, "9"/*39*/, 0,0,0,0,0,0,
0,"A"/*41*/, "B"/*42*/, "C"/*43*/, "D"/*44*/, "E"/*45*/, "F"/*46*/, "G"/*47*/,
"H"/*48*/, "I"/*49*/, "J"/*4A*/, "K"/*4B*/, "L"/*4C*/, "M"/*4D*/, "N"/*4E*/, "O"/*4F*/,
"P"/*50*/, "Q"/*51*/, "R"/*52*/, "S"/*53*/, "T"/*54*/, "U"/*55*/, "V"/*56*/, "W"/*57*/,
"X"/*58*/, "Y"/*59*/, "Z"/*5A*/, "LWIN"/*5B*/, "RWIN"/*5C*/, "APPS"/*5D*/,0, "SLEEP"/*5F*/,
"NUMPAD0"/*60*/, "NUMPAD1"/*61*/, "NUMPAD2"/*62*/, "NUMPAD3"/*63*/, "NUMPAD4"/*64*/, "NUMPAD5"/*65*/, "NUMPAD6"/*66*/, "NUMPAD7"/*67*/,
"NUMPAD8"/*68*/, "NUMPAD9"/*69*/, "MULTIPLY"/*6A*/, "ADD"/*6B*/, "SEPARATOR"/*6C*/, "SUBTRACT"/*6D*/, "DECIMAL"/*6E*/, "DIVIDE"/*6F*/,
"F1"/*70*/, "F2"/*71*/, "F3"/*72*/, "F4"/*73*/, "F5"/*74*/, "F6"/*75*/, "F7"/*76*/, "F8"/*77*/,
"F9"/*78*/, "F10"/*79*/, "F11"/*7A*/, "F12"/*7B*/, "F13"/*7C*/, "F14"/*7D*/, "F15"/*7E*/, "F16"/*7F*/,
"F17"/*80*/, "F18"/*81*/, "F19"/*82*/, "F20"/*83*/, "F21"/*84*/, "F22"/*85*/, "WHEELUP"/*86*/, "WHEELDOWN"/*87*/,
0,0,0,0,0,0,0,0,
"NUMLOCK"/*90*/, "SCROLL"/*91*/, "NUM="/*92*/,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"BROWSER_BACK"/*A6*/,"BROWSER_FORWARD"/*A7*/,"BROWSER_REFRESH"/*A8*/,
"BROWSER_STOP"/*A9*/, "BROWSER_SEARCH"/*AA*/, "BROWSER_FAVORITES"/*AB*/, "BROWSER_HOME"/*AC*/, "VOLUME_MUTE"/*AD*/, "VOLUME_DOWN"/*AE*/, "VOLUME_UP"/*AF*/,
"MEDIA_NEXT_TRACK"/*B0*/, "MEDIA_PREV_TRACK"/*B1*/, "MEDIA_STOP"/*B2*/, "MEDIA_PLAY_PAUSE"/*B3*/, "LAUNCH_MAIL"/*B4*/, "LAUNCH_MEDIA_SELECT"/*B5*/, "LAUNCH_APP1"/*B6*/, "LAUNCH_APP2"/*B7*/,
0,0,";:"/*BA*/, "+"/*BB*/, ","/*BC*/, "-"/*BD*/, "."/*BE*/, "/?"/*BF*/,
"`~"/*C0*/,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0, "[{"/*DB*/, "\\|"/*DC*/, "]}"/*DD*/, "'\""/*DE*/, "OEM_8"/*DF*/,
0,"OEM_AX"/*E1*/, "OEM_102"/*E2*/, "ICO_HELP"/*E3*/, "ICO_00"/*E4*/, "PROCESSKEY"/*E5*/, "ICO_CLEAR"/*E6*/, "PACKET"/*E7*/,
0, "OEM_RESET"/*E9*/, "OEM_JUMP"/*EA*/, "OEM_PA1"/*EB*/, "OEM_PA2"/*EC*/, "OEM_PA3"/*ED*/, "OEM_WSCTRL"/*EE*/, "OEM_CUSEL"/*EF*/,
"OEM_ATTN"/*F0*/, "OEM_FINISH"/*F1*/, "OEM_COPY"/*F2*/, "OEM_AUTO"/*F3*/, "OEM_ENLW"/*F4*/, "OEM_BACKTAB"/*F5*/, "ATTN"/*F6*/, "CRSEL"/*F7*/,
"EXSEL"/*F8*/, "EREOF"/*F9*/, "PLAY"/*FA*/, "ZOOM"/*FB*/, "NONAME"/*FC*/, "PA1"/*FD*/, "OEM_CLEAR"/*FE*/,0,
};
void formatKey(char *buf,int vk) {
	if (vk&0x400) buf+=sprintf(buf,"SHIFT+");
	if (vk&0x200) buf+=sprintf(buf,"CTRL+");
	if (vk&0x100) buf+=sprintf(buf,"ALT+");
	buf+=sprintf(buf,"%s",vk_names[vk&0xFF]);
}
static void setupKey(struct ToggleVar *p) {
	if (p->key==0xff) return;
	if(1<=p->key&&p->key<8*256){
		p->next=tKeyEventMap[p->key];
		tKeyEventMap[p->key] = p;
		if(p->value32==0 || p->isOn>1 ) p->value32 = p->isOn?p->isOn:1;
		if( p->isOn>1 ) p->isOn = 1;
	}
}
static void initKeyArray(ConfigVar *config ,int index ,int dimId){
	if (config->anArrayMax[dimId]>0) {
		for (int j = 0; j < config->anArrayMax[dimId]; j++)
			initKeyArray(config, index*(config->anArrayMax[dimId])+j, dimId+1);
	} else {
		struct ToggleVar *p=(struct ToggleVar *)config->pVar;
		p+=index*config->size32;
		for (int i=0;i<config->size32;i++) setupKey(p++);
	}
}
void InitKeyEvent(){
	static ToggleVar tvs[256];
	char buf[256];
	int n=0;
	for ( int i = 0; i < _ARRAYSIZE(aConfigVars); i++ ) {
		struct ConfigVar *pc=&aConfigVars[i];
		if (pc->type==CONFIG_VAR_TYPE_KEY) {
			ToggleVar *p = (ToggleVar *)pc->pVar;
			setupKey(p);
		} else if (pc->type==CONFIG_VAR_TYPE_KEYARRAY) {
			initKeyArray(pc,0,0);
		}
	}
	for (int i=0;i<_ARRAYSIZE(dwGameControlKeys);i++) {
		for (int j=0;j<_ARRAYSIZE(dwGameControlKeys[0]);j++) {
			int k=dwGameControlKeys[i][j];
			if (1<=k&&k<8*256) {
				ToggleVar *p=&tvs[n++];
				p->next=tKeyEventMap[k];
				tKeyEventMap[k] = p;
				p->type=TOGGLEVAR_DOWNUPPARAM;p->key=k;
				p->param=i;p->keyType=0;
				p->funcParam=GameControlStart;
				p->funcUp=GameControlEnd;
				sprintf(buf,"GameControl[%d:%s]",i+1,gamecontrol_names[i]? gamecontrol_names[i]:"");
				p->desc=heap_strdup(confHeap,buf);
				if (n>=256) {LOG("ERROR: too many keys\n");return;}
			}
		}
	}
	for (int i=0;i<_ARRAYSIZE(dwGameSoundKeys);i++) {
		int k=dwGameSoundKeys[i];
		if (1<=k&&k<8*256) {
			ToggleVar *p=&tvs[n++];
			p->next=tKeyEventMap[k];
			tKeyEventMap[k] = p;
			p->type=TOGGLEVAR_DOWNPARAM;p->key=k;
			p->param=i;p->keyType=0;
			p->funcParam=GameSound;
			sprintf(buf,"GameSound[%d]",i+1);
			p->desc=heap_strdup(confHeap,buf);
			if (n>=256) {LOG("ERROR: too many keys\n");return;}
		}
	}
	for (int i=0;i<_ARRAYSIZE(dwSwitchSkillKeys);i++) {
		int k=dwSwitchSkillKeys[i];
		if (1<=k&&k<8*256) {
			ToggleVar *p=&tvs[n++];
			p->next=tKeyEventMap[k];
			tKeyEventMap[k] = p;
			p->type=TOGGLEVAR_DOWNUPPARAM;p->key=k;
			p->param=i;p->keyType=0;
			p->funcParam=switchSkillStart;
			sprintf(buf,"SwitchSkill[%d]",i+1);
			p->desc=heap_strdup(confHeap,buf);
			p->funcUp=switchSkillEnd;
			if (n>=256) {LOG("ERROR: too many keys\n");return;}
		}
	}
	for (int i=0;i<_ARRAYSIZE(tMapScrollKey);i++) {
			ToggleVar *p = (ToggleVar *)&tMapScrollKey[i];
			p->type=TOGGLEVAR_DOWNUPPARAM;p->funcUp=ScrollMapEnd;
	}
	for (int i=0;i<_ARRAYSIZE(miniMapScrollKeys);i++) {
		for (int j=0;j<_ARRAYSIZE(miniMapScrollKeys[0]);j++) {
			int k=miniMapScrollKeys[i][j];
			if (1<=k&&k<8*256) {
				ToggleVar *p=&tvs[n++];
				p->next=tKeyEventMap[k];
				tKeyEventMap[k] = p;
				p->type=TOGGLEVAR_DOWNPARAM;p->key=k;
				p->param=i;p->keyType=0;
				p->funcParam=MiniMapScroll;
				sprintf(buf,"MiniMapScroll[%d]",i+1);
				p->desc=heap_strdup(confHeap,buf);
				if (n>=256) {LOG("ERROR: too many keys\n");return;}
			}
		}
	}
	if (!logfp) return;
	FILE *fp=openFile("keymap.log","w+");
	if (!fp) return;
	fprintf(fp,"Auto Generated Key Map report. DO NOT MODIFY\n");
	fprintf(fp,"自动生成的键盘布局报告，不要修改此文件\n");
	for (int vk=0;vk<256;vk++) {
		for (int mod=0;mod<8;mod++) {
			int k=(mod<<8)|vk;
			ToggleVar *p=tKeyEventMap[k];
			if (!p) continue;
			if (p->next) fprintf(fp,"Multiple ");
			fprintf(fp,"Key");
			fprintf(fp," 0x%02x ",k);
			if (vk_names[k&0xFF]) {
				if (k&0x400) fputs("SHIFT+",fp);
				if (k&0x200) fputs("CTRL+",fp);
				if (k&0x100) fputs("ALT+",fp);
				fprintf(fp,"%s",vk_names[k&0xFF]);
			} else {
			}
			fprintf(fp,":");
			while (p) {
				if (p->desc) fprintf(fp," \"%s\"",p->desc);
				else fprintf(fp," (%d)",p->type);
				if (p->keyType==1) fprintf(fp,"(OutGame)");
				else if (p->keyType==2) fprintf(fp,"(Always)");
				p=p->next;
			}
			fprintf(fp,"\n");
			fflush(fp);
		}
	}
	fclose(fp);
}

void AddMinimapConfig( DWORD dwLevelNo , WORD wPointNo ,BYTE fFlag , BYTE nColour ){
	MinimapConfig *pConfig = &aMinimapConfigs[dwLevelNo];
	if ( pConfig->nPoints>=30 ) return;
	int idx = pConfig->nPoints ;
	for ( int i=0;i<pConfig->nPoints ;i++){
		if ( wPointNo==pConfig->aPoint[i].wPointNo ) {
			idx = i ;
			break;
		}
	}
	pConfig->aPoint[idx].fFlag = fFlag ;
	pConfig->aPoint[idx].nColour = nColour ;
	pConfig->aPoint[idx].wPointNo = wPointNo ;
	if ( idx==pConfig->nPoints )pConfig->nPoints++;
}

void AddWarningMessage(char *msg , int flag=0){
	for( int i=0 ; i<nMsgCount ; i++){
		if( _stricmp( WarningMsg[i] , msg ) ==0 ){
			return;
		}
	}
	if (nMsgCount>=199) return ; 
	if (flag==0) sprintf(WarningMsg[nMsgCount], " Command -- %s -- unused , Check it ", msg);
	else strncpy( WarningMsg[nMsgCount] , msg , 99);
	LOG("Warning: %s\n",msg);
	nMsgCount++;
}
void ShowWarningMessage(){
	if( fShowCheckInfo && nMsgCount > 0 ){
		for( int i=0 ; i<nMsgCount ; i++){
			ShowGameMessage(WarningMsg[i]);
		}
		memset( WarningMsg,			'\0',		sizeof(WarningMsg) );
		nMsgCount = 0 ;
	}
}


char isquote(char c) {return ((c) == '"' || (c) == '`') ? c : 0;}
void filterSpace(char *s) {
	char *src=s,*dst=s;int quote = 0;
	while(1){
		char c=*src++;if (!c) break;
		if(isspace(c)&&!quote) continue;
		if(c=='/'&&*src=='/') break;
		if(isquote(c)) quote = !quote;
		*dst++=c;
	}
	*dst=0;
}
void splitLine(char *line,char **pleft,char **pright){
	filterSpace(line);//去掉无效字符
	char *e = strchr(line,':');
	if (e) {*e=0;*pleft=line;*pright=e+1;return;}
	//没有:号分割，用最后个]号来分隔
	if (line[0]) {
		LOG("Warning config line: %s\n",line);
		e = strrchr(line,']');
		if (e) {*e=0;*pleft=line;*pright=e+1;return;}
	}
	*pleft=NULL;*pright=NULL;
}
struct Symbol *findSymbol(char *name) {
	if (!fSymbolSorted) {
		//LOG("sort symbols %d\n",nSymbols);
		qsort(pSymbols,nSymbols,sizeof(struct Symbol),compareSymbol);
		fSymbolSorted=1;
	}
	struct Symbol c;c.name=name;
	struct Symbol *p=(Symbol *)bsearch(&c,pSymbols,nSymbols,sizeof(struct Symbol),compareSymbol);
	if (p) return p;
	for (int i=0;i<nTmpSymbols;i++) {
		p=&tmpSymbols[i];
		if (_stricmp(p->name,name)==0) return p;
	}
	return NULL;
}
void doneNameValues() {
	if (nTmpSymbols<=0) return;
	if (nSymbols+nTmpSymbols>capSymbols) {
		capSymbols*=2;
		pSymbols=(struct Symbol *)realloc(pSymbols,capSymbols*sizeof(struct Symbol));
	}
	memcpy(pSymbols+nSymbols,tmpSymbols,nTmpSymbols*sizeof(struct Symbol));
	nSymbols+=nTmpSymbols;nTmpSymbols=0;
	fSymbolSorted=0;
}
void addNameValue(char *name,char *value) {
	if (nTmpSymbols>=TMP_SYMBOLS) {LOG("FATAL ERROR\n");return;}
	struct Symbol *p=&tmpSymbols[nTmpSymbols++];
	p->name=name;p->value=value;
	if (nTmpSymbols>=TMP_SYMBOLS) doneNameValues();
}
char *replaceSymbols(char *value){
	char buf[1024],word[1024];
	char *s = value;
	char *pt = buf;
	char *pw = word;
	while(*s&&!isalnum(*s)) *pt++ = *s++;
	while(1){
		char c=*s;
		if (c&&( isalnum(c) || isspace(c) || c=='_' || c=='\'' || isquote(c) )) {
			*pw++ = c;
		} else {
			*pw = 0;
			pw = word;
			if(*pw){
				struct Symbol *p=findSymbol(word);
				char *ptr = p?p->value:word;
				int len=strlen(ptr);
				memcpy(pt,ptr,len);
				pt+=len;
			}
			*pt++ = c;
		}
		if(!c) break;
		s++;
	}
	int len=strlen(value);
	int len2=strlen(buf);
	if (len2<=len) {strcpy(value,buf);return value;}
	else return heap_strdup(confHeap,buf);
}
void ConvertToWString( wchar_t *dst , char  *src , int size) {
	int n=MultiByteToWideChar(CP_ACP, 0, src, -1, dst, size);
}
void ConvertToD2String( wchar_t *dst , char  *src , int size) {
	wchar_t *dst0 = dst;
	wchar_t *end=dst+size;
	char col = 0;
	int hascolor = 0;
	while(1){
		char ch = *src++;if (!ch) break;
		if(ch!='%') {*dst++ = ch;continue;}
		char next = *src;if(!next) break;
		hascolor = 1;
		if ( isalpha(next) ){
			col = toupper(next)-'A'+10;
			src++;
		}else{
			col = (char)strtol(src,&src,10);
		}
		*dst++ = 0xff;
		*dst++ = 'c';
		*dst++ = '0'+col;
  }
  if(hascolor) *dst++ = 0xff;
  *dst = 0 ;
  if(dst>=end) *end=0;
}
//int dbg_array=0;
BOOL StoreArrayConfig(ConfigVar *config ,int index ,int arrays, char *arrayStr , char *val,  int valsize,int iskey){
	int n=config->anArrayMax[arrays];
	if (n>0) {
		if(arrayStr[0]) {
			if (arrayStr[0]!='[') {LOG("%s:%d not array start %s\n",cfgFileName,lineId,arrayStr);return 0;}
			char *arrEnd=strchr(arrayStr,']');
			if (!arrEnd) {LOG("%s:%d Can't find array end %s\n",cfgFileName,lineId,arrayStr);return 0;}
			*arrEnd=0;
			arrayStr++;
			while(arrayStr[0]) {
				char *endptr=NULL;
				int num1=strtol(arrayStr,&endptr,0);
				if (endptr==arrayStr) {LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,arrayStr);return 0;}
				arrayStr=endptr;
				int num2 = num1;
				if ( (*arrayStr == '-') || (*arrayStr == '+') || (*arrayStr == '.') ) {
					arrayStr++;
					while (*arrayStr == '.') arrayStr++;
					num2=strtol(arrayStr,&endptr,0);
					if (endptr==arrayStr) num2 = config->anArrayMax[arrays];
					else arrayStr=endptr;
				}
				if ( num1 > num2 ) SwapInt(num1, num2);
				if ( num1 < config->base ) num1 = config->base;
				if ( num2>config->anArrayMax[arrays] ) num2 = config->anArrayMax[arrays];
				if ( num1 > num2 ) num1=num2;
				for (int j = num1; j <= num2; j++) {
					if (!StoreArrayConfig(config, index*(config->anArrayMax[arrays])+j-config->base, arrays+1, arrEnd+1, val,valsize,iskey)){
						return FALSE;
					};
				}
				if(arrayStr[0]) arrayStr++;
			}
			*arrEnd=']';
		} else {
			int idx0=index*n;
			if (!StoreArrayConfig(config, idx0, arrays+1, arrayStr , val,valsize,iskey)) return FALSE;
			int size=valsize;
			if (iskey) size*=sizeof(struct ToggleVar);
			for (int k=arrays+1;config->anArrayMax[k];k++) size*=config->anArrayMax[k];
			char *src=(char *)config->pVar+idx0*size;
			char *dst=src+size;
			for (int j = 1; j < n; j++) {
				//LOG("%d: copy %d <- %d size %d\n",lineId,dst-(char *)config->pVar,src-(char *)config->pVar,size);
				//executed 200K times each time config reload
				memcpy(dst, src, size);dst+=size;
			}
		}
	} else {
		if (arrayStr[0]) LOG("%s:%d Extra %s\n",cfgFileName,lineId,arrayStr);
		if (iskey) {
			struct ToggleVar *ptv=(struct ToggleVar *)config->pVar;
			int *pi=(int *)val;
			ptv+=index*valsize;
			for (int i=0;i<valsize;i++) {
				ptv->type=TOGGLEVAR_DOWNPARAM;
				ptv->key=*pi++;
				ptv->keyType=config->keyType;
				ptv->funcParam=config->keyFunc;
				ptv->param=index;
				if (!config->keyDesc) ptv->desc="(undefined)";
				else {
					char buf[256];
					sprintf(buf,config->keyDesc,index+1);
					ptv->desc=heap_strdup(confHeap,buf);
				}
				ptv++;
			}
		} else {
			/*if (dbg_array) {
				LOG("array: 0x%x+%d*%d <- %x %d\n",config->pVar,index,valsize,val,valsize);
			}*/
			memcpy((char *)config->pVar+index*valsize, val, valsize);
		}
	}
	return TRUE;
}
static void parseValues8(char *buf,char *values,int n) {
	int prev=0 ;
	for ( int i = 0 ; i < n ; i++ ){
		char *end;int t = strtol(values, &end, 0);
		if (values!=end) {values=end;buf[i]=(char)t;prev=t;}
		else buf[i]=(char)prev;
		if(*values) values++;
	}
	if (*values) LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,values);
}
static void parseValues32(int *buf,char *values,int n) {
	int def=0 ;
	for ( int i = 0 ; i < n ; i++ ){
		char *end;int t = strtol(values, &end, 0);
		if (values!=end) {values=end;buf[i]=t;}
		else buf[i]=def;
		if(*values) values++;
	}
	if (*values) LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,values);
}
void StoreConfig(ConfigVar *config , char *arrays , char *right) {
	if (!fLoadItemColours&&_memicmp(config->szCmdName,"ItemColours",11)==0) return; //for fast debugging
	if (!*right&&config->type!=CONFIG_VAR_TYPE_STR
		&&config->type!=CONFIG_VAR_TYPE_D2STR
		&&config->type!=CONFIG_VAR_TYPE_WSTR
		&&config->type!=CONFIG_VAR_TYPE_STRARRAY) {
		AddWarningMessage(config->szCmdName);
		return;
	}
	switch(config->type){
		case CONFIG_VAR_TYPE_ARRAY: {
			char buf[32];
			if (config->size32>=32) {LOG("FATAL: config size %d\n",config->size32);return;}
			parseValues8(buf,right,config->size32);
			//dbg_array=_stricmp(config->szCmdName,"DropProtectRuneword")==0;
			if (!StoreArrayConfig(config , 0 , 0 , arrays , buf,config->size32,0))
				AddWarningMessage(config->szCmdName);
			break;
		}
		case CONFIG_VAR_TYPE_INTARRAY: {
			int ibuf[32];
			if (config->size32>=32) {LOG("FATAL: config size %d\n",config->size32);return;}
			parseValues32(ibuf,right,config->size32);
			if (!StoreArrayConfig(config , 0 , 0 , arrays , (char *)ibuf,config->size32<<2,0))
				AddWarningMessage(config->szCmdName);
			break;
		}
		case CONFIG_VAR_TYPE_KEYARRAY: {
			int ibuf[32];
			if (config->size32>=32) {LOG("FATAL: config size %d\n",config->size32);return;}
			parseValues32(ibuf,right,config->size32);
			if (!StoreArrayConfig(config , 0 , 0 , arrays , (char *)ibuf,config->size32,1))
				AddWarningMessage(config->szCmdName);
			break;
		}
		case CONFIG_VAR_TYPE_STRARRAY: {
			if (!StoreArrayConfig(config , 0 , 0 , arrays , (char *)right,config->size32,0))
				AddWarningMessage(config->szCmdName);
			break;
		}
		#ifdef MODULE_MINIMAPPOINT
		case CONFIG_VAR_TYPE_MINIMAPPOINT:
			{
				DWORD dwLevelNo = 0 ;
				WORD wPointNo =0 ;
				BYTE fFlag =0 ;
				BYTE nColour=0;
				char *p=arrays;
				while (*p &&  *p != '[') p++;
				p++;
				if (*p) dwLevelNo = strtoul(p,&p,0);
				while (*p &&  *p != '[') p++;
				p++;
				if (*p) wPointNo = (WORD)strtoul(p,&p,0);
				char *endptr;
				fFlag = (BYTE)strtoul(right,&endptr,0);
				while(*endptr && !isdigit(*endptr)) endptr++;
				if(*endptr){
					nColour = (BYTE)strtoul(endptr,&endptr,0); 
				}else{
					nColour = (BYTE)-1;
				}
				if ( dwLevelNo && wPointNo){
					AddMinimapConfig( dwLevelNo,wPointNo,fFlag,nColour);
				}
			}
			break;
		#endif
		case CONFIG_VAR_TYPE_KEY: {
				char *endptr;
				ToggleVar *tgKey = (ToggleVar *)config->pVar;
				tgKey->isLoad = TRUE;
				if( tgKey->type == 1){ //开关类型
					tgKey->isOn = (BYTE)strtoul(right,&endptr,0);//第一项 0 关 非0 开 
					if(*endptr) endptr++;
					if(*endptr) tgKey->key = strtoul(endptr,&endptr,0); //快捷键
					else tgKey->key = -1;
					if(*endptr) endptr++;
					if(*endptr) {
						tgKey->value32 = strtoul(endptr,&endptr,0); //默认值
					}
				} else {
					//按键触发事件
					tgKey->key = strtoul(right,&endptr,0);	//快捷键
					if(*endptr) endptr++;
					if(*endptr) tgKey->param = strtoul(endptr,&endptr,0); //参数
				}
				if (endptr&&*endptr) LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,endptr);
			}
			break;
		case CONFIG_VAR_TYPE_INT: {
			char *endptr;
			//数值
			if (right[0]=='-') {
				if( config->size32 == 1 ){
					*((char *)config->pVar) = (char)strtol(right,&endptr,0);
				}else if( config->size32 == 2 ){
					*((short *)config->pVar) = (short)strtol(right,&endptr,0);
				}else if( config->size32 == 4 ){
					*((int *)config->pVar) = (int)strtol(right,&endptr,0);
				}
			} else {
				if( config->size32 == 1 ){
					*((BYTE *)config->pVar) = (BYTE)strtoul(right,&endptr,0);
				}else if( config->size32 == 2 ){
					*((WORD *)config->pVar) = (WORD)strtoul(right,&endptr,0);
				}else if( config->size32 == 4 ){
					*((DWORD *)config->pVar) = (DWORD)strtoul(right,&endptr,0);
				}
			}
			if (endptr&&*endptr) LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,endptr);
			break;
		}
		case CONFIG_VAR_TYPE_STR: {
				//字符串，单字节
				if (config->size32 >1){
					for( int i=0 ; i <config->size32 ; i ++ ){
						strncpy( (char *)config->pVar + config->anArrayMax[0]*i, right , config->anArrayMax[0]-1);
					}
				}else{
					strncpy( (char *)config->pVar , right , config->anArrayMax[0]-1 );
				}
			}
			break;
		case CONFIG_VAR_TYPE_WSTR: {
				//字符串，双字节
				if ( config->size32 >1 ){
					for( int i=0 ; i <config->size32 ; i ++ ){
						ConvertToWString( (wchar_t *)config->pVar + config->anArrayMax[0]*i, right ,config->anArrayMax[0]-1);
					}
				}else{
					ConvertToWString( (wchar_t *)config->pVar , right, config->anArrayMax[0]-1 );
				}
			}
			break;
		case CONFIG_VAR_TYPE_D2STR: {
				//字符串，双字节
				if ( config->size32 >1 ){
					for( int i=0 ; i <config->size32 ; i ++ ){
						ConvertToD2String( (wchar_t *)config->pVar + config->anArrayMax[0]*i, right ,config->anArrayMax[0]-1);
					}
				}else{
					ConvertToD2String( (wchar_t *)config->pVar , right, config->anArrayMax[0]-1 );
				}
			}
			break;

		#ifdef MODULE_GAMEMONITOR
		case CONFIG_VAR_TYPE_GAMEMONITOR:
			{
				DWORD dwStatNo = 0 ;
				DWORD dwColor = 0;
				char *p=arrays;
				while (*p &&  *p != '[') p++;
				p++;
				if (*p) dwStatNo = strtoul(p,&p,0);
				sMonitorStr[nMonitors].dwStatNo = dwStatNo;
				dwColor = strtoul(right,&right,0);
				right++ ;
				sMonitorStr[nMonitors].dwColor = dwColor;
				p = right++;
				while(*p && *p !=',') p++;
				if ( p != right ) *(p-1)='\0';
				ConvertToD2String( sMonitorStr[nMonitors].wszDesc[0] , right, 29 );
				if(*p)p++;// , 号
				if(*p)p++;// " 号
				char *endptr = p;
				while (*p) p++;
				if ( p != endptr ) *(p-1)='\0';
				if ( *endptr ){
					char ptemp[30];
					ConvertToColorString((BYTE*)ptemp,endptr,30);
					if( fLocalizationSupport>1 ) GB2GBK(ptemp);
					MyMultiByteToWideChar(CP_ACP, 0, ptemp, -1, sMonitorStr[nMonitors].wszDesc[1]  , 30);
				}else{
					wcscpy( sMonitorStr[nMonitors].wszDesc[1] , sMonitorStr[nMonitors].wszDesc[0]);
				}
				nMonitors++;
			}
			break;
		#endif
		default:
			break;
	}
}

static int isNumber(char *s) {
	int number=1;
	if (s[0]=='0'&&s[1]=='x') {
		s+=2;
		while (1) {
			char c=*s++;if (!c) break;
			if ('0'<=c&&c<='9'||'a'<=c&&c<='f'||'A'<=c&&c<='F') ;
			else {number=0;break;}
		}
	} else {
		while (1) {
			char c=*s++;if (!c) break;
			if ('0'<=c&&c<='9') ;
			else {number=0;break;}
		}
	}
	return number;
}
static void loadSymbolFile(char *path) {
	char *names,*value;
	FILE *fp=openFile(path,"rb");
	if (!fp) {LOG("load symbol file %s failed\n",path);return;}
	int size=0;char *p=loadFile(confHeap,fp,&size);if (!p) return;
	fclose(fp);
	int b=nSymbols;
	//LOG("loading symbols %s\n",path);
	char *end=p+size;
	while (p<end) {
		char *e=strchr(p,'\n');if (!e) e=end;
		char *line=p;p=e+1;
		*e=0;if (e[-1]=='\r') {e--;*e=0;}
		int len=e-line;
		if (line[0]=='/'&&line[1]=='/') continue;
		splitLine(line,&names,&value);
		if (names&&names[0]&&value&&value[0]) {
			if (!isNumber(value)) value=replaceSymbols(value);
			char *name = strtok(names,",");
			while(name) {
				addNameValue(name,value);
				name = strtok(NULL,",");
			}
		}
	}
	doneNameValues();
	LOG("load %d symbols from %s\n",nSymbols-b,path);
}
static int changeLogPath(char *path) {
	char fmt[512],buf[512];
	int id=1;
	if (path[0]&&path[1]==':') sprintf(fmt,"%s",path);
	else sprintf(fmt,"%s%s",szPluginPath,path);
	for (id=1;id<=10;id++) {
		sprintf(buf,fmt,id);
		if (!fileExist(buf)) break;
		if (DeleteFileA(buf)) break;
	}
	FILE *f=openFile(buf,"w+");
	if (f) {
		if (logfp) {
			fprintf(logfp,"Log Changed to %s\n",buf);
			fclose(logfp);
		}
		logfp=f;
	}
	return id;
}
static void loadConfig(char *path) {
	cfgFileName=path;
	//LOG("loading config %s\n",path);
	FILE *fp=openFile(path,"r");if (!fp) return;
	int nLines=0;
	while (1) {
		char buf[1024];
		char *line=fgets(buf,1024,fp);if (!line) break;
		nLines++;
		lineId=nLines;
		int len=strlen(line);
		if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[len-1]=='\r') {len--;line[len]=0;}
		if (strcmp(line,"exit")==0) {LOG("Abort reading\n");break;}
		__try {
			char *name,*value;
			if (line[0]=='/'&&line[1]=='/') continue;
			splitLine(line,&name,&value);
			if (!name&&!value) continue;
			if (!name||!value||!name[0]||!value[0]) {
				LOG("Warning config line: %s\n",line);
				AddWarningMessage(line);
				continue;
			}
			//LOG("%s %s\n",name,value);
			name=replaceSymbols(name);
			//如果第一个字符是" 则不作替换
			if (value[0]=='"') {
				value++;int len2=strlen(value);if (value[len2-1]=='"') {len2--;value[len2]=0;}
			} else {
				value=replaceSymbols(value);
			}
			int hascmd = 0;
			char *arr=name;
			while(*arr && ( isalnum(*arr) || isspace(*arr) || *arr=='_'  ) ) arr++;
			char arrStart=*arr;*arr='\0';
			if (name[0]){
				if (_stricmp(name,"LoadSymbolsFile")==0&&value[0]) {loadSymbolFile(value);continue;}
				if (_stricmp(name,"LoadConfigFile")==0&&value[0]) {
					loadConfig(value);
					cfgFileName=path;
					continue;
				}
				if (_stricmp(name,"LogPath")==0&&value[0]) {dwGameWindowId=changeLogPath(value);continue;}
				ConfigVar *pcv=findConfigVar(name);
				if (pcv) {
					*arr=arrStart;
					StoreConfig(pcv,arr,value);
					hascmd = 1;
				}
			}
			if (!hascmd){
				LOG("Warning config line: %s\n",line);
				AddWarningMessage(line);
			}
		} __except(EXCEPTION_EXECUTE_HANDLER) {
			LOG("ERROR: config line %s\n", line);
			AddWarningMessage("Load CFG Error!" , 1 );
		}
	}
	LOG("Load %d lines from %s\n",nLines,path);
}
void LoadLogNames(FILE *fp);
BOOL LoadConfig() {
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	QueryPerformanceCounter(&perfStart);
	if (confHeap) HeapDestroy(confHeap);
	confHeap=HeapCreate(0,128*1024,128*1024*1024);
	dwGameWindowId=changeLogPath("runtime\\d2hackmap%d.log");
	InitValues();
	loadConfig("config.txt");
	if (!fLoadItemColours) {
		AddWarningMessage("Load Item Colours disabled");
	}
	__try {
		FixValues();
		InitKeyEvent();
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		AddWarningMessage("Init Key Event Errors!" , 1 );
	}
	//Check
	LOG("Game window id: %d\n",dwGameWindowId);
	for (int i=1;i<_ARRAYSIZE(aConfigVars);i++) {
		if (compareConfigVar(&aConfigVars[i-1],&aConfigVars[i])>=0)
			LOG("duplicate config name %s %s\n",aConfigVars[i-1].szCmdName,aConfigVars[i].szCmdName);
	}
	if (!fSymbolSorted) qsort(pSymbols,nSymbols,sizeof(struct Symbol),compareSymbol);
	for (int i=1;i<nSymbols;i++) {
		if (compareSymbol(&pSymbols[i-1],&pSymbols[i])>=0)
			LOG("duplicate symbol %s %s\n",pSymbols[i-1].name,pSymbols[i].name);
	}
	LOG("loading names\n");
	LoadLogNames(logfp);
	QueryPerformanceCounter(&perfEnd);
	QueryPerformanceFrequency(&perfFreq);
	double timeMs=(perfEnd.QuadPart-perfStart.QuadPart)*1000.0/perfFreq.QuadPart;
	LOG("Load config time %.3lf ms\n",timeMs);
	if (fMonitorPacket) AddWarningMessage("Monitoring Network Packet");
	//LOG("DoTest=0x%x\n",(int)DoTest);
	return TRUE;
}
