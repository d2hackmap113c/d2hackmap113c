#ifdef VARIABLE_DECLARE
	#define GVAR(d1,v1,v2) d1 v1 = v2 ;
	#define GVAR2(d1,v1,...) d1 v1 = { __VA_ARGS__ } ;
#else
	#define FUNCTION_DECLARE
	#define GVAR(d1,v1,v2) extern d1 v1 ;
	#define GVAR2(d1,v1,...) extern d1 v1 ;
#endif

#ifdef FUNCTION_DECLARE
	int AutoSkillNow();
	void checkAutoSkill();
	void checkAutoSkillStatus();
	void AutoPickupNewGame();
	void AutoPickupLoop();
	int AutoPickupItem();
	void AutoEnchantPlayerNeedHelp(int id);
	void AutoSummonUnit(UnitAny *pUnit);
	void AutoSummonNewGame();
	void AutoSummonRunLoop();
#endif
	GVAR2(ToggleVar,	tAutoPickupToggle,				TOGGLEVAR_ONOFF,	0,	-1,	1,  "Auto Pickup" )
	GVAR2(char,			aAutoPickupItem[3000]	,0)
	GVAR(int,	fAutoEnchant,				0)
	GVAR(int,	fAutoEnchantUseLeftSkill,				0)
	GVAR2(ToggleVar,	tAutoEnchant,				TOGGLEVAR_ONOFF,	0,	-1,	 1,	"Auto Enchant",&checkAutoSkill)
	GVAR(int,	dwRenewEnchantMs,				180000)
	GVAR(int,	dwAutoEnchantMana,				100)
	GVAR(int,	dwRenewEnchantMana,				50)
	GVAR(int,	dwAutoEnchantPlayerLevel,				40)
	GVAR2(char,	dwAutoEnchantMonster[1024],				0)
	GVAR(int,	fAutoWarCry,				0)
	GVAR(int,	fAutoWarCryUseLeftSkill,				0)
	GVAR(int,	dwAutoWarCryRenewPercent,				0)
	GVAR2(ToggleVar,	tAutoWarCry,				TOGGLEVAR_ONOFF,	0,	-1,	 1,	"Auto War Cry",&checkAutoSkill)
	GVAR2(ToggleVar,	tAutoBackgroundSkill,				TOGGLEVAR_ONOFF,	0,	-1,	 0,	"Auto Background Skill",&checkAutoSkillStatus)
	GVAR2(ToggleVar,	tAutoForegroundSkillToggle,	TOGGLEVAR_ONOFF,	0,	-1,	 1, "Auto Foreground Skill Toggle",&checkAutoSkillStatus)
	GVAR2(ToggleVar,	tAutoForegroundSkill,	TOGGLEVAR_DOWN,	0,	-1,	 1, "Auto Foreground Skill",&AutoSkillNow)
	GVAR(int,	dwUseLeftSkillFirst,				0)
	GVAR(int,	dwAutoSkillCheckInterval,				500)
	GVAR(int,	dwAutoSkillIdleDelay,				500)
	GVAR(int,	dwAutoThrowMinQuantity, 3)
	GVAR(int,	dwAutoReloadThrowing, 0)
	GVAR(int,	dwAutoStaticFieldHPPercent, 10)
	GVAR(int,	dwAutoSkillCheckMs,				0)
	GVAR(int,	dwAutoSkillMode,				0)
	GVAR(int,	fAutoSkillNow,				0)
	GVAR(int,	fAutoSkill,				0)
	GVAR2(char,aAutoDimVisionMonster[1024]	,   0)
	GVAR2(char,aAutoSkillMonsterType[1024]	,   0)
	GVAR2(char,aAutoSkillDistance[512][2]	,   0)
GVAR2(ToggleVar,	tAutoSummon,				TOGGLEVAR_ONOFF,	0,	-1,	 0,	"Auto Summon",&checkAutoSkill)
GVAR2(ToggleVar,	tAutoSummonRevive,				TOGGLEVAR_ONOFF,	0,	-1,	 0,	"Auto Revive")
GVAR2(ToggleVar,	tShowSummonInfo,			TOGGLEVAR_ONOFF,	0,	-1,	 0,	"ShowSummonInfo")
GVAR(int,	fAutoSummon,				0)
GVAR(int,	fAutoSummonUseLeftSkill,				0)
GVAR(int,	dwAutoSummonStartMs,				0)
GVAR(int,	dwAutoSummonCheckEnchantMs,				0)
GVAR(int,	dwAutoSummonMana,				100)
GVAR(int,	dwSwitchSkillMs,				100)
GVAR(int,	dwAutoSummonSkeletonLevelAdjust,				8)
GVAR(int,	dwAutoSummonSkeletonMageLevelAdjust,				8)
GVAR(int,	dwAutoSummonReviveLevelAdjust,				8)
GVAR2(char,	aAutoReviveMonster[1024]	,0)
GVAR(int,	dwSkeletonCount,				0)
GVAR(int,	dwEnchantSkeletonCount,				0)
GVAR(int,	dwSkeletonMaxCount,				0)
GVAR(int,	dwSkeletonMageCount,				0)
GVAR(int,	dwSkeletonMageMaxCount,				0)
GVAR(int,	dwReviveCount,				0)
GVAR(int,	dwReviveMaxCount,				0)
GVAR(int,	dwSkeletonHPPercent,				0)
GVAR(int,	dwSkeletonMageHPPercent,				0)
#ifdef FUNCTION_DECLARE
	#undef FUNCTION_DECLARE
#endif
#undef GVAR
#undef GVAR2
