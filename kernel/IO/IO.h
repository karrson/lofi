#pragma once
#include <limine.h>
#include <Memory/Memory.h>

void OutB(uint16_t port, uint8_t value);
uint8_t InB(uint16_t port);
void IO_Wait();

typedef struct __attribute__((packed)) {
	unsigned char Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	uint8_t OEM_Id[6];
	uint8_t OEM_TableId[8];
	uint32_t OEM_Revision;
	uint32_t CreatorId;
	uint32_t CreatorRevision;
} SdtHeader;

typedef struct __attribute__((packed)) {
	SdtHeader Sdth;
	uint64_t Reserved;
} MCFG;

void RemapInterruptsForIO();
void EndInterruptsForMasterIO();
void EndInterruptsForSlaveIO();

#define MasterPIC_CommandPort 0x20
#define SlavePIC_CommandPort 0xA0
#define MasterPIC_DataPort 0x21
#define SlavePIC_DataPort 0xA1

#define ICW1_Init 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

#define PIC_Eoi 0x20

namespace Pci {
	typedef struct __attribute__((packed)) {
		uint16_t VendorId;
		uint16_t DeviceId;
		uint16_t Command;
		uint16_t Status;
		uint8_t RevisionId;
		uint8_t ProgramInterface;
		uint8_t DeviceSubClass;
		uint8_t DeviceClass;
		uint8_t CacheLineSize;
		uint8_t LatencyTimer;
		uint8_t HeaderType;
		uint8_t BuiltInSelfTest;
	} Pci;

	typedef struct {
		Pci HDR;
		uint32_t BAR0;
		uint32_t BAR1;
		uint32_t BAR2;
		uint32_t BAR3;
		uint32_t BAR4;
		uint32_t BAR5;
		uint32_t CardBusCIS_Pointer;
		uint16_t SubSystemVendorId;
		uint16_t SubSystemId;
		uint32_t ExpansionROM_BaseAddress;
		uint8_t CapabilitiesPointer;
		uint8_t RSV0;
		uint16_t RSV1;
		uint32_t RSV2;
		uint8_t InterruptLine;
		uint8_t InterruptPin;
		uint8_t MIN_Grant;
		uint8_t MAX_Latency;
	} PciHeader0;

	void WalkThroughFunction(uint64_t DeviceAddress, uint64_t FunctionIndex);
	void WalkThroughDevice(uint64_t BusAddress, uint64_t DeviceIndex);
	void WalkThroughBus(uint64_t BaseAddress, uint64_t BusIndex);
	void WalkThroughPciBus(MCFG* MCFG0);
	void GetPci();
}
