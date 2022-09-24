// LibMenu 0.1 から拝借しました

// ヘッダー
#include "blit.h"
#include "sysclib.h"
#include "fonts/font_cg.c"
#include "fonts/misaki8x8.c"

// 定義
#define		INVALID_FONT					('?' * CHAR_HEIGHT)
#define		INVALID_SJIS_INDEX				(0xFFFF)

// extern変数
extern vram_buffers vb;

// アルファブレンディング関数
static __inline unsigned int color_8888_to_5650(unsigned int color)
{
	return (((color >> 3) & 0x1F) | ((color >> 10) & 0x3F) << 5 | ((color >> 19) & 0x1F) << 11);
}

static __inline unsigned int color_8888_to_5551(unsigned int color)
{
	return (((color >> 3) & 0x1F) | ((color >> 11) & 0x1F) << 5 | ((color >> 19) & 0x1F) << 10 | ((color >> 24) ? 1 : 0) << 15);
}

static __inline unsigned int color_8888_to_4444(unsigned int color)
{
	return (((color >> 4) & 0xF) | ((color >> 12) & 0xF) << 4 | ((color >> 20) & 0xF) << 8 | ((color >> 28) & 0xF) << 12);
}

// 関数
static __inline u16 convert_u32_to_u16(u32 color)
{
	switch(vb.format)
	{
		case PSP_DISPLAY_PIXEL_FORMAT_565:		return color_8888_to_5650(color);
		case PSP_DISPLAY_PIXEL_FORMAT_5551:		return color_8888_to_5551(color);
		case PSP_DISPLAY_PIXEL_FORMAT_4444:		return color_8888_to_4444(color);
	}

	return color_8888_to_5650(color);
}

static __inline int buf_addr(int x, int y)
{
	return ((x + y * vb.linewidth) * vb.pixelsize);
}

void Set_Buffers(int sync)
{
	int unk;

	sceDisplayGetMode(&unk, &vb.width, &vb.height);
	sceDisplayGetFrameBuf(&vb.vram, &vb.linewidth, &vb.format, sync);

	if(vb.vram && vb.linewidth)
	{
		vb.vram = (void *)((int)vb.vram | VRAM_ADDR_TOP);

		vb.pixelsize	= (vb.format == PSP_DISPLAY_PIXEL_FORMAT_8888 ? 4 : 2);
		vb.linesize		= vb.linewidth * vb.pixelsize;
		vb.framesize	= vb.linesize * vb.height;
	}
}

void Clear_Buffers()
{
	memset(vb.vram, 0, vb.framesize);
}

inline void FillRect(int sx, int sy, int ex, int ey, u32 color)
{
	int start = 0;
	int w, h, x;
	u16 color16;

	// 横幅と高さを取得
	w = (sx > ex) ? (sx - ex) : (ex - sx);
	h = (sy > ey) ? (sy - ey) : (ey - sy);

	// VRAM位置取得
	start = (sx > ex) ? buf_addr(ex, ey) : buf_addr(sx, sy);

	// 16bitカラー取得
	color16 = convert_u32_to_u16(color);

	// 描画
	for(; h; h--, start += vb.linewidth)
	{
		for(x = 0; x < w; x++)
		{
			if(vb.format == 3)	*(u32 *)(vb.vram + start + x) = color;
			else				*(u16 *)(vb.vram + start + x) = color16;
		}
	}

	return;
}

unsigned short __inline get_glyph_index_by_sjis(unsigned char sjis_hi, unsigned char sjis_lo)
{
	//SJIS の無効領域
	if((sjis_hi < 0x81 || sjis_hi > 0xfc || sjis_lo < 0x40 || sjis_lo > 0xfc)) return INVALID_SJIS_INDEX;

	unsigned char  kuten_hi = sjis_hi - 0x81;
	unsigned short kuten_lo = sjis_lo - 0x40;

	if(sjis_hi > 0x9F) kuten_hi -= 0x40;
	if(sjis_lo > 0x7E) kuten_lo--;

	kuten_hi <<= 1;
	if(kuten_lo > 0x5D) kuten_hi++;

	kuten_lo %= 0x5E;

	if((kuten_hi >= 8 && kuten_hi <= 11) || (kuten_hi >= 13 && kuten_hi <= 14) || (kuten_hi >= 83 && kuten_lo > 5) || (kuten_hi > 83))
	{
		//美咲フォントの未定義領域
		return INVALID_SJIS_INDEX;
	}
	else
	{
		/* 16区以降であれば、未定義の9～15区分を引く */
		if(kuten_hi > 7 && kuten_hi <=12)
		{
			//9,10,11,12,
			kuten_hi -= 4;
			//kuten_lo--;
		}
		else if(kuten_hi >= 15)
		{
			//9,10,11,12,14,15
			kuten_hi -= 6;
			//kuten_lo--;
		}
	}

	//対象文字が格納されているアドレスを求める
	//  = kuten_hi(区) * 1区の文字数(94文字) + kuten_lo * PB_CHAR_HEIGHT(文字高 = 8)
	return ((kuten_hi * 94 + kuten_lo) * CHAR_HEIGHT);
}

int blit_string(int x, int y, unsigned int fg, unsigned int bg, const char *str)
{
	int start_addr, put_addr;
	unsigned int i;
	const char *glyph;
	unsigned char glyph_line_data, chr_width_bytes;
	unsigned short glyph_x, glyph_y;
	u32 color;
	u16 color16;

	//VRAMの開始アドレスを取得
	start_addr = buf_addr(x, y);
	put_addr = start_addr;

	int cnt;
	for(i = 0, cnt = 0; str[i]; i++, cnt++)
	{
		//改行する
		if((unsigned char)str[i] == '\n')
		{
			start_addr += vb.linesize * CHAR_HEIGHT;
			put_addr = start_addr;
			continue;//即座に次の文字へ
		}

		/* グリフ取得 */
		if((unsigned char)str[i] < 0x80)
		{
			// 0x80 < アスキー文字
			glyph = &(font_cg[(unsigned char)str[i] * CHAR_HEIGHT]);
		}

		else if((unsigned char)str[i] >= 0xA1 && (unsigned char)str[i] <= 0xDF)
		{
			//半角カタカナ
			glyph = &(font_hankaku_kana[((unsigned char)(str[i] - 0xA1) * CHAR_HEIGHT)]);
		}

		else if(str[i + 1])
		{
			//（2バイト文字)
			unsigned int glyph_index = get_glyph_index_by_sjis(str[i], str[i + 1]);
			i++;

			if(glyph_index != INVALID_SJIS_INDEX)
			{
				glyph = &(font_sjis[glyph_index]);
			}
			else
			{
				continue;
			}
		}

		else
		{
			//該当なし => ?
			continue;
		}

		chr_width_bytes = CHAR_WIDTH * vb.pixelsize;

		for(glyph_y = 0; glyph_y < CHAR_HEIGHT; glyph_y++, put_addr += vb.linesize - chr_width_bytes)
		{
			glyph_line_data = glyph[glyph_y];

			for(glyph_x = 0; glyph_x < CHAR_WIDTH; glyph_x++, glyph_line_data <<= 1, put_addr += vb.pixelsize)
			{
				if(vb.format == PSP_DISPLAY_PIXEL_FORMAT_8888)
				{
					color = glyph_line_data & 0x80 ? fg : bg;
					*(u32 *)(vb.vram + put_addr) = color;
				}
				else
				{
					color16 = glyph_line_data & 0x80 ? convert_u32_to_u16(fg) : convert_u32_to_u16(bg);
					*(u16 *)(vb.vram + put_addr) = color16;
				}
			}
		}

		put_addr -= vb.linesize * CHAR_HEIGHT - chr_width_bytes;
	}

	return cnt;
}

int blit_printf(int x, int y, unsigned int fg, unsigned int bg, const char *format, ...)
{
	va_list ap;
	char str[256];

	va_start(ap, format);
	vsnprintf(str, 256, format, ap);
	va_end(ap);

	return blit_string(x, y, fg, bg, str);
}

