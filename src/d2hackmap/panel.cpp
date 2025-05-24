#include "stdafx.h"
#include "header.h"

int installBigstashPatches();
void modifyStashGrid();
static int invCharTopDy=0,invCharBottomDy=0;
static char *stashImg="hackmap\\stash10x10";
static char *cube8x10Img="hackmap\\cube8x10";
static char *cube10x10Img="hackmap\\cube10x10";
static char *invCharImg="hackmap\\InvChar8x10";
static char *invCharHalfImg="hackmap\\InvChar8x10half";
int fEnableBigStash=0;
int dwInvType=0,clientInvType,serverInvType; //0:autoDetect 1:4x10 2:8x10
int dwStashType=0,clientStashType,serverStashType; //0:autoDetect 1:8x6|4x6 2:10x10
int dwCubeType=0,clientCubeType,serverCubeType; //0:autoDetect 1:4x3 2:8x10 3:10x10
int fEnableHighResolutionPatch;
static int dwBatchAddSkillPoints=5;
static ConfigVar aConfigVars[] = {
	{CONFIG_VAR_TYPE_INT,"EnableBigStash",  &fEnableBigStash,4},
	{CONFIG_VAR_TYPE_INT,"ClientInvType",  &clientInvType,4},
	{CONFIG_VAR_TYPE_INT,"ClientStashType",  &clientStashType,4},
	{CONFIG_VAR_TYPE_INT,"ClientCubeType",  &clientCubeType,4},
	{CONFIG_VAR_TYPE_INT,"ServerInvType",  &serverInvType,4},
	{CONFIG_VAR_TYPE_INT,"ServerStashType",  &serverStashType,4},
	{CONFIG_VAR_TYPE_INT,"ServerCubeType",  &serverCubeType,4},
	{CONFIG_VAR_TYPE_INT,"BatchAddSkillPoints",  &dwBatchAddSkillPoints,4},
	{CONFIG_VAR_TYPE_INT,"EnableHighResolutionPatch",&fEnableHighResolutionPatch,4},
};
void panel_addConfigVars() {
	for (int i=0;i<_ARRAYSIZE(aConfigVars);i++) addConfigVar(&aConfigVars[i]);
}
void dumpInvBin() {
	static char *eqName[12]={"inv","grid","rArm","torso","lArm","head","neck","rHand","lHand","belt","feet","gloves"};
	if (!logfp) return;
	InventoryBIN* pInvs=(InventoryBIN *)(*d2common_pInventoryTxt);
	for (int eqId=0;eqId<12;eqId++) {
		fprintf(logfp,"--- %s ---\n",eqName[eqId]);
		for (int i=0;i<32;i++) {
			InventoryBIN* inv=pInvs+i;
			EquipmentLayout *eq=(EquipmentLayout *)inv;
			eq+=eqId;
			fprintf(logfp,"%d+%d: (%d-%d,%d-%d) %d*%d\n",i>>4,i&0xF,eq->x0,eq->x1,eq->y0,eq->y1,eq->w,eq->h);
		}
	}
	if (1)
		for (int i=0;i<32;i++) {
			InventoryBIN* p=pInvs+i;
			fprintf(logfp,"Inventory %d:",i);
			fprintf(logfp," %d*%d",p->inv.w,p->inv.h);
			fprintf(logfp," (%d-%d,%d-%d)",p->inv.x0,p->inv.x1,p->inv.y0,p->inv.y1);
			fprintf(logfp," (%d-%d,%d-%d)",p->grid.x0,p->grid.x1,p->grid.y0,p->grid.y1);
			fprintf(logfp," %d*%d\n",p->grid.w,p->grid.h);
		}
	fflush(logfp);
}
enum EquipmentType {
	EquipInv=0,EquipGrid=1,EquipRArm=2,EquipTorso=3,EquipLArm=4,EquipHead=5,
	EquipNeck=6,EquipRHand=7,EquipLHand=8,EquipBelt=9,EquipFeet=10,EquipGloves=11
};
enum InventoryType {
	InvAma=0,InvSor=1,InvNec=2,InvPal=3,InvBar=4,Inv5=5,Inv6=6,Inv7=7,InvStashClassic=8,
	InvCube=9,Inv10=10,Inv11=11,InvStashExpansion=12,Inv13=13,InvDru=14,InvAsn=15
};
struct ServerOptionRecv {
	int valid;
	POINT inv,stash,cube;
	int invType,stashType,cubeType;
};
static ServerOptionRecv serverOption={0};
extern char *reloadStorage;
extern int reloadStorageSize;
char *printPanelConf(int isServer,char *s,char *end) {
	if (!fEnableBigStash) return s;
	if (isServer) {
		int inv=dwInvType,stash=dwStashType,cube=dwCubeType;
		dwInvType=serverInvType;dwStashType=serverStashType;dwCubeType=serverCubeType;
		if (inv!=dwInvType||stash!=dwStashType||cube!=dwCubeType) {
			installBigstashPatches();modifyStashGrid();
		}
	}
	if (dwInvType>=2) s+=_snprintf(s,end-s," Inv8x10");
	if (dwStashType>=2) s+=_snprintf(s,end-s," Stash10x10");
	if (dwCubeType==2) s+=_snprintf(s,end-s," Cube8x10");
	else if (dwCubeType==3) s+=_snprintf(s,end-s," Cube10x10");
	return s;
}
void panelNewGame() {
	if (serverOption.valid) {
		if (reloadStorage&&sizeof(ServerOptionRecv)<=reloadStorageSize)
			memcpy(reloadStorage,&serverOption,sizeof(ServerOptionRecv));
	} else {
		if (reloadStorage&&((ServerOptionRecv *)reloadStorage)->valid) {
			memcpy(&serverOption,reloadStorage,sizeof(ServerOptionRecv));
		}
	}
	if (fEnableBigStash) {
		int inv=dwInvType,stash=dwStashType,cube=dwCubeType;
		dwInvType=clientInvType;dwStashType=clientStashType;dwCubeType=clientCubeType;
		if (serverOption.valid) {
			if (!dwInvType) dwInvType=serverOption.invType;
			if (!dwStashType) dwStashType=serverOption.stashType;
			if (!dwCubeType) dwCubeType=serverOption.cubeType;
		}
		if (inv!=dwInvType||stash!=dwStashType||cube!=dwCubeType) {
			char buf[128];buf[0]=0;
			char *s=printPanelConf(0,buf,buf+128);
			gameMessage("<d2hackmap client> panel changed to %s",buf);
			installBigstashPatches();modifyStashGrid();
		}
	} else {
		dwInvType=0;dwStashType=0;dwCubeType=0;
	}
}
void panelEndGame() {
	serverOption.valid=0;
	if (reloadStorage) ((ServerOptionRecv *)reloadStorage)->valid=0;
}
void panel_reset() {
	if (fInGame) {
		//*d2client_pPanelInvChar=NULL;
	}
}
static void setStash(int invId,int stashType) {
	InventoryBIN* pInvs=(InventoryBIN *)(*d2common_pInventoryTxt);
	LOG("setStash %d %d\n",invId,stashType);
	for (int res=0;res<32;res+=16) {
		int dx=0,dy=0;
		if (res) {dx=80;dy=60;}
		InventoryBIN* pInv=pInvs+res+invId;
		if (stashType<=1) {
			pInv->inv.w=6;pInv->grid.x0=dx+74;pInv->grid.x1=dx+244;
			if (invId==InvStashClassic) {
				pInv->inv.h=4;pInv->grid.y0=dy+82;pInv->grid.y1=dy+273;
			} else {
				pInv->inv.h=8;pInv->grid.y0=dy+82;pInv->grid.y1=dy+386;
			}
		} else {
			pInv->inv.w=10;pInv->grid.x0=dx+16;pInv->grid.x1=dx+302;
			pInv->inv.h=10;pInv->grid.y0=dy+82;pInv->grid.y1=dy+371;
		}
	}
}
static void setCube(int cubeType) {
	InventoryBIN* pInvs=(InventoryBIN *)(*d2common_pInventoryTxt);
	for (int res=0;res<32;res+=16) {
		int dx=0,dy=0;
		if (res) {dx=80;dy=60;}
		InventoryBIN* pInv=pInvs+res+InvCube;
		switch (cubeType) {
			case 2:
				pInv->inv.y0=dy;
				pInv->inv.w=10;pInv->grid.x0=dx+16;pInv->grid.x1=dx+302;
				pInv->inv.h=8;pInv->grid.y0=dy+16;pInv->grid.y1=dy+250;
				break;
			case 3:
				pInv->inv.y0=dy;
				pInv->inv.w=10;pInv->grid.x0=dx+16;pInv->grid.x1=dx+302;
				pInv->inv.h=10;pInv->grid.y0=dy+16;pInv->grid.y1=dy+250+29*2;
				break;
			default:
				pInv->inv.y0=res?280:220;
				pInv->inv.w=3;pInv->grid.x0=dx+118;pInv->grid.x1=dx+205;
				pInv->inv.h=4;pInv->grid.y0=dy+139;pInv->grid.y1=dy+253;
				break;
		}
	}
}
static void setCharInv(int id,int invType) {
	InventoryBIN* pInvs=(InventoryBIN *)(*d2common_pInventoryTxt);
	for (int res=0;res<32;res+=16) {
		int dy=res?60:0;
		InventoryBIN* pInv=pInvs+res+id;
		if (invType<=1) {
			pInv->inv.h=4;pInv->inv.y1=dy+441;pInv->grid.y1=dy+368;pInv->grid.h=29;
		} else {
			pInv->inv.h=8;
			if (res==0) {
				pInv->grid.h=14;
			} else {
				pInv->inv.y1=428+29*4;pInv->grid.y1=428+29*4;
			}
		}
	}
}
void modifyStashGrid() {
	if (fIsRealmServer) {dwInvType=serverInvType;dwStashType=serverStashType;dwCubeType=serverCubeType;}
	LOG("modifyStashGrid inv=%d stash=%d cube=%d\n",dwInvType,dwStashType,dwCubeType);
	int inv=dwInvType?dwInvType:1;
	int stash=dwStashType?dwStashType:1;
	int cube=dwCubeType?dwCubeType:1;
	InventoryBIN* pInvs=(InventoryBIN *)(*d2common_pInventoryTxt);
	if (!pInvs) return;
	setStash(InvStashClassic,stash);setStash(InvStashExpansion,stash);
	setCube(cube);
	for (int i=0;i<5;i++) setCharInv(i,inv);
	setCharInv(14,inv);
	setCharInv(15,inv);
	//dumpInvBin();
}
void __fastcall MpqLoadPatch(char *path,char *buf) {
	char mypath[256];_snprintf(mypath,256,"%smpq\\%s",szPluginPath,path);
	int r=GetFileAttributesA(mypath);if ((r&0x10)==0) strcpy(buf,mypath);
}
//6FC18A4E - E8 7DE1FFFF           - call 6FC16BD0 //arg0:buf arg1: eax:path ret:absolute path
void __declspec(naked) MqpLoadPathPatch_ASM() {
	__asm {
		push dword ptr [esp+8]
		push dword ptr [esp+8]
		call storm_mk_abs_path
		pushad
		mov ecx,esi
		mov edx,eax
		call MpqLoadPatch
		popad
		retn 8
	}
}
/*
6FB3CBF7 - 8D 44 24 04           - lea eax,[esp+04]
6FB3CBFB - E8 20E8F9FF           - call 6FADB420 d2client_LoadUiImageFunc //classic stash
...
6FB3CC2C - 8D 44 24 04           - lea eax,[esp+04]
6FB3CC30 - E8 EBE7F9FF           - call 6FADB420 d2client_LoadUiImageFunc //expansion stash
*/
void __declspec(naked) ModifyStashImagePatch_ASM() {
	__asm {
		mov eax, stashImg
		jmp d2client_LoadUiImageFunc
	}
}
/*
6FB3CB75 - 6A 00                 - push 00 { 0 }
6FB3CB77 - 8D 44 24 04           - lea eax,[esp+04]
6FB3CB7B - E8 A0E8F9FF           - call 6FADB420 d2client_LoadUiImageFunc
6FB3CB80 - A3 78BCBC6F           - mov [6FBCBC78],eax { (05AF0000) }
*/
void __declspec(naked) ModifyCubeImagePatch_ASM() {
	__asm {
		cmp dwCubeType,2
		jne notType1
		mov eax, cube8x10Img
		jmp end
notType1:
		cmp dwCubeType,3
		jne end
		mov eax, cube10x10Img
end:
		jmp d2client_LoadUiImageFunc
	}
}
static CellFile *invChar8x10=NULL;
static void loadInvChar8x10() {
	if (invChar8x10) return;
	invChar8x10=d2client_LoadUiImage(invCharImg);
}

//6FAD7DE3 - E8 D8780900           - call 6FB6F6C0 d2client_LoadUiImageSubpath
void __declspec(naked) ModifyInvCharImagePatch_ASM() {
	__asm {
		pushad
		call loadInvChar8x10
		popad
		push 0
		mov eax,invCharHalfImg
		call d2client_LoadUiImageFunc
		ret
	}
}
//6FB49B29 - A1 C8A7BC6F           - mov eax,[6FBCA7C8 d2client_pPanelInvChar] <--------
//6FB49B2E - 8B 0D 4CBCB86F        - mov ecx,[6FB8BC4C d2client_ScreenHeight] 
//6FB49B34 - 8B 15 A4B9BC6F        - mov edx,[6FBCB9A4] { (-60) }
void __declspec(naked) ChooseInvCharImagePatch_ASM() {
	__asm {
		mov ecx,[d2gfx_pResolutionMode]
		mov eax,[ecx]
		cmp eax,0
		je res0
		mov eax,[invChar8x10]
		test eax,eax
		jnz ok
		pushad
		call loadInvChar8x10
		popad
		mov eax,[invChar8x10]
ok:
		ret
res0:
		mov ecx,[d2client_pPanelInvChar]
		mov eax,[ecx]
		ret
	}
}

static void calDrawOffset() {
	*d2client_pDrawOffsetX=(SCREENWIDTH-640)/2;
	int dy=-(SCREENHEIGHT-480)>>1;
	*d2client_pDrawOffsetY=dy;
	if (*d2gfx_pResolutionMode&&dwInvType>=2) {
		invCharTopDy=dy-58;invCharBottomDy=dy+58;
	} else {
		invCharTopDy=dy;invCharBottomDy=dy;
	}
}
/* D2CLIENT
6FB73A05 - C7 05 A4B9BC6F C4FFFFFF - mov [6FBCB9A4 d2client_DrawOffsetY],FFFFFFC4 { (0),-60 } <---
...
6FB73A17 - 89 1D A4B9BC6F        - mov [6FBCB9A4 d2client_DrawOffsetY],ebx { (0) }
*/
void __declspec(naked) CalDrawOffsetPatch_ASM() {
	__asm {
		pushad
		call calDrawOffset
		popad
		ret
	}
}
static void getEquipInvDrawOffset(int *dx,int *dy, void *src) {
	switch (*d2gfx_pResolutionMode) {
		case 0:*dx=(SCREENWIDTH-640)>>1;*dy=(SCREENHEIGHT-480)>>1;return;
		case 2:*dx=(SCREENWIDTH-800)>>1;*dy=(SCREENHEIGHT-600)>>1;break;
		default:*dx=0;*dy=0;return;
	}
	if (dwInvType<=1) return;
	int invTxtAddr=(int)(*d2common_pInventoryTxt);
	int srcAddr=(int)src;
	int invId=(srcAddr-invTxtAddr)/sizeof(InventoryBIN);
	if (invId>16) invId-=16;
	if (0<=invId&&invId<5||invId==14||invId==15) *dy-=58;
}
static void __fastcall getEquipmentSlotLayout(int *src, int *dst) {
	if (src[0]<0||src[1]<0||src[2]<0||src[3]<0) return;
	int dx,dy;getEquipInvDrawOffset(&dx,&dy,src);
	if (dx==0&&dy==0) return;
	dst[0]+=dx;dst[1]+=dx;dst[2]+=dy;dst[3]+=dy;
}
/*
call 20 times when resolution change
6fdd1f20 D2Common.dll/Ordinal10701 144
6FDD1F9C - 8B 40 10              - mov eax,[eax+10]
6FDD1F9F - 89 42 10              - mov [edx+10],eax
*/
void __declspec(naked) GetEquipmentSlotLayoutPatch_ASM() {
	__asm {
		pushad
		mov ecx, [eax+0x10]
		mov [edx+0x10],ecx
		mov ecx,eax
		call getEquipmentSlotLayout
		popad
		ret
	}
}
static void __fastcall getInventoryGridLayout(int *dst, int *src) {
	if (src[1]<0||src[2]<0||src[3]<0||src[4]<0) return;
	int dx,dy;getEquipInvDrawOffset(&dx,&dy,src);
	if (dx==0&&dy==0) return;
	dst[1]+=dx;dst[2]+=dx;dst[3]+=dy;dst[4]+=dy;
}
/*
call only once
6fdd1fb0 D2Common.dll/Ordinal10760 128
6FDD2020 - 8B 40 14              - mov eax,[eax+14]
6FDD2023 - 89 41 14              - mov [ecx+14],eax
*/
void __declspec(naked) GetInventoryGridLayoutPatch_ASM() {
	__asm {
		pushad
		mov edx,[eax+0x14]
		mov [ecx+0x14],edx
		mov edx,eax
		call getInventoryGridLayout
		popad
		ret
	}
}
static void __fastcall getInventoryPosition(int *dst,int *src) {
	if (src[0]<0||src[1]<0||src[2]<0||src[3]<0) return;
	int dx,dy;getEquipInvDrawOffset(&dx,&dy,src);
	if (dx==0&&dy==0) return;
	dst[0]+=dx;dst[1]+=dx;dst[2]+=dy;dst[3]+=dy;
}
/*
only call once after resolution change
D2FUNCNAME(d2common,0x6FDD2030,11012,getInventoryPos16,void,__stdcall,(int invId,int resId,EquipmentLayout16 *dst))
D2Common.getInventoryPos16 - 8B 44 24 08           - mov eax,[esp+08]
...
6FDD207C - 8B 10                 - mov edx,[eax] //0x190 400
6FDD207E - 8B 4C 24 0C           - mov ecx,[esp+0C] //arg2 6FBBB2F8
6FDD2082 - 89 11                 - mov [ecx],edx
6FDD2084 - 8B 50 04              - mov edx,[eax+04] //0x2d0 720
6FDD2087 - 89 51 04              - mov [ecx+04],edx
6FDD208A - 8B 50 08              - mov edx,[eax+08] //0x3c 60
6FDD208D - 89 51 08              - mov [ecx+08],edx
6FDD2090 - 8B 40 0C              - mov eax,[eax+0C] //0x1f5 501  <--- install here
6FDD2093 - 89 41 0C              - mov [ecx+0C],eax
6FDD2096 - C2 0C00               - ret 000C { 12 }
*/
void __declspec(naked) GetInventoryPositionPatch_ASM() {
	__asm {
		pushad
		mov edx, [eax+0xC]
		mov [ecx+0xC], edx
		mov edx,eax
		call getInventoryPosition
		popad
		ret
	}
}
static void __fastcall addSkillPatch(int skillId) {
	BYTE packet[3];packet[0]=0x3B;*(short *)(packet+1)=(short)skillId;
	if (IsKeyDown(VK_SHIFT)) {
		for (int i=0;i<dwBatchAddSkillPoints;i++) SendPacket(packet,3);
	} else SendPacket(packet,3);
}
//6FB29671 - 8B CE                 - mov ecx,esi
//6FB29673 - B0 3B                 - mov al,3B { 59 }
//6FB29675 - E8 96B3F9FF           - call 6FAC4A10 d2client_SendPacket3
void __declspec(naked) AddSkillPatch_ASM() {
	__asm {
		pushad
		call addSkillPatch
		popad
		ret
	}
}
extern int fStartingGame;
static int guessInvType(int row,int col) {return row<=4?1:2;}
static int guessStashType(int row,int col) {return col<=6?1:2;}
static int guessCubeType(int row,int col) {return row<=4?1:row<=8?2:3;}
static void __fastcall recvCommand26(char *packet) {
	if (packet[0]!=0x26) return;
	if (packet[1]!=4) return; //sys message
	if (!fInGame) fStartingGame=1;
	char *name="<d2hackmap113c>";
	if (strcmp(packet+10,name)!=0) return;
	memset(&serverOption,0,sizeof(ServerOptionRecv));
	char *s=packet+10+strlen(name)+1;
	char *end=s+256;
	s=strchr(s,':');if (s) s++;
	while (s&&*s&&s<end) {
		while (*s==' ') s++;
		LOG("%s\n",s);
		if (memcmp(s,"Inv",3)==0) {
			s+=3;serverOption.inv.y=strtol(s,&s,10);
			s=strchr(s,'x');if (!s) break;s++;serverOption.inv.x=strtol(s,&s,10);
			serverOption.invType=guessInvType(serverOption.inv.y,serverOption.inv.x);
		} if (memcmp(s,"Stash",5)==0) {
			s+=5;serverOption.stash.y=strtol(s,&s,10);
			s=strchr(s,'x');if (!s) break;s++;serverOption.stash.x=strtol(s,&s,10);
			serverOption.stashType=guessStashType(serverOption.stash.y,serverOption.stash.x);
		} if (memcmp(s,"Cube",4)==0) {
			s+=4;serverOption.cube.y=strtol(s,&s,10);
			s=strchr(s,'x');if (!s) break;s++;serverOption.cube.x=strtol(s,&s,10);
			serverOption.cubeType=guessCubeType(serverOption.cube.y,serverOption.cube.x);
		}
		if (s) s=strchr(s,' ');
	}
	serverOption.valid=1;
}
/*
6FB5F9F0 - 81 EC 1C010000        - sub esp,0000011C { 284 }
6FB5F9F6 - 8A 01                 - mov al,[ecx]
6FB5F9F8 - 8A 51 01              - mov dl,[ecx+01]
*/
void __declspec(naked) RecvCommand_26_Patch_ASM() {
	__asm {
		pushad
		call recvCommand26
		popad
		mov al,[ecx]
		mov dl,[ecx+1]
		ret
	}
}
int installPanelPatches() {
//d2client_79675: E8 96 B3 F9 FF     call d2client_14A10
PatchCall(d2client,0x79675,AddSkillPatch_ASM,5,"E8 96 B3 F9 FF");
//d2client_AF9F6: 8A 01              mov al, [ecx]
//d2client_AF9F8: 8A 51 01           mov dl, [ecx+0x1]
PatchCall(d2client,0xAF9F6,RecvCommand_26_Patch_ASM,5,"8A 01 8A 51 01");
if (!fEnableHighResolutionPatch) return 0;
//d2client_C3A05: C7 05 A4 B9 BC 6F C4 FF FF FF  mov dword ptr [d2client_11B9A4](->FFFFFFC4), 0xFFFFFFC4
PatchCall(d2client,0xC3A05,CalDrawOffsetPatch_ASM,10,"C7 05 $(+11B9A4) C4 FF FF FF"); //800*600
//d2client_C3A17: 89 1D A4 B9 BC 6F  mov [d2client_11B9A4](->FFFFFFC4), ebx
PatchCall(d2client,0xC3A17,CalDrawOffsetPatch_ASM,6,"89 1D $(+11B9A4)"); //640*480
//d2common_81F9C: 8B 40 10           mov eax, [eax+0x10]
//d2common_81F9F: 89 42 10           mov [edx+0x10], eax
PatchCall(d2common,0x81F9C,GetEquipmentSlotLayoutPatch_ASM,6,"8B 40 10 89 42 10");
//d2common_82020: 8B 40 14           mov eax, [eax+0x14]
//d2common_82023: 89 41 14           mov [ecx+0x14], eax
PatchCall(d2common,0x82020,GetInventoryGridLayoutPatch_ASM,6,"8B 40 14 89 41 14");
//d2common_82090: 8B 40 0C           mov eax, [eax+0xC]
//d2common_82093: 89 41 0C           mov [ecx+0xC], eax
PatchCall(d2common,0x82090,GetInventoryPositionPatch_ASM,6,"8B 40 0C 89 41 0C");
//storm_28A4E: E8 7D E1 FF FF     call storm_26BD0
PatchCall(storm,0x28A4E,MqpLoadPathPatch_ASM,5,"E8 7D E1 FF FF");
	return 0;
}
static InstalledPatch *panelPatches=NULL;
extern InstalledPatch **pPatchList;
int uninstallBigstashPatches() {removePatches(&panelPatches);return 0;}
int installBigstashPatches() {
	pPatchList=&panelPatches;
	uninstallBigstashPatches();
	if (!fEnableBigStash) return 1;
	LOG("installBigstashPatches inv=%d stash=%d cube=%d\n",dwInvType,dwStashType,dwCubeType);
	if (dwCubeType>=2) {
//d2client_8CB7B: E8 A0 E8 F9 FF     call d2client_2B420
PatchCall(d2client,0x8CB7B,ModifyCubeImagePatch_ASM,5,"E8 A0 E8 F9 FF"); //cube
		if (dwCubeType==3) {
//d2client_99678: 8D 8C 10 44 FF FF FF  lea ecx, [eax+edx-0xBC]
patchValue("transmute button Y",PATCH_ADDR(d2client,0x9967B),-130,4,"-3 8D 8C 10 44 FF FF FF");
//d2client_8B626: 8D B4 08 21 FF FF FF  lea esi, [eax+ecx-0xDF]
patchValue("transmute button hover",PATCH_ADDR(d2client,0x8B629),-0xDF+29*2,4,"-3 8D B4 08 21 FF FF FF");
//d2client_8B631: 8D 84 08 49 FF FF FF  lea eax, [eax+ecx-0xB7]
patchValue("transmute button hover",PATCH_ADDR(d2client,0x8B634),-0xB7+29*2,4,"-3 8D 84 08 49 FF FF FF");
//d2client_997A1: 8D 8C 10 21 FF FF FF  lea ecx, [eax+edx-0xDF]
patchValue("transmute button hover",PATCH_ADDR(d2client,0x997A4),-0xDF+29*2,4,"-3 8D 8C 10 21 FF FF FF");
		}
	}
	if (dwStashType>=2) {
//d2client_8CBFB: E8 20 E8 F9 FF     call d2client_2B420
PatchCall(d2client,0x8CBFB,ModifyStashImagePatch_ASM,5,"E8 20 E8 F9 FF"); //classic
//d2client_8CC30: E8 EB E7 F9 FF     call d2client_2B420
PatchCall(d2client,0x8CC30,ModifyStashImagePatch_ASM,5,"E8 EB E7 F9 FF"); //expansion
//d2client_99A18: 75 07              jnz d2client_99A21 ->+7 B99A21
patchValue("classic gold max Y",PATCH_ADDR(d2client,0x99A18),0xEB,1,"75 07");
//d2client_8D160: 74 1B              jz d2client_8D17D ->+27 B8D17D
patchValue("classic withdraw gold button text Y",PATCH_ADDR(d2client,0x8D160),0x9090,2,"74 1B");
//d2client_8D1B4: 74 47              jz d2client_8D1FD ->+71 B8D1FD
patchValue("classic withdraw mouse hover",PATCH_ADDR(d2client,0x8D1B4),0x9090,2,"74 47");
//d2client_8B6C5: 74 14              jz d2client_8B6DB ->+20 B8B6DB
patchValue("classic withdraw mouse click",PATCH_ADDR(d2client,0x8B6C5),0x9090,2,"74 14");
	}
	if (dwInvType>=2) {
//d2client_27DE3: E8 D8 78 09 00     call d2client_BF6C0
PatchCall(d2client,0x27DE3,ModifyInvCharImagePatch_ASM,5,"E8 D8 78 09 00"); //invchar
//d2client_99B29: A1 C8 A7 BC 6F     mov eax, [d2client_11A7C8]
PatchCall(d2client,0x99B29,ChooseInvCharImagePatch_ASM,5,"A1 $(+11A7C8)"); //invchar
		//move button to bottom
//d2client_8D10E: 8B 0D A4 B9 BC 6F  mov ecx, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("gold number, withdraw button",PATCH_ADDR(d2client,0x8D110),(int)&invCharBottomDy,4,"-2 8B 0D $(+11B9A4)");
//d2client_9A6F7: A1 A4 B9 BC 6F     mov eax, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("cancel button",PATCH_ADDR(d2client,0x9A6F8),(int)&invCharBottomDy,4,"-1 A1 $(+11B9A4)");
//d2client_93779: 8B 35 A4 B9 BC 6F  mov esi, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("hover cancel button",PATCH_ADDR(d2client,0x9377B),(int)&invCharBottomDy,4,"-2 8B 35 $(+11B9A4)");
//d2client_8D3E5: A1 A4 B9 BC 6F     mov eax, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("click cancel button",PATCH_ADDR(d2client,0x8D3E6),(int)&invCharBottomDy,4,"-1 A1 $(+11B9A4)");
//d2client_8BDAF: A1 A4 B9 BC 6F     mov eax, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("click cancel button",PATCH_ADDR(d2client,0x8BDB0),(int)&invCharBottomDy,4,"-1 A1 $(+11B9A4)");
		int dyAddr=(int)d2client_pDrawOffsetY;
//d2client_99B34: 8B 15 A4 B9 BC 6F  mov edx, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("InvChar6 top left",PATCH_ADDR(d2client,0x99B36),(int)&invCharTopDy,4,"-2 8B 15 $(+11B9A4)");
//d2client_99B75: 8B 0D A4 B9 BC 6F  mov ecx, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("InvChar6 top right",PATCH_ADDR(d2client,0x99B77),(int)&invCharTopDy,4,"-2 8B 0D $(+11B9A4)");
//d2client_99BB1: A1 A4 B9 BC 6F     mov eax, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("InvChar6 bottom left",PATCH_ADDR(d2client,0x99BB2),(int)&invCharBottomDy,4,"-1 A1 $(+11B9A4)");
//d2client_99BEC: 8B 15 A4 B9 BC 6F  mov edx, [d2client_11B9A4 int d2client_DrawOffsetY]
patchValue("InvChar6 bottom right",PATCH_ADDR(d2client,0x99BEE),(int)&invCharBottomDy,4,"-2 8B 15 $(+11B9A4)");
	}
	return 1;
}
