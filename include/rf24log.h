#ifndef RF24_LOG_H
#define RF24_LOG_H

extern int printf(const char *fmt, ...);

/* Defined RF24 log levels:
 *   - ERROR	1
 *   - INFO		2
 *   - DEBUG	3
 *   - TRACE	4
 */

#define RF24TAG	"RF24"

#ifndef LOG_LEVEL
#define LOG_LEVEL 3
#endif

#if LOG_LEVEL < 1 || LOG_LEVEL > 4
#error "Incorrect log level"
#endif

#if LOG_LEVEL > 0
#define rf24_err(...)			printf(RF24TAG __VA_ARGS__)
#else
#define rf24_err(...) ;
#endif

#if LOG_LEVEL > 1
#define rf24_info(...)			printf(RF24TAG __VA_ARGS__)
#else
#define rf24_info(...) ;
#endif

#if LOG_LEVEL > 2
#define rf24_debug(...)			printf(RF24TAG __VA_ARGS__)
#else
#define rf24_debug(...) ;
#endif

#if LOG_LEVEL > 3
#define rf24_trace(...)			printf(RF24TAG __VA_ARGS__)
#else
#define rf24_trace(...) ;
#endif

#endif /* RF24_LOG_H */
