#include "Heap.h"

void* HeapStart;
void* HeapEnd;
HeapSegmentHeader* LastHeapHDR;

void CreateHeap(void* HeapAddress, size_t PageCount)
{
	void* Position = HeapAddress;

	for (size_t Index = 0; Index < PageCount; Index++)
	{
		MapMemory(Pml4, Position, AllocateEmptyPage(), 4096);
		Position = (void*)((size_t)Position + 4096);
	}

	size_t Length = PageCount * 4096;
	HeapStart = HeapAddress;
	HeapEnd = (void*)((size_t)HeapStart + Length);
	HeapSegmentHeader* StartSegment = (HeapSegmentHeader*)HeapAddress;
	StartSegment->Length = Length - sizeof(HeapSegmentHeader);
	StartSegment->Next = nullptr;
	StartSegment->Last = nullptr;
	StartSegment->IsFree = true;
	LastHeapHDR = StartSegment;
}

void CreateHeap()
{
	// An extra page worth of space for global system data
	{
		MapMemory(Pml4, 0xFFFFFFFFBFFFF000, AllocateEmptyPage(), 4096);
		memset(0xFFFFFFFFBFFFF000, 0, 4096);
	}
	// The actual heap
	{
		CreateHeap(0xffffffffC0000000, 512);
	}
}

void done(void);

void* malloc(size_t Size)
{
	if (Size % 0x10 > 0)
	{
		Size -= Size % 0x10;
		Size += 0x10;
	}
	if (Size == 0) 
	{
		fputs("Out of memory", stdout);
		done();
		return nullptr;
	}
	HeapSegmentHeader* Current = (HeapSegmentHeader*)HeapStart;
	while (true)
	{
		if (Current->IsFree)
		{
			if (Current->Length > Size)
			{
				SegmentHeaderSplit(Current, Size);
				Current->IsFree = 0;
				return (void*)((uint64_t)Current + sizeof(HeapSegmentHeader));
			}
			if (Current->Length == Size)
			{
				Current->IsFree = 0;
				return (void*)((uint64_t)Current + sizeof(HeapSegmentHeader));
			}
		}
		if (Current->Next == 0) break;
		Current = Current->Next;
	}
	ExpandHeap(Size);
	return malloc(Size);
}

void free(void* Address)
{
	HeapSegmentHeader* HDR = (HeapSegmentHeader*)Address - 1;
	HDR->IsFree = 1;
	SegmentHeaderCombineForward(HDR);
	SegmentHeaderCombineBackward(HDR);
}

void ExpandHeap(size_t Length)
{
	if (Length % 4096 != 0)
	{
		Length -= Length % 4096;
		Length += 4096;
	}

	size_t PageCount = Length / 4096;
	HeapSegmentHeader* NewHDR = (HeapSegmentHeader*)HeapEnd;
	for (size_t i = 0; i < PageCount; i++)
	{
		MapMemory(Pml4, HeapEnd, AllocateEmptyPage(), 4096);
		HeapEnd = (void*)((size_t)HeapEnd + 4096);
	}
	NewHDR->IsFree = 1;
	NewHDR->Last = LastHeapHDR;
	LastHeapHDR->Next = NewHDR;
	LastHeapHDR = NewHDR;
	NewHDR->Next = nullptr;
	NewHDR->Length = Length - sizeof(HeapSegmentHeader);
	SegmentHeaderCombineBackward(NewHDR);
}

void SegmentHeaderCombineForward(HeapSegmentHeader* HDR)
{
	if (HDR->Next == 0) return;
	if (!((HeapSegmentHeader*)HDR->Next)->IsFree) return;
	if ((HeapSegmentHeader*)HDR->Next == LastHeapHDR) LastHeapHDR = HDR;
	if ((HeapSegmentHeader*)((HeapSegmentHeader*)HDR->Next)->Next != 0)
	{
		((HeapSegmentHeader*)((HeapSegmentHeader*)HDR->Next)->Next)->Last = HDR;
	}
	HDR->Length = HDR->Length + ((HeapSegmentHeader*)HDR->Next)->Length + sizeof(HeapSegmentHeader);
	HDR->Next = ((HeapSegmentHeader*)HDR->Next)->Next;
}

void SegmentHeaderCombineBackward(HeapSegmentHeader* HDR)
{
	if (HDR->Last != 0 && ((HeapSegmentHeader*)HDR->Last)->IsFree) SegmentHeaderCombineForward(HDR->Last);
}

HeapSegmentHeader* SegmentHeaderSplit(HeapSegmentHeader* HDR, size_t Length)
{
	if (Length < 0x10) return 0;
	int64_t SplitSegmentLength = HDR->Length - Length - sizeof(HeapSegmentHeader);
	if (SplitSegmentLength < 0x10) return 0;
	HeapSegmentHeader* NewHDR = (HeapSegmentHeader*)((size_t)HDR + Length + sizeof(HeapSegmentHeader));
	((HeapSegmentHeader*)HDR->Next)->Last = NewHDR;
	NewHDR->Next = HDR->Next;
	HDR->Next = NewHDR;
	NewHDR->Last = HDR;
	NewHDR->Length = SplitSegmentLength;
	NewHDR->IsFree = HDR->IsFree;
	HDR->Length = Length;
	if (LastHeapHDR == HDR) LastHeapHDR = NewHDR;
	return NewHDR;
}
