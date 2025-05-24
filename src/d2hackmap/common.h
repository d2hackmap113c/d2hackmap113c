#ifndef COMMON_H
#define COMMON_H

#define LOG(fmt,...) {if (logfp) {fprintf(logfp,fmt,__VA_ARGS__);fflush(logfp);}}
extern FILE *logfp;
extern char szPluginPath[MAX_PATH];
wchar_t * wscolorcpy(wchar_t *dest, wchar_t *src , BYTE color);
int MyMultiByteToWideChar(
						UINT CodePage,         // code page
						DWORD dwFlags,         // character-type options
						LPCSTR lpMultiByteStr, // string to map
						int cbMultiByte,       // number of bytes in string
						LPWSTR lpWideCharStr,  // wide-character buffer
						int cchWideChar        // size of buffer
						);
	void GB2GBK(char *szBuf);
int getFileSize(const char *path);
int fileExist(const char *path);
FILE *openFile(char *path,char *mode);
int readFully(FILE *fp,char *buf,int size);
char *heap_strdup(HANDLE heap,char *s);
char *loadFile(HANDLE heap,FILE *fp,int *psize);
void __cdecl partyMessage(char *fmt, ...);
void __cdecl gameMessage(char *fmt, ...);
void __cdecl gameMessageW(wchar_t *fmt, ...);
void __cdecl partyMessageW(wchar_t *fmt, ...);
void __cdecl partyMessageWcolor(int color,wchar_t *fmt, ...);
int get_cpu_usage(int *ucpu,int *kcpu);

BOOL IsFullWindow();
int GameControlStart(int cmd);
int GameControlEnd(int cmd);
void GameControl(int cmd);
int GameSound(int snd);
UnitAny *GetUnitFromIdSafe(DWORD dwUnitId, DWORD dwUnitType);
int getUnitRawDistance2(UnitAny *pUnit1,UnitAny *pUnit2);
float getUnitYardDistance2(UnitAny *pUnit1,UnitAny *pUnit2);
float getUnitYardDistance(UnitAny *pUnit1,UnitAny *pUnit2);
int getDistanceM256(int dx,int dy);
int getPlayerDistanceM256(UnitAny *pUnit);
float getPlayerDistanceRaw(UnitAny *pUnit);
float getPlayerDistanceYard(UnitAny *pUnit);
PetUnit * GetPetByType(UnitAny *pUnit,int type);
int getSkillLevel(UnitAny *pUnit, int skillId);
RosterUnit *getRosterUnit(int id);
void useBelt(int id,int shift);
UnitAny *getAreatileByTxt(int txt);
UnitAny *getPortalToArea(int level,char *owner);
int getAreatileTxtToLevel(int toLevel);
int singlePlayerSaveGame(char *path);
int encodeItem(UnitAny *pUnit,char *buf,int size);
int hasSkill(int id);
void selectSkill(int right,int skill);
void switchLeftSkill(int skill);
void switchRightSkill(int skill);
UnitAny *getSinglePlayerUnit(int id,int unitType);
int LeftClickPos(int x,int y);
int LeftClickUnit(UnitAny *pUnit);
int ShiftLeftClickPos(int x,int y);
int ShiftLeftClickUnit(UnitAny *pUnit);
int RightSkill(UnitAny *pUnit,int x,int y);
int RightSkillPos(int x,int y);
int RightSkillUnit(UnitAny *pUnit);
int canUseSkillNow(UnitAny *pUnit,int skillId);
int skillUnusable(UnitAny *pUnit,Skill *pSkill);
void SendPacket(BYTE *buf,int size);
int getFcrFrame(int type,UnitAny *pUnit);
int getGameControlKey(int cmd);
void SetBottomAlertMsg1(wchar_t *wszMsg, int ms, int color,int blink);
void SetBottomAlertMsg2(wchar_t *wszMsg, int ms, int color,int blink);
void SetBottomAlertMsg3(wchar_t *wszMsg, int ms, int color,int blink);

DWORD GetTextWidth(wchar_t *wStr);
DWORD GetTextHeight(wchar_t *wStr);

void DrawD2Text(DWORD dwFont, wchar_t *wStr, int xpos, int ypos, DWORD dwColor,DWORD dwAlign = 0);
void DrawCenterText(DWORD dwFont, wchar_t *wStr, int xpos, int ypos, DWORD dwColor, int div = 1 , DWORD dwAlign = 0 );
int drawBgText(wchar_t *wStr,int x,int y,int color,int bgColor);
int drawBgTextLeft(wchar_t *s,int x,int y,int color,int bgColor);
int drawBgTextMiddle(wchar_t *s,int x,int y,int color,int bgColor);

int GetAreaLevel();
int getPlayerClass(int id);
int sameParty(int dwUnitId);
int testPvpFlag(int dwUnitId);
int getMonsterOwnerId(int id);
int GetUnitResist(UnitAny *pUnit,DWORD dwStatNo);
BOOL CheckUnitSelect( UnitAny *pUnit );
int GetItemAlvl(int Ilvl,int Qlvl,int magic_lvl);
int GetItemILvlAfterCube(int icubeType,int Ilvl , int Clvl);
void DrawCenterAlertMsg();
UnitAny * GetUnitPet(UnitAny *pUnit) ;
DWORD GetUnitMagicStat(UnitAny *pUnit , DWORD dwStatNo);
int GetGameLanguage();
void ExitGame();
bool isInRect(AreaRectData* pData, int ux, int uy);
int portalToLevel(World* game, UnitAny* pUnit, int levelId);
int hasPortalInRect(AreaRectData* pData, int dstLvl);
int hasPortalNearby(AreaRectData* pData, int dstLvl);
int hasPortalInLevel(DrlgLevel* level, int dstLvl);
int unitHasItem(UnitAny *pUnit,int index);
World * getWorldByClientId(int clientId);
NetClient * getNetClient(World *world,int clientId);
void __cdecl sendChatMessageToServer(char *fmt,...);
void sendPacketToClient(NetClient *client,char *packet,int packetLen);
void sendMessageToClient(NetClient *client,int type,int color,char *name,char *msg);
CharStatsTxt* getCharStatsTxt(int cls);
World *getSinglePlayerWorld();
int getStatMaxValue(int id);
int cpLocaleName(wchar_t *dst,wchar_t *s,int max);
int acpLocaleName(char *dst,wchar_t *s,int bufsize);
void dc6cell2bmp(GfxCell *pcell,char *outpath);
#endif
