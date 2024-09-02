#ifndef __LL_PRINT_H__
#define __LL_PRINT_H__

#define PRINT_LOG

void ll_vprintf(const char *fmt, va_list va);

#ifdef PRINT_LOG
#define println(fmt, ...)    ll_invoke(ID_LOG_PRINT, fmt "\r\n", ##__VA_ARGS__)
#define print(fmt, ...)      ll_invoke(ID_LOG_PRINT, fmt, ##__VA_ARGS__)
#else
#define println(...)
#define print(...) 
#endif

#endif //__LL_PRINT_H__
