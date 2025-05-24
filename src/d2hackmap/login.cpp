#include "stdafx.h"
#include "header.h"

struct LoginInfo {int key;char *name,*pass;struct LoginInfo *next;};
static LoginInfo *accountInfos=NULL;
static LoginInfo *lastAccount=NULL;
int inputUserPassword(LoginInfo *pai);
int dwAutoLoginMs=0;
static char lastAccountName[32],lastCharacterName[32];
static int autoSelectedCharacterIndex=0;
int dwAutoSelectCharMs=0;

struct CharTag {char *name,*tag;};
static CharTag *charTags=NULL;
static CharTag curCharTags[8];
static int nCharTag=0,capCharTag=0;
int nCurCharTags=0;
static int compareCharTag(const void *a,const void *b) {return strcmp(((CharTag *)a)->name,((CharTag *)b)->name);}
void clearCharacterTag() {
	charTags=NULL;nCharTag=0;capCharTag=0;
}
void addCharacterTag(char *name,char *tag) {
	if (!charTags) {nCharTag=0;capCharTag=32;charTags=(CharTag *)HeapAlloc(confHeap,0,sizeof(CharTag)*capCharTag);}
	for (int i=0;i<nCharTag;i++) {
		CharTag *p=&charTags[i];
		if (strcmp(p->name,name)==0) {p->tag=tag;return;}
	}
	if (nCharTag>=capCharTag) {
		capCharTag*=2;charTags=(CharTag *)HeapReAlloc(dllHeap,0,charTags,sizeof(CharTag)*capCharTag);
	}
	CharTag *p=&charTags[nCharTag++];
	p->name=name;p->tag=tag;
}
char *getCharTag(char *name) {
	if (!name) return NULL;
	CharTag ct;ct.name=name;
	CharTag *pct=(CharTag *)bsearch(&ct,charTags,nCharTag,sizeof(CharTag),compareCharTag);
	if (pct) return pct->tag;
	return NULL;
}
void doneCharacterTag() {qsort(charTags,nCharTag,sizeof(CharTag),compareCharTag);}
void drawCharTags() {
	wchar_t wszbuf[256];
	D2Character *pchar=*d2launch_pD2Characters;
	if (!pchar) {nCurCharTags=0;return;}
	DWORD dwOldFone = d2win_SetTextFont(1);
	for (int i=0;i<nCurCharTags;i++) {
		int x=(i%2)==1?390:115;
		int y=175+90*(i/2);
		char *tag=curCharTags[i].tag;if (!tag) continue;
		wsprintfW(wszbuf, L"%hs",tag);
		d2win_DrawText(wszbuf,x,y,0,-1);
	}
	d2win_SetTextFont(dwOldFone);
}

void login_initConfigVars() {
	accountInfos=NULL;
}
void addAccount(int key,char *name,char *pass) {
	for (int i=0;i<100;i++) {
		int checkagain=0;
		for (LoginInfo *p=accountInfos;p;p=p->next) {
			if (p->key==key) {key++;checkagain=1;}
		}
		if (!checkagain) break;
	}
	LoginInfo *pai=(LoginInfo *)HeapAlloc(confHeap,0,sizeof(LoginInfo));
	pai->key=key;pai->name=heap_strdup(confHeap,name);pai->pass=heap_strdup(confHeap,pass);
	pai->next=accountInfos;accountInfos=pai;
	ToggleVar *ptv=addExtraKey(key);if (!ptv) {LOG("ERROR: too many keys\n");return;}
	ptv->type=TOGGLEVAR_DOWNPTR;ptv->key=key;
	ptv->paramPtr=pai;ptv->keyType=1;
	ptv->func=inputUserPassword;ptv->funcUp=NULL;
	ptv->desc="Account";
}
D2EditBox *isLoginScreen() {
	if (fInGame) return NULL;
	D2EditBox *focus=(*d2win_pFocusedControl);
	if (!focus) return NULL;
	D2EditBox *next=focus->pNext2;
	if (!next) return NULL;
	if (next->pNext2!=focus) return NULL;
	D2EditBox *nameBox,*passBox;
	if (focus->dwPosY<next->dwPosY) {nameBox=focus;passBox=next;}
	else {nameBox=next;passBox=focus;}
	if (nameBox->dwPosY!=342) return NULL;
	if (passBox->dwPosY!=396) return NULL;
	return nameBox;
}
static void c2w(char *p,wchar_t *w) {
	while (1) {*w=(*p)&0xFF;if (!*p) break;w++;p++;}
}
char *getLastUser() {
	static char name[20];
	D2EditBox *nameBox=isLoginScreen();
	if (!nameBox) return NULL;
	wchar_t *ws=d2win_GetEditBoxText(nameBox);
	for (int i=0;i<16;i++) {
		name[i]=(char)ws[i];if (!name[i]) break;
	}
	name[16]=0;
	return name;
}
int inputUserPassword(LoginInfo *pai) {
	char *name=pai->name;char *password=pai->pass;
	if (!name) return -1;if (!password) password="";
	wchar_t wb[64]={0};
	D2EditBox *focus=(*d2win_pFocusedControl);
	if (!focus) return -1;
	D2EditBox *next=focus->pNext2;
	if (!next) return -1;
	if (next->pNext2!=focus) return -1;
	D2EditBox *nameBox,*passBox;
	if (focus->dwPosY<next->dwPosY) {nameBox=focus;passBox=next;}
	else {nameBox=next;passBox=focus;}
	if (nameBox->dwPosY!=342) return -1;
	if (passBox->dwPosY!=396) return -1;
	c2w(password,wb);d2win_SetEditBoxText(passBox,wb);
	c2w(name,wb);d2win_SetEditBoxText(nameBox,wb);
	lastAccount=pai;
	return 0;
}
void autoLogin() {
	if (!lastAccount) {
		char *user=getLastUser();if (!user) return;
		for (LoginInfo *p=accountInfos;p;p=p->next) {
			if (strcmp(p->name,user)==0) {inputUserPassword(p);return;}
		}
		return;
	}
	inputUserPassword(lastAccount);
}
/*
6FA59524 - 68 00040000           - push 00000400 { 1024 }
6FA59529 - 8D 54 24 10           - lea edx,[esp+10]
6FA5952D - 52                    - push edx //buf
6FA5952E - 53                    - push ebx
6FA5952F - 68 B4E0A56F           - push 6FA5E0B4 { ("Last BNet") }
6FA59534 - 68 04E2A56F           - push 6FA5E204 { ("Diablo II") }
6FA59539 - E8 1602FFFF           - call 6FA49754 { ->->6FC2C2B0 storm_regQueryValue } <---------------
6FA5953E - 85 C0                 - test eax,eax
*/
int __stdcall LastBNet_Patch(char *keyName,char *valueName,int arg,char *buf,int len) {
	if (lastAccount) dwAutoLoginMs=dwCurMs+300;
	int ret=storm_regQueryValue(keyName,valueName,arg,buf,len);
	return ret;
}
static void __fastcall selectChar(int idx) {
	*d2launch_pSelectedCharIndex=idx;
	D2Character *pchar=*d2launch_pD2Characters;
	if (!pchar) return;
	char *account=d2mcpclient_pAccountInfo->accountName;
	for (int i=0;pchar;i++,pchar=pchar->next) {
		if (IsBadReadPtr(pchar,sizeof(D2Character))) return;
		if (idx==i) {
			strcpy(lastAccountName,account);
			strcpy(lastCharacterName,pchar->name);
			return;
		}
	}
}
// 6FA4E1AF - 85 C9                 - test ecx,ecx
// 6FA4E1B1 - A3 B04DA66F           - mov [6FA64DB0],eax { (2) }
void __declspec(naked) SelectCharPatch_ASM() {
	__asm {
		pushad
		mov ebx,d2launch_pSelectedCharIndex
		mov dword ptr [ebx],eax
		mov ecx,eax
		call selectChar
		popad
		test ecx,ecx
		ret 
	}
}
extern int mouseL;
void autoSelectChar() {
	dwAutoSelectCharMs=0;
	D2Character *pchar=*d2launch_pD2Characters;
	if (!pchar) return;
	LOG("auto select %d\n",autoSelectedCharacterIndex);
	int x=(autoSelectedCharacterIndex%2)==1?400:160;
	int y=128+90*(autoSelectedCharacterIndex/2);
	HWND hwnd=d2gfx_GetHwnd();
	int lParam=MAKELONG(x,y);
	PostMessage(hwnd, WM_MOUSEMOVE, 0, lParam);
	PostMessage(hwnd, WM_RBUTTONDOWN, 0, lParam);
	PostMessage(hwnd, WM_RBUTTONUP, 0, lParam);
	PostMessage(hwnd, WM_MOUSEMOVE, 0, mouseL);
}
void unicode2win(char *dst,wchar_t *src,int size);
void getSnapshotPath(char *buf,char *realm,char *account,char *name,char *subfolder,char *ext,char sep);
extern ToggleVar tSnapshot;
void saveCharMenu() {
	char path[512],realm[256];
	if (!tSnapshot.isOn) return;
	D2Character *pchar=*d2launch_pD2Characters;
	if (!pchar) return;
	char *account=d2mcpclient_pAccountInfo->accountName;
	FILE *fp=NULL;
	memset(curCharTags,0,sizeof(curCharTags));nCurCharTags=0;
	autoSelectedCharacterIndex=-1;
	for (int i=0;pchar;i++) {
		if (IsBadReadPtr(pchar,sizeof(D2Character))) return;
		if (!fp) {
			unicode2win(realm,pchar->realm,255);
			getSnapshotPath(path,realm,account,NULL,"account","txt",'_');
			fp=fopen(path,"w+");
		}
		fprintf(fp,"%s\n",pchar->name);
		if (strcmp(account,lastAccountName)==0&&strcmp(pchar->name,lastCharacterName)==0) {
			LOG("auto select %s %s\n",account,pchar->name);
			autoSelectedCharacterIndex=i;
		}
		if (nCharTag&&i<8) {
			CharTag ct;ct.name=pchar->name;
			CharTag *pct=(CharTag *)bsearch(&ct,charTags,nCharTag,sizeof(CharTag),compareCharTag);
			if (pct) {curCharTags[i]=*pct;nCurCharTags=i+1;}
		}
		pchar=pchar->next;
	}
	LOG("nCurCharTags=%d\n",nCurCharTags);
	if (autoSelectedCharacterIndex>=0) dwAutoSelectCharMs=dwCurMs+300;
	if (fp) fclose(fp);
}
/*
6FA4CBF6 - 0FB7 57 01            - movzx edx,word ptr [edi+01]
6FA4CBFA - 3B EA                 - cmp ebp,edx
*/
void __declspec(naked) OpenCharMenuPatch_ASM() {
	__asm {
		pushad
		call saveCharMenu
		popad
		movzx edx,word ptr [edi+1]
		cmp ebp,edx
		ret 
	}
}
