#include "Acpi.h"
#include <TTY0/puts.h>

volatile struct limine_rsdp_request RSDP_Request = {
	.id = LIMINE_RSDP_REQUEST,
	.revision = 0
};
void* RSDP;

void done(void);

AcpiData* GetAcpi()
{
	static AcpiData* AcpiData0 = nullptr;
	if (AcpiData0 == nullptr)
	{
		if (RSDP_Request.response == nullptr) done();
		RSDP = RSDP_Request.response->address;
		AcpiData0 = (AcpiData*)AllocateEmptyPage();
		AcpiData0->RSDP0 = (RSDP2*)RSDP;
		SdtHeader* Xsdt = (SdtHeader*)(AcpiData0->RSDP0->Xsdt);
		uint64_t ItemCount = (uint64_t)(((uint64_t)Xsdt->Length - (uint64_t)sizeof(SdtHeader)) / 8);

		for (uint64_t Index = 0; Index < ItemCount; Index++)
		{
			SdtHeader* Item = (SdtHeader*)*((uint64_t*)
				((uint64_t)Xsdt + sizeof(SdtHeader) + (Index * 8)));

			if (memcmp(Item->Signature, "MCFG", 4) == 0)
			{
				AcpiData0->MCFG0 = (MCFG*)Item;
				Pci::GetPci();
			}
		}
	}

	return AcpiData0;
}
