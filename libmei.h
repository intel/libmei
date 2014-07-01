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
#ifndef __LIBMEI_H__
#define __LIBMEI_H__

#include <linux/uuid.h>
#include <linux/mei.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif /*  __cplusplus */

enum {
	MEI_CL_STATE_ZERO = 0,
	MEI_CL_STATE_INTIALIZED = 1,    /** client is initialized */
	MEI_CL_STATE_CONNECTED,         /** client is connected */
	MEI_CL_STATE_DISCONNECTED,      /** client is disconnected */
	MEI_CL_STATE_NOT_PRESENT,       /** client with GUID is not present in the system */
	MEI_CL_STATE_ERROR,             /** client is in error state */
};

struct mei {
	uuid_le guid;
	unsigned int buf_size;
	unsigned char prot_ver;
	int fd;
	int state;
	int last_err;
	bool verbose;
};

#define MEI_DEFAULT_DEVICE "/dev/mei"

struct mei *mei_alloc(const char *device, const uuid_le *guid,
		unsigned char req_protocol_version, bool verbose);

void mei_free(struct mei *me);

int mei_init(struct mei *me, const char *device, const uuid_le *guid,
		unsigned char req_protocol_version, bool verbose);
void mei_deinit(struct mei *me);

int mei_connect(struct mei *me);

int mei_get_fd(struct mei *me);

ssize_t mei_recv_msg(struct mei *me, unsigned char *buffer, size_t len);

ssize_t mei_send_msg(struct mei *me, const unsigned char *buffer, size_t len);

int mei_set_dma_buf(struct mei *me, const char *buf, size_t length);

#ifdef __cplusplus
}
#endif /*  __cplusplus */

#endif /* __LIBMEI_H__ */
