#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum flag_t {
	FLAG_NONE	  = 0,
	FLAG_JUSTIFY_LEFT = 1 << 0,
	FLAG_FORCE_SIGN	  = 1 << 1,
	FLAG_PREFIX_SPACE = 1 << 2,
	FLAG_ALTERNATIVE  = 1 << 3,
	FLAG_LEADING_ZERO = 1 << 4,

	FLAG_ANY = FLAG_JUSTIFY_LEFT | FLAG_FORCE_SIGN | FLAG_PREFIX_SPACE |
		FLAG_ALTERNATIVE | FLAG_LEADING_ZERO
};

enum field_width_t {
	FIELD_WIDTH_NONE = 0,
	FIELD_WIDTH_ARG	 = 1 << 0,
	FIELD_WIDTH_NUM	 = 1 << 1,

	FIELD_WIDTH_ANY = FIELD_WIDTH_ARG | FIELD_WIDTH_NUM
};

enum precision_t {
	PRECISION_NONE	= 0,
	PRECISION_ARG	= 1 << 0,
	PRECISION_NUM	= 1 << 1,
	PRECISION_START = 1 << 2,

	PRECISION_ANY = PRECISION_ARG | PRECISION_NUM | PRECISION_START
};

enum length_t {
	LENGTH_NONE	 = 0,
	LENGTH_HH	 = 1 << 0,
	LENGTH_H	 = 1 << 1,
	LENGTH_L	 = 1 << 2,
	LENGTH_LL	 = 1 << 3,
	LENGTH_J	 = 1 << 4,
	LENGTH_Z	 = 1 << 5,
	LENGTH_T	 = 1 << 6,
	LENGTH_CAPITAL_L = 1 << 7,

	LENGTH_HAS_SIGN = LENGTH_HH | LENGTH_H | LENGTH_L | LENGTH_LL |
		LENGTH_J | LENGTH_Z | LENGTH_T,

	LENGTH_ANY = LENGTH_HH | LENGTH_H | LENGTH_L | LENGTH_LL | LENGTH_J |
		LENGTH_Z | LENGTH_T | LENGTH_CAPITAL_L
	// TODO: just make custom flags here for sign and shit
};

enum conversion_t {
	CONVERSION_NONE = 0,

	CONVERSION_INT	    = 1 << 0,
	CONVERSION_UINT	    = 1 << 1,
	CONVERSION_DOUBLE_F = 1 << 2,
	CONVERSION_DOUBLE_E = 1 << 3,
	CONVERSION_DOUBLE_G = 1 << 4,
	CONVERSION_DOUBLE_A = 1 << 5,
	CONVERSION_CHAR	    = 1 << 6,
	CONVERSION_STRING   = 1 << 7,
	CONVERSION_POINTER  = 1 << 8,
	CONVERSION_PWRITTEN = 1 << 9,
	CONVERSION_PERCENT  = 1 << 10,

	CONVERSION_HAS_SIGN = CONVERSION_UINT | CONVERSION_INT |
		CONVERSION_CHAR,

	CONVERSION_ANY = CONVERSION_INT | CONVERSION_UINT |
		CONVERSION_DOUBLE_F | CONVERSION_DOUBLE_E |
		CONVERSION_DOUBLE_G | CONVERSION_DOUBLE_A | CONVERSION_CHAR |
		CONVERSION_STRING | CONVERSION_POINTER | CONVERSION_PWRITTEN |
		CONVERSION_PERCENT

};

static inline enum flag_t what_flag_type(const char c)
{
	switch (c) {
	case '-': return FLAG_JUSTIFY_LEFT;
	case '+': return FLAG_FORCE_SIGN;
	case ' ': return FLAG_PREFIX_SPACE;
	case '#': return FLAG_ALTERNATIVE;
	case '0': return FLAG_LEADING_ZERO;
	default: return FLAG_NONE;
	}
}

// TODO: maybe keep a flag count, no reliance on spec_count (?)
struct format_info {
	/* WARNING: a max of 5 flags is not enforced by the standard (?) */
	enum flag_t flags;

	/* NOTE: either from in the string or from argument when '*' is used */
	enum field_width_t field_width_type;
	int		   field_width;

	/* NOTE: either from in the string or from argument when '*' is used */
	enum precision_t precision_type;
	int		 precision;

	enum length_t length_type;

	enum conversion_t conversion_type;

	union conversion_data_t {
		signed int    con_sint;
		unsigned int  con_uint;
		double	      con_double;
		unsigned char con_uchar;

		/* WARNING: standard says "character type,"
                 * not necessarily 'char'
                 */
		char	   *con_string;
		void	   *con_pvoid;
		signed int *con_pwritten;

		/* possibly unnecessary */
		char con_percent;
	} conversion_data;
	// TODO: set to default?

	bool has_signedness;
};

static const struct format_info default_info = {
	.flags		  = FLAG_NONE,
	.field_width_type = FIELD_WIDTH_NONE,
	.field_width	  = 0,
	.precision_type	  = PRECISION_NONE,
	.precision	  = 0,
	.length_type	  = LENGTH_NONE,
	.conversion_type  = CONVERSION_NONE,
	.conversion_data  = { 0 },
	.has_signedness	  = false,
};

static char *debug_buf = NULL;

static struct format_info info = default_info;

static inline bool is_flag(const char c)
{
	return what_flag_type(c) & FLAG_ANY;
}

// returns the number of flags used
static inline int handle_flags(const char *str)
{
	/* we assume this function is called when
         * is_flag(str[0]) is true
         */
	info.flags |= what_flag_type(str[0]);
	int count = 1; /* the first flag */

	/* check if the next 4 chars are flags */
	for (size_t i = 0; i < 4; i++) {
		if (!is_flag(str[i + 1]))
			return count;
		info.flags |= what_flag_type(str[i + 1]);
		count++;
	}

	/* zero flag is ignore if left justify is also present */
	if (info.flags & FLAG_JUSTIFY_LEFT && info.flags & FLAG_LEADING_ZERO)
		info.flags &= ~FLAG_LEADING_ZERO;

	/* space flag is ignore if force sign is also present */
	if (info.flags & FLAG_PREFIX_SPACE && info.flags & FLAG_FORCE_SIGN)
		info.flags &= ~FLAG_PREFIX_SPACE;

	return count; /* how many flags we found */
}

static inline enum field_width_t what_field_width_type(const char c)
{
	if (c == '*')
		return FIELD_WIDTH_ARG;
	if (c >= '1' && c <= '9')
		return FIELD_WIDTH_NUM;

	return FIELD_WIDTH_NONE;
}

static inline bool is_field_width(const char c)
{
	return (what_field_width_type(c) & FIELD_WIDTH_ANY);
}

/* WARNING: this can call va_arg */
static inline int handle_field_width(const char *fmt, va_list *args)
{
	/* we assume this function is called when
         * is_field_width(fmt[0]) is true
         */
	info.field_width_type = what_field_width_type(fmt[0]);

	/* it must be at least 1, or it can be less than 1 if
        type == FIELD_WIDTH_ARG and the arg is less than 1 */
	info.field_width = 1;
	int count	 = 0;

	if (info.field_width_type == FIELD_WIDTH_ARG) {
		info.field_width = va_arg(*args, int);

		/* WARNING: may need to look closer at this */
		if (info.field_width < 0) {
			info.field_width *= -1;
			info.flags |= FLAG_JUSTIFY_LEFT;
		}
		return 1;
	}

	for (size_t i = 0; (fmt[i] >= '0' && fmt[i] <= '9'); i++) {
		count++;
	}

	info.field_width = atoi(fmt);

	return count;
}

static inline enum precision_t what_precision_type(const char c)
{
	if (c == '.')
		return PRECISION_START;
	if (c == '*') {
		return PRECISION_ARG;
	}

	/* NOTE:maybe this should include 0 (?) */
	if (c >= '1' || c <= '9')
		return PRECISION_NUM;

	return PRECISION_NONE;
}

static inline bool is_precision(const char c)
{
	return (what_precision_type(c) & PRECISION_ANY);
}

/* WARNING: this can call va_arg */
static inline int handle_precision(const char *str, va_list *args)
{
	/* WARNING: unlike field width, precision starts with a sentinel '.' */

	/* we assume this function is called when
         * what_precision(str[0]) == PRECISION_START is true
         */
	str++;

	info.precision_type = what_precision_type(str[0]);

	if (info.precision_type == PRECISION_ARG && va_arg(*args, int) < 0) {
		info.precision_type = PRECISION_NONE;
		return 2;
	}

	if (info.precision_type == PRECISION_ARG) {
		info.precision = va_arg(*args, int);
		return 2; /* ".*" */
	}

	/* must be at least "." */
	int count = 1;

	for (size_t i = 0; (what_precision_type(str[i]) == PRECISION_NUM);
	     i++) {
		count++;
	}

	/* if precision is just '.' it should be 0 */
	if (count == 1) {
		info.precision = 0;
		return 1; /* "." */
	}

	info.precision = atoi(str);
	return count;
}

static inline enum length_t what_length_type(const char *str)
{
	switch (str[0]) {
	case 'h':
		if (str[1] == 'h')
			return LENGTH_HH;
		else
			return LENGTH_H;

	case 'l':
		if (str[1] == 'l')
			return LENGTH_LL;
		else
			return LENGTH_L;

	case 'j': return LENGTH_J;
	case 'z': return LENGTH_Z;
	case 't': return LENGTH_T;
	case 'L': return LENGTH_CAPITAL_L;

	default: return LENGTH_NONE;
	}
}

static inline bool is_length(const char *str)
{
	return (what_length_type(str) & LENGTH_ANY);
}

static inline int handle_length(const char *str)
{
	const auto type	 = what_length_type(str);
	info.length_type = type;

	if (type == LENGTH_HH || type == LENGTH_LL)
		return 2;

	return 1;
}

static inline enum conversion_t what_conversion_type(const char c)
{
	switch (c) {
	case 'd':
	case 'i': return CONVERSION_INT;

	case 'o':
	case 'u':
	case 'x':
	case 'X': return CONVERSION_UINT;

	case 'f':
	case 'F': return CONVERSION_DOUBLE_F;

	case 'e':
	case 'E': return CONVERSION_DOUBLE_E;

	case 'g':
	case 'G': return CONVERSION_DOUBLE_G;

	case 'a':
	case 'A': return CONVERSION_DOUBLE_A;

	case 'c': return CONVERSION_CHAR;
	case 's': return CONVERSION_STRING;
	case 'p': return CONVERSION_POINTER;
	case 'n': return CONVERSION_PWRITTEN;
	case '%': return CONVERSION_PERCENT;

	default: return CONVERSION_NONE;
	}
}

static inline bool is_conversion(const char c)
{
	return (what_conversion_type(c) & CONVERSION_ANY);
}

static inline int handle_conversion(const char c)
{
	info.conversion_type = what_conversion_type(c);

	return 1; /* always 1 width */
}

static inline int handle_specifiers(const char *format, va_list *args)
{
	/* current conversion specifier we are processing */
	/* so we know how far to skip ahead */
	int spec_count = 0;

	// TODO: if i call these inline i should handle them inline!!!

	if (is_flag(format[spec_count + 1])) {
		spec_count += handle_flags(&format[spec_count + 1]);
	}

	if (is_field_width(format[spec_count + 1])) {
		spec_count += handle_field_width(&format[spec_count + 1], args);
	}

	if (what_precision_type(format[spec_count + 1]) == PRECISION_START) {
		spec_count += handle_precision(&format[spec_count + 1], args);
	}

	if (is_length(&format[spec_count + 1])) {
		spec_count += handle_length(&format[spec_count + 1]);
	}

	// TODO: this must say so if no conversion was found
	if (is_conversion(format[spec_count + 1])) {
		spec_count += handle_conversion(format[spec_count + 1]);
	}

	return spec_count;
}

static inline void check_signedness(void)
{
	if (info.conversion_type & CONVERSION_HAS_SIGN)
		info.has_signedness = true;

	/* WARNING: we override signedness if length does, might cause problems */
	if (info.length_type != LENGTH_NONE)
		info.has_signedness = info.length_type & LENGTH_HAS_SIGN;
}

/* WARNING: "For d, i, o, u, x, and X conversions, if a precision is specified, the 0 flag is ignored. For other
   conversions, the behavior is undefined." 
 */

static inline void handle_conversion_data(va_list *args)
{
#define get_arg(type) va_arg(*args, type)
	union conversion_data_t *t = &info.conversion_data;

	switch (info.conversion_type) {
	case CONVERSION_INT: t->con_sint = get_arg(signed int); break;
	case CONVERSION_UINT: t->con_uint = get_arg(unsigned int); break;
	case CONVERSION_DOUBLE_F:
	case CONVERSION_DOUBLE_E:
	case CONVERSION_DOUBLE_G:
	case CONVERSION_DOUBLE_A: t->con_double = get_arg(double); break;

	case CONVERSION_CHAR:
		/* BUG: possible UB */
		t->con_uchar = (unsigned char)get_arg(unsigned char);
		break;
	case CONVERSION_STRING: t->con_string = get_arg(char *); break;
	case CONVERSION_POINTER: t->con_pvoid = get_arg(void *); break;
	case CONVERSION_PWRITTEN:
		t->con_pwritten = get_arg(signed int *);
		break;

		/* BUG: percent is scuffed */
	case CONVERSION_PERCENT: break;
	default: break;
	}
}

static bool print(const char *data, size_t length)
{
	const unsigned char *bytes = (const unsigned char *)data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

static int debug_count = 0;

static bool debug_print(const char *data, size_t length)
{
	const unsigned char *bytes = (const unsigned char *)data;
	for (size_t i = 0; i < length; i++) {
		debug_buf[debug_count] = bytes[i];
		debug_count++;
	}
	return true;
}

/* TODO: rename and refocus this */
static inline int print_formatted_string(const char *str, va_list *args)
{
	/* TODO: properly handle field width, conversion digits
         * and + or - and stuff take up space, field width is minimum
         */

	/* the conversion we are using */
	handle_conversion_data(args);

	/* check if we must print a sign */
	check_signedness();
	auto need_to_print_sign =
		((info.flags & FLAG_FORCE_SIGN) && info.has_signedness);

	if (need_to_print_sign) {
		auto sign = (info.conversion_data.con_sint < 0) ? '-' : '+';
		/* BUG: DEBUG CODE */
		debug_buf[debug_count] = sign;
		debug_count++;
	}

	/* BUG: might be wrong understanding of ' ' vs '0' vs precision */
	const char padding_char = (info.flags & FLAG_LEADING_ZERO) ? '0' : ' ';

	/* if no justify left, we can start printing padding */
	if ((info.field_width_type & FIELD_WIDTH_ANY) &&
	    !(info.flags & FLAG_JUSTIFY_LEFT)) {
		auto padding_remaining = info.field_width;

		if (need_to_print_sign)
			padding_remaining--;

		padding_remaining -= length(info.conversion_data.con_sint, 10);

		/* BUG: DEBUG CODE */
		while (padding_remaining--) {
			debug_buf[debug_count] = padding_char;
			debug_count++;
		}
	}

	char istr[100] = { '\0' };
	itos(info.conversion_data.con_sint, istr);

	/* BUG: DEBUG CODE */
	debug_print(istr, strlen(istr));

	return 1; //tmp
}

int fprintf(char *buf, const char *restrict format, ...)
{
	int written = 0;

	va_list args;
	va_start(args, format);

	debug_buf   = buf;
	debug_count = 0;

	while (*format) {
		/* the maximum amount of remaining characters we can safely
                print, currently ill-defined */
		const size_t max_rem = (size_t)(INTMAX_MAX - written);

		/* are we pointing to a conversion symbol? */
		if (format[0] == '%') {
			info	       = default_info;
			int spec_count = handle_specifiers(format, &args);
			int printed_count =
				print_formatted_string(format, &args);

			written += printed_count;
			format += spec_count;
		} else
			putchar(format[0]);

		format++;
	}

	/* 9 If a conversion specification is invalid, the behavior is undefined. If any argument is
         * not the correct type for the corresponding conversion specification, the behavior is
         * undefined
         */

	va_end(args);
	return written;
}

/* INFO: A negative field width argument is taken as a - flag
   followed by a positive field width. A negative precision argument is taken as if the
   precision were omitted
*/

/* INFO: if the user doesn't follow this order, the behaviour is undefined */

/*
 * NOTE:
 * order: zero or more flags (any order) .
 *        optional minimum field width .
 *        optional precision .
 *        optional length modifier .
 *        conversion specifier
 */

/* WARNING: field width and precision can fuck each other, (?field - precision?) */
