#include "stdafx.h"
#include "d2ptrs.h"

struct BugQuestInfo{
	int nQuestNo; //������
	int nStatus ; //����״̬
	int nType ;   //����ģʽ
	BYTE *fEnable ; //�Ƿ�ʼ
	char *szMsg; //����
};

extern ToggleVar tQuickNextGame;
int QuickExitGame();
int QuickNextGame(int addnum);
int ReSetTimer();
void CheckUnitKM();
void SendChatMessageW(wchar_t *myMsg);
void setKDCountDown(int en);
void setKBCountDown(int team,int en);
int dwQuestAlertMs;
static int alertCount;
static BugQuestInfo aBugInfo[5] ;

int dwBugAlertTimes=10;
wchar_t wszBugKBAlertMessage[256]={0};
ToggleVar tBugKCountess={TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug KCountess Protect" , &ReSetTimer};
ToggleVar tBugKM={TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug KM Protect" , &ReSetTimer};
ToggleVar tBugKD={TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug KD Protect" , &ReSetTimer};
ToggleVar tBugKB={TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug KB Protect" , &ReSetTimer};
ToggleVar tBugAutoQuit={TOGGLEVAR_ONOFF,	1,	-1,	1, "Bug Auto Quit Toggle"	,&ReSetTimer};
ToggleVar tBugAutoQuitHell={TOGGLEVAR_ONOFF,	1,	-1,	1, "Bug Auto Quit Toggle(Hell)"	,&ReSetTimer};
ToggleVar tBugAutoQuitHellAct4={TOGGLEVAR_ONOFF,	1,	-1,	1, "Bug Auto Quit Toggle(HellAct4)"	,&ReSetTimer};
ToggleVar tBugAutoQuitHellAct5={TOGGLEVAR_ONOFF,	1,	-1,	1, "Bug Auto Quit Toggle(HellAct5)"	,&ReSetTimer};
ToggleVar tBugAllHellAlert={TOGGLEVAR_ONOFF,	0,	-1,	1, "Bug All Hell Aert Toggle(Hell)"	,&ReSetTimer};
ToggleVar tAlertNoBug={TOGGLEVAR_ONOFF,	1,	-1,	1, "AlertNoBug"};
static ConfigVar aConfigVars[] = {
  {CONFIG_VAR_TYPE_KEY, "BugKCountessToggle",          &tBugKCountess         },
  {CONFIG_VAR_TYPE_KEY, "BugKMToggle",          &tBugKM         },
  {CONFIG_VAR_TYPE_KEY, "BugKDToggle",          &tBugKD         },
  {CONFIG_VAR_TYPE_KEY, "BugKBToggle",          &tBugKB         },
  {CONFIG_VAR_TYPE_KEY, "BugAutoQuitToggle",    &tBugAutoQuit   },
  {CONFIG_VAR_TYPE_KEY, "BugAutoQuitHellToggle",    &tBugAutoQuitHell   },
  {CONFIG_VAR_TYPE_KEY, "BugAutoQuitHellAct4Toggle",    &tBugAutoQuitHellAct4   },
  {CONFIG_VAR_TYPE_KEY, "BugAutoQuitHellAct5Toggle",    &tBugAutoQuitHellAct5   },
  {CONFIG_VAR_TYPE_KEY, "BugAllHellAlertToggle",    &tBugAllHellAlert   },
  {CONFIG_VAR_TYPE_INT, "BugAlertTimes",        &dwBugAlertTimes ,4 },
  {CONFIG_VAR_TYPE_KEY, "AlertNoBug",           &tAlertNoBug    },
  {CONFIG_VAR_TYPE_WSTR, "BugKBAlertMessage",       &wszBugKBAlertMessage,     1,  {256 }},
};
void quest_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
int ReSetTimer(){
	dwQuestAlertMs=0;
	return 0;
}
void QuestNewAct() {ReSetTimer();}
static void checkQuest(int id,int bitmask1,int bitmask2,char *name) {
	wchar_t wszbuf[256];
	unsigned short *player=*QUESTDATA;
	unsigned short *game=*GAMEQUESTDATA;
	int fPlayer = player[id]&bitmask1;//d2common_CheckQuestFlag(QUESTDATA	,id,bit1);
	int fGame = game[id]&bitmask2;//d2common_CheckQuestFlag(GAMEQUESTDATA	,id,bit2);
	if ( fPlayer== 0 ) {//��һ�û���ָ������
		if ( fGame==0 ){ //��ǰ��Ϸ�����
			wsprintfW(wszbuf, L"Warning: Not a BugK%hs Game game=%04X player=%04X",name,game[id],player[id]);
			d2client_ShowGameMessage(wszbuf, 1); //red
		} else {
			wsprintfW(wszbuf, L"BugK%hs game=%04X player=%04X",name,game[id],player[id]);
			d2client_ShowGameMessage(wszbuf, 2); //green
		}
	} else {
		if ( fGame==0 ){ //��ǰ��Ϸ�����
			wsprintfW(wszbuf, L"K%hs game=%04X player=%04X",name,game[id],player[id]);
			d2client_ShowGameMessage(wszbuf, 2); //green
		} else {
			wsprintfW(wszbuf, L"Already completed %hs game=%04X player=%04X",name,game[id],player[id]);
			d2client_ShowGameMessage(wszbuf, 0); //white
		}
	}
}
void QuestNewLevel() {
	if (ACTNO==2) CheckUnitKM();
	//LOG("QuestProtect: enter area %d\n",dwCurrentLevel);
	if (20<=dwCurrentLevel&&dwCurrentLevel<=25) {
		checkQuest(5,1,0x8000,"Countess");
	} else if (35<=dwCurrentLevel&&dwCurrentLevel<=37) {
		checkQuest(6,1,0x8000,"Andariel");
	} else if (dwCurrentLevel==74) {
		checkQuest(13,1,0x8000,"Summoner");
	} else if (dwCurrentLevel==46||66<=dwCurrentLevel&&dwCurrentLevel<=72) {
		checkQuest(14,1,0x8000,"Duriel");
		if (!(GAMEQUESTDATA[0][14]&0x8000)&&(QUESTDATA[0][14]&1)) { //game not done, player done
			//use key to pass ACT2
			if (QUESTDATA[0][14]&0x18) { //player not reset
				d2client_ShowGameMessage(L"Quest not reset yet,talk to Jerhyn and Meshif first", 1); //red
			} else {
				d2client_ShowGameMessage(L"Quest is already reset", 2); //green
			}
		}
	} else if (dwCurrentLevel==101||dwCurrentLevel==102) {
		checkQuest(22,1,0x8000,"Mephisto");
	} else if (dwCurrentLevel==105||dwCurrentLevel==106) {
		checkQuest(25,1,0x8000,"Izual");
	} else if (dwCurrentLevel==107) {
		checkQuest(27,1,0x8000,"Hephasto");
	} else if (dwCurrentLevel==108) {
		checkQuest(26,1,0x8000,"Diablo");
		if (aBugInfo[3].fEnable&&*aBugInfo[3].fEnable&&aBugInfo[3].nStatus&&aBugInfo[3].nType==2) {
			char buf[32];char *name="BugKD";
			if (tBugAutoQuit.isOn) {
				formatKey(buf,tBugAutoQuit.key);
				gameMessage("Not %s game, will quit later, press %s to disable protection",name,buf);
			}
			if (DIFFICULTY==2&&tBugAutoQuitHell.isOn) {
				formatKey(buf,tBugAutoQuitHell.key);
				gameMessage("Not %s game, will quit later, press %s to disable protection",name,buf);
			}
			if (DIFFICULTY==2&&ACTNO==3&&tBugAutoQuitHellAct4.isOn) {
				formatKey(buf,tBugAutoQuitHellAct4.key);
				gameMessage("Not %s game, will quit later, press %s to disable protection",name,buf);
			}
		}
	} else if (122<=dwCurrentLevel&&dwCurrentLevel<=124) {
		checkQuest(38,3,0xa000,"Nihlathak"); //player:800e
	} else if (129<=dwCurrentLevel&&dwCurrentLevel<=132) {
		checkQuest(40,1,0x8000,"Baal"); 
		if (aBugInfo[4].fEnable&&*aBugInfo[4].fEnable&&aBugInfo[4].nStatus&&aBugInfo[4].nType==2) {
			char buf[32];char *name="BugKB";
			if (tBugAutoQuit.isOn) {
				formatKey(buf,tBugAutoQuit.key);
				gameMessage("Not %s game, will quit later, press %s to disable protection",name,buf);
			}
			if (DIFFICULTY==2&&tBugAutoQuitHell.isOn) {
				formatKey(buf,tBugAutoQuitHell.key);
				gameMessage("Not %s game, will quit later, press %s to disable protection",name,buf);
			}
			if (DIFFICULTY==2&&ACTNO==4&&tBugAutoQuitHellAct5.isOn) {
				formatKey(buf,tBugAutoQuitHellAct5.key);
				gameMessage("Not %s game, will quit later, press %s to disable protection",name,buf);
			}
		}
	}
}
void quest_FixValues() {
	if( dwBugAlertTimes>10 ) dwBugAlertTimes=10;
	aBugInfo[0].fEnable = (BYTE *)&tBugKCountess.isOn;;
	aBugInfo[0].nType = tBugKCountess.value32;
	aBugInfo[0].szMsg="BugKCountess";
	aBugInfo[0].nQuestNo = 5; 
	aBugInfo[1].fEnable = 0;
	aBugInfo[2].fEnable = (BYTE *)&tBugKM.isOn;
	aBugInfo[2].nType = tBugKM.value32;
	aBugInfo[2].nQuestNo = 22 ; 
	aBugInfo[2].szMsg="BugKM";
	aBugInfo[3].fEnable = (BYTE *)&tBugKD.isOn;
	aBugInfo[3].nType = tBugKD.value32;
	aBugInfo[3].nQuestNo = 26 ; 
	aBugInfo[3].szMsg="BugKD";
	aBugInfo[4].fEnable = (BYTE *)&tBugKB.isOn;
	aBugInfo[4].nType = tBugKB.value32;
	aBugInfo[4].nQuestNo = 40 ; 
	aBugInfo[4].szMsg="BugKB";
}
void QuestNewGame() {
	int acts = EXPANSION ? 5 : 4 ; //����Ƭ����A5
	for(int i=0 ; i < acts ; i++ ){
		if (!aBugInfo[i].fEnable) continue;
		BOOL fPlayer = d2common_CheckQuestFlag(QUESTDATA	,aBugInfo[i].nQuestNo	,	0);
		BOOL fGame = d2common_CheckQuestFlag(GAMEQUESTDATA	,aBugInfo[i].nQuestNo	,	15);
		aBugInfo[i].nStatus = 0 ;
		if (fGame==0) { //��ǰ��Ϸ�����
			if (fPlayer==0) {//��һ�û���ָ������
				aBugInfo[i].nStatus=1;
				if (DIFFICULTY==2&& aBugInfo[i].nType == 2 && *(aBugInfo[i].fEnable)){
					gameMessage("<Hackmap>: Not A %s Game,will check when active" , aBugInfo[i].szMsg);
				}
			} else {//��������ָ������
				if (DIFFICULTY==2&&tAlertNoBug.isOn){
					gameMessage("<Hackmap>: Not A %s Game" , aBugInfo[i].szMsg);
				}
			}
		}
	}
	ReSetTimer();
}
void QuestRunLoop() { 
	wchar_t wszTemp[512];
	if (aBugInfo[ACTNO].nStatus==0) return ;
	if (!aBugInfo[ACTNO].fEnable||!(*aBugInfo[ACTNO].fEnable)) return;
	if (aBugInfo[ACTNO].nStatus == 1 && aBugInfo[ACTNO].nType==2) return;
	if (!dwQuestAlertMs||dwCurMs<dwQuestAlertMs) return;
	if (alertCount<=0) {
		if (tBugAutoQuit.isOn
			||DIFFICULTY==2&&tBugAutoQuitHell.isOn
			||DIFFICULTY==2&&ACTNO==3&&tBugAutoQuitHellAct4.isOn
			||DIFFICULTY==2&&ACTNO==4&&tBugAutoQuitHellAct5.isOn) {
			if (tQuickNextGame.key!=0&&tQuickNextGame.key!=(BYTE)-1) {
				QuickNextGame(1);
			} else {
				QuickExitGame();
			}
		}
		dwQuestAlertMs=0;
		return;
	}
	if (tBugAutoQuit.isOn
		||DIFFICULTY==2&&tBugAutoQuitHell.isOn
		||DIFFICULTY==2&&ACTNO==4&&tBugAutoQuitHellAct5.isOn ){
		if (dwGameLng) wsprintfW(wszTemp, L"%hs��������, %d����˳�", aBugInfo[ACTNO].szMsg, alertCount);
		else wsprintfW(wszTemp, L"%hs Protect On, Will quit in %d secs", aBugInfo[ACTNO].szMsg, alertCount);
		SetBottomAlertMsg1(wszTemp,3000,1,1);
	} else {
		if (dwGameLng) wsprintfW(wszTemp, L"<Hackmap>: ����!!!!��%hs��Ϸ!!!!(%d)", aBugInfo[ACTNO].szMsg, alertCount);
		else wsprintfW(wszTemp, L"<Hackmap>: Warning!!!!Not A %hs Game!!!!(%d)" , aBugInfo[ACTNO].szMsg, alertCount);
		d2client_ShowGameMessage(wszTemp, 8);
		SetBottomAlertMsg1(wszTemp,1000,1,1);
	}
	dwQuestAlertMs=dwCurMs+1000;alertCount--;
}

void __fastcall BugKDProtect(DWORD param2 ) {
	wchar_t wszbuf[64];
	switch (param2) {
		case 0:d2client_ShowGameMessage(L"Dens of Evil cleared", 2);return;
		case 1:d2client_ShowGameMessage(L"Portal to Tristram is opened", 2);return;
		case 3:d2client_ShowGameMessage(L"Tal Rasha's Chamber is opened", 2);return;
		case 6:d2client_ShowGameMessage(L"Got the amulet of viper", 2);return;
		case 7:d2client_ShowGameMessage(L"Q25 Summoner is dead", 2);return;
		case 8:d2client_ShowGameMessage(L"Q26 Duriel is dead", 2);return;
		case 0x0B:d2client_ShowGameMessage(L"Mephisto is dead", 2);return;
		case 0x0C:
			//KD�����ķ�ӡ�� 89 0C
			if (ACTNO==3 && aBugInfo[3].nStatus==1 && aBugInfo[3].nType==2 ){
				aBugInfo[3].nStatus = 2;
				alertCount=dwBugAlertTimes;dwQuestAlertMs=dwCurMs;
			}
			setKDCountDown(1);
			d2client_ShowGameMessage(L"Q43: All seals are open", 2);
			return;
		case 0x0E:d2client_ShowGameMessage(L"Q42 Mephisto's stone is destoried", 2);return;
		case 0x0F:d2client_ShowGameMessage(L"Q51 Overseer is dead", 2);return;
		case 0x10:d2client_ShowGameMessage(L"Q53 Anya is saved", 2);return;
		case 0x11:d2client_ShowGameMessage(L"Q54 Nihlathak is dead", 2);return;
		case 0x12:d2client_ShowGameMessage(L"Q55 1BB is dead", 2);return;
		case 0x13:d2client_ShowGameMessage(L"Q56 Baal is dead", 2);return;
		default:
			wsprintfW(wszbuf, L"Recv quest packet 89: %x",param2);
			d2client_ShowGameMessage(wszbuf, 0);
	}
}

void __fastcall BugKBProtect(BYTE *aPacket) {
	wchar_t wszbuf[256];int pos=0;
	if (tPacketHandler.isOn) {
		pos+=wsprintfW(wszbuf+pos, L"Recv quest packet:");
		for (int i=0;i<6;i++) 
			pos+=wsprintfW(wszbuf+pos, L" %02X",aPacket[i]);
		d2client_ShowGameMessage(wszbuf, 0);
		if (logfp) {
			fprintf(logfp,"Recv quest packet 5D:");
			for (int i=0;i<6;i++) fprintf(logfp," %02X",aPacket[i]);
			fputc('\n',logfp);
			fflush(logfp);
		}
	}
	/*
Q11 
	5D 01 00 02 00 00 //Someone enter Dens of Evil
	5D 01 20 04 (05-01) 00 // only 5-1 monster left
	5D 01 00 0C 00 00 //done
Q12 
	5D 02 00 02 00 00 //Enter Burial Grounds 
	5D 02 00 03 00 00 //kill Blood Raven
Q13
	5D 04 00 02 00 00 //get sroll
	5D 04 00 03 00 00 //identify sroll
	5D 04 00 04 00 00 //open red portal
	5D 04 00 06 00 00 //saved cain
	5D 04 02 00 00 00 //get ring
Q14
	5D 05 00 03 00 00 //enter forgotten tower
	5D 05 00 02 00 00 //enter forgotten tower level 5
	5D 05 00 0d 00 00 //killed Countess
Q15
	5D 03 00 02 00 00 //got the tool
	5D 03 00 04 00 00 //can't complete
	5D 03 00 0a 00 00 //talk to Charsi
Q16
	5D 06 00 02 00 00 //someone enter Catacombs Level 4
	5D 06 00 03 00 00 //killed Andariel
Q21
	5D 08 00 02 00 00 //reached 
	5D 08 00 03 00 00 //killed Quest=0x2032
	5D 08 02 00 00 00 //used book of skill Quest=0x2012
	5D 08 00 0d 00 00 //talk to NPC Quest=0x3011
Q22
	5D 09 00 04 00 00 //got cube
	5D 09 00 02 00 00 //got staff of king
	5D 09 00 03 00 00 //got the amulet
Q23
	5D 0A 00 01 00 00 //quest player receive this
	5D 0A 10 00 00 00 //enter Claw Viper Temple Level 1 & Level 2
	5D 0A 00 03 00 00 //got the amulet
Q24
	5D 0B 00 01 00 00 //combine the staff
	5D 0B 00 02 00 00 //after talk to Drogman, palace is open
	5D 0B 00 03 00 00 //after talk to king
	5D 0B 00 04 00 00 //someone enter Arcane Sanctuary
Q25
	5D 0C 00 02 00 00 //get close to the summoner
	5D 0C 00 04 00 00 //summoner is dead Quest=2006
	5D 0C 00 0D 00 00 //talk to cain Quest=3005 game=2000
Q26
	5D 0D 00 0C 00 00 //can't complete
	5D 0D 00 05 00 00 //talk to Angel
	5D 0D 00 06 00 00 //talk to king
Q35
	5D 13 00 01 00 00 //
	5D 13 00 03 00 00 //enter Travincal
	5D 13 00 0C 00 00 //killed
Q36
	5D 14 00 04 00 00 //some enter Durance of Hate Level 3
Q41
	5D 16 00 02 00 00 //get close
	5D 16 00 03 00 00 //killed
	5D 16 02 00 00 00 //got skill points
Q43
	5D 17 00 01 00 00 //enter Chaos Sanctuary
	5D 17 00 0D 00 00 //killed Diablo
	5D 17 02 00 00 00 //killed Diablo
Q52
	5D 20 00 05 0F 00 //rescue 5
	5D 20 20 02 0A 00 //rescue 5
	5D 20 20 02 05 00 //rescue 10
	5D 20 00 05 05 00 //rescue 15
	5D 20 00 03 00 00 //rescue 15
	*/
	switch (aPacket[1]) {
		case 0x05:
			if (aPacket[2] == 0x00) {
				if (aPacket[3]==0x03||aPacket[3]==0x02) {
					if (aPacket[3]==0x03)
						d2client_ShowGameMessage(L"Someone enter forgotten tower", 2);
					else if (aPacket[3]==0x02)
						d2client_ShowGameMessage(L"Someone enter forgotten tower Level 5", 2);
					if ( ACTNO==0&&aBugInfo[0].nStatus==1&&*(aBugInfo[0].fEnable)&&aBugInfo[0].nType==2 ) {
							aBugInfo[0].nStatus = 2 ;
							alertCount=dwBugAlertTimes;dwQuestAlertMs=dwCurMs;
					}
				} else if (aPacket[3]==0x0D)
					d2client_ShowGameMessage(L"Countess is dead", 2);
			}
			break;
		case 0x06:
			if (aPacket[2] == 0x00) {
				if (aPacket[3]==0x03)
					d2client_ShowGameMessage(L"Andariel is dead", 2);
			}
			break;
		case 0x14:
			if (aPacket[2] == 0x00&&aPacket[3]==0x04) {
				d2client_ShowGameMessage(L"Someone enter Durance of Hate Level 3", 2);
				if ( ACTNO==2 && aBugInfo[2].nStatus == 1 && *(aBugInfo[2].fEnable) && aBugInfo[2].nType==2 ) {
						aBugInfo[2].nStatus = 2 ;
						alertCount=dwBugAlertTimes;dwQuestAlertMs=dwCurMs;
				}
			}
			break;
		case 0x17:
			if (aPacket[2] == 0x00&&aPacket[3]==0x01) {
				//QuestNewLevel();
			}
			break;
		case 0x20:
			if (aPacket[1]==0x20&&aPacket[2]==0x20) {
				if (aPacket[3]==0x02) {
					wsprintfW(wszbuf, L"Q52: %d barbrian to rescue",aPacket[4]);
					d2client_ShowPartyMessage(wszbuf, 2);
				} else if (aPacket[3]==0x03) {
					d2client_ShowPartyMessage(L"Q52: All barbrian rescued",2);
				}
			}
			break;
		case 0x24:
			//Baalת����ȥ��5D 24 00 03 00 00 �����������˲����յ�
			if (aPacket[2] == 0x00 && aPacket[3] == 0x03  ) {
				setKBCountDown(0,0);
				QuestNewLevel();
				if ( ACTNO==4 && aBugInfo[4].nStatus==1 && aBugInfo[4].nType==2 ) {
					aBugInfo[4].nStatus = 2;
					alertCount=dwBugAlertTimes;dwQuestAlertMs=dwCurMs;
				}
			}
			break;
	}
}

void __declspec(naked) RecvCommand_89_Patch_ASM() {
  __asm
  {
	// ecx = aPacket[1] 
	push ecx;
    call BugKDProtect;
    pop ecx;
    
	cmp ecx, 0x20;
	jl orgjl
	ret
orgjl:
	add dword ptr[esp] ,7;
    ret
  }
}


void __declspec(naked) RecvCommand_5D_Patch_ASM() {
  __asm {
    push ecx;
    call BugKBProtect;
    pop ecx;
	
	pop edx;
	sub esp, 8;
	mov eax, dword ptr [ecx];
	push edx;
    ret
  }
}

void CheckUnitKM(){
	if ( ACTNO==2 && aBugInfo[2].nStatus == 1 && *(aBugInfo[2].fEnable) && aBugInfo[2].nType==2 ){
		if ( dwCurrentLevel == 102 ) {
			aBugInfo[2].nStatus = 2 ;
			alertCount=dwBugAlertTimes;dwQuestAlertMs=dwCurMs;
			return ;
		}
		for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext ) {
			if ( pUnit->dwLevelNo== 102 && TestPvpFlag( pUnit->dwUnitId , dwPlayerId )>=2) {
				aBugInfo[2].nStatus = 2 ;
				alertCount=dwBugAlertTimes;dwQuestAlertMs=dwCurMs;
				return ;
			}
		}
	}
}

extern char *questNames[41];
static void drawQuestState(unsigned short *player,unsigned short *game,int id,int row,int col) {
	int w,h;
	static int xs[3]={143,237,334};
	static int ys[2]={150,244};
	wchar_t wszbuf[64];
	int x=xs[col]-20;
	int y=ys[row];
	x+=(SCREENSIZE.x-800)>>1;
	y+=(SCREENSIZE.y-600)>>1;
	y-=5;
	wsprintfW(wszbuf, L"G %X",game[id]);
	d2win_GetTextAreaSize(wszbuf, &w, &h);d2gfx_DrawRectangle(x,y-12,x+w,y,0x10,5);
	d2win_DrawText(wszbuf, x, y, 0, 0);
	y+=15;
	wsprintfW(wszbuf, L"P %X",player[id]);
	d2win_GetTextAreaSize(wszbuf, &w, &h);d2gfx_DrawRectangle(x,y-12,x+w,y,0x10,5);
	d2win_DrawText(wszbuf, x, y, 0, 0);
	//wsprintfW(wszbuf, L"%hs",questNames[id]);
	//d2win_DrawText(wszbuf, x, y+20, 2, 0);
}
void __fastcall DrawQuestPagePatch(int ypos, int xpos) {
	//380,106 drawoffset  80,
	unsigned short *player=*QUESTDATA;
	unsigned short *game=*GAMEQUESTDATA;
	DWORD dwOldFone = d2win_SetTextFont(8);
	switch (QUESTPAGE) {
	case 0:
		drawQuestState(player,game,1,0,0);
		drawQuestState(player,game,2,0,1);
		drawQuestState(player,game,4,0,2);
		drawQuestState(player,game,5,1,0);
		drawQuestState(player,game,3,1,1);
		drawQuestState(player,game,6,1,2);
		break;
	case 1:
		drawQuestState(player,game,9,0,0);
		drawQuestState(player,game,10,0,1);
		drawQuestState(player,game,11,0,2);
		drawQuestState(player,game,12,1,0);
		drawQuestState(player,game,13,1,1);
		drawQuestState(player,game,14,1,2); 
		//102D->1035->1025
		break;
	case 2:
		drawQuestState(player,game,20,0,0);
		drawQuestState(player,game,19,0,1);
		drawQuestState(player,game,18,0,2);
		drawQuestState(player,game,17,1,0);
		drawQuestState(player,game,21,1,1);
		drawQuestState(player,game,22,1,2);
		break;
	case 3:
		drawQuestState(player,game,25,0,0);
		drawQuestState(player,game,27,0,1);
		drawQuestState(player,game,26,0,2);
		break;
	case 4:
		drawQuestState(player,game,35,0,0);
		drawQuestState(player,game,36,0,1);
		drawQuestState(player,game,37,0,2);
		drawQuestState(player,game,38,1,0);
		drawQuestState(player,game,39,1,1);
		drawQuestState(player,game,40,1,2);
		break;
	}
	d2win_SetTextFont(dwOldFone);

	if ( QUESTPAGE ==0 ) {//ţ��
		int cow = d2common_CheckQuestFlag(QUESTDATA, 4, 10);
		if (cow) DrawCenterText(8, L"X", 300+DRAWOFFSET.x, 50-DRAWOFFSET.y, 1);
	}else if ( QUESTPAGE ==1 ) {//act2 bug
		int dru = d2common_CheckQuestFlag(QUESTDATA, 14, 5);
		if (dru) DrawCenterText(8, L"X", 300+DRAWOFFSET.x, 50-DRAWOFFSET.y, 1);
	}
}

void __declspec(naked) DrawQuestPagePatch_ASM()
{
	__asm {
		push eax
		call DrawQuestPagePatch
		pop eax
//original code
		cmp esi,0x116
		ret
	}
}

void __fastcall PacketA4(char *buf) {
	short id=*(short *)(buf+1);
	switch (id) {
		case 62:break;
		case 23: //62(3e)->23(17) Baal hall cleared
			setKBCountDown(1,1);
			//d2client_ShowGameMessage(L"hall clear", 0); 
			break;
		case 105:break;
		case 381: //105(69)->381(17d) team 1 cleared
			setKBCountDown(2,1);
			//d2client_ShowGameMessage(L"team 1 clear", 0); 
			break;
		case 557:
			setKBCountDown(3,1);
			//d2client_ShowGameMessage(L"team 2 clear", 0); 
			break;
		case 558:
			setKBCountDown(4,1);
			//d2client_ShowGameMessage(L"team 3 clear", 0); 
			break;
		case 571: {
			setKBCountDown(5,1);
			unsigned short *game=GAMEQUESTDATA[0];
			int fGame = game[40]&0x8000;
			if (!fGame&&DIFFICULTY==2) {
				QuestNewLevel();
				if (wszBugKBAlertMessage[0]) SendChatMessageW(wszBugKBAlertMessage);
			}
			break;
		}
		default:
			wchar_t wszbuf[256];
			wsprintfW(wszbuf, L"packet A4: %d",id);
			d2client_ShowGameMessage(wszbuf, 0); 
	}
}
/*
skill 11d: laugh
skill 11e: Baal Monster Spawn
[14.734] RECV cmd_4d len=17 UnitCastSkill
	00: 4d 01 b0 00 - 00 00 1e 01 - 00 00 01 f2 - 3a a0 13 00 |M           :   
	10: 00          -             -             -             |                
*/
void __fastcall Packet4D(char *buf) {
	int skill=*(short *)(buf+6);
	switch (skill) {
		case 0x11d: //laugh
			break;
		case 0x11e: //baal monster spawn
			setKBCountDown(0,0);
			break;
	}
}
/*
A4 packet
6FB5D766 - 0FB7 59 01            - movzx ebx,word ptr [ecx+01]
6FB5D76A - 8B 08                 - mov ecx,[eax]
*/
void __declspec(naked) RecvCommand_A4_Patch_ASM() {
	__asm {
		pushad
		call PacketA4
		popad
//original code
		movzx ebx, word ptr [ecx+1]
		mov ecx, [eax]
		ret
	}
}
/*
4D packet
6FB5F1A9 - B9 15000000           - mov ecx,00000015 { 21 }
*/
void __declspec(naked) RecvCommand_4D_Patch_ASM() {
	__asm {
		pushad
		mov ecx, esi
		call Packet4D
		popad
//original code
		mov ecx, 0x15
		ret
	}
}