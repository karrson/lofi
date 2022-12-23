#include "Debug.h"
#include <ToString/ToString.h>
#include <TTY0/puts.h>

char LogNumberStr[256] = { 0 };

void LogNumber(uint64_t Number, bool IsHex)
{
	if (!IsHex) uint2str(Number, &LogNumberStr[0]);
	else hex2str(Number, &LogNumberStr[0]);
	fputs(LogNumberStr, stdout);
}

void LogNumber(int64_t Number)
{
	int2str(Number, &LogNumberStr[0]);
	fputs(LogNumberStr, stdout);
}