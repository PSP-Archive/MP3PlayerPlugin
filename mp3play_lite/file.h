#ifndef __FILE_H__
#define __FILE_H__

typedef struct
{
	char *name;
	int memid;
} FileList;

int FileCount(const char *path);
int GetFileList(const char *path, FileList *list, int *count, int pid);
void ClearFilelist(FileList *list, int list_memid, int count);

#endif

