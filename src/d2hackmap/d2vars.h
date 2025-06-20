#ifndef D2_VARS_H
#define D2_VARS_H

enum ItemQuality{
	ITEM_QUALITY_INVALID	= 0,
	ITEM_QUALITY_LOW		= 1 , 
	ITEM_QUALITY_NORMAL		= 2 , 
	ITEM_QUALITY_SUPERIOR	= 3 , 
	ITEM_QUALITY_MAGIC		= 4 , 
	ITEM_QUALITY_SET		= 5 , 
	ITEM_QUALITY_RARE		= 6 , 
	ITEM_QUALITY_UNIQUE		= 7 , 
	ITEM_QUALITY_CRAFTED	= 8 , 
	ITEM_QUALITY_TAMPERED	= 9 
};

enum ItemFlag{
	ITEMFLAG_IDENTIFIED  =	0x00000010,
	ITEMFLAG_SWITCHIN  =	0x00000040,
	ITEMFLAG_SWITCHOUT =	0x00000080,
	ITEMFLAG_BROKEN =		0x00000100,
	ITEMFLAG_SOCKETED =		0x00000800,
	ITEMFLAG_INSTORE =		0x00002000,
	ITEMFLAG_NAMED =		0x00008000,//for ears, personalized items etc
	ITEMFLAG_ISEAR =		0x00010000,
	ITEMFLAG_INEXPENSIVE =	0x00020000,//always costs 1 for repair /	sell
	ITEMFLAG_COMPACTSAVE =	0x00200000,
	ITEMFLAG_ETHEREAL =		0x00400000,
	ITEM_PERSONALIZED =		0x01000000,
	ITEMFLAG_RUNEWORD =		0x04000000
};

enum UnitNo {
	UNITNO_PLAYER	= 0,
	UNITNO_MONSTER	= 1,
	UNITNO_OBJECT	= 2,
	UNITNO_MISSILE	= 3,
	UNITNO_ITEM		= 4,
	UNITNO_AREA_ENTRANCE	= 5
};


enum UIVar {
	UIVAR_UNK0 =		0,		// always  1
	UIVAR_INVENTORY =	1,		// hotkey 'B'
	UIVAR_STATS =		2,		// hotkey 'C'
	UIVAR_CURRSKILL =	3,		// left or right hand skill selection
	UIVAR_SKILLS =		4,		// hotkey 'T'
	UIVAR_CHATINPUT =	5,		// chat with other players, hotkey ENTER
	UIVAR_NEWSTATS =	6,		// new stats button
	UIVAR_NEWSKILL =	7,		// new skills button
	UIVAR_INTERACT =	8,		// interact with NPC
	UIVAR_GAMEMENU =	9,		// hotkey ESC
	UIVAR_AUTOMAP =		10,		// hotkey TAB
	UIVAR_CFGCTRLS =	11,		// config control key combination
	UIVAR_NPCTRADE =	12,		// trade, game, repair with NPC
	UIVAR_SHOWITEMS =	13,		// hotkey ALT
	UIVAR_MODITEM =		14,		// add socket, imbue item
	UIVAR_QUEST =		15,		// hotkey 'Q'
	UIVAR_UNK16 =		16,
	UIVAR_NEWQUEST =	17,		// quest log button on the bottom left screen
	UIVAR_STATUSAREA =	18,		// lower panel can not redraw when set
	UIVAR_UNK19 =		19,		// init 1
	UIVAR_WAYPOINT =	20,
	UIVAR_MINIPANEL =	21,
	UIVAR_PARTY =		22,		// hotkey 'P'
	UIVAR_PPLTRADE =	23,		// trade, exchange items with other player
	UIVAR_MSGLOG =		24,
	UIVAR_STASH =		25,
	UIVAR_CUBE =		26,
	UIVAR_UNK27 =		27,
	UIVAR_INVENTORY2 =	28,
	UIVAR_INVENTORY3 =	29,
	UIVAR_INVENTORY4 =	30,
	UIVAR_BELT =		31,
	UIVAR_UNK32 =		32,
	UIVAR_HELP =		33,		// help scrren, hotkey 'H'
	UIVAR_UNK34 =		34,
	UIVAR_PARTYHEAD =	35,		// party head 
	UIVAR_PET =			36,		// hotkey 'O'
	UIVAR_QUESTSCROLL = 37,		// for showing quest information when click quest item.
};


enum UnitStat {
	STAT_STRENGTH =				0	,		// str
	STAT_ENERGY =				1	,		// energy
	STAT_DEXTERITY =			2	,		// dexterity
	STAT_VITALITY =				3	,		// vitality
	STAT_STATPOINTSLEFT	=		4	,		// stats point left
	STAT_NEWSKILLS =			5	,		// skill point left
	STAT_HP	=					6	,		// life
	STAT_MAXHP =				7	,		// max life
	STAT_MANA =					8	,		// mana
	STAT_MAXMANA =				9	,		// max mana
	STAT_STAMINA =				10  ,		// stamina
	STAT_MAXSTAMINA	=			11	,		// max stamina
	STAT_LEVEL =				12	,		// level
	STAT_EXP =					13	,		// experience
	STAT_GOLD =					14	,		// gold
	STAT_GOLDBANK =				15	,		// stash gold
	STAT_EnhancedDefense=16,
	STAT_EnhancedDamage=17,
	STAT_EnhancedMinDamage=18,
	STAT_AttackRating=19,
	STAT_TOBLOCK =				20	,		// to block
	STAT_MinDamage=23,
	STAT_MaxDamage=24,
	STATS_MANARECOVERY =		26	,		//
	STAT_LASTEXP =				29	,		//
	STAT_NEXTEXP=30,
	STAT_Defense=31,
	STAT_MAGIC_DAMAGE_REDUCED = 35	,		// damage reduction
	STAT_DAMAGE_REDUCED =		36	,		// magic damage reduction
	STAT_MAGIC_RESIST =			37	,		// magic resist
	STAT_MAX_MAGIC_RESIST =		38	,		// max magic resist
	STAT_FIRE_RESIST =			39	,		// fire resist
	STAT_MAX_FIRE_RESIST =		40	,		// max fire resist
	STAT_LIGHTING_RESIST =		41	,		// lightning resist
	STAT_MAX_LIGHTING_RESIST =	42	,		// max lightning resist
	STAT_COLD_RESIST =			43	,		// cold resist
	STAT_MAX_COLD_RESIST =		44	,		// max cold resist
	STAT_POSION_RESIST =		45	,		// poison resist
	STAT_MAX_POSION_RESIST =	46	,		// max poison resist
	STAT_LIFE_LEECH =			60	,		// Life Leech
	STAT_MANA_LEECH	=			62	,		// Mana Leech
	STAT_VELOCITYPERCENT =		67	,		// effective run/walk
	STAT_AMMOQUANTITY =			70	,		// ammo quantity(arrow/bolt/throwing)
	STAT_DURABILITY	=			72	,		// item durability
	STAT_MAXDURABILITY =		73	,		// max item durability
	STAT_HPREGEN =				74 ,		// hp regen
	STAT_EXTRA_GOLD	=			79	,		// Gold find (GF)
	STAT_MAGIC_FIND =			80	,		// magic find (MF)
	STAT_CLASS_SKILL=83,//
	STAT_Requirement=91,
	STAT_IAS =					93	,		// ias
	STAT_FRW =					96	,		// faster run/walk
	STAT_FHR =					99	,		// faster hit recovery
	STAT_FBR =					102	,		// faster block rate
	STAT_FCR =					105	,		// faster cast rate
	STAT_PLR =					110	, 
	STAT_DTM=114,		// damage taken goes to mana
	STAT_ITD=115,
	STAT_PHM=117,
	STAT_AllSkills=127,
	STAT_OW =					135	, 
	STAT_CB =					136	,		// crushing blow
	STAT_EK =					138	,		// mana after each kill
	STAT_DS =					141	,		//deadly strike 
	STAT_FIRE_ABSORB =			143	,
	STAT_LIGHTING_ABSORB_PERCENT=144,
	STAT_LIGHTING_ABSORB =		145	,
	STAT_COLD_ABSORB =			148	,
	STAT_AURA=151,
	STAT_CLASS_ONLY_SKILL=188,
	STAT_NUMSOCKETS	=			194	,
	STATS_ITEM_HP_PERLEVEL =	216	,
	STAT_ITEMDIFFICULTY	=		356,
};


enum BodyLocation {  
	EQUIP_NONE =			0 ,		// Not equipped
	EQUIP_HEAD =			1 ,		// Head
	EQUIP_AMULET =			2 ,		// Amulet
	EQUIP_BODY =			3 ,		// Body armor
	EQUIP_RIGHT_PRIMARY =	4 ,		// Right primary slot
	EQUIP_LEFT_PRIMARY =	5 ,		// Left primary slot
	EQUIP_RIGHT_RING =		6 ,		// Right ring
	EQUIP_LEFT_RING =		7 ,		// Left ring
	EQUIP_BELT =			8 ,		// Belt
	EQUIP_FEET =			9 ,		// Feet
	EQUIP_GLOVES =			10,		// Gloves
	EQUIP_RIGHT_SECONDARY =	11,		// Right secondary slot
	EQUIP_LEFT_SECONDARY =	12,		// Left secondary slot
};

enum Skills {
Skill_Attack=0,Skill_Kick=1,Skill_Throw=2,
//ama
Skill_MagicArrow=6, Skill_FireArrow=7, Skill_InnerSight=8, Skill_CriticalStrike=9, Skill_Jab=10,
Skill_ColdArrow=11, Skill_MultipleShot=12, Skill_Dodge=13, Skill_PowerStrike=14, Skill_PoisonJavelin=15,
Skill_ExplodingArrow=16, Skill_SlowMissiles=17, Skill_Avoid=18, Skill_Impale=19, Skill_LightningBolt=20,
Skill_IceArrow=21, Skill_GuidedArrow=22, Skill_Penetrate=23, Skill_ChargedStrike=24, Skill_PlagueJavelin=25,
Skill_Strafe=26, Skill_ImmolationArrow=27, Skill_Decoy=28, Skill_Evade=29, Skill_Fend=30,
Skill_FreezingArrow=31, Skill_Valkyrie=32, Skill_Pierce=33, Skill_LightningStrike=34, Skill_LightningFury=35,
//sor
Skill_FireBolt=36, Skill_Warmth=37, Skill_ChargedBolt=38, Skill_IceBolt=39, Skill_FrozenArmor=40,
Skill_Inferno=41, Skill_StaticField=42, Skill_Telekinesis=43, Skill_FrostNova=44, Skill_IceBlast=45,
Skill_Blaze=46, Skill_FireBall=47, Skill_Nova=48, Skill_Lightning=49, Skill_ShiverArmor=50,
Skill_FireWall=51, Skill_Enchant=52, Skill_ChainLightning=53, Skill_Teleport=54, Skill_GlacialSpike=55,
Skill_Meteor=56, Skill_ThunderStorm=57, Skill_EnergyShield=58, Skill_Blizzard=59, Skill_ChillingArmor=60,
Skill_FireMastery=61, Skill_Hydra=62, Skill_LightningMastery=63, Skill_FrozenOrb=64, Skill_ColdMastery=65,
//nec
Skill_AmplifyDamage=66, Skill_Teeth=67, Skill_BoneArmor=68, Skill_SkeletonMastery=69, Skill_RaiseSkeleton=70,
Skill_DimVision=71, Skill_Weaken=72, Skill_PoisonDagger=73, Skill_CorpseExplosion=74, Skill_ClayGolem=75,
Skill_IronMaiden=76, Skill_Terror=77, Skill_BoneWall=78, Skill_GolemMastery=79, Skill_RaiseSkeletalMage=80,
Skill_Confuse=81, Skill_LifeTap=82, Skill_PoisonExplosion=83, Skill_BoneSpear=84, Skill_BloodGolem=85,
Skill_Attract=86, Skill_Decrepify=87, Skill_BonePrison=88, Skill_SummonResist=89, Skill_IronGolem=90,
Skill_LowerResist=91, Skill_PoisonNova=92, Skill_BoneSpirit=93, Skill_FireGolem=94, Skill_Revive=95,
//pal
Skill_Sacrifice=96, Skill_Smite=97, Skill_Might=98, Skill_Prayer=99, Skill_ResistFire=100,
Skill_HolyBolt=101, Skill_HolyFire=102, Skill_Thorns=103, Skill_Defiance=104, Skill_ResistCold=105,
Skill_Zeal=106, Skill_Charge=107, Skill_BlessedAim=108, Skill_Cleansing=109, Skill_ResistLightning=110,
Skill_Vengeance=111, Skill_BlessedHammer=112, Skill_Concentration=113, Skill_HolyFreeze=114, Skill_Vigor=115,
Skill_Conversion=116, Skill_HolyShield=117, Skill_HolyShock=118, Skill_Sanctuary=119, Skill_Meditation=120,
Skill_FistoftheHeavens=121, Skill_Fanaticism=122, Skill_Conviction=123, Skill_Redemption=124, Skill_Salvation=125,
//bar
Skill_Bash=126, Skill_SwordMastery=127, Skill_AxeMastery=128, Skill_MaceMastery=129, Skill_Howl=130,
Skill_FindPotion=131, Skill_Leap=132, Skill_DoubleSwing=133, Skill_PoleArmMastery=134, Skill_ThrowingMastery=135,
Skill_SpearMastery=136, Skill_Taunt=137, Skill_Shout=138, Skill_Stun=139, Skill_DoubleThrow=140,
Skill_IncreasedStamina=141, Skill_FindItem=142, Skill_LeapAttack=143, Skill_Concentrate=144, Skill_IronSkin=145,
Skill_BattleCry=146, Skill_Frenzy=147, Skill_IncreasedSpeed=148, Skill_BattleOrders=149, Skill_GrimWard=150,
Skill_Whirlwind=151, Skill_Berserk=152, Skill_NaturalResistance=153, Skill_WarCry=154, Skill_BattleCommand=155,
//dru
Skill_Raven=221, Skill_PoisonCreeper=222, Skill_Werewolf=223, Skill_Lycanthropy=224, Skill_Firestorm=225,
Skill_OakSage=226, Skill_SummonSpiritWolf=227, Skill_Werebear=228, Skill_MoltenBoulder=229, Skill_ArcticBlast=230,
Skill_CarrionVine=231, Skill_FeralRage=232, Skill_Maul=233, Skill_Fissure=234, Skill_CycloneArmor=235,
Skill_HeartofWolverine=236, Skill_SummonDireWolf=237, Skill_Rabies=238, Skill_FireClaws=239, Skill_Twister=240,
Skill_SolarCreeper=241, Skill_Hunger=242, Skill_ShockWave=243, Skill_Volcano=244, Skill_Tornado=245,
Skill_SpiritofBarbs=246, Skill_SummonGrizzly=247, Skill_Fury=248, Skill_Armageddon=249, Skill_Hurricane=250,
//asn
Skill_FireBlast=251, Skill_ClawMastery=252, Skill_PsychicHammer=253, Skill_TigerStrike=254, Skill_DragonTalon=255,
Skill_ShockWeb=256, Skill_BladeSentinel=257, Skill_BurstofSpeed=258, Skill_FistsofFire=259, Skill_DragonClaw=260,
Skill_ChargedBoltSentry=261, Skill_WakeofFire=262, Skill_WeaponBlock=263, Skill_CloakofShadows=264,
Skill_CobraStrike=265, Skill_BladeFury=266, Skill_Fade=267, Skill_ShadowWarrior=268, Skill_ClawsofThunder=269,
Skill_DragonTail=270, Skill_LightningSentry=271, Skill_WakeofInferno=272, Skill_MindBlast=273,
Skill_BladesofIce=274, Skill_DragonFlight=275, Skill_DeathSentry=276, Skill_BladeShield=277,
Skill_Venom=278, Skill_ShadowMaster=279, Skill_PhoenixStrike=280,
};
enum PlayerMode {
	PlayerMode_Death=0, PlayerMode_Neutral=1, PlayerMode_Walk=2, PlayerMode_Run=3,
	PlayerMode_GetHit=4, PlayerMode_TownNeutral=5, PlayerMode_TownWalk=6, PlayerMode_Attacking1=7,
	PlayerMode_Attacking2=8, PlayerMode_Block=9, PlayerMode_Cast=10, PlayerMode_Throw=11,
	PlayerMode_Kick=12, PlayerMode_Skill1=13, PlayerMode_Skill2=14, PlayerMode_Skill3=15,
	PlayerMode_Skill4=16, PlayerMode_Dead=17, PlayerMode_Sequence=18, PlayerMode_KnockBack=19,
};
enum MonsterMode {
	MonsterMode_Death=0, MonsterMode_Neutral=1, MonsterMode_Walk=2, MonsterMode_GetHit=3,
	MonsterMode_Attack1=4, MonsterMode_Attack2=5, MonsterMode_Block=6, MonsterMode_Cast=7,
	MonsterMode_Skill1=8, MonsterMode_Skill2=9, MonsterMode_Skill3=10, MonsterMode_Skill4=11,
	MonsterMode_Dead=12, MonsterMode_Knockback=13, MonsterMode_Sequence=14, MonsterMode_Run=15,
};
enum ItemMode {
	ItemMode_Inventory=0,
	ItemMode_OnBody=1,
	ItemMode_OnBelt=2,
	ItemMode_OnGound=3,
};
enum UnitState {
	State_Freeze=1, State_Poison=2, State_ResistFire=3, State_ResistCold=4, State_ResistLightning=5,
	State_ResistMagic=6, State_PlayerBody=7, State_Salvation=8, State_AmplifyDamage=9, State_FrozenArmor=10,
	State_ColdSlow=11, State_Inferno=12, State_Blaze=13, State_BoneArmor=14, State_Concentrate=15,
	State_Enchant=16, State_InnerSight=17, State_SkillMove=18, State_Weaken=19, State_ChillingArmor=20,
	State_Stun=21, State_Spiderlay=22, State_DimVision=23, State_Slowed=24, State_Fetishaura=25,
	State_Shout=26, State_Taunt=27, State_Conviction=28, State_Convicted=29, State_Energyshield=30,
	State_Venomclaws=31, State_BattleOrders=32, State_Might=33, State_Prayer=34, State_HolyFire=35,
	State_Thorns=36, State_Defiance=37, State_Thunderstorm=38, State_Lightningbolt=39, State_BlessedAim=40,
	State_Vigor=41,//stamina?
	State_Concentration=42,
	State_HolyFreeze=43,//holywind?
	State_Holywindcold=44,
	State_Cleansing=45,
	State_HolyShock=46, State_Sanctuary=47, State_Meditation=48, State_Fanaticism=49, State_Redemption=50,
	State_BattleCommand=51, State_Preventheal=52, State_Conversion=53, State_Uninterruptable=54,State_IronMaiden=55,
	State_Terror=56, State_Attract=57, State_LifeTap=58, State_Confuse=59, State_Decrepify=60,
	State_LowerRes=61, State_OpenWound=62, State_dopplezon=63, State_criticalstrike=64, State_dodge=65,
	State_avoid=66, State_penetrate=67, State_evade=68, State_pierce=69, State_warmth=70,
	State_firemastery=71, State_lightningmastery=72, State_coldmastery=73, State_swordmastery=74, State_axemastery=75,
	State_macemastery=76, State_polearmmastery=77, State_throwingmastery=78, State_spearmastery=79, State_increasedstamina=80,
	State_ironskin=81, State_increasedspeed=82, State_naturalresistance=83, State_fingermagecurse=84, State_nomanaregen=85,
	State_justhit=86, State_slowmissiles=87, State_ShiverArmor=88, State_battlecry=89, State_blue=90,
	State_red=91, State_death_delay=92, State_valkyrie=93, State_frenzy=94, State_berserk=95,
	State_revive=96, State_skel_mastery=97, State_sourceunit=98, State_redeemed=99, State_HealingPotion=100,
	State_HolyShield=101, State_just_portaled=102, State_monfrenzy=103, State_corpse_nodraw=104, State_alignment=105,
	State_ManaPotion=106, State_shatter=107, State_sync_warped=108, State_conversion_save=109, State_pregnant=110,
	State_golem_mastery=111, State_rabies=112, State_defense_curse=113, State_blood_mana=114, State_burning=115,
	State_dragonflight=116, State_maul=117, State_corpse_noselect=118, State_shadowwarrior=119, State_feralrage=120,
	State_skilldelay=121,
	State_progressive_damage=122,
	State_progressive_steal=123,
	State_progressive_other=124,
	State_progressive_fire=125,
	State_progressive_cold=126,
	State_progressive_lightning=127,
	State_shrine_armor=128,
	State_shrine_combat=129,
	State_shrine_resist_lightning=130,
	State_shrine_resist_fire=131,
	State_shrine_resist_cold=132,
	State_shrine_resist_poison=133,
	State_ShrineSkill=134,
	State_shrine_mana_regen=135,
	State_shrine_stamina=136,
	State_ShrineExp=137,
	State_fenris_rage=138,
	State_wolf=139,
	State_bear=140,
	State_bloodlust=141, State_changeclass=142, State_attached=143, State_hurricane=144, State_armageddon=145,
	State_invis=146, State_barbs=147, State_wolverine=148, State_OakSage=149, State_vine_beast=150,
	State_cyclonearmor=151, State_clawmastery=152, State_cloak_of_shadows=153, State_recycled=154, State_weaponblock=155,
	State_cloaked=156, State_quickness=157, State_bladeshield=158, State_Fade=159, State_summonresist=160,
	State_oaksagecontrol=161, State_wolverinecontrol=162, State_barbscontrol=163, State_debugcontrol=164, State_itemset1=165,
	State_itemset2=166, State_itemset3=167, State_itemset4=168, State_itemset5=169, State_itemset6=170,
	State_runeword=171, State_restinpeace=172, State_corpseexp=173, State_whirlwind=174, State_fullsetgeneric=175,
	State_monsterset=176, State_delerium=177, State_antidote=178, State_thawing=179, State_staminapot=180,
	State_passive_resistfire=181, State_passive_resistcold=182, State_passive_resistltng=183, State_uberminion=184,
};
enum Level_No {
	Level_RogueEncampment=1, Level_BloodMoor=2, Level_ColdPlains=3, Level_StonyField=4,
	Level_DarkWood=5, Level_BlackMarsh=6, Level_TamoeHighland=7, Level_DenofEvil,DenEvil=8,
	Level_CaveLevel1=9, Level_UndergroundPassageLevel1=10, Level_HoleLevel1=11, Level_PitLevel1=12,
	Level_CaveLevel2=13, Level_UndergroundPassageLevel2=14, Level_HoleLevel2=15, Level_PitLevel2=16,
	Level_BurialGrounds=17, Level_Crypt=18, Level_Mausoleum=19, Level_ForgottenTower=20,
	Level_TowerCellarLevel1=21, Level_TowerCellarLevel2=22, Level_TowerCellarLevel3=23, Level_TowerCellarLevel4=24,
	Level_TowerCellarLevel5=25, Level_MonasteryGate=26, Level_OuterCloister=27, Level_Barracks=28,
	Level_JailLevel1=29, Level_JailLevel2=30, Level_JailLevel3=31, Level_InnerCloister=32,
	Level_Cathedral=33, Level_CatacombsLevel1=34, Level_CatacombsLevel2=35, Level_CatacombsLevel3=36,
	Level_CatacombsLevel4=37, Level_Tristram=38, Level_TheSecretCowLevel=39, Level_LutGholein=40,
	Level_RockyWaste=41, Level_DryHills=42, Level_FarOasis=43, Level_LostCity=44,
	Level_ValleyofSnakes=45, Level_CanyonoftheMagi=46, Level_SewersLevel1=47,
	Level_SewersLevel2=48, Level_SewersLevel3=49, Level_HaremLevel1=50,
	Level_HaremLevel2=51, Level_PalaceCellarLevel1=52, Level_PalaceCellarLevel2=53, Level_PalaceCellarLevel3=54,
	Level_StonyTombLevel1=55, Level_HallsoftheDeadLevel1=56,
	Level_HallsoftheDeadLevel2=57, Level_ClawViperTempleLevel1=58,
	Level_StonyTombLevel2=59, Level_HallsoftheDeadLevel3=60,
	Level_ClawViperTempleLevel2=61, Level_MaggotLairLevel1=62, Level_MaggotLairLevel2=63, Level_MaggotLairLevel3=64,
	Level_AncientTunnels=65, Level_TalRashaTomb1=66, Level_TalRashaTomb2=67, Level_TalRashaTomb3=68,
	Level_TalRashaTomb4=69, Level_TalRashaTomb5=70, Level_TalRashaTomb6=71, Level_TalRashaTomb7=72,
	Level_TalRashaChamber=73, Level_ArcaneSanctuary=74, Level_KurastDocks=75, Level_SpiderForest=76,
	Level_GreatMarsh=77, Level_FlayerJungle=78, Level_LowerKurast=79, Level_KurastBazaar=80,
	Level_UpperKurast=81, Level_KurastCauseway=82, Level_Travincal=83, Level_ArachnidLair=84,
	Level_SpiderCavern=85, Level_SwampyPitLevel1=86, Level_SwampyPitLevel2=87, Level_FlayerDungeonLevel1=88,
	Level_FlayerDungeonLevel2=89, Level_SwampyPitLevel3=90, Level_FlayerDungeonLevel3=91, Level_KurastSewersLevel1=92,
	Level_KurastSewersLevel2=93, Level_RuinedTemple=94, Level_DisusedFane=95, Level_ForgottenReliquary=96,
	Level_ForgottenTemple=97, Level_RuinedFane=98, Level_DisusedReliquary=99, Level_DuranceofHateLevel1=100,
	Level_DuranceofHateLevel2=101, Level_DuranceofHateLevel3=102, Level_ThePandemoniumFortress=103,
	Level_OuterSteppes=104, Level_PlainsofDespair=105, Level_CityoftheDamned=106,
	Level_RiverofFlame=107, Level_ChaosSanctuary=108, Level_Harrogath=109, Level_BloodyFoothills=110,
	Level_FrigidHighlands=111, Level_ArreatPlateau=112, Level_CrystallinePassage=113, Level_FrozenRiver=114,
	Level_GlacialTrail=115, Level_DrifterCavern=116, Level_FrozenTundra=117, Level_AncientsWay=118,
	Level_IcyCellar=119, Level_ArreatSummit=120, Level_NihlathaksTemple=121, Level_HallsofAnguish=122,
	Level_HallsofPain=123, Level_HallsofVaught=124, Level_Abaddon=125, Level_PitofAcheron=126,
	Level_InfernalPit=127, Level_WorldstoneKeepLevel1=128, Level_WorldstoneKeepLevel2=129,
	Level_WorldstoneKeepLevel3=130, Level_ThroneofDestruction=131, Level_TheWorldstoneChamber=132,
	Level_FurnaceofPain=133, Level_ForgottenSands=134, Level_MatronDen=135, Level_Tristram6boss=136,
};
enum MonsterTxtNo {
Mon_Skeleton=0, Mon_Returned1=1, Mon_BoneWarrior1=2, Mon_BurningDead=3, Mon_Horror=4, Mon_Zombie=5,
Mon_HungryDead=6, Mon_Ghoul=7, Mon_DrownedCarcass=8, Mon_PlagueBearer1=9, Mon_Afflicted1=10,
Mon_Tainted1=11, Mon_Misshapen1=12, Mon_Disfigured1=13, Mon_Damned1=14, Mon_FoulCrow1=15,
Mon_BloodHawk1=16, Mon_BlackRaptor1=17, Mon_CloudStalker1=18, Mon_Fallen=19, Mon_Carver1=20,
Mon_Devilkin1=21, Mon_DarkOne1=22, Mon_WarpedOne=23, Mon_Brute=24, Mon_Yeti=25, Mon_Crusher=26,
Mon_WailingBeast=27, Mon_GargantuanBeast=28, Mon_SandRaider1=29, Mon_Marauder1=30, Mon_Invader1=31,
Mon_Infidel1=32, Mon_Assailant1=33, Mon_unusedGorgon=34, Mon_unusedStoneStalker=35, Mon_unusedSerpentQueen=36,
Mon_unusedStygianWatcher=37, Mon_Ghost1=38, Mon_Wraith1=39, Mon_Specter1=40, Mon_Apparition=41,
Mon_DarkShape=42, Mon_DarkHunter=43, Mon_VileHunter=44, Mon_DarkStalker=45, Mon_BlackRogue=46,
Mon_FleshHunter=47, Mon_DuneBeast=48, Mon_RockDweller=49, Mon_JungleHunter=50, Mon_DoomApe=51,
Mon_TempleGuard1=52, Mon_MoonClan1=53, Mon_NightClan1=54, Mon_BloodClan1=55, Mon_HellClan1=56,
Mon_DeathClan1=57, Mon_FallenShaman=58, Mon_CarverShaman1=59, Mon_DevilkinShaman1=60, Mon_DarkShaman1=61,
Mon_WarpedShaman=62, Mon_QuillRat1=63, Mon_SpikeFiend1=64, Mon_ThornBeast=65, Mon_RazorSpine1=66,
Mon_BushBarb=67, Mon_SandMaggot=68, Mon_RockWorm=69, Mon_Devourer=70, Mon_GiantLamprey=71, Mon_BloodMaggot1=72,
Mon_TombViper1=73, Mon_ClawViper1=74, Mon_Salamander1=75, Mon_PitViper1=76, Mon_SerpentMagus1=77,
Mon_SandLeaper=78, Mon_CaveLeaper1=79, Mon_TombCreeper1=80, Mon_TreeLurker=81, Mon_CliffLurker=82,
Mon_Huntress1=83, Mon_SaberCat1=84, Mon_NightTiger=85, Mon_HellCat=86, Mon_Itchies=87, Mon_BlackLocusts=88,
Mon_PlagueBugs=89, Mon_HellSwarm=90, Mon_DungSoldier=91, Mon_DeathBeetle=92, Mon_Scarab1=93,
Mon_SteelScarab1=94, Mon_BoneScarab=95, Mon_DriedCorpse=96, Mon_Decayed=97, Mon_Embalmed=98,
Mon_PreservedDead=99, Mon_Cadaver1=100, Mon_HollowOne=101, Mon_Guardian1=102, Mon_Unraveler1=103,
Mon_HoradrimAncient1=104, Mon_BaalUnraveler=105, Mon_unusedDamnedHorde=106, Mon_unusedTwistedHorde=107,
Mon_unusedWickedHorde=108, Mon_unusedUnholyHorde=109, Mon_CarrionBird1=110, Mon_UndeadScavenger=111,
Mon_HellBuzzard=112, Mon_WingedNightmare=113, Mon_Sucker=114, Mon_Feeder=115, Mon_BloodHook=116,
Mon_BloodWing=117, Mon_Gloam1=118, Mon_SwampGhost=119, Mon_BurningSoul1=120, Mon_BlackSoul1=121,
Mon_Arach1=122, Mon_SandFisher=123, Mon_PoisonSpinner=124, Mon_FlameSpider=125, Mon_SpiderMagus=126,
Mon_ThornedHulk1=127, Mon_BrambleHulk=128, Mon_Thrasher=129, Mon_Spikefist=130, Mon_GhoulLord1=131,
Mon_NightLord1=132, Mon_DarkLord1=133, Mon_BloodLord1=134, Mon_TheBanished=135, Mon_DesertWing=136,
Mon_Fiend1=137, Mon_Gloombat1=138, Mon_BloodDiver=139, Mon_DarkFamiliar=140, Mon_RatMan1=141,
Mon_Fetish1=142, Mon_Flayer1=143, Mon_SoulKiller1=144, Mon_StygianDoll1=145, Mon_DeckardCain1=146,
Mon_Gheed=147, Mon_Akara=148, Mon_dummychicken=149, Mon_Kashya=150, Mon_dummyrat=151,
Mon_dummyRogueTown=152, Mon_dummyHellMeteor=153, Mon_Charsi=154, Mon_Warriv1=155,
Mon_Andariel=156, Mon_dummySmallbird=157, Mon_dummyLargebird=158, Mon_dummyBat=159,
Mon_DarkRanger=160, Mon_VileArcher1=161, Mon_DarkArcher1=162, Mon_BlackArcher=163, Mon_FleshArcher=164,
Mon_DarkSpearwoman=165, Mon_VileLancer1=166, Mon_DarkLancer1=167, Mon_BlackLancer1=168, Mon_FleshLancer=169,
Mon_SkeletonArcher=170, Mon_ReturnedArcher1=171, Mon_BoneArcher1=172, Mon_BurningDeadArcher1=173,
Mon_HorrorArcher1=174, Mon_Warriv2=175, Mon_Atma=176, Mon_Drognan=177, Mon_Fara=178, Mon_dummyCow=179,
Mon_SandMaggotYoung=180, Mon_RockWormYoung=181, Mon_DevourerYoung=182, Mon_GiantLampreyYoung=183,
Mon_BloodMaggotYoung1=184, Mon_dummyCamel=185, Mon_Blunderbore1=186, Mon_Gorebelly=187, Mon_Mauler1=188,
Mon_Urdar=189, Mon_SandMaggotEgg=190, Mon_RockWormEgg=191, Mon_DevourerEgg=192, Mon_GiantLampreyEgg=193,
Mon_BloodMaggotEgg1=194, Mon_dummyAct2Male=195, Mon_dummyAct2Female=196, Mon_dummyAct2Child=197, Mon_Greiz=198,
Mon_Elzix=199, Mon_Geglash=200, Mon_Jerhyn=201, Mon_Lysander=202, Mon_dummyAct2Guard=203, Mon_dummyAct2Vendor1=204,
Mon_dummyAct2Vendor2=205, Mon_FoulCrowNest=206, Mon_BloodHawkNest=207, Mon_BlackRaptorNest=208,
Mon_CloudStalkerNest=209, Mon_Meshif1=210, Mon_Duriel=211, Mon_UndeadRatMan=212, Mon_UndeadFetish=213,
Mon_UndeadFlayer=214, Mon_UndeadSoulKiller1=215, Mon_UndeadStygianDoll1=216, Mon_unusedDarkGuard=217,
Mon_unusedDarkKnight=218, Mon_unusedBloodGuard=219, Mon_unusedBloodKnight=220, Mon_unusedDarkPaladin=221,
Mon_unusedBloodMage=222, Mon_unusedDemonist=223, Mon_unusedBlackMagus=224, Mon_unusedDiabolist=225,
Mon_unusedDeathMage=226, Mon_Maggot=227, Mon_MummySarcophagus=228, Mon_Radament=229,
Mon_unusedFireBeast=230, Mon_unusedIceGlobe=231, Mon_unusedLightningBeast=232, Mon_unusedPoisonOrb=233,
Mon_FlyingScimitar=234, Mon_Zakarumite=235, Mon_Faithful=236, Mon_Zealot1=237,
Mon_Sexton=238, Mon_Cantor=239, Mon_Heirophant1=240, Mon_Heirophant2=241, Mon_Mephisto=242,
Mon_Diablo=243, Mon_DeckardCain2=244, Mon_DeckardCain3=245, Mon_DeckardCain4=246, Mon_SwampDweller=247,
Mon_BogCreature=248, Mon_SlimePrince=249, Mon_TheSummoner=250, Mon_Tyrael=251, Mon_Asheara=252,
Mon_Hratli=253, Mon_Alkor=254, Mon_Ormus=255, Mon_Izual=256, Mon_Halbu=257, Mon_WaterWatcherLimb=258,
Mon_RiverStalkerLimb=259, Mon_StygianWatcherLimb=260, Mon_WaterWatcherHead=261, Mon_RiverStalkerHead=262,
Mon_StygianWatcherHead=263, Mon_Meshif2=264, Mon_DeckardCain5=265, Mon_Flavie=266, Mon_BloodRaven=267,
Mon_dummybug=268, Mon_dummyscorpion=269, Mon_RogueScout=270, Mon_Act1Hireling=271,
Mon_dummyRogueTownShoot=272, Mon_GargoyleTrap=273, Mon_ReturnedMage1=274, Mon_BoneMage1=275,
Mon_BurningDeadMage1=276, Mon_HorrorMage1=277, Mon_RatManShaman=278, Mon_FetishShaman=279, Mon_FlayerShaman1=280,
Mon_SoulKillerShaman1=281, Mon_StygianDollShaman1=282, Mon_Larva=283, Mon_SandMaggotQueen=284,
Mon_RockWormQueen=285, Mon_DevourerQueen=286, Mon_GiantLampreyQueen=287, Mon_BloodMaggotQueen=288,
Mon_ClayGolem=289, Mon_BloodGolem=290, Mon_IronGolem=291, Mon_FireGolem=292, Mon_dummyFamiliar=293,
Mon_dummyAct3Male=294, Mon_NightMarauder=295, Mon_dummyAct3Female=296, Mon_Natalya=297, Mon_FleshSpawner1=298,
Mon_StygianHag=299, Mon_Grotesque1=300, Mon_FleshBeast1=301, Mon_StygianDog=302, Mon_GrotesqueWyrm1=303,
Mon_DoomCaster=304, Mon_Strangler1=305, Mon_StormCaster1=306, Mon_Corpulent=307, Mon_CorpseSpitter=308,
Mon_MawFiend1=309, Mon_DoomKnight1=310, Mon_AbyssKnight=311, Mon_OblivionKnight1=312, Mon_QuillBear=313,
Mon_SpikeGiant=314, Mon_ThornBrute=315, Mon_RazorBeast=316, Mon_BarbedGiant=317, Mon_dummySnake=318,
Mon_dummyParrot=319, Mon_dummyFish=320, Mon_dummyEvilHole1=321, Mon_dummyEvilHole2=322, Mon_dummyEvilHole3=323,
Mon_dummyEvilHole4=324, Mon_dummyEvilHole5=325, Mon_trapfirebolt1=326, Mon_trapunknown1=327, Mon_trapunknown2=328,
Mon_trappoison=329, Mon_trapchainlightning=330, Mon_Kaelan=331, Mon_dummyInvisoSpawner=332,
Mon_unusedDiabloClone=333, Mon_SuckerNest=334, Mon_FeederNest=335, Mon_BloodHookNest=336, Mon_BloodWingNest=337,
Mon_Act2Hireling=338, Mon_dummyMiniSpider=339, Mon_PrisonFL=340, Mon_PrisonFR=341, Mon_PrisonBL=342,
Mon_PrisonBR=343, Mon_Bonewall=344, Mon_CouncilMember1=345, Mon_CouncilMember2=346, Mon_CouncilMember3=347,
Mon_Turret1=348, Mon_Turret2=349, Mon_Turret3=350, Mon_Hydra1=351, Mon_Hydra2=352, Mon_Hydra3=353,
Mon_trappillar=354, Mon_dummyseventombs=355, Mon_Decoy=356, Mon_Valkyrie=357, Mon_dummyAct2Hireling=358,
Mon_Act3Hireling=359, Mon_Balrog1=360, Mon_PitLord1=361, Mon_VenomLord=362, Mon_Necroskeleton=363,
Mon_Necromage=364, Mon_Griswold=365, Mon_CompellingOrb=366, Mon_Tyrael1=367, Mon_DarkWanderer=368,
Mon_trapnova1=369, Mon_dummySpiritMummy=370, Mon_LightningSpire=371, Mon_FireTower=372, Mon_Slinger1=373,
Mon_SpearCat1=374, Mon_NightSlinger1=375, Mon_HellSlinger=376, Mon_dummyBobaFett=377, Mon_dummyDarthMaul=378,
Mon_ReturnedMage2=379, Mon_BoneMage2=380, Mon_DeathMage=381, Mon_HorrorMage2=382, Mon_ReturnedMage3=383,
Mon_BoneMage3=384, Mon_BurningDeadMage2=385, Mon_HorrorMage3=386, Mon_ReturnedMage4=387, Mon_BoneMage4=388,
Mon_BurningDeadMage3=389, Mon_HorrorMage4=390, Mon_HellBovine=391, Mon_windowright=392, Mon_windowleft=393,
Mon_SpearCat2=394, Mon_NightSlinger2=395, Mon_RatMan2=396, Mon_Fetish2=397, Mon_Flayer2=398, Mon_SoulKiller2=399,
Mon_StygianDoll2=400, Mon_dummyMephistoSpirit=401, Mon_TheSmith=402, Mon_TrappedSoul1=403, Mon_TrappedSoul2=404,
Mon_Jamella=405, Mon_IzualsGhost=406, Mon_RatMan3=407, Mon_Hadriel=408, Mon_HephastoTheArmorer=409,
Mon_WakeofFire=410, Mon_ChargedBoltSentry=411, Mon_LightningSentry=412, Mon_BladeSentinel=413, Mon_InvisPet=414,
Mon_InfernoSentry=415, Mon_DeathSentry=416, Mon_ShadowWarrior=417, Mon_ShadowMaster=418, Mon_Raven=419,
Mon_SpiritWolf=420, Mon_DireWolf=421, Mon_SpiritofBarbs=422, Mon_HeartofWolverine=423, Mon_OakSage=424,
Mon_PlaguePoppy=425, Mon_CycleofLife=426, Mon_VineCreature=427, Mon_Grizzly=428, Mon_Eagle=429, Mon_Wolf=430,
Mon_Bear=431, Mon_BarricadedDoor1=432, Mon_BarricadedDoor2=433, Mon_PrisonDoor=434, Mon_BarricadedTower=435,
Mon_RotWalker=436, Mon_ReanimatedHorde=437, Mon_ProwlingDead=438, Mon_UnholyCorpse1=439, Mon_DefiledWarrior=440,
Mon_SiegeBeast=441, Mon_CrushBeast=442, Mon_BloodBringer=443, Mon_GoreBearer=444, Mon_DemonSteed=445,
Mon_SnowDrifter=446, Mon_Abominable=447, Mon_ChilledFroth=448, Mon_FrozenAbyss=449, Mon_FallenWolfrider=450,
Mon_CarverWolfrider=451, Mon_DevilkinWolfrider=452, Mon_Enslaved=453, Mon_Slayer1=454, Mon_IceBoar=455,
Mon_FireBoar=456, Mon_HellSpawn1=457, Mon_IceSpawn=458, Mon_GreaterHellSpawn1=459, Mon_GreaterIceSpawn=460,
Mon_FanaticEnslaved=461, Mon_BerserkerSlayer=462, Mon_ConsumedIceBoar=463, Mon_ConsumedFireBoar=464,
Mon_FrenziedHellSpawn=465, Mon_FrenziedIceSpawn=466, Mon_InsaneHellSpawn=467, Mon_InsaneIceSpawn=468,
Mon_Succubus1=469, Mon_VileTemptress=470, Mon_StygianHarlot=471, Mon_HellTemptress1=472, Mon_BloodTemptress=473,
Mon_Siren1=474, Mon_VileWitch1=475, Mon_StygianFury=476, Mon_BloodWitch=477, Mon_HellWitch1=478,
Mon_Overseer=479, Mon_Lasher=480, Mon_Overlord=481, Mon_BloodBoss=482, Mon_HellWhip=483, Mon_DemonPortal1=484,
Mon_DemonPortal2=485, Mon_DemonPortal3=486, Mon_DemonPortal4=487, Mon_DemonPortal5=488, Mon_DemonPortal6=489,
Mon_DemonPortal7=490, Mon_DemonPortal8=491, Mon_DemonImp1=492, Mon_DemonRascal=493, Mon_DemonGremlin=494,
Mon_DemonTrickster1=495, Mon_DemonSprite=496, Mon_CatapultS=497, Mon_CatapultE=498, Mon_CatapultSiege=499,
Mon_CatapultW=500, Mon_FrozenCreeper=501, Mon_FrozenTerror=502, Mon_FrozenScourge=503, Mon_FrozenHorror=504,
Mon_FrozenScorch=505, Mon_MoonLord1=506, Mon_NightLord2=507, Mon_BloodLord2=508, Mon_HellLord1=509,
Mon_DeathLord=510, Mon_Larzuk=511, Mon_AnyaTown=512, Mon_Malah=513, Mon_NihlathakTown=514, Mon_QualKehk=515,
Mon_CatapultSpotterS=516, Mon_CatapultSpotterE=517, Mon_CatapultSpotterSiege=518, Mon_CatapultSpotterW=519,
Mon_DeckardCain6=520, Mon_Tyrael2=521, Mon_BarbarianFighter1=522, Mon_BarbarianFighter2=523,
Mon_BarricadeWallRight=524, Mon_BarricadeWallLeft=525, Mon_Nihlathak=526, Mon_Anya=527, Mon_EvilDemonHut=528,
Mon_DeathMauler1=529, Mon_DeathBrawler=530, Mon_DeathSlasher=531, Mon_DeathBerserker=532, Mon_DeathBrigadier=533,
Mon_BarbarianCaptive=534, Mon_Act5Townguard1=535, Mon_Act5Townguard2=536, Mon_TalicStatue=537, Mon_MadawcStatue=538,
Mon_KorlicStatue=539, Mon_Talic=540, Mon_Madawc=541, Mon_Korlic=542, Mon_BaalThrone=543, Mon_BaalCrab=544,
Mon_BaalTaunt=545, Mon_PutridDefiler=546, Mon_WretchedDefiler=547, Mon_FetidDefiler=548, Mon_RancidDefiler=549,
Mon_RankDefiler=550, Mon_PainWorm=551, Mon_TormentWorm=552, Mon_AgonyWorm=553, Mon_MenaceWorm=554,
Mon_AnguishWorm=555, Mon_Bunny=556, Mon_BaalCouncilMember=557, Mon_BaalVenomLord=558, Mon_BaalCrabtoStairs=559,
Mon_Act5Hireling1hs=560, Mon_Act5Hireling=561, Mon_FesteringAppendages1=562, Mon_FesteringAppendages2=563,
Mon_FesteringAppendages3=564, Mon_FesteringAppendages4=565, Mon_FesteringAppendages5=566, Mon_InjuredBarbarian1=567,
Mon_InjuredBarbarian2=568, Mon_InjuredBarbarian3=569, Mon_BaalCrabClone=570, Mon_MinionofDestruction1=571,
Mon_MinionofDestruction2=572, Mon_MinionofDestruction3=573, Mon_WorldstoneEffect=574, Mon_BurningDeadArcher2=575,
Mon_BoneArcher2=576, Mon_BurningDeadArcher3=577, Mon_ReturnedArcher2=578, Mon_HorrorArcher2=579,
Mon_Afflicted2=580, Mon_Tainted2=581, Mon_Misshapen2=582, Mon_Disfigured2=583, Mon_Damned2=584,
Mon_MoonClan2=585, Mon_NightClan2=586, Mon_HellClan2=587, Mon_BloodClan2=588, Mon_DeathClan2=589,
Mon_FoulCrow2=590, Mon_BloodHawk2=591, Mon_BlackRaptor2=592, Mon_CloudStalker2=593, Mon_ClawViper2=594,
Mon_PitViper2=595, Mon_Salamander2=596, Mon_TombViper2=597, Mon_SerpentMagus2=598, Mon_Marauder2=599,
Mon_Infidel2=600, Mon_SandRaider2=601, Mon_Invader2=602, Mon_Assailant2=603, Mon_DeathMauler2=604,
Mon_QuillRat2=605, Mon_SpikeFiend2=606, Mon_RazorSpine2=607, Mon_CarrionBird2=608, Mon_ThornedHulk2=609,
Mon_Slinger2=610, Mon_Slinger3=611, Mon_Slinger4=612, Mon_VileArcher2=613, Mon_DarkArcher2=614,
Mon_VileLancer2=615, Mon_DarkLancer2=616, Mon_BlackLancer2=617, Mon_Blunderbore2=618, Mon_Mauler2=619,
Mon_ReturnedMage5=620, Mon_BurningDeadMage4=621, Mon_ReturnedMage6=622, Mon_HorrorMage5=623, Mon_BoneMage5=624,
Mon_HorrorMage6=625, Mon_HorrorMage7=626, Mon_Huntress2=627, Mon_SaberCat2=628, Mon_CaveLeaper2=629,
Mon_TombCreeper2=630, Mon_Ghost2=631, Mon_Wraith2=632, Mon_Specter2=633, Mon_Succubus2=634,
Mon_HellTemptress2=635, Mon_Siren2=636, Mon_HellWitch2=637, Mon_VileWitch2=638, Mon_Gloam2=639,
Mon_BlackSoul2=640, Mon_BurningSoul2=641, Mon_Carver2=642, Mon_Devilkin2=643, Mon_DarkOne2=644,
Mon_CarverShaman2=645, Mon_DevilkinShaman2=646, Mon_DarkShaman2=647, Mon_BoneWarrior2=648, Mon_Returned2=649,
Mon_Gloombat2=650, Mon_Fiend2=651, Mon_MoonLord2=652, Mon_HellLord2=653, Mon_Scarab2=654,
Mon_SteelScarab2=655, Mon_Flayer3=656, Mon_StygianDoll3=657, Mon_SoulKiller3=658, Mon_Flayer4=659,
Mon_StygianDoll4=660, Mon_SoulKiller4=661, Mon_FlayerShaman2=662, Mon_StygianDollShaman2=663,
Mon_SoulKillerShaman2=664, Mon_TempleGuard2=665, Mon_TempleGuard3=666, Mon_Guardian2=667, Mon_Unraveler2=668,
Mon_HoradrimAncient2=669, Mon_HoradrimAncient3=670, Mon_Zealot2=671, Mon_Zealot3=672, Mon_Heirophant3=673,
Mon_Heirophant4=674, Mon_Grotesque2=675, Mon_FleshSpawner2=676, Mon_GrotesqueWyrm2=677, Mon_FleshBeast2=678,
Mon_BloodMaggot2=679, Mon_BloodMaggotYoung2=680, Mon_BloodMaggotEgg2=681, Mon_Slayer2=682, Mon_HellSpawn2=683,
Mon_GreaterHellSpawn2=684, Mon_Arach2=685, Mon_Balrog2=686, Mon_PitLord2=687, Mon_DemonImp2=688,
Mon_DemonTrickster2=689, Mon_UndeadStygianDoll2=690, Mon_UndeadSoulKiller2=691, Mon_Strangler2=692,
Mon_StormCaster2=693, Mon_MawFiend2=694, Mon_BloodLord3=695, Mon_GhoulLord2=696, Mon_DarkLord2=697,
Mon_UnholyCorpse2=698, Mon_DoomKnight2=699, Mon_DoomKnight3=700, Mon_OblivionKnight2=701, Mon_OblivionKnight3=702,
Mon_Cadaver2=703,
Mon_UberMephisto=704,Mon_UberDiablo=705,Mon_UberIzual=706,Mon_UberAndariel=707,Mon_UberDuriel=708,Mon_UberBaal=709,
mon_uber_diablo_pit_lord=711,
mon_uber_mephisto_squeleton=725,
mon_uber_mephisto_archer=726,
mon_uber_mephisto_fire=727,
mon_uber_mephisto_lightning=728,
mon_uber_mephisto_cold=729,
mon_uber_mephisto_poison=730,
mon_uber_baal_dark_lord=731,
mon_uber_baal_specter=732,
};
enum HackmapItemIndex {
	HoradricStaff=92,StaffOfKing=93,
	KhalimsFlail=174,KhalimsWill=175,
	ViperAmulet=2014,
	KhalimsEye=2046,KhalimsHeart=2047,KhalimsBrain=2048,
};

/*
UnitAny.dwFlags1
0x00000001 - tells the game to update the unit ( set after operateFn for objects, when reinitializing a unit etc ) 
0x00000002 - whenever the unit can be selected as a target 
0x00000004 - whenever the unit can be attacked 
0x00000008 - used to check if the unit is a valid target, curses use this 
0x00000010 - whenever the unit seed has been initialized 
0x00000020 - whenever to draw a shadow or not ( client only ) 
0x00000040 - whenever the SkillDo func has executed for the active skill 
0x00000080 - saw this used only with objects so far, when set the pre_operate is disabled 
0x00000100 - whenever the unit has a text message attached to it ( do not set this randomly ) 
0x00000200 - if this is set the unit will be treated as a hireling for certain things like lifebar display (but also for gameplay relevant aspects) 
0x00000400 - whenever the unit has a event sound specified ( server-side, do not set this randomly ) 
0x00000800 - this is only used for the summoner as far as I can tell, don't know what exactly for. 
0x00001000 - used by items to send a refresh message when they drop to the ground (etc) 
0x00002000 - whenever the unit is linked into a update message chain ( don't set this randomly ) 
0x00004000 - whenever to load new graphics when using a skill sequence ( ie the current sequence frame uses a different animation mode then the previous one ). 
0x00008000 - updates the client with the most recent life percentage and hitclass (used mostly by softhit attacks) 
0x00010000 - the unit is dead 
0x00020000 - disables treasureclass drops 
0x00080000 - this is set when the MonMode changes, didn't see exact use yet 
0x00100000 - whenever to predraw this unit ( ie treat it as a floor tile for display purposes, client specific ) 
0x00200000 - whenever this unit is an async unit ( exists only clientside like critters ) 
0x00400000 - whenever this unit is a client unit 
0x01000000 - set once the unit has been initialized, didn't check specifics 
0x02000000 - set for resurrected units and items on the floor 
0x04000000 - never gives experience when slain 
0x10000000 - automap related, not documented here 
0x20000000 - ditto 
0x40000000 - units that pet ais should ignore 
0x80000000 - this is a revived monster 

UnitAny.dwFlags2
0x00000001 - this item has an inventory attached ( aka sockets ) 
0x00000002 - whenever to update the inventory contents 
0x00000004 - set for items spawned inside a vendor shop 
0x00000008 - whenever the unit is currently shapeshifted 
0x00000010 - set for items, seams to be related to initialization 
0x00000080 - whenever the unit is within the clients line of sight 
0x00000100 - the unit has been deleted but not freed yet ( do not set this, ever ) 
0x00000400 - pUnit stores details about the owner ( missiles use this, but also set for linked pets ) 
0x00001000 - the unit is a corpse ( check +0xC4 & 0x10000 rather then this ) 
0x00010000 - the unit has teleported and needs to resync 
0x00020000 - whenever pUnit stores info about the last attacker 
0x00040000 - don't draw this unit 
0x02000000 - whenever this is an expansion unit 
0x04000000 - whenever this is a server-side unit
*/

#endif
