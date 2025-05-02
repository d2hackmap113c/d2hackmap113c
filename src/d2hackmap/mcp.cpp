#include "stdafx.h"
#include "header.h"

// 4*eax+6FA2A900
static D2EditBox *pD2FilterBox = NULL ;

static int *gameIdxs=NULL,gameIdxCap=0;
static char szFilter[30]={0};
static int mGames = 0;
static int mGameShows = 0 ;
static int fQuickRefresh = 0;
static int stillLoading = 0;
void ResetGameList() {mGames=0;mGameShows=0;}
static char diffChars[16]=" nH ";
static ToggleVar tShowGameDifficulty={TOGGLEVAR_ONOFF,1,-1,1,"Show Game Difficulty"};
static ToggleVar tSortGames={TOGGLEVAR_ONOFF,1,-1,1,"Sort Game List"};
static ToggleVar tGameListSortByPlayers={TOGGLEVAR_ONOFF,1,-1,1,"Sort games by players"};
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_KEY,"GameListSortByPlayersToggle",&tGameListSortByPlayers},
	{CONFIG_VAR_TYPE_KEY,"GameListShowDifficultyToggle",&tShowGameDifficulty},
	{CONFIG_VAR_TYPE_KEY,"SortGamesToggle",&tSortGames},
  {CONFIG_VAR_TYPE_STR,"GameDifficultyChars",&diffChars,1,{8}},
};
void gameFilter_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void hex(FILE *fp,int addr,void *buf1,int n);
BOOL  __fastcall D2MCPPacketHandler_05_Patch(BYTE* aPacket,int len){
	if (!pD2FilterBox) return 1;
	char szTemp[30] ;
	MCPPacket05 *p=(MCPPacket05 *)aPacket;
	int dwIndex = p->idx;//*(DWORD*)&aPacket[3];
	BYTE  nPlayers = p->nPlayers;//aPacket[7];
	BYTE  difficulty = p->difficulty;//aPacket[9]; //0 16 32
	if (tShowGameDifficulty.isOn) {
		int d=(difficulty>>4)&3;
		if (tGameListSortByPlayers.isOn) p->nPlayers=(p->nPlayers<<2)|d;
		else p->nPlayers|=d<<6;
	} 
	stillLoading  = TRUE ;
	if (!dwIndex||nPlayers==0) {stillLoading = FALSE;return TRUE;}
	if (!gameIdxs||mGames>=gameIdxCap) {
		if (!gameIdxs) {
			gameIdxCap=128;
			gameIdxs=(int *)HeapAlloc(dllHeap,0,gameIdxCap*sizeof(int));
		} else {
			gameIdxCap*=2;
			gameIdxs=(int *)HeapReAlloc(dllHeap,0,gameIdxs,gameIdxCap*sizeof(int));
		}
	}
	for (int i=0;i<mGames;i++) {if (gameIdxs[i]==dwIndex) return TRUE;}
	if (0&&logfp) {
		LOG("--- %d %d %d diff=%d len=%d---\n",mGames,dwIndex,nPlayers,difficulty,len);
		hex(logfp,(int)aPacket,aPacket,len);fflush(logfp);
	}
	strncpy(szTemp,p->game,29);// (char*)&aPacket[12] ,  29);
	_strupr(szTemp);
	if (szFilter[0] && !strstr(szTemp ,szFilter)) {
		mGames++;
		return FALSE;
	} else {
		gameIdxs[mGames++] = dwIndex;
		mGameShows++;
		return  TRUE;
	}
	return  TRUE;
}
void __declspec(naked) D2MCPPacketHandler_05_Patch_ASM() {
	__asm{
		pop eax  //ret addr
		push ecx
		push esi
		push eax //push ret addr
		push ecx
		push edx
		call D2MCPPacketHandler_05_Patch
		test eax , eax
		pop edx
		pop ecx
		mov eax, dword ptr [ecx+0x8]
		jz noadd
		ret
noadd:	
		add dword ptr [esp] , 0x45
		ret
	}
}


BOOL __stdcall Filterbox_ReturnHandler(wchar_t* wText) {
	wchar_t *p = wText;
	int i = 0 ;
	while(*p){
		szFilter[i++] = (char)*(p++);
	}
	szFilter[i] = '\0';
	_strupr(szFilter);
	fQuickRefresh = 1;
	return TRUE;
}

BOOL __stdcall Filterbox_InputHandler(D2EditBox* pBox, int dwLength, char *pChar) {
	return TRUE;
}

void CreateFilterBox(){
	if ( !pD2FilterBox ){
		ResetGameList();
		pD2FilterBox  = d2win_CreateEditBox(599, 185, 145, 41, 7, 18, 
				d2client_LoadUiImage("DATA\\GLOBAL\\UI\\FrontEnd\\textbox2"), 
				(DWORD)Filterbox_ReturnHandler, 0, 0, 
				d2multi_pEditboxPreferences);

		if (pD2FilterBox) {
			d2win_SetEditBoxProc( pD2FilterBox , Filterbox_InputHandler);
			pD2FilterBox->dwMaxLength = 6;
			//(pD2FilterBox->fnCallBack)( pD2FilterBox );
		}
	}

}

void __declspec(naked) CreateGameBoxPatch_ASM() {
	__asm{
		push esi
		push eax
		call CreateFilterBox
		pop eax
		pop esi
		mov edx, 0x10
		ret
	}
}
void __stdcall DestroyGamelistPatch( D2EditBox* pListBox ){

	if( pListBox && pD2FilterBox ){
		ResetGameList();
		d2win_DestroyEditBox(pD2FilterBox);
		pD2FilterBox = NULL;
	}
	d2win_DestroyListBox( pListBox );

}

//d2multi_DF4E: 3D 98 3A 00 00     cmp eax, 0x3A98 (15000)
//d2multi_DF53: 72 14              jb d2multi_DF69 ->+20 BDF69
void __declspec(naked) GameListRefreshTimePatch_ASM() {
	__asm{
		cmp eax, 0x3A98	//15000ms
		jb mytest
		jmp refresh
mytest:
		cmp eax , 0x5DC //1500ms
		jb norefresh
		cmp [stillLoading] , 1	//still loading
		je norefresh
		cmp [fQuickRefresh] , 0
		je norefresh
refresh:
		mov [fQuickRefresh] , 0
		pushad
		call ResetGameList
		popad
		ret
norefresh:
		add dword ptr [esp] , 0x16
		ret		
	}

}

void DrawGameListPatch(){

	if ( (*d2multi_pGameListControl) && pD2FilterBox ){

		wchar_t wszTemp[200];
		DWORD dwOldFone = d2win_SetTextFont(1);
		if( szFilter[0] ){
			wsprintfW(wszTemp ,  L" %d / %d (*%hs*)" , mGameShows , mGames ,szFilter );
		}else{
			wsprintfW(wszTemp ,  L" %d / %d " , mGameShows , mGames );
		}
		d2win_DrawText(L"Filter", 545, 170, 4, -1);
		d2win_DrawText(wszTemp,	  424, 190, 4, -1);
		if ( stillLoading ){
			static int loading = 50;
			int max = loading/50 ;
			for( int i = 0 ; i< max ;i++ ){
				wszTemp[i] = L'.';
			}
			wszTemp[max] = L'\0';

			d2win_DrawText(wszTemp,	  424, 170, 4, -1);
			loading++;
			if( loading>600 ) loading = 50;

		}
		d2win_SetTextFont(dwOldFone);

	}

}
void __declspec(naked) DrawGameListPatch_ASM() {
	__asm{
		push esi
		call DrawGameListPatch
		pop esi
		mov esi, dword ptr [esi+0x3C]
		test esi, esi
		ret
	}
}
static int compareMcp05Game(const void *a,const void *b) {
	Mcp05Game *g1=(Mcp05Game *)a;
	Mcp05Game *g2=(Mcp05Game *)b;
	return g2->nPlayers-g1->nPlayers;
}
static void sortGameList() {
	int n=*d2multi_pnGames;if (!n) return;
	qsort(d2multi_pmcp05_games,n,sizeof(Mcp05Game),compareMcp05Game);
	/*for (int i=0;i<n;i++) {
		Mcp05Game *p=&d2multi_pmcp05_games[i];
		LOG("%d/%d %d %d %s \n",i,n,p->idx,p->nPlayers,p->game);
	}
	*/
}
/*
sort games here:
d2multi_11FE2: A1 CC 9D A0 6F     mov eax, [d2multi_39DCC int d2multi_nGames]
*/
void __declspec(naked) SortGameList_Patch_ASM() {
	__asm {
		cmp tSortGames.isOn,0
		jz original
		pushad
		call sortGameList
		popad
original:
		mov eax, dword ptr [d2multi_pnGames]
		mov eax, dword ptr [eax]
		ret
	}
}
static void __fastcall showGameListDifficulty(void *list,char *buf) {
	int n=strtol(buf,NULL,10);
	int d=0;
	if (tGameListSortByPlayers.isOn) {d=n&3;n>>=2;}
	else {d=(n>>6)&3;n&=0x3F;}
	*buf++=diffChars[d];
	sprintf(buf,"%d",n);
}
/*
show difficulty here:
d2multi_1206F: 8B 0D C0 9C A0 6F  mov ecx, [d2multi_39CC0 D2EditBox *d2multi_GameListControl]
d2multi_12075: 83 C4 0C           add esp, 0xC (12)
d2multi_12078: 6A 01              push 1
d2multi_1207A: 57                 push edi
d2multi_1207B: 8D 54 24 20        lea edx, [esp+0x20]
d2multi_1207F: E8 EA 5B FF FF     call d2multi_7C6E->d2win_8A50 d2win/Ordinal10186(2 args)
*/
void __declspec(naked) ShowGameListDifficulty_Patch_ASM() {
	__asm {
		cmp tShowGameDifficulty.isOn,0
		jz original
		pushad
		call showGameListDifficulty
		popad
original:
		jmp d2win_setLineEndText
		ret
	}
}
