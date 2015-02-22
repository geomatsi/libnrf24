#ifndef RF24_DEBUG_H
#define RF24_DEBUG_H

#ifndef LIB_RF24_DEBUG

#define panic(fmt, ...) ;
#define printk(fmt, ...) ;
#define trace(fmt, ...) ;
#define dbg(fmt, ...) ;

#else

extern void dbg(const char *fmt, ...);
extern void panic(const char *fmt, ...);
extern void trace(const char *fmt, ...);
extern void printk(const char *fmt, ...);

#endif

#endif /* RF24_DEBUG_H */
