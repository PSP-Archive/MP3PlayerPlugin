#ifndef __MP3PP_H__
#define __MP3PP_H__

// header
#include <pspkernel.h>

// flag
#define		NEXT_MUSIC			4
#define		PREV_MUSIC			3
#define		DONE_MUSIC			2
#define		PLAYING_MUSIC		1
#define		PAUSE_MUSIC			0

// mode
#define		NORMAL_MODE			0
#define		LOOP_MODE			1
#define		RANDOM_MODE			2

// volume(1`100)
#define		MAX_VOLUME			32768
#define		MINI_VOLUME			327.68

// define
#define MAKE_CALL_FUNC(f) (0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF));
#define MAKE_JUMP(a, f) _sw(0x08000000 | (((u32)(f) & 0x0FFFFFFC) >> 2), a);
#define MAKE_CALL(a, f) _sw(0x0C000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a);
#define REDIRECT_FUNCTION(a, f) _sw(0x08000000 | (((u32)(f) >> 2) & 0x03FFFFFF), a); _sw(0, a + 4);

// ISODriverName
const char *drvname[] =
{
	"M33GalaxyController",
	"PRO_Inferno_Driver",
	"pspMarch33_Driver",
	"NP9660_Controller",
	"umd_driver",
	"PROGalaxyController",
};

#endif

