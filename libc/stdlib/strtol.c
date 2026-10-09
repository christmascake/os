#include <limits.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline bool is_valid_subject_char(const char c, const int base)
{
	if (base <= 10 && c >= '0' && c < ('0' + base))
		return true;

	if (base > 10) {
		if (c >= '0' && c < 58)
			return true;
		if (c >= 'A' && c < ('A' + (base - 10)))
			return true;
		if (c >= 'a' && c < ('a' + (base - 10)))
			return true;
	}

	return false;
}

static long int safe_concat(const long int a, const long int digit,
			    const int base, const int sign)
{
	auto t = a;

	if (sign == 1) {
		if (__builtin_smull_overflow(t, base, &t))
			return LONG_MAX;
		if (__builtin_saddl_overflow(t, digit, &t))
			return LONG_MAX;
	} else {
		if (__builtin_smull_overflow(t, base, &t))
			return LONG_MIN;
		if (__builtin_saddl_overflow(t, digit, &t))
			return LONG_MIN;
	}

	return t;
}

/* INFO: 7.22.1.4 The strtol, strtoll, strtoul, and strtoull functions */

// TODO: refactor this to something less insane, use descriptive functions, add errno support

long int strtol(const char *restrict nptr, char **restrict endptr, int base)
{
	char *c = (char *)nptr;

	if (endptr)
		*endptr = (char *)nptr;

	if (base < 0 || base > 36 || base == 1)
		return 0;

	while (isspace((unsigned char)*c)) c++;

	int signage = (c[0] == '-') ? -1 : 1;

	if (c[0] == '+' || c[0] == '-') {
		if (c[1] == '+' || c[1] == '-')
			return 0;
		c++;
	}

	if (base == 0) {
		if (c[0] == '0' && (c[1] == 'x' || c[1] == 'X') &&
		    is_valid_subject_char(c[2], 16))
			base = 16;
		else if (c[0] == '0')
			base = 8;
		else
			base = 10;
	}

	bool digit_found = false;

	for (size_t i = 0; i < strlen(c); i++) {
		if (is_valid_subject_char(c[i], base)) {
			digit_found = true;
			break;
		}
	}

	if (!digit_found) {
		if (endptr)
			*endptr = (char *)nptr;
		return 0;
	}

	if (base == 8 && c[0] == '0')
		c++;
	else if (base == 16 && c[0] == '0' && (c[1] == 'x' || c[1] == 'X')) {
		if (!is_valid_subject_char(c[2], 16)) {
			if (endptr) {
				*endptr = (char *)nptr;
				return 0;
			}
		}
		c += 2;
	}

	if (endptr)
		*endptr = (char *)c;

	long int ret = 0;

	while (is_valid_subject_char(*c, base)) {
		if (*c >= '0' && *c <= '9') {
			ret = safe_concat(ret, *c - '0', base, signage);
		} else if (*c >= 'A' && *c <= 'Z') {
			ret = safe_concat(ret, (*c - 'A') + 10, base, signage);
		} else if (*c >= 'a' && *c <= 'z') {
			ret = safe_concat(ret, (*c - 'a') + 10, base, signage);
		}

		c++;

		if (endptr)
			*endptr = (char *)c;
	}

	ret *= signage;
	return ret;
}
