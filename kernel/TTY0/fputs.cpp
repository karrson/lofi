#include "fputs.h"
#include <ToString/strlen.h>

volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

int fputs (const char *s, int stream)
{
	if (stream == stdout)
	{
		struct limine_terminal *terminal = terminal_request.response->terminals[0];
		terminal_request.response->write(terminal, s, strlen(s));
		return 1;
	}
	return -1;
}