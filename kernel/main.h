#pragma once
#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <TTY0/puts.h>
#include <Acpi/Acpi.h>
#include <Faults/Faults.h>
#include <Interrupts/Interrupts.h>
#include <IO/IO.h>
#include <Memory/Memory.h>
#include <Debug/Debug.h>
#include <ToString/ToString.h>
#include <assert.h>

extern volatile struct limine_terminal_request terminal_request;

void done(void);

extern "C" void _start();
void PrepareKernel();

extern uint64_t KernelStart;
extern uint64_t KernelEnd;