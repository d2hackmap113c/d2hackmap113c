#include "stdafx.h"
#include "d2gs.h"
#include "config.h"
#include "eventlog.h"
#include "vars.h"

char gSomeBuffer[2048] = { 0 };
typedef struct ConfigMap {
	const char*	KeyName;
	int		Type; // 0-9
	const char*	DefaultValue;
	void*		ValuePtr;
}ConfigMap;

char conf_d2hackmap_path0[256],conf_d2hackmap_path[256],conf_d2hackmap_cfg_path[256];
static ConfigMap gConfigMap[]={
	{"d2hackmap_path0",04,"..\\map113c\\dll\\d2hackmap.dll", conf_d2hackmap_path0},
	{"d2hackmap_path",04,"..\\map113c\\dll\\d2hackmapS.dll", conf_d2hackmap_path},
	{"d2hackmap_cfg_path",04,"..\\map113c\\", conf_d2hackmap_cfg_path},
	{REGKEY_D2CSIP					, 04, "127.0.0.1"	, d2gsconf.d2csipstr},
	{REGKEY_D2CSIP					, 10, "127.0.0.1"	, &d2gsconf.d2csip},
	{REGKEY_D2CSPORT				, 9, "6113"			, &d2gsconf.d2csport},
	{REGKEY_D2DBSIP					, 04, "127.0.0.1" , d2gsconf.d2csipstr},
	{REGKEY_D2DBSIP					, 10, "127.0.0.1" , &d2gsconf.d2dbsip},
	{REGKEY_D2DBSPORT				, 9, "6114"			, &d2gsconf.d2dbsport},
	{REGKEY_ENABLENTMODE			, 01, "1"			, &d2gsconf.enablentmode},
	{REGKEY_ENABLEGEPATCH			, 01, "1"			, &d2gsconf.enablegepatch},
	{REGKEY_ENABLEPRECACHEMODE		, 01, "1"			, &d2gsconf.enableprecachemode},
	{REGKEY_ENABLEGELOG				, 01, "0"			, &d2gsconf.enablegelog},
	{REGKEY_ENABLEGEMSG				, 01, "0"			, &d2gsconf.enablegemsg},
	{REGKEY_ENABLEGSLOG				, 01, "1"			, &d2gsconf.enablegslog},
	{REGKEY_DEBUGNETPACKET			, 01, "0"			, &d2gsconf.debugnetpacket},
	{REGKEY_DEBUGEVENTCALLBACK		, 01, "0"			, &d2gsconf.debugeventcallback},
	{REGKEY_IDLESLEEP				, 03, "1"			, &d2gsconf.idlesleep},
	{REGKEY_BUSYSLEEP				, 03, "1"			, &d2gsconf.busysleep},
	{REGKEY_MAXGAMES				, 03, "5"			, &d2gsconf.gsmaxgames},
	{REGKEY_MAXPREFERUSERS			, 03, "0"			, &d2gsconf.maxpreferusers},
	{REGKEY_MAXGAMELIFE				, 03, "36000"		, &d2gsconf.maxgamelife},
	{REGKEY_MOTD					, 04, "No commercial Purpose!", &d2gsconf.motd},
	{REGKEY_CHARPENDINGTIMEOUT		, 03, "200"			, &d2gsconf.charpendingtimeout},
	{REGKEY_INTERVALRECONNECTD2CS	, 03, "50"			, &d2gsconf.intervalreconnectd2cs},
	{REGKEY_D2CSSECRECT				, 04, gSomeBuffer	, d2gsconf.d2cssecrect},
	{REGKEY_MULTICPUMASK			, 03, "1"			, &d2gsconf.multicpumask},
	{REGKEY_MAX_PACKET_PER_SECOND   , 03, "300"			, &d2gsconf.maxpacketpersecond},
	{REGKEY_SERVER_CONF_FILE		, 04, "D2Server.ini", d2gsconf.serverconffile},
	{REGKEY_ADMINPWD				, 04, "9e75a42100e1b9e0b5d3873045084fae699adcb0", d2gsconf.adminpwd},
	{REGKEY_ADMINPORT				, 9, "8888"			, &d2gsconf.adminport},
	{REGKEY_ADMINTIMEOUT			, 03, "600"			, &d2gsconf.admintimeout},
	{REGKEY_AUTOUPDATE				, 01, "0"			, &gAutoUpdate.AutoUpdate},
	{REGKEY_AUTOUPDATE_TIMEOUT		, 03, "30000"		, &gAutoUpdate.AutoUpdateTimeout},
	{REGKEY_AUTOUPDATEVER			, 03, "0"			, &gAutoUpdate.AutoUpdateVer},
	{REGKEY_AUTOUPDATE_URL			, 04, gSomeBuffer	, gAutoUpdate.AutoUpdateUrl},
	{0,0,0,0}
};
char mpq_path[256]={0};//"d:\\game\\mpq";

char isquote(char c) {return ((c) == '"' || (c) == '`') ? c : 0;}
void filterSpace(char *s) {
	char *src=s,*dst=s;int quote = 0;
	while(1){
		char c=*src++;if (!c) break;
		if((c==' '||c=='\t')&&!quote) continue;
		if(c=='/'&&*src=='/') break;
		if(isquote(c)) quote = !quote;
		*dst++=c;
	}
	*dst=0;
}
static int compareConfigVar(const void *a,const void *b) {
	return strcmp(((ConfigMap *)a)->KeyName,((ConfigMap *)b)->KeyName);
}
void loadConfig() {
	u_long		ipaddr;
	DWORD		dwval;
	char		strbuf[256];
	int nConf=0;
	for (ConfigMap *confs=gConfigMap;confs->KeyName;confs++) {
		switch (confs->Type - 1) {
			case 0:case 2:*((int *)confs->ValuePtr)=strtol(confs->DefaultValue,0,0);break;
			case 1:case 4:case 5:case 6:case 7:
				D2GSEventLog("D2GSLoadRegConfigs", "Unknow conf_type for %s", confs->KeyName);
				break;
			case 3: {
				strncpy((char*)strbuf, confs->DefaultValue, 0xFF);strbuf[0xFF] = 0;
				strcpy((char*)confs->ValuePtr, strbuf);
				break;
			}
			case 8:*((WORD*)confs->ValuePtr) = (WORD)atoi(confs->DefaultValue);break;
			case 9:
				strncpy((char*)strbuf, confs->DefaultValue, 0xFF);strbuf[0xFF] = 0;
				ipaddr = inet_addr(strbuf);
				if (ipaddr == INADDR_NONE) {
					D2GSEventLog("D2GSReadConfig", "Invalid ip address '%s'", strbuf);
				}
				*(DWORD*)(confs->ValuePtr) = ipaddr;
				break;
			default:
				D2GSEventLog("D2GSLoadRegConfigs", "Unknow conf_type for %s", confs->KeyName);
				break;
		}
		nConf++;
	}
	qsort(gConfigMap,nConf,sizeof(ConfigMap),compareConfigVar);
	FILE *fp=fopen("config.txt","r");if (!fp) return;
	while (1) {
		char buf[256];char *line=fgets(buf,256,fp);if (!line) break;
		filterSpace(line);
		if (line[0]=='#'||line[0]=='/') continue;
		int len=strlen(line);
		if (line[len-1]=='\n') {len--;line[len]=0;}
		char *key=line;
		char *value=strchr(line,':');if (!value) continue;
		*value=0;value++;
		while (*value==' ') value++;
		len=strlen(value);
		if (isquote(value[0])&&isquote(value[len-1])) {value[len-1]=0;value++;}
		if (_stricmp("mpq_path",key)==0) {
			strncpy(mpq_path,value,255);
		} else {
			ConfigMap c;c.KeyName=key;
			ConfigMap *confs=(ConfigMap *)bsearch(&c,gConfigMap,nConf,sizeof(ConfigMap),compareConfigVar);
			if (confs) {
				switch (confs->Type - 1) {
				case 0:case 2:*((int *)confs->ValuePtr)=strtol(value,0,0);break;
				case 1:case 4:case 5:case 6:case 7:
					D2GSEventLog("D2GSLoadRegConfigs", "Unknow conf_type for %s", confs->KeyName);
					break;
				case 3: {
					strncpy((char*)strbuf, value, 0xFF);strbuf[0xFF] = 0;
					strcpy((char*)confs->ValuePtr, strbuf);
					break;
				}
				case 8:*((WORD*)confs->ValuePtr) = (WORD)strtol(value,0,0);break;
				case 9:
					strncpy((char*)strbuf, value, 0xFF);strbuf[0xFF] = 0;
					ipaddr = inet_addr(strbuf);
					if (ipaddr == INADDR_NONE) {
						D2GSEventLog("D2GSReadConfig", "Invalid ip address '%s'", strbuf);
					}
					*(DWORD*)(confs->ValuePtr) = ipaddr;
					break;
				default:
					D2GSEventLog("D2GSLoadRegConfigs", "Unknow conf_type for %s", confs->KeyName);
					break;
				}
			}
		}
	}
	fclose(fp);
}

int D2GSReadConfig(void) {
	d2gsconf.enablegslog = TRUE;
	loadConfig();
	d2gsconf.d2dbsport = htons(d2gsconf.d2dbsport);
	d2gsconf.d2csport = htons(d2gsconf.d2csport);
	d2gsconf.adminport = htons(d2gsconf.adminport);
	d2gsconf.gemaxgames = 2 * d2gsconf.gsmaxgames + 0xc8;
	d2gsconf.curgsmaxgames = 0;
	return TRUE;
} /* End of D2GSReadConfig() */

