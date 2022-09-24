// file

// header
#include <pspkernel.h>
#include <string.h>
#include "file.h"
#include "memory.h"
#include "sysclib.h"

// function
int FileCount(const char *path)
{
	SceUID dfd;
	SceIoDirent dir;
	char buf[512];
	int res, count = 0;

	dfd = sceIoDopen(path);
	if(dfd < 0) return 0;

	do
	{
		memset(&dir, 0, sizeof(SceIoDirent));
		res = sceIoDread(dfd, &dir);

		if(res > 0)
		{
			if(!strcmp(dir.d_name, ".") || !strcmp(dir.d_name, ".."))
				continue;

			if(strstr(dir.d_name, ".mp3") || strstr(dir.d_name, ".MP3"))
				count++;

			if(FIO_S_ISDIR(dir.d_stat.st_mode) || FIO_SO_ISDIR(dir.d_stat.st_attr))
			{
				snprintf(buf, 512, "%s/%s", path, dir.d_name);
				count += FileCount(buf);
			}
		}
	}
	while(res > 0);

	sceIoDclose(dfd);
	return count;
}

int GetFileList(const char *path, FileList *list, int *count, int pid)
{
	SceUID dfd;
	SceIoDirent dir;
	char buf[512];
	int res, len;

	dfd = sceIoDopen(path);
	if(dfd < 0) return 0;

	do
	{
		memset(&dir, 0, sizeof(SceIoDirent));
		res = sceIoDread(dfd, &dir);

		if(res > 0)
		{
			if(!strcmp(dir.d_name, ".") || !strcmp(dir.d_name, ".."))
				continue;

			if(strstr(dir.d_name, ".mp3") || strstr(dir.d_name, ".MP3"))
			{
				len = snprintf(buf, 512, "%s/%s", path, dir.d_name);

				list[(*count)].name = (char *)malloc_p((len + 1), &list[(*count)].memid, pid);

				if(list[(*count)].name != NULL)
				{
					strcpy(list[(*count)].name, buf);
					(*count) += 1;
				}
				else
				{
					break;
				}
			}

			if(FIO_S_ISDIR(dir.d_stat.st_mode) || FIO_SO_ISDIR(dir.d_stat.st_attr))
			{
				snprintf(buf, 512, "%s/%s", path, dir.d_name);
				GetFileList(buf, list, count, pid);
			}
		}
	}
	while(res > 0);

	sceIoDclose(dfd);
	return (*count);
}

void ClearFilelist(FileList *list, int list_memid, int count)
{
	int i;
	for(i = 0; i < count; i++)
	{
		free_p(list[i].memid);
	}

	free_p(list_memid);
	return;
}

