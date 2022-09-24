// Thanks maxem

// header
#include "sysclib.h"

// struct
typedef struct
{
	char *buf;
	const size_t *len;
	size_t cpylen;
} xprintf_ctx;

// function
static void snprnt_cb(void *ctx, int ch)
{
	if(ch == 0x200 || ch == 0x201)
		return;

	xprintf_ctx *ctx_p = (xprintf_ctx *)ctx;

	if(ctx_p->cpylen < *(ctx_p->len))
	{
		ctx_p->buf[ctx_p->cpylen] = ch;
		ctx_p->cpylen++;
	}

	return;
}

int vsnprintf(char *buf, size_t n, const char *fmt, va_list ap)
{
	xprintf_ctx ctx = { buf, (const size_t *)&n, 0 };

	prnt(snprnt_cb, (void *)&ctx, fmt, ap);

	if(ctx.cpylen >= *(ctx.len))
	{
		ctx.buf[*(ctx.len) - 1] = '\0';
	}
	else
	{
		ctx.buf[ctx.cpylen] = '\0';
	}

	return ctx.cpylen;
}

int snprintf(char *buf, size_t n, const char *fmt, ...)
{
	va_list ap;
	int res;

	va_start(ap, fmt);
	res = vsnprintf(buf, n, fmt, ap);
	va_end(ap);

	return res;
}

