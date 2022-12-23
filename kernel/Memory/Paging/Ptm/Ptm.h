#pragma once

#include <stddef.h>
#include <stdint.h>
#include <Memory/Paging/Pmm/Pmm.h>

extern void Halt();

extern uint64_t HHDM_Offset;

extern uint64_t KernelStart;
extern uint64_t KernelEnd;

extern uint64_t PhysicalKernelStart;
extern uint64_t VirtualKernelStart;

typedef struct {
	uint64_t Present : 1;
	uint64_t Writable : 1;
	uint64_t UserAccessible : 1;
	uint64_t WriteThroughCaching : 1;
	uint64_t DisableCache : 1;
	uint64_t Accessed : 1;
	uint64_t Reserved : 1;
	uint64_t HugePage : 1;
	uint64_t Global : 1;
	uint64_t Available0 : 3;
	uint64_t PhysicalAddress : 40;
	uint64_t Available1 : 11;
	uint64_t NoExecute : 1;
} PageTableEntry;

typedef struct __attribute__((aligned(0x1000))) {
	PageTableEntry Entries[512];
} PageTable;

uint64_t GetPageTableIndex(uint64_t VirtualAddress, uint64_t Level);
void GetNextPageLevel(PageTable** CurrentLevelPageTable, uint64_t NextLevelIndex, PageTable** NextLevelPageTable);
void MapMemory(PageTable* Pml4, void* VirtualBasePage, void* PhysicalBasePage, uint64_t Size);
void CreatePTM();
PageTableEntry SetupPageTableEntry(PageTableEntry CurrentPageTableEntry);