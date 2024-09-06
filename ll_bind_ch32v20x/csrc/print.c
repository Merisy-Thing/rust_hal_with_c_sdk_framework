#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "print.h"

#ifdef PRINT_LOG
extern void ll_putc(char c);

const char LOOKUP[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46};

static void ll_ch2hex(char ch, char p_hex[2])
{
    p_hex[0] = LOOKUP[(ch >> 4) & 0x0F];
    p_hex[1] = LOOKUP[ch & 0x0F];
}

static void ll_puts(char *s)
{
    char c;
    while((c = *s++)) {
        ll_putc(c);
    }
}

static int ll_itoa(int32_t value, int32_t radix, char *p_buf, int32_t size)
{
	int32_t index = size;
	bool neg = false;
	if ((value < 0) && (radix == 10)) {
		neg = true;
		value = -value;
	}

	p_buf[--index]='\0';
	while(1) {
		int32_t rem = value % radix;
		value /= radix;
		p_buf[--index] = LOOKUP[rem];
		if((value == 0) || (index == 0)) {
			break;
		}
	}
	if(neg && (index > 0)) {
		p_buf[--index] = '-';
	}

	return index;
}

void ll_vprintf(const char *fmt, va_list va)
{
    char str[12 + 4];
    char ch;
    int32_t idx;

    while ((ch = *(fmt++))) {
        if (ch != '%') {
            ll_putc(ch);
            continue;
        }

        char *ptr;
        ch = *(fmt++);

        while (ch >= '0' && ch <= '9') {
            ch = *(fmt++);
        }

        switch (ch)
        {
        case '\0':
            return;
        case 'c':
            ll_putc((char)(va_arg(va, int32_t)));
        break;
        case 'd':
            idx = ll_itoa((int32_t)va_arg(va, int32_t), 10, str, sizeof(str));
            ll_puts(&str[idx]);
            break;
        case 'x':
        case 'X':
            idx = ll_itoa((uint32_t)va_arg(va, uint32_t), 16, str, sizeof(str));
            ll_puts(&str[idx]);
            break;
        case 's':
            ptr = va_arg(va, char *);
            ll_puts(ptr);
            break;
		 case 'S':
            ptr = va_arg(va, char *);
            idx = (int32_t)va_arg(va, int32_t);
			while(idx--) {
				ll_putc(*ptr++);
			}
            break;
        case 'y':
            ptr = va_arg(va, char *);
            idx = (int32_t)va_arg(va, int32_t);
            str[2] = ' ';
            str[3] = '\0';
            while (idx--) {
                ll_ch2hex(*ptr++, str);
                ll_puts(str);
            }
            break;
        default:
            ll_putc(ch);
            break;
        }
    }
}
#endif
