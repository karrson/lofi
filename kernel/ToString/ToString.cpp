#include "ToString.h"

void uint2str(uint64_t val, char* out)
{
	uint64_t len = 1; uint64_t tmp = val;
	while (tmp / 10 > 0) { tmp /= 10; len++; }

	for (int64_t i = len - 1; i >= 0; i--)
	{
		*(out + i) = '0' + (val % 10);
		val /= 10;
	}
	*(out + len) = 0;
}

void hex2str(uint64_t val, char* out)
{
	uint64_t len = 1; uint64_t tmp = val;
	while (tmp / 16 > 0) { tmp /= 16; len++; }

	for (int64_t i = len - 1; i >= 0; i--)
	{
		uint64_t digit = val % 16;
		*(out + i) = (digit > 9 ? 'A' : '0') +
			((digit > 9 ? (digit - 10) : digit));
		val /= 16;
	}
	*(out + len) = 0;
}

void int2str(int64_t val, char* out)
{
	int64_t offset = 0;

	if (val < 0)
	{
		*out = '-';
		offset = 1;
		val *= -1;
	}

	uint64_t len = 1; uint64_t tmp = val;
	while (tmp / 10 > 0) { tmp /= 10; len++; }

	for (int64_t i = len - 1; i >= 0; i--)
	{
		*((out + offset) + i) = '0' + (val % 10);
		val /= 10;
	}
	*((out + offset) + len) = 0;
}

