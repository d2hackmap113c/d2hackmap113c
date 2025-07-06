#include "stdafx.h"
#include "header.h"
#include "multi.h"
#include "auto.h"
#include "map.h"

int isWaypointTxt(int txt);
int takeWaypointToLevel(UnitAny *waypoint,int level);
void markNextPathInvalid(int ms);
int RerouteRectPath();

extern int bossX,bossY,bossId,nBoss,bossHps[3];
POINT TPtarget;
AreaRectData *TPtargetRect,*TPfailedRect;
int dwTpMs,dwTpDoneMs;
static int fTpAvoiding,fTpAutoTarget,dwLastTpMs;
static char aMonYard[32][2];
static char aMonYardHC[3][2];
static int dwArcaneSanctuaryYard=4;
static int defMonYard,maxTpYard,tpDelayMs,tpMinStep,stepInvalidMs,avoidEdge=0,dwAutoTeleportSafeDistance;
static int reachDis=6;
static ToggleVar tLogTP={TOGGLEVAR_ONOFF,0,-1,1,"Log TP Toggle"};
static int dwAutoTeleportTelekinesisDistance,dwAutoTeleportTelekinesisSafeDistance;
static int dwAutoTeleportTelekinesisEnterChamber;
static int dwAutoTeleportRescueBarAvoid=10;
int maxTpDisRaw;
static int forceEnterCharLevelNormal=60;
static int forceEnterCharLevelNightmare=80;
ToggleVar tAutoTeleportEvade={TOGGLEVAR_ONOFF,0,-1,1,"Auto Teleport Evade Toggle"};
static int autoTPEvadeYard[2];
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_INT,"ForceEnterCharLevelNormal",&forceEnterCharLevelNormal,4},
  {CONFIG_VAR_TYPE_INT,"ForceEnterCharLevelNightmare",&forceEnterCharLevelNightmare,4},
	{CONFIG_VAR_TYPE_INT,"AutoTeleportSafeDistance",&dwAutoTeleportSafeDistance,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportMonsterDefaultDistance",&defMonYard,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportArcaneSanctuaryYard",&dwArcaneSanctuaryYard,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportMaxDistance",&maxTpYard,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportDelayMs",&tpDelayMs,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportAvoidEdge",&avoidEdge,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportMinStep",&tpMinStep,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportStepInvalidMs",&stepInvalidMs,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportTelekinesisEnterChamber",&dwAutoTeleportTelekinesisEnterChamber,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportTelekinesisDistance",&dwAutoTeleportTelekinesisDistance,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportTelekinesisSafeDistance",&dwAutoTeleportTelekinesisSafeDistance,4 },
	{CONFIG_VAR_TYPE_INT,"AutoTeleportRescueBarAvoid",&dwAutoTeleportRescueBarAvoid,4 },
	{CONFIG_VAR_TYPE_CHAR_ARRAY0,"AutoTeleportMonsterDistance",&aMonYard,2,{32}},
	{CONFIG_VAR_TYPE_CHAR_ARRAY0,"AutoTeleportMonsterDistanceHC",&aMonYardHC,2,{3}},
	{CONFIG_VAR_TYPE_KEY,"LogTPToggle",&tLogTP},
	{CONFIG_VAR_TYPE_KEY,"AutoTeleportEvadeToggle",&tAutoTeleportEvade},
  {CONFIG_VAR_TYPE_INT_ARRAY1,"AutoTeleportEvadeYard",&autoTPEvadeYard,2,{0}},
};
void autoteleport_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void autoteleport_initConfigVars() {
	memset(aMonYard,0,sizeof(aMonYard));
	memset(aMonYardHC,0,sizeof(aMonYardHC));
}
void autoteleport_fixValues() {
	maxTpDisRaw=(maxTpYard*3)>>1;
}
struct TPPos {POINT p;int disM256;AreaRectData *pData;};
struct Mon {POINT p;int dis;};
static POINT src,dst;
static TPPos *tposs=NULL;
static int nPos=0,posCap=0;
static Mon *mons=NULL,*mons_end;
static int monCap=0,nMons=0,maxTpDisM256;
static Mon *prevMons=NULL;
static int prevMonCap=0,nPrevMons=0;
static char monMap[32][32];
static int monMapX,monMapY;
static int maxDisM256;
static POINT bestP;
static AreaRectData *bestRect;
static int rectCountFromTarget=0;
static RECT curActiveBox,searchBox;
int isAutoTeleporting=0;

static int comparePos(const void *a,const void *b) {return ((TPPos *)a)->disM256-((TPPos *)b)->disM256;}
static int dis(POINT *p1,POINT *p2) {return getDistanceM256(p1->x-p2->x,p1->y-p2->y)>>8;}
static int disM256(POINT *p1,POINT *p2) {return getDistanceM256(p1->x-p2->x,p1->y-p2->y);}
//static int dis2(POINT *p1,POINT *p2) {int dx=p1->x-p2->x,dy=p1->y-p2->y;return dx*dx+dy*dy;}
static int findBoss(int txt,int unique) {
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_MONSTER*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (pUnit->dwUnitType!=UNITNO_MONSTER) break;
			if (pUnit->dwTxtFileNo==txt) {
				dst.x=pUnit->pMonPath->wUnitX;dst.y=pUnit->pMonPath->wUnitY;
				if (pUnit->dwMode==MonsterMode_Death||pUnit->dwMode==MonsterMode_Dead) return 0;
				if (unique&&!pUnit->pMonsterData->fUnique) continue;
				return 1;
			}
		}
	}
	return 0;
}
static void addMonMap(int x,int y) {
	x-=monMapX;y-=monMapY;if (x<0||x>=128||y<0||y>=128) return;
	monMap[y>>2][x>>2]=1;
}
static int hasMon(int x,int y) {
	x-=monMapX;y-=monMapY;if (x<0||x>=128||y<0||y>=128) return 0;
	return monMap[y>>2][x>>2];
}
static void resetMonsters() {
	if (!mons) {monCap=4096;mons=(Mon *)HeapAlloc(dllHeap,0,sizeof(Mon)*monCap);}
	memset(monMap,0,32*32);
	monMapX=dwPlayerX-64;monMapY=dwPlayerY-64;
	nMons=0;mons_end=mons;
}
int getUnitOwnerId(UnitAny *pUnit);
extern int dwBarbrianLeft;
static int isMonster(UnitAny *pMon) {
	if (pMon->dwUnitType!=UNITNO_MONSTER) return 0;
	if (pMon->dwMode==MonsterMode_Death) return 0; //dying?
	if (pMon->dwMode==MonsterMode_Dead) return 0; //already dead
	int owner=getUnitOwnerId(pMon);if (owner!=-1) return 0; 
	int mtype=aAutoSkillMonsterType[pMon->dwTxtFileNo];
	if (mtype&MONSTER_TYPE_NEUTRAL) return 0;
	if (!mtype) return MONSTER_TYPE_NORMAL;
	return mtype;
}
static void findMonsters(int evade) {
	resetMonsters();
	char *yard;
	if (fIsHardCoreGame) yard=aMonYardHC[DIFFICULTY];
	else yard=aMonYard[dwPlayerFcrFrame];
	for (int i=0;i<128;i++) {
		for (UnitAny *pMon=d2client_pUnitTable[UNITNO_MONSTER*128+i];pMon;pMon=pMon->pHashNext) {
			if (pMon->dwUnitType!=UNITNO_MONSTER) break;
			int mtype=isMonster(pMon);if (!mtype) continue;
			if (nMons>=monCap) {monCap*=2;mons=(Mon *)HeapReAlloc(dllHeap,0,mons,sizeof(Mon)*monCap);}
			Mon *pmon=&mons[nMons++];
			int x=pMon->pMonPath->wUnitX;int y=pMon->pMonPath->wUnitY;pmon->p.x=x;pmon->p.y=y;
			if (evade) {
				if ((pMon->dwTxtFileNo==Mon_FireTower||pMon->dwTxtFileNo==Mon_LightningSpire)
					&&dwCurrentLevel==Level_ArreatSummit) {nMons--;continue;}
				pmon->dis=autoTPEvadeYard[(mtype&MONSTER_TYPE_DANGROUS)?1:0];
			} else if (dwCurrentLevel==Level_ArcaneSanctuary) {
				pmon->dis=dwArcaneSanctuaryYard;
			} else {
				pmon->dis=yard[(mtype&MONSTER_TYPE_DANGROUS)?1:0];
				if (dwBarbrianLeft==5&&pMon->dwTxtFileNo==Mon_PrisonDoor&&LEVELNO==Level_FrigidHighlands) { 
					//rescue mission: prison door
					//int done=QUESTDATA->quests[36]&3;
					pmon->dis=dwAutoTeleportRescueBarAvoid;
				}
			}
			if (!pmon->dis) pmon->dis=defMonYard;
			pmon->dis=(pmon->dis*3)>>1;
			addMonMap(x-2,y-2);addMonMap(x-2,y+2);addMonMap(x+2,y-2);addMonMap(x+2,y+2);
		}
	}
	if (nPrevMons) {
		Mon *mend=prevMons+nPrevMons;
		for (Mon *p=prevMons;p<mend;p++) {
			int x=p->p.x,y=p->p.y;
			if (curActiveBox.left<=x&&x<curActiveBox.right&&curActiveBox.top<=y&&y<curActiveBox.bottom) continue;
			if (nMons>=monCap) {monCap*=2;mons=(Mon *)HeapReAlloc(dllHeap,0,mons,sizeof(Mon)*monCap);}
			mons[nMons++]=*p;
			addMonMap(x-2,y-2);addMonMap(x-2,y+2);addMonMap(x+2,y-2);addMonMap(x+2,y+2);
		}
	}
	mons_end=mons+nMons;
}
static void calBoundingBox() {
	int x1,y1,x2,y2;x1=y1=0x7fffffff;x2=y2=-0x7ffffffe;
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	for (int i=0;i<pData->nearbyRectCount;i++) {
		AreaRectData *pNData=pData->paDataNear[i];
		int x=pNData->unitX,y=pNData->unitY,w=pNData->unitW,h=pNData->unitH;
		if (x<x1) x1=x;if (y<y1) y1=y;
		x+=w;y+=h;if (x>x2) x2=x;if (y>y2) y2=y;
	}
	curActiveBox.left=x1;curActiveBox.top=y1;curActiveBox.right=x2;curActiveBox.bottom=y2;
	searchBox.left=x1+avoidEdge;searchBox.top=y1+avoidEdge;searchBox.right=x2-avoidEdge;searchBox.bottom=y2-avoidEdge;
}
static void savePrevMonsters() {
	if (!prevMons) {
		prevMonCap=4096;if (prevMonCap<nMons) prevMonCap=monCap;
		prevMons=(Mon *)HeapAlloc(dllHeap,0,sizeof(Mon)*prevMonCap);
	} else if (prevMonCap<nMons) {
		prevMonCap=monCap;HeapFree(dllHeap,0,prevMons);
		prevMons=(Mon *)HeapAlloc(dllHeap,0,sizeof(Mon)*prevMonCap);
	}
	nPrevMons=nMons;memcpy(prevMons,mons,sizeof(Mon)*nMons);
}
static int getMonDis(POINT *src) {
	int minDis=0x7FFFFFFF;
	for (Mon *p=mons;p<mons_end;p++) {
		int d=dis(src,&p->p)-p->dis;
		if (d<minDis) minDis=d;
	}
	return minDis;
}
static int addPossiblePoint(AreaRectData *pData,POINT p) {
	int w=pData->bitmap->unitW,h=pData->bitmap->unitH;
	int rx=p.x-pData->unitX,ry=p.y-pData->unitY;
	if (rx<0||rx>=w||ry<0||ry>=h) return 0;
	unsigned short *bitmap=pData->bitmap->bitmap;
	unsigned short *ptr=bitmap+ry*w+rx;
	if (((*ptr++)&0x1527)!=0) return 1;
	if (hasMon(p.x,p.y)) return 1;
	if (disM256(&src,&p)>maxTpDisM256) return 1;
	if (nPos>=posCap) {posCap*=2;tposs=(TPPos *)HeapReAlloc(dllHeap,0,tposs,sizeof(TPPos)*posCap);}
	int dstM256=0;//disM256(&dst,&p);
	TPPos *pp=&tposs[nPos++];pp->p=p;pp->disM256=dstM256;pp->pData=pData;
	return 1;
}

static void addPossiblePostions(AreaRectData *pCenter,AreaRectData *pData,POINT *p,int step) {
	if (!pData||!pData->bitmap) return;
	if (pData==TPfailedRect) return;
	unsigned short *bitmap=pData->bitmap->bitmap;
	int ux=pData->bitmap->unitX,uy=pData->bitmap->unitY;
	int w=pData->bitmap->unitW,h=pData->bitmap->unitH;
	unsigned short *end=bitmap+w*h;
	int x0=0,x1=w,y0=0,y1=h;
	if (p) {
		x0=p->x-pData->unitX-step;if (y0<0) y0=0;
		x1=p->x-pData->unitX+step;if (x1>w) x1=w;
		y0=p->y-pData->unitY-step;if (y0<0) y0=0;
		y1=p->y-pData->unitY+step;if (y1>h) y0=h;
		step=1;
	}
	if (ux+x0>=searchBox.right||ux+x1<=searchBox.left) return;
	if (uy+y0>=searchBox.bottom||uy+y1<=searchBox.top) return;
	if (ux+x0<searchBox.left) x0=searchBox.left-ux;
	if (ux+x1>searchBox.right) x1=searchBox.right-ux;
	if (uy+y0<searchBox.top) y0=searchBox.top-uy;
	if (uy+y1>searchBox.bottom) y1=searchBox.bottom-uy;
	if (x0<0) x0=0;if (x1>w) x1=w;
	if (y0<0) y0=0;if (y1>h) y1=h;
	/*if (pCenter!=pData) {
		if (pData->tileX<pCenter->tileX) {if (x0<avoidEdge) x0=avoidEdge;}
		else if (pData->tileX>pCenter->tileX) {if (x1>w-avoidEdge) x1=w-avoidEdge;}
		if (pData->tileY<pCenter->tileY) {if (y0<avoidEdge) y0=avoidEdge;}
		else if (pData->tileY>pCenter->tileY) {if (y1>h-avoidEdge) y1=h-avoidEdge;}
	}*/
	for (int ty=y0;ty<y1;ty+=step) {
		for (int tx=x0;tx<x1;tx+=step) {
			unsigned short *ptr=bitmap+ty*w+tx;
			int x2=tx+step;if (x2>w) x2=w;
			int y2=ty+step;if (y2>h) y2=h;
			int rowskip=w-(x2-tx);
			POINT p;
			for (int y=ty;y<y2;y++) {
				for (int x=tx;x<x2;x++) {
					if (ptr>=end) goto nextTile;
					if (((*ptr++)&0x1527)==0) {
						p.x=pData->unitX+x;
						p.y=pData->unitY+y;
						if (hasMon(p.x,p.y)) goto nextTile;
						if (disM256(&src,&p)>maxTpDisM256) goto nextTile;
						int dstM256=disM256(&dst,&p);
						if (dstM256>maxDisM256) goto nextTile;
						if (nPos>=posCap) {posCap*=2;tposs=(TPPos *)HeapReAlloc(dllHeap,0,tposs,sizeof(TPPos)*posCap);}
						TPPos *pp=&tposs[nPos++];pp->p=p;pp->disM256=dstM256;pp->pData=pData;
						goto nextTile;
					}
				}
				ptr+=rowskip;
			}
nextTile:
			continue;
		}
	}
}
static int findValidPosition() {
	qsort(tposs,nPos,sizeof(TPPos),comparePos);
	TPPos *end=tposs+nPos;
	for (TPPos *p=tposs;p<end;p++) {
		int minMonDis=0x7FFFFFFF;
		for (Mon *mon=mons;mon<mons_end;mon++) {
			int d=dis(&p->p,&mon->p)-mon->dis;
			if (d<0) goto nextPosition;
			if (d<minMonDis) minMonDis=d;
		}
		bestRect=p->pData;bestP=p->p;return minMonDis;
nextPosition:
		continue;
	}
	return -1;
}
int autoTpEvade(char *packet) {
	if (!tAutoTeleportEvade.isOn) return 0;
	if (isAutoTeleporting||!packet) return 0;
	if (!tposs) {posCap=4096;tposs=(TPPos *)HeapAlloc(dllHeap,0,sizeof(TPPos)*posCap);}
	int x,y;
	switch (packet[0]) {
		case 0x0c: //right skill map
		case 0x0f: //Right skill (hold)      0f [WORD x] [WORD y] 
			x=*(short *)(packet+1);y=*(short *)(packet+3);
			break;
		case 0x0d: //right skill unit
		case 0x10: {//Right skill unit (hold)      10 [DWORD entity kind] [DWORD id] 
			int type=*(int *)(packet+1),id=*(int *)(packet+5);
			UnitAny *pUnit=d2client_GetUnitFromId(id,type);
			if (!pUnit||!isMonster(pUnit)) return 0;
			x=pUnit->pMonPath->wUnitX;y=pUnit->pMonPath->wUnitY;
			break;
		}
		default:return 0;
	}
	dst.x=x;dst.y=y;
	maxTpDisM256=maxTpYard*256*3/2;
	src.x=dwPlayerX;src.y=dwPlayerY;
	calBoundingBox();
	findMonsters(1);
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	maxDisM256=0x7FFFFFFF;nPos=0;
	addPossiblePostions(pData,pData,NULL,5);
	for (int i=0;i<pData->nearbyRectCount;i++) {
		AreaRectData *pNData=pData->paDataNear[i];if (pNData==pData) continue;
		addPossiblePostions(pData,pNData,NULL,5);
	}
	int monDis=findValidPosition();
	//LOG("AutoTpEvade mons=%d pos=%d dis=%d\n",nMons,nPos,monDis);
	if (monDis<0) return 0;
	nPos=0;addPossiblePostions(pData,bestRect,&bestP,5);
	monDis=findValidPosition();
	if (bestP.x!=x||bestP.y!=y) {
		char keyname[256];keyname[0]=0;
		formatKey(keyname,tAutoTeleportEvade.key);
		wchar_t wszbuf[256];
		if (dwGameLng) wsprintfW(wszbuf,L"自动躲避 (%d,%d) (%hs)",bestP.x-x,bestP.y-y,keyname);
		else wsprintfW(wszbuf,L"AutoEvade (%d,%d) (%hs)",bestP.x-x,bestP.y-y,keyname);
		setBottomAlertMsg(3,wszbuf,3000,1,0,6);
		*(short *)(packet+1)=(short)bestP.x;*(short *)(packet+3)=(short)bestP.y;
		switch (packet[0]) {
			case 0x0c: //right skill map
			case 0x0f: //Right skill (hold)      0f [WORD x] [WORD y] 
				break;
			case 0x0d: //right skill unit
				packet[0]=0xc;
				break;
			case 0x10: //Right skill unit (hold)      10 [DWORD entity kind] [DWORD id] 
				packet[0]=0xf;
				break;
			default:return 0;
		}
		SendPacket((BYTE *)packet,5);
		return 1;
	}
	return 0;
}
int BackToTown();
static void interactObj(UnitAny *pUnit) {
	if (isWaypointTxt(pUnit->dwTxtFileNo)) {
		takeWaypointToLevel(pUnit,actlvls[ACTNO]);
		return;
	}
	LOG("interactObj type=%d id=%d txt=%d mode=%d\n",pUnit->dwUnitType,pUnit->dwUnitId,pUnit->dwTxtFileNo,pUnit->dwMode);
	switch (pUnit->dwTxtFileNo) {
		case 268: //wirt's body
			if (pUnit->dwMode==1) {//opened
				UnitAny *leg=NULL;
				for (int i=0;i<128;i++) {
					for (UnitAny *pUnit=d2client_pUnitTable[UNITNO_ITEM*128+i];pUnit;pUnit=pUnit->pHashNext) {
						if (pUnit->dwUnitType!=UNITNO_ITEM) break;
						if (pUnit->dwTxtFileNo!=88) continue;
						if (!pUnit->pItemData) continue;
						if (pUnit->pItemData->dwOwnerId==dwPlayerId) {BackToTown();return;}
						if (pUnit->pItemData->dwOwnerId==-1) {leg=pUnit;goto found_leg;}
					}
				}
found_leg:
				if (leg&&getPlayerDistanceRaw(leg)<7) {
					LeftClickUnit(leg);
				}
			} else {
				LeftClickUnit(pUnit);
			}
			break;
		case 392:case 393:case 394:case 395:case 396://seal
			if (pUnit->dwMode==0) LeftClickUnit(pUnit);
			break;
		default:
			LeftClickUnit(pUnit);
	}
}
static void interact(POINT *src,int type,int txt) {
	POINT p;
	if (type!=2&&type!=5) return;
	for (int i=0;i<128;i++) {
		for (UnitAny *pUnit=d2client_pUnitTable[type*128+i];pUnit;pUnit=pUnit->pHashNext) {
			if (pUnit->dwUnitType!=type) break;
			if (pUnit->dwTxtFileNo!=txt) continue;
			p.x=pUnit->pItemPath->unitX;p.y=pUnit->pItemPath->unitY;
			if (dis(src,&p)<=reachDis+3) {
				if (type==2) interactObj(pUnit); 
				else LeftClickUnit(pUnit);
				return;
			}
		}
	}
}
extern int dwTotalMonsterCount;
static int autoTeleportStartMs,leftSkill;
int fAutoTeleporting=0,unexpectedMonCount=0,unexpectedMonTotalMs=0;
int AutoTeleportStart() {
	fAutoTeleporting=1;autoTeleportStartMs=dwCurMs;fTpAvoiding=0;fTpAutoTarget=0;nPrevMons=0;
	leftSkill=-1;
	if (dwAutoTeleportTelekinesisEnterChamber&&dwPlayerClass==1&&hasSkill(Skill_Telekinesis)) {
		switch (dwCurrentLevel) {
			case Level_TalRashaTomb1:case Level_TalRashaTomb2:case Level_TalRashaTomb3:
			case Level_TalRashaTomb4:case Level_TalRashaTomb5:case Level_TalRashaTomb6:case Level_TalRashaTomb7:
				leftSkill=dwLeftSkill;switchLeftSkill(Skill_Telekinesis);
		}
	}
	return 0;
}
int AutoTeleportEnd() {
	fAutoTeleporting=0;
	if (leftSkill>=0) switchLeftSkill(leftSkill);
	if (unexpectedMonCount) {
		switch (dwCurrentLevel) {
			case Level_TheWorldstoneChamber:break;
			default:
			gameMessage("%d monster appear after TP, avgMs=%d",unexpectedMonCount,unexpectedMonTotalMs/unexpectedMonCount);
		}
		unexpectedMonCount=0;unexpectedMonTotalMs=0;
	}
	return 0;
}
static int targetType=0,targetTxt=0,hasTarget=0,dstType=0,curdis;
static int startProcessMs=0,tpMonCount;
//0:failed 1:ok
static int teleportForward() {
	wchar_t buf[128];
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	if (tLogTP.isOn) QueryPerformanceCounter(&perfStart);
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	maxDisM256=(curdis-tpMinStep)*256;
	nPos=0;
	addPossiblePostions(pData,pData,NULL,5);
	int dx=dst.x-src.x,dy=dst.y-src.y;
	for (int i=0;i<pData->nearbyRectCount;i++) {
		AreaRectData *pNData=pData->paDataNear[i];if (pNData==pData) continue;
		int dtx=pNData->tileX-pData->tileX;
		int dty=pNData->tileY-pData->tileY;
		if (dx>=0) {
			if (dy>=0) {
				if (dtx<0||dty<0) continue;
			} else {
				if (dtx<0||dty>0) continue;
			}
		} else {
			if (dy>=0) {
				if (dtx>0||dty<0) continue;
			} else {
				if (dtx>0||dty>0) continue;
			}
		}
		addPossiblePostions(pData,pNData,NULL,5);
	}
	int monDis=findValidPosition();if (monDis<0) return 0;
	nPos=0;addPossiblePostions(pData,bestRect,&bestP,5);
	monDis=findValidPosition();
	int newDis=getDistanceM256(bestP.x-dst.x,bestP.y-dst.y)>>8;
	if (abs(src.x-bestP.x)<=3&&abs(src.y-bestP.y)<=3) {startProcessMs=dwCurMs+300;return 2;}
	if (tLogTP.isOn) {
		QueryPerformanceCounter(&perfEnd);QueryPerformanceFrequency(&perfFreq);
		int timeUs=(int)((perfEnd.QuadPart-perfStart.QuadPart)*1000000.0/perfFreq.QuadPart);
		LOG(" TP (%d,%d)->(%d,%d) dis=%d new_distance=%d mon=%d monDis=%d search=%dus last=%dms\n",
			src.x,src.y,bestP.x,bestP.y,
			getDistanceM256(bestP.x-src.x,bestP.y-src.y)>>8,newDis,nMons,monDis,timeUs,dwLastTpMs);
	}
	tpMonCount=dwTotalMonsterCount;
	dwTpMs=dwCurMs;TPtarget=bestP;TPtargetRect=bestRect;dwTpDoneMs=0;TPfailedRect=NULL;
	isAutoTeleporting=1;
	RightSkillPos(bestP.x,bestP.y);
	isAutoTeleporting=0;
	wsprintfW(buf,dwGameLng?L"距离%d 时间%d秒":L"distance %d time %d",rectCountFromTarget,
		(dwCurMs-autoTeleportStartMs)/1000);
	setBottomAlertMsg(2,buf,dwPlayerFcrMs+50,0,2,0);
	return 1;
}
static int findTelekinesisPosition() {
	TPPos *end=tposs+nPos;int maxDis=0;
	for (TPPos *p=tposs;p<end;p++) {
		int monDis=getMonDis(&p->p);
		if ((getDistanceM256(p->p.x-src.x,p->p.y-src.y)>>8)>dwAutoTeleportTelekinesisDistance) continue;
		if (d2common_IsLineBlocked(PLAYER->pMonPath->pAreaRectData,&src,&p->p,4)) continue;
		if (monDis<dwAutoTeleportTelekinesisSafeDistance) continue;
		if (monDis>maxDis) {
			maxDis=monDis;bestRect=p->pData;bestP=p->p;
		}
	}
	return maxDis;
}
static int findSafestPosition() {
	TPPos *end=tposs+nPos;int maxDis=0;
	for (TPPos *p=tposs;p<end;p++) {
		int monDis=getMonDis(&p->p);
		if (monDis>maxDis) {maxDis=monDis;bestRect=p->pData;bestP=p->p;}
	}
	return maxDis;
}
static int teleportToSafety() {
	wchar_t buf[128];
	int monDis=getMonDis(&src);
	if (monDis>=dwAutoTeleportSafeDistance) {
		startProcessMs=dwCurMs+300;return 0;
	}
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	maxDisM256=0x7FFFFFFF;nPos=0;
	addPossiblePostions(pData,pData,NULL,5);
	for (int i=0;i<pData->nearbyRectCount;i++) {
		AreaRectData *pNData=pData->paDataNear[i];if (pNData==pData) continue;
		addPossiblePostions(pData,pNData,NULL,5);
	}
	findSafestPosition();
	nPos=0;addPossiblePostions(pData,bestRect,&bestP,5);
	monDis=findSafestPosition();
	if (abs(src.x-bestP.x)<=3&&abs(src.y-bestP.y)<=3) {startProcessMs=dwCurMs+300;return 0;}
	if (bestRect!=pData) savePrevMonsters();
	if (tLogTP.isOn) 
		LOG("TP find safe (%d,%d) curdis=%d monDis=%d\n",bestP.x,bestP.y,curdis,monDis);
	tpMonCount=dwTotalMonsterCount;
	dwTpMs=dwCurMs;TPtarget=bestP;TPtargetRect=bestRect;dwTpDoneMs=0;TPfailedRect=NULL;
	isAutoTeleporting=1;
	RightSkillPos(bestP.x,bestP.y);
	isAutoTeleporting=0;
	wsprintfW(buf,dwGameLng?L"躲避%d 时间%d秒":L"evade %d time %d",rectCountFromTarget,
		(dwCurMs-autoTeleportStartMs)/1000);
	setBottomAlertMsg(2,buf,dwPlayerFcrMs+50,1,1,2);
	return 1;
}
static int tpToBoss() {
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	int curdisM256=disM256(&src,&dst);
	if (curdisM256<5*256) return 1;
	if (curdisM256<maxTpDisM256) {
		dwTpMs=dwCurMs;TPtarget=dst;TPtargetRect=NULL;dwTpDoneMs=0;TPfailedRect=NULL;
		isAutoTeleporting=1;
		RightSkillPos(dst.x,dst.y);
		isAutoTeleporting=0;
		return 1;
	}
	resetMonsters();
	maxDisM256=0x7FFFFFFF;nPos=0;
	for (int i=0;i<pData->nearbyRectCount;i++) addPossiblePostions(pData,pData->paDataNear[i],NULL,5);
	findValidPosition();
	nPos=0;addPossiblePostions(pData,bestRect,&bestP,5);
	findValidPosition();
	dwTpMs=dwCurMs;TPtarget=bestP;TPtargetRect=bestRect;dwTpDoneMs=0;TPfailedRect=NULL;
	isAutoTeleporting=1;
	RightSkillPos(bestP.x,bestP.y);
	isAutoTeleporting=0;
	return 1;
}
static int telekinesisTarget(UnitAny *pObject) {
	if (dwLeftSkill!=Skill_Telekinesis) return teleportToSafety()?0:1;
	wchar_t buf[128];
	if ((int)getPlayerDistanceRaw(pObject)<=dwAutoTeleportTelekinesisDistance) {
		POINT p2;p2.x=pObject->pMonPath->wUnitX;p2.y=pObject->pMonPath->wUnitY;
		if (!d2common_IsLineBlocked(PLAYER->pMonPath->pAreaRectData,&src,&p2,4)) {
			ShiftLeftClickUnit(pObject);startProcessMs=dwCurMs+100;
			return 0;
		}
	}
	AreaRectData *pData = PLAYER->pMonPath->pAreaRectData;
	maxDisM256=0x7FFFFFFF;nPos=0;
	addPossiblePostions(pData,pData,NULL,5);
	for (int i=0;i<pData->nearbyRectCount;i++) {
		AreaRectData *pNData=pData->paDataNear[i];if (pNData==pData) continue;
		addPossiblePostions(pData,pNData,NULL,5);
	}
	bestRect=NULL;
	int monDis=findTelekinesisPosition();
	if (!bestRect) return teleportToSafety()?0:1;
	if (abs(src.x-bestP.x)<=3&&abs(src.y-bestP.y)<=3) {startProcessMs=dwCurMs+300;return 0;}
	if (bestRect!=pData) savePrevMonsters();
	if (tLogTP.isOn) 
		LOG("TP telekinesis (%d,%d) curdis=%d monDis=%d\n",bestP.x,bestP.y,curdis,monDis);
	tpMonCount=dwTotalMonsterCount;
	dwTpMs=dwCurMs;TPtarget=bestP;TPtargetRect=bestRect;dwTpDoneMs=0;TPfailedRect=NULL;
	RightSkillPos(bestP.x,bestP.y);
	wsprintfW(buf,dwGameLng?L"法师之手%d 时间%d秒":L"evade %d time %d",rectCountFromTarget,
		(dwCurMs-autoTeleportStartMs)/1000);
	setBottomAlertMsg(2,buf,dwPlayerFcrMs+50,1,1,2);
	return 1;
}
extern int dwDuranceOfHateLevel3TeleportShift[2];
static int autoDuranceofHateLevel3() {
	int scanX=dst.x-dwDuranceOfHateLevel3TeleportShift[0]-18;
	int scanY1=dst.y-dwDuranceOfHateLevel3TeleportShift[1]-24;
	int scanY2=dst.y-dwDuranceOfHateLevel3TeleportShift[1]+8;
	if (searchBox.left<scanX) {
		if (searchBox.right>scanX) searchBox.right=scanX;
	}
	return teleportToSafety()?0:1;
}
//return 1 if auto skill can be used
static int autoTarget() {
	int kbMode=!fIsHardCoreGame&&(dwLeftSkill==Skill_ChargedStrike||dwLeftSkill==Skill_Smite);
	switch (dwCurrentLevel) {
		case Level_ArcaneSanctuary:
		case Level_HallsofVaught:
			if (dwPlayerClass==1) {
				if (DIFFICULTY==2) {
					//if (bossHp>0) {dst.x=bossX;dst.y=bossY;}
					curdis=getDistanceM256(dst.x-src.x,dst.y-src.y)>>8;
					//LOG("Summoner %d\n",curdis);
					if (curdis<15) {
						if (dwCurrentLevel==Level_HallsofVaught) BackToTown();
						startProcessMs=dwCurMs+100;
						return 0;
					}
					if (teleportForward()) return 0;
				}
			}
			if (kbMode) {
				if (dwCurrentLevel==Level_ArcaneSanctuary&&(GAMEQUESTDATA->quests[14]&0x8000)) {
					if (findBoss(Mon_TheSummoner,0)) return tpToBoss();
				} else if (dwCurrentLevel==Level_HallsofVaught) {
					if (findBoss(Mon_Nihlathak,1)) return tpToBoss();
				}
			}
			break;
		case Level_TalRashaTomb1:case Level_TalRashaTomb2:case Level_TalRashaTomb3:
		case Level_TalRashaTomb4:case Level_TalRashaTomb5:case Level_TalRashaTomb6:case Level_TalRashaTomb7:
			if (dwLeftSkill==Skill_Telekinesis&&dwAutoTeleportTelekinesisEnterChamber) {
				int txt=getAreatileTxtToLevel(Level_TalRashaChamber);
				if (txt) {
					UnitAny *portal=getAreatileByTxt(txt);
					if (portal) return telekinesisTarget(portal);
				}
			}
			break;
		case Level_DuranceofHateLevel3:
			return autoDuranceofHateLevel3();
	}
	return teleportToSafety()?0:1;
}
//return 1 if auto skill can be used
int AutoTeleport() {
	if (!tposs) {posCap=4096;tposs=(TPPos *)HeapAlloc(dllHeap,0,sizeof(TPPos)*posCap);}
	targetType=0;targetTxt=0;hasTarget=0;dstType=0;
	if (src.x==dwPlayerX&&src.y==dwPlayerY) {
		if (dwCurMs-dwTpMs<300&&dwTotalMonsterCount>tpMonCount) {
			int n=dwTotalMonsterCount-tpMonCount;
			unexpectedMonCount+=n;unexpectedMonTotalMs+=n*(dwCurMs-dwTpMs);
			tpMonCount=dwTotalMonsterCount;
		}
	} else if (!dwTpDoneMs) {
		dwTpDoneMs=dwCurMs;dwLastTpMs=dwTpDoneMs-dwTpMs;
		if (dwTpMs&&abs(dwPlayerX-TPtarget.x)>3&&abs(dwPlayerY-TPtarget.y)>3
			&&abs(dwPlayerX-src.x)<3&&abs(dwPlayerY-src.y)<3) {
			TPfailedRect=TPtargetRect;
			LOG("Tp failed\n");
		}
		startProcessMs=dwCurMs+tpDelayMs;
	}
	if (dwCurMs<startProcessMs) return 0;
	if (!fCanUseRightSkill||dwRightSkill!=Skill_Teleport||fPlayerInTown||fAutoFollowMoving) return 0;
	if (PLAYER->pInventory->pCursorItem) return 0;
	if (PLAYER->dwMode==PlayerMode_Attacking1||PLAYER->dwMode==PlayerMode_Attacking2||PLAYER->dwMode==PlayerMode_Cast)
		return 0;
	if (!fIsHardCoreGame) {
		switch (dwCurrentLevel) {
			case Level_TheWorldstoneChamber:
				if (findBoss(Mon_BaalCrab,0)) hasTarget=1;
				break;
			case Level_ChaosSanctuary:
				if (DIFFICULTY==0&&dwPlayerLevel>=forceEnterCharLevelNormal
					||DIFFICULTY==1&&dwPlayerLevel>=forceEnterCharLevelNightmare) {
					if (nBoss>0&&bossHps[0]>0) {dst.x=bossX;dst.y=bossY;hasTarget=1;}
				}
				break;
			//case Level_TowerCellarLevel5:if (findBoss(Mon_DarkStalker,1)) hasTarget=1;break;
		}
	}
	maxTpDisM256=maxTpYard*256*3/2;
	src.x=dwPlayerX;src.y=dwPlayerY;
	calBoundingBox();
	if (hasTarget) return tpToBoss();
	for (int retries=0;retries<6;retries++) {
		dstType=AutoTeleportGetTarget(&dst,&targetType,&targetTxt,&rectCountFromTarget);
		//0:noTarget 1:continue 2:end 3:interact 4:forceEnter 5:auto >=6:keep safe distance
		if (dstType>=6&&!fIsHardCoreGame) {
			if (DIFFICULTY==0&&dwPlayerLevel>=forceEnterCharLevelNormal
				||DIFFICULTY==1&&dwPlayerLevel>=forceEnterCharLevelNightmare) {
				switch (dwCurrentLevel) {
				case Level_StonyField:
				case Level_MaggotLairLevel3:dstType=2;break;
				case Level_ChaosSanctuary:dstType=3;break;
				}
			}
		}
		if (!dstType) {dstType=60;fTpAvoiding=1;} //route failed
		if (!dstType) {startProcessMs=dwCurMs+200;return 1;}
		if (dstType!=4) findMonsters(0);
		else resetMonsters();
		curdis=getDistanceM256(dst.x-src.x,dst.y-src.y)>>8;
		if (dstType==5) {
			switch (dwCurrentLevel) {
				case Level_RiverofFlame:
					if (DIFFICULTY>=2) dstType=60;
					else {
						if (curdis<10) dstType=2;	
						else dstType=1;
					}
					break;
			}
		}
		if (curdis<reachDis&&dstType==3) {
			interact(&src,targetType,targetTxt);startProcessMs=dwCurMs+300;return 0;
		}
		if (fTpAvoiding||dstType>=6&&curdis<dstType) { //find safe place
			fTpAvoiding=1;
			return teleportToSafety()?0:1;
		}
		if (fTpAutoTarget||dstType==5&&curdis<20) {
			fTpAutoTarget=1;
			return autoTarget();
		}
		if (dstType>=2&&curdis<reachDis) { //reached
			int monDis=getMonDis(&src);
			if (monDis>=dwAutoTeleportSafeDistance) {
				//if (tLogTP.isOn) LOG("reached dst=%d monDis=%d\n",curdis,monDis);
				startProcessMs=dwCurMs+300;return 2;
			} else {
				return teleportToSafety()?0:1;
			}
		}
		if (teleportForward()) return 0;
		if (dstType<2) {
			markNextPathInvalid(stepInvalidMs);
			RerouteRectPath();
			continue;
		}
		return teleportToSafety()?0:1;
	}
	return 0;
}
