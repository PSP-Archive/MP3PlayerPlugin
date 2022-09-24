// blit

#ifndef BLIT_H__
#define BLIT_H__

// ヘッダー
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdisplay_kernel.h>
#include <pspsysclib.h>
#include <stdarg.h>
#include <string.h>

// 定義
#define		SCR_WIDTH		480
#define		SCR_HEIGHT		272
#define		SCR_BUF			512

#define		VRAM_ADDR_TOP	0x44000000
#define		VRAM_SIZE		0x22000

#define		CHAR_WIDTH		8
#define		CHAR_HEIGHT		8

// color
#define		WHITE			0xffFFFFFF
#define		BLACK			0xff000000
#define		RED				0xff0000FF
#define		GREEN			0xff00FF00
#define		BLUE			0xffFF0000

// 構造体
typedef struct
{
	int width;
	int height;
	int linewidth;
	int format;
	int pixelsize;
	int linesize;
	int framesize;
	void *vram;
} vram_buffers;

// 関数
void Set_Buffers(int sync);
void Clear_Buffers();
void FillRect(int sx, int sy, int ex, int ey, u32 color);

int blit_string(int x, int y, unsigned int fg, unsigned int bg, const char *str);
int blit_printf(int x, int y, unsigned int fg, unsigned int bg, const char *format, ...);

#endif

