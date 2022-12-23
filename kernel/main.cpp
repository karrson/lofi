#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <main.h>

void puts_n(const char* str, size_t s)
{
	for (int i = 0; i < s; i++)
	{
		putc(str[i]);
	}
}

extern "C" void _start()
{
    if (terminal_request.response == NULL
     || terminal_request.response->terminal_count < 1) {
        done();
    }
	
    PrepareKernel();
	
	puts("This is a test! :)");
	
	void (**k_puts_n)(const char*) = AddressOfSystemData;
	*k_puts_n = puts_n;
	
	(*k_puts)("hmmmmmmmmmmmm");
	
    done();
}

void PrepareKernel()
{
	CreateGdt();
	CreatePMM();
	CreateIdt();
	RegisterFaults();
	CreatePTM();
	CreateHeap();
	memset(AddressOfSystemData, 0, 4096);
	AcpiData* Acpi = GetAcpi();
}

void done(void)
{
    for (;;) {
        __asm__("hlt");
    }
}