#ifndef __NRF24_STD_H__
#define __NRF24_STD_H__

#include <stdint.h>

#define BIT(x)	(1 << (x))
#define BIN(x)	(0b##x)

#define min_t(type, x, y) ({	\
	type __min1 = (x);	\
	type __min2 = (y);	\
	__min1 < __min2 ? __min1: __min2; })

#endif /* __NRF24_STD_H__ */
