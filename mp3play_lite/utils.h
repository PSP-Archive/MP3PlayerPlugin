#ifndef __UTILS_H____
#define __UTILS_H____

// define
#define REDIRECT_FUNCTION(a, f) _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a); _sw(0, a + 4);

// struct
typedef struct
{
	const char *name;
	u16 version;
	u16 attribute;
	u8 entlen;
	u8 varcount;
	u16 funccount;
	u32 *fnids;
	u32 *funcs;
	u32 *vnids;
	u32 *vars;
} PspModuleImport;

// function
u32 FindImport(const char *szMod, const char *szLib, u32 nid);

#endif

