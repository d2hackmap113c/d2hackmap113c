#ifndef INSTALL_H
#define INSTALL_H



BOOL Install();
void Uninstall();
BOOL LoadConfig();
int ReloadConfig();
void ShowWarningMessage();
BOOL InstallD2Patchs(Patch_t* pPatchStart, Patch_t* pPatchEnd);
void RemoveD2Patchs(Patch_t* pPatchStart, Patch_t* pPatchEnd);
void InstallPatchAfterLoad_ASM();
void InitCellFiles();
void DeleteCellFiles();

#endif
