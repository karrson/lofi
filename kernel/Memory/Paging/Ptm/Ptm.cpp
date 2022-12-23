#include "Ptm.h"

extern volatile struct limine_memmap_request MemoryMapRequest;

volatile struct limine_hhdm_request HHDM_Request = {
	.id = LIMINE_HHDM_REQUEST,
	.revision = 0
};
uint64_t HHDM_Offset;

volatile struct limine_kernel_address_request KernelAddressRequest = {
	.id = LIMINE_KERNEL_ADDRESS_REQUEST,
	.revision = 0
};
uint64_t PhysicalKernelStart;
uint64_t VirtualKernelStart;

PageTable* Pml4;
PageTableEntry CurrentPageTableEntry;

uint64_t GetFullSizeOfMemory()
{
	static uint64_t FullSizeOfMemory = 0;
	if (FullSizeOfMemory > 0) return FullSizeOfMemory;

	for (uint64_t Index = 0; Index < MemoryMapRequest.response->entry_count; Index++)
	{
		struct limine_memmap_entry* Entry = MemoryMapRequest.response->entries[Index];

		uint64_t EndOfSegment = Entry->base + Entry->length;
		if (EndOfSegment > FullSizeOfMemory)
		{
			FullSizeOfMemory = EndOfSegment;
		}
	}

	return FullSizeOfMemory;
}

void done(void);

void CreatePTM()
{
	if (HHDM_Request.response == NULL) done();
	HHDM_Offset = HHDM_Request.response->offset;

	if (KernelAddressRequest.response == NULL) done();
	PhysicalKernelStart = KernelAddressRequest.response->physical_base;
	VirtualKernelStart = KernelAddressRequest.response->virtual_base;

	Pml4 = (PageTable*)AllocateEmptyPage();

	uint64_t KernelSize = (uint64_t)&KernelEnd - (uint64_t)&KernelStart;

	MapMemory(Pml4, (void*)0, (void*)0, GetFullSizeOfMemory());
	MapMemory(Pml4, (void*)VirtualKernelStart, (void*)PhysicalKernelStart, KernelSize);
	MapMemory(Pml4, (void*)HHDM_Offset, (void*)0, GetFullSizeOfMemory());
	
	asm volatile ("mov %0, %%cr3" : : "r" (Pml4) : "memory");
}

PageTableEntry SetupPageTableEntry(PageTableEntry CurrentPageTableEntry)
{
	CurrentPageTableEntry.Writable = 1;
	CurrentPageTableEntry.UserAccessible = 1;

	return CurrentPageTableEntry;
}

void MapMemory(PageTable* Pml4, void* VirtualBasePage, void* PhysicalBasePage, uint64_t Size)
{
	void* CurrentVirtualPage = VirtualBasePage;
	void* CurrentPhysicalPage = PhysicalBasePage;

	while ((uint64_t)CurrentPhysicalPage < (uint64_t)((void*)PhysicalBasePage + Size))
	{
		PageTable* CurrentPageTable = Pml4;
		PageTable* NextPageTable = 0;

		GetNextPageLevel(&CurrentPageTable, GetPageTableIndex((uint64_t)CurrentVirtualPage, 4), &NextPageTable);
		CurrentPageTable = NextPageTable;
		NextPageTable = nullptr;

		GetNextPageLevel(&CurrentPageTable, GetPageTableIndex((uint64_t)CurrentVirtualPage, 3), &NextPageTable);
		CurrentPageTable = NextPageTable;
		NextPageTable = nullptr;

		GetNextPageLevel(&CurrentPageTable, GetPageTableIndex((uint64_t)CurrentVirtualPage, 2), &NextPageTable);
		CurrentPageTable = NextPageTable;
		NextPageTable = nullptr;

		CurrentPageTableEntry = CurrentPageTable->Entries[GetPageTableIndex((uint64_t)CurrentVirtualPage, 1)];
		CurrentPageTableEntry.PhysicalAddress = (uint64_t)CurrentPhysicalPage >> 12;
		CurrentPageTableEntry.Present = 1;
		CurrentPageTableEntry = SetupPageTableEntry(CurrentPageTableEntry);

		CurrentPageTable->Entries[GetPageTableIndex((uint64_t)CurrentVirtualPage, 1)] = CurrentPageTableEntry;

		CurrentVirtualPage = (void*)((uint64_t)CurrentVirtualPage + 4096);
		CurrentPhysicalPage = (void*)((uint64_t)CurrentPhysicalPage + 4096);
	}
}

uint64_t GetPageTableIndex(uint64_t VirtualAddress, uint64_t Level)
{
	return (VirtualAddress >> (3 + 9 * Level)) & 0x1FF;
}

void GetNextPageLevel(PageTable** CurrentLevelPageTable, uint64_t NextLevelIndex, PageTable** NextLevelPageTable)
{
	CurrentPageTableEntry = (*CurrentLevelPageTable)->Entries[NextLevelIndex];

	if (!CurrentPageTableEntry.Present)
	{
		*NextLevelPageTable = (PageTable*)AllocateEmptyPage();

		CurrentPageTableEntry.PhysicalAddress = (uint64_t)(*NextLevelPageTable) >> 12;
		CurrentPageTableEntry.Present = 1;
		CurrentPageTableEntry = SetupPageTableEntry(CurrentPageTableEntry);

		(*CurrentLevelPageTable)->Entries[NextLevelIndex] = CurrentPageTableEntry;
	}
	else *NextLevelPageTable = (PageTable*)((uint64_t)CurrentPageTableEntry.PhysicalAddress << 12);
}

