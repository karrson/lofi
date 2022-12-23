#pragma once

#include "Gdt/Gdt.h"
#include "Heap/Heap.h"
#include "Paging/Paging.h"

int memcmp(const void* s1, const void* s2, size_t n);

#define AddressOfSystemData 0xFFFFFFFFBFFFF000