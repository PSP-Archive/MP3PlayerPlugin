// KeyConfig

// header
#include <pspkernel.h>
#include <pspctrl.h>
#include <string.h>
#include <stdio.h>
#include "conf.h"

// prototype
int Check_EOF(SceUID fd);
u32 Conv_Key(const char *buf);
char *ch_token(char *str, const char *delim);
u32 Get_Key(const char *str);

// function
int Read_Line(SceUID fd, char *buf, int n)
{
	int res, count = 0;
	char c;

	if(fd < 0)
		return 0;

	do
	{
		res = sceIoRead(fd, &c, 1);

		if(c == '\r' || c == ' ')	continue;
		else if(c == '\n')			break;

		else
		{
			buf[count++] = c;
		}
	}
	while (res > 0 && count < n);

	buf[count] = '\0';
	return count;
}

int Check_EOF(SceUID fd)
{
	char c;

	if(sceIoRead(fd, &c, 1) == 1)
	{
		sceIoLseek(fd, -1, PSP_SEEK_CUR);
		return 0;
	}

	return 1;
}

u32 Conv_Key(const char *buf)
{
	if(strcmp(buf, "SELECT") == 0)			return PSP_CTRL_SELECT;
	else if(strcmp(buf, "START") == 0)		return PSP_CTRL_START;
	else if(strcmp(buf, "UP") == 0)			return PSP_CTRL_UP;
	else if(strcmp(buf, "RIGHT") == 0)		return PSP_CTRL_RIGHT;
	else if(strcmp(buf, "DOWN") == 0)		return PSP_CTRL_DOWN;
	else if(strcmp(buf, "LEFT") == 0)		return PSP_CTRL_LEFT;
	else if(strcmp(buf, "L_TRI") == 0)		return PSP_CTRL_LTRIGGER;
	else if(strcmp(buf, "R_TRI") == 0)		return PSP_CTRL_RTRIGGER;
	else if(strcmp(buf, "TRIANGLE") == 0)	return PSP_CTRL_TRIANGLE;
	else if(strcmp(buf, "CIRCLE") == 0)		return PSP_CTRL_CIRCLE;
	else if(strcmp(buf, "CROSS") == 0)		return PSP_CTRL_CROSS;
	else if(strcmp(buf, "SQUARE") == 0)		return PSP_CTRL_SQUARE;
	else if(strcmp(buf, "HOME") == 0)		return PSP_CTRL_HOME;
	else if(strcmp(buf, "HOLD") == 0)		return PSP_CTRL_HOLD;
	else if(strcmp(buf, "NOTE") == 0)		return PSP_CTRL_NOTE;
	else if(strcmp(buf, "SCREEN") == 0)		return PSP_CTRL_SCREEN;
	else if(strcmp(buf, "VOLUP") == 0)		return PSP_CTRL_VOLUP;
	else if(strcmp(buf, "VOLDOWN") == 0)	return PSP_CTRL_VOLDOWN;
	else return 0;
}

char *ch_token(char *str, const char *delim)
{
	char *ptr = strstr(str, delim);
	if(ptr == NULL) return NULL;

	*ptr = '\0';
	return ptr + strlen(delim);
}

u32 Get_Key(const char *str)
{
	u32 key = 0;
	char buf[256];
	char *ptr;

	strcpy(buf, str);

	do
	{
		ptr = ch_token(buf, "+");
		key |= Conv_Key(buf);

		if(ptr != NULL)
			strcpy(buf, ptr);
	}
	while (ptr != NULL);

	return key;
}

int Read_Conf(const char *path, Conf_Key *key)
{
	SceUID fd;
	char buf[256];
	char ms_path[256];
	char *ptr;

	strcpy(ms_path, path);

	ptr = strrchr(ms_path, '/');
	strcpy((ptr + 1), "mp3play.ini");

	// Default Setting
	strcpy(key->dirpath, "ef0:/MUSIC");
	key->Play_Pause_Key		= (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_SQUARE);
	key->Next_Key			= (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_RIGHT);
	key->Prev_Key			= (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_LEFT);
	key->Up_Key				= (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_UP);
	key->Down_Key			= (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_DOWN);
	key->Hold_Key			= (PSP_CTRL_LTRIGGER | PSP_CTRL_RTRIGGER | PSP_CTRL_START);

	// Open
	fd = sceIoOpen(ms_path, PSP_O_RDONLY, 0777);
	if(fd < 0)
	{
		ms_path[0] = 'm';
		ms_path[1] = 's';

		fd = sceIoOpen(ms_path, PSP_O_RDONLY, 0777);
		if(fd < 0) return 0;
	}

	while (Check_EOF(fd) == 0)
	{
		Read_Line(fd, buf, 255);

		if(buf[0] != '#' && buf[0] != '\n' && buf[0] != '\0')
		{
			ptr = ch_token(buf, "=");
			if(ptr == NULL) continue;

			if(strcmp(buf, "MP3DIRPATH") == 0)
			{
				strcpy(key->dirpath, ptr);
			}
			else if(strcmp(buf, "PLAY_PAUSE_KEY") == 0)
			{
				key->Play_Pause_Key = Get_Key(ptr);
			}
			else if(strcmp(buf, "NEXT_KEY") == 0)
			{
				key->Next_Key = Get_Key(ptr);
			}
			else if(strcmp(buf, "PREV_KEY") == 0)
			{
				key->Prev_Key = Get_Key(ptr);
			}
			else if(strcmp(buf, "UP_KEY") == 0)
			{
				key->Up_Key = Get_Key(ptr);
			}
			else if(strcmp(buf, "DOWN_KEY") == 0)
			{
				key->Down_Key = Get_Key(ptr);
			}
			else if(strcmp(buf, "HOLD_KEY") == 0)
			{
				key->Hold_Key = Get_Key(ptr);
			}
		}
	}

	sceIoClose(fd);
	return 0;
}

