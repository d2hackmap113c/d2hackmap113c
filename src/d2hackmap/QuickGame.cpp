#include "stdafx.h"

#ifdef MODULE_QUICKGAME

void SaveGameName() {
	if(GAMEINFO->szGameName[0]) {
		//wcscpy2(wszGameName, GAMEINFO->szGameName);
		int n=MultiByteToWideChar(CP_ACP, 0, GAMEINFO->szGameName, -1, wszGameName, 32);
		wcscpy2(wszGamePassword, GAMEINFO->szGamePassword);
		MultiByteToWideChar(CP_ACP, 0, GAMEINFO->szGameDescription, -1, wszGameDescription, 32);
	}
}
int QuickExitGame(){
	if (fCanExitGame==FALSE) return -1;
	fCanExitGame = FALSE;
	ExitGame();
	return 0;
}

int QuickNextGame(int addnum) {	
	if (fCanExitGame==FALSE) return -1;
	fCanExitGame = FALSE;
	SaveGameName();
	wchar_t *s = wszGameName;
	wchar_t *endptr;
	int len=wcslen(wszGameName);
	int end=0;
	for (int i=len-1;i>=0;i--) {
		wchar_t c=wszGameName[i];
		if ('0'<=c&&c<='9') {end=i+1;break;}
	}
	if (end) {
		wchar_t fmt[8],buf[32];
		int start=end-1;
		for (int i=start-1;i>=0;i--) {
			wchar_t c=wszGameName[i];
			if ('0'<=c&&c<='9') start=i;else break;
		}
		fmt[0]='%';fmt[1]='0';
		wsprintfW(fmt+2, L"%dd%%s",end-start);
		wcscpy(buf,wszGameName+end);
		long n = wcstol(wszGameName+start, &endptr, 10);
		wsprintfW(wszGameName+start, fmt, n+addnum,buf);
	} else {
		wszGameName[len]='1';
		wszGameName[len+1]=0;
	}
	ExitGame();
	return 0;
}


void __fastcall NextGameNamePatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,DWORD,char*)) {
	if (fAutoNextGameName) {
		D2SetEditBoxText(box, wszGameName);
		D2SelectEditBoxText(box);
	}
	// original code
	D2SetEditBoxProc(box, FunCallBack);
}
void __fastcall NextGamePasswordPatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,DWORD,char*)) {
	if (fAutoNextGamePassword) {
		D2SetEditBoxText(box, wszGamePassword);
		D2SelectEditBoxText(box);
	}
	// original code
	D2SetEditBoxProc(box, FunCallBack);
}
void __fastcall NextGameDecriptionPatch(D2EditBox* box, BOOL (__stdcall *FunCallBack)(D2EditBox*,DWORD,char*)) {
	if (fAutoNextGameDescription) {
		D2SetEditBoxText(box, wszGameDescription);
		D2SelectEditBoxText(box);
	}
	// original code
	D2SetEditBoxProc(box, FunCallBack);
}
void *__fastcall SelectGamePatch(D2EditBox* box, wchar_t* wcszTxt) {
	if (!fInGame&&box) {
		D2EditBox *next=box->pNext2;
		if (next&&next->pNext2==box&&box->dwPosY==148&&next->dwPosY==148
			&&box->dwPosX==432&&next->dwPosX==606) {
			D2SetEditBoxText(next, L"");
		}
	}
	if (wcszTxt[1]==':') wcszTxt+=2;
	// original code
	return D2SetEditBoxText(box, wcszTxt);
}


#endif
