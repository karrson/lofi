#include "Pmm.h"

volatile struct limine_memmap_request MemoryMapRequest = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};

uint64_t GetSizeOfMemory()
{
	static uint64_t SizeOfMemory = 0;
	if (SizeOfMemory > 0) return SizeOfMemory;

	for (uint64_t Index = 0; Index < MemoryMapRequest.response->entry_count; Index++)
	{
		struct limine_memmap_entry* Entry = MemoryMapRequest.response->entries[Index];

		uint64_t EndOfSegment = Entry->base + Entry->length;
		if ((Entry->type == LIMINE_MEMMAP_USABLE || 
			 Entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE))
		{
			if (EndOfSegment > SizeOfMemory)
			{
				SizeOfMemory = EndOfSegment;
			}
		}
	}

	uint64_t Unit = 1048576;
	if (SizeOfMemory % Unit != 0) SizeOfMemory = (SizeOfMemory / Unit + 1) * Unit;

	return SizeOfMemory;
}

void memset(void* Base, uint8_t Val, uint64_t Length)
{
	for (uint64_t Index = 0; Index < Length; Index++)
		*(uint8_t*)((uint64_t)Base + Index) = Val;
}

uint64_t* FreeList;
uint64_t  FreeListLength = 0;
uint64_t  LastAddress = 0;

void CreatePMM()
{
	uint64_t MAX_Size = 0;
	for (uint64_t Index = 0; Index < MemoryMapRequest.response->entry_count; Index++)
	{
		struct limine_memmap_entry* Entry = MemoryMapRequest.response->entries[Index];
		if (Entry->type == LIMINE_MEMMAP_USABLE)
		{
			if (Entry->length > MAX_Size)
			{
				MAX_Size = Entry->length;
				FreeList = (uint64_t*)Entry->base;
			}
		}
	}
}

uint64_t LastPageIndex = 0;

void done(void);

void* AllocatePage()
{
	if (FreeListLength > 0)
	{
		uint64_t FreeAddress = *(uint64_t*)((uint64_t)FreeList + (FreeListLength - 1) * 8);
		FreeListLength--;
		return (void*)FreeAddress;
	}
	else
	{
		for (uint64_t Index = 0; Index < MemoryMapRequest.response->entry_count; Index++)
		{
			struct limine_memmap_entry* Entry = MemoryMapRequest.response->entries[Index];
			if (Entry->type == LIMINE_MEMMAP_USABLE)
			{
				for (uint64_t PageIndex = 0; PageIndex < Entry->length / 4096; PageIndex++)
				{
					uint64_t Address = Entry->base + PageIndex * 4096;
					if (Address > LastAddress)
					{
						LastAddress = Address;
						return (void*)Address;
					}
				}
			}
		}
	}
	fputs("Out of memory", stdout);
	done();
	return nullptr;
}

void* AllocateEmptyPage()
{
	void* Address = AllocatePage();
	memset(Address, 0, 4096);
	return Address;
}

void FreePage(void* ptr)
{
	FreeListLength++;
	*(uint64_t*)((uint64_t)FreeList + (FreeListLength - 1) * 8) = (uint64_t)ptr;
}
