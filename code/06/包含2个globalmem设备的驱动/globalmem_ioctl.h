#ifndef GLOBALMEM_IOCTL_H
#define GLOBALMEM_IOCTL_H

#include <linux/ioctl.h>

#define GLOBALMEM_SIZE 0x1000
#define GLOBALMEM_DEVICE_COUNT 2

#define GLOBALMEM_IOC_MAGIC 'g'

struct globalmem_ioc_byte {
	unsigned int index;
	unsigned char value;
};

/* No payload: clear the whole device buffer. */
#define GLOBALMEM_IOCTL_CLEAR \
	_IO(GLOBALMEM_IOC_MAGIC, 0)
/* Read-only payload: return device size to userspace. */
#define GLOBALMEM_IOCTL_GET_SIZE \
	_IOR(GLOBALMEM_IOC_MAGIC, 1, int)
/* Write-only payload: fill the whole device buffer with one byte. */
#define GLOBALMEM_IOCTL_SET_FILL \
	_IOW(GLOBALMEM_IOC_MAGIC, 2, unsigned char)
/* Read/write payload: write a byte and return the old value. */
#define GLOBALMEM_IOCTL_EXCHANGE_BYTE \
	_IOWR(GLOBALMEM_IOC_MAGIC, 3, struct globalmem_ioc_byte)
/* Raw _IOC example: same semantics as EXCHANGE_BYTE. */
#define GLOBALMEM_IOCTL_RAW_EXCHANGE_BYTE \
	_IOC(_IOC_READ | _IOC_WRITE, GLOBALMEM_IOC_MAGIC, 4, \
	     sizeof(struct globalmem_ioc_byte))

#define GLOBALMEM_IOC_MAXNR 4

#endif
