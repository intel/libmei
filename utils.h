/******************************************************************************
 * Intel Management Engine Interface (Intel MEI) Linux driver
 * Intel MEI Interface Header
 *
 * This file is provided under BSD license.
 *
 * BSD LICENSE
 *
 * Copyright(c) 2003 - 2014 Intel Corporation. All rights reserved.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name Intel Corporation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
#ifndef __LIBMEI_UTILS_H__
#define __LIBMEI_UTILS_H__
#include <stdio.h>
#include <sys/time.h>

static inline void dump_buffer(const unsigned char *buf, size_t buf_len) 
{
	size_t i;
	uint32_t *b32 = (uint32_t *)buf;
	for (i = 0 ; i < buf_len/4 ; i++)
		printf("%08zX:0x%08X\n", i, b32[i]);
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
