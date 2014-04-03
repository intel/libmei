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
#include <linux/mei_i.h>

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

const char* mei_get_devname()
{
	const char* devname = getenv("MEI_DEV");
	return (NULL == devname) ? "/dev/mei" : devname;
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
	me->last_err = 0;
}

static inline int __mei_errno_to_state(struct mei *me)
{
	switch(me->last_err) {
	case 0:         return me->state;
	case ENOTTY:    return MEI_CL_STATE_NOT_PRESENT;
	case EBUSY:     return MEI_CL_STATE_DISCONNECTED;
	case ENODEV:    return MEI_CL_STATE_DISCONNECTED;
	default:        return MEI_CL_STATE_ERROR;
	}
}

static inline int __mei_open(struct mei *me, const char *devname)
{
	errno = 0;
	me->fd = open(devname, O_RDWR);
	me->last_err = errno;
	return me->fd == -1 ? -me->last_err : me->fd;
}

static inline int __mei_connect(struct mei *me, struct mei_connect_client_data *d)
{
	errno = 0;
	int rc = ioctl(me->fd, IOCTL_MEI_CONNECT_CLIENT, d);
	me->last_err = errno;
	return rc == -1 ? -me->last_err : 0;
}

static inline int __mei_dma_buff(struct mei *me, struct mei_client_dma_data *d)
{
	errno = 0;
	int rc = ioctl(me->fd, IOCTL_MEI_SETUP_DMA_BUF, d);
	me->last_err = errno;
	return rc == -1 ? -me->last_err : 0;
}

static inline ssize_t __mei_read(struct mei *me, unsigned char *buf, size_t len)
{
	ssize_t rc;
	errno = 0;
	rc = read(me->fd, buf, len);
	me->last_err = errno;
	return rc <= 0 ? -me->last_err : rc;
}

static inline ssize_t __mei_write(struct mei *me, const unsigned char *buf, size_t len)
{
	ssize_t rc;
	errno = 0;
	rc = write(me->fd, buf, len);
	me->last_err = errno;
	return rc <= 0 ? -me->last_err : rc;
}

static inline int __mei_select(struct mei *me,
		fd_set *readfds, fd_set *writefds,
		fd_set *exceptfds, struct timeval *timeout)

{
	int rc;
	errno = 0;
	rc = select(me->fd + 1, readfds, writefds, exceptfds, timeout);
	me->last_err = errno;
	return rc;
}

int mei_init(struct mei *me, const uuid_le *guid,
		unsigned char req_protocol_version, bool verbose)
{
	const char* devname = mei_get_devname();
	int rc;

	if (!me || !guid)
		return -EINVAL;

	/* if me is unitialized it will close wrong file descriptor */
	me->fd = -1;
	mei_deinit(me);

	me->verbose = verbose;
	me->profile = false;

	rc = __mei_open(me, devname);
	if (rc < 0) {
		mei_err(me, "Cannot establish a handle to the Intel MEI driver %s [%d]:%s\n",
			devname, rc, strerror(-rc));
		return rc;
	}

	mei_msg(me, "Opened %s: fd = %d\n", devname, me->fd);

	memcpy(&me->guid, guid, sizeof(*guid));
	me->prot_ver = req_protocol_version;

	me->state = MEI_CL_STATE_INTIALIZED;

	return 0;
}

struct mei *mei_alloc(const uuid_le *guid,
		unsigned char req_protocol_version, bool verbose)
{
	struct mei *me;

	if (!guid)
		return NULL;

	me = malloc(sizeof(struct mei));
	if (!me)
		return NULL;

	if (mei_init(me, guid, req_protocol_version, verbose)) {
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

int mei_connect(struct mei *me)
{
	struct mei_client *cl;
	struct mei_connect_client_data data;
	int rc;

	if (!me)
		return -EINVAL;

	if (me->state != MEI_CL_STATE_INTIALIZED &&
	    me->state != MEI_CL_STATE_DISCONNECTED) {
		mei_err(me, "client state [%d]\n", me->state);
		return -EINVAL;
	}

	memset(&data, 0, sizeof(data));
	memcpy(&data.in_client_uuid, &me->guid, sizeof(me->guid));

	rc = __mei_connect(me, &data);
	if (rc < 0) {
		me->state = __mei_errno_to_state(me);
		mei_err(me, "Cannot connect to client [%d]:%s\n", rc, strerror(-rc));
		return rc;
	}

	cl = &data.out_client_properties;
	mei_msg(me, "max_message_length %d\n", cl->max_msg_length);
	mei_msg(me, "protocol_version %d\n", cl->protocol_version);

	/* FIXME: need to be exported otherwise */
	if ((me->prot_ver > 0) &&
	     (cl->protocol_version != me->prot_ver)) {
		mei_err(me, "Intel MEI protocol version not supported\n");
		return -EINVAL;
	}

	me->buf_size = cl->max_msg_length;
	me->prot_ver = cl->protocol_version;

	me->state =  MEI_CL_STATE_CONNECTED;

	return 0;
}

ssize_t mei_recv_msg(struct mei *me, unsigned char *buffer,
			size_t len, unsigned long timeout)
{
	ssize_t rc;
	timestamp_declare(s,e);

	if (!me || !buffer)
		return -EINVAL;

	mei_msg(me, "call read length = %zd\n", len);

	timestamp_get(me, s);
	rc = __mei_read(me, buffer, len);
	timestamp_get(me, e);
	if (rc < 0) {
		me->state = __mei_errno_to_state(me);
		mei_err(me, "read failed with status [%zd]:%s\n", rc, strerror(-rc));
		goto out;
	}
	mei_msg(me, "read succeeded with result %zd\n", rc);
	if (me->verbose)
		mei_dump_hex_buffer(buffer, rc);
out:
	timestamp_print(me, s, e, "mei read: ");
	return rc;
}

ssize_t mei_send_msg(struct mei *me, const unsigned char *buffer,
			size_t len, unsigned long timeout)
{
	struct timeval tv;
	ssize_t rc, wr;
	fd_set set;
	timestamp_declare(s,e);

	if (!me || !buffer)
		return -EINVAL;

	mei_msg(me, "call write length = %zd\n", len);
	if (me->verbose)
		mei_dump_hex_buffer(buffer, len);

	timestamp_get(me, s);
	rc  = __mei_write(me, buffer, len);
	timestamp_get(me, e);
	if (rc < 0) {
		me->state = __mei_errno_to_state(me);
		mei_err(me, "write failed with status [%zd]:%s\n",
			rc, strerror(-rc));
		return rc;
	}
	timestamp_print(me, s, e, "mei write: ");

	if (!timeout)
		return rc;
	wr = rc;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000000;

	FD_ZERO(&set);
	FD_SET(me->fd, &set);
	rc = __mei_select(me, &set, NULL, NULL, &tv);
	if (rc > 0 && FD_ISSET(me->fd, &set)) {
		mei_msg(me, "write success\n");
		rc = wr;
	} else if (rc == 0) {
		mei_err(me, "write failed on timeout\n");
		return -ETIME;
	} else { /* rc < 0 */
		mei_err(me, "write failed on select with status [%d]:%s\n",
			-me->last_err, strerror(me->last_err));
	}

	return rc;
}

int mei_set_dma_buf(struct mei *me, const char *buf, size_t length)
{
	struct mei_client_dma_data data;
	int rc;

	if (!me)
		return -EINVAL;

	if (!buf || length == 0)
		return -EINVAL;

	if (me->state != MEI_CL_STATE_CONNECTED)
		return -EINVAL;

	data.userptr = (unsigned long)buf;
	data.length = length;
	data.handle = 0;

	rc = __mei_dma_buff(me, &data);
	if (rc < 0) {
		me->state = __mei_errno_to_state(me);
		mei_err(me, "Cannot set dma buffer to client [%d]:%s\n", rc, strerror(-rc));
		return rc;
	}

	mei_msg(me, "dma handle %d\n", data.handle);

	return data.handle;
}


#ifdef TEST
int main(int argc, char **argv)
{

}
#endif
