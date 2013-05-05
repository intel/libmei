#ifndef __LIBMEI_UTILS_H__
#define __LIBMEI_UTILS_H__
#include <stdio.h>
#include <sys/time.h>

static inline void dump_buffer(const unsigned char *buf, size_t buf_len) 
{
	int i;
	uint32_t *b32 = (uint32_t *)buf;
	for (i = 0 ; i < buf_len/4 ; i++)
		printf("%08X:0x%08X\n", i, b32[i]);
}

#define timestamp_declare(_s,_e) 		\
	struct timeval _s,_e;

#define timestamp_get(c, _tv)	do {		\
	if (c->profile)				\
		gettimeofday(&_tv, NULL);	\
} while (0)

#define timestamp_print(c, _s,_e, fmt, ...) do {	\
	if (c->profile) {				\
		struct timeval _res;			\
		timersub(&_e,&_s, &_res);		\
		printf(fmt, ##__VA_ARGS__);		\
		printf("usec = %ld\n",			\
		_res.tv_sec * 1000000 + _res.tv_usec);	\
	}						\
} while (0)

#endif /* __LIBMEI_UTILS_H__ */
