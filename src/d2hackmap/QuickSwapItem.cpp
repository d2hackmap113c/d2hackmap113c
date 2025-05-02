#include "stdafx.h"
#include "header.h"
#include "d2BinFile.h"

extern int dwQuickSwapItemDelayMs;
ToggleVar 	tRightClickSwap={    TOGGLEVAR_ONOFF,	1,	-1,	1 ,	"Right Click Swap"};
ToggleVar 	tQuickDropToggle={	TOGGLEVAR_ONOFF,	0,	-1,	1 , "Quick Drop"};

static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY, "RightClickSwapToggle",  &tRightClickSwap   },
  {CONFIG_VAR_TYPE_KEY, "QuickDropToggle",       &tQuickDropToggle  },
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
int PutItemIntoCube(D2MSG *pMsg,int dx,int dy) {
	UnitAny *cube=getCube();
	int location= cube->pItemData->nItemLocation;
	InventoryType *pInvType=NULL;
	if (location==0) { //bag
		if (!d2client_CheckUiStatus(UIVAR_INVENTORY)&&!d2client_CheckUiStatus(UIVAR_STASH)) return 0;
		pInvType=&invTypes[0];
		updateInv(pInvType,0);
	} else if (location==4) { //stash
		if (!d2client_CheckUiStatus(UIVAR_STASH)) return 0;
		if (EXPANSION) pInvType=&invTypes[1];
		else pInvType=&invTypes[3];
		updateInv(pInvType,EXPANSION?12:8);
	}
	int left=pInvType->left;
	SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(pInvType->left + dx, pInvType->bottom + dy));
	if (*d2client_pCursorInvGridX == 2) //10*10 big box
		left-=2*pInvType->nGridWidth;
	int x=cube->pItemPath->unitX;
	int y=cube->pItemPath->unitY;
	int xpos = left + x*pInvType->nGridWidth+dx;
	int ypos = pInvType->bottom + y*pInvType->nGridHeight+dy;
	SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(xpos, ypos));
	SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, pMsg->wParam, MAKELONG(xpos, ypos));
	SendMessage(pMsg->hwnd, WM_LBUTTONUP, pMsg->wParam, MAKELONG(xpos, ypos));
	SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
	return 1;
}
void MoveItem(D2MSG *pMsg) {
	int dx=(SCREENSIZE.x-800)/2;
	int dy=(SCREENSIZE.y-600)/2;
	UnitAny *pItem = PLAYER->pInventory->pCursorItem;
	if (!pItem) return ;  //手上没东西
	if (GetKeyState(VK_MENU)&0x8000) {
		int index = GetItemIndex(pItem->dwTxtFileNo)+1;//all config arrays are based 1
		if (2006<=index&&index<=2010) ; //potions
		else if (2080<=index&&index<=2089) ; //healing and mana potion
		else if (PutItemIntoCube(pMsg,dx,dy)) return;
	}
	int invtype = 0;
	if ( d2client_CheckUiStatus(UIVAR_STASH) ) {
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
			//if (gridX) {nx+=gridX;left-=gridX*pInvType->nGridWidth;}
			//if (gridY) {ny+=gridY;bottom-=gridY*pInvType->nGridHeight;}
			/*switch (invtype) {
				case 1: //stash
					if (gridX==2) {nx=10;ny=10;}
					break;
				case 2: //cube
					if (gridX==4) {nx=10;ny=8;}
					break;
			}*/
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

//Ctrl+点击控制
//在联网模式，由于需要等待数据包返回才能拿起物品，所以在ButtonUp中不一定能处理好
//采用先在LButtonUp事件中处理，如果不能处理，激活游戏循环判定，直到处理完毕

static int nClickStatus = 0;    //是否Ctrl+点击事件
static BOOL fSwapProc = FALSE;  //是否已开始处理
static BOOL fSwapProcLoop = FALSE; //是否交给游戏循环处理
static D2MSG unDealMsg; //LButtonUp消息结构缓存
static DWORD dwSwapTimer;//计时器

/*
void __stdcall LButtonUpHandlerPatch(D2MSG *pMsg) {
	if ( fSwapProc ) return ;
	if ( nClickStatus==0 ) return ;
	fSwapProc = TRUE;
	fSwapProcLoop = FALSE;
	dwSwapTimer = dwCurMs;
	fSwapProcLoop = TRUE;
	unDealMsg = *pMsg;
	UnitAny *pItem = PLAYER->pInventory->pCursorItem;
	if ( pItem){
		Sleep(150);
		MoveItem( pMsg );
	}else{
		fSwapProcLoop = TRUE;
		unDealMsg = *pMsg;
		return;
	}
	fSwapProc = FALSE;
	nClickStatus = 0;
}
void __declspec(naked) LButtonUpHandlerPatch_ASM()
{
	//esp+4  = pMsg
	__asm {
		push ebp
		push ebx

		push [esp+0xC]
		call D2Storm511    //原始调用，先调用将LButtonUp消息处理掉

		push [esp+0xC]
		call LButtonUpHandlerPatch

		pop ebx
		pop ebp
		ret 4
	}
}
*/
