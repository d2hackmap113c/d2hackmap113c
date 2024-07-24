#include "stdafx.h"

#ifdef MODULE_MONSTERDEATH


/*bossÀ¿Õˆ∂Øª≠*/
void __declspec(naked) MonsterDeathPatch_ASM()
{
	__asm {
		cmp [afMonsterDeathAnims+eax],0
		je noanim
		cmp eax,0x100 //original code
		ret
noanim:
		pop eax // discard return address
		pop esi // restore esi, check prolog of this function carefully
		ret 4 // return to upper level function
	}
}



#endif