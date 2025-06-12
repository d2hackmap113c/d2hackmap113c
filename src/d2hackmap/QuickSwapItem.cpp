#include "stdafx.h"
#include "header.h"
#include "d2BinFile.h"

//InvAma=0,InvSor=1,InvNec=2,InvPal=3,InvBar=4,Inv5=5,Inv6=6,Inv7=7,InvStashClassic=8,
//InvCube=9,Inv10=10,Inv11=11,InvStashExpansion=12,Inv13=13,InvDru=14,InvAsn=15
extern int dwQuickSwapItemDelayMs;
ToggleVar tRightClickSwap={TOGGLEVAR_ONOFF,	1,	-1,	1 ,	"Right Click Swap"};
ToggleVar tQuickDropToggle={TOGGLEVAR_ONOFF,	0,	-1,	1 , "Quick Drop"};
ToggleVar tEnableGemAutoMerge={TOGGLEVAR_ONOFF,	0,	-1,	1 , "Auto Merge Gem"};
ToggleVar tEnableAutoSimpleItemStack={TOGGLEVAR_ONOFF,	0,	-1,	1 , "Auto Simple Item Stack"};
static int autoIdentifyCount=10;
static int identifyingId=0;

static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_INT,"AutoIdentifyCount",&autoIdentifyCount,4},
  {CONFIG_VAR_TYPE_KEY,"RightClickSwapToggle",&tRightClickSwap},
  {CONFIG_VAR_TYPE_KEY,"QuickDropToggle",&tQuickDropToggle},
  {CONFIG_VAR_TYPE_KEY,"EnableGemAutoMerge",&tEnableGemAutoMerge},
  {CONFIG_VAR_TYPE_KEY,"EnableAutoSimpleItemStack",&tEnableAutoSimpleItemStack},
};
void quickswap_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
struct InventoryType {
	BYTE invType;
	short left;
	short bottom;
	BYTE nGridXs;
	BYTE nGridYs;
	BYTE nGridWidth;
	BYTE nGridHeight;
};
static int startProcessMs=0;
static int cubeIdx,cubeN;
static char cubeMap[256],invMap[256],stashMap[256];
static UnitAny *cube=NULL,*cubeItem,*pGem[30],*pSkeleton[5],*rune7;
static UnitAny *unidentifiedItem,*idBook;
static UnitAny *pGemStack[30],*pSkeletonStack[5],*pRuneStack[34];
static int nGem[30],nSkeleton[5],nrune7;
static int invUsed,invLeft;
InventoryBIN *getInvTxt() {
	InventoryBIN* pInvs=*d2common_pInventoryTxt;if (!pInvs) return 0;
	InventoryBIN *pInv;
	if (dwPlayerClass<5) pInv=pInvs+16+dwPlayerClass;
	else if (dwPlayerClass==5) pInv=pInvs+16+14; //dru
	else if (dwPlayerClass==6) pInv=pInvs+16+15; //asn
	else pInv=pInvs+16;
	return pInv;
}
static void updateCubeInvMap() {
	invUsed=0;cube=NULL;cubeItem=NULL;cubeIdx=0;cubeN=0;
	memset(cubeMap,0,sizeof(cubeMap));
	memset(invMap,0,sizeof(invMap));
	memset(stashMap,0,sizeof(invMap));
	memset(pGem,0,sizeof(pGem));
	memset(pSkeleton,0,sizeof(pSkeleton));
	rune7=NULL;unidentifiedItem=NULL;idBook=NULL;
	memset(nGem,0,sizeof(nGem));
	memset(nSkeleton,0,sizeof(nSkeleton));
	memset(pGemStack,0,sizeof(pGemStack));
	memset(pSkeletonStack,0,sizeof(pSkeletonStack));
	memset(pRuneStack,0,sizeof(pRuneStack));
	nrune7=0;

	for (UnitAny *pItem=d2common_GetFirstItemInInv(PLAYER->pInventory);pItem;pItem=d2common_GetNextItemInInv(pItem)) {
		if (pItem->dwUnitType!=UNITNO_ITEM) continue ;
		if (pItem->pItemData->nLocation!=1) continue; //cube/stash/inv
		int index = GetItemIndex(pItem->dwTxtFileNo)+1;
		if (index==2042) cube=pItem;
		ItemTxt *pItemTxt=d2common_GetItemTxt(pItem->dwTxtFileNo);
		int x=pItem->pItemPath->unitX,y=pItem->pItemPath->unitY;
		int w=pItemTxt->nInvwidth,h=pItemTxt->nInvheight;
		char *map=NULL;
		switch (pItem->pItemData->nItemLocation) {
			case 0://inv
				invUsed+=w*h;
				if (!d2common_CheckItemFlag(pItem,ITEMFLAG_IDENTIFIED,0,"?")) {
					if (identifyingId!=pItem->dwUnitId)
						unidentifiedItem=pItem;
				}
				if (index==2012) {
					int count=d2common_GetUnitStat(pItem,STAT_AMMOQUANTITY,0);
					if (count>autoIdentifyCount) idBook=pItem;
				} else if (2050<=index&&index<=2079) {
					pGem[index-2050]=pItem;
					nGem[index-2050]++;
				} else if (2090<=index&&index<=2094) {
					pSkeleton[index-2090]=pItem;
					nSkeleton[index-2090]++;
				} else if (index==2109) {
					rune7=pItem;
					nrune7++;
				}
				map=invMap;
				break;
			case 3: //cube
				cubeItem=pItem;
				if (cubeIdx!=-1) {
					if (cubeN==0) cubeIdx=index;
					else if (cubeIdx!=index) {
						cubeIdx=-1;
					}
					cubeN++;
				}
				map=cubeMap;
				break;
			case 4: //stash
				map=stashMap;
		}
		if (map) {
			for (int i=0;i<h;i++) {
				if (w==1) map[((y+i)<<4)+x]=1;
				else memset(&map[((y+i)<<4)+x],1,w);
			}
		}
		if (index==simpleItemStackIdx) {
			switch (pItem->pItemData->nItemLocation) {
				case 0:if (!*d2client_pUiInventoryOn&&!*d2client_pUiCubeOn&&!*d2client_pUiStashOn) continue;break;
				case 3:if (!*d2client_pUiCubeOn) continue;break;
				case 4:if (!*d2client_pUiStashOn) continue;break;
			}
			int txt=0;int count=getSimpleItemStackContent(pItem,&txt);
			if (count) {
				int index=GetItemIndex(txt)+1;
				if (2103<=index&&index<=2135) {//runes
					pRuneStack[index-2103]=pItem;
				} else if (2050<=index&&index<=2079) {//gems
					pGemStack[index-2050]=pItem;
				} else if (2090<=index&&index<=2094) { //skeleton
					pSkeletonStack[index-2090]=pItem;
				}
			}
		}
	}
	InventoryBIN *pInv=getInvTxt();
	invLeft=pInv->inv.w*pInv->inv.h-invUsed;
}
int putIntoCubeIndirect() {
	if (!cube) return 0;
	UnitAny *pCursorItem = PLAYER->pInventory->pCursorItem;if (!pCursorItem) return 0;
	switch (cube->pItemData->nItemLocation) {
		case 0: //inv
			if (!*d2client_pUiInventoryOn&&!*d2client_pUiCubeOn&&!*d2client_pUiStashOn) return 0;
			break;
		case 4: //stash
			if (!*d2client_pUiStashOn) return 0;
			break;
	}
	BYTE packet[9];packet[0]=0x2a;
	*(int *)&packet[1]=pCursorItem->dwUnitId;
	*(int *)&packet[5]=cube->dwUnitId;
	SendPacket(packet,sizeof(packet));
	startProcessMs=dwCurMs+300;
	return 1;
}
static int canPutIn(char *map,int x,int y,int w,int h) {
	for (int i=0;i<h;i++) {
		char *s=map+((y+i)<<4)+x;
		for (int j=0;j<w;j++,s++) {
			if (*s) return 0;
		}
	}
	return 1;
}
int putItem(InventoryBIN *pInv,char *map,int nItemLocation) {
	UnitAny *pCursorItem = PLAYER->pInventory->pCursorItem;if (!pCursorItem) return 0;
	ItemTxt *pItemTxt=d2common_GetItemTxt(pCursorItem->dwTxtFileNo);
	int iw=pItemTxt->nInvwidth,ih=pItemTxt->nInvheight;
	int w=pInv->inv.w,h=pInv->inv.h;
	int tx=-1,ty=-1;
	for (int y=0;y<h;y++) {
		for (int x=0;x<w;x++) {
			if (iw==1&&ih==1) {
				if (!map[(y<<4)+x]) {tx=x;ty=y;goto find;}
			} else {
				if (canPutIn(map,x,y,iw,ih)) {tx=x;ty=y;goto find;}
			}
		}
	}
	if (tx<0) return 0;
find:
	BYTE packet[17];packet[0]=0x18;
	*(int *)&packet[1]=pCursorItem->dwUnitId;
	*(int *)&packet[5]=tx;
	*(int *)&packet[9]=ty;
	*(int *)&packet[13]=nItemLocation;
	SendPacket(packet,sizeof(packet));
	startProcessMs=dwCurMs+300;
	return 1;
}
int putIntoInv() {
	return putItem(getInvTxt(),invMap,0);
}
int putIntoCube() {
	UnitAny *pCursorItem = PLAYER->pInventory->pCursorItem;if (!pCursorItem) return 0;
	InventoryBIN* pInvs=*d2common_pInventoryTxt;if (!pInvs) return 0;
	InventoryBIN *pInv=pInvs+16+9;
	return putItem(pInv,cubeMap,3);
}
int putIntoStash() {
	InventoryBIN* pInvs=*d2common_pInventoryTxt;if (!pInvs) return 0;
	InventoryBIN *pInv=pInvs+16+(EXPANSION?12:8);
	return putItem(pInv,stashMap,4);
}
static int pickupItem(UnitAny *pItem) {
	if (!pItem) return 0;
	UnitAny *pCursorItem = PLAYER->pInventory->pCursorItem;if (pCursorItem) return 0;
	int index=GetItemIndex(pItem->dwTxtFileNo)+1;
	//LOG("pickup %d\n",index);
	BYTE packet[5];packet[0]=0x19;
	*(int *)&packet[1]=pItem->dwUnitId;
	SendPacket(packet,sizeof(packet));
	startProcessMs=dwCurMs+300;
	return 1;
}
static int dropItemToGound() {
	UnitAny *pCursorItem = PLAYER->pInventory->pCursorItem;if (!pCursorItem) return 0;
	BYTE packet[5];packet[0]=0x17;
	*(int *)&packet[1]=pCursorItem->dwUnitId;
	SendPacket(packet,sizeof(packet));
	startProcessMs=dwCurMs+300;
	return 1;
}

extern int dwQuickSwapItemMs;
static InventoryType invTypes[] = {   //left和bottom已经偏移了格子一半大小
	{0, 430, 325, 10, 4, 29, 29}, // Inventory
	{4, 165, 152, 6, 8, 29, 29}, // Stash
	{3, 220, 220, 3, 4, 29, 29}, // Cube
	{4, 165, 345, 6, 4, 29, 29}, // Stash_Classic
	{2, 110, 325, 10, 4, 29, 29}, // Trade 1 for others trade
};
static UnitAny *getCube() {
	UnitAny *pUnit = d2common_GetFirstItemInInv(PLAYER->pInventory);
	while (pUnit) {
		if (pUnit->dwUnitType==UNITNO_ITEM) {
			int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
			if (index==2042) return pUnit;
		}
		pUnit = d2common_GetNextItemInInv(pUnit);
	}
	return pUnit;
}
static UnitAny *findSimpleItemStack(int txtNo) {
	for (UnitAny *pUnit = d2common_GetFirstItemInInv(PLAYER->pInventory);pUnit;pUnit = d2common_GetNextItemInInv(pUnit)) {
		if (pUnit->dwUnitType!=UNITNO_ITEM) continue;
		int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
		if (index!=simpleItemStackIdx) continue;
		if (!pUnit->pStatList) continue;
		switch (pUnit->pItemData->nItemLocation) {
			case 0:if (!*d2client_pUiInventoryOn&&!*d2client_pUiCubeOn&&!*d2client_pUiStashOn) continue;break;
			case 3:if (!*d2client_pUiCubeOn) continue;break;
			case 4:if (!*d2client_pUiStashOn) continue;break;
		}
		int txt=0;int count=getSimpleItemStackContent(pUnit,&txt);
		if (count&&txt==txtNo) return pUnit;
	}
	return NULL;
}
extern int dwInvType; //0:4*10 1:8*10
static void updateInv(InventoryType *inv, int id) {
	InventoryBIN* pInvs=*d2common_pInventoryTxt;if (!pInvs) return;
	id+=16;
	InventoryBIN *pInv=pInvs+id;
	inv->left=pInv->grid.x0+15;
	inv->bottom=pInv->grid.y0+15;
	if (id==16&&dwInvType) inv->bottom-=58;
	inv->nGridXs=pInv->inv.w;
	inv->nGridYs=pInv->inv.h;
	inv->nGridWidth=pInv->grid.w;
	inv->nGridHeight=pInv->grid.h;
	//LOG("update inv %d (%d,%d) %d*%d %d*%d\n",id,inv->left,inv->bottom,inv->nGridXs,inv->nGridYs,inv->nGridWidth,inv->nGridHeight);
}
static int putIntoSimpleItemStack() {
	UnitAny *pItem = PLAYER->pInventory->pCursorItem;if (!pItem) return 0;
	UnitAny *rc=findSimpleItemStack(pItem->dwTxtFileNo);if (!rc) return 0;
	int x=rc->pItemPath->unitX;
	int y=rc->pItemPath->unitY;
	BYTE packet[17];packet[0]=0x1f;
	*(int *)&packet[1]=pItem->dwUnitId;
	*(int *)&packet[5]=rc->dwUnitId;
	*(int *)&packet[9]=x;
	*(int *)&packet[13]=y;
	SendPacket(packet,sizeof(packet));
	startProcessMs=dwCurMs+300;
	return 1;
}
void dumpInvBin();
void MoveItem(D2MSG *pMsg) {
	if (!*d2client_pUiInventoryOn&&!*d2client_pUiCubeOn&&!*d2client_pUiStashOn&&!*d2client_pUiPPLTradeOn) return;
	int dx=(SCREENSIZE.x-800)/2;
	int dy=(SCREENSIZE.y-600)/2;
	UnitAny *pItem = PLAYER->pInventory->pCursorItem;if (!pItem) return;
	updateCubeInvMap();
	if (GetKeyState(VK_MENU)&0x8000) {
		int index = GetItemIndex(pItem->dwTxtFileNo)+1;//all config arrays are based 1
		if (2006<=index&&index<=2010) ; //potions
		else if (2080<=index&&index<=2089) ; //healing and mana potion
		else if (putIntoCubeIndirect()) return;
	}
	if (GetKeyState(VK_CONTROL)&0x8000) {
		int index = GetItemIndex(pItem->dwTxtFileNo)+1;//all config arrays are based 1
		if (2103<=index&&index<=2135 //runes
			||2050<=index&&index<=2079 //gems
			||2090<=index&&index<=2094) { //skeleton
			putIntoSimpleItemStack();return;
		}
	}
	if (pMsg->xpos<SCREENSIZE.x/2) {putIntoInv();return;}
	InventoryBIN* pInvs=*d2common_pInventoryTxt;if (!pInvs) return;
	InventoryBIN *pInv=NULL;
	int targetLocation=0;
	if (*d2client_pUiStashOn) {
		pInv=pInvs+16+(EXPANSION?12:8);
		putIntoStash();return;
	} else if (*d2client_pUiCubeOn) {
		putIntoCubeIndirect();return;
	} else if (*d2client_pUiPPLTradeOn) {
		//dumpInvBin();
		//pInv=pInvs+16+9;
	} else if (tQuickDropToggle.isOn) {
		if (d2client_CheckUiStatus(UIVAR_INVENTORY) && 
			!d2client_CheckUiStatus(UIVAR_STATS) && 
			!d2client_CheckUiStatus(UIVAR_QUEST) && 
			!d2client_CheckUiStatus(UIVAR_PARTY) &&
			!d2client_CheckUiStatus(UIVAR_PET) &&
			!d2client_CheckUiStatus(UIVAR_QUESTSCROLL)) {
			dropItemToGound();
			return;
		}
	}
	int invtype = 0;
	if (d2client_CheckUiStatus(UIVAR_STASH)) {
		if ( EXPANSION ) invtype = 1; 
		else invtype = 3; //非资箱子
		updateInv(&invTypes[invtype],EXPANSION?12:8);
	} else if ( d2client_CheckUiStatus(UIVAR_CUBE) ){
		invtype = 2;
		updateInv(&invTypes[invtype],9);
	} else if ( d2client_CheckUiStatus(UIVAR_PPLTRADE) ) {
		invtype = 4; //交易窗体
	}
	if ( invtype>0 ) {//左边开
		InventoryType *pInvType = (pMsg->xpos < SCREENSIZE.x/2) ? &invTypes[0] : &invTypes[invtype];
		if (pMsg->xpos < SCREENSIZE.x/2) updateInv(&invTypes[0],0);
		if (pInvType) {
			int bottom=pInvType->bottom;
			int left=pInvType->left;
			int nx=pInvType->nGridXs;
			int ny=pInvType->nGridYs;
			SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(pInvType->left + dx, pInvType->bottom + dy));
			int gridX=*d2client_pCursorInvGridX;
			int gridY=*d2client_pCursorInvGridY;
			if (gridX<0&&gridY<0) {LOG("swap error\n");return;}
			if (gridX||gridY) LOG("box %d (%d,%d) size %d*%d\n",invtype,gridX,gridY,nx,ny);
			DWORD dwBoxType = d2common_GetBoxType(PLAYER, pInvType->invType, EXPANSION);
			for (int x = 0; x < nx; ++x) {
				for (int y = 0; y < ny; ++y) {
					int xpos = left + x*pInvType->nGridWidth+dx;
					int ypos = bottom + y*pInvType->nGridHeight+dy;
					int gridcount = 0;
					int itemcount = 0;
					SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(xpos, ypos));
					d2common_CheckInvLocation(PLAYER->pInventory, pItem, *d2client_pCursorInvGridX, *d2client_pCursorInvGridY, dwBoxType, &gridcount, &itemcount, pInvType->invType);
					if ( itemcount==0 ) {
						//LOG("MoveItem %x %x %d %d\n",pMsg->hwnd,pMsg->wParam,xpos,ypos);
						SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, pMsg->wParam, MAKELONG(xpos, ypos));
						SendMessage(pMsg->hwnd, WM_LBUTTONUP, pMsg->wParam, MAKELONG(xpos, ypos));
						x = y = 1000; // break;
					}
				}
			}
			SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
		}
	}else if (tQuickDropToggle.isOn){
		if ( d2client_CheckUiStatus(UIVAR_INVENTORY) && 
			!d2client_CheckUiStatus(UIVAR_STATS) && 
			!d2client_CheckUiStatus(UIVAR_QUEST) && 
			!d2client_CheckUiStatus(UIVAR_PARTY) &&
			!d2client_CheckUiStatus(UIVAR_PET) &&
			!d2client_CheckUiStatus(UIVAR_QUESTSCROLL) ){
			int xpos = 230;
			int ypos = 200;
			SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(xpos+dx, ypos+dy));
			SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, pMsg->wParam, MAKELONG(xpos+dx, ypos+dy));
			SendMessage(pMsg->hwnd, WM_LBUTTONUP, pMsg->wParam, MAKELONG(xpos+dx, ypos+dy));
			SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
		}
	}
}

void __stdcall RButtonUpHandlerPatch(D2MSG *pMsg) {
	if( fInGame==FALSE )return;
	if (tRightClickSwap.isOn && pMsg->dwMessage == WM_RBUTTONUP) {
		MoveItem( pMsg );
	}
}


void __declspec(naked) RButtonUpHandlerPatch_ASM() {
	__asm {
		push [esp+8];
		call RButtonUpHandlerPatch;
//oldcode:
		mov eax, d2client_pInGame;
		mov eax, [eax];
		ret;
	}
}

void __declspec(naked) RButtonInTradePatch_ASM() {
	__asm {
		push [esp+4];
		call RButtonUpHandlerPatch
		jmp d2client_Storm511
	}
}
void __declspec(naked) CtrlClickHandlerPatch_ASM() {
	__asm {
		je noNpc 
		ret//如果NPC，跳过处理
noNpc:
		//cmp [tCtrlClickSwap.isOn] , 0
		//jmp orgjz
		push ebx
		mov eax, dword ptr [dwCurMs]
		mov ebx, dword ptr [dwQuickSwapItemDelayMs]
		add eax, ebx
		mov dword ptr [dwQuickSwapItemMs], eax
		pop ebx
		add dword ptr [esp] ,0x12D //强制拿起物品
		xor eax ,eax
		ret
//orgjz:	
		//add dword ptr [esp] ,0x141
		//ret
	}
}

static UnitAny *findGemInv(int idx,int count) {
	UnitAny *pItem=NULL;
	if (2050<=idx&&idx<=2079) {
		pItem=pGem[idx-2050];
		if (nGem[idx-2050]<count) pItem=NULL;
		if ((idx%5)==4) pItem=NULL;
	} else if (2090<=idx&&idx<=2093) {
		pItem=pSkeleton[idx-2090];
		if (nSkeleton[idx-2090]<count) pItem=NULL;
	} else if (idx==2109) {
		pItem=rune7;
	}
	return pItem;
}
void clickCubeTransformButton() {
	BYTE packet[7];packet[0]=0x4f;
	*(int *)&packet[1]=0x18;
	*(short *)&packet[5]=0;
	SendPacket(packet,sizeof(packet));
	startProcessMs=dwCurMs+300;
}
extern int dwSkillChangeCount;
int autoCubeTransform() {
	static int dwSkillChangeCount1=0; 
	if (!tEnableGemAutoMerge.isOn) return 0;
	if (startProcessMs&&dwCurMs<startProcessMs) {
		if (dwSkillChangeCount1==dwSkillChangeCount) return 0;
	}
	if (!*d2client_pUiCubeOn) return 0;
	updateCubeInvMap();
	dwSkillChangeCount1=dwSkillChangeCount;
	if (cubeIdx==-1) {LOG("cube diff\n");goto err;} //Has different item in cube
	UnitAny *pCursorItem = PLAYER->pInventory->pCursorItem;
	if (cubeN>3) {LOG("cube %d\n",cubeN);goto err;}
	if (cubeN==3) {
		if (pCursorItem) {LOG("cube 3+cursor\n");goto err;}
		if (2050<=cubeIdx&&cubeIdx<=2079) {
			if ((cubeIdx%5)==4) {LOG("cube perf\n");goto err;}
		} else if (2090<=cubeIdx&&cubeIdx<=2093) {
		} else if (cubeIdx==2109) {
		} else {LOG("cube idx %d\n",cubeIdx);goto err;}
		clickCubeTransformButton();
		return 1;
	} else if (cubeN>0) {
		//cube has 1 or 2 gem
		if (pCursorItem) {
			int index=GetItemIndex(pCursorItem->dwTxtFileNo)+1;
			if (index!=cubeIdx) goto err;
			return putIntoCubeIndirect();//putIntoCube();
		}
		UnitAny *pItem=findGemInv(cubeIdx,3-cubeN);
		if (pItem) return pickupItem(pItem);
		if (cubeN==1&&cubeItem) return pickupItem(cubeItem);
		goto err;
	}
	//cube empty
	if (pCursorItem) {
		int index=GetItemIndex(pCursorItem->dwTxtFileNo)+1;
		UnitAny *pItem=findGemInv(index,2);
		if (pItem) return putIntoCubeIndirect();//putIntoCube();
		return putIntoInv();
	}
	UnitAny *pItem=NULL; 
	for (int i=0;i<30;i++) {
		if ((i%5)==4) continue;
		if (nGem[i]>=3) {pItem=pGem[i];break;}
	}
	if (!pItem) {
		for (int i=0;i<4;i++) {
			if (nSkeleton[i]>=3) {pItem=pSkeleton[i];break;}
		}
	}
	if (!pItem) {
		if (nrune7>=3) pItem=rune7;
	}
	if (pItem) return pickupItem(pItem);
err:
	startProcessMs=dwCurMs+300;
	return 0;
}
static int useStackItemCount=1,useStackItemCount1=0,useStackItemArg=-1,autoIdMs=0;
void quickswap_NewGame() {
	useStackItemCount=1;useStackItemCount1=0;useStackItemArg=-1;autoIdMs=0;
}
/*
	d2client_AF910: 83 EC 08           sub esp, 8
	d2client_AF913: 8B 01              mov eax, [ecx]
	d2client_AF915: 8B 49 04           mov ecx, [ecx+0x4]
	d2client_AF918: 8D 14 24           lea edx, [esp]
	d2client_AF91B: 52                 push edx
	d2client_AF91C: 89 44 24 04        mov [esp+0x4], eax
	d2client_AF920: 89 4C 24 08        mov [esp+0x8], ecx
  d2client_AF924: E8 17 71 FD FF     call d2client_86A40 void __stdcall d2client_useStackableItem(char *packet)(1 args)
*/
void __declspec(naked) RecvPacket3FPatch_ASM() {
	__asm {
		inc useStackItemCount
		mov useStackItemArg,eax
		jmp d2client_useStackableItem
	}
}
void activeItemStack(int id) {
	int x=PLAYER->pMonPath->wUnitX;
	int y=PLAYER->pMonPath->wUnitY;
	BYTE packet[13];
	packet[0]=0x20;*(DWORD*)&packet[1]=idBook->dwUnitId;
	*(DWORD*)&packet[5]=x;*(DWORD*)&packet[9]=y;
	SendPacket(packet,13);
}
int autoIdentify() {
	if (!*d2client_pUiInventoryOn) return 0;
	if (autoIdMs&&dwCurMs<autoIdMs&&useStackItemCount1==useStackItemCount) return 1;
	updateCubeInvMap();
	if (!unidentifiedItem||!idBook) return 0;
	if (((useStackItemArg>>8)&0xff)==0) { //0:ready to use, 0xFF:used
		identifyingId=unidentifiedItem->dwUnitId;
		useStackItemCount1=useStackItemCount;
		BYTE packet[9];packet[0]=0x27;
		*(DWORD*)&packet[1]=unidentifiedItem->dwUnitId;
		*(DWORD*)&packet[5]=idBook->dwUnitId;
		SendPacket(packet,9);
		autoIdMs=dwCurMs+300;
		return 1;
	}
	identifyingId=0;
	useStackItemCount1=useStackItemCount;
	activeItemStack(idBook->dwUnitId);
	autoIdMs=dwCurMs+300;
	return 1;
}
int autoSimpleItemStack() {
	static int dwSkillChangeCount1=0; 
	if (!tEnableAutoSimpleItemStack.isOn) return 0;
	if (startProcessMs&&dwCurMs<startProcessMs) {
		if (dwSkillChangeCount1==dwSkillChangeCount) return 0;
	}
	if (!*d2client_pUiStashOn) return 0;
	dwSkillChangeCount1=dwSkillChangeCount;
	UnitAny *pCursorItem=PLAYER->pInventory->pCursorItem;
	if (pCursorItem) {
		putIntoSimpleItemStack();
		startProcessMs=dwCurMs+300;
		return 0;
	}
	updateCubeInvMap();
	for (UnitAny *pItem=d2common_GetFirstItemInInv(PLAYER->pInventory);pItem;pItem=d2common_GetNextItemInInv(pItem)) {
		if (pItem->dwUnitType!=UNITNO_ITEM) continue ;
		if (pItem->pItemData->nLocation!=1) continue; //cube/stash/inv
		if (pItem->pItemData->nItemLocation!=0) continue; //inv
		int index=GetItemIndex(pItem->dwTxtFileNo)+1;
		if (2103<=index&&index<=2135) {//runes
			if (pRuneStack[index-2103]) {pickupItem(pItem);break;}
		} else if (2050<=index&&index<=2079) {//gems
			if (pGemStack[index-2050]) {pickupItem(pItem);break;}
		} else if (2090<=index&&index<=2094) { //skeleton
			if (pSkeletonStack[index-2090]) {pickupItem(pItem);break;}
		}
	}
	startProcessMs=dwCurMs+300;
	return 0;
}
