#pragma once

#include <stdint.h>

typedef struct __attribute__((packed))
{
	uint16_t Size;
	uint64_t Offset;
} Gdt;

extern "C" uint64_t Gdt0[6];

extern "C" void load_gdt(Gdt* Gdt0);

void CreateGdt();

