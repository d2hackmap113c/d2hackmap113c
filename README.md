Some improvements of d2hackmap 1.13c

Using comand line like the following to start the game:

D:\game\diablo2\D2Loader.exe -locale chi -lq -pdir D:\d2hackmap\map113c -direct -skiptobnet -w

Source code can be compiled using Microsoft Visual Studio 2008. The pre-compiled dll doesn't look well optimized.

Improvements:

More settings of auto enchant:
Auto Enchant Toggle: 1, CTRL_ALT_VK_E
Renew Enchant Seconds:	300 
Enchant Mana: 70 
Auto Enchant Group: 3 //enchant to group 3, player is group 1
Auto Enchant Monster Group[Act 1 Hireling,Act 2 Hireling,Act 5 Hireling 1hs,Act 5 Hireling 2hs]: 2 
Auto Enchant Monster Group[Clay Golem-Fire Golem,Necroskeleton]: 3 
Auto Enchant Monster Group[Spirit Wolf,Dire Wolf,Heart of Wolverine,Grizzly]: 3 
Auto Enchant Monster Group[Shadow Warrior,Shadow Master]: 3 
Auto Enchant Monster Group[Valkyrie]: 3 

Support CONTROL,SHIFT,ALT Key combinations, for example:
Full Screen Key: ALT_VK_RETURN 
Mini Map Scroll Keys[1]: VK_UP,VK_WHEELUP
Mini Map Scroll Offset[1]: 0,-6
Mini Map Scroll Keys[2]: VK_DOWN,VK_WHEELDOWN
Mini Map Scroll Offset[2]: 0,6
Mini Map Scroll Keys[3]: VK_LEFT,SHIFT_VK_WHEELUP
Mini Map Scroll Offset[3]: -6,0
Mini Map Scroll Keys[4]: VK_RIGHT,SHIFT_VK_WHEELDOWN
Mini Map Scroll Offset[4]: 6,0

Ability to control game using key combination, for example:
GameControlKeys[GameControlSayHelp]: SHIFT_VK_H,-1,-1,-1
GameControlKeys[GameControlSayFollowme]: SHIFT_VK_F,-1,-1,-1
GameControlKeys[GameControlSayThisisforyou]: SHIFT_VK_Y,-1,-1,-1
GameControlKeys[GameControlSayThanks]: SHIFT_VK_T,-1,-1,-1
GameControlKeys[GameControlSaySorry]: SHIFT_VK_S,-1,-1,-1
GameControlKeys[GameControlSayBye]: SHIFT_VK_B,-1,-1,-1
GameControlKeys[GameControlSayNowyoudie]: SHIFT_VK_D,-1,-1,-1
GameControlKeys[GameControlSayRetreat]: SHIFT_VK_R,-1,-1,-1
GameControlKeys[GameControlHireUseBelt1]: ALT_VK_1,-1,-1,-1
GameControlKeys[GameControlHireUseBelt2]: ALT_VK_2,-1,-1,-1
GameControlKeys[GameControlHireUseBelt3]: ALT_VK_3,-1,-1,-1
GameControlKeys[GameControlHireUseBelt4]: ALT_VK_4,-1,-1,-1
GameControlKeys[GameControlCharacterScreen]: ALT_VK_A,ALT_VK_C,-1,-1
GameControlKeys[GameControlQuestLog]: ALT_VK_Q,-1,-1,-1
GameControlKeys[GameControlSkillTree]: ALT_VK_T,-1,-1,-1

Ability to switch both skills upon press and release, for example:
SwitchSkillKeys[4]: VK_D
SwitchSkillLeft[4][ClassNec]: SkillTeeth,-1,-1,-1 //
SwitchSkillRight[4][ClassNec]: SkillAmplifyDamage,-1,-1,-1
SwitchSkillRightUp[4][ClassNec]: SkillDecrepify
SwitchSkillStandStill[4]: 1
SwitchSkillKeys[5]: VK_S
SwitchSkillLeft[5][ClassNec]: SkillRaiseSkeletalMage,-1,-1,-1
SwitchSkillRight[5][ClassNec]: SkillRaiseSkeleton,-1,-1,-1
SwitchSkillStandStill[5]: 1
SwitchSkillKeys[10]: VK_TAB
SwitchSkillRight[10]: SwapWeapon,-1,-1,-1
SwitchSkillRightUp[10]: SwapWeapon
SwitchSkillStandStill[10]: 1

New setting for item showing based on character level:
Item Show CLvl[2006,2080-2081]: 15  //Stamina Mina Light Healing
Item Show CLvl[2082]:           20  //Healing

More settings for life protection:
Chicken Max Life:        800 //condition to use the next value
Chicken Life:            600 
Chicken Life Min Clevel: 3  //disable life protection when character level less than 3 
ChickenLifeForcedClevel: 85 //force life protection when character level greater than 85
ChickenLifeEnterGame:    1 //auto turn on life protection each time enter game

Create a snapshot of the character on local hard disk. A text file will be created for text searching. A d2s file will be created for local single player playing. Quest data of a difficulty is only available after entering a game of that difficulty. Waypoints of a difficulty is only available after opening a waypoint in a game of that difficulty.
Snapshot New Game: 1,-1 //Do snapshot when enter a game
Snapshot End Game: 1,-1 //Do snapshot when exit a game
Snapshot Key: VK_L
Snapshot Path: "D:\snapshot"
Snapshot Names Path: "names.txt"

Auto Summoning skeleton and skeleton mage
Auto Summon Toggle: 1, CTRL_ALT_VK_S, 1 //the third value means only active in Nihlathak's Temple
AutoSummonSkeletonCount: 8 //Max skeleton count, auto switch to skeleton mage then
AutoSummonSkeletonMageCount: 8 //Max skeleton mage count, auto switch to skeleton then
