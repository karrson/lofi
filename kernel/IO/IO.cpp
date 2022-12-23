#include "IO.h"
#include <Acpi/Acpi.h>
#include <Ahci/Ahci.h>
#include <TTY0/puts.h>
#include <Debug/Debug.h>

void Pci::WalkThroughFunction(uint64_t DeviceAddress, uint64_t FunctionIndex)
{
	uint64_t FunctionOffset = FunctionIndex << 12;
	uint64_t FunctionAddress = DeviceAddress + FunctionOffset;

	Pci* Pci0 = (Pci*)FunctionAddress;

	if (Pci0->DeviceId != 0 && Pci0->DeviceId != 0xFFFF)
	{
		if (Pci0->DeviceClass == 0x01 &&
			Pci0->DeviceSubClass == 0x06 &&
			Pci0->ProgramInterface == 0x01)
		{
			new Ahci::AhciDriver(Pci0);
		}
	}
}

void Pci::WalkThroughDevice(uint64_t BusAddress, uint64_t DeviceIndex)
{
	uint64_t DeviceOffset = DeviceIndex << 15;
	uint64_t DeviceAddress = BusAddress + DeviceOffset;

	Pci* Pci0 = (Pci*)DeviceAddress;

	if (Pci0->DeviceId != 0 && Pci0->DeviceId != 0xFFFF)
	{
		for (uint64_t FunctionIndex = 0; FunctionIndex < 8; FunctionIndex++)
		{
			WalkThroughFunction(DeviceAddress, FunctionIndex);
		}
	}
}

void Pci::WalkThroughBus(uint64_t BaseAddress, uint64_t BusIndex)
{
	uint64_t BusOffset = BusIndex << 20;
	uint64_t BusAddress = BaseAddress + BusOffset;

	Pci* Pci0 = (Pci*)BusAddress;

	if (Pci0->DeviceId != 0 && Pci0->DeviceId != 0xFFFF)
	{
		for (uint64_t DeviceIndex = 0; DeviceIndex < 32; DeviceIndex++)
		{
			WalkThroughDevice(BusAddress, DeviceIndex);
		}
	}
}

void Pci::WalkThroughPciBus(MCFG* MCFG0)
{
	uint64_t ItemCount = ((MCFG0->Sdth.Length) - sizeof(MCFG)) / sizeof(Acpi);

	for (uint64_t Index = 0; Index < ItemCount; Index++)
	{
		Acpi* DeviceConfiguration = (Acpi*)((uint64_t)MCFG0 +
			(sizeof(MCFG)) + (Index * sizeof(Acpi)));

		for (uint64_t BusIndex = DeviceConfiguration->FirstBusIndex; BusIndex < DeviceConfiguration->LastBusIndex; BusIndex++)
		{
			WalkThroughBus(DeviceConfiguration->BaseAddress, BusIndex);
		}
	}
}

void Pci::GetPci()
{
	static bool IsReady = false;
	if (!IsReady)
	{
		AcpiData* Acpi0 = GetAcpi();
		WalkThroughPciBus(Acpi0->MCFG0);
		IsReady = true;
	}
}

void OutB(uint16_t port, uint8_t value)
{
	asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

uint8_t InB(uint16_t port)
{
	uint8_t value;
	asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
	return value;
}

void IO_Wait()
{
	asm volatile("outb %%al, $0x80" : : "a"(0));
}

void RemapInterruptsForIO()
{
	uint8_t MasterDataValue;
	uint8_t SlaveDataValue;

	MasterDataValue = InB(MasterPIC_DataPort); IO_Wait();
	SlaveDataValue = InB(SlavePIC_DataPort); IO_Wait();

	OutB(MasterPIC_CommandPort, ICW1_Init | ICW1_ICW4); IO_Wait();
	OutB(SlavePIC_CommandPort, ICW1_Init | ICW1_ICW4); IO_Wait();

	OutB(MasterPIC_DataPort, 0x20); IO_Wait();
	OutB(SlavePIC_DataPort, 0x28); IO_Wait();

	OutB(MasterPIC_DataPort, 4); IO_Wait();
	OutB(SlavePIC_DataPort, 2); IO_Wait();

	OutB(MasterPIC_DataPort, ICW4_8086); IO_Wait();
	OutB(SlavePIC_DataPort, ICW4_8086); IO_Wait();

	OutB(MasterPIC_DataPort, MasterDataValue); IO_Wait();
	OutB(SlavePIC_DataPort, SlaveDataValue); IO_Wait();
}

void EndInterruptsForMasterIO()
{
	OutB(MasterPIC_CommandPort, PIC_Eoi);
}

void EndInterruptsForSlaveIO()
{
	OutB(SlavePIC_CommandPort, PIC_Eoi);
	EndInterruptsForMasterIO();
}
