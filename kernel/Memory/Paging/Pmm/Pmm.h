#pragma once

#include <limine.h>
#include <stddef.h>
#include <stdint.h>
#include <TTY0/puts.h>

extern uint64_t KernelStart;
extern uint64_t KernelEnd;

extern uint64_t PhysicalKernelStart;
extern uint64_t VirtualKernelStart;

extern volatile struct limine_memmap_request MemoryMapRequest;

uint64_t GetSizeOfMemory();
void memset(void* Base, uint8_t Val, uint64_t Length);
void CreatePMM();
void* AllocatePage();
void* AllocatePages(uint64_t Count);
void* AllocateEmptyPage();
void* AllocateEmptyPages(uint64_t Count);
void FreePage(void* ptr);
void FreePages(void* ptr, uint64_t Count);
