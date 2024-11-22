#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <psapi.h>
#include <dbghelp.h>
#include <assert.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
HWND hwnd;
DWORD pid,threadIds[128],nThread=0;
HANDLE handle;
int exitFlag=0,removed=0;
FILE *logfp;
void removeAllBp();
void onDll();
void onThread(const LPDEBUG_EVENT DebugEv);
FARPROC DebugActiveProcessStopAddr;
int DebugActiveProcessStop1(DWORD pid) {
	DebugActiveProcessStopAddr(pid);	
}
void perr() {
	int dw=GetLastError();
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,dw,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),(LPTSTR) &lpMsgBuf,0, NULL );
	printf("err=%d %s\n",dw,lpMsgBuf);
}
BOOL SetPrivilege(HANDLE hToken,LPCTSTR lpszPrivilege,BOOL bEnablePrivilege) {
	TOKEN_PRIVILEGES tp;
	LUID luid;
	if ( !LookupPrivilegeValue(NULL,lpszPrivilege,&luid)) {
		printf("LookupPrivilegeValue error: %u\n", GetLastError() ); 
		return FALSE; 
	}
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege) tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else tp.Privileges[0].Attributes = 0;
	if ( !AdjustTokenPrivileges(hToken, FALSE, &tp, 
				sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) ) { 
		perr();
		printf("AdjustTokenPrivileges error: %u\n", GetLastError() ); 
		return FALSE; 
	} 
	return TRUE;
}
void sedebug() {
	HANDLE token;
	int r=OpenProcessToken(GetCurrentProcess(),PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_VM_OPERATION,&token);
	assert(r);
	SetPrivilege(token,SE_DEBUG_NAME,1);
}
void hex(int addr,u8 *buf,int n) {
	for (int i=0;i<n;i+=16) {
		printf("%06x: ",addr+i);
		for (int j=0;j<16;j++) {
			if (i+j>=n) printf("   ");
			else printf("%02x ",buf[i+j]);
			if (j==3||j==7||j==11) printf("- ");
		}
		printf("|");
		for (int j=0;j<16;j++) {
			if (i+j>=n) {printf(" ");continue;}
			u8 c=buf[i+j];
			if (c<' '||c>127) printf(" ");
			else printf("%c",c);
		}
		printf("\n");
	}
	printf("\n");
}
void rbuf(HANDLE handle,int addr,char *buf,int size) {
	SIZE_T rn;int r=ReadProcessMemory(handle,(LPCVOID)addr,buf,size,&rn);
	if (rn!=size) printf("read 0x%x failed %d/%d\n",addr,rn,size);
	//assert(r);assert(rn==size);
}
void wbuf(HANDLE handle,int addr,char *buf,int size) {
	SIZE_T rn;int r=WriteProcessMemory(handle,(LPVOID)addr,buf,size,&rn);
	//assert(r);assert(rn==size);
}
short r2(HANDLE handle,int addr) {short v;rbuf(handle,addr,(char *)&v,2);return v;}
int r4(HANDLE handle,int addr) {int v;rbuf(handle,addr,(char *)&v,4);return v;}
void w4(HANDLE handle,int addr,int v) {wbuf(handle,addr,(char *)&v,4);}
u8 r1(HANDLE handle,int addr) {u8 v;rbuf(handle,addr,(char *)&v,1);return v;}
void w1(HANDLE handle,int addr,u8 v) {wbuf(handle,addr,(char *)&v,1);}
void onProcess();void onBpt(int threadId);void onStep();
int endMs;
void EnterDebugLoop(const LPDEBUG_EVENT DebugEv) {
	DWORD dwContinueStatus = DBG_CONTINUE; // exception continuation 
	while (1) {
		if ((int)GetTickCount()>endMs) exitFlag=1;
		if (GetAsyncKeyState(VK_F12)&1) exitFlag=1;
		if (GetAsyncKeyState(VK_F11)&1) {
			if (logfp) fprintf(logfp,"---------\n");
			printf("-----------\n");
		}
		if (exitFlag&&!removed) removeAllBp();
		int r=WaitForDebugEvent(DebugEv, 200);//INFINITE); 
		if (!r) {if (removed) break;continue;}
		//printf("event %d %d\n",r,DebugEv->dwDebugEventCode);
		switch (DebugEv->dwDebugEventCode) { 
			case EXCEPTION_DEBUG_EVENT: 
				switch(DebugEv->u.Exception.ExceptionRecord.ExceptionCode) { 
					case EXCEPTION_ACCESS_VIOLATION:exitFlag++;printf("access violation\n"); break;
					case EXCEPTION_BREAKPOINT: onBpt(DebugEv->dwThreadId); break;
					case EXCEPTION_DATATYPE_MISALIGNMENT:exitFlag++;printf("datatype misalign\n"); break;
					case EXCEPTION_SINGLE_STEP: onStep();break;
					case DBG_CONTROL_C: exitFlag++;printf("ctrl+c\n");break;
					default: printf("unknown exception %d\n",DebugEv->dwDebugEventCode);break;
				} 
				break;
			case CREATE_THREAD_DEBUG_EVENT: onThread(DebugEv); break;
																			//SuspendThread and ResumeThread
			case CREATE_PROCESS_DEBUG_EVENT: onProcess(); break;
			case EXIT_THREAD_DEBUG_EVENT: break;
			case EXIT_PROCESS_DEBUG_EVENT: break;
			case LOAD_DLL_DEBUG_EVENT: onDll();break;
			case UNLOAD_DLL_DEBUG_EVENT: break;
			case OUTPUT_DEBUG_STRING_EVENT: break;
			case RIP_EVENT: break;
		} 
		ContinueDebugEvent(DebugEv->dwProcessId, DebugEv->dwThreadId, dwContinueStatus);
   }
}
BOOL CALLBACK EnumSymProc(PSYMBOL_INFO pSymInfo,ULONG SymbolSize,PVOID UserContext) {
	if (!pSymInfo) return TRUE;
    if (logfp) fprintf(logfp,"%x %d %s\n", (int)pSymInfo->Address, SymbolSize, pSymInfo->Name);
    return TRUE;
}
/*
void enum_dll() {
    HANDLE hProcess = GetCurrentProcess();
    DWORD64 BaseOfDll;
    char *Mask = "*";
		char *dllname="c:\\windows\\system32\\kernel32.dll";
    BOOL status;
    status = SymInitialize(hProcess, NULL, FALSE);assert(status);
    BaseOfDll = SymLoadModuleEx(hProcess,NULL,dllname,NULL,0,0,NULL,0);
    if (BaseOfDll == 0) {perr();printf("load %s failed\n",dllname);SymCleanup(hProcess);return;}
    if (SymEnumSymbols(hProcess,BaseOfDll,Mask,EnumSymProc,NULL)) {
    } else {perr();printf("SymEnumSymbols failed: %d\n", GetLastError());}
    SymCleanup(hProcess);
}
*/
//BOOL DebugActiveProcessStop(DWORD dwProcessId);
DEBUG_EVENT de;
char szProcessName[MAX_PATH];
void getProcessNameAndID( DWORD id ) {
	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,id);
	if (!hProcess ) {printf("open %x failed\n",id);return;}
	HMODULE hMod;DWORD cbNeeded;
	if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) ) {
		GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
	}
	CloseHandle( hProcess );
}
int find_process(char *name) {
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) ) return 1;
	cProcesses = cbNeeded / sizeof(DWORD);
	for ( i = 0; i < cProcesses; i++ ) {
		if( aProcesses[i] == 0 ) continue;
		int id=aProcesses[i];getProcessNameAndID( id );
		if (strcmp((const char *)szProcessName,name)==0) return id;
		//printf("%x %s\n", id,szProcessName);
	}
	return -1;
}
struct Bp {int addr;u8 code,removed;};
struct Bp bps[1024];int nbp=0;
struct Th {int id;int addr;} ths[8192];
void installBp(int addr) {
	struct Bp *p=&bps[nbp++];
	p->addr=addr;p->code=r1(handle,addr);p->removed=0;
	w1(handle,addr,0xcc);
	printf("install %x code=%02x\n",addr,p->code);
}
HANDLE openThread(int tid) {
	HANDLE ht=OpenThread(THREAD_GET_CONTEXT|THREAD_SET_CONTEXT,0,tid);
	if (!ht) {perr();printf("open thread %d failed\n",tid);return NULL;}
	return ht;
}
void removeAllHbps(CONTEXT *p) {p->Dr0=0;p->Dr1=0;p->Dr2=0;p->Dr3=0;p->Dr7=0;}
void removeAllBp() {
	for (int i=0;i<nbp;i++) {
		struct Bp *p=&bps[i];
		u8 c=r1(handle,p->addr);
		if (c==p->code) {if (!p->removed) printf("ERR:%x %02x not installed\n",p->addr,p->code);continue;}
		if (c!=0xCC) printf("ERR:%x is not CC %02x\n",p->addr,p->code);
		w1(handle,p->addr,p->code);p->removed=1;
		printf("remove %x %02x\n",p->addr,p->code);
	}
	if (0) {
		for (int i=0;i<nThread;i++) {
			CONTEXT context;
			HANDLE ht=openThread(threadIds[i]);if (!ht) continue;
			context.ContextFlags=CONTEXT_CONTROL|CONTEXT_INTEGER|CONTEXT_DEBUG_REGISTERS;// CONTEXT_ALL
			if (!GetThreadContext(ht,&context)) {exitFlag++;printf("get thread context error\n");goto end;}
			removeAllHbps(&context);
			if (!SetThreadContext(ht,&context)) {exitFlag++;printf("set thread context error\n");}
	end:
			CloseHandle(ht);
		}
	}
	removed=1;
}
void bpt(int threadId,int addr,CONTEXT *pc);
//mode 0:exec, 1:write, 2:undefine, 3:read
//len 0:1byte, 1:two byte, 2:undefined, 3:four bytes
void sethbp(CONTEXT *p,int id,int addr,int mode,int len,int en) {
	if (id<0||id>3||len<0||len>3||mode==2||len==2) {printf("hbp err\n");return;}
	if (mode==0) len=0;
	if (id==0) p->Dr0=addr;
	else if (id==1) p->Dr1=addr;
	else if (id==2) p->Dr2=addr;
	else if (id==3) p->Dr3=addr;
	if (en) en=1;
	int t=p->Dr7;
	t&=0xFFFF03FF;
	t&=~(3<<(2*id));t&=~(0xF<<(16+4*id));
	t|=en<<(id*2);
	t|=(mode|(len<<2))<<(16+4*id);
	if (p->Dr6&0xF) {
		printf("hbp Detect %x\n",p->Dr6);
	}
	p->Dr6=0;
	p->Dr7=t;
	if (0) {
	printf("Dr0=%x\n",p->Dr0);
	printf("Dr1=%x\n",p->Dr1);
	printf("Dr2=%x\n",p->Dr2);
	printf("Dr3=%x\n",p->Dr3);
	printf("Dr6=%x\n",p->Dr6);
	printf("Dr7=%x\n",p->Dr7);
	}
}
void hbp(CONTEXT *p,int id,int addr,int write,int len) {
	if (len==1) len=0;
	else if (len==1) len=2;
	else if (len==4) len=3;
	else {printf("len %d error\n",len);return;}
	sethbp(p,id,addr,write?1:3,len,1);
}
void onBpt(int threadId) {
	CONTEXT context;
	int tid=de.dwThreadId;
	int addr=(int)de.u.Exception.ExceptionRecord.ExceptionAddress;
	//printf("bpt tid=%d %x\n",tid,addr);
	int code=-1;
	for (int i=0;i<nbp;i++) {
		struct Bp *p=&bps[i];
		if (p->addr==addr) code=p->code&0xFF;
	}
	if (code!=-1) w1(handle,addr,(u8)code);
	HANDLE ht=openThread(tid);if (!ht) return;
	context.ContextFlags=CONTEXT_CONTROL|CONTEXT_INTEGER|CONTEXT_DEBUG_REGISTERS;// CONTEXT_DEBUG_REGISTERS CONTEXT_ALL
	if (!GetThreadContext(ht,&context)) {exitFlag++;printf("get thread context error\n");goto end;}
	int eip=context.Eip;
	if (code!=-1) {
		context.Eip--;
		if (!SetThreadContext(ht,&context)) {exitFlag++;printf("set thread context error\n");goto end;}
	}
	bpt(threadId,addr,&context);
	if (code!=-1&&!exitFlag) context.EFlags|=(1<<8); //single step
	//printf("eflags %x\n",context.EFlags);
	if (!SetThreadContext(ht,&context)) {exitFlag++;printf("set thread context error\n");}
end:
	CloseHandle(ht);
	//printf("need reinstall tid=%d %x %x\n",tid,addr,code);
	if (code!=-1&&!exitFlag) {
		struct Th *pth=&ths[tid%8192];
		pth->id=tid;
		pth->addr=addr;
	}
}
void onDll() {
	return;
	//char name[256];
	int base=(int)de.u.LoadDll.lpBaseOfDll;
	LPVOID name=de.u.LoadDll.lpImageName;
	//GetModuleFileName(de.u.LoadDll.hFile,name,255);
	if (logfp) fprintf(logfp,"------- %x %s -----------\n",base,name);
	char *Mask = "*";
	if (SymEnumSymbols(handle,base,Mask,EnumSymProc,NULL)) {
	} else {perr();printf("SymEnumSymbols failed: %d\n", GetLastError());}
}
void onHbp(CONTEXT *pc,int id,int addr);
void onStep() {
	CONTEXT context;
	int tid=de.dwThreadId;
	int addr=(int)de.u.Exception.ExceptionRecord.ExceptionAddress;
	//printf("step %d %x\n",tid,addr);
	struct Th *pth=&ths[tid%8192];
	if (pth->id!=tid) {printf("ERR: tid err %d %d\n",pth->id,tid);return;}
	HANDLE ht=openThread(tid);if (!ht) return;
	context.ContextFlags=CONTEXT_CONTROL|CONTEXT_INTEGER|CONTEXT_DEBUG_REGISTERS;// CONTEXT_ALL
	if (!GetThreadContext(ht,&context)) {exitFlag++;printf("get thread context error\n");goto end;}
	//printf("eflags %x\n",context.EFlags);
	if (context.EFlags&(1<<8)) {//step
		context.EFlags&=~(1<<8); //single step
	}
	if (context.Dr6&0xF) { //hardware breakpoint
		if (context.Dr6&1) onHbp(&context,0,addr);
		if (context.Dr6&2) onHbp(&context,1,addr);
		if (context.Dr6&4) onHbp(&context,2,addr);
		if (context.Dr6&8) onHbp(&context,3,addr);
		context.Dr6=0;
	} else {
		if (!exitFlag) {
			w1(handle,pth->addr,0xcc);
			//printf("reinstall %x\n",pth->addr);
		}
	}
	if (!SetThreadContext(ht,&context)) {exitFlag++;printf("set thread context error\n");}
end:
	CloseHandle(ht);
}
void dbg() {
	assert(DebugActiveProcess(pid));
	EnterDebugLoop(&de);
	removeAllBp();
	printf("stop\n");
	DebugActiveProcessStop1(pid);
	printf("done\n");
}
void onThread(const LPDEBUG_EVENT DebugEv) {
	int id=DebugEv->dwThreadId;
	HANDLE hThread=DebugEv->u.CreateThread.hThread;
	printf("Thread %d base=0x%x start=0x%x\n",id,
		DebugEv->u.CreateThread.lpThreadLocalBase,
		DebugEv->u.CreateThread.lpStartAddress);
	return;
	threadIds[nThread++]=id;
	CONTEXT context;
	context.ContextFlags=CONTEXT_CONTROL|CONTEXT_INTEGER|CONTEXT_DEBUG_REGISTERS;// CONTEXT_ALL
	if (!GetThreadContext(hThread,&context)) {exitFlag++;printf("get thread context error\n");return;}
	int addr=0x6FBCC2F4;
	printf("install hbp %x on thread 0x%x\n",addr,id);
	hbp(&context,0,addr,1,4);//write
	if (!SetThreadContext(hThread,&context)) {exitFlag++;printf("set thread context error\n");}
}
int START=0x10008bd0;
int END=0x10008bf0;
int addr1=0x10007a40;
int addr2=0x10007a60;
int addr3;
void onProcess() {
	//addr1=0x7C831EC5;installBp(addr1); //DeleteFileA
	addr2=0x6FA4CBD8;installBp(addr2); 
	//addr3=0x77d561b3;installBp(addr3);
	//installBp(START); //start
	//installBp(END); //end
	return;
}
void ihbp(CONTEXT *pc) {
	int raddr=0x12d8df;
	int raddr2=0x12573f;
	//hbp(pc,0,raddr,0,4);//read
	//hbp(pc,0,raddr,1,4);//write
	//hbp(pc,1,raddr2,1,4);//write
}
void onHbp(CONTEXT *pc,int id,int addr) {
	printf("*** hbp %d addr=%x eip=%x\n",id,addr,pc->Eip);
	printf("	eax=%x ebx=%x ecx=%x edx=%x\n",pc->Eax,pc->Ebx,pc->Ecx,pc->Edx);
	printf("	esi=%x edi=%x esp=%x \n",pc->Esi,pc->Edi,pc->Esp);
}
void isbp() {
}
void bpt(int threadId,int addr,CONTEXT *pc) {
	static int id=0,nextMs=0,lastId=0;
	int espV[16],is[16];
	char buf[256],buf2[256];
	//printf("* bp %x\n",addr);
	int esp=pc->Esp;
	rbuf(handle,esp,(char *)espV,32);
	int pos=0,args=-1;
	if (addr==START) {
		printf("--- start\n");
		ihbp(pc);
		//isbp();
	} else if (addr==END) {
		printf("--- end\n");
		removeAllHbps(pc);
		exitFlag=1;
	} else if (addr==addr1) {
		static int rets[64],nret=0;
		int r=espV[0];
		if (nret<64) {
			for (int i=0;i<nret;i++) if (rets[i]==r) goto has;
			rets[nret++]=r;
			printf("ret%d: %x\n",nret,r);
		}
		has:
		rbuf(handle,espV[1],buf,128);
		printf("ret=%x %x %s\n",espV[0],espV[1],buf);
		return;
		args=3;
		exitFlag=1;
	} else if (addr==addr2) {
		//rbuf(handle,espV[1],buf,128);
		//rbuf(handle,espV[2],buf2,128);
		printf("ret=%x\n",espV[0]);
		args=0;
	} else {
		args=3;
	}
	if (args>=0) {
		printf("%d:%x esp=%x ret=%x ",threadId,addr,pc->Esp,espV[0]);
		for (int i=0;i<args;i++) {
			printf(" %d:0x%x",i,espV[1+i]);
		}
	}
	printf("\n");
	return;
}

char *load(char *path,int *psize) {
	FILE *fp=fopen(path,"rb");assert(fp);
	fseek(fp,0,2);int size=ftell(fp);*psize=size;
	char *p=malloc(size+1);p[size]=0;fseek(fp,0,0);
	int n=fread(p,1,size,fp);assert(n==size);fclose(fp);
	return p;
}
void save(char *path,char *buf,int size) {
	FILE *fp=fopen(path,"wb+");assert(fp);
	int n=fwrite(buf,1,size,fp);assert(n==size);fclose(fp);
}
HWND d2hwnd=NULL;
int enumWin(HWND hwnd,int param) {
	char buf[256];
	int n=GetWindowTextA(hwnd,buf,256);
	printf("%s\n",buf);
	n=GetClassName(hwnd,buf,256);
	if (n&&strcmp(buf,"Diablo II")==0) d2hwnd=hwnd;
}
HWND find_d2win(int id) {
	char buf[256],path[512];
	sprintf(path,"D:\\d2hackmap\\map113c\\runtime\\d2_%d.txt",id);
	FILE *fp=fopen(path,"r");if (!fp) return 0;
	HWND hwnd=0;
	while (1) {
		char *line=fgets(buf,256,fp);if (!line) break;
		int len=strlen(line);if (line[len-1]=='\n') {len--;line[len]=0;}if (line[len-1]=='\r') {len--;line[len]=0;}
		char *name=line;char *value=strchr(line,':');
		if (!value) continue;*value=0;value++;while (*value==' ') value++;
		if (strcmp(name,"hwnd")==0) hwnd=(HWND)strtol(value,0,16);
	}
	fclose(fp);
	if (!hwnd) return 0;
	int len=GetClassName(hwnd,buf,256);
	if (len>0&&strcmp(buf,"Diablo II")==0) return hwnd;
	return 0;
}
int main(int argc, char* argv[]) {
	//EnumWindows(enumWin,0);
	if (1) {
		HWND hwnd=find_d2win(1);//FindWindowA(NULL,title);
		if (!hwnd) hwnd=FindWindowA(NULL,"D2Loader v1.13c - Build On Jul 22 2010");
		assert(hwnd);
		if (GetWindowThreadProcessId(hwnd,&pid)==0) {printf("getprocessid error\n");return;} 
	} else {
		pid=find_process("tcc.exe");if (!pid) return;
	}
	handle=OpenProcess(PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_VM_OPERATION|PROCESS_ALL_ACCESS,0,pid);
	//SymInitialize(handle,NULL,1);
	printf("handle=%x\n",handle);
	char *path="kernel32.dll";
	DWORD offset = (DWORD)LoadLibrary(path);
	if (!offset) {printf("Load custom library %s failed\r\n", path);return 0;} 
	HMODULE module = GetModuleHandle(path);
	DebugActiveProcessStopAddr = GetProcAddress(module,(LPCSTR)"DebugActiveProcessStop");
	if (!DebugActiveProcessStopAddr) {printf("failed\r\n");return 0;}
	logfp=fopen("d:\\d2hackmap\\src\\tools\\t\\my_log.txt","w+");assert(logfp);
	sedebug();
	endMs=GetTickCount()+3000;
	endMs=GetTickCount()+3000000;
	dbg();
	if (logfp) fclose(logfp);
	return 0;
}

