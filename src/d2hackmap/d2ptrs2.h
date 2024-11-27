#ifdef INIT_D2PTR_ADDR
  #define D2VARADDR(dll,addr1,name1,t1)     dll##_p##name1=(t1 *)(addr1-##dll##_ADDR+(DWORD)addr_##dll);
  #define D2FUNCADDR(dll,addr1,name1,t1,t2,t3) dll##_##name1=(t1 (t2 *) t3)(addr1-##dll##_ADDR+(DWORD)addr_##dll);
  #define D2FUNCNAME(dll,addr1,str1,name1,t1,t2,t3) dll##_##name1=(t1 (t2 *) t3)GetProcAddress(addr_##dll,(LPCSTR)str1);
  #define D2ASMPTR(dll,addr1,name1)        dll##_v##name1=addr1-##dll##_ADDR+(DWORD)addr_##dll;
#else
	#ifdef DECLARE_D2PTR_VARIBLE
		#define D2VARADDR(dll,addr1,name1,t1)     t1 *dll##_p##name1;
		#define D2FUNCADDR(dll,addr1,name1,t1,t2,t3) t1 (t2 * dll##_##name1) t3;
		#define D2FUNCNAME(dll,addr1,str1,name1,t1,t2,t3) t1 (t2 * dll##_##name1) t3;
		#define D2ASMPTR(dll,addr1,name1)        DWORD dll##_v##name1;
	#else
		#ifdef EXPORT_D2PTR_NAME_ADDRESS
			#define D2VARADDR(dll,addr1,name1,t1)     {addr1,#dll,#name1},
			#define D2FUNCADDR(dll,addr1,name1,t1,t2,t3) {addr1,#dll,#name1},
			#define D2FUNCNAME(dll,addr1,str1,name1,t1,t2,t3) {addr1,#dll,#name1},
			#define D2ASMPTR(dll,addr1,name1)        {addr1,#dll,#name1},
		#else
			#define D2VARADDR(dll,addr1,name1,t1)     extern t1 *dll##_p##name1;
			#define D2FUNCADDR(dll,addr1,name1,t1,t2,t3) extern t1 (t2 * dll##_##name1) t3;
			#define D2FUNCNAME(dll,addr1,str1,name1,t1,t2,t3) extern t1 (t2 * dll##_##name1) t3;
			#define D2ASMPTR(dll,addr1,name1)        extern DWORD dll##_v##name1;
			#define DEFINE_D2PTR
		#endif
	#endif
#endif

#define d2multi_ADDR 0x6F9D0000
	D2VARADDR(d2multi,0x6F9E9C60, EditboxPreferences,    void *)  //6F9E9C5C+4
	D2VARADDR(d2multi,0x6FA09CC0, GameListControl,       D2EditBox *)
	D2VARADDR(d2multi,0x6FA09D2C, GameName,       D2EditBox *)
	D2VARADDR(d2multi,0x6FA09D30, GamePassword,       D2EditBox *)
	D2VARADDR(d2multi,0x6FA09D34, GameDescription,       D2EditBox *)

#undef d2multi_ADDR

#ifdef DEFINE_D2PTR
typedef struct {
	char off00[0x48];
	char accountName[16];
} D2AccountInfo;
#endif

#define d2mcpclient_ADDR 0x6FA20000
	D2VARADDR(d2mcpclient,0x6FA2AEE0, AccountInfo,       D2AccountInfo)
#undef d2mcpclient_ADDR
//------------ Newly added pointers which will be moved to d2ptrs.h in batch -------------

#define d2client_ADDR 0x6FAB0000
	D2FUNCADDR(d2client,0x6FB5FA90,RecvPacket01,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C4F0,RecvPacket02,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DB50,RecvPacket03,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DFE0,RecvPacket04,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5CB70,RecvPacket05,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5CAA0,RecvPacket06,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C440,RecvPacket07,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C3D0,RecvPacket08,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DF70,RecvPacket09,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5CA40,RecvPacket0a,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D6E0,RecvPacket0b,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DAA0,RecvPacket11,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C370,RecvPacket12,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C360,RecvPacket13,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C350,RecvPacket14,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F640,RecvPacket15,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D9D0,RecvPacket16,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5ECA0,RecvPacket18,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EDF0,RecvPacket19,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EDF0,RecvPacket1a,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EDF0,RecvPacket1b,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EDF0,RecvPacket1c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EDF0,RecvPacket1d,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EDF0,RecvPacket1e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EDF0,RecvPacket1f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5ED80,RecvPacket20,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C910,RecvPacket21,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C800,RecvPacket22,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C780,RecvPacket23,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F9F0,RecvPacket26,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D680,RecvPacket27,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5FA70,RecvPacket28,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C330,RecvPacket29,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F9B0,RecvPacket2a,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D740,RecvPacket2c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E300,RecvPacket3e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F910,RecvPacket3f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D340,RecvPacket40,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DB70,RecvPacket42,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C130,RecvPacket45,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E2B0,RecvPacket47,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E260,RecvPacket48,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C0B0,RecvPacket4e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C0E0,RecvPacket4f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D4B0,RecvPacket50,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DF40,RecvPacket51,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D520,RecvPacket52,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E9C0,RecvPacket53,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C120,RecvPacket54,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D560,RecvPacket57,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F980,RecvPacket58,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DF20,RecvPacket59,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D6B0,RecvPacket5a,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D8F0,RecvPacket5b,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D8E0,RecvPacket5c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F8F0,RecvPacket5d,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C090,RecvPacket5e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C050,RecvPacket5f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D2F0,RecvPacket60,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F940,RecvPacket61,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DFD0,RecvPacket62,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F8B0,RecvPacket63,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D890,RecvPacket65,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C040,RecvPacket66,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EEE0,RecvPacket73,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DEF0,RecvPacket74,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D870,RecvPacket75,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D370,RecvPacket76,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F930,RecvPacket77,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5BFD0,RecvPacket78,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5BFB0,RecvPacket79,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C720,RecvPacket7a,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C6D0,RecvPacket7b,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D2A0,RecvPacket7c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E1F0,RecvPacket7d,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D240,RecvPacket7e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D820,RecvPacket7f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EBA0,RecvPacket81,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D160,RecvPacket82,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5BFA0,RecvPacket89,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DB30,RecvPacket8a,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D800,RecvPacket8b,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D7B0,RecvPacket8c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D780,RecvPacket8d,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D130,RecvPacket8e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5BF40,RecvPacket8f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D220,RecvPacket90,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C750,RecvPacket91,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E230,RecvPacket92,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D3D0,RecvPacket93,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5C880,RecvPacket94,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EBE0,RecvPacket95,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D950,RecvPacket96,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5BF50,RecvPacket97,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D4F0,RecvPacket98,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F0F0,RecvPacket99,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F090,RecvPacket9a,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E1E0,RecvPacket9b,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E080,RecvPacket9c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E870,RecvPacket9d,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EA40,RecvPacket9e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EA40,RecvPacket9f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EA40,RecvPacketa0,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EA40,RecvPacketa1,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EA40,RecvPacketa2,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5EFB0,RecvPacketa3,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D760,RecvPacketa4,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5F7F0,RecvPacketa5,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5BF20,RecvPacketa6,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DEB0,RecvPacketa7,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DD70,RecvPacketa8,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DD30,RecvPacketa9,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5DB90,RecvPacketaa,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D0B0,RecvPacketab,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5E420,RecvPacketac,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FB5D630,RecvPacketae,void,__fastcall,())
	D2FUNCADDR(d2client,0x6FBAAE28,RecvPacketaf,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f5f0,RecvPacket8_0c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f590,RecvPacket8_0d,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f560,RecvPacket8_0e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f510,RecvPacket8_0f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f4c0,RecvPacket8_10,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f240,RecvPacket8_17,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f1d0,RecvPacket8_4c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f160,RecvPacket8_4d,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f460,RecvPacket8_67,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f3f0,RecvPacket8_68,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f390,RecvPacket8_69,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f340,RecvPacket8_6a,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f7c0,RecvPacket8_6b,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f750,RecvPacket8_6c,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5f2b0,RecvPacket8_6d,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5c3c0,RecvPacket8_6e,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5c3b0,RecvPacket8_6f,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5c3a0,RecvPacket8_70,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5c390,RecvPacket8_71,void,__fastcall,())
	D2FUNCADDR(d2client,0x6fb5c380,RecvPacket8_72,void,__fastcall,())
#undef d2client_ADDR

#define d2common_ADDR 0x6FD50000
	D2FUNCNAME(d2common,0x6FD80F80,10589,getUnitSize,int,__stdcall,(UnitAny *pUnit))
	D2FUNCNAME(d2common,0x6FD7F370,11153,allocUnit,UnitAny *,__stdcall,(int arg0,int unitType))
#undef d2common_ADDR

#define storm_ADDR 0x6FBF0000
	D2FUNCNAME(storm,0x6FC1A0B0,266,openMpq,int,__stdcall,(LPCSTR lpFileName,int dwPriority,int dwFlags,void *mpq))
	D2FUNCNAME(storm,0x6FC2C2B0,422,regQueryValue,int,__stdcall,(char *keyName,char *valueName,int arg,char *buf,int len))
#undef storm_ADDR

#ifdef DEFINE_D2PTR
struct D2Character {
	char name[16]; //+00
	char off10[0xF0]; //+10
	wchar_t realm[16]; //+100
	char off110[0x22C]; //+120
	struct D2Character *next; //+34C
};
#endif
#define d2launch_ADDR 0x6FA40000
	D2FUNCADDR(d2launch,0x6FA4101B,sprintf,int,__cdecl,(char *buf,char *fmt,...))
	D2FUNCADDR(d2launch,0x6FA4F3A0,DeleteCharacter,void,__fastcall,(char *name)) //eax:region
	D2FUNCADDR(d2launch,0x6FA4DB60,DeleteSelectedCharacter,int,__stdcall,(int arg)) //return 0:failed 1:deleted
	D2VARADDR(d2launch,0x6FA64DB0,SelectedCharIndex,int)
	D2VARADDR(d2launch,0x6FA65EC8,D2Characters,D2Character *)
	D2VARADDR(d2launch,0x6FA65ED0,charCount,int)
#undef d2launch_ADDR

#define fog_ADDR 0x6FF50000
	D2FUNCNAME(fog,0x6FF6CD50,10045,allocMemory,void *,__fastcall,(int arg0,int size,char *source,int line,int arg4))
	D2FUNCNAME(fog,0x6FF6CCC0,10046,freeMemory,int,__fastcall,(int arg0,void *ptr,char *source,int line,int arg4))
#undef fog_ADDR
//----------------------------------------

#undef D2FUNCADDR
#undef D2FUNCNAME
#undef D2VARADDR
#undef D2ASMPTR
#ifdef DEFINE_D2PTR
	#undef DEFINE_D2PTR
#endif
