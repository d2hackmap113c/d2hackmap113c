#ifndef D2_CALLSTUB_H
#define D2_CALLSTUB_H

void __fastcall d2client_SetAutomapParty(DWORD flag);
void __fastcall d2client_SetAutomapNames(DWORD flag);
DWORD __fastcall d2client_ChatInput(D2MSG *pmsg);
int __fastcall d2client_CheckUiStatus(int dwUiNo);
AutomapLayer* __fastcall d2client_InitAutomapLayer(int nLayerNo);
UniqueItemTxt* __fastcall d2common_GetUniqueItemTxt(int dwIdx);
ItemStatCostTxt* __fastcall d2common_GetItemStatCostTxt(int dwStatNo);
PropertiesTxt* __fastcall d2common_GetPropertiesTxt(int dwPropNo);
SetItemTxt* __fastcall d2common_GetSetItemTxt(int dwIdx);
ItemTypeTxt* __fastcall d2common_GetItemTypeTxt(int dwIdx);
RuneWordTxt* __fastcall d2common_GetRuneWordTxt(int dwIdx);
int __fastcall d2common_GetRuneWordTxtIndex(int dwFileNo);
CellFile* __fastcall d2client_LoadUiImage(char* szPath);

#endif
