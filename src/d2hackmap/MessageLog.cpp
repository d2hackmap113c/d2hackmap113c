#include "stdafx.h"
#include "header.h"

extern ToggleVar tShowExp;
extern int   currentgameexp;
ToggleVar 	tLogMessage={		TOGGLEVAR_ONOFF,	0,	-1,	1,"LogMessage"};
int	dwMessageFileSize=1048576;
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY, "MessageLogToggle",    &tLogMessage},
  {CONFIG_VAR_TYPE_INT, "MessageLogFileSize",  &dwMessageFileSize,4},
};
void messagelog_AddConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
HANDLE OpenFileNew(char *filename) {
	return CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}
HANDLE OpenFileWrite(char *filename) {
	return CreateFile(filename, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}
DWORD WriteFile(HANDLE hFile, void *buf, DWORD len) {
	DWORD numdone = 0;
	WriteFile(hFile, buf, len, &numdone, NULL);
	return numdone;
}

void mainMenuMsg(wchar_t *s,int color);
static void __fastcall LogMessage(wchar_t* wstr) {
	char asc_buf[0x400];
	wchar_t buf[0x400];
	mainMenuMsg(wstr,0);
	if (!tLogMessage.isOn) return;
	wchar_t *p = buf;
	// strip the D2 color code
	for (wchar_t *pw = wstr;*pw; ++pw) {
		if (*pw == 0xFF) {pw+=2;continue;}
		else {*p++ = *pw;}
	}
	*p++ = '\r';
	*p++ = '\n';
	*p = '\0';
	if (wcslen(buf) == 2) {
		// blank message , I just enter a game, log the game name and password.
		GameStructInfo *gi = GAMEINFO;
		if(!gi)return;
		if(gi->szGameName[0]) {
			if (gi->szGamePassword[0])
				wsprintfW(buf,L"---%hs(*%hs) enter game[%hs] %hs/%hs---\r\n",gi->szCharName,gi->szAccountName,gi->szRealmName,gi->szGameName,gi->szGamePassword);
			else
				wsprintfW(buf,L"---%hs(*%hs) enter game[%hs] %hs---\r\n",gi->szCharName,gi->szAccountName,gi->szRealmName,gi->szGameName);
		}
	}
	else if ( tLogMessage.value32==2 ) return;
	// convert to UTF-8 string
//	WChar2MChar(buf,asc_buf,wcslen(buf));
	WideCharToMultiByte(CP_UTF8,NULL,buf,-1,asc_buf,0x400,NULL,NULL);

	SYSTEMTIME t;
	char timebuf[256];
	GetLocalTime(&t);
	sprintf(timebuf,"[%04X][%04d-%02d-%02d %02d:%02d:%02d]",GetCurrentProcessId(),t.wYear,t.wMonth,t.wDay,t.wHour, t.wMinute, t.wSecond );

	HANDLE hFile = OpenFileWrite("d2messages.log");
	if (hFile != INVALID_HANDLE_VALUE){
		if ((int)GetFileSize(hFile,NULL)/1024>dwMessageFileSize){
			CloseHandle(hFile);
			__try{
				char newname[256];
				sprintf(newname,"d2messages%04d%02d%02d.log",t.wYear,t.wMonth,t.wDay);
				rename("d2messages.log",newname);
				hFile = OpenFileNew("d2messages.log");
				if ( hFile == INVALID_HANDLE_VALUE ) return ;
			}__except(EXCEPTION_EXECUTE_HANDLER) {
				return;
			}
		}
		SetFilePointer(hFile, 0, 0, FILE_END);
		WriteFile(hFile,timebuf,strlen(timebuf));
		WriteFile(hFile,asc_buf,strlen(asc_buf));
		CloseHandle(hFile);
	}
 }

void __declspec(naked) MessageLogPatch1_ASM() {
	__asm {
//		cmp [tLogMessage.isOn],0
//		je oldcode
		mov     ecx, [esp+0x2C]
		pushad
		call LogMessage
		popad
//oldcode:
		mov	edx,[d2client_pScreenWidth]
		mov edx, [edx]
		ret
	}
}

void __declspec(naked) MessageLogPatch2_ASM() {
	__asm {
//		cmp [tLogMessage.isOn],0
//		je oldcode
		pushad
		call LogMessage
		popad
//oldcode:
		add eax, 0x2710
		ret
	}
}

void MessageLogExitGame() {
	if (tLogMessage.isOn==0) return;
	GameStructInfo *gi = GAMEINFO;
	if(!gi)return;
	char asc_buf[0x400];
	char asc_buf1[0x400];
	// I just exit a game, log it.
	sprintf(asc_buf,"---%s(*%s) exit game---\r\n",gi->szCharName,gi->szAccountName);
	SYSTEMTIME t;
	char timebuf[256];
	GetLocalTime(&t);
	HANDLE hFile = OpenFileWrite("d2messages.log");
	if ( hFile == INVALID_HANDLE_VALUE ) return ;
	sprintf(timebuf,"[%04X][%04d-%02d-%02d %02d:%02d:%02d]",GetCurrentProcessId(),t.wYear,t.wMonth,t.wDay,t.wHour, t.wMinute, t.wSecond);

	SetFilePointer(hFile, 0, 0, FILE_END);

	if (tShowExp.isOn){
		int exp = d2common_GetUnitStat(PLAYER, STAT_EXP, 0);
		sprintf(asc_buf1,"---EXP gained[%d]---\r\n",exp - currentgameexp);

		WriteFile(hFile,timebuf,strlen(timebuf));
		WriteFile(hFile,asc_buf1,strlen(asc_buf1));
	}
	WriteFile(hFile,timebuf,strlen(timebuf));
	WriteFile(hFile,asc_buf,strlen(asc_buf));
	CloseHandle(hFile);
}

