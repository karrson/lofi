#pragma once
#include <limine.h>

#define IntTA_Gate 0b10001110
#define IntTA_CallGate 0b10001100
#define IntTA_TrapGate 0b10001111

void CreateIdt();

typedef struct { } InterruptFrame;

void AddHandler(void (*Handler)(InterruptFrame*), uint64_t Offset);
void AddHandler(void (*Handler)(InterruptFrame*), uint64_t Offset, uint64_t Selector);

typedef struct
{
	uint16_t Offset0;
	uint16_t Selector;
	uint8_t StackListOffset;
	uint8_t TypeAttribute;
	uint16_t Offset1;
	uint32_t Offset2;
	uint32_t Ignore;
} InterruptEntry;

void SetInterruptOffset(InterruptEntry* IntAddress, uint64_t Offset);
uint64_t GetInterruptOffset(InterruptEntry* IntAddress);

typedef struct __attribute__((packed))
{
	uint16_t Limit;
	uint64_t Offset;
} Idt;

extern Idt Idt0;
