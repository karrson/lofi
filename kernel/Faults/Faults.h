#pragma once
#include <limine.h>
#include <Interrupts/Interrupts.h>

void Panic(char* str);

__attribute__((interrupt)) void OnPageFault(InterruptFrame* Frame);
__attribute__((interrupt)) void OnDoubleFault(InterruptFrame* Frame);
__attribute__((interrupt)) void OnGeneralProtectionFault(InterruptFrame* Frame);

void RegisterFaults();