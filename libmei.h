/* Copyright 2014 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __LIBMEI_H__
#define __LIBMEI_H__

#include <linux/uuid.h>
#include <linux/mei.h>
#include <stdbool.h>
#include <stdint.h>

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
	bool profile;
};

struct mei *mei_alloc(const uuid_le *guid,
		unsigned char req_protocol_version, bool verbose);

void mei_free(struct mei *me);

int mei_init(struct mei *me, const uuid_le *guid,
		unsigned char req_protocol_version, bool verbose);
void mei_deinit(struct mei *me);

int mei_connect(struct mei *me);

ssize_t mei_recv_msg(struct mei *me, unsigned char *buffer,
			size_t len, unsigned long timeout);

ssize_t mei_send_msg(struct mei *me, const unsigned char *buffer,
			size_t len, unsigned long timeout);

int mei_set_dma_buf(struct mei *me, const char *buf, size_t length);

#ifdef __cplusplus
}
#endif /*  __cplusplus */

#endif /* __LIBMEI_H__ */
