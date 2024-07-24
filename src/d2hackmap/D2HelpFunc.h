#ifndef D2_HELPFUNC_H
#define D2_HELPFUNC_H

UnitAny *GetUnitFromIdSafe(DWORD dwUnitId, DWORD dwUnitType);
PetUnit * GetPetByType(UnitAny *pUnit,int type);
int getSkillLevel(UnitAny *pUnit,int cls, int type, int skillId);
RosterUnit *getRosterUnit(int id);
void useBelt(int id,int shift);
int singlePlayerSaveGame(char *path);
int encodeItem(UnitAny *pUnit,char *buf,int size);
int hasSkill(int id);
void selectSkill(int right,int skill);
UnitAny *getSinglePlayerUnit(int id,int unitType);
int RunToPos(int x,int y);
int LeftClickObject(UnitAny *pUnit);
int RightSkill(UnitAny *pUnit,int x,int y);
int canUseSkillNow(UnitAny *pUnit,int skillId);
void SendPacket(BYTE *buf,int size);
void ShowGameMessage(char *Msg);
void ShowGameMessage(DWORD val);
int getFcrFrame(int type,UnitAny *pUnit);
int getGameControlKey(int cmd);

CellFile *InitCellFile(CellFile *pCellFile);
void DeleteCellFile(CellFile *pCellFile);
CellFile *LoadBmpCellFile(char *filename);

DWORD GetTextWidth(wchar_t *wStr);
DWORD GetTextHeight(wchar_t *wStr);
void DrawAutomapCell(CellFile *pCellfile, int xpos, int ypos, BYTE col);

void DrawD2Text(DWORD dwFont, wchar_t *wStr, int xpos, int ypos, DWORD dwColor,DWORD dwAlign = 0);
void DrawCenterText(DWORD dwFont, wchar_t *wStr, int xpos, int ypos, DWORD dwColor, int div = 1 , DWORD dwAlign = 0 );
void DrawDefaultFontText(wchar_t *wStr, int xpos, int ypos, DWORD dwColor, int div = 1 , DWORD dwAlign = 0 );

void PreparePosForDraw(POINT* pos, int x, int y);
void MapToAbsScreen(POINT* pos, int x, int y);
WORD GetAreaLevel();
int getPlayerClass(DWORD id);
int sameParty(DWORD dwUnitId , DWORD dwTargetUnitId);
BYTE TestPvpFlag(DWORD dwUnitId , DWORD dwTargetUnitId);
int GetUnitResist(UnitAny *pUnit,DWORD dwStatNo);
BOOL TestMonsterOnScreen(UnitAny *mon, UnitAny *pla);
double __fastcall GetUnitDistanceInSubtiles(UnitAny *pUnit1 , UnitAny *pUnit2);
BOOL CheckUnitSelect( UnitAny *pUnit );
int GetItemAlvl(int Ilvl,int Qlvl,int magic_lvl);
int GetItemILvlAfterCube(int icubeType,int Ilvl , int Clvl);
void DrawCenterAlertMsg();
void SetCenterAlertMsgParam(int color1,int color2,int ms);
void SetCenterAlertMsg( BOOL draw ,wchar_t *wszMsg );
UnitAny * GetUnitPet(UnitAny *pUnit) ;
DWORD GetUnitMagicStat(UnitAny *pUnit , DWORD dwStatNo);
int GetGameLanguage();
void ExitGame();
#endif
