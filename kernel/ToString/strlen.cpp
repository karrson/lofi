#include "strlen.h"

size_t strlen_length = 0;
size_t strlen (const char *s)
{
	strlen_length = 0;
	for (; *s != '\0'; s++) strlen_length++;
	return strlen_length;
}