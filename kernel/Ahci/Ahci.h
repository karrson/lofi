#pragma once
#include <limine.h>
#include <IO/IO.h>

#define ATA_DEV_Busy 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxIS_TFES (1 << 30)
#define ATA_CommandReadDMA_EX 0x25

#define HBA_PortDevicePresent 0x3
#define HBA_PortIPM_Active 0x1

#define SATA_SIG_Atapi 0xEB140101
#define SATA_SIG_ATA 0x00000101
#define SATA_SIG_Semb 0xC33C0101
#define SATA_SIG_PM 0x96690101

#define HBA_PxCmdCR 0x8000
#define HBA_PxCmdFRE 0x0010
#define HBA_PxCmdSt 0x0001
#define HBA_PxCmdFR 0x1000

namespace Ahci
{
	enum PortType
	{
		None = 0,
		SATA = 1,
		Semb = 2,
		PM = 3,
		SATA_Pi = 4
	};

	enum FIS_Type
	{
		REG_H2D = 0x27,
		REG_D2H = 0x34,
		DMA_Act = 0x39,
		DMA_Setup = 0x41,
		Data = 0x46,
		Bist = 0x58,
		PIO_Setup = 0x5F,
		DevBits = 0xA1
	};

	struct HBA_Port
	{
		uint32_t CommandListBase;
		uint32_t CommandListBaseUpper;
		uint32_t FIS_BaseAddress;
		uint32_t FIS_BaseAddressUpper;
		uint32_t InterruptStatus;
		uint32_t InterruptEnable;
		uint32_t CommandStatus;
		uint32_t RSV0;
		uint32_t TaskFileData;
		uint32_t Signature;
		uint32_t SATA_Status;
		uint32_t SATA_Control;
		uint32_t SATA_Error;
		uint32_t SATA_Active;
		uint32_t CommandIssue;
		uint32_t SATA_Notification;
		uint32_t FIS_SwitchControl;
		uint32_t RSV1[11];
		uint32_t Vendor[4];
	};

	struct HBA_Memory
	{
		uint32_t HostCapability;
		uint32_t GlobalHostControl;
		uint32_t InterruptStatus;
		uint32_t PortsImplemented;
		uint32_t Version;
		uint32_t CCC_Control;
		uint32_t CCC_Ports;
		uint32_t EnclosureManagementLocation;
		uint32_t EnclosureManagementControl;
		uint32_t HostCapabilitiesExtended;
		uint32_t BIOS_HandoffControlStatus;
		uint8_t RSV0[0x74];
		uint8_t Vendor[0x60];
		HBA_Port Ports[1];
	};

	struct HBA_CommandHeader
	{
		uint8_t CommandFIS_Length : 5;
		uint8_t ATA_Pi:1;
		uint8_t Write:1;
		uint8_t Prefetchable:1;
		uint8_t Reset:1;
		uint8_t Bist:1;
		uint8_t ClearBusy:1;
		uint8_t RSV0:1;
		uint8_t PortMultiplier:4;
		uint16_t PrdtLength;
		uint32_t PrdbCount;
		uint32_t CommandTableBaseAddress;
		uint32_t CommandTableBaseAddressUpper;
		uint32_t RSV1[4];
	};

	struct FIS_REG_H2D
	{
		uint8_t FIS_Type;
		uint8_t PortMultiplier:4;
		uint8_t RSV0:3;
		uint8_t CommandControl:1;
		uint8_t Command;
		uint8_t FeatureLow;
		uint8_t LBA0;
		uint8_t LBA1;
		uint8_t LBA2;
		uint8_t DeviceRegister;
		uint8_t LBA3;
		uint8_t LBA4;
		uint8_t LBA5;
		uint8_t FeatureHigh;
		uint8_t CountLow;
		uint8_t CountHigh;
		uint8_t ISO_CommandCompletion;
		uint8_t Control;
		uint8_t RSV1[4];
	};

	struct HBA_PrdtEntry
	{
		uint32_t DbAddress;
		uint32_t DbAddressUpper;
		uint32_t RSV0;
		uint32_t ByteCount:22;
		uint32_t RSV1:9;
		uint32_t InterruptOnCompletion:1;
	};

	struct HBA_CommandTable
	{
		uint8_t CommandFIS[64];
		uint8_t ATA_PiCommand[16];
		uint8_t RSV[48];
		HBA_PrdtEntry PrdtEntries[];
	};

	class Port
	{
	public:
		HBA_Port* HBA;
		PortType Type;
		uint8_t* Buffer;
		uint8_t PortNumber;
		void Configure();
		void StopCommand();
		void StartCommand();
		bool Read(uint64_t Sector, uint32_t SectorCount, void* Buffer);
	};

	class AhciDriver
	{
	public:
		AhciDriver(Pci::Pci* BaseAddress);
		Pci::Pci* PciBaseAddress;
		HBA_Memory* ABAR;
		void ProbePorts();
		PortType GetPortType(HBA_Port* Port);
		Port* Ports[32];
		uint8_t PortCount;
	};
}
