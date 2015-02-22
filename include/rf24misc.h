#ifndef RF24_MISC_H
#define RF24_MISC_H

#define min_t(type, x, y) ({	\
		type __min1 = (x);		\
		type __min2 = (y);		\
		__min1 < __min2 ? __min1: __min2; })

#define BIN(x)		0b##x

#define _BV(x)		(1 << (x))

#endif /* RF24_MISC_H */
