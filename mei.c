/******************************************************************************
 * Intel Management Engine Interface (Intel MEI) Linux driver
 * Intel MEI Interface Header
 *
 * This file is provided under a dual BSD/GPLv2 license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * GPL LICENSE SUMMARY
 *
 * Copyright(c) 2012 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110,
 * USA
 *
 * The full GNU General Public License is included in this distribution
 * in the file called LICENSE.GPL.
 *
 * Contact Information:
 *	Intel Corporation.
 *	linux-mei@linux.intel.com
 *	http://www.intel.com
 *
 * BSD LICENSE
 *
 * Copyright(c) 2003 - 2012 Intel Corporation. All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <linux/mei.h>

#include "libmei.h"

#include "utils.h"

/*****************************************************************************
 * Intel Management Enginin Interface
 *****************************************************************************/
#ifdef ANDROID
#define LOG_TAG "libmei"
#include <cutils/log.h>
#define mei_msg(_me, fmt, ARGS...) ALOGV_IF(_me->verbose, fmt, ##ARGS)
#define mei_err(_me, fmt, ARGS...) ALOGE_IF(_me->verbose, fmt, ##ARGS)

#else
#define mei_msg(_me, fmt, ARGS...) do {         \
	if (_me->verbose)                       \
		fprintf(stderr, "me: " fmt, ##ARGS);	\
} while (0)

#define mei_err(_me, fmt, ARGS...) do {         \
	fprintf(stderr, "me: error: " fmt, ##ARGS); \
} while (0)
#endif /* ANDROID */
void mei_dump_hex_buffer(const unsigned char* buf, size_t len)
{
 
	int j = 0;
	while (len-- > 0) {
		fprintf(stdout, "%02X ", *buf++);
		if (++j == 16) {
			fprintf(stdout, "\n");
			j = 0;
		}
	}
	if (j)
	fprintf(stdout, "\n");

}

void mei_deinit(struct mei *me)
{
	if (!me)
		return;

	if (me->fd != -1)
		close(me->fd);
	me->fd = -1;
	me->buf_size = 0;
	me->prot_ver = 0;
	me->state = MEI_CL_STATE_ZERO;
}


int mei_connect(struct mei *me)
{
	struct mei_client *cl;
	struct mei_connect_client_data data;
	int result;

	if (me->state != MEI_CL_STATE_INTIALIZED)
		return EINVAL;

	memset(&data, 0, sizeof(data));

	memcpy(&data.in_client_uuid, &me->guid, sizeof(me->guid));

	result = ioctl(me->fd, IOCTL_MEI_CONNECT_CLIENT, &data);
	if (result) {
		mei_err(me, "IOCTL_MEI_CONNECT_CLIENT receive message. err=%d\n", result);
		return result;
	}
	cl = &data.out_client_properties;
	mei_msg(me, "max_message_length %d\n", cl->max_msg_length);
	mei_msg(me, "protocol_version %d\n", cl->protocol_version);

	if ((me->prot_ver > 0) &&
	     (cl->protocol_version != me->prot_ver)) {
		mei_err(me, "Intel MEI protocol version not supported\n");
		return EINVAL;
	}

	me->buf_size = cl->max_msg_length;
	me->prot_ver = cl->protocol_version;

	me->state =  MEI_CL_STATE_CONNECTED;

	return 0;
}

const char* mei_get_devname()
{
	const char* devname = getenv("MEI_DEV");
	return (NULL == devname) ? "/dev/mei" : devname;
}

bool mei_init(struct mei *me, const uuid_le *guid,
		unsigned char req_protocol_version, bool verbose)
{
	const char* devname = mei_get_devname();

	if (!me)
		return false;

	/* if me is unitialized it will close wrong file descriptor */
	me->fd = -1;
	mei_deinit(me);

	me->verbose = verbose;
	me->profile = false;

	me->fd = open(devname, O_RDWR);
	if (me->fd == -1) {
		mei_err(me, "Cannot establish a handle to the Intel MEI driver %s\n",
			devname);
		goto err;
	}
	mei_msg(me, "Opened %s: fd = %d\n", devname, me->fd);

	memcpy(&me->guid, guid, sizeof(*guid));
	me->prot_ver = req_protocol_version;

	me->state = MEI_CL_STATE_INTIALIZED;

	return true;
err:
	mei_deinit(me);
	return false;
}

struct mei *mei_alloc(const uuid_le *guid,
		unsigned char req_protocol_version, bool verbose)
{
	struct mei *me = malloc(sizeof(struct mei));
 
	if (!me)
		return NULL;

	if (!mei_init(me, guid, req_protocol_version, verbose)) {
		free(me);
		return NULL;
	}
	return me;
}

void mei_free(struct mei *me)
{
	if (!me)
		return;
	mei_deinit(me);
	free(me);
}

ssize_t mei_recv_msg(struct mei *me, unsigned char *buffer,
			ssize_t len, unsigned long timeout)
{
	ssize_t rc;
	timestamp_declare(s,e);

	if (!me)
		return -EINVAL;

	mei_msg(me, "call read length = %zd\n", len);

	timestamp_get(me, s);
	rc = read(me->fd, buffer, len);
	timestamp_get(me, e);
	if (rc < 0) {
		mei_err(me, "read failed with status %zd %s\n",
				rc, strerror(errno));
		mei_deinit(me);
	} else {
		mei_msg(me, "read succeeded with result %zd\n", rc);
		if (me->verbose)
			mei_dump_hex_buffer(buffer, rc);
	}
	timestamp_print(me, s, e, "mei read: ");
	return rc;
}

ssize_t mei_send_msg(struct mei *me, const unsigned char *buffer,
			ssize_t len, unsigned long timeout)
{
	struct timeval tv;
	ssize_t written;
	ssize_t rc;
	fd_set set;
	timestamp_declare(s,e);

	if (!me || !buffer)
		return -EINVAL;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000000;

	mei_msg(me, "call write length = %zd\n", len);
	if (me->verbose)
		mei_dump_hex_buffer(buffer, len);

	timestamp_get(me, s);
	written = write(me->fd, buffer, len);
	timestamp_get(me, e);
	if (written < 0) {
		rc = -errno;
		mei_err(me, "write failed with status %zd %s\n",
			written, strerror(errno));
		goto out;
	}
	timestamp_print(me, s, e, "mei write: ");

	if (!timeout)
		goto nowait;

	FD_ZERO(&set);
	FD_SET(me->fd, &set);
	rc = select(me->fd + 1 , &set, NULL, NULL, &tv);
	if (rc > 0 && FD_ISSET(me->fd, &set)) {
		mei_msg(me, "write success\n");
	} else if (rc == 0) {
		mei_err(me, "write failed on timeout with status\n");
		goto out;
	} else { /* rc < 0 */
		mei_err(me, "write failed on select with status %zd\n", rc);
		goto out;
	}

nowait:
	rc = written;
out:
	if (rc < 0)
		mei_deinit(me);

	return rc;
}

#ifdef TEST
int main(int argc, char **argv)
{

}
#endif
