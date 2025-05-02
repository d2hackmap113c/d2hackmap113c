#ifdef INIT_D2PTR_ADDR
  #define D2VARADDR(dll,addr1,name1,t1)     dll##_p##name1=(t1 *)(addr1-##dll##_ADDR+(DWORD)addr_##dll);
  #define D2FUNCADDR(dll,addr1,name1,t1,t2,t3) dll##_##name1=(t1 (t2 *) t3)(addr1-##dll##_ADDR+(DWORD)addr_##dll);
  #define D2FUNCNAME(dll,addr1,str1,name1,t1,t2,t3) dll##_##name1=(t1 (t2 *) t3)GetProcAddress(addr_##dll,(LPCSTR)str1);
#else
	#ifdef DECLARE_D2PTR_VARIBLE
		#define D2VARADDR(dll,addr1,name1,t1)     t1 *dll##_p##name1;
		#define D2FUNCADDR(dll,addr1,name1,t1,t2,t3) t1 (t2 * dll##_##name1) t3;
		#define D2FUNCNAME(dll,addr1,str1,name1,t1,t2,t3) t1 (t2 * dll##_##name1) t3;
	#else
		#define D2VARADDR(dll,addr1,name1,t1)     extern t1 *dll##_p##name1;
		#define D2FUNCADDR(dll,addr1,name1,t1,t2,t3) extern t1 (t2 * dll##_##name1) t3;
		#define D2FUNCNAME(dll,addr1,str1,name1,t1,t2,t3) extern t1 (t2 * dll##_##name1) t3;
		#define DEFINE_D2PTR
	#endif
#endif


	D2FUNCNAME(d2win,0x6F8F2FA0,10150,   DrawText,            void, __fastcall, (wchar_t *wStr, long xPos, long yPos, int dwColor, int dwAlign))
	D2FUNCNAME(d2win,0x6F8F2FE0,10184,   SetTextFont,            int,__fastcall, (int dwFont))
	D2FUNCADDR(d2win,0x6F8F87E0,drawMainMenu,void,__stdcall,())
	D2FUNCADDR(d2client,0x6FB714F0,14F0,void,__stdcall,())
	D2FUNCNAME(d2gfx,0x6FA8ADD0,10040,flushFramebuf,void,__stdcall, ())

#undef D2FUNCADDR
#undef D2FUNCNAME
#undef D2VARADDR
#ifdef DEFINE_D2PTR
	#undef DEFINE_D2PTR
#endif
