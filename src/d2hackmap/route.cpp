#include "stdafx.h"
#include "header.h"
#include "map.h"

void AutoMapRoute();
int RerouteRectPath();

static int minimapDrawN,scrDrawN,nCachedRoute,playerYard,playerRaw;
static BYTE tpLineColor,routeDebugLineColor;
ToggleVar tAutomapRouteInfo={TOGGLEVAR_ONOFF,0,-1,1,"AutomapRouteInfo"};
ToggleVar tScreenRouteGuide={TOGGLEVAR_ONOFF,0,-1,1,"ScreenRouteInfo"};
ToggleVar tReroute={TOGGLEVAR_DOWN,0,-1,1,"Reroute",&RerouteRectPath};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_INT, "MinimapGuideLength",       &minimapDrawN,      4},
	{CONFIG_VAR_TYPE_INT, "ScreenGuideLength",       &scrDrawN,      4},
	{CONFIG_VAR_TYPE_INT, "ScreenGuidePlayerYard",       &playerYard,      4},
	{CONFIG_VAR_TYPE_INT, "CachedRouteData",     &nCachedRoute,      4},
  {CONFIG_VAR_TYPE_KEY, "AutomapRouteInfoToggle",		&tAutomapRouteInfo     },
  {CONFIG_VAR_TYPE_KEY, "ScreenGuideToggle",		&tScreenRouteGuide     },
  {CONFIG_VAR_TYPE_KEY, "RerouteKey",		&tReroute     },
	{CONFIG_VAR_TYPE_INT,"TPLineColor",&tpLineColor,1},
	{CONFIG_VAR_TYPE_INT,"RouteDebugLineColor",&routeDebugLineColor,1},
};
void autoroute_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void autoroute_fixValues() {playerRaw=(playerYard*3)>>1;}
struct RouteRect { //40*40
	int id,tileX,tileY,tileW,tileH,blocks,valid,emitting;
	int left,right,top,bottom;
	struct RouteRect *dir[4];
	int skipDirMs[4];
	struct RouteRect *next,*emitNext;
	int dis,unitX,unitY,drawX,drawY; //path point
	struct RouteRect *pathNext;int pathDir;
	MinimapLevelTarget *pTarget;
};
struct RouteTile { //5*5
	int id,tileX,tileY,emitting;
	struct RouteTile *dir[4];
	struct RouteTile *next,*emitNext;
	struct RouteTile *pathNext;int pathDir;
	MinimapLevelTarget *pTarget;
};

extern BYTE routeLineColor;
extern BYTE nAutomapReadyRectColors;
extern MinimapLevel minimapLevels[140];
extern ToggleVar tMinimap;
extern CellFile* pMinimapObjectCell;
void DrawAutomapNode(CellFile *pCellfile, int xpos, int ypos, char color); 
void mapReveal(AreaRectInfo *pInfo);
void mapHide(AreaRectInfo *pInfo);

static MinimapLevelTarget *recentTargets;
static RouteRect *rectRecycle=NULL;
static RouteTile *tileRecycle=NULL;
static int autoRouteMemUsed;

RouteRect *pSrcRect=NULL;
void AutoRouteNewGame() {
	autoRouteMemUsed=0;rectRecycle=NULL;recentTargets=NULL;
}
static void drawAARect(RouteRect *pr) {
	POINT topLeft, topRight, bottomLeft, bottomRight;
	int color=nAutomapReadyRectColors;//0x6F;
	tile2map(&topLeft,		 pr->tileX,					pr->tileY);
	tile2map(&topRight,		 pr->tileX+pr->tileW,	pr->tileY);
	tile2map(&bottomLeft,	 pr->tileX,					pr->tileY+pr->tileH);
	tile2map(&bottomRight,   pr->tileX+pr->tileW,	pr->tileY+pr->tileH);
	d2gfx_DrawLine(topLeft.x,		topLeft.y,	topRight.x,	 topRight.y,		color, -1);
	d2gfx_DrawLine(bottomLeft.x,	bottomLeft.y, bottomRight.x, bottomRight.y,	color, -1);
	d2gfx_DrawLine(topLeft.x,		topLeft.y,	bottomLeft.x,  bottomLeft.y,	color, -1);
	d2gfx_DrawLine(topRight.x,	topRight.y,	bottomRight.x, bottomRight.y,	color, -1);
}
extern int dwLevelChangeMs;
void RevealAutomapLevel(DrlgLevel *pDrlgLevel,int mode);
int AutoRouteLocateSource(int canRoute) {
	MinimapLevel *pMapLevel=&minimapLevels[dwCurrentLevel];
	MinimapLevelTarget *pTarget=pMapLevel->curTarget;
	if (!pTarget) return 0;
	if (!pTarget->rectRoute&&canRoute&&dwCurMs-dwLevelChangeMs>100) {
		DrlgLevel *pDrlgLevel = PLAYER->pMonPath->pAreaRectData->pAreaRectInfo->pDrlgLevel;
		if (pDrlgLevel) RevealAutomapLevel(pDrlgLevel,1);
		AutoMapRoute();
	}
	if (!pTarget->rectRoute) return 0;
	AreaRectInfo *pSrcInfo=PLAYER->pMonPath->pAreaRectData->pAreaRectInfo;
	if (pSrcRect&&pSrcRect->pTarget==pTarget&&pSrcRect->tileX==pSrcInfo->tileX&&pSrcRect->tileY==pSrcInfo->tileY) 
		return 1;
	if (pSrcRect) {
		for (int i=0;i<4;i++) {
			struct RouteRect *pr=pSrcRect->dir[i];
			if (pr&&pr->tileX==pSrcInfo->tileX&&pr->tileY==pSrcInfo->tileY) {pSrcRect=pr;return 1;}
		}
	}
	for (RouteRect *pr=pTarget->rectRoute;pr;pr=pr->next) {
		if (pr->tileX==pSrcInfo->tileX&&pr->tileY==pSrcInfo->tileY) {pSrcRect=pr;return 1;}
	}
	pSrcRect=NULL;return 0;
}
int dwTargetDistance,dwTargetDistanceMs;
extern POINT TPtarget;extern int dwTpMs,dwTpDoneMs;
extern ToggleVar tMinimapDebugToggle;
void minimapDebug() {
	AreaRectData *pAreaRectData = PLAYER->pMonPath->pAreaRectData;
	int sx=d2client_GetScreenDrawX(),sy=d2client_GetScreenDrawY();
	for (int i=0;i<pAreaRectData->nearbyRectCount;i++) {
		AreaRectData *pData=pAreaRectData->paDataNear[i];
		AreaRectInfo *pInfo=pData->pAreaRectInfo;
		for (PresetUnit *pUnit=pInfo->pPresetUnits;pUnit;pUnit=pUnit->pNext) {
			int map=0;
			if (pUnit->dwUnitType==UNITNO_OBJECT) {
				ObjectTxt *pObj = d2common_GetObjectTxt(pUnit->dwTxtFileNo);
				if (pObj) map=pObj->nAutoMap;
			}
			int unitX=pData->unitX+pUnit->unitDx;
			int unitY=pData->unitY+pUnit->unitDy;
			int drawX = (unitX-unitY)*16;
			int drawY = (unitX+unitY)*8;
			wchar_t temp[16];
			wsprintfW(temp,L"%d:%d:%d",pUnit->dwUnitType,pUnit->dwTxtFileNo,map);
			drawBgTextMiddle(temp,drawX-sx,drawY-sy,0,0x10);
		}
	}
}
void AutoRouteDrawScreenPath() {
	if (tMinimapDebugToggle.isOn) minimapDebug();
	if (!scrDrawN) return;
	if (!AutoRouteLocateSource(0)) return;
	int sx=d2client_GetScreenDrawX(),sy=d2client_GetScreenDrawY();
	RouteRect *path=pSrcRect;if (!path->dis) return;
	int x1=dwPlayerX,y1=dwPlayerY;
	int n=0,color=routeLineColor;
	for (RouteRect *pp=path->pathNext?path->pathNext:path;pp;pp=pp->pathNext) {
		int x2=pp->drawX-sx,y2=pp->drawY-sy;
		if (n==0) {
			int dx=pp->unitX-x1,dy=pp->unitY-y1;
			int dis=getDistanceM256(dx,dy)>>8;
			dwTargetDistance=dis+((pp->dis>>16)-1)*pp->tileW*5;dwTargetDistanceMs=dwCurMs+500;
			if (dis<=playerRaw) {x1+=dx>>1;y1+=dy>>1;}
			else {x1+=dx*playerRaw/dis;y1+=dy*playerRaw/dis;}
			int t=(x1-y1)<<4;y1=(x1+y1)<<3;x1=t;
			x1-=sx;y1-=sy;
		}
		if (!pp->pathNext) {
			int dx=x2-x1,dy=y2-y1;
			if (abs(dx)<15&&abs(dy)<15) break;
		} 
		d2gfx_DrawLine(x1,y1,x2,y2,color,-1);x1=x2;y1=y2;
		if (n++>scrDrawN) break;
	}
	if (tpLineColor&&dwTpDoneMs<dwTpMs&&dwCurMs-dwTpMs<=dwPlayerFcrMs) {
		d2gfx_DrawLine(PLAYER->pMonPath->drawX-sx,PLAYER->pMonPath->drawY-sy,
			((TPtarget.x-TPtarget.y)<<4)-sx,((TPtarget.x+TPtarget.y)<<3)-sy,tpLineColor,-1);
	}
}
void AutoRouteDrawMinimapPath() {
	if (tpLineColor&&dwTpDoneMs<dwTpMs&&dwCurMs-dwTpMs<=dwPlayerFcrMs) {
		POINT p1;draw2map(&p1,PLAYER->pMonPath->drawX,PLAYER->pMonPath->drawY);
		POINT p2;draw2map(&p2,(TPtarget.x-TPtarget.y)<<4,(TPtarget.x+TPtarget.y)<<3);
		d2gfx_DrawLine(p1.x,p1.y,p2.x,p2.y,tpLineColor,-1);
	}
	if (tAutomapRouteInfo.isOn) {
		MinimapLevel *pMapLevel=&minimapLevels[dwCurrentLevel];
		if (pMapLevel->curTarget) {
			wchar_t buf[64];
			for (RouteRect *pr=pMapLevel->curTarget->rectRoute;pr;pr=pr->next) {
				drawAARect(pr);
				if (pr->pathNext) {
					POINT p1;draw2map(&p1,pr->drawX,pr->drawY);
					POINT p2;draw2map(&p2,pr->pathNext->drawX,pr->pathNext->drawY);
					d2gfx_DrawLine(p1.x,p1.y,p2.x,p2.y,routeDebugLineColor,-1);
				}
				POINT pt;tile2map(&pt,pr->tileX+(pr->tileW>>1),pr->tileY+(pr->tileH>>1));
				int pos=0;
				pos+=wsprintfW(buf+pos,L"%d:",pr->id);
				if (pr->dis) pos+=wsprintfW(buf+pos,L"%d",pr->dis>>16);
				drawBgTextMiddle(buf,pt.x,pt.y-6,0,0x10);
			}
		}
	}
	if (!minimapDrawN) return;
	if (!AutoRouteLocateSource(0)) return;
	RouteRect *path=pSrcRect;if (!path->dis) return;
	POINT p1;draw2map(&p1,PLAYER->pMonPath->drawX,PLAYER->pMonPath->drawY);
	int n=0,color=routeLineColor;
	for (RouteRect *pp=path->pathNext?path->pathNext:path;pp;pp=pp->pathNext) {
		POINT p2;draw2map(&p2,pp->drawX,pp->drawY);
		if (n==0) {
			int dx=p2.x-p1.x,dy=p2.y-p1.y;
			int dis=getDistanceM256(dx,dy)>>8;
			dwTargetDistance=dis+((pp->dis>>16)-1)*pp->tileW*5;dwTargetDistanceMs=dwCurMs+500;
			p1.x+=dx>>2;
			p1.y+=dy>>2;
		}
		if (!pp->pathNext) {
			int dx=p2.x-p1.x,dy=p2.y-p1.y;
			if (abs(dx)<15&&abs(dy)<15) break;
			d2gfx_DrawLine(p1.x,p1.y,p2.x,p2.y,color,-1);p1=p2;
			if (pMinimapObjectCell) {
				DrawAutomapNode(pMinimapObjectCell,p2.x,p2.y,color);
			} else {
				wchar_t temp[8]={0xff,'c','0'+color,'*',0};
				DrawCenterText(6, temp, p2.x, p2.y, color);
			}
			break;
		} else {
			d2gfx_DrawLine(p1.x,p1.y,p2.x,p2.y,color,-1);p1=p2;
		}
		if (n++>minimapDrawN) break;
	}
}
void markNextPathInvalid(int ms) {
	MinimapLevel *pMapLevel=&minimapLevels[dwCurrentLevel];
	MinimapLevelTarget *pTarget=pMapLevel->curTarget;
	if (!AutoRouteLocateSource(0)) return;if (!pSrcRect) return;
	if (!pSrcRect->pathNext) return;
	pSrcRect->skipDirMs[pSrcRect->pathDir]=dwCurMs+ms;
}
//return 0:noTarget 1:continue 2:end 3:enter 4:forceEnter 5:auto >=6:keep safe distance
int AutoTeleportGetTarget(POINT *dst,int *unitType,int *unitTxt,int *pdis) {
	if (!AutoRouteLocateSource(1)) return 0;if (!pSrcRect) return 0;
	MinimapLevel *pMapLevel=&minimapLevels[dwCurrentLevel];
	MinimapLevelTarget *pTarget=pMapLevel->curTarget;
	if (!pTarget) return 0;
	*unitType=pTarget->p.unitType;*unitTxt=pTarget->p.unitTxt;
	RouteRect *path=pSrcRect;if (!path->dis) return 0;
	int forward=0;
	if (path->pathNext) {path=path->pathNext;forward++;}
	if (path->pathNext) {path=path->pathNext;forward++;}
	dst->x=path->unitX;dst->y=path->unitY;
	if (pdis) *pdis=(path->dis>>16)-1+forward;
	if (path->pathNext) return 1;
	if (path->pTarget->type==1) return 3;
	if (path->pTarget->type==2) return 4;
	if (path->pTarget->type==3) return 5;
	if (path->pTarget->type>=5) return path->pTarget->type;
	return 2;
}
static int canTeleport(AreaRectData *pData,RouteRect *pr) {
	if (!pData||!pData->bitmap) return 0;
	unsigned short *ptr=pData->bitmap->bitmap;
	int w=pData->bitmap->unitW,h=pData->bitmap->unitH;
	int n=w*h;
	unsigned short *endptr=ptr+n;
	int blocks=0;
	pr->top=h;pr->bottom=-1;pr->left=w;pr->right=-1;
	for (int y=0;y<h;y++) {
		for (int x=0;x<w;x++) {
			if ((*ptr++)&0x427) {blocks++;continue;}
			if (x<pr->left) pr->left=x;
			if (x>pr->right) pr->right=x;
			if (y<pr->top) pr->top=y;
			if (y>pr->bottom) pr->bottom=y;
		}
	}
	pr->blocks=blocks;pr->valid=blocks<n;
	return pr->valid;
}
static void recycleRouteInfo(MinimapLevelTarget *pTarget) {
	int n=0;
	for (MinimapLevelTarget *pLvl=recentTargets;pLvl;pLvl=pLvl->recentNext) {
		if (pLvl->recentNext==pTarget) {pLvl->recentNext=pTarget->recentNext;continue;}
		n++;
	}
	pTarget->recentNext=recentTargets;recentTargets=pTarget;n++;
	if (nCachedRoute<3) nCachedRoute=3;
	if (n>nCachedRoute) {
		MinimapLevelTarget *pFree=NULL;
		for (MinimapLevelTarget *pLvl=recentTargets;pLvl&&pLvl->recentNext;pLvl=pLvl->recentNext) {
			if (!pLvl->recentNext->recentNext) {pFree=pLvl->recentNext;pLvl->recentNext=NULL;break;}
		}
		if (pFree) {
			int nf=0;
			while (pFree->rectRoute) {
				RouteRect *pr=pFree->rectRoute;pFree->rectRoute=pr->next;
				pr->next=rectRecycle;rectRecycle=pr;nf++;
			}
			LOG("Auto route free lvl=%d recyle=%d\n",pFree->level->lvl,nf);
		}
	}
}
extern int dwBarbrianLeft;
static int initRectRoute(MinimapLevelTarget *pTarget) {
	if (pTarget->rectRoute) return 0;
	LOG("AutoRouteInit lvl=%d minimap target %d/%d\n",dwCurrentLevel,pTarget->id,pTarget->level->n);
	AreaRectInfo *pSrcInfo=PLAYER->pMonPath->pAreaRectData->pAreaRectInfo;
	DrlgLevel *pDrlgLevel=pSrcInfo->pDrlgLevel;
	pSrcRect=NULL;
	if (recentTargets!=pTarget) recycleRouteInfo(pTarget);
	int startMs=GetTickCount();
	int id=0;
	for (AreaRectInfo *pInfo=pDrlgLevel->pAreaRectInfo;pInfo;pInfo=pInfo->pNext) {
		RouteRect *pr=rectRecycle;if (pr) rectRecycle=pr->next;
		if (!pr) {
			pr=(RouteRect *)HeapAlloc(gameHeap,HEAP_ZERO_MEMORY,sizeof(RouteRect));
			autoRouteMemUsed+=sizeof(RouteRect);
		} else memset(pr,0,sizeof(RouteRect));
		pr->next=pTarget->rectRoute;pTarget->rectRoute=pr;pr->pTarget=pTarget;
		pr->id=id++;pr->tileX=pInfo->tileX;pr->tileY=pInfo->tileY;pr->tileW=pInfo->tileW;pr->tileH=pInfo->tileH;
		if (pInfo==pSrcInfo) pSrcRect=pr;
		if (pTarget->ready&2) {
			int ux=pr->tileX*5,uy=pr->tileY*5,uw=pr->tileW*5,uh=pr->tileH*5;
			if (ux<=pTarget->p.unitX&&pTarget->p.unitX<ux+uw&&uy<=pTarget->p.unitY&&pTarget->p.unitY<uy+uh)
				pTarget->dstRect=pr;
		} else {
			if (pr->tileX==pTarget->p.rectTileX&&pr->tileY==pTarget->p.rectTileY) pTarget->dstRect=pr;
		}
		int needHide=0;
		if (!pInfo->pAreaRectData) {mapReveal(pInfo);needHide=1;}
		canTeleport(pInfo->pAreaRectData,pr);
		if (needHide) mapHide(pInfo);
	}
	//find nearby rect
	for (RouteRect *p1=pTarget->rectRoute;p1;p1=p1->next) {
		for (RouteRect *p2=pTarget->rectRoute;p2;p2=p2->next) {
			if (p1->tileX==p2->tileX) {
				if (p1->tileY<p2->tileY&&p1->tileY+p1->tileH==p2->tileY) {p1->dir[AAR_BOTTOM]=p2;p2->dir[AAR_TOP]=p1;}
				else if (p1->tileY>p2->tileY&&p1->tileY==p2->tileY+p2->tileH) {p1->dir[AAR_TOP]=p2;p2->dir[AAR_BOTTOM]=p1;}
			} else if (p1->tileY==p2->tileY) {
				if (p1->tileX<p2->tileX&&p1->tileX+p1->tileW==p2->tileX) {p1->dir[AAR_RIGHT]=p2;p2->dir[AAR_LEFT]=p1;}
				else if (p1->tileX>p2->tileX&&p1->tileX==p2->tileX+p2->tileW) {p1->dir[AAR_LEFT]=p2;p2->dir[AAR_RIGHT]=p1;}
			}
		}
	}
	switch (dwCurrentLevel) {
		case Level_FrigidHighlands:
			if (pTarget->dstLvl==3002) { //rescue mission
				if (pTarget->dstRect&&pTarget->dstRect->dir[AAR_RIGHT]) {
					int done=QUESTDATA->quests[36]&3;
					if (done)
						pTarget->dstRect->dir[AAR_RIGHT]->valid=0;
				}
			}
			break;
		case Level_Travincal:
			if (pTarget->dstLvl==Level_DuranceofHateLevel1) {
				if (pTarget->dstRect&&pTarget->dstRect->dir[AAR_BOTTOM]) {
					pTarget->dstRect->dir[AAR_BOTTOM]->valid=0;
				}
			}
			break;
	}
	LOG("time %d ms memory=%dKB\n",GetTickCount()-startMs,autoRouteMemUsed/1024);
	return 0;
}
extern int maxTpDisRaw;
static void performRectRoute(MinimapLevelTarget *pTarget) {
	int startMs=GetTickCount();
	int pathId=0;
	for (RouteRect *pr=pTarget->rectRoute;pr;pr=pr->next) {
		pr->dis=0;pr->emitting=0;pr->pathNext=NULL;
	}
	RouteRect *head=NULL,*tail=NULL;head=tail=pTarget->dstRect;if (!head) return;
	pTarget->ready|=4;
	head->unitX=pTarget->p.unitX;head->unitY=pTarget->p.unitY;
	head->drawX=(pTarget->p.unitX-pTarget->p.unitY)*16;
	head->drawY=(pTarget->p.unitX+pTarget->p.unitY)*8;
	head->dis=0x10000;
	head->emitNext=NULL;
	while (head) {
		RouteRect *pr=head;head=head->emitNext;if (!head) tail=NULL;
		pr->emitting=0;
		int distance=(pr->dis&0xFFFF0000)+0x10000+pr->blocks;
		for (int i=0;i<4;i++) {
			RouteRect *r=pr->dir[i];if (!r||!r->valid) continue;
			if (r->dis&&r->dis<=distance) continue;
			if (r->skipDirMs[3-i]) {
				if (dwCurMs<r->skipDirMs[3-i]) continue;
				r->skipDirMs[3-i]=0;
			}
			int d;
			switch (i) {
				case AAR_TOP:d=r->tileH*5 + pr->top - r->bottom;break;
				case AAR_BOTTOM:d=pr->tileH*5 + r->top - pr->bottom;break;
				case AAR_LEFT:d=r->tileW*5 + pr->left - r->right;break;
				case AAR_RIGHT:d=pr->tileW*5 + r->left - pr->right;break;
			}
			if (d>maxTpDisRaw) continue;
			r->dis=distance;r->pathNext=pr;r->pathDir=3-i;
			if (!r->emitting) {
				r->emitting=1;
				if (!head) {head=tail=r;r->emitNext=NULL;}
				else {tail->emitNext=r;r->emitNext=NULL;tail=r;}
			}
		}
	}
	//calculate path points
	for (RouteRect *pr=pTarget->rectRoute;pr;pr=pr->next) {
		if (!pr->pathNext) continue;
		int x,y;
		switch (pr->pathDir) {
		case AAR_TOP:x=(pr->left+pr->right)>>1;y=pr->top;break;
		case AAR_BOTTOM:x=(pr->left+pr->right)>>1;y=pr->bottom;break;
		case AAR_LEFT:x=pr->left;y=(pr->top+pr->bottom)>>1;break;
		case AAR_RIGHT:x=pr->right;y=(pr->top+pr->bottom)>>1;break;
		}
		x=x+pr->tileX*5;y=y+pr->tileY*5;
		pr->unitX=x;pr->unitY=y;pr->drawX=(x-y)<<4;pr->drawY=(x+y)<<3;
	}
	LOG("Rect level route time %d ms\n",GetTickCount()-startMs);
}
static int checkTile(AreaRectData *pData,RouteTile *pr) {
	
}
static int initTileRoute(MinimapLevelTarget *pTarget) {
	if (pTarget->tileRoute) return 0;
	LOG("tileRouteInit lvl=%d minimap target %d/%d\n",dwCurrentLevel,pTarget->id,pTarget->level->n);
	AreaRectInfo *pSrcInfo=PLAYER->pMonPath->pAreaRectData->pAreaRectInfo;
	DrlgLevel *pDrlgLevel=pSrcInfo->pDrlgLevel;
	if (recentTargets!=pTarget) recycleRouteInfo(pTarget);
	int startMs=GetTickCount();
	int id=0;
	for (AreaRectInfo *pInfo=pDrlgLevel->pAreaRectInfo;pInfo;pInfo=pInfo->pNext) {
		RouteTile *pr=tileRecycle;if (pr) tileRecycle=pr->next;
		if (!pr) {
			pr=(RouteTile *)HeapAlloc(gameHeap,HEAP_ZERO_MEMORY,sizeof(RouteTile));
			autoRouteMemUsed+=sizeof(RouteTile);
		} else memset(pr,0,sizeof(RouteTile));
		pr->next=pTarget->tileRoute;pTarget->tileRoute=pr;pr->pTarget=pTarget;
		pr->id=id++;pr->tileX=pInfo->tileX;pr->tileY=pInfo->tileY;
		//if (pr->tileX==pTarget->tileX&&pr->tileY==pTarget->tileY) pTarget->dstRect=pr;
		int needHide=0;
		if (!pInfo->pAreaRectData) {mapReveal(pInfo);needHide=1;}
		checkTile(pInfo->pAreaRectData,pr);
		if (needHide) mapHide(pInfo);
	}
	LOG("time %d ms memory=%dKB\n",GetTickCount()-startMs,autoRouteMemUsed/1024);
	return 0;
}
int RerouteRectPath() {
	char keyname[256];
	MinimapLevel *pMapLevel=&minimapLevels[dwCurrentLevel];
	MinimapLevelTarget *pTarget=pMapLevel->curTarget;
	if (!pTarget||!pTarget->ready) return 0;
	if (!pTarget->rectRoute) initRectRoute(pTarget);
	int ms=GetTickCount();
	performRectRoute(pTarget);
	formatKey(keyname,tReroute.key);
	gameMessage("reroute in %d ms (%s)",GetTickCount()-ms,keyname);
	return 0;
}
void AutoMapRoute() {
	switch (dwCurrentLevel) {case 40:case 75:case 103:case 109:case 137:return;}
	MinimapLevel *pMapLevel=&minimapLevels[dwCurrentLevel];
	MinimapLevelTarget *pTarget=pMapLevel->curTarget;
	if (!pTarget||!pTarget->ready) return;
	if (pTarget->rectRoute) return;
	initRectRoute(pTarget);
	performRectRoute(pTarget);
}
