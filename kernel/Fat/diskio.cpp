#include "diskio.h"
#include <Ahci/Ahci.h>

Ahci::Port* Root;
void* RootBuffer = nullptr;

DSTATUS disk_initialize(void)
{
	if (RootBuffer == nullptr) 
	{
		RootBuffer = AllocatePage();
		memset(RootBuffer, 0, 512);
	}
	return 0;
}

DRESULT disk_readp (
  BYTE* buff,    /* [OUT] Pointer to the read buffer */
  DWORD sector,  /* [IN]  Sector number to read */
  UINT offset,   /* [IN]  Byte offset in the sector to start to read */
  UINT count     /* [IN]  Number of bytes to read */
)
{
	Root->Buffer = (uint8_t*)RootBuffer;
	memset(Root->Buffer, 0, 512);
	if (Root->Read(sector, 1, Root->Buffer))
	{
		uint64_t j = 0;
		for (uint64_t i = offset; i < offset + count; i++)
		{
			buff[j] = Root->Buffer[i];
			j++;
		}
		return RES_OK;
	}
	return RES_ERROR;
}