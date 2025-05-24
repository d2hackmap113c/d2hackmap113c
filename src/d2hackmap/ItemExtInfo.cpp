#include "stdafx.h"
#include "header.h"

extern int fLocalizationSupport;
extern ToggleVar 	tItemExtInfo;
extern char 			szItemExtInfoCfgName[2][256];
extern BYTE 			nDefaultExtInfoColour;
void ShowItemExtInfo();
int LoadExtInfo();
void UnLoadExtInfo();
void SetExtInfoPos(int xPos , int yPos , int txtWidth , int txtHeight);

void ItemExtInfoNewGame() {
	LoadExtInfo();
}
//需要在ItemDesc.cpp中调用SetExtInfoPos
static int xShowPos = 0 ;
static int yShowPos = 0 ;
static BOOL fLeft = FALSE;

static wchar_t *pwszNormalInfo[3000][5] = {{L'\0'}};
static wchar_t *pwszUniqueInfo[500] = {L'\0'};
static wchar_t *pwszSetInfo[200] = {L'\0'};
static wchar_t *pwszRuneWordInfo[170] = {L'\0'};
static wchar_t *wszRuneInfo[40] = {{L'\0'}};
static wchar_t *wszCubeMain[20] = {{L'\0'}};
static BOOL fExtInfoLoad = FALSE;
void ConvertToColorString( BYTE *dst , char  *src , int size) {
  char ch = 0, col = 0, next;
  BYTE *pOld = dst;
  int sizecount = 0 ;
  ch = *src++;
  sizecount++;
  while(ch){
    next = *src;
    if( ch=='%' && next){
      if ( isalpha(next) ){
        col = toupper(next)-'A'+10;
		if(col>12){
			col = 0;
			*dst++ = ch;
		}else{
			src++;
		}
      }else if (isdigit(next)){
        col = (char)strtol(src,&src,10);
	  }else{
	    col = 0;
	    *dst++ = ch;
	  }
      if ( col ){
        *dst++ = 0xff;
        *dst++ = 'c';
        *dst++ = '0'+col;
        pOld = dst;
      }
      
    }else{
      *dst++ = ch;
    }
	ch = *src++;
    sizecount++;

  }
  if(col){
	*dst++ = 0xff;
  }
  *dst = 0 ;
  if(sizecount>=size){
    *(pOld+size) = L'\0';
  }

}

int LoadExtInfo(){
	if (tItemExtInfo.isOn == 0) return -1;
	if (fExtInfoLoad) return 0;
	fExtInfoLoad = TRUE;
	if (!szItemExtInfoCfgName[dwGameLng][0]){
		tItemExtInfo.isOn = 0 ;
		return -1;
	}
	FILE *fp=openFile(szItemExtInfoCfgName[dwGameLng],"r");if (!fp) return -1;
	LOG("loading %s\n",szItemExtInfoCfgName[dwGameLng]);
	char line[1024], left[1024], right[2048];
	BOOL fWaitCmd = TRUE;
	BOOL fWaitEnd = FALSE;
	BOOL fWaitStart = FALSE;
	wchar_t **pInfo = NULL;
	int mCmdtype = 0 ;
	int linecount = 0 ;
	char szInfo[40][512];
	int lineno = 0;
	while (1) {
		char *line0=fgets(line,1024,fp);if (!line0) break;
		__try {
			char *p = line;
			lineno ++;
			if ( fWaitCmd ){
				pInfo = NULL;
				mCmdtype = 0 ;
				char *pLeft = left;

				while(*p){
					if( *p=='/' && *(p+1)=='/' ){
						break;
					}else if (!isspace(*p) && isalpha(*p)){
						while ( *p && isalpha(*p) ){
							*pLeft = *p;
							p++;
							pLeft++;
						}
						*pLeft = '\0';
						if ( _stricmp(left, "UniqueItemInfo") ==0 ){
							mCmdtype = 1 ;
						}else if ( _stricmp(left, "SetItemInfo") ==0 ){
							mCmdtype = 2;
						}else if ( _stricmp(left, "RuneWordInfo") ==0 ){
							mCmdtype = 3;
						}else if ( _stricmp(left, "NormalItemInfo") ==0 ){
							mCmdtype = 4;
						}else if ( _stricmp(left, "RuneInfo") ==0 ){
							mCmdtype = 5;
						}else if ( _stricmp(left, "CubeMain") ==0 ){
							mCmdtype = 6;
						}else{
							//ShowGameMessage("UnKnow CMD!" );
						}
						break;
					}
					p++;
				}//while end

				if ( mCmdtype ){
					int idx1 = 0 ;
					int idx2 = 0 ;
					while(*p && !isdigit(*p)) p++;
					if (*p){
						char *endptr;
						idx1 = strtoul(p,&endptr,0);
						if (idx1<0)idx1=0;
						if (mCmdtype==4){
							while(*endptr && !isdigit(*endptr)) endptr++;
							if(*endptr) idx2 = strtoul(endptr,&endptr,0);
							if ( idx2 > 4 ) idx2 = 4;
							if ( idx2 < 0 ) idx2 = 0;
						}
					}
					switch(mCmdtype){
						case 1:
							if (idx1>499)idx1=499;
							pInfo = &pwszUniqueInfo[idx1];
							break;
						case 2:
							if (idx1>199)idx1=199;
							pInfo = &pwszSetInfo[idx1];
							break;
						case 3:
							if (idx1>169)idx1=169;
							pInfo = &pwszRuneWordInfo[idx1];
							break;
						case 4:
							if (idx1>3000 ) idx1 = 3000;
							if ( idx1<1 ) idx1 = 1;
							pInfo = &pwszNormalInfo[idx1-1][idx2];
							break;
						case 5:
							if ( idx1>33 ) idx1 = 33;
							if ( idx1<1 ) idx1 = 1;
							pInfo = &wszRuneInfo[idx1-1];
							break;
						case 6:
							if ( idx1>19 ) idx1 = 19;
							pInfo = &wszCubeMain[idx1];
							break;
					}
					
					fWaitCmd = FALSE;
					fWaitStart = TRUE;
				
				} //if mCmdtype end
			}//if fWaitCmd end

			if ( fWaitStart ){
				while(*p && *p != '{')p++;
				if (*p) {
					fWaitStart = FALSE;
					fWaitEnd = TRUE;
				}
				memset(right,0,sizeof(right));
				memset(szInfo,0,sizeof(szInfo));
				linecount = 0;
				continue;
			}
			if ( fWaitEnd ){
				char *p1 = line;
				while(*p1 && *p1!='}')p1++;
				if ( *p1 ){
					//找到结束标识
					linecount--;
					while(linecount>=0){
						strcat(right,szInfo[linecount]);
						if (linecount>0){
							strcat(right,"\n");
						}
						if(strlen(right)>2048)break;
						linecount--;
					}

					if ( right[0] ){//有信息
						//做个颜色处理
						char temp[2048];
						memset(temp,0,sizeof(temp));
						ConvertToColorString((BYTE*)temp,right,2047);

						int len = strlen(temp);
						if(len>1023)len=1023;
						if (!(*pInfo)){
							*pInfo = new wchar_t[len+1];
							if( fLocalizationSupport>1 ) GB2GBK(temp);
							MyMultiByteToWideChar(CP_ACP, 0, temp, -1,*pInfo , 0x400);

						}
					}

					fWaitEnd = FALSE;
					fWaitCmd = TRUE;
				}else{
					p1 = line;
					if( *p1=='&' && *(p1+1) && isdigit(*(p1+1)) ){
						p1++;
						int cubeidx = strtoul(p1,0,0);
						if (cubeidx>19)cubeidx=19;
						WideCharToMultiByte(CP_ACP,NULL,wszCubeMain[cubeidx],-1,line,0x400,NULL,NULL);
					}else if(*p1=='@' && *(p1+1) && isdigit(*(p1+1)) ){
						p1++;
						int runeidx = strtoul(p1,0,0);
						if (runeidx>169)runeidx=169;
						RuneWordTxt *pRuneWordTxt = d2common_GetRuneWordTxt(runeidx);
						if (pRuneWordTxt){
							wchar_t temp1[1024];
							wchar_t temp2[512];
							memset(temp1,0,sizeof(temp1));
							memset(temp2,0,sizeof(temp2));
							wchar_t *pwt = temp2;
							wcscpy(temp2,d2lang_GetLocaleText(pRuneWordTxt->wLocaleTxtNo));
							int j=0;
							while(temp2[j]){
								if ( temp2[j] == L'\n'||temp2[j] == L'\r') pwt = &temp2[j+1];
								j++;
							}
							wsprintfW(temp1+wcslen(temp1),L"%d " ,pRuneWordTxt->runeTxtId[0]-609 );
							for(int x=1 ;x<6;x++){
								if ((int)(pRuneWordTxt->runeTxtId[x])>0){
									wsprintfW(temp1+wcslen(temp1),L"+ %d ",pRuneWordTxt->runeTxtId[x]-609);
								}
							}
							wsprintfW(temp1+wcslen(temp1),L"\n%s:",pwt);

							WideCharToMultiByte(CP_ACP,NULL,temp1,-1,line,0x400,NULL,NULL);
						}
					}else{
						while(*p1){
							if (*p1==13||*p1==10){
								*p1 = 0;//取消所有回车换行
							}
							p1++;
						}
					}
					int len = strlen(line);
					if (linecount<40 && len>0){
						strncpy(szInfo[linecount++] ,line ,len+1 );
					}else if(linecount>=40){
						fWaitEnd = FALSE;
						fWaitCmd = TRUE;
						fWaitStart = FALSE;
						wchar_t errtemp[512];
						if ( dwGameLng ){
							wsprintfW(errtemp,L"加载物品信息出错!行数过多,跳过此命令 ,行号: %d", lineno);
						}else{
							wsprintfW(errtemp,L"Load ItemInfo Error!Too Many Lines!Skipped! LineNo: %d", lineno);
						}
						d2client_ShowGameMessage(errtemp , 1);						
					}
				}

			}
		} __except(EXCEPTION_EXECUTE_HANDLER) {
			gameMessage("Load ItemInfo Error!" );
		}
	}
	
	fclose(fp);
	return 0;
}

void UnLoadExtInfo(){
	if ( fExtInfoLoad ){
		for (int i=0;i<3000;i++){
			for(int j=0;j<5;j++){
				if( pwszNormalInfo[i][j] ) {
					delete[] pwszNormalInfo[i][j];
				}
			}
		}

		for( int i=0;i<500;i++){
			if( pwszUniqueInfo[i] ) {
				delete[] pwszUniqueInfo[i];
			}
		}

		for( int i=0;i<200;i++){
			if( pwszSetInfo[i] ) {
				delete[] pwszSetInfo[i];
			}
		}

		for( int i=0;i<170;i++){
			if( pwszRuneWordInfo[i] ) {
				delete[] pwszRuneWordInfo[i];
			}
		}
		for( int i=0;i<40;i++){
			if( wszRuneInfo[i] ) {
				delete[] wszRuneInfo[i];
			}
		}
		for( int i=0;i<20;i++){
			if( wszCubeMain[i] ) {
				delete[] wszCubeMain[i];
			}
		}
		fExtInfoLoad = FALSE;
	}
}


void SetExtInfoPos(int xPos , int yPos , int txtWidth , int txtHeight){
	if ( tItemExtInfo.isOn == 0 )return;
	xShowPos = xPos;
	yShowPos = yPos;

	if ( xPos > (*d2client_pScreenWidth)/2){
		if ( xPos + (int)txtWidth/2 > *d2client_pScreenWidth ){
			xShowPos =  *d2client_pScreenWidth - txtWidth -15 ;
		}else{
			xShowPos = xPos -  txtWidth/2 -10;
		}
		fLeft = TRUE;
	}else{
		if ( xPos - (int)txtWidth/2 < 0  ){
			xShowPos = txtWidth + 20;
		}else{
			xShowPos = xPos + txtWidth/2 +20;
		}
		fLeft = FALSE;
	}
	if ( yPos > *d2client_pScreenHeight ){
		yShowPos = *d2client_pScreenHeight - txtHeight +10;
	}else{
		yShowPos = yPos - txtHeight +10;
	}

}
void ShowItemExtInfo(){

	if (tItemExtInfo.isOn==0)return;
	UnitAny *pUnit = *d2client_pCurrentViewItem;
	if (!pUnit) return ;
	int itemIndex = GetItemIndex(pUnit->dwTxtFileNo);
	DWORD dwQuality = (pUnit->pItemData->dwQuality-1)&7;
	ItemTxt *pItemTxt = d2common_GetItemTxt( pUnit->dwTxtFileNo );
	wchar_t *pDesc = NULL;
	switch(dwQuality){
		case 0: //破碎:
		case 1://普通
		case 2://超强
			{
				if ( pItemTxt->nType == 74 ){//符文
					DWORD dwRuneNo = (pItemTxt->szCode[1]-'0')*10+(pItemTxt->szCode[2]-'0')-1;
					if ( dwRuneNo < 40 ) pDesc = wszRuneInfo[dwRuneNo];
				}else if ( d2common_CheckItemFlag(pUnit, ITEMFLAG_RUNEWORD, 0, "?") ){
					pDesc = pwszRuneWordInfo[d2common_GetRuneWordTxtIndex( pUnit->pItemData->wMagicPrefix[0] )];
				}else{
					if ( d2common_CheckItemFlag(pUnit, ITEMFLAG_ETHEREAL, 0, "?") ){
						pDesc = pwszNormalInfo[itemIndex][1];
					}else{
						pDesc = pwszNormalInfo[itemIndex][0];
					}
				}
			}
			break;
		case 3://蓝色
			{
				if ( itemIndex>=1112 && itemIndex<=1114 ) itemIndex = 1115;
				if ( (itemIndex>=286 && itemIndex<=290) || (itemIndex>=276 && itemIndex<=280) || (itemIndex>=296 && itemIndex<=300)){
					itemIndex= 300;
				}
				pDesc = pwszNormalInfo[itemIndex][2];
			}
			break;
		case 4://绿色
			pDesc = pwszSetInfo[d2common_GetItemFileIndex(pUnit)];
			break;
		case 5://黄金
			{
				if ( itemIndex>=1112 && itemIndex<=1114 ) itemIndex = 1115;
				if ( (itemIndex>=286 && itemIndex<=290) || (itemIndex>=276 && itemIndex<=280) || (itemIndex>=296 && itemIndex<=300)){
					itemIndex= 300;
				}
				pDesc = pwszNormalInfo[itemIndex][3];
			}
			break;
		case 6://暗金
			pDesc = pwszUniqueInfo[d2common_GetItemFileIndex(pUnit)];
			break;
		case 7://橙色
			pDesc = pwszNormalInfo[itemIndex][4];
			break;
	}

	if ( pDesc ){
		int dwWidth, dwHeight;
		d2win_GetTextAreaSize(pDesc, &dwWidth, &dwHeight);
		int xPos = xShowPos;
		int yPos = yShowPos;
		if ( fLeft ){
			xPos = xPos - dwWidth;
		}
		if(yPos+(int)dwHeight+7 > (*d2client_pScreenHeight-80)){
			yPos = *d2client_pScreenHeight-(int)dwHeight-80;
		}
		d2gfx_DrawRectangle( xPos-10 ,yPos ,xPos+dwWidth+10 ,yPos+dwHeight+7 ,0 ,2 );
		DrawD2Text(1 ,pDesc , xPos , yPos+dwHeight , nDefaultExtInfoColour , 0);
	}

}

void __declspec(naked) DrawHoverPatch_ASM()
{
	__asm {
		call d2win_DrawHover;
		call ShowItemExtInfo
		ret;
	}
}

