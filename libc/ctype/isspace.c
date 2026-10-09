#include <ctype.h>

static inline bool is_standard_whitespace(const int c)
{
	switch (c) {
	case ' ':
	case '\f':
	case '\n':
	case '\r':
	case '\t':
	case '\v': return c;
	default: return 0;
	}
}

int isspace(int c)
{
	// TODO:implement locale checking
	return is_standard_whitespace(c);
}
