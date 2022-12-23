#include "puts.h"

int puts (const char *s)
{
	fputs(s, stdout);
	return fputs("\n", stdout);
}