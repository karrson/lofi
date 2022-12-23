#include "Ahci.h"
#include <TTY0/puts.h>
#include <Debug/Debug.h>

int memcmp(const void* s1, const void* s2, size_t n);

extern PageTable* Pml4;
extern Ahci::Port* Root;

namespace Ahci
{
	void* Buffer = nullptr;
	
	AhciDriver::AhciDriver(Pci::Pci* Base_Address)
	{
		Buffer = AllocateEmptyPage();
		this->PciBaseAddress = Base_Address;
		this->ABAR = (HBA_Memory*)((Pci::PciHeader0*)this->PciBaseAddress)->BAR5;
		MapMemory(Pml4, this->ABAR, this->ABAR, 4096);
		this->ProbePorts();
		for (uint32_t Index = 0; Index < this->PortCount; Index++)
		{
			Port* Port = this->Ports[Index];
			Port->Configure();
			bool IsRoot = false;
			Port->Buffer = (uint8_t*)Buffer;
			for (uint64_t s = 0; s < 1000; s++)
			{
				memset(Port->Buffer, 0, 512);
				if (Port->Read(0, 1, Port->Buffer))
				{
					for (uint64_t i = 0; i < 512; i++)
					{
						if (memcmp(&Port->Buffer[i], "ROOT", 4) == 0)
						{
							Root = Port;
						}
					}
				}
			}
		}
	}

	bool Port::Read(uint64_t Sector, uint32_t SectorCount, void* Buffer)
	{
		MapMemory(Pml4, Buffer, Buffer, 4096);
		
		uint32_t SectorLow = (uint32_t)Sector;
		uint32_t SectorHigh = (uint32_t)(Sector >> 32);

		HBA->InterruptStatus = (uint32_t)-1;
		HBA_CommandHeader* CommandHeader = (HBA_CommandHeader*)HBA->CommandListBase;
		CommandHeader->CommandFIS_Length = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
		CommandHeader->Write = 0;
		CommandHeader->PrdtLength = 1;

		HBA_CommandTable* CommandTable = (HBA_CommandTable*)(CommandHeader->CommandTableBaseAddress);
		memset(CommandTable, 0, sizeof(HBA_CommandTable) + (CommandHeader->PrdtLength - 1) * sizeof(HBA_PrdtEntry));

		CommandTable->PrdtEntries[0].DbAddress = (uint32_t)(uint64_t)Buffer;
		CommandTable->PrdtEntries[0].DbAddressUpper = (uint32_t)((uint64_t)Buffer >> 32);
		CommandTable->PrdtEntries[0].ByteCount = (SectorCount << 9) - 1;
		CommandTable->PrdtEntries[0].InterruptOnCompletion = 1;

		FIS_REG_H2D* Command_FIS = (FIS_REG_H2D*)(&CommandTable->CommandFIS);
		Command_FIS->FIS_Type = FIS_Type::REG_H2D;
		Command_FIS->CommandControl = 1;
		Command_FIS->Command = ATA_CommandReadDMA_EX;
		Command_FIS->LBA0 = (uint8_t)SectorLow;
		Command_FIS->LBA1 = (uint8_t)(SectorLow >> 8);
		Command_FIS->LBA2 = (uint8_t)(SectorLow >> 16);
		Command_FIS->LBA3 = (uint8_t)SectorHigh;
		Command_FIS->LBA4 = (uint8_t)(SectorHigh >> 8);
		Command_FIS->LBA5 = (uint8_t)(SectorHigh >> 16);

		Command_FIS->DeviceRegister = 1 << 6;

		Command_FIS->CountLow = SectorCount & 0xFF;
		Command_FIS->CountHigh = (SectorCount >> 8) & 0xFF;

		uint64_t Spin = 0;
		while ((HBA->TaskFileData & (ATA_DEV_Busy | ATA_DEV_DRQ)) && Spin < 10000)
		{
			Spin++;
			IO_Wait();
		}
		if (Spin == 100) return false;

		HBA->CommandIssue = 1;
		while (true)
		{
			IO_Wait();
			if (HBA->CommandIssue == 0) break;
			if (HBA->InterruptStatus & HBA_PxIS_TFES) return false;
		}
		return true;
	}

	void Port::Configure()
	{
		StopCommand();

		void* NewBase = AllocateEmptyPage();
		HBA->CommandListBase = (uint32_t)(uint64_t)NewBase;
		HBA->CommandListBaseUpper = (uint32_t)((uint64_t)NewBase >> 32);
		memset((void*)NewBase, 0, 1024);

		void* FIS_Base = AllocateEmptyPage();
		HBA->FIS_BaseAddress = (uint32_t)(uint64_t)FIS_Base;
		HBA->FIS_BaseAddressUpper = (uint32_t)((uint64_t)FIS_Base >> 32);
		memset((void*)FIS_Base, 0, 256);

		HBA_CommandHeader* CommandHeader = (HBA_CommandHeader*)
			((uint64_t)HBA->CommandListBase + ((uint64_t)HBA->CommandListBaseUpper << 32));

		for (uint32_t Index = 0; Index < 32; Index++)
		{
			CommandHeader[Index].PrdtLength = 8;

			void* CommandTableAddress = AllocateEmptyPage();
			uint64_t Address = (uint64_t)CommandTableAddress + (Index << 8);
			CommandHeader[Index].CommandTableBaseAddress = (uint32_t)(uint64_t)Address;
			CommandHeader[Index].CommandTableBaseAddressUpper = (uint32_t)((uint64_t)Address >> 32);
			memset(CommandTableAddress, 0, 256);
		}

		StartCommand();
	}

	void Port::StopCommand()
	{
		HBA->CommandStatus &= ~HBA_PxCmdSt;
		HBA->CommandStatus &= ~HBA_PxCmdFRE;

		while (true)
		{
			if ((HBA->CommandStatus & HBA_PxCmdFR) || (HBA->CommandStatus & HBA_PxCmdCR)) continue;
			else break;
		}
	}

	void Port::StartCommand()
	{
		while (HBA->CommandStatus & HBA_PxCmdCR);

		HBA->CommandStatus |= HBA_PxCmdFRE;
		HBA->CommandStatus |= HBA_PxCmdSt;
	}

	void AhciDriver::ProbePorts()
	{
		uint32_t PortsImplemented = ABAR->PortsImplemented;
		for (uint32_t Index = 0; Index < 32; Index++)
		{
			if (PortsImplemented & (1 << Index))
			{
				PortType Type = GetPortType(&ABAR->Ports[Index]);
				if (Type == PortType::SATA || Type == PortType::SATA_Pi)
				{
					Ports[PortCount] = new Port();
					Ports[PortCount]->Type = Type;
					Ports[PortCount]->HBA = &ABAR->Ports[Index];
					Ports[PortCount]->PortNumber = PortCount;
					PortCount++;
				}
			}
		}
	}

	PortType AhciDriver::GetPortType(HBA_Port* Port)
	{
		uint32_t SATA_Status = Port->SATA_Status;

		uint8_t InterfacePowerManagement = (SATA_Status >> 8) && 0b111;
		uint8_t DeviceDetection = SATA_Status & 0b111;

		if (DeviceDetection != HBA_PortDevicePresent ||
			InterfacePowerManagement != HBA_PortIPM_Active)
		{
			return PortType::None;
		}

		if (Port->Signature == SATA_SIG_Atapi)
		{
			return PortType::SATA_Pi;
		}

		if (Port->Signature == SATA_SIG_ATA)
		{
			return PortType::SATA;
		}

		if (Port->Signature == SATA_SIG_PM)
		{
			return PortType::PM;
		}

		if (Port->Signature == SATA_SIG_Semb)
		{
			return PortType::Semb;
		}

		return PortType::None;
	}
}
