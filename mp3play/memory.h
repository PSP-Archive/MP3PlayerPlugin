#ifndef __MEM_H__
#define __MEM_H__

void *malloc_p(u32 size, int *memid, int pid);
int free_p(int memid);

void *memory_alloc(u32 size);
int free_alloc(void *ptr);

#endif

