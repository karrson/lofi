#pragma once
#include <limine.h>
#include <IO/IO.h>
#include <Memory/Memory.h>
#include <Faults/Faults.h>

extern void* RSDP;

typedef struct __attribute__((packed))
{
	unsigned char Signature[8];
	uint8_t Checksum;
	uint8_t OEM_Id[6];
	uint8_t Revision;
	uint32_t RSDP_Table;
	uint32_t RSDP_TableLength;
	uint64_t Xsdt;
	uint8_t XsdtChecksum;
	uint8_t Reserved[3];
} RSDP2;

typedef struct
{
	RSDP2* RSDP0;
	MCFG* MCFG0;
} AcpiData;

AcpiData* GetAcpi();

typedef struct __attribute__((packed))
{
	uint64_t BaseAddress;
	uint16_t PciSegmentGroup;
	uint8_t FirstBusIndex;
	uint8_t LastBusIndex;
	uint32_t Reserved;
} Acpi;

void GetPci();