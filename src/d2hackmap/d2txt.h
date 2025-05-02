//Generated based on the input of d2common_5EF40 Ordinal10849 
//CompileTxtFile(int unused,char *filename,TxtField *fields,int *ptRecordCount,int recordLength))
//compcode size=0x4 count=d2common_nCompCodes ret=d2common_6175E
struct compcode_txt {
	int code; //+00 type10 link=d2common_pCompCodesLink
};
//itemtypes size=0xE4 count=d2common_nItemTypes ret=d2common_72B29
struct itemtypes_txt {
	int code; //+00 type10 link=d2common_pItemTypesLink
	short equiv1; //+04 type15 link=d2common_pItemTypesLink
	short equiv2; //+06 type15 link=d2common_pItemTypesLink
	char repair; //+08 type4
	char body; //+09 type4
	char bodyloc1; //+0A type13 link=d2common_pBodyLocsLink
	char bodyloc2; //+0B type13 link=d2common_pBodyLocsLink
	short shoots; //+0C type15 link=d2common_pItemTypesLink
	short quiver; //+0E type15 link=d2common_pItemTypesLink
	char throwable; //+10 type4
	char reload; //+11 type4
	char reequip; //+12 type4
	char autostack; //+13 type4
	char magic; //+14 type4
	char rare; //+15 type4
	char normal; //+16 type4
	char charm; //+17 type4
	char gem; //+18 type4
	char beltable; //+19 type4
	char maxsock1; //+1A type4
	char maxsock25; //+1B type4
	char maxsock40; //+1C type4
	char treasureclass; //+1D type4
	char rarity; //+1E type4
	char staffmods; //+1F type13 link=d2common_pPlayerClassLink
	char costformula; //+20 type4
	char class; //+21 type13 link=d2common_pPlayerClassLink
	char storepage; //+22 type13 link=d2common_pStorePageLink
	char varinvgfx; //+23 type4
	char invgfx1[31]; //+24 type1
	char pad_43; //+43
	char invgfx2[31]; //+44 type1
	char pad_63; //+63
	char invgfx3[31]; //+64 type1
	char pad_83; //+83
	char invgfx4[31]; //+84 type1
	char pad_A3; //+A3
	char invgfx5[31]; //+A4 type1
	char pad_C3; //+C3
	char invgfx6[31]; //+C4 type1
	char pad_E3; //+E3
};
//montype size=0xC count=d2common_nMonType ret=d2common_56B3C
struct montype_txt {
	short type; //+00 type17 link=d2common_pMonTypeLink
	short equiv1; //+02 type20 link=d2common_pMonTypeLink
	short equiv2; //+04 type20 link=d2common_pMonTypeLink
	short equiv3; //+06 type20 link=d2common_pMonTypeLink
	short strsing; //+08 type22 link=d2common_5E350
	short strplur; //+0A type22 link=d2common_5E350
};
//pettype size=0xE0 count=d2common_nPetTypes ret=d2common_5F68D
struct pettype_txt {
	int pet_type; //+00 type18 link=d2common_pPetTypesLink
	//int warp:1; //+04 type26 bit=0
	//int range:1; //+04 type26 bit=1
	//int partysend:1; //+04 type26 bit=2
	//int unsummon:1; //+04 type26 bit=3
	//int automap:1; //+04 type26 bit=4
	//int drawhp:1; //+04 type26 bit=5
	char pad_4[4]; //+4
	short group; //+08 type3
	short basemax; //+0A type3
	short name; //+0C type22 link=d2common_5E350
	char icontype; //+0E type4
	char baseicon[32]; //+0F type1
	char micon1[32]; //+2F type1
	char micon2[32]; //+4F type1
	char micon3[32]; //+6F type1
	char micon4[32]; //+8F type1
	char pad_AF[3]; //+AF
	short mclass1; //+B2 type3
	short mclass2; //+B4 type3
	short mclass3; //+B6 type3
	short mclass4; //+B8 type3
	char pad_BA[38]; //+BA
};
//overlay size=0x84 count=d2common_nOverlay ret=d2common_833D9
struct overlay_txt {
	short overlay; //+00 type17 link=d2common_pOverlayTxt
	char Filename[64]; //+02 type1
	short version; //+42 type3
	int Frames; //+44 type2
	char PreDraw; //+48 type4
	char pad_49[3]; //+49
	int 1ofN; //+4C type2
	char Dir; //+50 type4
	char Open; //+51 type4
	char Beta; //+52 type4
	char pad_53; //+53
	int Xoffset; //+54 type2
	int Yoffset; //+58 type2
	int Height1; //+5C type2
	int Height2; //+60 type2
	int Height3; //+64 type2
	int Height4; //+68 type2
	int AnimRate; //+6C type2
	int InitRadius; //+70 type2
	int Radius; //+74 type2
	int LoopWaitTime; //+78 type2
	char Trans; //+7C type4
	char Red; //+7D type4
	char Green; //+7E type4
	char Blue; //+7F type4
	char NumDirections; //+80 type4
	char LocalBlood; //+81 type4
	char pad_82[2]; //+82
};
//itemstatcost size=0x144 count=d2common_nItemStatCost ret=d2common_73C06
struct itemstatcost_txt {
	short stat; //+00 type17 link=d2common_pItemStatCostLink
	char pad_2[2]; //+2
	//int send_other:1; //+04 type26 bit=0
	//int signed:1; //+04 type26 bit=1
	//int damagerelated:1; //+04 type26 bit=2
	//int itemspecific:1; //+04 type26 bit=3
	//int direct:1; //+04 type26 bit=4
	//int updateanimrate:1; //+04 type26 bit=9
	//int fmin:1; //+04 type26 bit=10
	//int fcallback:1; //+04 type26 bit=11
	//int saved:1; //+04 type26 bit=12
	//int csvsigned:1; //+04 type26 bit=13
	char pad_4[4]; //+4
	char send_bits; //+08 type4
	char send_param_bits; //+09 type4
	char csvbits; //+0A type4
	char csvparam; //+0B type4
	int divide; //+0C type2
	int multiply; //+10 type2
	int add; //+14 type2
	char valshift; //+18 type4
	char save_bits; //+19 type4
	char 1.09-save_bits; //+1A type4
	char pad_1B; //+1B
	int save_add; //+1C type2
	int 1.09-save_add; //+20 type2
	int save_param_bits; //+24 type2
	char pad_28[4]; //+28
	int minaccr; //+2C type2
	char encode; //+30 type4
	char pad_31; //+31
	short maxstat; //+32 type20 link=d2common_pItemStatCostLink
	short descpriority; //+34 type3
	char descfunc; //+36 type4
	char descval; //+37 type4
	short descstrpos; //+38 type22 link=d2common_5E350
	short descstrneg; //+3A type22 link=d2common_5E350
	short descstr2; //+3C type22 link=d2common_5E350
	short dgrp; //+3E type3
	char dgrpfunc; //+40 type4
	char dgrpval; //+41 type4
	short dgrpstrpos; //+42 type22 link=d2common_5E350
	short dgrpstrneg; //+44 type22 link=d2common_5E350
	short dgrpstr2; //+46 type22 link=d2common_5E350
	short itemevent1; //+48 type20 link=d2common_pEventsLink
	short itemevent2; //+4A type20 link=d2common_pEventsLink
	short itemeventfunc1; //+4C type3
	short itemeventfunc2; //+4E type3
	char keepzero; //+50 type4
	char pad_51[3]; //+51
	char op; //+54 type4
	char op_param; //+55 type4
	short op_base; //+56 type20 link=d2common_pItemStatCostLink
	short op_stat1; //+58 type20 link=d2common_pItemStatCostLink
	short op_stat2; //+5A type20 link=d2common_pItemStatCostLink
	short op_stat3; //+5C type20 link=d2common_pItemStatCostLink
	char pad_5E[226]; //+5E
	int stuff; //+140 type2
};
//properties size=0x2E count=d2common_nProperties ret=d2common_73314
struct properties_txt {
	short code; //+00 type17 link=d2common_pPropertiesLink
	char set1; //+02 type4
	char set2; //+03 type4
	char set3; //+04 type4
	char set4; //+05 type4
	char set5; //+06 type4
	char set6; //+07 type4
	char set7; //+08 type4
	char pad_9; //+9
	short val1; //+0A type3
	short val2; //+0C type3
	short val3; //+0E type3
	short val4; //+10 type3
	short val5; //+12 type3
	short val6; //+14 type3
	short val7; //+16 type3
	char func1; //+18 type4
	char func2; //+19 type4
	char func3; //+1A type4
	char func4; //+1B type4
	char func5; //+1C type4
	char func6; //+1D type4
	char func7; //+1E type4
	char pad_1F; //+1F
	short stat1; //+20 type20 link=d2common_pItemStatCostLink
	short stat2; //+22 type20 link=d2common_pItemStatCostLink
	short stat3; //+24 type20 link=d2common_pItemStatCostLink
	short stat4; //+26 type20 link=d2common_pItemStatCostLink
	short stat5; //+28 type20 link=d2common_pItemStatCostLink
	short stat6; //+2A type20 link=d2common_pItemStatCostLink
	short stat7; //+2C type20 link=d2common_pItemStatCostLink
};
//missiles size=0x1A4 count=d2common_nMissiles ret=d2common_7E1B8
struct missiles_txt {
	short Missile; //+00 type17 link=d2common_pMissilesLink
	char pad_2[2]; //+2
	//int LastCollide:1; //+04 type26 bit=0
	//int Explosion:1; //+04 type26 bit=1
	//int Pierce:1; //+04 type26 bit=2
	//int CanSlow:1; //+04 type26 bit=3
	//int CanDestroy:1; //+04 type26 bit=4
	//int ClientSend:1; //+04 type26 bit=5
	//int GetHit:1; //+04 type26 bit=6
	//int SoftHit:1; //+04 type26 bit=7
	//int ApplyMastery:1; //+04 type26 bit=8
	//int ReturnFire:1; //+04 type26 bit=9
	//int Town:1; //+04 type26 bit=10
	//int SrcTown:1; //+04 type26 bit=11
	//int NoMultiShot:1; //+04 type26 bit=12
	//int NoUniqueMod:1; //+04 type26 bit=13
	//int Half2HSrc:1; //+04 type26 bit=14
	//int MissileSkill:1; //+04 type26 bit=15
	char pad_4[4]; //+4
	short pCltDoFunc; //+08 type3
	short pCltHitFunc; //+0A type3
	short pSrvDoFunc; //+0C type3
	short pSrvHitFunc; //+0E type3
	short pSrvDmgFunc; //+10 type3
	short TravelSound; //+12 type20 link=d2common_pSoundCodesLink
	short HitSound; //+14 type20 link=d2common_pSoundCodesLink
	short ExplosionMissile; //+16 type20 link=d2common_pMissilesLink
	short SubMissile1; //+18 type20 link=d2common_pMissilesLink
	short SubMissile2; //+1A type20 link=d2common_pMissilesLink
	short SubMissile3; //+1C type20 link=d2common_pMissilesLink
	short CltSubMissile1; //+1E type20 link=d2common_pMissilesLink
	short CltSubMissile2; //+20 type20 link=d2common_pMissilesLink
	short CltSubMissile3; //+22 type20 link=d2common_pMissilesLink
	short HitSubMissile1; //+24 type20 link=d2common_pMissilesLink
	short HitSubMissile2; //+26 type20 link=d2common_pMissilesLink
	short HitSubMissile3; //+28 type20 link=d2common_pMissilesLink
	short HitSubMissile4; //+2A type20 link=d2common_pMissilesLink
	short CltHitSubMissile1; //+2C type20 link=d2common_pMissilesLink
	short CltHitSubMissile2; //+2E type20 link=d2common_pMissilesLink
	short CltHitSubMissile3; //+30 type20 link=d2common_pMissilesLink
	short CltHitSubMissile4; //+32 type20 link=d2common_pMissilesLink
	short ProgSound; //+34 type20 link=d2common_pSoundCodesLink
	short ProgOverlay; //+36 type20 link=d2common_pOverlayTxt
	int Param1; //+38 type2
	int Param2; //+3C type2
	int Param3; //+40 type2
	int Param4; //+44 type2
	int Param5; //+48 type2
	int sHitPar1; //+4C type2
	int sHitPar2; //+50 type2
	int sHitPar3; //+54 type2
	int CltParam1; //+58 type2
	int CltParam2; //+5C type2
	int CltParam3; //+60 type2
	int CltParam4; //+64 type2
	int CltParam5; //+68 type2
	int cHitPar1; //+6C type2
	int cHitPar2; //+70 type2
	int cHitPar3; //+74 type2
	int dParam1; //+78 type2
	int dParam2; //+7C type2
	int SrvCalc1; //+80 type25 link=d2common_7C850
	int CltCalc1; //+84 type25 link=d2common_7C850
	int SHitCalc1; //+88 type25 link=d2common_7C850
	int CHitCalc1; //+8C type25 link=d2common_7C850
	int DmgCalc1; //+90 type25 link=d2common_7C850
	char HitClass; //+94 type4
	char pad_95; //+95
	short Range; //+96 type3
	short LevRange; //+98 type3
	char Vel; //+9A type4
	char VelLev; //+9B type4
	char MaxVel; //+9C type4
	char pad_9D; //+9D
	short Accel; //+9E type3
	short animrate; //+A0 type3
	short xoffset; //+A2 type3
	short yoffset; //+A4 type3
	short zoffset; //+A6 type3
	int HitFlags; //+A8 type8
	short ResultFlags; //+AC type3
	char KnockBack; //+AE type4
	char pad_AF; //+AF
	int MinDamage; //+B0 type2
	int MaxDamage; //+B4 type2
	int MinLevDam1; //+B8 type2
	int MinLevDam2; //+BC type2
	int MinLevDam3; //+C0 type2
	int MinLevDam4; //+C4 type2
	int MinLevDam5; //+C8 type2
	int MaxLevDam1; //+CC type2
	int MaxLevDam2; //+D0 type2
	int MaxLevDam3; //+D4 type2
	int MaxLevDam4; //+D8 type2
	int MaxLevDam5; //+DC type2
	int DmgSymPerCalc; //+E0 type25 link=d2common_7C850
	char EType; //+E4 type13 link=d2common_pElemTypesLink
	char pad_E5[3]; //+E5
	int EMin; //+E8 type2
	int EMax; //+EC type2
	int MinELev1; //+F0 type2
	int MinELev2; //+F4 type2
	int MinELev3; //+F8 type2
	int MinELev4; //+FC type2
	int MinELev5; //+100 type2
	int MaxELev1; //+104 type2
	int MaxELev2; //+108 type2
	int MaxELev3; //+10C type2
	int MaxELev4; //+110 type2
	int MaxELev5; //+114 type2
	int EDmgSymPerCalc; //+118 type25 link=d2common_7C850
	int ELen; //+11C type2
	int ELevLen1; //+120 type2
	int ELevLen2; //+124 type2
	int ELevLen3; //+128 type2
	char CltSrcTown; //+12C type4
	char SrcDamage; //+12D type4
	char SrcMissDmg; //+12E type4
	char Holy; //+12F type4
	char Light; //+130 type4
	char Flicker; //+131 type4
	char Red; //+132 type4
	char Green; //+133 type4
	char Blue; //+134 type4
	char InitSteps; //+135 type4
	char Activate; //+136 type4
	char LoopAnim; //+137 type4
	char CelFile[64]; //+138 type1
	char AnimLen; //+178 type4
	char pad_179[3]; //+179
	int RandStart; //+17C type2
	char SubLoop; //+180 type4
	char SubStart; //+181 type4
	char SubStop; //+182 type4
	char CollideType; //+183 type4
	char Collision; //+184 type4
	char ClientCol; //+185 type4
	char CollideKill; //+186 type4
	char CollideFriend; //+187 type4
	char NextHit; //+188 type4
	char NextDelay; //+189 type4
	char Size; //+18A type4
	char ToHit; //+18B type4
	char AlwaysExplode; //+18C type4
	char Trans; //+18D type4
	char Qty; //+18E type4
	char pad_18F; //+18F
	int SpecialSetup; //+190 type2
	short Skill; //+194 type20 link=d2common_pSkillCodesLink
	char HitShift; //+196 type4
	char pad_197[5]; //+197
	int DamageRate; //+19C type2
	char NumDirections; //+1A0 type4
	char AnimSpeed; //+1A1 type4
	char LocalBlood; //+1A2 type4
	char pad_1A3; //+1A3
};
//states size=0x3C count=d2common_nStates ret=d2common_602AC
struct states_txt {
	short state; //+00 type17 link=d2common_pStatesLink
	short overlay1; //+02 type20 link=d2common_pOverlayTxt
	short overlay2; //+04 type20 link=d2common_pOverlayTxt
	short overlay3; //+06 type20 link=d2common_pOverlayTxt
	short overlay4; //+08 type20 link=d2common_pOverlayTxt
	short castoverlay; //+0A type20 link=d2common_pOverlayTxt
	short removerlay; //+0C type20 link=d2common_pOverlayTxt
	short pgsvoverlay; //+0E type20 link=d2common_pOverlayTxt
	//int nosend:1; //+10 type26 bit=0
	//int aura:1; //+10 type26 bit=1
	//int hide:1; //+10 type26 bit=2
	//int transform:1; //+10 type26 bit=3
	//int pgsv:1; //+10 type26 bit=4
	//int active:1; //+10 type26 bit=5
	//int remhit:1; //+10 type26 bit=6
	//int damblue:1; //+10 type26 bit=7
	//int damred:1; //+10 type26 bit=8
	//int attblue:1; //+10 type26 bit=9
	//int attred:1; //+10 type26 bit=10
	//int curse:1; //+10 type26 bit=11
	//int curable:1; //+10 type26 bit=12
	//int plrstaydeath:1; //+10 type26 bit=13
	//int monstaydeath:1; //+10 type26 bit=14
	//int bossstaydeath:1; //+10 type26 bit=15
	//int disguise:1; //+10 type26 bit=16
	//int restrict:1; //+10 type26 bit=17
	//int blue:1; //+10 type26 bit=18
	//int armblue:1; //+10 type26 bit=19
	//int rfblue:1; //+10 type26 bit=20
	//int rcblue:1; //+10 type26 bit=21
	//int rlblue:1; //+10 type26 bit=22
	//int rpblue:1; //+10 type26 bit=23
	//int stambarblue:1; //+10 type26 bit=24
	//int armred:1; //+10 type26 bit=25
	//int rfred:1; //+10 type26 bit=26
	//int rcred:1; //+10 type26 bit=27
	//int rlred:1; //+10 type26 bit=28
	//int rpred:1; //+10 type26 bit=29
	//int exp:1; //+10 type26 bit=30
	//int shatter:1; //+10 type26 bit=31
	//int life:1; //+10 type26 bit=32
	//int udead:1; //+10 type26 bit=33
	//int green:1; //+10 type26 bit=34
	//int nooverlays:1; //+10 type26 bit=35
	//int noclear:1; //+10 type26 bit=36
	//int bossinv:1; //+10 type26 bit=37
	//int meleeonly:1; //+10 type26 bit=38
	//int notondead:1; //+10 type26 bit=39
	char pad_10[8]; //+10
	short stat; //+18 type20 link=d2common_pItemStatCostLink
	short setfunc; //+1A type3
	short remfunc; //+1C type3
	short group; //+1E type3
	char colorpri; //+20 type4
	char colorshift; //+21 type4
	char light-r; //+22 type4
	char light-g; //+23 type4
	char light-b; //+24 type4
	char pad_25; //+25
	short onsound; //+26 type20 link=d2common_pSoundCodesLink
	short offsound; //+28 type20 link=d2common_pSoundCodesLink
	short itemtype; //+2A type15 link=d2common_pItemTypesLink
	char itemtrans; //+2C type13 link=d2common_pColorsLink
	char gfxtype; //+2D type4
	short gfxclass; //+2E type3
	short cltevent; //+30 type20 link=d2common_pEventsLink
	short clteventfunc; //+32 type3
	short cltactivefunc; //+34 type3
	short srvactivefunc; //+36 type3
	short skill; //+38 type20 link=d2common_pSkillCodesLink
	short missile; //+3A type20 link=d2common_pMissilesLink
};
//skills size=0x23C count=d2common_nSkills ret=d2common_65DD8
struct skills_txt {
	short skill; //+00 type17 link=d2common_pSkillsLink
	char pad_2[2]; //+2
	//int decquant:1; //+04 type26 bit=0
	//int lob:1; //+04 type26 bit=1
	//int progressive:1; //+04 type26 bit=2
	//int finishing:1; //+04 type26 bit=3
	//int passive:1; //+04 type26 bit=4
	//int aura:1; //+04 type26 bit=5
	//int periodic:1; //+04 type26 bit=6
	//int prgstack:1; //+04 type26 bit=7
	//int InTown:1; //+04 type26 bit=8
	//int Kick:1; //+04 type26 bit=9
	//int InGame:1; //+04 type26 bit=10
	//int repeat:1; //+04 type26 bit=11
	//int stsuccessonly:1; //+04 type26 bit=12
	//int stsounddelay:1; //+04 type26 bit=13
	//int weaponsnd:1; //+04 type26 bit=14
	//int immediate:1; //+04 type26 bit=15
	//int noammo:1; //+04 type26 bit=16
	//int enhanceable:1; //+04 type26 bit=17
	//int durability:1; //+04 type26 bit=18
	//int UseAttackRate:1; //+04 type26 bit=19
	//int TargetableOnly:1; //+04 type26 bit=20
	//int SearchEnemyXY:1; //+04 type26 bit=21
	//int SearchEnemyNear:1; //+04 type26 bit=22
	//int SearchOpenXY:1; //+04 type26 bit=23
	//int TargetCorpse:1; //+04 type26 bit=24
	//int TargetPet:1; //+04 type26 bit=25
	//int TargetAlly:1; //+04 type26 bit=26
	//int TargetItem:1; //+04 type26 bit=27
	//int AttackNoMana:1; //+04 type26 bit=28
	//int ItemTgtDo:1; //+04 type26 bit=29
	//int leftskill:1; //+04 type26 bit=30
	//int interrupt:1; //+04 type26 bit=31
	//int TgtPlaceCheck:1; //+04 type26 bit=32
	//int ItemCheckStart:1; //+04 type26 bit=33
	//int ItemCltCheckStart:1; //+04 type26 bit=34
	//int general:1; //+04 type26 bit=35
	//int scroll:1; //+04 type26 bit=36
	//int usemanaondo:1; //+04 type26 bit=37
	//int warp:1; //+04 type26 bit=38
	char pad_4[8]; //+4
	char charclass; //+0C type13 link=d2common_pPlayerClassLink
	char pad_D[3]; //+D
	char anim; //+10 type13 link=d2common_pPlrModeLink
	char monanim; //+11 type13 link=d2common_pMonModeLink
	char seqtrans; //+12 type13 link=d2common_pPlrModeLink
	char seqnum; //+13 type4
	char range; //+14 type13 link=12E1AC
	char SelectProc; //+15 type4
	char seqinput; //+16 type4
	char pad_17; //+17
	short itypea1; //+18 type15 link=d2common_pItemTypesLink
	short itypea2; //+1A type15 link=d2common_pItemTypesLink
	short itypea3; //+1C type15 link=d2common_pItemTypesLink
	short itypeb1; //+1E type15 link=d2common_pItemTypesLink
	short itypeb2; //+20 type15 link=d2common_pItemTypesLink
	short itypeb3; //+22 type15 link=d2common_pItemTypesLink
	short etypea1; //+24 type15 link=d2common_pItemTypesLink
	short etypea2; //+26 type15 link=d2common_pItemTypesLink
	short etypeb1; //+28 type15 link=d2common_pItemTypesLink
	short etypeb2; //+2A type15 link=d2common_pItemTypesLink
	short srvstfunc; //+2C type3
	short srvdofunc; //+2E type3
	short srvprgfunc1; //+30 type3
	short srvprgfunc2; //+32 type3
	short srvprgfunc3; //+34 type3
	char pad_36[2]; //+36
	int prgcalc1; //+38 type25 link=d2common_60850
	int prgcalc2; //+3C type25 link=d2common_60850
	int prgcalc3; //+40 type25 link=d2common_60850
	char prgdam; //+44 type4
	char pad_45; //+45
	short srvmissile; //+46 type20 link=d2common_pMissilesLink
	short srvmissilea; //+48 type20 link=d2common_pMissilesLink
	short srvmissileb; //+4A type20 link=d2common_pMissilesLink
	short srvmissilec; //+4C type20 link=d2common_pMissilesLink
	short srvoverlay; //+4E type20 link=d2common_pOverlayTxt
	int aurafilter; //+50 type2
	short aurastat1; //+54 type20 link=d2common_pItemStatCostLink
	short aurastat2; //+56 type20 link=d2common_pItemStatCostLink
	short aurastat3; //+58 type20 link=d2common_pItemStatCostLink
	short aurastat4; //+5A type20 link=d2common_pItemStatCostLink
	short aurastat5; //+5C type20 link=d2common_pItemStatCostLink
	short aurastat6; //+5E type20 link=d2common_pItemStatCostLink
	int auralencalc; //+60 type25 link=d2common_60850
	int aurarangecalc; //+64 type25 link=d2common_60850
	int aurastatcalc1; //+68 type25 link=d2common_60850
	int aurastatcalc2; //+6C type25 link=d2common_60850
	int aurastatcalc3; //+70 type25 link=d2common_60850
	int aurastatcalc4; //+74 type25 link=d2common_60850
	int aurastatcalc5; //+78 type25 link=d2common_60850
	int aurastatcalc6; //+7C type25 link=d2common_60850
	short aurastate; //+80 type20 link=d2common_pStatesLink
	short auratargetstate; //+82 type20 link=d2common_pStatesLink
	short auraevent1; //+84 type20 link=d2common_pEventsLink
	short auraevent2; //+86 type20 link=d2common_pEventsLink
	short auraevent3; //+88 type20 link=d2common_pEventsLink
	short auraeventfunc1; //+8A type3
	short auraeventfunc2; //+8C type3
	short auraeventfunc3; //+8E type3
	short auratgtevent; //+90 type20 link=d2common_pEventsLink
	short auratgteventfunc; //+92 type3
	short passivestate; //+94 type20 link=d2common_pStatesLink
	short passiveitype; //+96 type15 link=d2common_pItemTypesLink
	short passivestat1; //+98 type20 link=d2common_pItemStatCostLink
	short passivestat2; //+9A type20 link=d2common_pItemStatCostLink
	short passivestat3; //+9C type20 link=d2common_pItemStatCostLink
	short passivestat4; //+9E type20 link=d2common_pItemStatCostLink
	short passivestat5; //+A0 type20 link=d2common_pItemStatCostLink
	char pad_A2[2]; //+A2
	int passivecalc1; //+A4 type25 link=d2common_60850
	int passivecalc2; //+A8 type25 link=d2common_60850
	int passivecalc3; //+AC type25 link=d2common_60850
	int passivecalc4; //+B0 type25 link=d2common_60850
	int passivecalc5; //+B4 type25 link=d2common_60850
	short passiveevent; //+B8 type20 link=d2common_pEventsLink
	short passiveeventfunc; //+BA type3
	short summon; //+BC type20 link=12E1B0
	char pettype; //+BE type21 link=d2common_pPetTypesLink
	char summode; //+BF type13 link=d2common_pMonModeLink
	int petmax; //+C0 type25 link=d2common_60850
	short sumskill1; //+C4 type20 link=d2common_pSkillsLink
	short sumskill2; //+C6 type20 link=d2common_pSkillsLink
	short sumskill3; //+C8 type20 link=d2common_pSkillsLink
	short sumskill4; //+CA type20 link=d2common_pSkillsLink
	short sumskill5; //+CC type20 link=d2common_pSkillsLink
	char pad_CE[2]; //+CE
	int sumsk1calc; //+D0 type25 link=d2common_60850
	int sumsk2calc; //+D4 type25 link=d2common_60850
	int sumsk3calc; //+D8 type25 link=d2common_60850
	int sumsk4calc; //+DC type25 link=d2common_60850
	int sumsk5calc; //+E0 type25 link=d2common_60850
	short sumumod; //+E4 type3
	short sumoverlay; //+E6 type20 link=d2common_pOverlayTxt
	short cltmissile; //+E8 type20 link=d2common_pMissilesLink
	short cltmissilea; //+EA type20 link=d2common_pMissilesLink
	short cltmissileb; //+EC type20 link=d2common_pMissilesLink
	short cltmissilec; //+EE type20 link=d2common_pMissilesLink
	short cltmissiled; //+F0 type20 link=d2common_pMissilesLink
	short cltstfunc; //+F2 type3
	short cltdofunc; //+F4 type3
	short cltprgfunc1; //+F6 type3
	short cltprgfunc2; //+F8 type3
	short cltprgfunc3; //+FA type3
	short stsound; //+FC type20 link=d2common_pSoundCodesLink
	short stsoundclass; //+FE type20 link=d2common_pSoundCodesLink
	short dosound; //+100 type20 link=d2common_pSoundCodesLink
	short dosound_a; //+102 type20 link=d2common_pSoundCodesLink
	short dosound_b; //+104 type20 link=d2common_pSoundCodesLink
	short castoverlay; //+106 type20 link=d2common_pOverlayTxt
	short tgtoverlay; //+108 type20 link=d2common_pOverlayTxt
	short tgtsound; //+10A type20 link=d2common_pSoundCodesLink
	short prgoverlay; //+10C type20 link=d2common_pOverlayTxt
	short prgsound; //+10E type20 link=d2common_pSoundCodesLink
	short cltoverlaya; //+110 type20 link=d2common_pOverlayTxt
	short cltoverlayb; //+112 type20 link=d2common_pOverlayTxt
	int cltcalc1; //+114 type25 link=d2common_60850
	int cltcalc2; //+118 type25 link=d2common_60850
	int cltcalc3; //+11C type25 link=d2common_60850
	char ItemTarget; //+120 type4
	char pad_121; //+121
	short ItemCastSound; //+122 type20 link=d2common_pSoundCodesLink
	short ItemCastOverlay; //+124 type20 link=d2common_pOverlayTxt
	char pad_126[2]; //+126
	int perdelay; //+128 type25 link=d2common_60850
	short maxlvl; //+12C type3
	short ResultFlags; //+12E type3
	int HitFlags; //+130 type8
	int HitClass; //+134 type8
	int calc1; //+138 type25 link=d2common_60850
	int calc2; //+13C type25 link=d2common_60850
	int calc3; //+140 type25 link=d2common_60850
	int calc4; //+144 type25 link=d2common_60850
	int Param1; //+148 type2
	int Param2; //+14C type2
	int Param3; //+150 type2
	int Param4; //+154 type2
	int Param5; //+158 type2
	int Param6; //+15C type2
	int Param7; //+160 type2
	int Param8; //+164 type2
	char weapsel; //+168 type4
	char pad_169; //+169
	short ItemEffect; //+16A type3
	short ItemCltEffect; //+16C type3
	char pad_16E[2]; //+16E
	int skpoints; //+170 type25 link=d2common_60850
	short reqlevel; //+174 type3
	short reqstr; //+176 type3
	short reqdex; //+178 type3
	short reqint; //+17A type3
	short reqvit; //+17C type3
	short reqskill1; //+17E type20 link=d2common_pSkillsLink
	short reqskill2; //+180 type20 link=d2common_pSkillsLink
	short reqskill3; //+182 type20 link=d2common_pSkillsLink
	short startmana; //+184 type3
	short minmana; //+186 type3
	short manashift; //+188 type3
	short mana; //+18A type3
	short lvlmana; //+18C type3
	char attackrank; //+18E type4
	char lineofsight; //+18F type4
	int delay; //+190 type25 link=d2common_60850
	short skilldesc; //+194 type20 link=12E1B4
	char pad_196[2]; //+196
	int ToHit; //+198 type2
	int LevToHit; //+19C type2
	int ToHitCalc; //+1A0 type25 link=d2common_60850
	char HitShift; //+1A4 type4
	char SrcDam; //+1A5 type4
	char pad_1A6[2]; //+1A6
	int MinDam; //+1A8 type2
	int MaxDam; //+1AC type2
	int MinLevDam1; //+1B0 type2
	int MinLevDam2; //+1B4 type2
	int MinLevDam3; //+1B8 type2
	int MinLevDam4; //+1BC type2
	int MinLevDam5; //+1C0 type2
	int MaxLevDam1; //+1C4 type2
	int MaxLevDam2; //+1C8 type2
	int MaxLevDam3; //+1CC type2
	int MaxLevDam4; //+1D0 type2
	int MaxLevDam5; //+1D4 type2
	int DmgSymPerCalc; //+1D8 type25 link=d2common_60850
	char EType; //+1DC type13 link=d2common_pElemTypesLink
	char pad_1DD[3]; //+1DD
	int EMin; //+1E0 type2
	int EMax; //+1E4 type2
	int EMinLev1; //+1E8 type2
	int EMinLev2; //+1EC type2
	int EMinLev3; //+1F0 type2
	int EMinLev4; //+1F4 type2
	int EMinLev5; //+1F8 type2
	int EMaxLev1; //+1FC type2
	int EMaxLev2; //+200 type2
	int EMaxLev3; //+204 type2
	int EMaxLev4; //+208 type2
	int EMaxLev5; //+20C type2
	int EDmgSymPerCalc; //+210 type25 link=d2common_60850
	int ELen; //+214 type2
	int ELevLen1; //+218 type2
	int ELevLen2; //+21C type2
	int ELevLen3; //+220 type2
	int ELenSymPerCalc; //+224 type25 link=d2common_60850
	char restrict; //+228 type4
	char pad_229; //+229
	short state1; //+22A type20 link=d2common_pStatesLink
	short state2; //+22C type20 link=d2common_pStatesLink
	short state3; //+22E type20 link=d2common_pStatesLink
	char aitype; //+230 type4
	char pad_231; //+231
	short aibonus; //+232 type3
	int cost_mult; //+234 type2
	int cost_add; //+238 type2
};
//skilldesc size=0x120 count=d2common_nSkillDesc ret=d2common_65E24
struct skilldesc_txt {
	short skilldesc; //+00 type17 link=d2common_pSkillDescLink
	char skillpage; //+02 type4
	char skillrow; //+03 type4
	char skillcolumn; //+04 type4
	char ListRow; //+05 type4
	char ListPool; //+06 type4
	char iconcel; //+07 type4
	short str_name; //+08 type22 link=d2common_5E350
	short str_short; //+0A type22 link=d2common_5E350
	short str_long; //+0C type22 link=d2common_5E350
	short str_alt; //+0E type22 link=d2common_5E350
	short str_mana; //+10 type22 link=d2common_5E350
	short descdam; //+12 type3
	short descatt; //+14 type3
	char pad_16[2]; //+16
	int ddam_calc1; //+18 type25 link=d2common_607D0
	int ddam_calc2; //+1C type25 link=d2common_607D0
	char p1dmelem; //+20 type13 link=d2common_pElemTypesLink
	char p2dmelem; //+21 type13 link=d2common_pElemTypesLink
	char p3dmelem; //+22 type13 link=d2common_pElemTypesLink
	char pad_23; //+23
	int p1dmmin; //+24 type25 link=d2common_607D0
	int p2dmmin; //+28 type25 link=d2common_607D0
	int p3dmmin; //+2C type25 link=d2common_607D0
	int p1dmmax; //+30 type25 link=d2common_607D0
	int p2dmmax; //+34 type25 link=d2common_607D0
	int p3dmmax; //+38 type25 link=d2common_607D0
	short descmissile1; //+3C type20 link=d2common_pMissilesLink
	short descmissile2; //+3E type20 link=d2common_pMissilesLink
	short descmissile3; //+40 type20 link=d2common_pMissilesLink
	char descline1; //+42 type4
	char descline2; //+43 type4
	char descline3; //+44 type4
	char descline4; //+45 type4
	char descline5; //+46 type4
	char descline6; //+47 type4
	char dsc2line1; //+48 type4
	char dsc2line2; //+49 type4
	char dsc2line3; //+4A type4
	char dsc2line4; //+4B type4
	char dsc3line1; //+4C type4
	char dsc3line2; //+4D type4
	char dsc3line3; //+4E type4
	char dsc3line4; //+4F type4
	char dsc3line5; //+50 type4
	char dsc3line6; //+51 type4
	char dsc3line7; //+52 type4
	char pad_53; //+53
	short desctexta1; //+54 type22 link=d2common_5E350
	short desctexta2; //+56 type22 link=d2common_5E350
	short desctexta3; //+58 type22 link=d2common_5E350
	short desctexta4; //+5A type22 link=d2common_5E350
	short desctexta5; //+5C type22 link=d2common_5E350
	short desctexta6; //+5E type22 link=d2common_5E350
	short dsc2texta1; //+60 type22 link=d2common_5E350
	short dsc2texta2; //+62 type22 link=d2common_5E350
	short dsc2texta3; //+64 type22 link=d2common_5E350
	short dsc2texta4; //+66 type22 link=d2common_5E350
	short dsc3texta1; //+68 type22 link=d2common_5E350
	short dsc3texta2; //+6A type22 link=d2common_5E350
	short dsc3texta3; //+6C type22 link=d2common_5E350
	short dsc3texta4; //+6E type22 link=d2common_5E350
	short dsc3texta5; //+70 type22 link=d2common_5E350
	short dsc3texta6; //+72 type22 link=d2common_5E350
	short dsc3texta7; //+74 type22 link=d2common_5E350
	short desctextb1; //+76 type22 link=d2common_5E350
	short desctextb2; //+78 type22 link=d2common_5E350
	short desctextb3; //+7A type22 link=d2common_5E350
	short desctextb4; //+7C type22 link=d2common_5E350
	short desctextb5; //+7E type22 link=d2common_5E350
	short desctextb6; //+80 type22 link=d2common_5E350
	short dsc2textb1; //+82 type22 link=d2common_5E350
	short dsc2textb2; //+84 type22 link=d2common_5E350
	short dsc2textb3; //+86 type22 link=d2common_5E350
	short dsc2textb4; //+88 type22 link=d2common_5E350
	short dsc3textb1; //+8A type22 link=d2common_5E350
	short dsc3textb2; //+8C type22 link=d2common_5E350
	short dsc3textb3; //+8E type22 link=d2common_5E350
	short dsc3textb4; //+90 type22 link=d2common_5E350
	short dsc3textb5; //+92 type22 link=d2common_5E350
	short dsc3textb6; //+94 type22 link=d2common_5E350
	short dsc3textb7; //+96 type22 link=d2common_5E350
	int desccalca1; //+98 type25 link=d2common_607D0
	int desccalca2; //+9C type25 link=d2common_607D0
	int desccalca3; //+A0 type25 link=d2common_607D0
	int desccalca4; //+A4 type25 link=d2common_607D0
	int desccalca5; //+A8 type25 link=d2common_607D0
	int desccalca6; //+AC type25 link=d2common_607D0
	int dsc2calca1; //+B0 type25 link=d2common_607D0
	int dsc2calca2; //+B4 type25 link=d2common_607D0
	int dsc2calca3; //+B8 type25 link=d2common_607D0
	int dsc2calca4; //+BC type25 link=d2common_607D0
	int dsc3calca1; //+C0 type25 link=d2common_607D0
	int dsc3calca2; //+C4 type25 link=d2common_607D0
	int dsc3calca3; //+C8 type25 link=d2common_607D0
	int dsc3calca4; //+CC type25 link=d2common_607D0
	int dsc3calca5; //+D0 type25 link=d2common_607D0
	int dsc3calca6; //+D4 type25 link=d2common_607D0
	int dsc3calca7; //+D8 type25 link=d2common_607D0
	int desccalcb1; //+DC type25 link=d2common_607D0
	int desccalcb2; //+E0 type25 link=d2common_607D0
	int desccalcb3; //+E4 type25 link=d2common_607D0
	int desccalcb4; //+E8 type25 link=d2common_607D0
	int desccalcb5; //+EC type25 link=d2common_607D0
	int desccalcb6; //+F0 type25 link=d2common_607D0
	int dsc2calcb1; //+F4 type25 link=d2common_607D0
	int dsc2calcb2; //+F8 type25 link=d2common_607D0
	int dsc2calcb3; //+FC type25 link=d2common_607D0
	int dsc2calcb4; //+100 type25 link=d2common_607D0
	int dsc3calcb1; //+104 type25 link=d2common_607D0
	int dsc3calcb2; //+108 type25 link=d2common_607D0
	int dsc3calcb3; //+10C type25 link=d2common_607D0
	int dsc3calcb4; //+110 type25 link=d2common_607D0
	int dsc3calcb5; //+114 type25 link=d2common_607D0
	int dsc3calcb6; //+118 type25 link=d2common_607D0
	int dsc3calcb7; //+11C type25 link=d2common_607D0
};
//charstats size=0xC4 count=d2common_nCharStats ret=d2common_6169D
struct charstats_txt {
	char pad_0[32]; //+0
	char class[15]; //+20 type1
	char pad_2F; //+2F
	char str; //+30 type4
	char dex; //+31 type4
	char int; //+32 type4
	char vit; //+33 type4
	char stamina; //+34 type4
	char hpadd; //+35 type4
	char PercentStr; //+36 type4
	char PercentInt; //+37 type4
	char PercentDex; //+38 type4
	char PercentVit; //+39 type4
	char ManaRegen; //+3A type4
	char pad_3B; //+3B
	int ToHitFactor; //+3C type2
	char WalkVelocity; //+40 type4
	char RunVelocity; //+41 type4
	char RunDrain; //+42 type4
	char LifePerLevel; //+43 type4
	char StaminaPerLevel; //+44 type4
	char ManaPerLevel; //+45 type4
	char LifePerVitality; //+46 type4
	char StaminaPerVitality; //+47 type4
	char ManaPerMagic; //+48 type4
	char BlockFactor; //+49 type4
	char pad_4A[2]; //+4A
	int basewclass; //+4C type9
	char StatPerLevel; //+50 type4
	char pad_51; //+51
	short StrAllSkills; //+52 type22 link=d2common_5E350
	short StrSkillTab1; //+54 type22 link=d2common_5E350
	short StrSkillTab2; //+56 type22 link=d2common_5E350
	short StrSkillTab3; //+58 type22 link=d2common_5E350
	short StrClassOnly; //+5A type22 link=d2common_5E350
	int item1; //+5C type9
	char item1loc; //+60 type13 link=d2common_pBodyLocsLink
	char item1count; //+61 type4
	char pad_62[2]; //+62
	int item2; //+64 type9
	char item2loc; //+68 type13 link=d2common_pBodyLocsLink
	char item2count; //+69 type4
	char pad_6A[2]; //+6A
	int item3; //+6C type9
	char item3loc; //+70 type13 link=d2common_pBodyLocsLink
	char item3count; //+71 type4
	char pad_72[2]; //+72
	int item4; //+74 type9
	char item4loc; //+78 type13 link=d2common_pBodyLocsLink
	char item4count; //+79 type4
	char pad_7A[2]; //+7A
	int item5; //+7C type9
	char item5loc; //+80 type13 link=d2common_pBodyLocsLink
	char item5count; //+81 type4
	char pad_82[2]; //+82
	int item6; //+84 type9
	char item6loc; //+88 type13 link=d2common_pBodyLocsLink
	char item6count; //+89 type4
	char pad_8A[2]; //+8A
	int item7; //+8C type9
	char item7loc; //+90 type13 link=d2common_pBodyLocsLink
	char item7count; //+91 type4
	char pad_92[2]; //+92
	int item8; //+94 type9
	char item8loc; //+98 type13 link=d2common_pBodyLocsLink
	char item8count; //+99 type4
	char pad_9A[2]; //+9A
	int item9; //+9C type9
	char item9loc; //+A0 type13 link=d2common_pBodyLocsLink
	char item9count; //+A1 type4
	char pad_A2[2]; //+A2
	int item10; //+A4 type9
	char item10loc; //+A8 type13 link=d2common_pBodyLocsLink
	char item10count; //+A9 type4
	char pad_AA[2]; //+AA
	short StartSkill; //+AC type20 link=d2common_pSkillsLink
	short Skill_1; //+AE type20 link=d2common_pSkillsLink
	short Skill_2; //+B0 type20 link=d2common_pSkillsLink
	short Skill_3; //+B2 type20 link=d2common_pSkillsLink
	short Skill_4; //+B4 type20 link=d2common_pSkillsLink
	short Skill_5; //+B6 type20 link=d2common_pSkillsLink
	short Skill_6; //+B8 type20 link=d2common_pSkillsLink
	short Skill_7; //+BA type20 link=d2common_pSkillsLink
	short Skill_8; //+BC type20 link=d2common_pSkillsLink
	short Skill_9; //+BE type20 link=d2common_pSkillsLink
	short Skill_10; //+C0 type20 link=d2common_pSkillsLink
	char pad_C2[2]; //+C2
};
//arena size=0x1C count=0 ret=d2common_80E70
struct arena_txt {
	int Suicide; //+00 type2
	int PlayerKill; //+04 type2
	int PlayerKillPercent; //+08 type2
	int MonsterKill; //+0C type2
	int PlayerDeath; //+10 type2
	int PlayerDeathPercent; //+14 type2
	int MonsterDeath; //+18 type2
};
//chartemplate size=0xF0 count=d2common_A13FC ret=d2common_80D2E
struct chartemplate_txt {
	char Name[29]; //+00 type1
	char pad_1D; //+1D
	char class; //+1E type13 link=d2common_pPlayerClassLink
	char level; //+1F type4
	char act; //+20 type4
	char str; //+21 type4
	char dex; //+22 type4
	char int; //+23 type4
	char vit; //+24 type4
	char Mana; //+25 type4
	char Hitpoints; //+26 type4
	char ManaRegenBonus; //+27 type4
	char Velocity; //+28 type4
	char AttackRate; //+29 type4
	char OtherRate; //+2A type4
	char pad_2B; //+2B
	int RightSkill; //+2C type2
	int Skill1; //+30 type2
	int Skill2; //+34 type2
	int Skill3; //+38 type2
	int Skill4; //+3C type2
	int Skill5; //+40 type2
	int Skill6; //+44 type2
	int Skill7; //+48 type2
	int Skill8; //+4C type2
	int Skill9; //+50 type2
	int SkillLevel1; //+54 type2
	int SkillLevel2; //+58 type2
	int SkillLevel3; //+5C type2
	int SkillLevel4; //+60 type2
	int SkillLevel5; //+64 type2
	int SkillLevel6; //+68 type2
	int SkillLevel7; //+6C type2
	int SkillLevel8; //+70 type2
	int SkillLevel9; //+74 type2
	int item1; //+78 type9
	char item1loc; //+7C type4
	char item1count; //+7D type4
	char pad_7E[2]; //+7E
	int item2; //+80 type9
	char item2loc; //+84 type4
	char item2count; //+85 type4
	char pad_86[2]; //+86
	int item3; //+88 type9
	char item3loc; //+8C type4
	char item3count; //+8D type4
	char pad_8E[2]; //+8E
	int item4; //+90 type9
	char item4loc; //+94 type4
	char item4count; //+95 type4
	char pad_96[2]; //+96
	int item5; //+98 type9
	char item5loc; //+9C type4
	char item5count; //+9D type4
	char pad_9E[2]; //+9E
	int item6; //+A0 type9
	char item6loc; //+A4 type4
	char item6count; //+A5 type4
	char pad_A6[2]; //+A6
	int item7; //+A8 type9
	char item7loc; //+AC type4
	char item7count; //+AD type4
	char pad_AE[2]; //+AE
	int item8; //+B0 type9
	char item8loc; //+B4 type4
	char item8count; //+B5 type4
	char pad_B6[2]; //+B6
	int item9; //+B8 type9
	char item9loc; //+BC type4
	char item9count; //+BD type4
	char pad_BE[2]; //+BE
	int item10; //+C0 type9
	char item10loc; //+C4 type4
	char item10count; //+C5 type4
	char pad_C6[2]; //+C6
	int item11; //+C8 type9
	char item11loc; //+CC type4
	char item11count; //+CD type4
	char pad_CE[2]; //+CE
	int item12; //+D0 type9
	char item12loc; //+D4 type4
	char item12count; //+D5 type4
	char pad_D6[2]; //+D6
	int item13; //+D8 type9
	char item13loc; //+DC type4
	char item13count; //+DD type4
	char pad_DE[2]; //+DE
	int item14; //+E0 type9
	char item14loc; //+E4 type4
	char item14count; //+E5 type4
	char pad_E6[2]; //+E6
	int item15; //+E8 type9
	char item15loc; //+EC type4
	char item15count; //+ED type4
	char pad_EE[2]; //+EE
};
//weapons size=0x1A8 count=d2common_WeaponsTxts ret=d2common_7A8E6
struct weapons_txt {
	char flippyfile[31]; //+00 type1
	char pad_1F; //+1F
	char invfile[31]; //+20 type1
	char pad_3F; //+3F
	char uniqueinvfile[31]; //+40 type1
	char pad_5F; //+5F
	char setinvfile[31]; //+60 type1
	char pad_7F; //+7F
	int code; //+80 type10 link=d2common_pItemLink
	int normcode; //+84 type9
	int ubercode; //+88 type9
	int ultracode; //+8C type9
	int alternategfx; //+90 type9
	int pSpell; //+94 type2
	short state; //+98 type20 link=d2common_pStatesLink
	short cstate1; //+9A type20 link=d2common_pStatesLink
	short cstate2; //+9C type20 link=d2common_pStatesLink
	short stat1; //+9E type20 link=d2common_pItemStatCostLink
	short stat2; //+A0 type20 link=d2common_pItemStatCostLink
	short stat3; //+A2 type20 link=d2common_pItemStatCostLink
	int calc1; //+A4 type25 link=d2common_71BA0
	int calc2; //+A8 type25 link=d2common_71BA0
	int calc3; //+AC type25 link=d2common_71BA0
	int len; //+B0 type25 link=d2common_71BA0
	char spelldesc; //+B4 type4
	char pad_B5; //+B5
	short spelldescstr; //+B6 type22 link=d2common_5E350
	int spelldesccalc; //+B8 type25 link=d2common_71BA0
	int BetterGem; //+BC type9
	int wclass; //+C0 type9
	int 2handedwclass; //+C4 type9
	int TMogType; //+C8 type9
	int minac; //+CC type2
	int maxac; //+D0 type2
	int gamble_cost; //+D4 type2
	int speed; //+D8 type2
	int bitfield1; //+DC type2
	int cost; //+E0 type2
	int minstack; //+E4 type2
	int maxstack; //+E8 type2
	int spawnstack; //+EC type2
	int gemoffset; //+F0 type2
	short namestr; //+F4 type22 link=d2common_5E350
	short version; //+F6 type3
	short auto_prefix; //+F8 type3
	short missiletype; //+FA type3
	char rarity; //+FC type4
	char level; //+FD type4
	char mindam; //+FE type6
	char maxdam; //+FF type6
	char minmisdam; //+100 type4
	char maxmisdam; //+101 type4
	char 2handmindam; //+102 type6
	char 2handmaxdam; //+103 type6
	char rangeadder; //+104 type6
	char pad_105; //+105
	short strbonus; //+106 type3
	short dexbonus; //+108 type3
	short reqstr; //+10A type3
	short reqdex; //+10C type3
	char absorbs; //+10E type6
	char invwidth; //+10F type4
	char invheight; //+110 type4
	char block; //+111 type6
	char durability; //+112 type6
	char nodurability; //+113 type4
	char missile; //+114 type6
	char component; //+115 type4
	char rArm; //+116 type6
	char lArm; //+117 type6
	char torso; //+118 type6
	char legs; //+119 type6
	char rspad; //+11A type6
	char lspad; //+11B type6
	char 2handed; //+11C type6
	char useable; //+11D type4
	short type; //+11E type15 link=d2common_pItemTypesLink
	short type2; //+120 type15 link=d2common_pItemTypesLink
	char subtype; //+122 type6
	char pad_123; //+123
	short dropsound; //+124 type20 link=d2common_pSoundCodesLink
	short usesound; //+126 type20 link=d2common_pSoundCodesLink
	char dropsfxframe; //+128 type4
	char unique; //+129 type4
	char quest; //+12A type4
	char questdiffcheck; //+12B type4
	char transparent; //+12C type4
	char transtbl; //+12D type4
	char pad_12E; //+12E
	char lightradius; //+12F type4
	char belt; //+130 type4
	char autobelt; //+131 type4
	char stackable; //+132 type4
	char spawnable; //+133 type4
	char spellicon; //+134 type6
	char durwarning; //+135 type4
	char qntwarning; //+136 type4
	char hasinv; //+137 type6
	char gemsockets; //+138 type6
	char Transmogrify; //+139 type6
	char TMogMin; //+13A type6
	char TMogMax; //+13B type6
	char hit_class; //+13C type13 link=d2common_pHitClassLink
	char 1or2handed; //+13D type6
	char gemapplytype; //+13E type4
	char levelreq; //+13F type4
	char magic_lvl; //+140 type4
	char Transform; //+141 type4
	char InvTrans; //+142 type4
	char compactsave; //+143 type6
	char SkipName; //+144 type4
	char Nameable; //+145 type4
	char AkaraMin; //+146 type4
	char GheedMin; //+147 type4
	char CharsiMin; //+148 type4
	char FaraMin; //+149 type4
	char LysanderMin; //+14A type4
	char DrognanMin; //+14B type4
	char HraltiMin; //+14C type4
	char AlkorMin; //+14D type4
	char OrmusMin; //+14E type4
	char ElzixMin; //+14F type4
	char AshearaMin; //+150 type4
	char CainMin; //+151 type4
	char HalbuMin; //+152 type4
	char JamellaMin; //+153 type4
	char MalahMin; //+154 type4
	char LarzukMin; //+155 type4
	char DrehyaMin; //+156 type4
	char AkaraMax; //+157 type4
	char GheedMax; //+158 type4
	char CharsiMax; //+159 type4
	char FaraMax; //+15A type4
	char LysanderMax; //+15B type4
	char DrognanMax; //+15C type4
	char HraltiMax; //+15D type4
	char AlkorMax; //+15E type4
	char OrmusMax; //+15F type4
	char ElzixMax; //+160 type4
	char AshearaMax; //+161 type4
	char CainMax; //+162 type4
	char HalbuMax; //+163 type4
	char JamellaMax; //+164 type4
	char MalahMax; //+165 type4
	char LarzukMax; //+166 type4
	char DrehyaMax; //+167 type4
	char AkaraMagicMin; //+168 type4
	char GheedMagicMin; //+169 type4
	char CharsiMagicMin; //+16A type4
	char FaraMagicMin; //+16B type4
	char LysanderMagicMin; //+16C type4
	char DrognanMagicMin; //+16D type4
	char HraltiMagicMin; //+16E type4
	char AlkorMagicMin; //+16F type4
	char OrmusMagicMin; //+170 type4
	char ElzixMagicMin; //+171 type4
	char AshearaMagicMin; //+172 type4
	char CainMagicMin; //+173 type4
	char HalbuMagicMin; //+174 type4
	char JamellaMagicMin; //+175 type4
	char MalahMagicMin; //+176 type4
	char LarzukMagicMin; //+177 type4
	char DrehyaMagicMin; //+178 type4
	char AkaraMagicMax; //+179 type4
	char GheedMagicMax; //+17A type4
	char CharsiMagicMax; //+17B type4
	char FaraMagicMax; //+17C type4
	char LysanderMagicMax; //+17D type4
	char DrognanMagicMax; //+17E type4
	char HraltiMagicMax; //+17F type4
	char AlkorMagicMax; //+180 type4
	char OrmusMagicMax; //+181 type4
	char ElzixMagicMax; //+182 type4
	char AshearaMagicMax; //+183 type4
	char CainMagicMax; //+184 type4
	char HalbuMagicMax; //+185 type4
	char JamellaMagicMax; //+186 type4
	char MalahMagicMax; //+187 type4
	char LarzukMagicMax; //+188 type4
	char DrehyaMagicMax; //+189 type4
	char AkaraMagicLvl; //+18A type4
	char GheedMagicLvl; //+18B type4
	char CharsiMagicLvl; //+18C type4
	char FaraMagicLvl; //+18D type4
	char LysanderMagicLvl; //+18E type4
	char DrognanMagicLvl; //+18F type4
	char HraltiMagicLvl; //+190 type4
	char AlkorMagicLvl; //+191 type4
	char OrmusMagicLvl; //+192 type4
	char ElzixMagicLvl; //+193 type4
	char AshearaMagicLvl; //+194 type4
	char CainMagicLvl; //+195 type4
	char HalbuMagicLvl; //+196 type4
	char JamellaMagicLvl; //+197 type4
	char MalahMagicLvl; //+198 type4
	char LarzukMagicLvl; //+199 type4
	char DrehyaMagicLvl; //+19A type4
	char pad_19B; //+19B
	int NightmareUpgrade; //+19C type9
	int HellUpgrade; //+1A0 type9
	char PermStoreItem; //+1A4 type4
	char multibuy; //+1A5 type4
	char pad_1A6[2]; //+1A6
};
//armor size=0x1A8 count=d2common_ArmorTxts ret=d2common_7A906
struct armor_txt {
	char flippyfile[31]; //+00 type1
	char pad_1F; //+1F
	char invfile[31]; //+20 type1
	char pad_3F; //+3F
	char uniqueinvfile[31]; //+40 type1
	char pad_5F; //+5F
	char setinvfile[31]; //+60 type1
	char pad_7F; //+7F
	int code; //+80 type10 link=d2common_pItemLink
	int normcode; //+84 type9
	int ubercode; //+88 type9
	int ultracode; //+8C type9
	int alternategfx; //+90 type9
	int pSpell; //+94 type2
	short state; //+98 type20 link=d2common_pStatesLink
	short cstate1; //+9A type20 link=d2common_pStatesLink
	short cstate2; //+9C type20 link=d2common_pStatesLink
	short stat1; //+9E type20 link=d2common_pItemStatCostLink
	short stat2; //+A0 type20 link=d2common_pItemStatCostLink
	short stat3; //+A2 type20 link=d2common_pItemStatCostLink
	int calc1; //+A4 type25 link=d2common_71BA0
	int calc2; //+A8 type25 link=d2common_71BA0
	int calc3; //+AC type25 link=d2common_71BA0
	int len; //+B0 type25 link=d2common_71BA0
	char spelldesc; //+B4 type4
	char pad_B5; //+B5
	short spelldescstr; //+B6 type22 link=d2common_5E350
	int spelldesccalc; //+B8 type25 link=d2common_71BA0
	int BetterGem; //+BC type9
	int wclass; //+C0 type9
	int 2handedwclass; //+C4 type9
	int TMogType; //+C8 type9
	int minac; //+CC type2
	int maxac; //+D0 type2
	int gamble_cost; //+D4 type2
	int speed; //+D8 type2
	int bitfield1; //+DC type2
	int cost; //+E0 type2
	int minstack; //+E4 type2
	int maxstack; //+E8 type2
	int spawnstack; //+EC type2
	int gemoffset; //+F0 type2
	short namestr; //+F4 type22 link=d2common_5E350
	short version; //+F6 type3
	short auto_prefix; //+F8 type3
	short missiletype; //+FA type3
	char rarity; //+FC type4
	char level; //+FD type4
	char mindam; //+FE type6
	char maxdam; //+FF type6
	char minmisdam; //+100 type4
	char maxmisdam; //+101 type4
	char 2handmindam; //+102 type6
	char 2handmaxdam; //+103 type6
	char rangeadder; //+104 type6
	char pad_105; //+105
	short strbonus; //+106 type3
	short dexbonus; //+108 type3
	short reqstr; //+10A type3
	short reqdex; //+10C type3
	char absorbs; //+10E type6
	char invwidth; //+10F type4
	char invheight; //+110 type4
	char block; //+111 type6
	char durability; //+112 type6
	char nodurability; //+113 type4
	char missile; //+114 type6
	char component; //+115 type4
	char rArm; //+116 type6
	char lArm; //+117 type6
	char torso; //+118 type6
	char legs; //+119 type6
	char rspad; //+11A type6
	char lspad; //+11B type6
	char 2handed; //+11C type6
	char useable; //+11D type4
	short type; //+11E type15 link=d2common_pItemTypesLink
	short type2; //+120 type15 link=d2common_pItemTypesLink
	char subtype; //+122 type6
	char pad_123; //+123
	short dropsound; //+124 type20 link=d2common_pSoundCodesLink
	short usesound; //+126 type20 link=d2common_pSoundCodesLink
	char dropsfxframe; //+128 type4
	char unique; //+129 type4
	char quest; //+12A type4
	char questdiffcheck; //+12B type4
	char transparent; //+12C type4
	char transtbl; //+12D type4
	char pad_12E; //+12E
	char lightradius; //+12F type4
	char belt; //+130 type4
	char autobelt; //+131 type4
	char stackable; //+132 type4
	char spawnable; //+133 type4
	char spellicon; //+134 type6
	char durwarning; //+135 type4
	char qntwarning; //+136 type4
	char hasinv; //+137 type6
	char gemsockets; //+138 type6
	char Transmogrify; //+139 type6
	char TMogMin; //+13A type6
	char TMogMax; //+13B type6
	char hit_class; //+13C type13 link=d2common_pHitClassLink
	char 1or2handed; //+13D type6
	char gemapplytype; //+13E type4
	char levelreq; //+13F type4
	char magic_lvl; //+140 type4
	char Transform; //+141 type4
	char InvTrans; //+142 type4
	char compactsave; //+143 type6
	char SkipName; //+144 type4
	char Nameable; //+145 type4
	char AkaraMin; //+146 type4
	char GheedMin; //+147 type4
	char CharsiMin; //+148 type4
	char FaraMin; //+149 type4
	char LysanderMin; //+14A type4
	char DrognanMin; //+14B type4
	char HraltiMin; //+14C type4
	char AlkorMin; //+14D type4
	char OrmusMin; //+14E type4
	char ElzixMin; //+14F type4
	char AshearaMin; //+150 type4
	char CainMin; //+151 type4
	char HalbuMin; //+152 type4
	char JamellaMin; //+153 type4
	char MalahMin; //+154 type4
	char LarzukMin; //+155 type4
	char DrehyaMin; //+156 type4
	char AkaraMax; //+157 type4
	char GheedMax; //+158 type4
	char CharsiMax; //+159 type4
	char FaraMax; //+15A type4
	char LysanderMax; //+15B type4
	char DrognanMax; //+15C type4
	char HraltiMax; //+15D type4
	char AlkorMax; //+15E type4
	char OrmusMax; //+15F type4
	char ElzixMax; //+160 type4
	char AshearaMax; //+161 type4
	char CainMax; //+162 type4
	char HalbuMax; //+163 type4
	char JamellaMax; //+164 type4
	char MalahMax; //+165 type4
	char LarzukMax; //+166 type4
	char DrehyaMax; //+167 type4
	char AkaraMagicMin; //+168 type4
	char GheedMagicMin; //+169 type4
	char CharsiMagicMin; //+16A type4
	char FaraMagicMin; //+16B type4
	char LysanderMagicMin; //+16C type4
	char DrognanMagicMin; //+16D type4
	char HraltiMagicMin; //+16E type4
	char AlkorMagicMin; //+16F type4
	char OrmusMagicMin; //+170 type4
	char ElzixMagicMin; //+171 type4
	char AshearaMagicMin; //+172 type4
	char CainMagicMin; //+173 type4
	char HalbuMagicMin; //+174 type4
	char JamellaMagicMin; //+175 type4
	char MalahMagicMin; //+176 type4
	char LarzukMagicMin; //+177 type4
	char DrehyaMagicMin; //+178 type4
	char AkaraMagicMax; //+179 type4
	char GheedMagicMax; //+17A type4
	char CharsiMagicMax; //+17B type4
	char FaraMagicMax; //+17C type4
	char LysanderMagicMax; //+17D type4
	char DrognanMagicMax; //+17E type4
	char HraltiMagicMax; //+17F type4
	char AlkorMagicMax; //+180 type4
	char OrmusMagicMax; //+181 type4
	char ElzixMagicMax; //+182 type4
	char AshearaMagicMax; //+183 type4
	char CainMagicMax; //+184 type4
	char HalbuMagicMax; //+185 type4
	char JamellaMagicMax; //+186 type4
	char MalahMagicMax; //+187 type4
	char LarzukMagicMax; //+188 type4
	char DrehyaMagicMax; //+189 type4
	char AkaraMagicLvl; //+18A type4
	char GheedMagicLvl; //+18B type4
	char CharsiMagicLvl; //+18C type4
	char FaraMagicLvl; //+18D type4
	char LysanderMagicLvl; //+18E type4
	char DrognanMagicLvl; //+18F type4
	char HraltiMagicLvl; //+190 type4
	char AlkorMagicLvl; //+191 type4
	char OrmusMagicLvl; //+192 type4
	char ElzixMagicLvl; //+193 type4
	char AshearaMagicLvl; //+194 type4
	char CainMagicLvl; //+195 type4
	char HalbuMagicLvl; //+196 type4
	char JamellaMagicLvl; //+197 type4
	char MalahMagicLvl; //+198 type4
	char LarzukMagicLvl; //+199 type4
	char DrehyaMagicLvl; //+19A type4
	char pad_19B; //+19B
	int NightmareUpgrade; //+19C type9
	int HellUpgrade; //+1A0 type9
	char PermStoreItem; //+1A4 type4
	char multibuy; //+1A5 type4
	char pad_1A6[2]; //+1A6
};
//misc size=0x1A8 count=d2common_9FBB0 ret=d2common_7A924
struct misc_txt {
	char flippyfile[31]; //+00 type1
	char pad_1F; //+1F
	char invfile[31]; //+20 type1
	char pad_3F; //+3F
	char uniqueinvfile[31]; //+40 type1
	char pad_5F; //+5F
	char setinvfile[31]; //+60 type1
	char pad_7F; //+7F
	int code; //+80 type10 link=d2common_pItemLink
	int normcode; //+84 type9
	int ubercode; //+88 type9
	int ultracode; //+8C type9
	int alternategfx; //+90 type9
	int pSpell; //+94 type2
	short state; //+98 type20 link=d2common_pStatesLink
	short cstate1; //+9A type20 link=d2common_pStatesLink
	short cstate2; //+9C type20 link=d2common_pStatesLink
	short stat1; //+9E type20 link=d2common_pItemStatCostLink
	short stat2; //+A0 type20 link=d2common_pItemStatCostLink
	short stat3; //+A2 type20 link=d2common_pItemStatCostLink
	int calc1; //+A4 type25 link=d2common_71BA0
	int calc2; //+A8 type25 link=d2common_71BA0
	int calc3; //+AC type25 link=d2common_71BA0
	int len; //+B0 type25 link=d2common_71BA0
	char spelldesc; //+B4 type4
	char pad_B5; //+B5
	short spelldescstr; //+B6 type22 link=d2common_5E350
	int spelldesccalc; //+B8 type25 link=d2common_71BA0
	int BetterGem; //+BC type9
	int wclass; //+C0 type9
	int 2handedwclass; //+C4 type9
	int TMogType; //+C8 type9
	int minac; //+CC type2
	int maxac; //+D0 type2
	int gamble_cost; //+D4 type2
	int speed; //+D8 type2
	int bitfield1; //+DC type2
	int cost; //+E0 type2
	int minstack; //+E4 type2
	int maxstack; //+E8 type2
	int spawnstack; //+EC type2
	int gemoffset; //+F0 type2
	short namestr; //+F4 type22 link=d2common_5E350
	short version; //+F6 type3
	short auto_prefix; //+F8 type3
	short missiletype; //+FA type3
	char rarity; //+FC type4
	char level; //+FD type4
	char mindam; //+FE type6
	char maxdam; //+FF type6
	char minmisdam; //+100 type4
	char maxmisdam; //+101 type4
	char 2handmindam; //+102 type6
	char 2handmaxdam; //+103 type6
	char rangeadder; //+104 type6
	char pad_105; //+105
	short strbonus; //+106 type3
	short dexbonus; //+108 type3
	short reqstr; //+10A type3
	short reqdex; //+10C type3
	char absorbs; //+10E type6
	char invwidth; //+10F type4
	char invheight; //+110 type4
	char block; //+111 type6
	char durability; //+112 type6
	char nodurability; //+113 type4
	char missile; //+114 type6
	char component; //+115 type4
	char rArm; //+116 type6
	char lArm; //+117 type6
	char torso; //+118 type6
	char legs; //+119 type6
	char rspad; //+11A type6
	char lspad; //+11B type6
	char 2handed; //+11C type6
	char useable; //+11D type4
	short type; //+11E type15 link=d2common_pItemTypesLink
	short type2; //+120 type15 link=d2common_pItemTypesLink
	char subtype; //+122 type6
	char pad_123; //+123
	short dropsound; //+124 type20 link=d2common_pSoundCodesLink
	short usesound; //+126 type20 link=d2common_pSoundCodesLink
	char dropsfxframe; //+128 type4
	char unique; //+129 type4
	char quest; //+12A type4
	char questdiffcheck; //+12B type4
	char transparent; //+12C type4
	char transtbl; //+12D type4
	char pad_12E; //+12E
	char lightradius; //+12F type4
	char belt; //+130 type4
	char autobelt; //+131 type4
	char stackable; //+132 type4
	char spawnable; //+133 type4
	char spellicon; //+134 type6
	char durwarning; //+135 type4
	char qntwarning; //+136 type4
	char hasinv; //+137 type6
	char gemsockets; //+138 type6
	char Transmogrify; //+139 type6
	char TMogMin; //+13A type6
	char TMogMax; //+13B type6
	char hit_class; //+13C type13 link=d2common_pHitClassLink
	char 1or2handed; //+13D type6
	char gemapplytype; //+13E type4
	char levelreq; //+13F type4
	char magic_lvl; //+140 type4
	char Transform; //+141 type4
	char InvTrans; //+142 type4
	char compactsave; //+143 type6
	char SkipName; //+144 type4
	char Nameable; //+145 type4
	char AkaraMin; //+146 type4
	char GheedMin; //+147 type4
	char CharsiMin; //+148 type4
	char FaraMin; //+149 type4
	char LysanderMin; //+14A type4
	char DrognanMin; //+14B type4
	char HraltiMin; //+14C type4
	char AlkorMin; //+14D type4
	char OrmusMin; //+14E type4
	char ElzixMin; //+14F type4
	char AshearaMin; //+150 type4
	char CainMin; //+151 type4
	char HalbuMin; //+152 type4
	char JamellaMin; //+153 type4
	char MalahMin; //+154 type4
	char LarzukMin; //+155 type4
	char DrehyaMin; //+156 type4
	char AkaraMax; //+157 type4
	char GheedMax; //+158 type4
	char CharsiMax; //+159 type4
	char FaraMax; //+15A type4
	char LysanderMax; //+15B type4
	char DrognanMax; //+15C type4
	char HraltiMax; //+15D type4
	char AlkorMax; //+15E type4
	char OrmusMax; //+15F type4
	char ElzixMax; //+160 type4
	char AshearaMax; //+161 type4
	char CainMax; //+162 type4
	char HalbuMax; //+163 type4
	char JamellaMax; //+164 type4
	char MalahMax; //+165 type4
	char LarzukMax; //+166 type4
	char DrehyaMax; //+167 type4
	char AkaraMagicMin; //+168 type4
	char GheedMagicMin; //+169 type4
	char CharsiMagicMin; //+16A type4
	char FaraMagicMin; //+16B type4
	char LysanderMagicMin; //+16C type4
	char DrognanMagicMin; //+16D type4
	char HraltiMagicMin; //+16E type4
	char AlkorMagicMin; //+16F type4
	char OrmusMagicMin; //+170 type4
	char ElzixMagicMin; //+171 type4
	char AshearaMagicMin; //+172 type4
	char CainMagicMin; //+173 type4
	char HalbuMagicMin; //+174 type4
	char JamellaMagicMin; //+175 type4
	char MalahMagicMin; //+176 type4
	char LarzukMagicMin; //+177 type4
	char DrehyaMagicMin; //+178 type4
	char AkaraMagicMax; //+179 type4
	char GheedMagicMax; //+17A type4
	char CharsiMagicMax; //+17B type4
	char FaraMagicMax; //+17C type4
	char LysanderMagicMax; //+17D type4
	char DrognanMagicMax; //+17E type4
	char HraltiMagicMax; //+17F type4
	char AlkorMagicMax; //+180 type4
	char OrmusMagicMax; //+181 type4
	char ElzixMagicMax; //+182 type4
	char AshearaMagicMax; //+183 type4
	char CainMagicMax; //+184 type4
	char HalbuMagicMax; //+185 type4
	char JamellaMagicMax; //+186 type4
	char MalahMagicMax; //+187 type4
	char LarzukMagicMax; //+188 type4
	char DrehyaMagicMax; //+189 type4
	char AkaraMagicLvl; //+18A type4
	char GheedMagicLvl; //+18B type4
	char CharsiMagicLvl; //+18C type4
	char FaraMagicLvl; //+18D type4
	char LysanderMagicLvl; //+18E type4
	char DrognanMagicLvl; //+18F type4
	char HraltiMagicLvl; //+190 type4
	char AlkorMagicLvl; //+191 type4
	char OrmusMagicLvl; //+192 type4
	char ElzixMagicLvl; //+193 type4
	char AshearaMagicLvl; //+194 type4
	char CainMagicLvl; //+195 type4
	char HalbuMagicLvl; //+196 type4
	char JamellaMagicLvl; //+197 type4
	char MalahMagicLvl; //+198 type4
	char LarzukMagicLvl; //+199 type4
	char DrehyaMagicLvl; //+19A type4
	char pad_19B; //+19B
	int NightmareUpgrade; //+19C type9
	int HellUpgrade; //+1A0 type9
	char PermStoreItem; //+1A4 type4
	char multibuy; //+1A5 type4
	char pad_1A6[2]; //+1A6
};
//magicsuffix size=0x90 count=12FA6C ret=d2common_78603
struct magicsuffix_txt {
	char name[31]; //+00 type1
	char pad_1F[3]; //+1F
	short version; //+22 type3
	int mod1code; //+24 type19 link=d2common_pPropertiesLink
	int mod1param; //+28 type25 link=d2common_718B0
	int mod1min; //+2C type2
	int mod1max; //+30 type2
	int mod2code; //+34 type19 link=d2common_pPropertiesLink
	int mod2param; //+38 type25 link=d2common_718B0
	int mod2min; //+3C type2
	int mod2max; //+40 type2
	int mod3code; //+44 type19 link=d2common_pPropertiesLink
	int mod3param; //+48 type25 link=d2common_718B0
	int mod3min; //+4C type2
	int mod3max; //+50 type2
	char spawnable; //+54 type4
	char pad_55; //+55
	char transformcolor; //+56 type13 link=d2common_pColorsLink
	char pad_57; //+57
	int level; //+58 type2
	int group; //+5C type2
	int maxlevel; //+60 type2
	char rare; //+64 type4
	char levelreq; //+65 type4
	char classspecific; //+66 type13 link=d2common_pPlayerClassLink
	char class; //+67 type13 link=d2common_pPlayerClassLink
	char classlevelreq; //+68 type4
	char pad_69; //+69
	short itype1; //+6A type15 link=d2common_pItemTypesLink
	short itype2; //+6C type15 link=d2common_pItemTypesLink
	short itype3; //+6E type15 link=d2common_pItemTypesLink
	short itype4; //+70 type15 link=d2common_pItemTypesLink
	short itype5; //+72 type15 link=d2common_pItemTypesLink
	short itype6; //+74 type15 link=d2common_pItemTypesLink
	short itype7; //+76 type15 link=d2common_pItemTypesLink
	short etype1; //+78 type15 link=d2common_pItemTypesLink
	short etype2; //+7A type15 link=d2common_pItemTypesLink
	short etype3; //+7C type15 link=d2common_pItemTypesLink
	short etype4; //+7E type15 link=d2common_pItemTypesLink
	short etype5; //+80 type15 link=d2common_pItemTypesLink
	char frequency; //+82 type4
	char pad_83; //+83
	int divide; //+84 type2
	int multiply; //+88 type2
	int add; //+8C type2
};
//magicprefix size=0x90 count=12FA60 ret=d2common_7861F
struct magicprefix_txt {
	char name[31]; //+00 type1
	char pad_1F[3]; //+1F
	short version; //+22 type3
	int mod1code; //+24 type19 link=d2common_pPropertiesLink
	int mod1param; //+28 type25 link=d2common_718B0
	int mod1min; //+2C type2
	int mod1max; //+30 type2
	int mod2code; //+34 type19 link=d2common_pPropertiesLink
	int mod2param; //+38 type25 link=d2common_718B0
	int mod2min; //+3C type2
	int mod2max; //+40 type2
	int mod3code; //+44 type19 link=d2common_pPropertiesLink
	int mod3param; //+48 type25 link=d2common_718B0
	int mod3min; //+4C type2
	int mod3max; //+50 type2
	char spawnable; //+54 type4
	char pad_55; //+55
	char transformcolor; //+56 type13 link=d2common_pColorsLink
	char pad_57; //+57
	int level; //+58 type2
	int group; //+5C type2
	int maxlevel; //+60 type2
	char rare; //+64 type4
	char levelreq; //+65 type4
	char classspecific; //+66 type13 link=d2common_pPlayerClassLink
	char class; //+67 type13 link=d2common_pPlayerClassLink
	char classlevelreq; //+68 type4
	char pad_69; //+69
	short itype1; //+6A type15 link=d2common_pItemTypesLink
	short itype2; //+6C type15 link=d2common_pItemTypesLink
	short itype3; //+6E type15 link=d2common_pItemTypesLink
	short itype4; //+70 type15 link=d2common_pItemTypesLink
	short itype5; //+72 type15 link=d2common_pItemTypesLink
	short itype6; //+74 type15 link=d2common_pItemTypesLink
	short itype7; //+76 type15 link=d2common_pItemTypesLink
	short etype1; //+78 type15 link=d2common_pItemTypesLink
	short etype2; //+7A type15 link=d2common_pItemTypesLink
	short etype3; //+7C type15 link=d2common_pItemTypesLink
	short etype4; //+7E type15 link=d2common_pItemTypesLink
	short etype5; //+80 type15 link=d2common_pItemTypesLink
	char frequency; //+82 type4
	char pad_83; //+83
	int divide; //+84 type2
	int multiply; //+88 type2
	int add; //+8C type2
};
//automagic size=0x90 count=12FA68 ret=d2common_7863D
struct automagic_txt {
	char name[31]; //+00 type1
	char pad_1F[3]; //+1F
	short version; //+22 type3
	int mod1code; //+24 type19 link=d2common_pPropertiesLink
	int mod1param; //+28 type25 link=d2common_718B0
	int mod1min; //+2C type2
	int mod1max; //+30 type2
	int mod2code; //+34 type19 link=d2common_pPropertiesLink
	int mod2param; //+38 type25 link=d2common_718B0
	int mod2min; //+3C type2
	int mod2max; //+40 type2
	int mod3code; //+44 type19 link=d2common_pPropertiesLink
	int mod3param; //+48 type25 link=d2common_718B0
	int mod3min; //+4C type2
	int mod3max; //+50 type2
	char spawnable; //+54 type4
	char pad_55; //+55
	char transformcolor; //+56 type13 link=d2common_pColorsLink
	char pad_57; //+57
	int level; //+58 type2
	int group; //+5C type2
	int maxlevel; //+60 type2
	char rare; //+64 type4
	char levelreq; //+65 type4
	char classspecific; //+66 type13 link=d2common_pPlayerClassLink
	char class; //+67 type13 link=d2common_pPlayerClassLink
	char classlevelreq; //+68 type4
	char pad_69; //+69
	short itype1; //+6A type15 link=d2common_pItemTypesLink
	short itype2; //+6C type15 link=d2common_pItemTypesLink
	short itype3; //+6E type15 link=d2common_pItemTypesLink
	short itype4; //+70 type15 link=d2common_pItemTypesLink
	short itype5; //+72 type15 link=d2common_pItemTypesLink
	short itype6; //+74 type15 link=d2common_pItemTypesLink
	short itype7; //+76 type15 link=d2common_pItemTypesLink
	short etype1; //+78 type15 link=d2common_pItemTypesLink
	short etype2; //+7A type15 link=d2common_pItemTypesLink
	short etype3; //+7C type15 link=d2common_pItemTypesLink
	short etype4; //+7E type15 link=d2common_pItemTypesLink
	short etype5; //+80 type15 link=d2common_pItemTypesLink
	char frequency; //+82 type4
	char pad_83; //+83
	int divide; //+84 type2
	int multiply; //+88 type2
	int add; //+8C type2
};
//raresuffix size=0x48 count=12FC88 ret=d2common_77DBC
struct raresuffix_txt {
	char pad_0[14]; //+0
	short version; //+0E type3
	short itype1; //+10 type15 link=d2common_pItemTypesLink
	short itype2; //+12 type15 link=d2common_pItemTypesLink
	short itype3; //+14 type15 link=d2common_pItemTypesLink
	short itype4; //+16 type15 link=d2common_pItemTypesLink
	short itype5; //+18 type15 link=d2common_pItemTypesLink
	short itype6; //+1A type15 link=d2common_pItemTypesLink
	short itype7; //+1C type15 link=d2common_pItemTypesLink
	short etype1; //+1E type15 link=d2common_pItemTypesLink
	short etype2; //+20 type15 link=d2common_pItemTypesLink
	short etype3; //+22 type15 link=d2common_pItemTypesLink
	short etype4; //+24 type15 link=d2common_pItemTypesLink
	char name[31]; //+26 type1
	char pad_45[3]; //+45
};
//rareprefix size=0x48 count=12FC84 ret=d2common_77DD5
struct rareprefix_txt {
	char pad_0[14]; //+0
	short version; //+0E type3
	short itype1; //+10 type15 link=d2common_pItemTypesLink
	short itype2; //+12 type15 link=d2common_pItemTypesLink
	short itype3; //+14 type15 link=d2common_pItemTypesLink
	short itype4; //+16 type15 link=d2common_pItemTypesLink
	short itype5; //+18 type15 link=d2common_pItemTypesLink
	short itype6; //+1A type15 link=d2common_pItemTypesLink
	short itype7; //+1C type15 link=d2common_pItemTypesLink
	short etype1; //+1E type15 link=d2common_pItemTypesLink
	short etype2; //+20 type15 link=d2common_pItemTypesLink
	short etype3; //+22 type15 link=d2common_pItemTypesLink
	short etype4; //+24 type15 link=d2common_pItemTypesLink
	char name[31]; //+26 type1
	char pad_45[3]; //+45
};
//uniqueitems size=0x14C count=d2common_nUniqueItems ret=d2common_77AC5
struct uniqueitems_txt {
	char pad_0[2]; //+0
	char index[31]; //+02 type1
	char pad_21[3]; //+21
	short version; //+24 type3
	char pad_26[2]; //+26
	int code; //+28 type9
	//int enabled:1; //+2C type26 bit=0
	//int nolimit:1; //+2C type26 bit=1
	//int carry1:1; //+2C type26 bit=2
	//int ladder:1; //+2C type26 bit=3
	char pad_2C[4]; //+2C
	short rarity; //+30 type3
	char pad_32[2]; //+32
	short lvl; //+34 type3
	short lvl_req; //+36 type3
	char chrtransform; //+38 type13 link=d2common_pColorsLink
	char invtransform; //+39 type13 link=d2common_pColorsLink
	char flippyfile[31]; //+3A type1
	char pad_59; //+59
	char invfile[31]; //+5A type1
	char pad_79[3]; //+79
	int cost_mult; //+7C type2
	int cost_add; //+80 type2
	short dropsound; //+84 type20 link=d2common_pSoundCodesLink
	short usesound; //+86 type20 link=d2common_pSoundCodesLink
	char dropsfxframe; //+88 type4
	char pad_89[3]; //+89
	int prop1; //+8C type19 link=d2common_pPropertiesLink
	int par1; //+90 type25 link=d2common_718B0
	int min1; //+94 type2
	int max1; //+98 type2
	int prop2; //+9C type19 link=d2common_pPropertiesLink
	int par2; //+A0 type25 link=d2common_718B0
	int min2; //+A4 type2
	int max2; //+A8 type2
	int prop3; //+AC type19 link=d2common_pPropertiesLink
	int par3; //+B0 type25 link=d2common_718B0
	int min3; //+B4 type2
	int max3; //+B8 type2
	int prop4; //+BC type19 link=d2common_pPropertiesLink
	int par4; //+C0 type25 link=d2common_718B0
	int min4; //+C4 type2
	int max4; //+C8 type2
	int prop5; //+CC type19 link=d2common_pPropertiesLink
	int par5; //+D0 type25 link=d2common_718B0
	int min5; //+D4 type2
	int max5; //+D8 type2
	int prop6; //+DC type19 link=d2common_pPropertiesLink
	int par6; //+E0 type25 link=d2common_718B0
	int min6; //+E4 type2
	int max6; //+E8 type2
	int prop7; //+EC type19 link=d2common_pPropertiesLink
	int par7; //+F0 type25 link=d2common_718B0
	int min7; //+F4 type2
	int max7; //+F8 type2
	int prop8; //+FC type19 link=d2common_pPropertiesLink
	int par8; //+100 type25 link=d2common_718B0
	int min8; //+104 type2
	int max8; //+108 type2
	int prop9; //+10C type19 link=d2common_pPropertiesLink
	int par9; //+110 type25 link=d2common_718B0
	int min9; //+114 type2
	int max9; //+118 type2
	int prop10; //+11C type19 link=d2common_pPropertiesLink
	int par10; //+120 type25 link=d2common_718B0
	int min10; //+124 type2
	int max10; //+128 type2
	int prop11; //+12C type19 link=d2common_pPropertiesLink
	int par11; //+130 type25 link=d2common_718B0
	int min11; //+134 type2
	int max11; //+138 type2
	int prop12; //+13C type19 link=d2common_pPropertiesLink
	int par12; //+140 type25 link=d2common_718B0
	int min12; //+144 type2
	int max12; //+148 type2
};
//sets size=0x128 count=d2common_nSets ret=d2common_75E10
struct sets_txt {
	short index; //+00 type17 link=d2common_pSetsLink
	short name; //+02 type22 link=d2common_5E350
	short version; //+04 type3
	char pad_6[10]; //+6
	int pcode2a; //+10 type19 link=d2common_pPropertiesLink
	int pparam2a; //+14 type25 link=d2common_718B0
	int pmin2a; //+18 type2
	int pmax2a; //+1C type2
	int pcode2b; //+20 type19 link=d2common_pPropertiesLink
	int pparam2b; //+24 type25 link=d2common_718B0
	int pmin2b; //+28 type2
	int pmax2b; //+2C type2
	int pcode3a; //+30 type19 link=d2common_pPropertiesLink
	int pparam3a; //+34 type25 link=d2common_718B0
	int pmin3a; //+38 type2
	int pmax3a; //+3C type2
	int pcode3b; //+40 type19 link=d2common_pPropertiesLink
	int pparam3b; //+44 type25 link=d2common_718B0
	int pmin3b; //+48 type2
	int pmax3b; //+4C type2
	int pcode4a; //+50 type19 link=d2common_pPropertiesLink
	int pparam4a; //+54 type25 link=d2common_718B0
	int pmin4a; //+58 type2
	int pmax4a; //+5C type2
	int pcode4b; //+60 type19 link=d2common_pPropertiesLink
	int pparam4b; //+64 type25 link=d2common_718B0
	int pmin4b; //+68 type2
	int pmax4b; //+6C type2
	int pcode5a; //+70 type19 link=d2common_pPropertiesLink
	int pparam5a; //+74 type25 link=d2common_718B0
	int pmin5a; //+78 type2
	int pmax5a; //+7C type2
	int pcode5b; //+80 type19 link=d2common_pPropertiesLink
	int pparam5b; //+84 type25 link=d2common_718B0
	int pmin5b; //+88 type2
	int pmax5b; //+8C type2
	int fcode1; //+90 type19 link=d2common_pPropertiesLink
	int fparam1; //+94 type25 link=d2common_718B0
	int fmin1; //+98 type2
	int fmax1; //+9C type2
	int fcode2; //+A0 type19 link=d2common_pPropertiesLink
	int fparam2; //+A4 type25 link=d2common_718B0
	int fmin2; //+A8 type2
	int fmax2; //+AC type2
	int fcode3; //+B0 type19 link=d2common_pPropertiesLink
	int fparam3; //+B4 type25 link=d2common_718B0
	int fmin3; //+B8 type2
	int fmax3; //+BC type2
	int fcode4; //+C0 type19 link=d2common_pPropertiesLink
	int fparam4; //+C4 type25 link=d2common_718B0
	int fmin4; //+C8 type2
	int fmax4; //+CC type2
	int fcode5; //+D0 type19 link=d2common_pPropertiesLink
	int fparam5; //+D4 type25 link=d2common_718B0
	int fmin5; //+D8 type2
	int fmax5; //+DC type2
	int fcode6; //+E0 type19 link=d2common_pPropertiesLink
	int fparam6; //+E4 type25 link=d2common_718B0
	int fmin6; //+E8 type2
	int fmax6; //+EC type2
	int fcode7; //+F0 type19 link=d2common_pPropertiesLink
	int fparam7; //+F4 type25 link=d2common_718B0
	int fmin7; //+F8 type2
	int fmax7; //+FC type2
	int fcode8; //+100 type19 link=d2common_pPropertiesLink
	int fparam8; //+104 type25 link=d2common_718B0
	int fmin8; //+108 type2
	int fmax8; //+10C type2
	char pad_110[24]; //+110
};
//setitems size=0x1B8 count=d2common_nSetItems ret=d2common_76E3B
struct setitems_txt {
	char pad_0[2]; //+0
	char index[31]; //+02 type1
	char pad_21[7]; //+21
	int item; //+28 type9
	short set; //+2C type20 link=d2common_pSetsLink
	char pad_2E[2]; //+2E
	short lvl; //+30 type3
	short lvl_req; //+32 type3
	int rarity; //+34 type2
	int cost_mult; //+38 type2
	int cost_add; //+3C type2
	char chrtransform; //+40 type13 link=d2common_pColorsLink
	char invtransform; //+41 type13 link=d2common_pColorsLink
	char flippyfile[31]; //+42 type1
	char pad_61; //+61
	char invfile[31]; //+62 type1
	char pad_81; //+81
	short dropsound; //+82 type20 link=d2common_pSoundCodesLink
	short usesound; //+84 type20 link=d2common_pSoundCodesLink
	char dropsfxframe; //+86 type4
	char add_func; //+87 type4
	int prop1; //+88 type19 link=d2common_pPropertiesLink
	int par1; //+8C type25 link=d2common_718B0
	int min1; //+90 type2
	int max1; //+94 type2
	int prop2; //+98 type19 link=d2common_pPropertiesLink
	int par2; //+9C type25 link=d2common_718B0
	int min2; //+A0 type2
	int max2; //+A4 type2
	int prop3; //+A8 type19 link=d2common_pPropertiesLink
	int par3; //+AC type25 link=d2common_718B0
	int min3; //+B0 type2
	int max3; //+B4 type2
	int prop4; //+B8 type19 link=d2common_pPropertiesLink
	int par4; //+BC type25 link=d2common_718B0
	int min4; //+C0 type2
	int max4; //+C4 type2
	int prop5; //+C8 type19 link=d2common_pPropertiesLink
	int par5; //+CC type25 link=d2common_718B0
	int min5; //+D0 type2
	int max5; //+D4 type2
	int prop6; //+D8 type19 link=d2common_pPropertiesLink
	int par6; //+DC type25 link=d2common_718B0
	int min6; //+E0 type2
	int max6; //+E4 type2
	int prop7; //+E8 type19 link=d2common_pPropertiesLink
	int par7; //+EC type25 link=d2common_718B0
	int min7; //+F0 type2
	int max7; //+F4 type2
	int prop8; //+F8 type19 link=d2common_pPropertiesLink
	int par8; //+FC type25 link=d2common_718B0
	int min8; //+100 type2
	int max8; //+104 type2
	int prop9; //+108 type19 link=d2common_pPropertiesLink
	int par9; //+10C type25 link=d2common_718B0
	int min9; //+110 type2
	int max9; //+114 type2
	int aprop1a; //+118 type19 link=d2common_pPropertiesLink
	int apar1a; //+11C type25 link=d2common_718B0
	int amin1a; //+120 type2
	int amax1a; //+124 type2
	int aprop1b; //+128 type19 link=d2common_pPropertiesLink
	int apar1b; //+12C type25 link=d2common_718B0
	int amin1b; //+130 type2
	int amax1b; //+134 type2
	int aprop2a; //+138 type19 link=d2common_pPropertiesLink
	int apar2a; //+13C type25 link=d2common_718B0
	int amin2a; //+140 type2
	int amax2a; //+144 type2
	int aprop2b; //+148 type19 link=d2common_pPropertiesLink
	int apar2b; //+14C type25 link=d2common_718B0
	int amin2b; //+150 type2
	int amax2b; //+154 type2
	int aprop3a; //+158 type19 link=d2common_pPropertiesLink
	int apar3a; //+15C type25 link=d2common_718B0
	int amin3a; //+160 type2
	int amax3a; //+164 type2
	int aprop3b; //+168 type19 link=d2common_pPropertiesLink
	int apar3b; //+16C type25 link=d2common_718B0
	int amin3b; //+170 type2
	int amax3b; //+174 type2
	int aprop4a; //+178 type19 link=d2common_pPropertiesLink
	int apar4a; //+17C type25 link=d2common_718B0
	int amin4a; //+180 type2
	int amax4a; //+184 type2
	int aprop4b; //+188 type19 link=d2common_pPropertiesLink
	int apar4b; //+18C type25 link=d2common_718B0
	int amin4b; //+190 type2
	int amax4b; //+194 type2
	int aprop5a; //+198 type19 link=d2common_pPropertiesLink
	int apar5a; //+19C type25 link=d2common_718B0
	int amin5a; //+1A0 type2
	int amax5a; //+1A4 type2
	int aprop5b; //+1A8 type19 link=d2common_pPropertiesLink
	int apar5b; //+1AC type25 link=d2common_718B0
	int amin5b; //+1B0 type2
	int amax5b; //+1B4 type2
};
//gems size=0xC0 count=d2common_9FBCC ret=d2common_74D78
struct gems_txt {
	char name[31]; //+00 type1
	char pad_1F; //+1F
	char letter[5]; //+20 type1
	char pad_25[3]; //+25
	int code; //+28 type11 link=d2common_pItemLink
	char pad_2C[2]; //+2C
	char nummods; //+2E type4
	char transform; //+2F type4
	int weaponmod1code; //+30 type19 link=d2common_pPropertiesLink
	int weaponmod1param; //+34 type25 link=d2common_718B0
	int weaponmod1min; //+38 type2
	int weaponmod1max; //+3C type2
	int weaponmod2code; //+40 type19 link=d2common_pPropertiesLink
	int weaponmod2param; //+44 type25 link=d2common_718B0
	int weaponmod2min; //+48 type2
	int weaponmod2max; //+4C type2
	int weaponmod3code; //+50 type19 link=d2common_pPropertiesLink
	int weaponmod3param; //+54 type25 link=d2common_718B0
	int weaponmod3min; //+58 type2
	int weaponmod3max; //+5C type2
	int helmmod1code; //+60 type19 link=d2common_pPropertiesLink
	int helmmod1param; //+64 type25 link=d2common_718B0
	int helmmod1min; //+68 type2
	int helmmod1max; //+6C type2
	int helmmod2code; //+70 type19 link=d2common_pPropertiesLink
	int helmmod2param; //+74 type25 link=d2common_718B0
	int helmmod2min; //+78 type2
	int helmmod2max; //+7C type2
	int helmmod3code; //+80 type19 link=d2common_pPropertiesLink
	int helmmod3param; //+84 type25 link=d2common_718B0
	int helmmod3min; //+88 type2
	int helmmod3max; //+8C type2
	int shieldmod1code; //+90 type19 link=d2common_pPropertiesLink
	int shieldmod1param; //+94 type25 link=d2common_718B0
	int shieldmod1min; //+98 type2
	int shieldmod1max; //+9C type2
	int shieldmod2code; //+A0 type19 link=d2common_pPropertiesLink
	int shieldmod2param; //+A4 type25 link=d2common_718B0
	int shieldmod2min; //+A8 type2
	int shieldmod2max; //+AC type2
	int shieldmod3code; //+B0 type19 link=d2common_pPropertiesLink
	int shieldmod3param; //+B4 type25 link=d2common_718B0
	int shieldmod3min; //+B8 type2
	int shieldmod3max; //+BC type2
};
//books size=0x20 count=d2common_9FDA0 ret=d2common_74647
struct books_txt {
	short name; //+00 type22 link=d2common_5E350
	char spellicon; //+02 type4
	char pad_3; //+3
	int pSpell; //+04 type2
	int scrollskill; //+08 type19 link=d2common_pSkillsLink
	int bookskill; //+0C type19 link=d2common_pSkillsLink
	int basecost; //+10 type2
	int costpercharge; //+14 type2
	int scrollspellcode; //+18 type9
	int bookspellcode; //+1C type9
};
//qualityitems size=0x70 count=d2common_9FD90 ret=d2common_75201
struct qualityitems_txt {
	char armor; //+00 type4
	char weapon; //+01 type4
	char shield; //+02 type4
	char scepter; //+03 type4
	char wand; //+04 type4
	char staff; //+05 type4
	char bow; //+06 type4
	char boots; //+07 type4
	char gloves; //+08 type4
	char belt; //+09 type4
	char nummods; //+0A type4
	char pad_B; //+B
	int mod1code; //+0C type19 link=d2common_pPropertiesLink
	int mod1param; //+10 type2
	int mod1min; //+14 type2
	int mod1max; //+18 type2
	int mod2code; //+1C type19 link=d2common_pPropertiesLink
	int mod2param; //+20 type2
	int mod2min; //+24 type2
	int mod2max; //+28 type2
	char effect1[31]; //+2C type1
	char pad_4B; //+4B
	char effect2[31]; //+4C type1
	char pad_6B[5]; //+6B
};
//lowqualityitems size=0x22 count=d2common_9FD98 ret=d2common_74456
struct lowqualityitems_txt {
	char Name[31]; //+00 type1
	char pad_1F[3]; //+1F
};
//runes size=0x120 count=d2common_RuneWords ret=53C0A40
struct runes_txt {
	char name[63]; //+00 type1
	char pad_3F; //+3F
	char rune_name[63]; //+40 type1
	char pad_7F; //+7F
	char complete; //+80 type4
	char server; //+81 type4
	char pad_82[4]; //+82
	short itype1; //+86 type15 link=d2common_pItemTypesLink
	short itype2; //+88 type15 link=d2common_pItemTypesLink
	short itype3; //+8A type15 link=d2common_pItemTypesLink
	short itype4; //+8C type15 link=d2common_pItemTypesLink
	short itype5; //+8E type15 link=d2common_pItemTypesLink
	short itype6; //+90 type15 link=d2common_pItemTypesLink
	short etype1; //+92 type15 link=d2common_pItemTypesLink
	short etype2; //+94 type15 link=d2common_pItemTypesLink
	short etype3; //+96 type15 link=d2common_pItemTypesLink
	int rune1; //+98 type11 link=d2common_pItemLink
	int rune2; //+9C type11 link=d2common_pItemLink
	int rune3; //+A0 type11 link=d2common_pItemLink
	int rune4; //+A4 type11 link=d2common_pItemLink
	int rune5; //+A8 type11 link=d2common_pItemLink
	int rune6; //+AC type11 link=d2common_pItemLink
	int t1code1; //+B0 type19 link=d2common_pPropertiesLink
	int t1param1; //+B4 type25 link=d2common_718B0
	int t1min1; //+B8 type2
	int t1max1; //+BC type2
	int t1code2; //+C0 type19 link=d2common_pPropertiesLink
	int t1param2; //+C4 type25 link=d2common_718B0
	int t1min2; //+C8 type2
	int t1max2; //+CC type2
	int t1code3; //+D0 type19 link=d2common_pPropertiesLink
	int t1param3; //+D4 type25 link=d2common_718B0
	int t1min3; //+D8 type2
	int t1max3; //+DC type2
	int t1code4; //+E0 type19 link=d2common_pPropertiesLink
	int t1param4; //+E4 type25 link=d2common_718B0
	int t1min4; //+E8 type2
	int t1max4; //+EC type2
	int t1code5; //+F0 type19 link=d2common_pPropertiesLink
	int t1param5; //+F4 type25 link=d2common_718B0
	int t1min5; //+F8 type2
	int t1max5; //+FC type2
	int t1code6; //+100 type19 link=d2common_pPropertiesLink
	int t1param6; //+104 type25 link=d2common_718B0
	int t1min6; //+108 type2
	int t1max6; //+10C type2
	int t1code7; //+110 type19 link=d2common_pPropertiesLink
	int t1param7; //+114 type25 link=d2common_718B0
	int t1min7; //+118 type2
	int t1max7; //+11C type2
};
//itemratio size=0x44 count=d2common_9FD88 ret=d2common_743E3
struct itemratio_txt {
	int Unique; //+00 type2
	int UniqueDivisor; //+04 type2
	int UniqueMin; //+08 type2
	int Rare; //+0C type2
	int RareDivisor; //+10 type2
	int RareMin; //+14 type2
	int Set; //+18 type2
	int SetDivisor; //+1C type2
	int SetMin; //+20 type2
	int Magic; //+24 type2
	int MagicDivisor; //+28 type2
	int MagicMin; //+2C type2
	int HiQuality; //+30 type2
	int HiQualityDivisor; //+34 type2
	int Normal; //+38 type2
	int NormalDivisor; //+3C type2
	short Version; //+40 type3
	char Uber; //+42 type4
	char Class_Specific; //+43 type4
};
//gamble size=0xC count=d2common_9FBF0 ret=d2common_72CA4
struct gamble_txt {
	int code; //+00 type9
	char pad_4[8]; //+4
};
//plrtype size=0x34 count=12FD64 ret=d2common_196CA
struct plrtype_txt {
	char name[31]; //+00 type1
	char pad_1F; //+1F
	int token; //+20 type9
	char pad_24[16]; //+24
};
//plrmode size=0x34 count=12FD60 ret=d2common_196E3
struct plrmode_txt {
	char name[31]; //+00 type1
	char pad_1F; //+1F
	int token; //+20 type9
	char pad_24[16]; //+24
};
//monmode size=0x34 count=12FC28 ret=d2common_1962C
struct monmode_txt {
	char name[31]; //+00 type1
	char pad_1F; //+1F
	int token; //+20 type9
	char DT_Dir; //+24 type6
	char NU_Dir; //+25 type6
	char WL_Dir; //+26 type6
	char GH_Dir; //+27 type6
	char A1_Dir; //+28 type6
	char A2_Dir; //+29 type6
	char BL_Dir; //+2A type6
	char SC_Dir; //+2B type6
	char S1_Dir; //+2C type6
	char S2_Dir; //+2D type6
	char S3_Dir; //+2E type6
	char S4_Dir; //+2F type6
	char DD_Dir; //+30 type6
	char KB_Dir; //+31 type6
	char SQ_Dir; //+32 type6
	char RN_Dir; //+33 type6
};
//objtype size=0x34 count=12FD64 ret=d2common_1920A
struct objtype_txt {
	char name[31]; //+00 type1
	char pad_1F; //+1F
	int token; //+20 type9
	char pad_24[16]; //+24
};
//objmode size=0x34 count=12FD60 ret=d2common_19223
struct objmode_txt {
	char name[31]; //+00 type1
	char pad_1F; //+1F
	int token; //+20 type9
	char pad_24[16]; //+24
};
//composit size=0x34 count=0 ret=d2common_19180
struct composit_txt {
	char name[31]; //+00 type1
	char pad_1F; //+1F
	int token; //+20 type9
	char pad_24[16]; //+24
};
//armtype size=0x34 count=0 ret=d2common_19100
struct armtype_txt {
	char name[31]; //+00 type1
	char pad_1F; //+1F
	int token; //+20 type9
	char pad_24[16]; //+24
};
//experience size=0x20 count=0 ret=d2common_60A6B
struct experience_txt {
	int Amazon; //+00 type2
	int Sorceress; //+04 type2
	int Necromancer; //+08 type2
	int Paladin; //+0C type2
	int Barbarian; //+10 type2
	int Druid; //+14 type2
	int Assassin; //+18 type2
	int ExpRatio; //+1C type2
};
//uniquetitle size=0x2 count=d2common_A0B74 ret=d2common_5D5C9
struct uniquetitle_txt {
	short Name; //+00 type22 link=d2common_5E350
};
//uniqueprefix size=0x2 count=d2common_A0B78 ret=d2common_5D615
struct uniqueprefix_txt {
	short Name; //+00 type22 link=d2common_5E350
};
//uniquesuffix size=0x2 count=d2common_A0B7C ret=d2common_5D661
struct uniquesuffix_txt {
	short Name; //+00 type22 link=d2common_5E350
};
//uniqueappellation size=0x2 count=d2common_A0B80 ret=d2common_5D6AD
struct uniqueappellation_txt {
	short Name; //+00 type22 link=d2common_5E350
};
//monlvl size=0x78 count=d2common_nMonLvl ret=d2common_55A77
struct monlvl_txt {
	int AC; //+00 type2
	int AC(N); //+04 type2
	int AC(H); //+08 type2
	int L-AC; //+0C type2
	int L-AC(N); //+10 type2
	int L-AC(H); //+14 type2
	int TH; //+18 type2
	int TH(N); //+1C type2
	int TH(H); //+20 type2
	int L-TH; //+24 type2
	int L-TH(N); //+28 type2
	int L-TH(H); //+2C type2
	int HP; //+30 type2
	int HP(N); //+34 type2
	int HP(H); //+38 type2
	int L-HP; //+3C type2
	int L-HP(N); //+40 type2
	int L-HP(H); //+44 type2
	int DM; //+48 type2
	int DM(N); //+4C type2
	int DM(H); //+50 type2
	int L-DM; //+54 type2
	int L-DM(N); //+58 type2
	int L-DM(H); //+5C type2
	int XP; //+60 type2
	int XP(N); //+64 type2
	int XP(H); //+68 type2
	int L-XP; //+6C type2
	int L-XP(N); //+70 type2
	int L-XP(H); //+74 type2
};
//treasureclassex size=0x2E0 count=12FB04 ret=d2common_5A52A
struct treasureclassex_txt {
	char treasure_class[32]; //+00 type1
	int picks; //+20 type2
	short group; //+24 type3
	short level; //+26 type3
	short magic; //+28 type3
	short rare; //+2A type3
	short set; //+2C type3
	short unique; //+2E type3
	char pad_30[4]; //+30
	int nodrop; //+34 type2
	char item1[64]; //+38 type1
	char item2[64]; //+78 type1
	char item3[64]; //+B8 type1
	char item4[64]; //+F8 type1
	char item5[64]; //+138 type1
	char item6[64]; //+178 type1
	char item7[64]; //+1B8 type1
	char item8[64]; //+1F8 type1
	char item9[64]; //+238 type1
	char item10[64]; //+278 type1
	int prob1; //+2B8 type2
	int prob2; //+2BC type2
	int prob3; //+2C0 type2
	int prob4; //+2C4 type2
	int prob5; //+2C8 type2
	int prob6; //+2CC type2
	int prob7; //+2D0 type2
	int prob8; //+2D4 type2
	int prob9; //+2D8 type2
	int prob10; //+2DC type2
};
//monstats2 size=0x134 count=d2common_nMonStats2 ret=d2common_58100
struct monstats2_txt {
	short Id; //+00 type17 link=d2common_pMonStats2Link
	//OVERLAP //type23 HDv; //+00 type23 link=d2common_53C80
	//OVERLAP //type23 TRv; //+01 type23 link=d2common_53C80
	//type23 LGv; //+02 type23 link=d2common_53C80
	char pad_2; //+2
	//type23 Rav; //+03 type23 link=d2common_53C80
	char pad_3; //+3
	//type23 Lav; //+04 type23 link=d2common_53C80
	//int noGfxHitTest:1; //+04 type26 bit=0
	//int noMap:1; //+04 type26 bit=1
	//int noOvly:1; //+04 type26 bit=2
	//int isSel:1; //+04 type26 bit=3
	//int alSel:1; //+04 type26 bit=4
	//int noSel:1; //+04 type26 bit=5
	//int shiftSel:1; //+04 type26 bit=6
	//int corpseSel:1; //+04 type26 bit=7
	//int revive:1; //+04 type26 bit=8
	//int isAtt:1; //+04 type26 bit=9
	//int small:1; //+04 type26 bit=10
	//int large:1; //+04 type26 bit=11
	//int soft:1; //+04 type26 bit=12
	//int critter:1; //+04 type26 bit=13
	//int shadow:1; //+04 type26 bit=14
	//int noUniqueShift:1; //+04 type26 bit=15
	//int compositeDeath:1; //+04 type26 bit=16
	//int inert:1; //+04 type26 bit=17
	//int objCol:1; //+04 type26 bit=18
	//int deadCol:1; //+04 type26 bit=19
	//int unflatDead:1; //+04 type26 bit=20
	char pad_4; //+4
	//type23 RHv; //+05 type23 link=d2common_53C80
	char pad_5; //+5
	//type23 LHv; //+06 type23 link=d2common_53C80
	char pad_6; //+6
	//type23 SHv; //+07 type23 link=d2common_53C80
	char pad_7; //+7
	char SizeX; //+08 type4
	//OVERLAP //type23 S1v; //+08 type23 link=d2common_53C80
	char SizeY; //+09 type4
	//OVERLAP //type23 S2v; //+09 type23 link=d2common_53C80
	char spawnCol; //+0A type4
	//OVERLAP //type23 S3v; //+0A type23 link=d2common_53C80
	char Height; //+0B type4
	//OVERLAP //type23 S4v; //+0B type23 link=d2common_53C80
	char overlayHeight; //+0C type4
	//OVERLAP //type23 S5v; //+0C type23 link=d2common_53C80
	char pixHeight; //+0D type4
	//OVERLAP //type23 S6v; //+0D type23 link=d2common_53C80
	char MeleeRng; //+0E type4
	//OVERLAP //type23 S7v; //+0E type23 link=d2common_53C80
	//type23 S8v; //+0F type23 link=d2common_53C80
	char pad_F; //+F
	int BaseW; //+10 type9
	char HitClass; //+14 type4
	char pad_15[211]; //+15
	//int HD:1; //+E8 type26 bit=0
	//int TR:1; //+E8 type26 bit=1
	//int LG:1; //+E8 type26 bit=2
	//int RA:1; //+E8 type26 bit=3
	//int LA:1; //+E8 type26 bit=4
	//int RH:1; //+E8 type26 bit=5
	//int LH:1; //+E8 type26 bit=6
	//int SH:1; //+E8 type26 bit=7
	//int S1:1; //+E8 type26 bit=8
	//int S2:1; //+E8 type26 bit=9
	//int S3:1; //+E8 type26 bit=10
	//int S4:1; //+E8 type26 bit=11
	//int S5:1; //+E8 type26 bit=12
	//int S6:1; //+E8 type26 bit=13
	//int S7:1; //+E8 type26 bit=14
	//int S8:1; //+E8 type26 bit=15
	char pad_E8[4]; //+E8
	char TotalPieces; //+EC type4
	char pad_ED[3]; //+ED
	//int mDT:1; //+F0 type26 bit=0
	//int mNU:1; //+F0 type26 bit=1
	//int mWL:1; //+F0 type26 bit=2
	//int mGH:1; //+F0 type26 bit=3
	//int mA1:1; //+F0 type26 bit=4
	//int mA2:1; //+F0 type26 bit=5
	//int mBL:1; //+F0 type26 bit=6
	//int mSC:1; //+F0 type26 bit=7
	//int mS1:1; //+F0 type26 bit=8
	//int mS2:1; //+F0 type26 bit=9
	//int mS3:1; //+F0 type26 bit=10
	//int mS4:1; //+F0 type26 bit=11
	//int mDD:1; //+F0 type26 bit=12
	//int mKB:1; //+F0 type26 bit=13
	//int mSQ:1; //+F0 type26 bit=14
	//int mRN:1; //+F0 type26 bit=15
	char pad_F0[4]; //+F0
	char dDT; //+F4 type4
	char dNU; //+F5 type4
	char dWL; //+F6 type4
	char dGH; //+F7 type4
	char dA1; //+F8 type4
	char dA2; //+F9 type4
	char dBL; //+FA type4
	char dSC; //+FB type4
	char dS1; //+FC type4
	char dS2; //+FD type4
	char dS3; //+FE type4
	char dS4; //+FF type4
	char dDD; //+100 type4
	char dKB; //+101 type4
	char dSQ; //+102 type4
	char dRN; //+103 type4
	//int A1mv:1; //+104 type26 bit=4
	//int A2mv:1; //+104 type26 bit=5
	//int SCmv:1; //+104 type26 bit=7
	//int S1mv:1; //+104 type26 bit=8
	//int S2mv:1; //+104 type26 bit=9
	//int S3mv:1; //+104 type26 bit=10
	//int S4mv:1; //+104 type26 bit=11
	char pad_104[4]; //+104
	char InfernoLen; //+108 type4
	char InfernoAnim; //+109 type4
	char InfernoRollback; //+10A type4
	char ResurrectMode; //+10B type13 link=d2common_pMonModeLink
	short ResurrectSkill; //+10C type20 link=d2common_pSkillsLink
	short htTop; //+10E type3
	short htLeft; //+110 type3
	short htWidth; //+112 type3
	short htHeight; //+114 type3
	char pad_116[2]; //+116
	int automapCel; //+118 type2
	char localBlood; //+11C type4
	char bleed; //+11D type4
	char light; //+11E type4
	char light-r; //+11F type4
	char light-g; //+120 type4
	char light-b; //+121 type4
	char Utrans; //+122 type4
	char Utrans(N); //+123 type4
	char Utrans(H); //+124 type4
	char pad_125[3]; //+125
	int Heart; //+128 type9
	int BodyPart; //+12C type9
	char restore; //+130 type4
	char pad_131[3]; //+131
};
//monprop size=0x138 count=d2common_nMonProp ret=d2common_569F9
struct monprop_txt {
	int Id; //+00 type18 link=d2common_pMonPropLink
	int prop1; //+04 type19 link=d2common_pPropertiesLink
	int par1; //+08 type2
	int min1; //+0C type2
	int max1; //+10 type2
	int prop2; //+14 type19 link=d2common_pPropertiesLink
	int par2; //+18 type2
	int min2; //+1C type2
	int max2; //+20 type2
	int prop3; //+24 type19 link=d2common_pPropertiesLink
	int par3; //+28 type2
	int min3; //+2C type2
	int max3; //+30 type2
	int prop4; //+34 type19 link=d2common_pPropertiesLink
	int par4; //+38 type2
	int min4; //+3C type2
	int max4; //+40 type2
	int prop5; //+44 type19 link=d2common_pPropertiesLink
	int par5; //+48 type2
	int min5; //+4C type2
	int max5; //+50 type2
	int prop6; //+54 type19 link=d2common_pPropertiesLink
	int par6; //+58 type2
	int min6; //+5C type2
	int max6; //+60 type2
	int prop1_(N); //+64 type19 link=d2common_pPropertiesLink
	int par1_(N); //+68 type2
	int min1_(N); //+6C type2
	int max1_(N); //+70 type2
	int prop2_(N); //+74 type19 link=d2common_pPropertiesLink
	int par2_(N); //+78 type2
	int min2_(N); //+7C type2
	int max2_(N); //+80 type2
	int prop3_(N); //+84 type19 link=d2common_pPropertiesLink
	int par3_(N); //+88 type2
	int min3_(N); //+8C type2
	int max3_(N); //+90 type2
	int prop4_(N); //+94 type19 link=d2common_pPropertiesLink
	int par4_(N); //+98 type2
	int min4_(N); //+9C type2
	int max4_(N); //+A0 type2
	int prop5_(N); //+A4 type19 link=d2common_pPropertiesLink
	int par5_(N); //+A8 type2
	int min5_(N); //+AC type2
	int max5_(N); //+B0 type2
	int prop6_(N); //+B4 type19 link=d2common_pPropertiesLink
	int par6_(N); //+B8 type2
	int min6_(N); //+BC type2
	int max6_(N); //+C0 type2
	int prop1_(H); //+C4 type19 link=d2common_pPropertiesLink
	int par1_(H); //+C8 type2
	int min1_(H); //+CC type2
	int max1_(H); //+D0 type2
	int prop2_(H); //+D4 type19 link=d2common_pPropertiesLink
	int par2_(H); //+D8 type2
	int min2_(H); //+DC type2
	int max2_(H); //+E0 type2
	int prop3_(H); //+E4 type19 link=d2common_pPropertiesLink
	int par3_(H); //+E8 type2
	int min3_(H); //+EC type2
	int max3_(H); //+F0 type2
	int prop4_(H); //+F4 type19 link=d2common_pPropertiesLink
	int par4_(H); //+F8 type2
	int min4_(H); //+FC type2
	int max4_(H); //+100 type2
	int prop5_(H); //+104 type19 link=d2common_pPropertiesLink
	int par5_(H); //+108 type2
	int min5_(H); //+10C type2
	int max5_(H); //+110 type2
	int prop6_(H); //+114 type19 link=d2common_pPropertiesLink
	int par6_(H); //+118 type2
	int min6_(H); //+11C type2
	int max6_(H); //+120 type2
	char chance1; //+124 type4
	char chance2; //+125 type4
	char chance3; //+126 type4
	char chance4; //+127 type4
	char chance5; //+128 type4
	char chance6; //+129 type4
	char chance1_(N); //+12A type4
	char chance2_(N); //+12B type4
	char chance3_(N); //+12C type4
	char chance4_(N); //+12D type4
	char chance5_(N); //+12E type4
	char chance6_(N); //+12F type4
	char chance1_(H); //+130 type4
	char chance2_(H); //+131 type4
	char chance3_(H); //+132 type4
	char chance4_(H); //+133 type4
	char chance5_(H); //+134 type4
	char chance6_(H); //+135 type4
	char pad_136[2]; //+136
};
//monsounds size=0x94 count=d2common_nMonSounds ret=d2common_58809
struct monsounds_txt {
	short Id; //+00 type17 link=d2common_pMonSoundsLink
	char pad_2[2]; //+2
	int Attack1; //+04 type19 link=d2common_pSoundCodesLink
	int Att1Del; //+08 type2
	int Att1Prb; //+0C type2
	int Weapon1; //+10 type19 link=d2common_pSoundCodesLink
	int Wea1Del; //+14 type2
	int Wea1Vol; //+18 type2
	int Attack2; //+1C type19 link=d2common_pSoundCodesLink
	int Att2Del; //+20 type2
	int Att2Prb; //+24 type2
	int Weapon2; //+28 type19 link=d2common_pSoundCodesLink
	int Wea2Del; //+2C type2
	int Wea2Vol; //+30 type2
	int HitSound; //+34 type19 link=d2common_pSoundCodesLink
	int HitDelay; //+38 type2
	int DeathSound; //+3C type19 link=d2common_pSoundCodesLink
	int DeaDelay; //+40 type2
	int Skill1; //+44 type19 link=d2common_pSoundCodesLink
	int Skill2; //+48 type19 link=d2common_pSoundCodesLink
	int Skill3; //+4C type19 link=d2common_pSoundCodesLink
	int Skill4; //+50 type19 link=d2common_pSoundCodesLink
	int Footstep; //+54 type19 link=d2common_pSoundCodesLink
	int FootstepLayer; //+58 type19 link=d2common_pSoundCodesLink
	int FsCnt; //+5C type2
	int FsOff; //+60 type2
	int FsPrb; //+64 type2
	int Neutral; //+68 type19 link=d2common_pSoundCodesLink
	int NeuTime; //+6C type2
	int Init; //+70 type19 link=d2common_pSoundCodesLink
	int Taunt; //+74 type19 link=d2common_pSoundCodesLink
	int Flee; //+78 type19 link=d2common_pSoundCodesLink
	char CvtMo1; //+7C type13 link=d2common_pMonModeLink
	char CvtTgt1; //+7D type13 link=d2common_pMonModeLink
	char pad_7E[2]; //+7E
	int CvtSk1; //+80 type19 link=d2common_pSkillsLink
	char CvtMo2; //+84 type13 link=d2common_pMonModeLink
	char CvtTgt2; //+85 type13 link=d2common_pMonModeLink
	char pad_86[2]; //+86
	int CvtSk2; //+88 type19 link=d2common_pSkillsLink
	char CvtMo3; //+8C type13 link=d2common_pMonModeLink
	char CvtTgt3; //+8D type13 link=d2common_pMonModeLink
	char pad_8E[2]; //+8E
	int CvtSk3; //+90 type19 link=d2common_pSkillsLink
};
//monseq size=0x6 count=d2common_nMonSeq ret=d2common_5533C
struct monseq_txt {
	short sequence; //+00 type17 link=d2common_pMonSeqLink
	char mode; //+02 type13 link=d2common_pMonModeLink
	char frame; //+03 type4
	char dir; //+04 type4
	char event; //+05 type4
};
//monstats size=0x1A8 count=d2common_nMonStats ret=d2common_5D51F
struct monstats_txt {
	short Id; //+00 type17 link=d2common_pMonStatsLink
	//OVERLAP //type23 Sk1mode; //+00 type23 link=d2common_54130
	//OVERLAP //type23 Sk2mode; //+01 type23 link=d2common_54130
	short BaseId; //+02 type20 link=d2common_pMonStatsLink
	//OVERLAP //type23 Sk3mode; //+02 type23 link=d2common_54130
	//OVERLAP //type23 Sk4mode; //+03 type23 link=d2common_54130
	short NextInClass; //+04 type20 link=d2common_pMonStatsLink
	//OVERLAP //type23 Sk5mode; //+04 type23 link=d2common_54130
	//OVERLAP //type23 Sk6mode; //+05 type23 link=d2common_54130
	short NameStr; //+06 type22 link=d2common_5E350
	//OVERLAP //type23 Sk7mode; //+06 type23 link=d2common_54130
	//OVERLAP //type23 Sk8mode; //+07 type23 link=d2common_54130
	short DescStr; //+08 type22 link=d2common_5E350
	char pad_A[2]; //+A
	//int isSpawn:1; //+0C type26 bit=0
	//int isMelee:1; //+0C type26 bit=1
	//int noRatio:1; //+0C type26 bit=2
	//int opendoors:1; //+0C type26 bit=3
	//int SetBoss:1; //+0C type26 bit=4
	//int BossXfer:1; //+0C type26 bit=5
	//int boss:1; //+0C type26 bit=6
	//int primeevil:1; //+0C type26 bit=7
	//int npc:1; //+0C type26 bit=8
	//int interact:1; //+0C type26 bit=9
	//int inTown:1; //+0C type26 bit=10
	//int lUndead:1; //+0C type26 bit=11
	//int hUndead:1; //+0C type26 bit=12
	//int demon:1; //+0C type26 bit=13
	//int flying:1; //+0C type26 bit=14
	//int killable:1; //+0C type26 bit=15
	//int switchai:1; //+0C type26 bit=16
	//int nomultishot:1; //+0C type26 bit=17
	//int neverCount:1; //+0C type26 bit=18
	//int petIgnore:1; //+0C type26 bit=19
	//int deathDmg:1; //+0C type26 bit=20
	//int genericSpawn:1; //+0C type26 bit=21
	//int zoo:1; //+0C type26 bit=22
	//int placespawn:1; //+0C type26 bit=23
	//int inventory:1; //+0C type26 bit=24
	//int enabled:1; //+0C type26 bit=25
	//int NoShldBlock:1; //+0C type26 bit=26
	//int noaura:1; //+0C type26 bit=27
	//int rangedtype:1; //+0C type26 bit=28
	char pad_C[4]; //+C
	int Code; //+10 type9
	short MonSound; //+14 type20 link=d2common_pMonSoundsLink
	short UMonSound; //+16 type20 link=d2common_pMonSoundsLink
	short MonStatsEx; //+18 type20 link=d2common_pMonStats2Link
	short MonProp; //+1A type20 link=d2common_pMonPropLink
	short MonType; //+1C type20 link=d2common_pMonTypeLink
	short AI; //+1E type20 link=d2common_pMonAiLink
	short spawn; //+20 type20 link=d2common_pMonStatsLink
	char spawnx; //+22 type4
	char spawny; //+23 type4
	char spawnmode; //+24 type13 link=d2common_pMonModeLink
	char pad_25; //+25
	short minion1; //+26 type20 link=d2common_pMonStatsLink
	short minion2; //+28 type20 link=d2common_pMonStatsLink
	char pad_2A[2]; //+2A
	char PartyMin; //+2C type4
	char PartyMax; //+2D type4
	char Rarity; //+2E type4
	char MinGrp; //+2F type4
	char MaxGrp; //+30 type4
	char sparsePopulate; //+31 type4
	short Velocity; //+32 type3
	short Run; //+34 type3
	char pad_36[4]; //+36
	short MissA1; //+3A type20 link=d2common_pMissilesLink
	short MissA2; //+3C type20 link=d2common_pMissilesLink
	short MissS1; //+3E type20 link=d2common_pMissilesLink
	short MissS2; //+40 type20 link=d2common_pMissilesLink
	short MissS3; //+42 type20 link=d2common_pMissilesLink
	short MissS4; //+44 type20 link=d2common_pMissilesLink
	short MissC; //+46 type20 link=d2common_pMissilesLink
	short MissSQ; //+48 type20 link=d2common_pMissilesLink
	char pad_4A[2]; //+4A
	char Align; //+4C type4
	char TransLvl; //+4D type4
	char threat; //+4E type4
	char aidel; //+4F type4
	char aidel(N); //+50 type4
	char aidel(H); //+51 type4
	char aidist; //+52 type4
	char aidist(N); //+53 type4
	char aidist(H); //+54 type4
	char pad_55; //+55
	short aip1; //+56 type3
	short aip1(N); //+58 type3
	short aip1(H); //+5A type3
	short aip2; //+5C type3
	short aip2(N); //+5E type3
	short aip2(H); //+60 type3
	short aip3; //+62 type3
	short aip3(N); //+64 type3
	short aip3(H); //+66 type3
	short aip4; //+68 type3
	short aip4(N); //+6A type3
	short aip4(H); //+6C type3
	short aip5; //+6E type3
	short aip5(N); //+70 type3
	short aip5(H); //+72 type3
	short aip6; //+74 type3
	short aip6(N); //+76 type3
	short aip6(H); //+78 type3
	short aip7; //+7A type3
	short aip7(N); //+7C type3
	short aip7(H); //+7E type3
	short aip8; //+80 type3
	short aip8(N); //+82 type3
	short aip8(H); //+84 type3
	short TreasureClass1; //+86 type20 link=d2common_pTreasureClassExLink
	short TreasureClass2; //+88 type20 link=d2common_pTreasureClassExLink
	short TreasureClass3; //+8A type20 link=d2common_pTreasureClassExLink
	short TreasureClass4; //+8C type20 link=d2common_pTreasureClassExLink
	short TreasureClass1(N); //+8E type20 link=d2common_pTreasureClassExLink
	short TreasureClass2(N); //+90 type20 link=d2common_pTreasureClassExLink
	short TreasureClass3(N); //+92 type20 link=d2common_pTreasureClassExLink
	short TreasureClass4(N); //+94 type20 link=d2common_pTreasureClassExLink
	short TreasureClass1(H); //+96 type20 link=d2common_pTreasureClassExLink
	short TreasureClass2(H); //+98 type20 link=d2common_pTreasureClassExLink
	short TreasureClass3(H); //+9A type20 link=d2common_pTreasureClassExLink
	short TreasureClass4(H); //+9C type20 link=d2common_pTreasureClassExLink
	char TCQuestId; //+9E type4
	char TCQuestCP; //+9F type4
	char Drain; //+A0 type4
	char Drain(N); //+A1 type4
	char Drain(H); //+A2 type4
	char ToBlock; //+A3 type4
	char ToBlock(N); //+A4 type4
	char ToBlock(H); //+A5 type4
	char Crit; //+A6 type4
	char pad_A7; //+A7
	short SkillDamage; //+A8 type20 link=d2common_pSkillsLink
	short Level; //+AA type3
	short Level(N); //+AC type3
	short Level(H); //+AE type3
	short MinHP; //+B0 type3
	short MinHP(N); //+B2 type3
	short MinHP(H); //+B4 type3
	short MaxHP; //+B6 type3
	short MaxHP(N); //+B8 type3
	short MaxHP(H); //+BA type3
	short AC; //+BC type3
	short AC(N); //+BE type3
	short AC(H); //+C0 type3
	short A1TH; //+C2 type3
	short A1TH(N); //+C4 type3
	short A1TH(H); //+C6 type3
	short A2TH; //+C8 type3
	short A2TH(N); //+CA type3
	short A2TH(H); //+CC type3
	short S1TH; //+CE type3
	short S1TH(N); //+D0 type3
	short S1TH(H); //+D2 type3
	short Exp; //+D4 type3
	short Exp(N); //+D6 type3
	short Exp(H); //+D8 type3
	short A1MinD; //+DA type3
	short A1MinD(N); //+DC type3
	short A1MinD(H); //+DE type3
	short A1MaxD; //+E0 type3
	short A1MaxD(N); //+E2 type3
	short A1MaxD(H); //+E4 type3
	short A2MinD; //+E6 type3
	short A2MinD(N); //+E8 type3
	short A2MinD(H); //+EA type3
	short A2MaxD; //+EC type3
	short A2MaxD(N); //+EE type3
	short A2MaxD(H); //+F0 type3
	short S1MinD; //+F2 type3
	short S1MinD(N); //+F4 type3
	short S1MinD(H); //+F6 type3
	short S1MaxD; //+F8 type3
	short S1MaxD(N); //+FA type3
	short S1MaxD(H); //+FC type3
	char El1Mode; //+FE type13 link=d2common_pMonModeLink
	char El2Mode; //+FF type13 link=d2common_pMonModeLink
	char El3Mode; //+100 type13 link=d2common_pMonModeLink
	char El1Type; //+101 type13 link=d2common_pElemTypesLink
	char El2Type; //+102 type13 link=d2common_pElemTypesLink
	char El3Type; //+103 type13 link=d2common_pElemTypesLink
	char El1Pct; //+104 type4
	char El1Pct(N); //+105 type4
	char El1Pct(H); //+106 type4
	char El2Pct; //+107 type4
	char El2Pct(N); //+108 type4
	char El2Pct(H); //+109 type4
	char El3Pct; //+10A type4
	char El3Pct(N); //+10B type4
	char El3Pct(H); //+10C type4
	char pad_10D; //+10D
	short El1MinD; //+10E type3
	short El1MinD(N); //+110 type3
	short El1MinD(H); //+112 type3
	short El2MinD; //+114 type3
	short El2MinD(N); //+116 type3
	short El2MinD(H); //+118 type3
	short El3MinD; //+11A type3
	short El3MinD(N); //+11C type3
	short El3MinD(H); //+11E type3
	short El1MaxD; //+120 type3
	short El1MaxD(N); //+122 type3
	short El1MaxD(H); //+124 type3
	short El2MaxD; //+126 type3
	short El2MaxD(N); //+128 type3
	short El2MaxD(H); //+12A type3
	short El3MaxD; //+12C type3
	short El3MaxD(N); //+12E type3
	short El3MaxD(H); //+130 type3
	short El1Dur; //+132 type3
	short El1Dur(N); //+134 type3
	short El1Dur(H); //+136 type3
	short El2Dur; //+138 type3
	short El2Dur(N); //+13A type3
	short El2Dur(H); //+13C type3
	short El3Dur; //+13E type3
	short El3Dur(N); //+140 type3
	short El3Dur(H); //+142 type3
	short ResDm; //+144 type3
	short ResDm(N); //+146 type3
	short ResDm(H); //+148 type3
	short ResMa; //+14A type3
	short ResMa(N); //+14C type3
	short ResMa(H); //+14E type3
	short ResFi; //+150 type3
	short ResFi(N); //+152 type3
	short ResFi(H); //+154 type3
	short ResLi; //+156 type3
	short ResLi(N); //+158 type3
	short ResLi(H); //+15A type3
	short ResCo; //+15C type3
	short ResCo(N); //+15E type3
	short ResCo(H); //+160 type3
	short ResPo; //+162 type3
	short ResPo(N); //+164 type3
	short ResPo(H); //+166 type3
	char ColdEffect; //+168 type4
	char ColdEffect(N); //+169 type4
	char ColdEffect(H); //+16A type4
	char pad_16B; //+16B
	int SendSkills; //+16C type8
	short Skill1; //+170 type20 link=d2common_pSkillsLink
	short Skill2; //+172 type20 link=d2common_pSkillsLink
	short Skill3; //+174 type20 link=d2common_pSkillsLink
	short Skill4; //+176 type20 link=d2common_pSkillsLink
	short Skill5; //+178 type20 link=d2common_pSkillsLink
	short Skill6; //+17A type20 link=d2common_pSkillsLink
	short Skill7; //+17C type20 link=d2common_pSkillsLink
	short Skill8; //+17E type20 link=d2common_pSkillsLink
	char pad_180[24]; //+180
	char Sk1lvl; //+198 type4
	char Sk2lvl; //+199 type4
	char Sk3lvl; //+19A type4
	char Sk4lvl; //+19B type4
	char Sk5lvl; //+19C type4
	char Sk6lvl; //+19D type4
	char Sk7lvl; //+19E type4
	char Sk8lvl; //+19F type4
	int DamageRegen; //+1A0 type2
	char SplEndDeath; //+1A4 type4
	char SplGetModeChart; //+1A5 type4
	char SplEndGeneric; //+1A6 type4
	char SplClientEnd; //+1A7 type4
};
//monumod size=0x20 count=d2common_nMonUMod ret=d2common_59F4C
struct monumod_txt {
	short uniquemod; //+00 type17 link=d2common_pMonUModLink
	char pad_2[2]; //+2
	short version; //+04 type3
	char enabled; //+06 type4
	char xfer; //+07 type4
	char champion; //+08 type4
	char fpick; //+09 type4
	short exclude1; //+0A type20 link=d2common_pMonTypeLink
	short exclude2; //+0C type20 link=d2common_pMonTypeLink
	short cpick; //+0E type3
	short cpick_(N); //+10 type3
	short cpick_(H); //+12 type3
	short upick; //+14 type3
	short upick_(N); //+16 type3
	short upick_(H); //+18 type3
	char pad_1A[2]; //+1A
	int constants; //+1C type2
};
//superuniques size=0x34 count=d2common_nSuperUniques ret=d2common_59BF6
struct superuniques_txt {
	short Superunique; //+00 type17 link=d2common_pSuperUniquesLink
	short Name; //+02 type22 link=d2common_5E350
	int Class; //+04 type19 link=d2common_pMonStatsLink
	int hcIdx; //+08 type2
	int Mod1; //+0C type2
	int Mod2; //+10 type2
	int Mod3; //+14 type2
	int MonSound; //+18 type19 link=d2common_pMonSoundsLink
	int MinGrp; //+1C type2
	int MaxGrp; //+20 type2
	char AutoPos; //+24 type4
	char EClass; //+25 type4
	char Stacks; //+26 type4
	char Replaceable; //+27 type4
	char Utrans; //+28 type4
	char Utrans(N); //+29 type4
	char Utrans(H); //+2A type4
	char pad_2B; //+2B
	short TC; //+2C type20 link=d2common_pTreasureClassExLink
	short TC(N); //+2E type20 link=d2common_pTreasureClassExLink
	short TC(H); //+30 type20 link=d2common_pTreasureClassExLink
	char pad_32[2]; //+32
};
//monpreset size=0x4 count=12FD30 ret=d2common_55506
struct monpreset_txt {
	char Act; //+00 type4
	//OVERLAP //type23 Place; //+00 type23 link=d2common_52E70
	char pad_1[3]; //+1
};
//hireling size=0x118 count=d2common_nHirelings ret=d2common_59718
struct hireling_txt {
	short version; //+00 type3
	char pad_2[2]; //+2
	int id; //+04 type2
	int class; //+08 type2
	int act; //+0C type2
	int difficulty; //+10 type2
	int seller; //+14 type2
	int gold; //+18 type2
	int level; //+1C type2
	int exp/lvl; //+20 type2
	int hp; //+24 type2
	int hp/lvl; //+28 type2
	int defense; //+2C type2
	int def/lvl; //+30 type2
	int str; //+34 type2
	int str/lvl; //+38 type2
	int dex; //+3C type2
	int dex/lvl; //+40 type2
	int ar; //+44 type2
	int ar/lvl; //+48 type2
	int share; //+4C type2
	int dmg-min; //+50 type2
	int dmg-max; //+54 type2
	int dmg/lvl; //+58 type2
	int resist; //+5C type2
	int resist/lvl; //+60 type2
	int defaultchance; //+64 type2
	int head; //+68 type2
	int torso; //+6C type2
	int weapon; //+70 type2
	int shield; //+74 type2
	int skill1; //+78 type19 link=d2common_pSkillsLink
	int skill2; //+7C type19 link=d2common_pSkillsLink
	int skill3; //+80 type19 link=d2common_pSkillsLink
	int skill4; //+84 type19 link=d2common_pSkillsLink
	int skill5; //+88 type19 link=d2common_pSkillsLink
	int skill6; //+8C type19 link=d2common_pSkillsLink
	int chance1; //+90 type2
	int chance2; //+94 type2
	int chance3; //+98 type2
	int chance4; //+9C type2
	int chance5; //+A0 type2
	int chance6; //+A4 type2
	int chanceperlvl1; //+A8 type2
	int chanceperlvl2; //+AC type2
	int chanceperlvl3; //+B0 type2
	int chanceperlvl4; //+B4 type2
	int chanceperlvl5; //+B8 type2
	int chanceperlvl6; //+BC type2
	char mode1; //+C0 type4
	char mode2; //+C1 type4
	char mode3; //+C2 type4
	char mode4; //+C3 type4
	char mode5; //+C4 type4
	char mode6; //+C5 type4
	char level1; //+C6 type4
	char level2; //+C7 type4
	char level3; //+C8 type4
	char level4; //+C9 type4
	char level5; //+CA type4
	char level6; //+CB type4
	char lvlperlvl1; //+CC type4
	char lvlperlvl2; //+CD type4
	char lvlperlvl3; //+CE type4
	char lvlperlvl4; //+CF type4
	char lvlperlvl5; //+D0 type4
	char lvlperlvl6; //+D1 type4
	char hiredesc; //+D2 type13 link=d2common_pHireDescLink
	char namefirst[32]; //+D3 type1
	char namelast[32]; //+F3 type1
	char pad_113[5]; //+113
};
//npc size=0x4C count=d2common_nNpcs ret=d2common_58B46
struct npc_txt {
	int npc; //+00 type19 link=d2common_pMonStatsLink
	int sell_mult; //+04 type2
	int buy_mult; //+08 type2
	int rep_mult; //+0C type2
	int questflag_A; //+10 type2
	int questflag_B; //+14 type2
	int questflag_C; //+18 type2
	int questsellmult_A; //+1C type2
	int questsellmult_B; //+20 type2
	int questsellmult_C; //+24 type2
	int questbuymult_A; //+28 type2
	int questbuymult_B; //+2C type2
	int questbuymult_C; //+30 type2
	int questrepmult_A; //+34 type2
	int questrepmult_B; //+38 type2
	int questrepmult_C; //+3C type2
	int max_buy; //+40 type2
	int max_buy_(N); //+44 type2
	int max_buy_(H); //+48 type2
};
//monequip size=0x1C count=d2common_nMonEquip ret=d2common_55113
struct monequip_txt {
	short monster; //+00 type20 link=d2common_pMonStatsLink
	short level; //+02 type3
	char oninit; //+04 type4
	char pad_5[3]; //+5
	int item1; //+08 type9 link=d2common_pItemLink
	int item2; //+0C type9 link=d2common_pItemLink
	int item3; //+10 type9 link=d2common_pItemLink
	char loc1; //+14 type13 link=d2common_pBodyLocsLink
	char loc2; //+15 type13 link=d2common_pBodyLocsLink
	char loc3; //+16 type13 link=d2common_pBodyLocsLink
	char mod1; //+17 type4
	char mod2; //+18 type4
	char mod3; //+19 type4
	char pad_1A[2]; //+1A
};
//levels size=0x220 count=d2common_nLevels ret=d2common_6F8E8
struct levels_txt {
	char Id; //+00 type4
	char pad_1; //+1
	char Pal; //+02 type4
	char Act; //+03 type4
	char Teleport; //+04 type4
	char Rain; //+05 type4
	char Mud; //+06 type4
	char NoPer; //+07 type4
	char IsInside; //+08 type4
	char DrawEdges; //+09 type4
	char pad_A[2]; //+A
	int WarpDist; //+0C type2
	short MonLvl1; //+10 type3
	short MonLvl2; //+12 type3
	short MonLvl3; //+14 type3
	short MonLvl1Ex; //+16 type3
	short MonLvl2Ex; //+18 type3
	short MonLvl3Ex; //+1A type3
	int MonDen; //+1C type2
	int MonDen(N); //+20 type2
	int MonDen(H); //+24 type2
	char MonUMin; //+28 type4
	char MonUMin(N); //+29 type4
	char MonUMin(H); //+2A type4
	char MonUMax; //+2B type4
	char MonUMax(N); //+2C type4
	char MonUMax(H); //+2D type4
	char MonWndr; //+2E type4
	char MonSpcWalk; //+2F type4
	char Quest; //+30 type4
	char rangedspawn; //+31 type4
	char NumMon; //+32 type4
	char pad_33[3]; //+33
	short mon1; //+36 type20 link=d2common_pMonStatsLink
	short mon2; //+38 type20 link=d2common_pMonStatsLink
	short mon3; //+3A type20 link=d2common_pMonStatsLink
	short mon4; //+3C type20 link=d2common_pMonStatsLink
	short mon5; //+3E type20 link=d2common_pMonStatsLink
	short mon6; //+40 type20 link=d2common_pMonStatsLink
	short mon7; //+42 type20 link=d2common_pMonStatsLink
	short mon8; //+44 type20 link=d2common_pMonStatsLink
	short mon9; //+46 type20 link=d2common_pMonStatsLink
	short mon10; //+48 type20 link=d2common_pMonStatsLink
	short mon11; //+4A type20 link=d2common_pMonStatsLink
	short mon12; //+4C type20 link=d2common_pMonStatsLink
	short mon13; //+4E type20 link=d2common_pMonStatsLink
	short mon14; //+50 type20 link=d2common_pMonStatsLink
	short mon15; //+52 type20 link=d2common_pMonStatsLink
	short mon16; //+54 type20 link=d2common_pMonStatsLink
	short mon17; //+56 type20 link=d2common_pMonStatsLink
	short mon18; //+58 type20 link=d2common_pMonStatsLink
	short mon19; //+5A type20 link=d2common_pMonStatsLink
	short mon20; //+5C type20 link=d2common_pMonStatsLink
	short mon21; //+5E type20 link=d2common_pMonStatsLink
	short mon22; //+60 type20 link=d2common_pMonStatsLink
	short mon23; //+62 type20 link=d2common_pMonStatsLink
	short mon24; //+64 type20 link=d2common_pMonStatsLink
	short mon25; //+66 type20 link=d2common_pMonStatsLink
	short nmon1; //+68 type20 link=d2common_pMonStatsLink
	short nmon2; //+6A type20 link=d2common_pMonStatsLink
	short nmon3; //+6C type20 link=d2common_pMonStatsLink
	short nmon4; //+6E type20 link=d2common_pMonStatsLink
	short nmon5; //+70 type20 link=d2common_pMonStatsLink
	short nmon6; //+72 type20 link=d2common_pMonStatsLink
	short nmon7; //+74 type20 link=d2common_pMonStatsLink
	short nmon8; //+76 type20 link=d2common_pMonStatsLink
	short nmon9; //+78 type20 link=d2common_pMonStatsLink
	short nmon10; //+7A type20 link=d2common_pMonStatsLink
	short nmon11; //+7C type20 link=d2common_pMonStatsLink
	short nmon12; //+7E type20 link=d2common_pMonStatsLink
	short nmon13; //+80 type20 link=d2common_pMonStatsLink
	short nmon14; //+82 type20 link=d2common_pMonStatsLink
	short nmon15; //+84 type20 link=d2common_pMonStatsLink
	short nmon16; //+86 type20 link=d2common_pMonStatsLink
	short nmon17; //+88 type20 link=d2common_pMonStatsLink
	short nmon18; //+8A type20 link=d2common_pMonStatsLink
	short nmon19; //+8C type20 link=d2common_pMonStatsLink
	short nmon20; //+8E type20 link=d2common_pMonStatsLink
	short nmon21; //+90 type20 link=d2common_pMonStatsLink
	short nmon22; //+92 type20 link=d2common_pMonStatsLink
	short nmon23; //+94 type20 link=d2common_pMonStatsLink
	short nmon24; //+96 type20 link=d2common_pMonStatsLink
	short nmon25; //+98 type20 link=d2common_pMonStatsLink
	short umon1; //+9A type20 link=d2common_pMonStatsLink
	short umon2; //+9C type20 link=d2common_pMonStatsLink
	short umon3; //+9E type20 link=d2common_pMonStatsLink
	short umon4; //+A0 type20 link=d2common_pMonStatsLink
	short umon5; //+A2 type20 link=d2common_pMonStatsLink
	short umon6; //+A4 type20 link=d2common_pMonStatsLink
	short umon7; //+A6 type20 link=d2common_pMonStatsLink
	short umon8; //+A8 type20 link=d2common_pMonStatsLink
	short umon9; //+AA type20 link=d2common_pMonStatsLink
	short umon10; //+AC type20 link=d2common_pMonStatsLink
	short umon11; //+AE type20 link=d2common_pMonStatsLink
	short umon12; //+B0 type20 link=d2common_pMonStatsLink
	short umon13; //+B2 type20 link=d2common_pMonStatsLink
	short umon14; //+B4 type20 link=d2common_pMonStatsLink
	short umon15; //+B6 type20 link=d2common_pMonStatsLink
	short umon16; //+B8 type20 link=d2common_pMonStatsLink
	short umon17; //+BA type20 link=d2common_pMonStatsLink
	short umon18; //+BC type20 link=d2common_pMonStatsLink
	short umon19; //+BE type20 link=d2common_pMonStatsLink
	short umon20; //+C0 type20 link=d2common_pMonStatsLink
	short umon21; //+C2 type20 link=d2common_pMonStatsLink
	short umon22; //+C4 type20 link=d2common_pMonStatsLink
	short umon23; //+C6 type20 link=d2common_pMonStatsLink
	short umon24; //+C8 type20 link=d2common_pMonStatsLink
	short umon25; //+CA type20 link=d2common_pMonStatsLink
	short cmon1; //+CC type20 link=d2common_pMonStatsLink
	short cmon2; //+CE type20 link=d2common_pMonStatsLink
	short cmon3; //+D0 type20 link=d2common_pMonStatsLink
	short cmon4; //+D2 type20 link=d2common_pMonStatsLink
	short cpct1; //+D4 type3
	short cpct2; //+D6 type3
	short cpct3; //+D8 type3
	short cpct4; //+DA type3
	short camt1; //+DC type3
	//OVERLAP short camt2; //+DC type3
	//OVERLAP short camt3; //+DC type3
	//OVERLAP short camt4; //+DC type3
	char pad_DE[6]; //+DE
	char Waypoint; //+E4 type4
	char ObjGrp0; //+E5 type4
	char ObjGrp1; //+E6 type4
	char ObjGrp2; //+E7 type4
	char ObjGrp3; //+E8 type4
	char ObjGrp4; //+E9 type4
	char ObjGrp5; //+EA type4
	char ObjGrp6; //+EB type4
	char ObjGrp7; //+EC type4
	char ObjPrb0; //+ED type4
	char ObjPrb1; //+EE type4
	char ObjPrb2; //+EF type4
	char ObjPrb3; //+F0 type4
	char ObjPrb4; //+F1 type4
	char ObjPrb5; //+F2 type4
	char ObjPrb6; //+F3 type4
	char ObjPrb7; //+F4 type4
	char LevelName[39]; //+F5 type1
	char pad_11C; //+11C
	char LevelWarp[39]; //+11D type1
	char pad_144; //+144
	char EntryFile[39]; //+145 type1
	char pad_16C[164]; //+16C
	int Themes; //+210 type2
	int FloorFilter; //+214 type2
	int BlankScreen; //+218 type2
	char SoundEnv; //+21C type4
	char pad_21D[3]; //+21D
};
//leveldefs size=0x9C count=0 ret=d2common_6E0AB
struct leveldefs_txt {
	int QuestFlag; //+00 type2
	int QuestFlagEx; //+04 type2
	int Layer; //+08 type2
	int SizeX; //+0C type2
	int SizeX(N); //+10 type2
	int SizeX(H); //+14 type2
	int SizeY; //+18 type2
	int SizeY(N); //+1C type2
	int SizeY(H); //+20 type2
	int OffsetX; //+24 type2
	int OffsetY; //+28 type2
	int Depend; //+2C type2
	int DrlgType; //+30 type2
	int LevelType; //+34 type2
	int SubType; //+38 type2
	int SubTheme; //+3C type2
	int SubWaypoint; //+40 type2
	int SubShrine; //+44 type2
	int Vis0; //+48 type2
	int Vis1; //+4C type2
	int Vis2; //+50 type2
	int Vis3; //+54 type2
	int Vis4; //+58 type2
	int Vis5; //+5C type2
	int Vis6; //+60 type2
	int Vis7; //+64 type2
	int Warp0; //+68 type2
	int Warp1; //+6C type2
	int Warp2; //+70 type2
	int Warp3; //+74 type2
	int Warp4; //+78 type2
	int Warp5; //+7C type2
	int Warp6; //+80 type2
	int Warp7; //+84 type2
	char Intensity; //+88 type4
	char Red; //+89 type4
	char Green; //+8A type4
	char Blue; //+8B type4
	int Portal; //+8C type2
	int Position; //+90 type2
	int SaveMonsters; //+94 type2
	int LOSDraw; //+98 type2
};
//lvltypes size=0x788 count=d2common_A1428 ret=d2common_6D960
struct lvltypes_txt {
	char File_1[59]; //+00 type1
	char pad_3B; //+3B
	char File_2[59]; //+3C type1
	char pad_77; //+77
	char File_3[59]; //+78 type1
	char pad_B3; //+B3
	char File_4[59]; //+B4 type1
	char pad_EF; //+EF
	char File_5[59]; //+F0 type1
	char pad_12B; //+12B
	char File_6[59]; //+12C type1
	char pad_167; //+167
	char File_7[59]; //+168 type1
	char pad_1A3; //+1A3
	char File_8[59]; //+1A4 type1
	char pad_1DF; //+1DF
	char File_9[59]; //+1E0 type1
	char pad_21B; //+21B
	char File_10[59]; //+21C type1
	char pad_257; //+257
	char File_11[59]; //+258 type1
	char pad_293; //+293
	char File_12[59]; //+294 type1
	char pad_2CF; //+2CF
	char File_13[59]; //+2D0 type1
	char pad_30B; //+30B
	char File_14[59]; //+30C type1
	char pad_347; //+347
	char File_15[59]; //+348 type1
	char pad_383; //+383
	char File_16[59]; //+384 type1
	char pad_3BF; //+3BF
	char File_17[59]; //+3C0 type1
	char pad_3FB; //+3FB
	char File_18[59]; //+3FC type1
	char pad_437; //+437
	char File_19[59]; //+438 type1
	char pad_473; //+473
	char File_20[59]; //+474 type1
	char pad_4AF; //+4AF
	char File_21[59]; //+4B0 type1
	char pad_4EB; //+4EB
	char File_22[59]; //+4EC type1
	char pad_527; //+527
	char File_23[59]; //+528 type1
	char pad_563; //+563
	char File_24[59]; //+564 type1
	char pad_59F; //+59F
	char File_25[59]; //+5A0 type1
	char pad_5DB; //+5DB
	char File_26[59]; //+5DC type1
	char pad_617; //+617
	char File_27[59]; //+618 type1
	char pad_653; //+653
	char File_28[59]; //+654 type1
	char pad_68F; //+68F
	char File_29[59]; //+690 type1
	char pad_6CB; //+6CB
	char File_30[59]; //+6CC type1
	char pad_707; //+707
	char File_31[59]; //+708 type1
	char pad_743; //+743
	char File_32[59]; //+744 type1
	char pad_77F; //+77F
	char Act; //+780 type4
	char pad_781[3]; //+781
	int Expansion; //+784 type2
};
//lvlprest size=0x1B0 count=d2common_nLvlPrest ret=d2common_704FD
struct lvlprest_txt {
	int Def; //+00 type2
	int LevelId; //+04 type2
	int Populate; //+08 type2
	int Logicals; //+0C type2
	int Outdoors; //+10 type2
	int Animate; //+14 type2
	int KillEdge; //+18 type2
	int FillBlanks; //+1C type2
	int Expansion; //+20 type2
	char pad_24[4]; //+24
	int SizeX; //+28 type2
	int SizeY; //+2C type2
	int AutoMap; //+30 type2
	int Scan; //+34 type2
	int Pops; //+38 type2
	int PopPad; //+3C type2
	int Files; //+40 type2
	char File1[59]; //+44 type1
	char pad_7F; //+7F
	char File2[59]; //+80 type1
	char pad_BB; //+BB
	char File3[59]; //+BC type1
	char pad_F7; //+F7
	char File4[59]; //+F8 type1
	char pad_133; //+133
	char File5[59]; //+134 type1
	char pad_16F; //+16F
	char File6[59]; //+170 type1
	char pad_1AB; //+1AB
	int Dt1Mask; //+1AC type2
};
//lvlwarp size=0x30 count=12FCA0 ret=d2common_6D389
struct lvlwarp_txt {
	int Id; //+00 type2
	int SelectX; //+04 type2
	int SelectY; //+08 type2
	int SelectDX; //+0C type2
	int SelectDY; //+10 type2
	int ExitWalkX; //+14 type2
	int ExitWalkY; //+18 type2
	int OffsetX; //+1C type2
	int OffsetY; //+20 type2
	int LitVersion; //+24 type2
	int Tiles; //+28 type2
	char Direction[1]; //+2C type1
	char pad_2D[3]; //+2D
};
//lvlmaze size=0x1C count=d2common_A1438 ret=d2common_6D174
struct lvlmaze_txt {
	int Level; //+00 type2
	int Rooms; //+04 type2
	int Rooms(N); //+08 type2
	int Rooms(H); //+0C type2
	int SizeX; //+10 type2
	int SizeY; //+14 type2
	int Merge; //+18 type2
};
//lvlsub size=0x15C count=d2common_A1440 ret=d2common_6FF59
struct lvlsub_txt {
	int Type; //+00 type2
	char File[59]; //+04 type1
	char pad_3F; //+3F
	int CheckAll; //+40 type2
	int BordType; //+44 type2
	int Dt1Mask; //+48 type2
	int GridSize; //+4C type2
	char pad_50[204]; //+50
	int Prob0; //+11C type2
	int Prob1; //+120 type2
	int Prob2; //+124 type2
	int Prob3; //+128 type2
	int Prob4; //+12C type2
	int Trials0; //+130 type2
	int Trials1; //+134 type2
	int Trials2; //+138 type2
	int Trials3; //+13C type2
	int Trials4; //+140 type2
	int Max0; //+144 type2
	int Max1; //+148 type2
	int Max2; //+14C type2
	int Max3; //+150 type2
	int Max4; //+154 type2
	int Expansion; //+158 type2
};
//automap size=0x2C count=12FCD4 ret=d2common_6CE9F
struct automap_txt {
	char LevelName[15]; //+00 type1
	char pad_F; //+F
	char TileName[7]; //+10 type1
	char pad_17; //+17
	char Style; //+18 type4
	char StartSequence; //+19 type4
	char EndSequence; //+1A type4
	char pad_1B; //+1B
	int Cel1; //+1C type2
	int Cel2; //+20 type2
	int Cel3; //+24 type2
	int Cel4; //+28 type2
};
//objects size=0x1C0 count=d2common_pArenaTxt ret=d2common_40B2A
struct objects_txt {
	char Name[63]; //+00 type1
	char pad_3F[129]; //+3F
	char Token[2]; //+C0 type1
	char pad_C2; //+C2
	char SpawnMax; //+C3 type4
	char Selectable0; //+C4 type4
	char Selectable1; //+C5 type4
	char Selectable2; //+C6 type4
	char Selectable3; //+C7 type4
	char Selectable4; //+C8 type4
	char Selectable5; //+C9 type4
	char Selectable6; //+CA type4
	char Selectable7; //+CB type4
	char TrapProb; //+CC type4
	char pad_CD[3]; //+CD
	int SizeX; //+D0 type2
	int SizeY; //+D4 type2
	int FrameCnt0; //+D8 type2
	int FrameCnt1; //+DC type2
	int FrameCnt2; //+E0 type2
	int FrameCnt3; //+E4 type2
	int FrameCnt4; //+E8 type2
	int FrameCnt5; //+EC type2
	int FrameCnt6; //+F0 type2
	int FrameCnt7; //+F4 type2
	short FrameDelta0; //+F8 type3
	short FrameDelta1; //+FA type3
	short FrameDelta2; //+FC type3
	short FrameDelta3; //+FE type3
	short FrameDelta4; //+100 type3
	short FrameDelta5; //+102 type3
	short FrameDelta6; //+104 type3
	short FrameDelta7; //+106 type3
	char CycleAnim0; //+108 type4
	char CycleAnim1; //+109 type4
	char CycleAnim2; //+10A type4
	char CycleAnim3; //+10B type4
	char CycleAnim4; //+10C type4
	char CycleAnim5; //+10D type4
	char CycleAnim6; //+10E type4
	char CycleAnim7; //+10F type4
	char Lit0; //+110 type4
	char Lit1; //+111 type4
	char Lit2; //+112 type4
	char Lit3; //+113 type4
	char Lit4; //+114 type4
	char Lit5; //+115 type4
	char Lit6; //+116 type4
	char Lit7; //+117 type4
	char BlocksLight0; //+118 type4
	char BlocksLight1; //+119 type4
	char BlocksLight2; //+11A type4
	char BlocksLight3; //+11B type4
	char BlocksLight4; //+11C type4
	char BlocksLight5; //+11D type4
	char BlocksLight6; //+11E type4
	char BlocksLight7; //+11F type4
	char HasCollision0; //+120 type4
	char HasCollision1; //+121 type4
	char HasCollision2; //+122 type4
	char HasCollision3; //+123 type4
	char HasCollision4; //+124 type4
	char HasCollision5; //+125 type4
	char HasCollision6; //+126 type4
	char HasCollision7; //+127 type4
	char IsAttackable0; //+128 type4
	char Start0; //+129 type4
	char Start1; //+12A type4
	char Start2; //+12B type4
	char Start3; //+12C type4
	char Start4; //+12D type4
	char Start5; //+12E type4
	char Start6; //+12F type4
	char Start7; //+130 type4
	char OrderFlag0; //+131 type4
	char OrderFlag1; //+132 type4
	char OrderFlag2; //+133 type4
	char OrderFlag3; //+134 type4
	char OrderFlag4; //+135 type4
	char OrderFlag5; //+136 type4
	char OrderFlag6; //+137 type4
	char OrderFlag7; //+138 type4
	char EnvEffect; //+139 type4
	char IsDoor; //+13A type4
	char BlocksVis; //+13B type4
	char Orientation; //+13C type4
	char PreOperate; //+13D type4
	char Trans; //+13E type4
	char Mode0; //+13F type4
	char Mode1; //+140 type4
	char Mode2; //+141 type4
	char Mode3; //+142 type4
	char Mode4; //+143 type4
	char Mode5; //+144 type4
	char Mode6; //+145 type4
	char Mode7; //+146 type4
	char pad_147; //+147
	int Xoffset; //+148 type2
	int Yoffset; //+14C type2
	char Draw; //+150 type4
	char HD; //+151 type4
	char TR; //+152 type4
	char LG; //+153 type4
	char RA; //+154 type4
	char LA; //+155 type4
	char RH; //+156 type4
	char LH; //+157 type4
	char SH; //+158 type4
	char S1; //+159 type4
	char S2; //+15A type4
	char S3; //+15B type4
	char S4; //+15C type4
	char S5; //+15D type4
	char S6; //+15E type4
	char S7; //+15F type4
	char S8; //+160 type4
	char TotalPieces; //+161 type4
	char XSpace; //+162 type4
	char YSpace; //+163 type4
	char Red; //+164 type4
	char Green; //+165 type4
	char Blue; //+166 type4
	char SubClass; //+167 type4
	int NameOffset; //+168 type2
	char pad_16C; //+16C
	char MonsterOK; //+16D type4
	char OperateRange; //+16E type4
	char ShrineFunction; //+16F type4
	char Act; //+170 type4
	char Lockable; //+171 type4
	char Gore; //+172 type4
	char Restore; //+173 type4
	char RestoreVirgins; //+174 type4
	char Sync; //+175 type4
	char pad_176[2]; //+176
	int Parm0; //+178 type2
	int Parm1; //+17C type2
	int Parm2; //+180 type2
	int Parm3; //+184 type2
	int Parm4; //+188 type2
	int Parm5; //+18C type2
	int Parm6; //+190 type2
	int Parm7; //+194 type2
	char nTgtFX; //+198 type4
	char nTgtFY; //+199 type4
	char nTgtBX; //+19A type4
	char nTgtBY; //+19B type4
	char Damage; //+19C type4
	char CollisionSubst; //+19D type4
	char pad_19E[2]; //+19E
	int Left; //+1A0 type2
	int Top; //+1A4 type2
	int Width; //+1A8 type2
	int Height; //+1AC type2
	char Beta; //+1B0 type4
	char InitFn; //+1B1 type4
	char PopulateFn; //+1B2 type4
	char OperateFn; //+1B3 type4
	char ClientFn; //+1B4 type4
	char Overlay; //+1B5 type4
	char BlockMissile; //+1B6 type4
	char DrawUnder; //+1B7 type4
	char OpenWarp; //+1B8 type4
	char pad_1B9[3]; //+1B9
	int AutoMap; //+1BC type2
};
//objgroup size=0x34 count=d2common_pWaypoints ret=d2common_3F0AC
struct objgroup_txt {
	int ID0; //+00 type2
	int ID1; //+04 type2
	int ID2; //+08 type2
	int ID3; //+0C type2
	int ID4; //+10 type2
	int ID5; //+14 type2
	int ID6; //+18 type2
	int ID7; //+1C type2
	char DENSITY0; //+20 type4
	char DENSITY1; //+21 type4
	char DENSITY2; //+22 type4
	char DENSITY3; //+23 type4
	char DENSITY4; //+24 type4
	char DENSITY5; //+25 type4
	char DENSITY6; //+26 type4
	char DENSITY7; //+27 type4
	char PROB0; //+28 type4
	char PROB1; //+29 type4
	char PROB2; //+2A type4
	char PROB3; //+2B type4
	char PROB4; //+2C type4
	char PROB5; //+2D type4
	char PROB6; //+2E type4
	char PROB7; //+2F type4
	char SHRINES; //+30 type4
	char WELLS; //+31 type4
	char pad_32[2]; //+32
};
//shrines size=0xB8 count=d2common_pCharTemplateTxt ret=d2common_3EC49
struct shrines_txt {
	char Code; //+00 type4
	char pad_1[3]; //+1
	int Arg0; //+04 type2
	int Arg1; //+08 type2
	int Duration_in_frames; //+0C type2
	char reset_time_in_minutes; //+10 type4
	char rarity; //+11 type4
	char view_name[31]; //+12 type1
	char pad_31; //+31
	char niftyphrase[127]; //+32 type1
	char pad_B1; //+B1
	char effectclass; //+B2 type4
	char pad_B3; //+B3
	int LevelMin; //+B4 type2
};
//inventory size=0xF0 count=d2common_InventoryTxtCount ret=d2common_82CA9
struct inventory_txt {
	int invLeft; //+00 type2
	int invRight; //+04 type2
	int invTop; //+08 type2
	int invBottom; //+0C type2
	char gridX; //+10 type4
	char gridY; //+11 type4
	char pad_12[2]; //+12
	int gridLeft; //+14 type2
	int gridRight; //+18 type2
	int gridTop; //+1C type2
	int gridBottom; //+20 type2
	char gridBoxWidth; //+24 type4
	char gridBoxHeight; //+25 type4
	char pad_26[2]; //+26
	int rArmLeft; //+28 type2
	int rArmRight; //+2C type2
	int rArmTop; //+30 type2
	int rArmBottom; //+34 type2
	char rArmWidth; //+38 type4
	char rArmHeight; //+39 type4
	char pad_3A[2]; //+3A
	int torsoLeft; //+3C type2
	int torsoRight; //+40 type2
	int torsoTop; //+44 type2
	int torsoBottom; //+48 type2
	char torsoWidth; //+4C type4
	char torsoHeight; //+4D type4
	char pad_4E[2]; //+4E
	int lArmLeft; //+50 type2
	int lArmRight; //+54 type2
	int lArmTop; //+58 type2
	int lArmBottom; //+5C type2
	char lArmWidth; //+60 type4
	char lArmHeight; //+61 type4
	char pad_62[2]; //+62
	int headLeft; //+64 type2
	int headRight; //+68 type2
	int headTop; //+6C type2
	int headBottom; //+70 type2
	char headWidth; //+74 type4
	char headHeight; //+75 type4
	char pad_76[2]; //+76
	int neckLeft; //+78 type2
	int neckRight; //+7C type2
	int neckTop; //+80 type2
	int neckBottom; //+84 type2
	char neckWidth; //+88 type4
	char neckHeight; //+89 type4
	char pad_8A[2]; //+8A
	int rHandLeft; //+8C type2
	int rHandRight; //+90 type2
	int rHandTop; //+94 type2
	int rHandBottom; //+98 type2
	char rHandWidth; //+9C type4
	char rHandHeight; //+9D type4
	char pad_9E[2]; //+9E
	int lHandLeft; //+A0 type2
	int lHandRight; //+A4 type2
	int lHandTop; //+A8 type2
	int lHandBottom; //+AC type2
	char lHandWidth; //+B0 type4
	char lHandHeight; //+B1 type4
	char pad_B2[2]; //+B2
	int beltLeft; //+B4 type2
	int beltRight; //+B8 type2
	int beltTop; //+BC type2
	int beltBottom; //+C0 type2
	char beltWidth; //+C4 type4
	char beltHeight; //+C5 type4
	char pad_C6[2]; //+C6
	int feetLeft; //+C8 type2
	int feetRight; //+CC type2
	int feetTop; //+D0 type2
	int feetBottom; //+D4 type2
	char feetWidth; //+D8 type4
	char feetHeight; //+D9 type4
	char pad_DA[2]; //+DA
	int glovesLeft; //+DC type2
	int glovesRight; //+E0 type2
	int glovesTop; //+E4 type2
	int glovesBottom; //+E8 type2
	char glovesWidth; //+EC type4
	char glovesHeight; //+ED type4
	char pad_EE[2]; //+EE
};
//belts size=0x108 count=12F87C ret=d2common_13EDD
struct belts_txt {
	char pad_0[4]; //+0
	char numboxes; //+04 type4
	char pad_5[3]; //+5
	int box1left; //+08 type2
	int box1right; //+0C type2
	int box1top; //+10 type2
	int box1bottom; //+14 type2
	int box2left; //+18 type2
	int box2right; //+1C type2
	int box2top; //+20 type2
	int box2bottom; //+24 type2
	int box3left; //+28 type2
	int box3right; //+2C type2
	int box3top; //+30 type2
	int box3bottom; //+34 type2
	int box4left; //+38 type2
	int box4right; //+3C type2
	int box4top; //+40 type2
	int box4bottom; //+44 type2
	int box5left; //+48 type2
	int box5right; //+4C type2
	int box5top; //+50 type2
	int box5bottom; //+54 type2
	int box6left; //+58 type2
	int box6right; //+5C type2
	int box6top; //+60 type2
	int box6bottom; //+64 type2
	int box7left; //+68 type2
	int box7right; //+6C type2
	int box7top; //+70 type2
	int box7bottom; //+74 type2
	int box8left; //+78 type2
	int box8right; //+7C type2
	int box8top; //+80 type2
	int box8bottom; //+84 type2
	int box9left; //+88 type2
	int box9right; //+8C type2
	int box9top; //+90 type2
	int box9bottom; //+94 type2
	int box10left; //+98 type2
	int box10right; //+9C type2
	int box10top; //+A0 type2
	int box10bottom; //+A4 type2
	int box11left; //+A8 type2
	int box11right; //+AC type2
	int box11top; //+B0 type2
	int box11bottom; //+B4 type2
	int box12left; //+B8 type2
	int box12right; //+BC type2
	int box12top; //+C0 type2
	int box12bottom; //+C4 type2
	int box13left; //+C8 type2
	int box13right; //+CC type2
	int box13top; //+D0 type2
	int box13bottom; //+D4 type2
	int box14left; //+D8 type2
	int box14right; //+DC type2
	int box14top; //+E0 type2
	int box14bottom; //+E4 type2
	int box15left; //+E8 type2
	int box15right; //+EC type2
	int box15top; //+F0 type2
	int box15bottom; //+F4 type2
	int box16left; //+F8 type2
	int box16right; //+FC type2
	int box16top; //+100 type2
	int box16bottom; //+104 type2
};
//monitempercent size=0x4 count=d2common_pExpFieldD2 ret=d2common_5A018
struct monitempercent_txt {
	char HeartPercent; //+00 type4
	char BodyPartPercent; //+01 type4
	char TreasureClassPercent; //+02 type4
	char ComponentPercent; //+03 type4
};
//cubemain size=0x148 count=d2common_A1468 ret=d2common_1D6C6
struct cubemain_txt {
	char enabled; //+00 type4
	//OVERLAP //type23 input_1; //+00 type23 link=d2common_1C490
	//OVERLAP //type23 output; //+00 type23 link=d2common_1C460
	char ladder; //+01 type4
	//OVERLAP //type23 input_2; //+01 type23 link=d2common_1C490
	//OVERLAP //type23 output_b; //+01 type23 link=d2common_1C460
	char min_diff; //+02 type4
	//OVERLAP //type23 input_3; //+02 type23 link=d2common_1C490
	//OVERLAP //type23 output_c; //+02 type23 link=d2common_1C460
	char class; //+03 type13 link=d2common_pPlayerClassLink
	//OVERLAP //type23 input_4; //+03 type23 link=d2common_1C490
	char op; //+04 type4
	//OVERLAP //type23 input_5; //+04 type23 link=d2common_1C490
	//type23 input_6; //+05 type23 link=d2common_1C490
	char pad_5; //+5
	//type23 input_7; //+06 type23 link=d2common_1C490
	char pad_6[2]; //+6
	int param; //+08 type2 link=d2common_1B8E0
	int value; //+0C type2
	char numinputs; //+10 type4
	char pad_11; //+11
	short version; //+12 type3
	char pad_14[65]; //+14
	char lvl; //+55 type4
	char plvl; //+56 type4
	char ilvl; //+57 type4
	char pad_58[12]; //+58
	int mod_1; //+64 type19 link=d2common_pPropertiesLink
	short mod_1_param; //+68 type3
	short mod_1_min; //+6A type3
	short mod_1_max; //+6C type3
	char mod_1_chance; //+6E type4
	char pad_6F; //+6F
	int mod_2; //+70 type19 link=d2common_pPropertiesLink
	short mod_2_param; //+74 type3
	short mod_2_min; //+76 type3
	short mod_2_max; //+78 type3
	char mod_2_chance; //+7A type4
	char pad_7B; //+7B
	int mod_3; //+7C type19 link=d2common_pPropertiesLink
	short mod_3_param; //+80 type3
	short mod_3_min; //+82 type3
	short mod_3_max; //+84 type3
	char mod_3_chance; //+86 type4
	char pad_87; //+87
	int mod_4; //+88 type19 link=d2common_pPropertiesLink
	short mod_4_param; //+8C type3
	short mod_4_min; //+8E type3
	short mod_4_max; //+90 type3
	char mod_4_chance; //+92 type4
	char pad_93; //+93
	int mod_5; //+94 type19 link=d2common_pPropertiesLink
	short mod_5_param; //+98 type3
	short mod_5_min; //+9A type3
	short mod_5_max; //+9C type3
	char mod_5_chance; //+9E type4
	char pad_9F[10]; //+9F
	char b_lvl; //+A9 type4
	char b_plvl; //+AA type4
	char b_ilvl; //+AB type4
	char pad_AC[12]; //+AC
	int b_mod_1; //+B8 type19 link=d2common_pPropertiesLink
	short b_mod_1_param; //+BC type3
	short b_mod_1_min; //+BE type3
	short b_mod_1_max; //+C0 type3
	char b_mod_1_chance; //+C2 type4
	char pad_C3; //+C3
	int b_mod_2; //+C4 type19 link=d2common_pPropertiesLink
	short b_mod_2_param; //+C8 type3
	short b_mod_2_min; //+CA type3
	short b_mod_2_max; //+CC type3
	char b_mod_2_chance; //+CE type4
	char pad_CF; //+CF
	int b_mod_3; //+D0 type19 link=d2common_pPropertiesLink
	short b_mod_3_param; //+D4 type3
	short b_mod_3_min; //+D6 type3
	short b_mod_3_max; //+D8 type3
	char b_mod_3_chance; //+DA type4
	char pad_DB; //+DB
	int b_mod_4; //+DC type19 link=d2common_pPropertiesLink
	short b_mod_4_param; //+E0 type3
	short b_mod_4_min; //+E2 type3
	short b_mod_4_max; //+E4 type3
	char b_mod_4_chance; //+E6 type4
	char pad_E7; //+E7
	int b_mod_5; //+E8 type19 link=d2common_pPropertiesLink
	short b_mod_5_param; //+EC type3
	short b_mod_5_min; //+EE type3
	short b_mod_5_max; //+F0 type3
	char b_mod_5_chance; //+F2 type4
	char pad_F3[10]; //+F3
	char c_lvl; //+FD type4
	char c_plvl; //+FE type4
	char c_ilvl; //+FF type4
	char pad_100[12]; //+100
	int c_mod_1; //+10C type19 link=d2common_pPropertiesLink
	short c_mod_1_param; //+110 type3
	short c_mod_1_min; //+112 type3
	short c_mod_1_max; //+114 type3
	char c_mod_1_chance; //+116 type4
	char pad_117; //+117
	int c_mod_2; //+118 type19 link=d2common_pPropertiesLink
	short c_mod_2_param; //+11C type3
	short c_mod_2_min; //+11E type3
	short c_mod_2_max; //+120 type3
	char c_mod_2_chance; //+122 type4
	char pad_123; //+123
	int c_mod_3; //+124 type19 link=d2common_pPropertiesLink
	short c_mod_3_param; //+128 type3
	short c_mod_3_min; //+12A type3
	short c_mod_3_max; //+12C type3
	char c_mod_3_chance; //+12E type4
	char pad_12F; //+12F
	int c_mod_4; //+130 type19 link=d2common_pPropertiesLink
	short c_mod_4_param; //+134 type3
	short c_mod_4_min; //+136 type3
	short c_mod_4_max; //+138 type3
	char c_mod_4_chance; //+13A type4
	char pad_13B; //+13B
	int c_mod_5; //+13C type19 link=d2common_pPropertiesLink
	short c_mod_5_param; //+140 type3
	short c_mod_5_min; //+142 type3
	short c_mod_5_max; //+144 type3
	char c_mod_5_chance; //+146 type4
	char pad_147; //+147
};
//difficultylevels size=0x58 count=d2common_nDifficultyLevels ret=d2common_6078F
struct difficultylevels_txt {
	int ResistPenalty; //+00 type2
	int DeathExpPenalty; //+04 type2
	int UberCodeOddsNormal; //+08 type2
	int UberCodeOddsGood; //+0C type2
	int MonsterSkillBonus; //+10 type2
	int MonsterFreezeDivisor; //+14 type2
	int MonsterColdDivisor; //+18 type2
	int AiCurseDivisor; //+1C type2
	int UltraCodeOddsNormal; //+20 type2
	int UltraCodeOddsGood; //+24 type2
	int LifeStealDivisor; //+28 type2
	int ManaStealDivisor; //+2C type2
	int UniqueDamageBonus; //+30 type2
	int ChampionDamageBonus; //+34 type2
	int HireableBossDamagePercent; //+38 type2
	int MonsterCEDamagePercent; //+3C type2
	int StaticFieldMin; //+40 type2
	int GambleRare; //+44 type2
	int GambleSet; //+48 type2
	int GambleUnique; //+4C type2
	int GambleUber; //+50 type2
	int GambleUltra; //+54 type2
};

