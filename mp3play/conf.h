// KeyConfig

#ifndef __CONF_H__
#define	__CONF_H__

typedef struct
{
	char dirpath[256];
	u32 Play_Pause_Key;
	u32 Next_Key;
	u32 Prev_Key;
	u32 Up_Key;
	u32 Down_Key;
	u32 Hold_Key;
} Conf_Key;

int Read_Conf(const char *path, Conf_Key *key);

#endif

