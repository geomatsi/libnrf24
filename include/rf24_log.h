#ifndef __NRF24_LOG_H__
#define __NRF24_LOG_H__

int printf(const char *fmt, ...);

/* Defined RF24 log levels:
 *   - ERROR	1
 *   - INFO	2
 *   - DEBUG	3
 *   - TRACE	4
 */

#define RF24TAG	"RF24"

#ifndef NRF24_LOG_LEVEL
#define NRF24_LOG_LEVEL 3
#endif

#if NRF24_LOG_LEVEL < 1 || NRF24_LOG_LEVEL > 4
#error "Incorrect log level"
#endif

#if NRF24_LOG_LEVEL > 0
#define rf24_err(...)			printf(RF24TAG " : " __VA_ARGS__)
#else
#define rf24_err(...) ;
#endif

#if NRF24_LOG_LEVEL > 1
#define rf24_info(...)			printf(RF24TAG " : " __VA_ARGS__)
#else
#define rf24_info(...) ;
#endif

#if NRF24_LOG_LEVEL > 2
#define rf24_debug(...)			printf(RF24TAG " : " __VA_ARGS__)
#else
#define rf24_debug(...) ;
#endif

#if NRF24_LOG_LEVEL > 3
#define rf24_trace(...)			printf(RF24TAG " : " __VA_ARGS__)
#else
#define rf24_trace(...) ;
#endif

#endif /* __NRF24_LOG_H__ */
