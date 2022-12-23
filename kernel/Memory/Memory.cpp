#include "Memory.h"

int memcmp(const void* s1, const void* s2, size_t n)
{
	uint8_t* a = (uint8_t*)s1;
	uint8_t* b = (uint8_t*)s2;
	
	for (int i = 0; i < n; i++)
	{
		uint8_t A = 0;
		if (s1 != NULL) A = *a;
		uint8_t B = 0;
		if (s2 != NULL) B = *b;
		if (A < B) return -1;
		if (A > B) return 1;
		if (s1 != NULL) a++;
		if (s2 != NULL) b++;
	}
	
	return 0;
}
