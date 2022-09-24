#ifndef __DRAW_H__
#define __DRAW_H__

// header
#include <psptypes.h>

// define
#define RGB(r, g, b)			 ((r) | ((g) << 8) | ((b) << 16))
#define RGB_8888(r, g, b)	(u32)((r) | ((g) << 8) | ((b) << 16))
#define RGB_565(r, g, b)	(u16)((r) | ((g) << 5) | ((b) << 11))
#define RGB_5551(r, g, b)	(u16)((r) | ((g) << 5) | ((b) << 10))
#define RGB_4444(r, g, b)	(u16)((r) | ((g) << 4) | ((b) <<  8))

// struct
typedef struct
{
	int width;
	int height;
	int linewidth;
	int pixelformat;
	u32 *buffer;
} CANVAS;

// function
int GetCanvas(void);
void FixChar(char *buf);

int DrawSmallFont(int sx, int sy, u32 fg, u32 bg, char *str);
int DrawSmallFontPrintf(int x, int y, u32 fg, u32 bg, const char *format, ...);
void FillRect(int x, int y, int width, int height, u32 fill, u32 border);

#endif

