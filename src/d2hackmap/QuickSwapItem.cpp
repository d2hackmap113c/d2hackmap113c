#include "stdafx.h"

#ifdef MODULE_QUICKSWAPITEM

extern int dwQuickSwapItemMs;
static InventoryType invTypes[] = {   //left和bottom已经偏移了格子一半大小
	{0, 430, 325, 10, 4, 29, 29}, // Inventory
	{4, 165, 152, 6, 8, 29, 29}, // Stash
	{3, 220, 220, 3, 4, 29, 29}, // Cube
	{4, 165, 345, 6, 4, 29, 29}, // Stash_Classic
	{2, 110, 325, 10, 4, 29, 29}, // Trade 1 for others trade
};
static UnitAny *getCube() {
	UnitAny *pUnit = D2GetFirstItemInInv(PLAYER->pInventory);
	while (pUnit) {
		if (pUnit->dwUnitType==UNITNO_ITEM) {
			int index = GetItemIndex(pUnit->dwTxtFileNo)+1;
			if (index==2042) return pUnit;
		}
		pUnit = D2GetNextItemInInv(pUnit);
	}
	return pUnit;
}
int PutItemIntoCube(D2MSG *pMsg,int dx,int dy) {
	UnitAny *cube=getCube();
	int location= cube->pItemData->nItemLocation;
	InventoryType *pInvType=NULL;
	if (location==0) { //bag
		if (!D2CheckUiStatus(UIVAR_INVENTORY)&&!D2CheckUiStatus(UIVAR_STASH)) return 0;
		pInvType=&invTypes[0];
	} else if (location==4) { //stash
		if (!D2CheckUiStatus(UIVAR_STASH)) return 0;
		if (EXPANSION) pInvType=&invTypes[1];
		else pInvType=&invTypes[3];
	}
	int left=pInvType->left;
	SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(pInvType->left + dx, pInvType->bottom + dy));
	if (*p_D2CursorInvGridX == 2) //10*10 big box
		left-=2*pInvType->nGridWidth;
	int x=cube->pItemPath->dwPosX;
	int y=cube->pItemPath->dwPosY;
	int xpos = left + x*pInvType->nGridWidth+dx;
	int ypos = pInvType->bottom + y*pInvType->nGridHeight+dy;
	SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(xpos, ypos));
	SendMessage(pMsg->hwnd, WM_LBUTTONDOWN, pMsg->wParam, MAKELONG(xpos, ypos));
	SendMessage(pMsg->hwnd, WM_LBUTTONUP, pMsg->wParam, MAKELONG(xpos, ypos));
	SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, pMsg->lParam);
	return 1;
}
void  MoveItem(D2MSG *pMsg ){
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
	if ( D2CheckUiStatus(UIVAR_STASH) ) {
		if ( EXPANSION ) invtype = 1; 
		else invtype = 3; //非资箱子
	} else if ( D2CheckUiStatus(UIVAR_CUBE) ){
		invtype = 2;
	} else if ( D2CheckUiStatus(UIVAR_PPLTRADE) ) {
		invtype = 4; //交易窗体
	}
	if ( invtype>0 ) {//左边开
		InventoryType *pInvType = (pMsg->xpos < SCREENSIZE.x/2) ? &invTypes[0] : &invTypes[invtype];
		if (pInvType) {
			int left=pInvType->left;
			int nx=pInvType->nGridXs;
			int ny=pInvType->nGridYs;
			SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(pInvType->left + dx, pInvType->bottom + dy));
			if (*p_D2CursorInvGridX == 2) {//10*10 big box
				nx=10;ny=10;left-=2*pInvType->nGridWidth;
			}
			//LOG("%d*%d\n",nx,ny);
			DWORD dwBoxType = D2GetBoxType(PLAYER, pInvType->invType, EXPANSION);
			for (int x = 0; x < nx; ++x) {
				for (int y = 0; y < ny; ++y) {
					int xpos = left + x*pInvType->nGridWidth+dx;
					int ypos = pInvType->bottom + y*pInvType->nGridHeight+dy;
					int gridcount = 0;
					int itemcount = 0;
					SendMessage(pMsg->hwnd, WM_MOUSEMOVE, pMsg->wParam, MAKELONG(xpos, ypos));
					D2CheckInvLocation(PLAYER->pInventory, pItem, *p_D2CursorInvGridX, *p_D2CursorInvGridY, dwBoxType, &gridcount, &itemcount, pInvType->invType);
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
		if ( D2CheckUiStatus(UIVAR_INVENTORY) && 
			!D2CheckUiStatus(UIVAR_STATS) && 
			!D2CheckUiStatus(UIVAR_QUEST) && 
			!D2CheckUiStatus(UIVAR_PARTY) &&
			!D2CheckUiStatus(UIVAR_PET) &&
			!D2CheckUiStatus(UIVAR_QUESTSCROLL) ){
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
		mov eax, p_D2InGame;
		mov eax, [eax];
		ret;
	}
}

void __declspec(naked) RButtonInTradePatch_ASM() {
	__asm {
		push [esp+4];
		call RButtonUpHandlerPatch
		jmp D2Storm511
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


#endif
