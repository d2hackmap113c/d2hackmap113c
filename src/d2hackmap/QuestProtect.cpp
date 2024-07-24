#include "stdafx.h"

#ifdef MODULE_QUESTPROTECT

void SendChatMessageW(wchar_t *myMsg);
void setKBCountDown(int team,int en);
static BugQuestInfo aBugInfo[5] ;
static DWORD BugTimer = 0 ;
static BYTE BugTimerSet[10] = { 0 ,};
static DWORD dwLastAct = 0 ; 

int ReSetTimer(){
	BugTimer = GetTickCount();
	memset(BugTimerSet,	0,	10*sizeof(BYTE));
	SetCenterAlertMsg(FALSE , L"\0");
	return 0;
}
static void checkQuest(int id,int bitmask1,int bitmask2,char *name) {
	wchar_t wszbuf[256];
	unsigned short *player=(unsigned short *)(*(int **)QUESTDATA);
	unsigned short *game=(unsigned short *)(*(int **)GAMEQUESTDATA);
	int fPlayer = player[id]&bitmask1;//D2CheckQuestFlag(QUESTDATA	,id,bit1);
	int fGame = game[id]&bitmask2;//D2CheckQuestFlag(GAMEQUESTDATA	,id,bit2);
	if ( fPlayer== 0 ) {//玩家还没完成指定任务
		if ( fGame==0 ){ //当前游戏可完成
			wsprintfW(wszbuf, L"Warning: Not a BugK%hs Game game=%04X player=%04X",name,game[id],player[id]);
			D2ShowGameMessage(wszbuf, 1); //red
		} else {
			wsprintfW(wszbuf, L"BugK%hs game=%04X player=%04X",name,game[id],player[id]);
			D2ShowGameMessage(wszbuf, 2); //green
		}
	} else {
		if ( fGame==0 ){ //当前游戏可完成
			wsprintfW(wszbuf, L"K%hs game=%04X player=%04X",name,game[id],player[id]);
			D2ShowGameMessage(wszbuf, 2); //green
		} else {
			wsprintfW(wszbuf, L"Already completed %hs game=%04X player=%04X",name,game[id],player[id]);
			D2ShowGameMessage(wszbuf, 0); //white
		}
	}
}
void checkAllBugQuests() {
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
	} else if (dwCurrentLevel==101||dwCurrentLevel==102) {
		checkQuest(22,1,0x8000,"Mephisto");
	} else if (dwCurrentLevel==105||dwCurrentLevel==106) {
		checkQuest(25,1,0x8000,"Izual");
	} else if (dwCurrentLevel==107) {
		checkQuest(27,1,0x8000,"Hephasto");
	} else if (dwCurrentLevel==108) {
		checkQuest(26,1,0x8000,"Diablo");
	} else if (122<=dwCurrentLevel&&dwCurrentLevel<=124) {
		checkQuest(38,3,0xa000,"Nihlathak"); //player:800e
	} else if (129<=dwCurrentLevel&&dwCurrentLevel<=132) {
		checkQuest(40,1,0x8000,"Baal"); 
	}
}

void InitBugInfo(){
	if( dwBugAlertTimes>10 ) dwBugAlertTimes = 10 ;
	aBugInfo[0].fEnable = (BYTE *)&tBugKCountess.isOn;;
	aBugInfo[0].nType = tBugKCountess.value32;
	strcpy(aBugInfo[0].szMsg ,"BugKCountess") ;
	aBugInfo[0].nQuestNo = 5; 
	aBugInfo[1].fEnable = 0;
	aBugInfo[2].fEnable = (BYTE *)&tBugKM.isOn;
	aBugInfo[2].nType = tBugKM.value32;
	aBugInfo[2].nQuestNo = 22 ; 
	strcpy(aBugInfo[2].szMsg ,"BugKM") ;
	aBugInfo[3].fEnable = (BYTE *)&tBugKD.isOn;
	aBugInfo[3].nType = tBugKD.value32;
	aBugInfo[3].nQuestNo = 26 ; 
	strcpy(aBugInfo[3].szMsg ,"BugKD") ;
	aBugInfo[4].fEnable = (BYTE *)&tBugKB.isOn;
	aBugInfo[4].nType = tBugKB.value32;
	aBugInfo[4].nQuestNo = 40 ; 
	strcpy(aBugInfo[4].szMsg ,"BugKB") ;
}


void SetBugQuestStatus() {
	wchar_t wszTemp[512];
	int acts = EXPANSION ? 5 : 4 ; //资料片才有A5
	for(int i=0 ; i < acts ; i++ ){
		if ( aBugInfo[i].fEnable ){
			BOOL fPlayer = D2CheckQuestFlag(QUESTDATA	,aBugInfo[i].nQuestNo	,	0);
			BOOL fGame = D2CheckQuestFlag(GAMEQUESTDATA	,aBugInfo[i].nQuestNo	,	15);
			aBugInfo[i].nStatus = 0 ;
			if ( fGame==0 ){ //当前游戏可完成
				if ( fPlayer== 0 ) {//玩家还没完成指定任务
					aBugInfo[i].nStatus = 1 ;
					if ( aBugInfo[i].nType == 2 && *(aBugInfo[i].fEnable)){
						wsprintfW2(wszTemp, "<Hackmap>: Not A %s Game,will check when active" , aBugInfo[i].szMsg);
						D2ShowGameMessage(wszTemp, 8);
					}
				} else {//玩家已完成指定任务
					if ( tAlertNoBug.isOn ){
						wsprintfW2(wszTemp, "<Hackmap>: Not A %s Game" , aBugInfo[i].szMsg);
						D2ShowGameMessage(wszTemp, 8);
					}
				}
			}
		}
	}
	ReSetTimer();
	dwLastAct = ACT->dwActNo ;
}

void ShowBugInfo() { 
	if ( dwLastAct != ACTNO ) {
		dwLastAct = ACTNO;
		ReSetTimer();
		return;
	}
	if ( aBugInfo[ACTNO].nStatus==0 ) return ;
	if( aBugInfo[ACTNO].fEnable && *aBugInfo[ACTNO].fEnable ){
		if ( aBugInfo[ACTNO].nStatus == 1 && aBugInfo[ACTNO].nType==2 ) return;
		DWORD mysecs = (dwCurMs - BugTimer)/1000;
		if (mysecs >= dwBugAlertTimes) {
			// Force quit game
			if ( tBugAutoQuit.isOn
				||DIFFICULTY==2&&tBugAutoQuitHell.isOn
				||DIFFICULTY==2&&ACTNO==4&&tBugAutoQuitHellAct5.isOn ){
				#ifdef MODULE_QUICKGAME
					if ( tQuickNextGame.key !=0 && tQuickNextGame.key != (BYTE)-1 ){
						QuickNextGame(1);
					}else{
						QuickExitGame();
					}
				#else
					ExitGame();
				#endif
			}
		}else{
			if (BugTimerSet[mysecs] == 0) {
				wchar_t wszTemp[512];
				BugTimerSet[mysecs] = 1;
				if (tBugAutoQuit.isOn
					||DIFFICULTY==2&&tBugAutoQuitHell.isOn
					||DIFFICULTY==2&&ACTNO==4&&tBugAutoQuitHellAct5.isOn ){
					wsprintfW2(wszTemp, "%s Protect On, Will quit in %d secs", aBugInfo[ACTNO].szMsg, dwBugAlertTimes-mysecs);
					SetCenterAlertMsgParam(1,2,250);
					SetCenterAlertMsg(TRUE , wszTemp);
					return;
				} else {
					wsprintfW2(wszTemp, "<Hackmap>: Warning!!!!Not A %s Room!!!!(%d)" , aBugInfo[ACTNO].szMsg, dwBugAlertTimes-mysecs);
					D2ShowGameMessage(wszTemp, 8);
					if (mysecs >= dwBugAlertTimes-1) {
						SetCenterAlertMsg(0, wszTemp);
					} else {
						SetCenterAlertMsgParam(1,2,250);
						SetCenterAlertMsg(1, wszTemp);
					}
				}
			}
		}
	}
}

void __fastcall BugKDProtect(DWORD param2 ) {
	wchar_t wszbuf[64];
	switch (param2) {
		case 0:D2ShowGameMessage(L"Dens of Evil cleared", 2);return;
		case 1:D2ShowGameMessage(L"Portal to Tristram is opened", 2);return;
		case 3:D2ShowGameMessage(L"Tal Rasha's Chamber is opened", 2);return;
		case 6:D2ShowGameMessage(L"Got the amulet of viper", 2);return;
		case 7:D2ShowGameMessage(L"Q25 Summoner is dead", 2);return;
		case 8:D2ShowGameMessage(L"Q26 Duriel is dead", 2);return;
		case 0x0B:D2ShowGameMessage(L"Mephisto is dead", 2);return;
		case 0x0C:
			//KD打开最后的封印： 89 0C
			if (ACTNO==3 && aBugInfo[3].nStatus==1 && aBugInfo[3].nType==2 ){
				aBugInfo[3].nStatus = 2;
				ReSetTimer();
			}
			D2ShowGameMessage(L"Q43: All seals are open", 2);
			return;
		case 0x0E:D2ShowGameMessage(L"Q42 Mephisto's stone is destoried", 2);return;
		case 0x0F:D2ShowGameMessage(L"Q51 Overseer is dead", 2);return;
		case 0x10:D2ShowGameMessage(L"Q53 Anya is saved", 2);return;
		case 0x11:D2ShowGameMessage(L"Q54 Nihlathak is dead", 2);return;
		case 0x12:D2ShowGameMessage(L"Q55 1BB is dead", 2);return;
		case 0x13:D2ShowGameMessage(L"Q56 Baal is dead", 2);return;
		default:
			wsprintfW(wszbuf, L"Recv quest packet 89: %x",param2);
			D2ShowGameMessage(wszbuf, 0);
	}
}

void __fastcall BugKBProtect(BYTE *aPacket) {
	/*wchar_t wszbuf[256];int pos=0;
	pos+=wsprintfW(wszbuf+pos, L"Recv quest packet:");
	for (int i=0;i<6;i++) 
		pos+=wsprintfW(wszbuf+pos, L" %02x",aPacket[i]);
	D2ShowGameMessage(wszbuf, 0);
	*/
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
	if (logfp) {
		fprintf(logfp,"Recv quest packet 5D:");
		for (int i=0;i<6;i++) fprintf(logfp," %02x",aPacket[i]);
		fputc('\n',logfp);
		fflush(logfp);
	}
	*/
	switch (aPacket[1]) {
		case 0x05:
			if (aPacket[2] == 0x00) {
				if (aPacket[3]==0x03||aPacket[3]==0x02) {
					if (aPacket[3]==0x03)
						D2ShowGameMessage(L"Someone enter forgotten tower", 2);
					else if (aPacket[3]==0x02)
						D2ShowGameMessage(L"Someone enter forgotten tower Level 5", 2);
					if ( ACTNO==0&&aBugInfo[0].nStatus==1&&*(aBugInfo[0].fEnable)&&aBugInfo[0].nType==2 ) {
							aBugInfo[0].nStatus = 2 ;
							ReSetTimer();
					}
				} else if (aPacket[3]==0x0D)
					D2ShowGameMessage(L"Countess is dead", 2);
			}
			break;
		case 0x06:
			if (aPacket[2] == 0x00) {
				if (aPacket[3]==0x03)
					D2ShowGameMessage(L"Andariel is dead", 2);
			}
			break;
		case 0x14:
			if (aPacket[2] == 0x00) {
				if (aPacket[3]==0x04) {
					D2ShowGameMessage(L"Someone enter Durance of Hate Level 3", 2);
					if ( ACTNO==2 && aBugInfo[2].nStatus == 1 && *(aBugInfo[2].fEnable) && aBugInfo[2].nType==2 ) {
							aBugInfo[2].nStatus = 2 ;
							ReSetTimer();
					}
				}
			}
			break;
		case 0x24:
			//Baal转身下去：5D 24 00 03 00 00 已完成任务的人不会收到
			if (aPacket[2] == 0x00 && aPacket[3] == 0x03  ) {
				setKBCountDown(0,0);
				checkAllBugQuests();
				if ( ACTNO==4 && aBugInfo[4].nStatus==1 && aBugInfo[4].nType==2 ) {
				 aBugInfo[4].nStatus = 2;
				 ReSetTimer();
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
			ReSetTimer();
			return ;
		}
		for (RosterUnit *pUnit = PLAYERLIST; pUnit; pUnit = pUnit->pNext ) {
			if ( pUnit->dwLevelNo== 102 && TestPvpFlag( pUnit->dwUnitId , dwPlayerId )>=2) {
				aBugInfo[2].nStatus = 2 ;
				ReSetTimer();
				return ;
			}
		}
	}
}

extern char *questNames[41];
static void drawQuestState(unsigned short *player,unsigned short *game,int id,int row,int col) {
	static int xs[3]={143,237,334};
	static int ys[2]={150,244};
	wchar_t wszbuf[64];
	int x=xs[col]-20;
	int y=ys[row];
	wsprintfW(wszbuf, L"G %X",game[id]);
	D2DrawText(wszbuf, x, y-5, 0, 0);
	D2DrawText(wszbuf, x+1, y-5+1, 6, 0);
	wsprintfW(wszbuf, L"P %X",player[id]);
	D2DrawText(wszbuf, x, y+10, 0, 0);
	D2DrawText(wszbuf, x+1, y+10+1, 6, 0);
	//wsprintfW(wszbuf, L"%hs",questNames[id]);
	//D2DrawText(wszbuf, x, y+20, 2, 0);
}
void __fastcall DrawQuestPagePatch(int ypos, int xpos) {
	//380,106 drawoffset  80,
	unsigned short *player=(unsigned short *)(*(int **)QUESTDATA);
	unsigned short *game=(unsigned short *)(*(int **)GAMEQUESTDATA);
	DWORD dwOldFone = D2SetTextFont(8);
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
	D2SetTextFont(dwOldFone);

	if ( QUESTPAGE ==0 ) {//牛关
		int cow = D2CheckQuestFlag(QUESTDATA, 4, 10);
		if (cow) DrawCenterText(8, L"X", 300+DRAWOFFSET.x, 50-DRAWOFFSET.y, 1);
	}else if ( QUESTPAGE ==1 ) {//act2 bug
		int dru = D2CheckQuestFlag(QUESTDATA, 14, 5);
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
			//D2ShowGameMessage(L"hall clear", 0); 
			break;
		case 105:break;
		case 381: //105(69)->381(17d) team 1 cleared
			setKBCountDown(2,1);
			//D2ShowGameMessage(L"team 1 clear", 0); 
			break;
		case 557:
			setKBCountDown(3,1);
			//D2ShowGameMessage(L"team 2 clear", 0); 
			break;
		case 558:
			setKBCountDown(4,1);
			//D2ShowGameMessage(L"team 3 clear", 0); 
			break;
		case 571: {
			setKBCountDown(5,1);
			unsigned short *game=(unsigned short *)(*(int **)GAMEQUESTDATA);
			int fGame = game[40]&0x8000;
			if (!fGame&&DIFFICULTY==2) {
				D2ShowGameMessage(L"Not bugKB game", 0); 
				if (wszBugKBAlertMessage[0]) SendChatMessageW(wszBugKBAlertMessage);
			}
			break;
		}
		default:
			wchar_t wszbuf[256];
			wsprintfW(wszbuf, L"packet A4: %d",id);
			D2ShowGameMessage(wszbuf, 0); 
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



#endif
