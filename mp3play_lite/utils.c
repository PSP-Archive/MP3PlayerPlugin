// utils

// header
#include <pspkernel.h>
#include <string.h>
#include "utils.h"

// function
u32 FindImport(const char *szMod, const char *szLib, u32 nid)
{
	PspModuleImport *imp = NULL;
	SceModule2 *mod = NULL;
	void *stub_top = NULL;
	int stub_size = 0;
	int count = 0;
	int i = 0;

	mod = sceKernelFindModuleByName(szMod);

	if(mod == NULL)
		return 0;

	stub_top = mod->stub_top;
	stub_size = mod->stub_size;

	while(i < stub_size)
	{
		imp = (PspModuleImport *)(stub_top + i);

		if(imp->name != NULL && (strcmp(imp->name, szLib) == 0))
		{
			for(count = 0; count < imp->funccount; count++)
			{
				if(imp->fnids[count] == nid)
				{
					return (u32)&imp->funcs[count * 2];
				}
			}
		}

		i += (imp->entlen * 4);
	}

	return 0;
}

