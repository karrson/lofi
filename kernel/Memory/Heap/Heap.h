#pragma once
#include <stdint.h>
#include <stddef.h>
#include <Memory/Paging/Paging.h>

extern PageTable* Pml4;
extern void* aligned_malloc;

void CreateHeap(void* HeapAddress, size_t PageCount);
void CreateHeap();
void* malloc(size_t Size);
void* aligned_alloc(size_t Alignment, size_t Size);
void free(void* Address);
void ExpandHeap(size_t Length);

typedef struct {
	size_t Length;
	void* Next;
	void* Last;
	bool IsFree;
} HeapSegmentHeader;

void SegmentHeaderCombineForward(HeapSegmentHeader* HDR);
void SegmentHeaderCombineBackward(HeapSegmentHeader* HDR);
HeapSegmentHeader* SegmentHeaderSplit(HeapSegmentHeader* HDR, size_t Length);

inline void* operator new(size_t Size)
{
	return malloc(Size);
}

inline void* operator new[](size_t Size)
{
	return malloc(Size);
}

inline void operator delete(void* Address)
{
	free(Address);
}
