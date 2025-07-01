#include "stdafx.h"
#include "header.h"
#include "auto.h"
#include "multi.h"

extern int fLeftBtnDown,fRightBtnDown,dwBtnReleaseMs,fUserOperating;
int ShiftLeftClickPos(int x,int y);
enum WarCryState {
	State_Scan=0,
	State_BCBO=1,
	State_BO=2,
	State_BCshout=3,
	State_shout=4,
	State_BConly=5,
};
static int checkMs,renewBoMs,renewBcMs,switchTimeout;
static int curState,shoutLv,boLv,bcLv,boSeconds,bcSeconds,fcrMs,dwSkillChangeCountVerify;
void AutoWarCryNewGame() {
	checkMs=dwCurMs+3000;
	curState=0;renewBoMs=0;renewBcMs=0;shoutLv=0;boLv=0;bcLv=0;fcrMs=600;dwSkillChangeCountVerify=0;
}
static void switchTo(int skill) {
	if (dwRightSkill==skill||dwLeftSkill==skill) return;
	if (dwRightSkill==Skill_BattleOrders||dwRightSkill==Skill_BattleCommand||dwRightSkill==Skill_Shout) {
		switchRightSkill(skill);checkMs=dwCurMs+dwSwitchSkillMs;switchTimeout=dwCurMs+500;
	} else if (dwLeftSkill==Skill_BattleOrders||dwLeftSkill==Skill_BattleCommand||dwLeftSkill==Skill_Shout) {
		switchLeftSkill(skill);checkMs=dwCurMs+dwSwitchSkillMs;switchTimeout=dwCurMs+500;
	} else {gameMessage("AutoWarCry error switch %d",skill);checkMs=dwCurMs+10000;curState=0;}
}
static int useSkill(int skill) {
	if (dwRightSkill==skill) {
		if (!fCanUseRightSkill) return 0;
		RightSkillPos(PLAYER->pMonPath->wUnitX,PLAYER->pMonPath->wUnitY);
	} else if (dwLeftSkill==skill) {
		if (!fCanUseLeftSkill) return 0;
		ShiftLeftClickPos(PLAYER->pMonPath->wUnitX+1,PLAYER->pMonPath->wUnitY+1);
	} else {
		if (dwCurMs<switchTimeout) return 0;
		gameMessage("AutoWarCry switch skill timeout %d",skill);checkMs=dwCurMs+10000;curState=0;return 0;
	}
	if (skill==Skill_BattleOrders) renewBoMs=dwCurMs+boSeconds*10*dwAutoWarCryRenewPercent;
	else if (skill==Skill_BattleCommand) renewBcMs=dwCurMs+bcSeconds*10*dwAutoWarCryRenewPercent;
	return 1;
}
void AutoWarCryRunLoop() {
	if (!fAutoWarCry) return;
	if (dwCurMs<checkMs) return;
	if (fPlayerInTown||fAutoFollowMoving) return;
	if (fWinActive&&(fLeftBtnDown||fRightBtnDown||dwCurMs<dwBtnReleaseMs+dwAutoSkillIdleDelay))
		return;
	if (dwSkillChangeCountVerify!=dwSkillChangeCount||!boLv) {
		shoutLv=getSkillLevel(PLAYER,Skill_Shout); //mana6
		boLv=getSkillLevel(PLAYER,Skill_BattleOrders); //mana7
		bcLv=getSkillLevel(PLAYER,Skill_BattleCommand); //mana11
		boSeconds=!boLv?0:30+10*boLv;bcSeconds=!bcLv?0:5+10*bcLv;
		fcrMs=getFcrFrame(dwPlayerClass,PLAYER)*40;
		dwSkillChangeCountVerify=dwSkillChangeCount;
	}
	if (!boLv) {fAutoWarCry=0;return;}
	if (dwPlayerMana<11) { //Not enough mana
		checkMs=dwCurMs+500;//Check mana again after 500ms
		return;
	}
	if (PLAYER->dwMode==PlayerMode_Attacking1||PLAYER->dwMode==PlayerMode_Attacking2||PLAYER->dwMode==PlayerMode_Cast) return;
	if (!fCanUseLeftSkill&&!fCanUseRightSkill) return;
	switch (curState) {
		case 0: {
			int needBO=dwCurMs>=renewBoMs,needBC=dwCurMs>=renewBcMs,needShout=0;
			if (!needBO) {
				if (dwPlayerClass!=4) {
					int bo=d2common_CheckUnitState(PLAYER, State_BattleOrders);
					int bc=d2common_CheckUnitState(PLAYER, State_BattleCommand);
					if (!bo) needBO=1;
					else if (bcLv&&!bc) needBC=1;
				} else {
					for (RosterUnit *pRU=PLAYERLIST;pRU;pRU=pRU->pNext) {
						if (dwPlayerPartyId!=pRU->wPartyId) continue;
						UnitAny *pUnit=d2client_GetUnitFromId(pRU->dwUnitId, UNITNO_PLAYER);if (!pUnit) continue;
						int bo=d2common_CheckUnitState(pUnit, State_BattleOrders);
						int shout=d2common_CheckUnitState(pUnit, State_Shout);
						int bc=d2common_CheckUnitState(pUnit, State_BattleCommand);
						if (bo&&shout&&bc) continue;
						if (d2common_IsUnitBlocked(PLAYER,pUnit,4)) continue;
						if (!bo) {needBO=1;break;}
						if (shoutLv&&!shout) needShout=1;
						if (bcLv&&!bc) needBC=1;
					}
				}
			}
			if (needBO) {
				if (bcLv) {switchTo(Skill_BattleCommand);curState=State_BCBO;}
				else {switchTo(Skill_BattleOrders);curState=State_BO;}
			} else if (needShout) {
				if (bcLv) {switchTo(Skill_BattleCommand);curState=State_BCshout;}
				else {switchTo(Skill_Shout);curState=State_shout;}
			} else if (needBC) {curState=State_BConly;switchTo(Skill_BattleCommand);}
			else checkMs=dwCurMs+3000;
			break;
		}
		case State_BCBO:
			if (!useSkill(Skill_BattleCommand)) return;
			switchTo(Skill_BattleOrders);curState=State_BO;break;
		case State_BO:
			if (!useSkill(Skill_BattleOrders)) return;
			curState=0;break;
		case State_BCshout:
			if (!useSkill(Skill_BattleCommand)) return;
			switchTo(Skill_Shout);curState=State_shout;break;
		case State_shout:
			if (!useSkill(Skill_Shout)) return;
			curState=0;break;
		case State_BConly:
			if (!useSkill(Skill_BattleCommand)) return;
			curState=0;break;
		default:curState=0;checkMs=dwCurMs+3000;break;
	}
	checkMs=dwCurMs+300;
}
