#include "Interrupts.h"

void SetInterruptOffset(InterruptEntry* IntAddress, uint64_t Offset)
{
	IntAddress->Offset0 = (uint16_t)((Offset & 0x000000000000FFFF));
	IntAddress->Offset1 = (uint16_t)((Offset & 0x00000000FFFF0000) >> 16);
	IntAddress->Offset2 = (uint32_t)((Offset & 0xFFFFFFFF00000000) >> 32);
}

uint64_t GetInterruptOffset(InterruptEntry* IntAddress)
{
	uint64_t Offset = 0;

	Offset |= (uint64_t)IntAddress->Offset0;
	Offset |= (uint64_t)IntAddress->Offset1 << 16;
	Offset |= (uint64_t)IntAddress->Offset2 << 32;

	return Offset;
}

Idt Idt0;
uint8_t IdtPage[4096] = { 0 };

void CreateIdt()
{
	Idt0.Limit = 0x0FFF;
	Idt0.Offset = (uint64_t)&IdtPage[0];
}

void AddHandler(void (*Handler)(InterruptFrame*), uint64_t Offset)
{
	AddHandler(Handler, Offset, 0x08);
}

void AddHandler(void (*Handler)(InterruptFrame*), uint64_t Offset, uint64_t Selector)
{
	InterruptEntry* PageFaultHandler = (InterruptEntry*)(Idt0.Offset + Offset * sizeof(InterruptEntry));
	SetInterruptOffset(PageFaultHandler, (uint64_t)Handler);
	PageFaultHandler->TypeAttribute = IntTA_Gate;
	PageFaultHandler->Selector = Selector;

	asm("lidt %0" : : "m" (Idt0));
	asm("sti");
}
