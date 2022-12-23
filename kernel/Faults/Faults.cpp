#include "Faults.h"
#include <TTY0/puts.h>

void done(void);

__attribute__((interrupt)) void OnPageFault(InterruptFrame* Frame)
{
	fputs("PF.", stdout);
	done();
}

__attribute__((interrupt)) void OnDoubleFault(InterruptFrame* Frame)
{
	fputs("DF.", stdout);
	done();
}

__attribute__((interrupt)) void OnGeneralProtectionFault(InterruptFrame* Frame)
{
	fputs("GP.", stdout);
	done();
}


void RegisterFaults()
{
	AddHandler(OnPageFault, 0xE);
	AddHandler(OnDoubleFault, 0x8);
	AddHandler(OnGeneralProtectionFault, 0xD);
}