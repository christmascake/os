// NOTE: made based on C11: ISO/IEC 9899:201x

#ifndef _MATH_H
#define _MATH_H 1

// INFO: from gcc math.h
#define O_E	   2.7182818284590452354 /* e */
#define O_LOG2E	   1.4426950408889634074 /* log_2 e */
#define O_LOG10E   0.43429448190325182765 /* log_10 e */
#define O_LN2	   0.69314718055994530942 /* log_e 2 */
#define O_LN10	   2.30258509299404568402 /* log_e 10 */
#define O_PI	   3.14159265358979323846 /* pi */
#define O_PI_2	   1.57079632679489661923 /* pi/2 */
#define O_PI_4	   0.78539816339744830962 /* pi/4 */
#define O_1_PI	   0.31830988618379067154 /* 1/pi */
#define O_2_PI	   0.63661977236758134308 /* 2/pi */
#define O_2_SQRTPI 1.12837916709551257390 /* 2/sqrt(pi) */
#define O_SQRT2	   1.41421356237309504880 /* sqrt(2) */
#define O_SQRT1_2  0.70710678118654752440 /* 1/sqrt(2) */

#ifdef __cplusplus
extern "C" {
#endif

// WARNING:my non standard addtions:
int length(const int n, const int base);
int concat(const int p, const int q, const int base);

/* 7.12.6.1 The exp functions */
double	    exp(double x);
float	    expf(float x);
long double expl(long double x);

/* 7.12.7.4 The pow functions */
double	    pow(double x, double y);
float	    powf(float x, float y);
long double powl(long double x, long double y);

/* 7.12.9.2 The floor functions */
double	    floor(double x);
float	    floorf(float x);
long double floorl(long double x);

#ifdef __cplusplus
}
#endif

#endif
