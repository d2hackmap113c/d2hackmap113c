#include "stdafx.h"
#include "header.h"
#include "auto.h"

extern int dwRescueBarSafeDistance;
extern int dwScreenScrollCount,dwMinimapScrollCount;
extern ToggleVar 	tScreenScrollKeys[16];
extern int 			dwScreenScrollOffset[16][2];
extern ToggleVar tMiniMapScrollKeys[16];
extern int 			dwMiniMapScrollOffset[16][2];
extern int dwSwitchSkillKeyCount;
extern int dwSwitchSkillKeys[32];
extern int dwDumpMpqPath;
ToggleVar *tKeyEventMap[8*256] = {0};
FILE *logfp=NULL;
char szRuntimePath[256]="runtime";
static int is_utf8_config=0;
static char *cfgFileName=NULL;
int fShowCheckInfo=1;
int fLoadItemColours=1;
struct WarningMsg {char *msg;struct WarningMsg *next;};
struct WarningMsg *warningMsgs=NULL;
static int warningMsgCount=0,lineId;
struct Symbol {char *name,*value;};
#define TMP_SYMBOLS 128
struct Symbol *pSymbols=NULL;
struct Symbol tmpSymbols[TMP_SYMBOLS];
int nSymbols=0,capSymbols=0,nTmpSymbols=0,fSymbolSorted=0;

int DoTest();
void packetDebug();
int ScrollScreenStart(int idx);
int ScrollScreenEnd(int idx);
void addAccount(int key,char *name,char *pass);
void addMapTarget(int srcLvl,int dstLvl,int type,wchar_t *enName,wchar_t *chName);
int MiniMapScroll(int id);
void addQuickMessage(int key,wchar_t *msg,int len);
void addQuickToLevel(int key,int area1,int area2);

static ConfigVar **configVars=NULL;int configVarCap=0,configVarCount=0;
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_INT, "ShowCFGCheckInfo",			&fShowCheckInfo  ,   4 },
	{CONFIG_VAR_TYPE_STR, "RuntimePath",			szRuntimePath, 1,  {255 }},
	{CONFIG_VAR_TYPE_INT, "LoadItemColours",        &fLoadItemColours,  4},
	{CONFIG_VAR_TYPE_KEY, "ShowTestInfoToggle",          &tShowTestInfo         },
	{CONFIG_VAR_TYPE_INT, "DumpMpqPath",          &dwDumpMpqPath,4         },
};
int compareConfigVar(const void *a,const void *b) {
	return _stricmp((*(struct ConfigVar **)a)->szCmdName,(*(struct ConfigVar **)b)->szCmdName);
}
int compareSymbol(const void *a,const void *b) {
	return _stricmp(((struct Symbol *)a)->name,((struct Symbol *)b)->name);
}
ConfigVar *findConfigVar(char *name) {
	ConfigVar c,*pc;c.szCmdName=name;pc=&c;
	ConfigVar *cv=*(ConfigVar **)bsearch(&pc,configVars,configVarCount,sizeof(struct ConfigVar *),compareConfigVar);
	if (cv&&strcmp(name,cv->szCmdName)!=0) LOG("Case Warning: %s\n",name);
	return cv;
}
void addConfigVar(ConfigVar *var) {
	if (configVarCount>=configVarCap) {
		configVarCap*=2;configVars=(ConfigVar **)HeapReAlloc(confHeap,0,configVars,sizeof(ConfigVar *)*configVarCap);
	}
	configVars[configVarCount++]=var;
}
void Install_AddConfigVars();void Resolution_AddConfigVars();void Snapshot_AddConfigVars();
void lifebar_addConfigVars();void automap_addConfigVars();void winmsg_addConfigVars();
void dangerous_addConfigVars();void env_addConfigVars();void quest_addConfigVars();
void packet_addConfigVars();void DropProtection_addConfigVars();void multiclient_addConfigVars();
void gamemonitor_addConfigVars();void item_addConfigVars();void autoskill_addConfigVars();
void info_AddConfigVars();void NpcTrade_AddConfigVars();void messagelog_AddConfigVars();
void quick_addConfigVars();void quickswap_addConfigVars();void PartyHelp_addConfigVars();
void GameChat_addConfigVars();void misc_addConfigVars();void loop_addConfigVars();
void autoteleport_addConfigVars();void autoroute_addConfigVars();
void panel_addConfigVars();void server_addConfigVars();void uber_addConfigVars();
void gameFilter_addConfigVars();void common_addConfigVars();

void login_initConfigVars();
void automap_initConfigVars();void lifebar_initConfigVars();void env_initConfigVars();
void autoskill_initConfigVars();void waypoint_initConfigVars();void winmsg_initConfigVars();
void item_initConfigVars();void dangerous_initConfigVars();void DropProtection_initConfigVars();
void multiclient_initConfigVars();void packet_initConfigVars();void gamemonitor_initConfigVars();
void autoteleport_initConfigVars();void chat_initConfigVars();
static void InitValues(){
	static char *base0arrays[]={
		"DropProtectionRuneword","DropProtectionUnique", 
		"DropProtectionSet","MonitorSendCmd","MonitorRecvCmd","AutoFollowClickObject"};
	configVarCount=0;configVarCap=1024;
	configVars=(ConfigVar **)HeapAlloc(confHeap,0,sizeof(ConfigVar *)*configVarCap);
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
	Install_AddConfigVars();Resolution_AddConfigVars();Snapshot_AddConfigVars();
	env_addConfigVars();quest_addConfigVars();packet_addConfigVars();
	DropProtection_addConfigVars();multiclient_addConfigVars();gamemonitor_addConfigVars();
	item_addConfigVars();autoskill_addConfigVars();info_AddConfigVars();
	NpcTrade_AddConfigVars();messagelog_AddConfigVars();quick_addConfigVars();
	quickswap_addConfigVars();PartyHelp_addConfigVars();GameChat_addConfigVars();
	misc_addConfigVars();loop_addConfigVars();autoteleport_addConfigVars();
	autoroute_addConfigVars();lifebar_addConfigVars();automap_addConfigVars();
	winmsg_addConfigVars();dangerous_addConfigVars();server_addConfigVars();
	uber_addConfigVars();panel_addConfigVars();gameFilter_addConfigVars();
	common_addConfigVars();
	LOG("%d config vars\n",configVarCount);
	qsort(configVars,configVarCount,sizeof(ConfigVar *),compareConfigVar);
	for (int i=0;i<configVarCount;i++) {
		configVars[i]->useCount=0;
		configVars[i]->base=1;
	}
	for (int i=0;i<_ARRAYSIZE(base0arrays);i++) {
		ConfigVar *pcv=findConfigVar(base0arrays[i]);
		if (pcv) pcv->base=0;
		else LOG("Can't find %s\n",base0arrays[i]);
	}
	if (!capSymbols) {
		capSymbols=8192;
		pSymbols=(struct Symbol *)malloc(capSymbols*sizeof(struct Symbol));
	}
	nSymbols=0;warningMsgCount=0;
	memset(tKeyEventMap,0,sizeof(tKeyEventMap));
	warningMsgs=NULL;
	login_initConfigVars();winmsg_initConfigVars();automap_initConfigVars();env_initConfigVars();
	lifebar_initConfigVars();item_initConfigVars();dangerous_initConfigVars();
	DropProtection_initConfigVars();packet_initConfigVars();gamemonitor_initConfigVars();
	autoteleport_initConfigVars();multiclient_initConfigVars();autoskill_initConfigVars();
	waypoint_initConfigVars();chat_initConfigVars();
}

void WinMessageFixValues();void quest_FixValues();void packet_FixValues();
void info_FixValues();void Install_FixValues();void multiclient_FixValues();
void autoteleport_fixValues();void autoroute_fixValues();void automap_fixValues();
void FixValues(){
	Install_FixValues();info_FixValues();quest_FixValues();
	packet_FixValues();multiclient_FixValues();WinMessageFixValues();
	autoteleport_fixValues();autoroute_fixValues();automap_fixValues();
}

static const char *vk_names[256]={
0,"LBUTTON"/*01*/, "RBUTTON"/*02*/, "CANCEL"/*03*/, "MBUTTON"/*04*/, "XBUTTON1"/*05*/, "XBUTTON2"/*06*/,0,
"BACK"/*08*/, "TAB"/*09*/,0,0, "CLEAR"/*0C*/, "RETURN"/*0D*/,0,0,
"SHIFT"/*10*/, "CONTROL"/*11*/, "MENU"/*12*/, "PAUSE"/*13*/, "CAPITAL"/*14*/, "KANA"/*15*/,0, "JUNJA"/*17*/,
"FINAL"/*18*/, "HANJA"/*19*/,0, "ESCAPE"/*1B*/, "CONVERT"/*1C*/, "NONCONVERT"/*1D*/, "ACCEPT"/*1E*/, "MODECHANGE"/*1F*/,
"SPACE"/*20*/, "PRIOR"/*21*/, "NEXT"/*22*/, "END"/*23*/, "HOME"/*24*/, "LEFT"/*25*/, "UP"/*26*/, "RIGHT"/*27*/,
"DOWN"/*28*/, "SELECT"/*29*/, "PRINT"/*2A*/, "EXECUTE"/*2B*/, "SNAPSHOT"/*2C*/, "INSERT"/*2D*/, "DELETE"/*2E*/, "HELP"/*2F*/,
"0"/*30*/, "1"/*31*/, "2"/*32*/, "3"/*33*/, "4"/*34*/, "5"/*35*/, "6"/*36*/, "7"/*37*/,
"8"/*38*/, "9"/*39*/, 0,0,0,0,0,0,
0,"A"/*41*/, "B"/*42*/, "C"/*43*/, "D"/*44*/, "E"/*45*/, "F"/*46*/, "G"/*47*/,
"H"/*48*/, "I"/*49*/, "J"/*4A*/, "K"/*4B*/, "L"/*4C*/, "M"/*4D*/, "N"/*4E*/, "O"/*4F*/,
"P"/*50*/, "Q"/*51*/, "R"/*52*/, "S"/*53*/, "T"/*54*/, "U"/*55*/, "V"/*56*/, "W"/*57*/,
"X"/*58*/, "Y"/*59*/, "Z"/*5A*/, "LWIN"/*5B*/, "RWIN"/*5C*/, "APPS"/*5D*/,0, "SLEEP"/*5F*/,
"NUMPAD0"/*60*/, "NUMPAD1"/*61*/, "NUMPAD2"/*62*/, "NUMPAD3"/*63*/, "NUMPAD4"/*64*/, "NUMPAD5"/*65*/, "NUMPAD6"/*66*/, "NUMPAD7"/*67*/,
"NUMPAD8"/*68*/, "NUMPAD9"/*69*/, "MULTIPLY"/*6A*/, "ADD"/*6B*/, "SEPARATOR"/*6C*/, "SUBTRACT"/*6D*/, "DECIMAL"/*6E*/, "DIVIDE"/*6F*/,
"F1"/*70*/, "F2"/*71*/, "F3"/*72*/, "F4"/*73*/, "F5"/*74*/, "F6"/*75*/, "F7"/*76*/, "F8"/*77*/,
"F9"/*78*/, "F10"/*79*/, "F11"/*7A*/, "F12"/*7B*/, "F13"/*7C*/, "F14"/*7D*/, "F15"/*7E*/, "F16"/*7F*/,
"F17"/*80*/, "F18"/*81*/, "F19"/*82*/, "F20"/*83*/, "F21"/*84*/, "F22"/*85*/, "WHEELUP"/*86*/, "WHEELDOWN"/*87*/,
0,0,0,0,0,0,0,0,
"NUMLOCK"/*90*/, "SCROLL"/*91*/, "NUM="/*92*/,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,"BROWSER_BACK"/*A6*/,"BROWSER_FORWARD"/*A7*/,"BROWSER_REFRESH"/*A8*/,
"BROWSER_STOP"/*A9*/, "BROWSER_SEARCH"/*AA*/, "BROWSER_FAVORITES"/*AB*/, "BROWSER_HOME"/*AC*/, "VOLUME_MUTE"/*AD*/, "VOLUME_DOWN"/*AE*/, "VOLUME_UP"/*AF*/,
"MEDIA_NEXT_TRACK"/*B0*/, "MEDIA_PREV_TRACK"/*B1*/, "MEDIA_STOP"/*B2*/, "MEDIA_PLAY_PAUSE"/*B3*/, "LAUNCH_MAIL"/*B4*/, "LAUNCH_MEDIA_SELECT"/*B5*/, "LAUNCH_APP1"/*B6*/, "LAUNCH_APP2"/*B7*/,
0,0,";:"/*BA*/, "+"/*BB*/, ","/*BC*/, "-"/*BD*/, "."/*BE*/, "/?"/*BF*/,
"`~"/*C0*/,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0, "[{"/*DB*/, "\\|"/*DC*/, "]}"/*DD*/, "'\""/*DE*/, "OEM_8"/*DF*/,
0,"OEM_AX"/*E1*/, "OEM_102"/*E2*/, "ICO_HELP"/*E3*/, "ICO_00"/*E4*/, "PROCESSKEY"/*E5*/, "ICO_CLEAR"/*E6*/, "PACKET"/*E7*/,
0, "OEM_RESET"/*E9*/, "OEM_JUMP"/*EA*/, "OEM_PA1"/*EB*/, "OEM_PA2"/*EC*/, "OEM_PA3"/*ED*/, "OEM_WSCTRL"/*EE*/, "OEM_CUSEL"/*EF*/,
"OEM_ATTN"/*F0*/, "OEM_FINISH"/*F1*/, "OEM_COPY"/*F2*/, "OEM_AUTO"/*F3*/, "OEM_ENLW"/*F4*/, "OEM_BACKTAB"/*F5*/, "ATTN"/*F6*/, "CRSEL"/*F7*/,
"EXSEL"/*F8*/, "EREOF"/*F9*/, "PLAY"/*FA*/, "ZOOM"/*FB*/, "NONAME"/*FC*/, "PA1"/*FD*/, "OEM_CLEAR"/*FE*/,0,
};
void formatKey(char *buf,int vk) {
	if (vk&0x400) buf+=sprintf(buf,"SHIFT+");
	if (vk&0x200) buf+=sprintf(buf,"CTRL+");
	if (vk&0x100) buf+=sprintf(buf,"ALT+");
		buf+=sprintf(buf,"%s",vk_names[vk&0xFF]);
}
void formatKeyW(wchar_t *buf,int vk) {
	if (vk&0x400) buf+=wsprintfW(buf,L"SHIFT+");
	if (vk&0x200) buf+=wsprintfW(buf,L"CTRL+");
	if (vk&0x100) buf+=wsprintfW(buf,L"ALT+");
	switch (vk&0xFF) {
		case 0x86:buf+=wsprintfW(buf,dwGameLng?L"鼠标滚轮上":L"WheelUp");break;
		case 0x87:buf+=wsprintfW(buf,dwGameLng?L"鼠标滚轮下":L"WheelDown");break;
		default:buf+=wsprintfW(buf,L"%hs",vk_names[vk&0xFF]);break;
	}
}
void setupKey(ToggleVar *p) {
	if (p->key==0xff) return;
	if (1<=p->key&&p->key<8*256) {p->next=tKeyEventMap[p->key];tKeyEventMap[p->key]=p;}
}
static void initKeyArray(ConfigVar *config ,int index ,int dimId){
	if (config->anArrayMax[dimId]>0) {
		for (int j = 0; j < config->anArrayMax[dimId]; j++)
			initKeyArray(config, index*(config->anArrayMax[dimId])+j, dimId+1);
	} else {
		struct ToggleVar *p=(struct ToggleVar *)config->pVar;
		p+=index*config->size32;
		for (int i=0;i<config->size32;i++) setupKey(p++);
	}
}
ToggleVar *addExtraKey(int key) {
	if (key<0||key>=0x800) return NULL;
	ToggleVar *p=(ToggleVar *)HeapAlloc(confHeap,0,sizeof(ToggleVar));memset(p,0,sizeof(ToggleVar));
	p->next=tKeyEventMap[key];tKeyEventMap[key]=p;
	return p;
}
void automap_initKeyEvent();
void WinMessage_initKeyEvent();
void InitKeyEvent(){
	for ( int i = 0; i < configVarCount; i++ ) {
		struct ConfigVar *pc=configVars[i];
		if (pc->type==CONFIG_VAR_TYPE_KEY) {
			ToggleVar *p = (ToggleVar *)pc->pVar;
			setupKey(p);
		} else if (pc->type==CONFIG_VAR_TYPE_KEY_ARRAY0||pc->type==CONFIG_VAR_TYPE_KEY_ARRAY1) {
			initKeyArray(pc,0,0);
		}
	}
	automap_initKeyEvent();
	WinMessage_initKeyEvent();
	FILE *fp=openFile("runtime\\keymap.log","w+");
	if (!fp) return;
	fprintf(fp,"Auto Generated Key Map report. DO NOT MODIFY\n");
	fprintf(fp,"自动生成的键盘布局报告，不要修改此文件\n");fflush(fp);
	for (int vk=0;vk<256;vk++) {
		for (int mod=0;mod<8;mod++) {
			int k=(mod<<8)|vk;
			ToggleVar *p=tKeyEventMap[k];
			if (!p) continue;
			if (IsBadReadPtr(p,sizeof(ToggleVar))) {
				fprintf(fp,"Error: can't access key=%x ptr=%x",k,p);fflush(fp);
				continue;
			}
			if (p->next) fprintf(fp,"Multiple ");
			fprintf(fp,"Key");
			fprintf(fp," 0x%02x ",k);
			if (vk_names[k&0xFF]) {
				if (k&0x400) fputs("SHIFT+",fp);
				if (k&0x200) fputs("CTRL+",fp);
				if (k&0x100) fputs("ALT+",fp);
				fprintf(fp,"%s",vk_names[k&0xFF]);
			}
			fprintf(fp,":");
			while (p) {
				if (p->desc) fprintf(fp," \"%s\"",p->desc);
				else fprintf(fp," (%d)",p->type);
				if (p->keyType==1) fprintf(fp,"(OutGame)");
				else if (p->keyType==2) fprintf(fp,"(Always)");
				p=p->next;
			}
			fprintf(fp,"\n");
			fflush(fp);
		}
	}
	fclose(fp);
}
void AddWarningMessage(char *msg,int keep) {
	char buf[256];
	for (WarningMsg *p=warningMsgs;p;p=p->next) if(_stricmp(p->msg,msg)==0) return;
	if (warningMsgCount>=199) return ; 
	if (keep) strncpy(buf,msg,99);
	else sprintf(buf,"Parse Error line %d: %s", lineId,msg);
	LOG("Warning: %s\n",buf);
	WarningMsg *p=(WarningMsg *)HeapAlloc(confHeap,0,sizeof(WarningMsg));
	p->msg=heap_strdup(confHeap,buf);p->next=warningMsgs;warningMsgs=p;warningMsgCount++;
}
void ShowWarningMessage(){
	if (fShowCheckInfo&&warningMsgCount>0) {
		for (WarningMsg *p=warningMsgs;p;p=p->next) gameMessage(p->msg);
		warningMsgs=NULL;warningMsgCount=0 ;
	}
}
char isquote(char c) {return ((c) == '"' || (c) == '`') ? c : 0;}
void filterSpace(char *s) {
	char *src=s,*dst=s;int quote = 0;
	while(1){
		char c=*src++;if (!c) break;
		//if(isspace(c)&&!quote) continue;
		if((c==' '||c=='\t')&&!quote) continue;
		if(c=='/'&&*src=='/') break;
		if(isquote(c)) quote = !quote;
		*dst++=c;
	}
	*dst=0;
}
void splitLine(char *line,char **pleft,char **pright){
	filterSpace(line);
	char *e = strchr(line,':');
	if (e) {*e=0;*pleft=line;*pright=e+1;return;}
	if (line[0]) {
		LOG("Warning config line: %s\n",line);
		e = strrchr(line,']');
		if (e) {*e=0;*pleft=line;*pright=e+1;return;}
	}
	*pleft=NULL;*pright=NULL;
}
struct Symbol *findSymbol(char *name) {
	if (!fSymbolSorted) {
		//LOG("sort symbols %d\n",nSymbols);
		qsort(pSymbols,nSymbols,sizeof(struct Symbol),compareSymbol);
		fSymbolSorted=1;
	}
	struct Symbol c;c.name=name;
	struct Symbol *p=(Symbol *)bsearch(&c,pSymbols,nSymbols,sizeof(struct Symbol),compareSymbol);
	if (p) return p;
	for (int i=0;i<nTmpSymbols;i++) {
		p=&tmpSymbols[i];
		if (_stricmp(p->name,name)==0) return p;
	}
	return NULL;
}
void doneNameValues() {
	if (nTmpSymbols<=0) return;
	if (nSymbols+nTmpSymbols>capSymbols) {
		capSymbols*=2;
		pSymbols=(struct Symbol *)realloc(pSymbols,capSymbols*sizeof(struct Symbol));
	}
	memcpy(pSymbols+nSymbols,tmpSymbols,nTmpSymbols*sizeof(struct Symbol));
	nSymbols+=nTmpSymbols;nTmpSymbols=0;
	fSymbolSorted=0;
}
void addNameValue(char *name,char *value) {
	if (nTmpSymbols>=TMP_SYMBOLS) {LOG("FATAL ERROR\n");return;}
	struct Symbol *p=&tmpSymbols[nTmpSymbols++];
	p->name=name;p->value=value;
	if (nTmpSymbols>=TMP_SYMBOLS) doneNameValues();
}
char *replaceSymbols(char *value){
	char buf[1024],word[1024];
	char *s = value;
	char *pt = buf;
	char *pw = word;
	while(*s&&!isalnum(*s)) *pt++ = *s++;
	while(1){
		char c=*s;
		if (c&&( isalnum(c) || isspace(c) || c=='_' || c=='\'' || isquote(c) )) {
			*pw++ = c;
		} else {
			*pw = 0;
			pw = word;
			if(*pw){
				struct Symbol *p=findSymbol(word);
				char *ptr = p?p->value:word;
				int len=strlen(ptr);
				memcpy(pt,ptr,len);
				pt+=len;
			}
			*pt++ = c;
		}
		if(!c) break;
		s++;
	}
	int len=strlen(value);
	int len2=strlen(buf);
	if (len2<=len) {strcpy(value,buf);return value;}
	else return heap_strdup(confHeap,buf);
}
int ConvertToWString(wchar_t *dst,char *src,int size) {
	return MultiByteToWideChar(is_utf8_config?CP_UTF8:CP_ACP, 0, src, -1, dst, size);
}
void ConvertToD2String(wchar_t *dst,char *src,int size) {
	wchar_t *dst0=dst;
	wchar_t *end=dst+size;
	char col = 0;
	int hascolor = 0;
	while(1){
		char ch=*src++;if (!ch) break;
		if(ch!='%') {*dst++ = ch;continue;}
		char next=*src;if(!next) break;
		hascolor=1;
		if (isalpha(next)) {col = toupper(next)-'A'+10;src++;}
		else col=(char)strtol(src,&src,10);
		*dst++=0xff;*dst++='c';*dst++='0'+col;
  }
  if(hascolor) *dst++ = 0xff;
  *dst = 0 ;
  if(dst>=end) *end=0;
}
//int dbg_array=0;
BOOL StoreArrayConfig(ConfigVar *config ,int index ,int arrays, char *arrayStr , char *val,  int valsize,int iskey){
	int n=config->anArrayMax[arrays];
	if (n>0) {
		if(arrayStr[0]) {
			if (arrayStr[0]!='[') {LOG("%s:%d not array start %s\n",cfgFileName,lineId,arrayStr);return 0;}
			char *arrEnd=strchr(arrayStr,']');
			if (!arrEnd) {LOG("%s:%d Can't find array end %s\n",cfgFileName,lineId,arrayStr);return 0;}
			*arrEnd=0;
			arrayStr++;
			while(arrayStr[0]) {
				char *endptr=NULL;
				int num1=strtol(arrayStr,&endptr,0);
				if (endptr==arrayStr) {LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,arrayStr);return 0;}
				arrayStr=endptr;
				int num2 = num1;
				if ( (*arrayStr == '-') || (*arrayStr == '+') || (*arrayStr == '.') ) {
					arrayStr++;
					while (*arrayStr == '.') arrayStr++;
					num2=strtol(arrayStr,&endptr,0);
					if (endptr==arrayStr) num2 = config->anArrayMax[arrays];
					else arrayStr=endptr;
				}
				if (num1>num2) {int t=num1;num1=num2;num2=t;}
				if (num1<config->base) num1=config->base;
				if (num2>=config->anArrayMax[arrays]-1+config->base) num2=config->anArrayMax[arrays]-1+config->base;
				if (num1>num2) num1=num2;
				for (int j=num1;j<=num2;j++) {
					if (!StoreArrayConfig(config, index*(config->anArrayMax[arrays])+j-config->base, arrays+1, arrEnd+1, val,valsize,iskey)){
						return FALSE;
					};
				}
				if(arrayStr[0]) arrayStr++;
			}
			*arrEnd=']';
		} else {
			int idx0=index*n;
			if (!StoreArrayConfig(config, idx0, arrays+1, arrayStr , val,valsize,iskey)) return FALSE;
			int size=valsize;
			if (iskey) size*=sizeof(struct ToggleVar);
			for (int k=arrays+1;config->anArrayMax[k];k++) size*=config->anArrayMax[k];
			char *src=(char *)config->pVar+idx0*size;
			char *dst=src+size;
			for (int j = 1; j < n; j++) {
				//LOG("%d: copy %d <- %d size %d\n",lineId,dst-(char *)config->pVar,src-(char *)config->pVar,size);
				//executed 200K times each time config reload
				memcpy(dst, src, size);dst+=size;
			}
		}
	} else {
		if (arrayStr[0]) LOG("%s:%d Extra %s\n",cfgFileName,lineId,arrayStr);
		if (iskey) {
			struct ToggleVar *ptv=(struct ToggleVar *)config->pVar;
			int *pi=(int *)val;
			ptv+=index*valsize;
			for (int i=0;i<valsize;i++) {
				ptv->type=TOGGLEVAR_DOWNPARAM;
				ptv->key=*pi++;
				ptv->keyType=config->keyType;
				ptv->funcInt=config->keyFunc;
				ptv->param=index;
				if (!config->keyDesc) ptv->desc="(undefined)";
				else {
					char buf[256];
					sprintf(buf,config->keyDesc,index+1);
					ptv->desc=heap_strdup(confHeap,buf);
				}
				ptv++;
			}
		} else {
			/*if (dbg_array) {
				LOG("array: 0x%x+%d*%d <- %x %d\n",config->pVar,index,valsize,val,valsize);
			}*/
			memcpy((char *)config->pVar+index*valsize, val, valsize);
		}
	}
	return TRUE;
}
static int parseInt(char *s,char **pend) {
	char *e;int t=strtol(s,&e,0);if (e==s) {if (pend) *pend=e;return 0;}
	if (*e=='|') return t|parseInt(e+1,pend);
	else if (*e=='+') return t+parseInt(e+1,pend);
	if (pend) *pend=e;return t;
}
static void parseValues8(char *buf,char *values,int n) {
	int prev=0 ;
	for ( int i = 0 ; i < n ; i++ ){
		char *end;int t = parseInt(values, &end);
		if (values!=end) {values=end;buf[i]=(char)t;prev=t;}
		else buf[i]=(char)prev;
		if(*values) values++;
	}
	if (*values) LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,values);
}
static void parseValues32(int *buf,char *values,int n) {
	int def=0 ;
	for ( int i = 0 ; i < n ; i++ ){
		char *end;int t = parseInt(values, &end);
		if (values!=end) {values=end;buf[i]=t;}
		else buf[i]=def;
		if(*values) values++;
	}
	if (*values) LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,values);
}
void StoreConfig(ConfigVar *config , char *arrays , char *right) {
	if (!fLoadItemColours&&_memicmp(config->szCmdName,"ItemColours",11)==0) return; //for fast debugging
	if (!*right&&config->type!=CONFIG_VAR_TYPE_STR
		&&config->type!=CONFIG_VAR_TYPE_D2STR
		&&config->type!=CONFIG_VAR_TYPE_WSTR
		&&config->type!=CONFIG_VAR_TYPE_STR_ARRAY0
		&&config->type!=CONFIG_VAR_TYPE_STR_ARRAY1) {
		AddWarningMessage(config->szCmdName,0);
		return;
	}
	switch(config->type){
		case CONFIG_VAR_TYPE_CHAR_ARRAY0: 
		case CONFIG_VAR_TYPE_CHAR_ARRAY1: {
			char buf[32];
			config->base=config->type==CONFIG_VAR_TYPE_CHAR_ARRAY1?1:0;
			if (config->size32>=32) {LOG("FATAL: config size %d\n",config->size32);return;}
			parseValues8(buf,right,config->size32);
			//dbg_array=_stricmp(config->szCmdName,"DropProtectRuneword")==0;
			if (!StoreArrayConfig(config , 0 , 0 , arrays , buf,config->size32,0))
				AddWarningMessage(arrays,0);
			break;
		}
		case CONFIG_VAR_TYPE_INT_ARRAY0: 
		case CONFIG_VAR_TYPE_INT_ARRAY1: {
			int ibuf[32];
			config->base=config->type==CONFIG_VAR_TYPE_INT_ARRAY1?1:0;
			if (config->size32>=32) {LOG("FATAL: config size %d\n",config->size32);return;}
			parseValues32(ibuf,right,config->size32);
			if (!StoreArrayConfig(config , 0 , 0 , arrays , (char *)ibuf,config->size32<<2,0))
				AddWarningMessage(arrays,0);
			break;
		}
		case CONFIG_VAR_TYPE_KEY_ARRAY0: 
		case CONFIG_VAR_TYPE_KEY_ARRAY1: {
			int ibuf[32];
			config->base=config->type==CONFIG_VAR_TYPE_KEY_ARRAY1?1:0;
			if (config->size32>=32) {LOG("FATAL: config size %d\n",config->size32);return;}
			parseValues32(ibuf,right,config->size32);
			if (!StoreArrayConfig(config , 0 , 0 , arrays , (char *)ibuf,config->size32,1))
				AddWarningMessage(arrays,0);
			break;
		}
		case CONFIG_VAR_TYPE_STR_ARRAY0:
		case CONFIG_VAR_TYPE_STR_ARRAY1: {
			config->base=config->type==CONFIG_VAR_TYPE_STR_ARRAY1?1:0;
			if (!StoreArrayConfig(config , 0 , 0 , arrays , (char *)right,config->size32,0))
				AddWarningMessage(arrays,0);
			break;
		}
		case CONFIG_VAR_TYPE_ACCOUNT: {
			int key=0;char *name,*pass="";
			char *p=arrays;while (*p&&*p!='[') p++;p++;
			if (*p) key=parseInt(p,&p);
			name=right;
			while (*right&&*right!='\"') right++;
			if (*right) {
				*right=0;right++;
				while (*right&&*right!='\"') right++;
				if (*right) {right++;pass=right;}
			}
			addAccount(key,name,pass);
			break;
		}
		case CONFIG_VAR_TYPE_LEVEL_TARGET: {
			int lvl=0,dstLvl=0,type=0,n;
			char *name=NULL;wchar_t wbuf[64],*enName=NULL,*chName=NULL;
			char *p=arrays;while (*p&&*p!='[') p++;p++;
			if (*p) lvl=parseInt(p,&p);
			while (*p&&*p!='[') p++;p++;
			if (*p) dstLvl=parseInt(p,&p);
			while (*right&&!isdigit(*right)&&*right!='-') right++;
			type=*right?parseInt(right,&right):-1; 
			if (!(*right)) goto addTargetEnd;
			while (*right&&*right!='\"') right++;
			if (!(*right)) goto addTargetEnd;
			right++;name=right;
			while (*right&&*right!='\"') right++;if (*right) {*right=0;right++;}
			n=ConvertToWString(wbuf,name,64);
			enName=(wchar_t *)HeapAlloc(confHeap,0,(n+1)*sizeof(wchar_t));
			memcpy(enName,wbuf,(n+1)*sizeof(wchar_t));
			if (!(*right)) goto addTargetEnd;
			while (*right&&*right!='\"') right++;
			if (!(*right)) goto addTargetEnd;
			right++;name=right;
			while (*right&&*right!='\"') right++;if (*right) {*right=0;right++;}
			n=ConvertToWString(wbuf,name,64);
			chName=(wchar_t *)HeapAlloc(confHeap,0,(n+1)*sizeof(wchar_t));
			memcpy(chName,wbuf,(n+1)*sizeof(wchar_t));
addTargetEnd:
			if (lvl&&dstLvl) addMapTarget(lvl,dstLvl,type,enName,chName);
			break;
		}
		case CONFIG_VAR_TYPE_KEY: {
				char *endptr;
				ToggleVar *tgKey = (ToggleVar *)config->pVar;
				tgKey->isLoad = TRUE;
				if( tgKey->type == 1){ //开关类型
					tgKey->isOn = parseInt(right,&endptr);//第一项 0 关 非0 开 
					if(*endptr) endptr++;
					if(*endptr) tgKey->key = parseInt(endptr,&endptr); //快捷键
					else tgKey->key = -1;
					if(*endptr) endptr++;
					if(*endptr) {
						tgKey->value32 = parseInt(endptr,&endptr); //默认值
					}
				} else {
					//按键触发事件
					tgKey->key = parseInt(right,&endptr);	//快捷键
					if(*endptr) endptr++;
					if(*endptr) tgKey->param = parseInt(endptr,&endptr); //参数
				}
				if (endptr&&*endptr) LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,endptr);
			}
			break;
		case CONFIG_VAR_TYPE_INT: {
			char *endptr;
			//数值
			if( config->size32 == 1 ){
				*((char *)config->pVar) = (char)parseInt(right,&endptr);
			}else if( config->size32 == 2 ){
				*((short *)config->pVar) = (short)parseInt(right,&endptr);
			}else if( config->size32 == 4 ){
				*((int *)config->pVar) = (int)parseInt(right,&endptr);
			}
			if (endptr&&*endptr) LOG("%s:%d Can't parse %s\n",cfgFileName,lineId,endptr);
			break;
		}
		case CONFIG_VAR_TYPE_STR: {
				//字符串，单字节
				if (config->size32 >1){
					for( int i=0 ; i <config->size32 ; i ++ ){
						strncpy( (char *)config->pVar + config->anArrayMax[0]*i, right , config->anArrayMax[0]-1);
					}
				}else{
					strncpy( (char *)config->pVar , right , config->anArrayMax[0]-1 );
				}
			}
			break;
		case CONFIG_VAR_TYPE_WSTR: {
				//Unicode character
				if (config->size32>1) {
					for (int i=0;i<config->size32;i++) {
						ConvertToWString((wchar_t *)config->pVar + config->anArrayMax[0]*i, right ,config->anArrayMax[0]-1);
					}
				}else{
					ConvertToWString((wchar_t *)config->pVar , right, config->anArrayMax[0]-1 );
				}
			}
			break;
		case CONFIG_VAR_TYPE_D2STR: {
				//字符串，双字节
				if (config->size32>1) {
					for (int i=0;i<config->size32;i++) 
						ConvertToD2String((wchar_t *)config->pVar+config->anArrayMax[0]*i,right,config->anArrayMax[0]-1);
				} else {
					ConvertToD2String((wchar_t *)config->pVar,right,config->anArrayMax[0]-1);
				}
			}
			break;
		case CONFIG_VAR_TYPE_MINIMAP_SCROLL: 
			if (arrays[0]!='[') {
					AddWarningMessage(arrays,0);
			} else {
				char buf[64],kbuf[32];
				char *p=arrays+1;
				int key=parseInt(p,&p);
				int id=-1;
				for (int i=0;i<dwMinimapScrollCount;i++) {if (tMiniMapScrollKeys[i].key==key) {id=i;break;}}
				if (id<0) {
					if (dwMinimapScrollCount>=16) {LOG("ERROR: too many screen scroll keys\n");return;}
					id=dwMinimapScrollCount++;
					ToggleVar *pv = (ToggleVar *)&tMiniMapScrollKeys[id];
					pv->type=TOGGLEVAR_DOWNPARAM;
					pv->key=key;pv->param=id;
					pv->func=MiniMapScroll;
					formatKey(kbuf,key);sprintf(buf,"MiniMapScroll[%s]",kbuf);pv->desc=heap_strdup(confHeap,buf);
				}
				while (*p==']') p++;
				int ibuf[2];parseValues32(ibuf,right,2);
				dwMiniMapScrollOffset[id][0]=ibuf[0];dwMiniMapScrollOffset[id][1]=ibuf[1];
			}
			break;
		case CONFIG_VAR_TYPE_SCREEN_SCROLL: 
			if (arrays[0]!='[') {
					AddWarningMessage(arrays,0);
			} else {
				char buf[64],kbuf[32];
				char *p=arrays+1;
				int key=parseInt(p,&p);
				int id=-1;
				for (int i=0;i<dwScreenScrollCount;i++) {if (tScreenScrollKeys[i].key==key) {id=i;break;}}
				if (id<0) {
					if (dwScreenScrollCount>=16) {LOG("ERROR: too many screen scroll keys\n");return;}
					id=dwScreenScrollCount++;
					ToggleVar *pv = (ToggleVar *)&tScreenScrollKeys[id];
					pv->type=TOGGLEVAR_DOWNUPPARAM;
					pv->key=key;pv->param=id;
					pv->func=ScrollScreenStart;
					pv->funcUp=ScrollScreenEnd;
					formatKey(kbuf,key);sprintf(buf,"ScreenScroll[%s]",kbuf);pv->desc=heap_strdup(confHeap,buf);
				}
				while (*p==']') p++;
				int ibuf[2];parseValues32(ibuf,right,2);
				dwScreenScrollOffset[id][0]=ibuf[0];dwScreenScrollOffset[id][1]=ibuf[1];
			}
			break;
		case CONFIG_VAR_TYPE_SWITCH_SKILL: 
			if (arrays[0]!='[') {
					AddWarningMessage(arrays,0);
			} else {
				char *p=arrays+1;
				int key=parseInt(p,&p);
				int id=-1;
				for (int i=0;i<dwSwitchSkillKeyCount;i++) {if (dwSwitchSkillKeys[i]==key) {id=i;break;}}
				if (id<0) {
					if (dwSwitchSkillKeyCount>=32) {LOG("ERROR: too many switch skill keys\n");return;}
					id=dwSwitchSkillKeyCount++;
					dwSwitchSkillKeys[id]=key;
				}
				while (*p==']') p++;
				int ibuf[4];if (config->size32>4) {LOG("FATAL: config size %d\n",config->size32);return;}
				parseValues32(ibuf,right,config->size32);
				if (!StoreArrayConfig(config , id , 1 , p , (char *)ibuf,config->size32<<2,0))
					AddWarningMessage(p,0);
			}
			break;
		case CONFIG_VAR_TYPE_SEND_MESSAGE: 
			if (arrays[0]!='[') AddWarningMessage(arrays,0);
			else {
				wchar_t buf[256];
				char *p=arrays+1;
				int key=parseInt(p,&p);
				int len=ConvertToWString(buf,right,255);if (len>255) len=255;buf[len]=0;
				addQuickMessage(key,buf,len);
			}
			break;
		case CONFIG_VAR_TYPE_QUICK_TO_LEVEL: 
			if (arrays[0]!='[') AddWarningMessage(arrays,0);
			else {
				char *p=arrays+1;
				int key=parseInt(p,&p);
				int ibuf[2];
				parseValues32(ibuf,right,2);
				addQuickToLevel(key,ibuf[0],ibuf[1]);
			}
			break;
		default:
			break;
	}
}

static int isNumber(char *s) {
	int number=1;
	if (s[0]=='0'&&s[1]=='x') {
		s+=2;
		while (1) {
			char c=*s++;if (!c) break;
			if ('0'<=c&&c<='9'||'a'<=c&&c<='f'||'A'<=c&&c<='F') ;
			else {number=0;break;}
		}
	} else {
		while (1) {
			char c=*s++;if (!c) break;
			if ('0'<=c&&c<='9') ;
			else {number=0;break;}
		}
	}
	return number;
}
void clearCharacterTag();
void addCharacterTag(char *name,char *tag);
void doneCharacterTag();
static void loadTagFile(char *path) {
	char *charname,*tag;
	FILE *fp=openFile(path,"rb");
	if (!fp) {LOG("load tag file %s failed\n",path);return;}
	int size=0;char *p=loadFile(confHeap,fp,&size);if (!p) {fclose(fp);return;}
	char *end=p+size;
	if (p[0]==(char)0xef&&p[1]==(char)0xbb&&p[2]==(char)0xbf) {
		p+=3;
		wchar_t *ws=(wchar_t *)HeapAlloc(confHeap,0,size*sizeof(wchar_t));
		int nws=MultiByteToWideChar(CP_UTF8,0,p,end-p,ws,size);
		char *acp=(char *)HeapAlloc(confHeap,0,(size+1)*sizeof(char));
		int n=WideCharToMultiByte(CP_ACP,0,ws,nws,acp,size,NULL,NULL);
		HeapFree(confHeap,0,p-3);
		acp[n]=0;p=acp;end=p+n;
		HeapFree(confHeap,0,ws);
	}
	int n=0;
	while (p<end) {
		char *e=strchr(p,'\n');if (!e) e=end;
		char *line=p;p=e+1;
		*e=0;if (e[-1]=='\r') {e--;*e=0;}
		int len=e-line;
		if (line[0]=='/'&&line[1]=='/') continue;
		if (line[0]=='#') continue;
		splitLine(line,&charname,&tag);
		if (charname&&charname[0]) {addCharacterTag(charname,tag);n++;}
	}
	doneCharacterTag();
	LOG("load %d tags from %s\n",n,path);
	fclose(fp);
}
static void loadSymbolFile(char *path) {
	char *names,*value;
	FILE *fp=openFile(path,"rb");
	if (!fp) {LOG("load symbol file %s failed\n",path);return;}
	int size=0;char *p=loadFile(confHeap,fp,&size);if (!p) return;
	fclose(fp);
	int b=nSymbols;
	//LOG("loading symbols %s\n",path);
	char *end=p+size;
	while (p<end) {
		char *e=strchr(p,'\n');if (!e) e=end;
		char *line=p;p=e+1;
		*e=0;if (e[-1]=='\r') {e--;*e=0;}
		int len=e-line;
		if (line[0]=='/'&&line[1]=='/') continue;
		if (line[0]=='#') continue;
		splitLine(line,&names,&value);
		if (names&&names[0]&&value&&value[0]) {
			if (!isNumber(value)) value=replaceSymbols(value);
			char *name = strtok(names,",");
			while(name) {
				addNameValue(name,value);
				name = strtok(NULL,",");
			}
		}
	}
	doneNameValues();
	LOG("load %d symbols from %s\n",nSymbols-b,path);
}
extern HMODULE hInstDLL;
static int changeLogPath(char *path) {
	char fmt[512],buf[512];
	int id=1;
	if (path[0]&&path[1]==':') sprintf(fmt,"%s",path);
	else sprintf(fmt,"%s%s",szPluginPath,path);
	for (id=1;id<=10;id++) {
		sprintf(buf,fmt,id);
		if (!fileExist(buf)) break;
		if (DeleteFileA(buf)) break;
	}
	char *mode="w+";
	FILE *f=openFile(buf,mode);
	if (f) {
		if (logfp&&logfp!=stdout) {fprintf(logfp,"Log Changed to %s\n",buf);fclose(logfp);}
		logfp=f;
		GetModuleFileName(hInstDLL,buf,512);
		LOG("%x %s\n",hInstDLL,buf);
	}
	return id;
}
struct LoadingConfig {char *path;struct LoadingConfig *next;} *loadingConfigs=NULL;
static void loadConfig(char *path) {
	for (LoadingConfig *lc=loadingConfigs;lc;lc=lc->next) {
		if (_stricmp(path,lc->path)==0) {LOG("ERROR: looping loading config %s\n",path);return;}
	}
	FILE *fp=openFile(path,"r");if (!fp) return;
	int utf8_config=0;
	char header[3];fread(header,1,3,fp);
	if (header[0]==(char)0xef&&header[1]==(char)0xbb&&header[2]==(char)0xbf) utf8_config=1;
	else fseek(fp,0,0);

	is_utf8_config=utf8_config;
	LOG("loading config %s\n",path);
	LoadingConfig *lc=(LoadingConfig *)HeapAlloc(confHeap,0,sizeof(LoadingConfig));
	lc->path=heap_strdup(confHeap,path);lc->next=loadingConfigs;loadingConfigs=lc;
	cfgFileName=path;
	int nLines=0;
	while (1) {
		char buf[1024];
		char *line=fgets(buf,1024,fp);if (!line) break;
		nLines++;
		lineId=nLines;
		int len=strlen(line);
		if (line[len-1]=='\n') {len--;line[len]=0;}
		if (line[len-1]=='\r') {len--;line[len]=0;}
		if (strcmp(line,"exit")==0) {LOG("Abort reading\n");break;}
		__try {
			char *name,*value;
			if (line[0]=='/'&&line[1]=='/') continue;
			if (line[0]=='#') continue;
			splitLine(line,&name,&value);
			if (!name&&!value) continue;
			if (!name||!value||!name[0]||!value[0]) {
				LOG("Warning config line: %s\n",line);
				AddWarningMessage(line,1);
				continue;
			}
			//LOG("%s %s\n",name,value);
			name=replaceSymbols(name);
			//如果第一个字符是" 则不作替换
			if (value[0]=='"') {
				value++;int len2=strlen(value);if (value[len2-1]=='"') {len2--;value[len2]=0;}
			} else {
				value=replaceSymbols(value);
			}
			int hascmd = 0;
			char *arr=name;
			while(*arr && ( isalnum(*arr) || isspace(*arr) || *arr=='_'  ) ) arr++;
			char arrStart=*arr;*arr='\0';
			if (name[0]){
				if (_stricmp(name,"LoadSymbolsFile")==0&&value[0]) {loadSymbolFile(value);continue;}
				if (_stricmp(name,"LoadConfigFile")==0&&value[0]) {
					loadConfig(value);
					cfgFileName=path;is_utf8_config=utf8_config;
					continue;
				}
				if (_stricmp(name,"LogPath")==0&&value[0]) {dwGameWindowId=changeLogPath(value);continue;}
				if (_stricmp(name,"LoadTagFile")==0&&value[0]) {loadTagFile(value);continue;}
				ConfigVar *pcv=findConfigVar(name);
				if (pcv) {
					pcv->useCount++;
					*arr=arrStart;
					StoreConfig(pcv,arr,value);
					hascmd = 1;
				}
			}
			if (!hascmd){
				LOG("Warning config line: %s\n",line);
				AddWarningMessage(line,1);
			}
		} __except(EXCEPTION_EXECUTE_HANDLER) {
			LOG("ERROR: config line %s\n", line);
			AddWarningMessage("Load CFG Error!" , 1 );
		}
	}
	LOG("Load %d lines from %s\n",nLines,path);
	if (loadingConfigs) loadingConfigs=loadingConfigs->next;
}
void LoadLogNames(FILE *fp);
int LoadConfigFile(char *path,int isServer) {
	char cfgName[MAX_PATH];
	strcpy(cfgName,path);
	char *cmdLine=(char *)GetCommandLine();
	char *s=strstr(cmdLine,"-d2hackmapcfg=");
	if (s) {
		s=strchr(s,'=');s++;strcpy(cfgName,s);
		s=strchr(cfgName,' ');if (s) *s=0;
	}
	LARGE_INTEGER perfFreq,perfStart,perfEnd;
	QueryPerformanceCounter(&perfStart);
	clearCharacterTag();
	if (confHeap) HeapDestroy(confHeap);
	confHeap=HeapCreate(0,128*1024,0);
	if (logfp&&logfp!=stdout) {fclose(logfp);logfp=NULL;}
	if (isServer) {
		dwGameWindowId=0;
	} else {
		dwGameWindowId=changeLogPath("runtime\\d2hackmap%d.log");
	}
	InitValues();
	loadingConfigs=NULL;
	loadConfig(cfgName);
	if (!fLoadItemColours) {
		AddWarningMessage("Load Item Colours disabled",1);
	}
	__try {
		FixValues();
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		AddWarningMessage("Fix value error" , 1 );
	}
	__try {
		InitKeyEvent();
	} __except(EXCEPTION_EXECUTE_HANDLER) {
		AddWarningMessage("Init key event error" , 1 );
	}
	//Check
	LOG("Game window id: %d\n",dwGameWindowId);
	for (int i=1;i<configVarCount;i++) {
		if (compareConfigVar(&configVars[i-1],&configVars[i])>=0)
			LOG("duplicate config name %s %s\n",configVars[i-1]->szCmdName,configVars[i]->szCmdName);
	}
	if (!fSymbolSorted) qsort(pSymbols,nSymbols,sizeof(struct Symbol),compareSymbol);
	for (int i=1;i<nSymbols;i++) {
		if (compareSymbol(&pSymbols[i-1],&pSymbols[i])>=0)
			LOG("duplicate symbol %s %s\n",pSymbols[i-1].name,pSymbols[i].name);
	}
	LOG("loading names\n");
	LoadLogNames(logfp);
	QueryPerformanceCounter(&perfEnd);
	QueryPerformanceFrequency(&perfFreq);
	double timeMs=(perfEnd.QuadPart-perfStart.QuadPart)*1000.0/perfFreq.QuadPart;
	LOG("Load config time %.3lf ms\n",timeMs);
	if (0&&logfp) {
		for (int i=0;i<configVarCount;i++) {
			if (configVars[i]->useCount==0) {
				fprintf(logfp,"Unused config: %s\n",configVars[i]->szCmdName);
			}
		}
	}
	//LOG("DoTest function: %x\n", &DoTest);
	//LOG("Packet debug function: %x\n", &packetDebug);
	//LOG("Weapons=%d Armors=%d\n",*d2common_pWeaponsTxts,*d2common_pArmorTxts);//Weapons=306 Armors=202
	return TRUE;
}
