/*
 * main.c: main routine of this program
 * 
 * 2001-08-20 faster
 *   add initialization routine and main loop of this program
 */

#include "stdafx.h"
#include "d2server.h"
#include "d2gs.h"
#include "eventlog.h"
#include "vars.h"
#include "config.h"
#include "d2ge.h"
#include "net.h"
#include "timer.h"
#include "telnetd.h"
#include "d2gamelist.h"
#include "handle_s2s.h"

//#define DEBUG_ON_CONSOLE

/* function declarations */
int  DoCleanup(void);
BOOL D2GSCheckRunning(void);
int  CleanupRoutineForServerMutex(void);
void D2GSShutdown(unsigned int exitCode);
extern BOOL D2GSCheckGameInfo(void);
extern BOOL D2GSSaveAllGames(DWORD dwMilliseconds);

/* CTRL+C or CTRL+Break signal handler */
BOOL WINAPI ControlHandler(DWORD dwCtrlType);


/* some variables used just in this file */
static HANDLE			hD2GSMutex  = NULL;
static HANDLE			hStopEvent  = NULL;
static HANDLE			hWatchDog	= NULL;
static CLEANUP_RT_ITEM	*pCleanupRT = NULL;
static CRITICAL_SECTION	csWatchDog;
static DWORD			dwShutdownStatus = 0;
static DWORD			dwShutdownTickCount = 0;
static DWORD			dwWatchDogCounter = 0;


int Install();
char server_root[256];
extern char mpq_path[256];
static int hasFile(char *path) {
	struct stat st;
	if (stat(path,&st)!=0) return 0;
	return st.st_size>0;
}
int fileCheck() {
	static char *dlls[]={"D2Client.dll","D2CMP.dll","D2Common.dll","D2Game.dll","D2gfx.dll","D2Lang.dll",
		"D2MCPClient.dll","D2Net.dll","D2sound.dll","D2Win.dll","Fog.dll","Storm.dll","ijl11.dll",0};
	static char *mpqs[]={"d2data.mpq","d2sfx.mpq","d2speech.mpq","patch_d2.mpq","d2exp.mpq",0};
	char path[256];
	for (int i=0;dlls[i];i++) {
		if (!hasFile(dlls[i])) {
			printf("Can't find %s\n",dlls[i]);
			return 0;
		}
	}
	for (int i=0;mpqs[i];i++) {
		_snprintf(path,256,"%s\\%s",mpq_path,mpqs[i]);
		if (!hasFile(mpqs[i])&&!hasFile(path)) {
			printf("Can't find %s or %s\n",mpqs[i],path);
			return 0;
		}
	}
	return 1;
}
void check_reload_d2hackmap();
DWORD WINAPI consoleThread(LPVOID lpParameter) {
	while (1) {
		char buf[256];gets(buf);
		printf("checking d2hackmap.dll");fflush(stdout);
		check_reload_d2hackmap();
		printf(" done\n");
	}
	printf("Console thread end\n");
}
void startConsoleThread() {
	DWORD		dwThreadId;
	CreateThread(NULL, 0, consoleThread, 0, 0, &dwThreadId);
}
void wait_input() {
	char buf[256];
	printf("Press ENTER to continue ...");fflush(stdout);
	gets(buf);
}
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
int main(int argn,char *argv[]) {
	char	path[256],*e;
	DWORD	dwWait;
	int pid=GetCurrentProcessId();
	//FILE *fp=fopen("/t/pid.txt","wb+");if (fp) {fwrite(&pid,4,1,fp);fclose(fp);}
	printf("pid: 0x%X\n",pid);
	GetModuleFileName(NULL, server_root, 256);
	e=strrchr(server_root,'\\');if (e) {*e=0;SetCurrentDirectoryA(server_root);}
	GetCurrentDirectoryA(256,path);printf("current directory: %s\n",path);
	//wait_input();
	startConsoleThread();
	/* reset cleanup routine list */
	pCleanupRT = NULL;

	InitializeCriticalSection(&gsShutDown);

	/* init log system first */
	if (!D2GSEventLogInitialize()) return -1;

	d2gsconf.enablegslog = TRUE;
	D2GSEventLog("main", "Starting GS Server...");

	/* setup signal capture */
	SetConsoleCtrlHandler(ControlHandler, TRUE);

	/* check if another instance is running */
	if (D2GSCheckRunning()) {
		D2GSEventLog("main", "Seems another server is running");
		DoCleanup();
		return -1;
	}

	/* create a name event, for "d2gssvc" server controler to terminate me */
	hStopEvent= CreateEvent(NULL, TRUE, FALSE, D2GS_STOP_EVENT_NAME);
	if (!hStopEvent) {
		D2GSEventLog("main", "failed create stop event object");
		DoCleanup();
		return -1;
	}

	/* init variables */
	if (!D2GSVarsInitialize()) {
		D2GSEventLog("main", "Failed initialize global variables");
		DoCleanup();
		return -1;
	}

	/* read configurations */
	if (!D2GSReadConfig()) {
		D2GSEventLog("main", "Failed getting configurations from registry");
		DoCleanup();
		return -1;
	}
	//loadConfig();
	if (!fileCheck()) {
		printf("ERROR: Missing required files\n");
		printf("Press Any Key to exit");_getch();
		return 0;
	}
	Install();

	/* create timer */
	if (!D2GSTimerInitialize()) {
		D2GSEventLog("main", "Failed Startup Timer");
		DoCleanup();
		return -1;
	}

	/* start up game engine */
	if (!D2GEStartup()) {
		D2GSEventLog("main", "Failed Startup Game Engine");
		D2GSEventLog("main", "Please enable GELog, and see the message");
		DoCleanup();
		return -1;
	}

	/* initialize the net connection */
	if (!D2GSNetInitialize()) {
		D2GSEventLog("main", "Failed Startup Net Connector");
		DoCleanup();
		return -1;
	}

	/* administration console */
	/*if (!D2GSAdminInitialize()) {
		D2GSEventLog("main", "Failed Startup Administration Console");
		DoCleanup();
		return -1;
	}*/

	/* create timer */
	if (!D2GSTimerInitialize())
	{
		D2GSEventLog("main", "Failed Startup Timer");
		DoCleanup();
		return -1;
	}

	/* main server loop */
	D2GSEventLog("main", "Entering Main Server Loop");
	while(TRUE) {
		dwWait = WaitForSingleObject(hStopEvent, 1000);
		if (dwWait!=WAIT_OBJECT_0) continue;
		/* service controler tell me to stop now. "Yes, sir!" */
		D2GSSetD2CSMaxGameNumber(0);
		D2GSActive(FALSE);
		d2gsconf.enablegslog = TRUE;
		D2GSEventLog("main", "I am going to stop");
		d2gsconf.enablegslog = TRUE;
		D2GSSaveAllGames(5000);
		d2gsconf.enablegslog = FALSE;
		Sleep(3000);
		D2GSShutdown(0);
		break;
	}

	/*DoCleanup();*/
	return 0;

} /* End of main() */



/*********************************************************************
 * Purpose: to add an cleanup routine item to the list
 * Return: TRUE(success) or FALSE(failed)
 *********************************************************************/
int CleanupRoutineInsert(CLEANUP_ROUTINE pRoutine, char *comment)
{
	CLEANUP_RT_ITEM		*pitem;

	if (pRoutine==NULL) return FALSE;
	pitem = (CLEANUP_RT_ITEM *)malloc(sizeof(CLEANUP_RT_ITEM));
	if (!pitem) {
		D2GSEventLog("CleanupRoutineInsert", "Can't alloc memory");
		return FALSE;
	}
	ZeroMemory(pitem, sizeof(CLEANUP_RT_ITEM));

	/* fill the structure */
	if (comment)
		strncpy(pitem->comment, comment, sizeof(pitem->comment)-1);
	else
		strncpy(pitem->comment, "unknown", sizeof(pitem->comment)-1);
	pitem->cleanup = pRoutine;
	pitem->next = pCleanupRT;
	pCleanupRT = pitem;

	return TRUE;

} /* End of CleanupRoutineInsert() */


/*********************************************************************
 * Purpose: call the cleanup routine to do real cleanup work
 * Return: TRUE or FALSE
 *********************************************************************/
int DoCleanup(void)
{
	CLEANUP_RT_ITEM		*pitem, *pprev;

	pitem = pCleanupRT;
	while(pitem)
	{
		D2GSEventLog("DoCleanup", "Calling cleanup routine '%s'", pitem->comment);
		pitem->cleanup();
		pprev = pitem;
		pitem = pitem->next;
		free(pprev);
	}
	pCleanupRT = NULL;

	/* at last, cleanup event log system */
	D2GSEventLog("DoCleanup", "Cleanup done.");
	D2GSEventLogCleanup();

	/* Close the mutex */
	if (hD2GSMutex)	CloseHandle(hD2GSMutex);
	if (hStopEvent) CloseHandle(hStopEvent);

#ifdef DEBUG_ON_CONSOLE
	printf("Press Any Key to Continue");
	_getch();
#endif

	return TRUE;

} /* End of DoCleanup() */


/*********************************************************************
 * Purpose: check if other instance is running
 * Return: TRUE(server is running) or FALSE(not running)
 *********************************************************************/
BOOL D2GSCheckRunning(void)
{
	HANDLE	hMutex;

	hD2GSMutex = NULL;
	hMutex = CreateMutex(NULL, TRUE, D2GSERVER_MUTEX_NAME);
	if (hMutex==NULL) {
		return TRUE;
	} else if (GetLastError()==ERROR_ALREADY_EXISTS) {
		CloseHandle(hMutex);
		return TRUE;
	} else {
		if (CleanupRoutineInsert(CleanupRoutineForServerMutex, "Server Mutex")) {
			hD2GSMutex = hMutex;
			return FALSE;
		} else {
			/* insert cleanup routine failed, assume server is running */
			CloseHandle(hMutex);
			return TRUE;
		}
	}

} /* End of D2GServerCheckRun() */


/*********************************************************************
 * Purpose: cleanup routine for release the global server mutex
 * Return: TRUE or FALSE
 *********************************************************************/
int CleanupRoutineForServerMutex(void)
{
	if (!hD2GSMutex) return FALSE;
	return CloseHandle(hD2GSMutex);

} /* End of CleanupRoutineServerMutex() */


/*********************************************************************
 * Purpose: catch CTRL+C or CTRL+Break signal
 * Return: TRUE or FALSE
 *********************************************************************/
BOOL WINAPI ControlHandler(DWORD dwCtrlType)
{
	switch( dwCtrlType )
	{
		case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
		case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
			D2GSEventLog("ControlHandler", "CTRL_BREAK or CTRL_C event caught");
			DoCleanup();
			ExitProcess(0);
			return TRUE;
			break;
    }
    return FALSE;

} /* End of ControlHandler */


/*********************************************************************
 * Purpose: to close the server mutex
 * Return: none
 *********************************************************************/
void CloseServerMutex(void)
{
	if (hD2GSMutex) CloseHandle(hD2GSMutex);
	hD2GSMutex = NULL;

} /* End of CloseServerMutex() */


void D2GSBeforeShutdown(DWORD status, DWORD dwSecondsRemaining)
{
	// GetTickCount milliseconds
	DWORD temp = 0;
	D2GSSetD2CSMaxGameNumber(0);
	D2GSActive(FALSE);
	EnterCriticalSection(&gsShutDown);
	dwShutdownStatus = status;
	dwShutdownTickCount = GetTickCount() + dwSecondsRemaining * 1000;
	switch (status)
	{
	case 1:
	case 3:
		D2GSEventLog("D2GSShutdown", "Restart GS in %lu seconds", dwSecondsRemaining);
		break;
	case 2:
	case 4:
		D2GSEventLog("D2GSShutdown", "Shutdown GS in %lu seconds", dwSecondsRemaining);
		break;
	}
	LeaveCriticalSection(&gsShutDown);
}

DWORD D2GSGetShutdownStatus(void)
{
	DWORD status = 0;
	EnterCriticalSection(&gsShutDown);
	status = dwShutdownStatus;
	LeaveCriticalSection(&gsShutDown);
	return status;
}

void D2GSShutdown(unsigned int exitCode)
{
	if (D2GSCheckGameInfo())
	{
		D2GSSaveAllGames(5000);
		Sleep(3000);
	}

	if (bGERunning != 0)
	{
		if (D2GSAfterEnd() != 0)
		{
			D2GSAfterEnd();
		}
	}

	ExitProcess(exitCode);
}


