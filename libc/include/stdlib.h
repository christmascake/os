#ifndef _STDLIB_H
#define _STDLIB_H 1

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__)) void abort(void);

int	      atoi(const char *nptr);
long int      atol(const char *nptr);
long long int atoll(const char *nptr);

long int strtol(const char *restrict nptr, char **restrict endptr, int base);
long long int	  strtoll(const char *restrict nptr, char **restrict endptr,
			  int base);
unsigned long int strtoul(const char *restrict nptr, char **restrict endptr,
			  int base);
unsigned long long int strtoull(const char *restrict nptr,
				char **restrict endptr, int base);

char *itoa(int value, char *str, int base);
char *itos(int value, char *str);

int abs(int n);

#ifdef __cplusplus
}
#endif

#endif
