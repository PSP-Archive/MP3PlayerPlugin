// memory

// header
#include <pspkernel.h>
#include <pspsysmem.h>
#include <string.h>

// function
void *malloc_p(u32 size, int *memid, int pid)
{
	SceUID id = 0;
	void *ptr = NULL;

	if(!size)
		size = 1;

	id = sceKernelAllocPartitionMemory(pid, "block", 0, size, NULL);
	if(id < 0) return NULL;

	ptr = sceKernelGetBlockHeadAddr(id);
	if(!ptr) return NULL;

	*memid = id;
	return ptr;
}

int free_p(int memid)
{
	return sceKernelFreePartitionMemory(memid);
}

void *memory_alloc(u32 size)
{
	SceUID memid = 0;
	void *ptr = NULL;

	if(!size)
		size = 1;

	memid = sceKernelAllocPartitionMemory(2, "umem", 0, size, NULL);
	if(memid < 0) return NULL;

	ptr = (void *)((int)sceKernelGetBlockHeadAddr(memid) + sizeof(SceUID));
	memcpy((void *)((int)ptr - sizeof(SceUID)), (void *)&memid, sizeof(SceUID));

	return ptr;
}

int free_alloc(void *ptr)
{
	SceUID memid = 0;

	if(!ptr)
		return -1;

	memcpy((void *)&memid, (void *)((int)ptr - sizeof(SceUID)), sizeof(SceUID));
	return sceKernelFreePartitionMemory(memid);
}

