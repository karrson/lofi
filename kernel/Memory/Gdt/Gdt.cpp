#include "Gdt.h"

__attribute__((aligned(0x1000)))
uint64_t Gdt0[6] = {
	0x0000000000000000,
	0x0020980000000000,
	0x0000920000000000,
	0x0020f80000000000,
	0x0000f20000000000
};

void CreateGdt()
{
	Gdt Gdtr;
	Gdtr.Size = (5 * 8) - 1;
	Gdtr.Offset = (uint64_t)&Gdt0;
	load_gdt(&Gdtr);
}
