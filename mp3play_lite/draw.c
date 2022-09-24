// draw

// header
#include <pspkernel.h>
#include <pspdisplay.h>
#include <string.h>
#include "draw.h"
#include "sysclib.h"

// global
CANVAS canvas;

// font

// x = WHITE
// - = BLACK

char small_font[][10] =
{
	// 0x00
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x01
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x02
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x03
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x04
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x05
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x06
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x07
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x08
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x09
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x0A
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x0B
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x0C
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x0D
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x0E
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x0F
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x10
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x11
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x12
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x13
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x14
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x15
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x16
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x17
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x18
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x19
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x1A
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x1B
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x1C
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x1D
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x1E
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x1F
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x20
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// (!) 0x21
	"  ---  ",
	"  -x-  ",
	"  -x-  ",
	"  -x-  ",
	"  ---  ",
	"  -x-  ",
	"  ---  ",
	// (") 0x22
	" ----- ",
	" -x-x- ",
	" ----- ",
	"       ",
	"       ",
	"       ",
	"       ",
	// (#) 0x23
	" ----- ",
	"--x-x--",
	"-xxxxx-",
	"--x-x--",
	"-xxxxx-",
	"--x-x--",
	" ----- ",
	// ($) 0x24
	" --x---",
	"--xxxx-",
	"-x x---",
	"--xxx--",
	"---x-x-",
	"-xxxx--",
	"---x-- ",
	// (%) 0x25
	"       ",
	"------ ",
	"-x--x- ",
	" --x-- ",
	"--x--- ",
	"-x--x- ",
	"------ ",
	// (&) 0x26
	" ------",
	"--xxxx-",
	"--x----",
	"--xx---",
	"-x--xx-",
	"-xxx-x-",
	"-------",
	// (') 0x27
	"  ---  ",
	"  -x-  ",
	"  ---  ",
	"       ",
	"       ",
	"       ",
	"       ",
	// (() 0x28
	"  ---  ",
	" --x-  ",
	" -x--  ",
	" -x-   ",
	" -x--  ",
	" --x-  ",
	"  ---  ",
	// ()) 0x29
	"  ---  ",
	"  -x-- ",
	"  --x- ",
	"   -x- ",
	"  --x- ",
	"  -x-- ",
	"  ---  ",
	// (*) 0x2A
	"-------",
	"-x-x-x-",
	"--xxx--",
	"-xxxxx-",
	"--xxx--",
	"-x-x-x-",
	"-------",
	// (+) 0x2B
	"  ---  ",
	"  -x-  ",
	"---x---",
	"-xxxxx-",
	"---x---",
	"  -x-  ",
	"  ---  ",
	// (,) 0x2C
	"       ",
	"       ",
	"       ",
	"       ",
	"  ---  ",
	" --x-  ",
	" -x--  ",
	// (-) 0x2D
	"       ",
	"       ",
	"-------",
	"-xxxxx-",
	"-------",
	"       ",
	"       ",
	// (.) 0x2E
	"       ",
	"       ",
	"       ",
	"       ",
	"  ---  ",
	"  -x-  ",
	"  ---  ",
	// (/) 0x2F
	"    ---",
	"   --x-",
	"  --x--",
	" --x-- ",
	"--x--  ",
	"-x--   ",
	"---    ",
	// (0) 0x30
	"-------",
    "-xxxxx-",
    "-x---x-",
    "-x- -x-",
    "-x---x-",
	"-xxxxx-",
    "-------",
	// (1) 0x31
	"    ---",
	"    -x-",
	"    -x-",
	"    -x-",
	"    -x-",
	"    -x-",
	"    ---",
	// (2) 0x32
	"-------",
	"-xxxxx-",
	"-----x-",
	"-xxxxx-",
	"-x-----",
	"-xxxxx-",
	"-------",
	// (3) 0x33
	"-------",
	"-xxxxx-",
	"-----x-",
	"-xxxxx-",
	"-----x-",
	"-xxxxx-",
	"-------",
	// (4) 0x34
	"--- ---",
	"-x- -x-",
	"-x---x-",
	"-xxxxx-",
	"-----x-",
	"    -x-",
	"    ---",
	// (5) 0x35
	"-------",
	"-xxxxx-",
	"-x-----",
	"-xxxxx-",
	"-----x-",
	"-xxxxx-",
	"-------",
	// (6) 0x36
	"-------",
	"-xxxxx-",
	"-x-----",
	"-xxxxx-",
	"-x---x-",
	"-xxxxx-",
	"-------",
	// (7) 0x37
	"-------",
	"-xxxxx-",
	"-----x-",
	"    -x-",
	"    -x-",
	"    -x-",
	"    ---",
	// (8) 0x38
	"-------",
	"-xxxxx-",
	"-x---x-",
	"-xxxxx-",
	"-x---x-",
	"-xxxxx-",
	"-------",
	// (9) 0x39
	"-------",
	"-xxxxx-",
	"-x---x-",
	"-xxxxx-",
	"-----x-",
	"-xxxxx-",
	"-------",
	// (:) 0x3A
	"       ",
	"  ---  ",
	"  -x-  ",
	"  ---  ",
	"  -x-  ",
	"  ---  ",
	"       ",
	// (;) 0x3B
	"       ",
	"  ---  ",
	"  -x-  ",
	"  ---  ",
	" --x-  ",
	" -x--  ",
	" ---   ",
	// (<) 0x3C
	"   --- ",
	"  --x- ",
	" --x-- ",
	" -x--  ",
	" --x-- ",
	"  --x- ",
	"   --- ",
	// (=) 0x3D
	"       ",
	"-------",
	"-xxxxx-",
	"-------",
	"-xxxxx-",
	"-------",
	"       ",
	// (>) 0x3E
	" ---   ",
	" -x--  ",
	" --x-- ",
	"  --x- ",
	" --x-- ",
	" -x--  ",
	" ---   ",
	// (?) 0x3F
	"-------",
	"-xxxxx-",
	"----xx-",
	"  -x---",
	"  ---  ",
	"  -x-  ",
	"  ---  ",
	// (@) 0x40
	" ----- ",
	"--xxx--",
	"-x-x-x-",
	"-x-xxx-",
	"-x-----",
	"--xxx--",
	" ----- ",
	// (A) 0x41
	"-------",
	"-xxxxx-",
	"-x---x-",
	"-xxxxx-",
	"-x---x-",
	"-x- -x-",
	"--- ---",
	// (B) 0x42
	"------ ",
	"-xxxx--",
    "-x---x-",
    "-xxxx--",
    "-x---x-",
	"-xxxx--",
    "------ ",
	// (C) 0x43
	"-------",
	"-xxxxx-",
    "-x-----",
    "-x-    ",
    "-x-----",
	"-xxxxx-",
    "-------",
	// (D) 0x44
	"------ ",
	"-xxxx--",
    "-x---x-",
    "-x- -x-",
    "-x---x-",
	"-xxxx--",
    "------ ",
	// (E) 0x45
	"-------",
	"-xxxxx-",
    "-x-----",
    "-xxxxx-",
    "-x-----",
	"-xxxxx-",
    "-------",
	// (F) 0x46
	"-------",
	"-xxxxx-",
    "-x-----",
    "-xxxxx-",
    "-x-----",
	"-x-    ",
    "---    ",
	// (G) 0x47
	"-------",
	"-xxxxx-",
    "-x-----",
    "-x-xxx-",
    "-x---x-",
	"-xxxxx-",
    "-------",
	// (H) 0x48
	"--- ---",
	"-x- -x-",
    "-x---x-",
    "-xxxxx-",
    "-x---x-",
	"-x- -x-",
    "--- ---",
	// (I) 0x49
	"  ---  ",
	"  -x-  ",
    "  -x-  ",
    "  -x-  ",
    "  -x-  ",
	"  -x-  ",
    "  ---  ",
	// (J) 0x4A
	"-------",
	"-xxxxx-",
    "-----x-",
    "--- -x-",
    "-x---x-",
	"-xxxxx-",
    "-------",
	// (K) 0x4B
	"--- ---",
	"-x---x-",
    "-x--x--",
    "-xxx-- ",
    "-x--x--",
	"-x---x-",
    "--- ---",
	// (L) 0x4C
	"---    ",
	"-x-    ",
    "-x-    ",
    "-x-    ",
    "-x-----",
	"-xxxxx-",
    "-------",
	// (M) 0x4D
	"-------",
	"-xxxxx-",
    "-x-x-x-",
    "-x-x-x-",
    "-x-x-x-",
	"-x-x-x-",
    "-------",
	// (N) 0x4E
	"--- ---",
	"-x---x-",
    "-xx--x-",
    "-x-x-x-",
    "-x--xx-",
	"-x---x-",
    "--- ---",
	// (O) 0x4F
	"-------",
	"-xxxxx-",
    "-x---x-",
    "-x- -x-",
    "-x---x-",
	"-xxxxx-",
    "-------",
	// (P) 0x50
	"-------",
	"-xxxxx-",
    "-x---x-",
    "-xxxxx-",
    "-x-----",
	"-x-    ",
    "---    ",
	// (Q) 0x51
	"-------",
	"-xxxxx-",
    "-x---x-",
    "-x-x-x-",
    "-x--xx-",
	"-xxxxx-",
    "-------",
	// (R) 0x52
	"------ ",
	"-xxxx--",
    "-x---x-",
    "-xxxx--",
    "-x---x-",
	"-x- -x-",
    "--- ---",
	// (S) 0x53
	"-------",
	"-xxxxx-",
    "-x-----",
    "-xxxxx-",
    "-----x-",
	"-xxxxx-",
    "-------",
	// (T) 0x54
	"-------",
	"-xxxxx-",
    "---x---",
    "  -x-  ",
    "  -x-  ",
	"  -x-  ",
    "  ---  ",
	// (U) 0x55
	"--- ---",
	"-x- -x-",
    "-x- -x-",
    "-x- -x-",
    "-x---x-",
	"-xxxxx-",
    "-------",
	// (V) 0x56
	"--- ---",
	"-x- -x-",
    "-x- -x-",
    "-x---x-",
    "--x-x--",
	" --x-- ",
    "  ---  ",
	// (W) 0x57
	"-------",
	"-x-x-x-",
    "-x-x-x-",
    "-x-x-x-",
    "-x-x-x-",
	"-xxxxx-",
    "-------",
	// (X) 0x58
	"--- ---",
	"-x- -x-",
    "-x---x-",
    "--xxx--",
    "-x---x-",
	"-x- -x-",
    "--- ---",
	// (Y) 0x59
	"--- ---",
	"-x- -x-",
    "-x---x-",
    "--xxx--",
    " --x-- ",
	"  -x-  ",
    "  ---  ",
	// (Z) 0x5A
	"-------",
	"-xxxxx-",
    "----x--",
    " --x-- ",
    "--x----",
	"-xxxxx-",
    "-------",
	// ([) 0x5B
	" ----- ",
	" -xxx- ",
    " -x--- ",
    " -x-   ",
    " -x--- ",
	" -xxx- ",
    " ----- ",
	// (\) 0x5C
	"---    ",
	"-x--   ",
    "--x--  ",
    " --x-- ",
    "  --x--",
	"   --x-",
    "    ---",
	// (]) 0x5D
	" ----- ",
	" -xxx- ",
    " ---x- ",
    "  --x- ",
    " ---x- ",
	" -xxx- ",
    " ----- ",
	// (^) 0x5E
	"  ---  ",
	" --x-- ",
    " -x-x- ",
    " ----- ",
    "       ",
	"       ",
    "       ",
	// (_) 0x5F
	"       ",
	"       ",
    "       ",
    "       ",
    "-------",
	"-xxxxx-",
    "-------",
	// (') 0x60
	"  ---  ",
	"  -x-  ",
	"  ---  ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x61
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x62
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x63
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x64
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x65
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x66
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x67
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x68
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x69
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x6A
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x6B
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x6C
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x6D
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x6E
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x6F
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x70
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x71
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x72
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x73
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x74
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x75
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x76
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x77
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x78
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x79
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x7A
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x7B
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x7C
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x7D
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x7E
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	// 0x7F
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
	"       ",
};

// function
static __inline u16 convert_8888_to_565(u32 color)
{
	int r, g, b;

	b = (color >> 19) & 0x1F;
	g = (color >> 10) & 0x3F;
	r = (color >>  3) & 0x1F;

	return r | (g << 5) | (b << 11);
}

static __inline u16 convert_8888_to_5551(u32 color)
{
	int r, g, b, a;

	a = (color >> 24) ? 0x8000 : 0;
	b = (color >> 19) & 0x1F;
	g = (color >> 11) & 0x1F;
	r = (color >>  3) & 0x1F;

	return a | r | (g << 5) | (b << 10);
}

static __inline u16 convert_8888_to_4444(u32 color)
{
	int r, g, b, a;

	a = (color >> 28) & 0xF;
	b = (color >> 20) & 0xF;
	g = (color >> 12) & 0xF;
	r = (color >>  4) & 0xF;

	return (a << 12) | r | (g << 4) | (g << 8);
}

static __inline u16 convert_u32_to_u16(u32 color, int pixelformat)
{
	switch(pixelformat)
	{
		case PSP_DISPLAY_PIXEL_FORMAT_565:		return convert_8888_to_565(color);
		case PSP_DISPLAY_PIXEL_FORMAT_5551:		return convert_8888_to_5551(color);
		case PSP_DISPLAY_PIXEL_FORMAT_4444:		return convert_8888_to_4444(color);
	}

	return convert_8888_to_565(color);
}

int GetCanvas(void)
{
	int unk = 0;

	sceDisplayGetMode(&unk, &canvas.width, &canvas.height);
	sceDisplayGetFrameBuf((void *)&canvas.buffer, &canvas.linewidth, &canvas.pixelformat, 0);

	if(canvas.width > 0 && canvas.height > 0 && canvas.linewidth > 0 && canvas.buffer != NULL) return 1;
	return 0;
}

void FixChar(char *buf)
{
	int i;
	for(i = 0; buf[i]; i++)
	{
		switch(buf[i])
		{
			case 'a':	{ buf[i] = 'A'; break; }
			case 'b':	{ buf[i] = 'B'; break; }
			case 'c':	{ buf[i] = 'C'; break; }
			case 'd':	{ buf[i] = 'D'; break; }
			case 'e':	{ buf[i] = 'E'; break; }
			case 'f':	{ buf[i] = 'F'; break; }
			case 'g':	{ buf[i] = 'G'; break; }
			case 'h':	{ buf[i] = 'H'; break; }
			case 'i':	{ buf[i] = 'I'; break; }
			case 'j':	{ buf[i] = 'J'; break; }
			case 'k':	{ buf[i] = 'K'; break; }
			case 'l':	{ buf[i] = 'L'; break; }
			case 'm':	{ buf[i] = 'M'; break; }
			case 'n':	{ buf[i] = 'N'; break; }
			case 'o':	{ buf[i] = 'O'; break; }
			case 'p':	{ buf[i] = 'P'; break; }
			case 'q':	{ buf[i] = 'Q'; break; }
			case 'r':	{ buf[i] = 'R'; break; }
			case 's':	{ buf[i] = 'S'; break; }
			case 't':	{ buf[i] = 'T'; break; }
			case 'u':	{ buf[i] = 'U'; break; }
			case 'v':	{ buf[i] = 'V'; break; }
			case 'w':	{ buf[i] = 'W'; break; }
			case 'x':	{ buf[i] = 'X'; break; }
			case 'y':	{ buf[i] = 'Y'; break; }
			case 'z':	{ buf[i] = 'Z'; break; }
			default:	break;
		}
	}

	return;
}

int DrawSmallFont(int sx, int sy, u32 fg, u32 bg, char *str)
{
	u32 *vram32, *vram32_ptr;
	u16 *vram16, *vram16_ptr;
	int c, i, j, ch, cnt;
	int x, y;
	u16 fg16, bg16;
	char *font;

	x = sx;
	y = sy;

	for(c = 0, cnt = 0; str[c]; c++, cnt++)
	{
		if(x < 0 || y < 0 || (x + 8) > canvas.width || (y + 8) > canvas.height) break;

		ch = (int)str[c];

		if(ch >= 1 && ch <= 127)
		{
			switch(canvas.pixelformat)
			{
				case PSP_DISPLAY_PIXEL_FORMAT_8888:
				{
					vram32 = canvas.buffer + x + y * canvas.linewidth;

					for(i = 0; i < 7; i++, vram32 += canvas.linewidth)
					{
						vram32_ptr = vram32;
						font = small_font[((int)ch) * 7 + i];

						for(j = 0; j < 7; j++)
						{
							switch(font[j])
							{
								case 'x': { *vram32_ptr++ = fg;	break; }
								case '-': { *vram32_ptr++ = bg;	break; }
								default:  { vram32_ptr++; 		break; }
							}
						}
					}
					break;
				}

				case PSP_DISPLAY_PIXEL_FORMAT_565:
				case PSP_DISPLAY_PIXEL_FORMAT_5551:
				case PSP_DISPLAY_PIXEL_FORMAT_4444:
				{
					vram16 = (u16 *)canvas.buffer + x + y * canvas.linewidth;

					fg16 = convert_u32_to_u16(fg, canvas.pixelformat);
					bg16 = convert_u32_to_u16(bg, canvas.pixelformat);

					for(i = 0; i < 7; i++, vram16 += canvas.linewidth)
					{
						vram16_ptr = vram16;
						font = small_font[((int)ch) * 7 + i];

						for(j = 0; j < 7; j++)
						{
							switch(font[j])
							{
								case 'x': { *vram16_ptr++ = fg16;	break; }
								case '-': { *vram16_ptr++ = bg16;	break; }
								default:  { vram16_ptr++;			break; }
							}
						}
					}
					break;
				}
			}
		}
		x += 6;
	}
	return cnt;
}

int DrawSmallFontPrintf(int x, int y, u32 fg, u32 bg, const char *format, ...)
{
	char str[256];
	va_list ap;

	va_start(ap, format);
	vsnprintf(str, 256, format, ap);
	va_end(ap);

	return DrawSmallFont(x, y, fg, bg, str);
}

void FillRect(int x, int y, int width, int height, u32 fill, u32 border)
{
	u32 *vram32, *vram32_ptr;
	u16 *vram16, *vram16_ptr;
	int i, j;
	u16 fill16 = convert_u32_to_u16(fill, canvas.pixelformat);
	u16 border16 = convert_u32_to_u16(border, canvas.pixelformat);

	switch(canvas.pixelformat)
	{
		case PSP_DISPLAY_PIXEL_FORMAT_8888:
		{
			vram32 = canvas.buffer + x + y * canvas.linewidth;
			vram32_ptr = vram32;

			for(i = 0; i < width; i++, vram32_ptr++)
				*vram32_ptr = border;

			vram32 += canvas.linewidth;

			for(j = 0; j < (height - 2); j++)
			{
				vram32_ptr = vram32;
				*vram32_ptr = border;
				vram32_ptr++;

				for(i = 0; i < (width - 2); i++, vram32_ptr++)
					*vram32_ptr = fill;

				*vram32_ptr = border;
				vram32 += canvas.linewidth;
			}

			vram32_ptr = vram32;

			for(i = 0; i < width; i++, vram32_ptr++)
				*vram32_ptr = border;

			break;
		}

		case PSP_DISPLAY_PIXEL_FORMAT_565:
		case PSP_DISPLAY_PIXEL_FORMAT_5551:
		case PSP_DISPLAY_PIXEL_FORMAT_4444:
		{
			vram16 = (u16 *)canvas.buffer + x + y * canvas.linewidth;
			vram16_ptr = vram16;

			for(i = 0; i < width; i++, vram16_ptr++)
				*vram16_ptr = border16;

			vram16 += canvas.linewidth;
			for(j = 0; j < (height - 2); j++)
			{
				vram16_ptr = vram16;
				*vram16_ptr = border16;
				vram16_ptr++;

				for(i = 0; i < (width - 2); i++, vram16_ptr++)
					*vram16_ptr = fill16;

				*vram16_ptr = border16;
				vram16 += canvas.linewidth;
			}

			vram16_ptr = vram16;

			for(i = 0; i < width; i++, vram16_ptr++)
				*vram16_ptr = border16;

			break;
		}
	}
	return;
}

