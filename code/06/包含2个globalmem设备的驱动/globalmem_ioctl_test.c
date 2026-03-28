#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include "globalmem_ioctl.h"

static int expect_all_bytes(int fd, unsigned char expected)
{
	unsigned char buf[256];
	size_t total = 0;

	if (lseek(fd, 0, SEEK_SET) < 0) {
		perror("lseek");
		return -1;
	}

	while (total < GLOBALMEM_SIZE) {
		ssize_t nr;
		size_t i;

		nr = read(fd, buf, sizeof(buf));
		if (nr < 0) {
			perror("read");
			return -1;
		}
		if (nr == 0) {
			fprintf(stderr, "unexpected EOF at %zu bytes\n", total);
			return -1;
		}

		for (i = 0; i < (size_t)nr; i++) {
			if (buf[i] != expected) {
				fprintf(stderr,
					"byte mismatch at offset %zu: expected 0x%02x, got 0x%02x\n",
					total + i, expected, buf[i]);
				return -1;
			}
		}
		total += (size_t)nr;
	}

	return 0;
}

static int test_get_size(int fd)
{
	int size = 0;

	if (ioctl(fd, GLOBALMEM_IOCTL_GET_SIZE, &size) < 0) {
		perror("ioctl GET_SIZE");
		return -1;
	}
	if (size != GLOBALMEM_SIZE) {
		fprintf(stderr, "GET_SIZE mismatch: expected %d, got %d\n",
			GLOBALMEM_SIZE, size);
		return -1;
	}

	printf("GET_SIZE ok: %d bytes\n", size);
	return 0;
}

static int test_set_fill(int fd, unsigned char value)
{
	if (ioctl(fd, GLOBALMEM_IOCTL_SET_FILL, &value) < 0) {
		perror("ioctl SET_FILL");
		return -1;
	}
	if (expect_all_bytes(fd, value) < 0)
		return -1;

	printf("SET_FILL ok: 0x%02x\n", value);
	return 0;
}

static int test_clear(int fd)
{
	if (ioctl(fd, GLOBALMEM_IOCTL_CLEAR) < 0) {
		perror("ioctl CLEAR");
		return -1;
	}
	if (expect_all_bytes(fd, 0) < 0)
		return -1;

	printf("CLEAR ok\n");
	return 0;
}

static int test_exchange(int fd, unsigned long cmd, const char *name,
			 unsigned int index, unsigned char new_value,
			 unsigned char expected_old)
{
	struct globalmem_ioc_byte data;
	unsigned char verify;

	data.index = index;
	data.value = new_value;

	if (ioctl(fd, cmd, &data) < 0) {
		fprintf(stderr, "%s failed: %s\n", name, strerror(errno));
		return -1;
	}
	if (data.value != expected_old) {
		fprintf(stderr, "%s old-value mismatch: expected 0x%02x, got 0x%02x\n",
			name, expected_old, data.value);
		return -1;
	}

	if (lseek(fd, (off_t)index, SEEK_SET) < 0) {
		perror("lseek");
		return -1;
	}
	if (read(fd, &verify, 1) != 1) {
		perror("read");
		return -1;
	}
	if (verify != new_value) {
		fprintf(stderr, "%s new-value mismatch at index %u: expected 0x%02x, got 0x%02x\n",
			name, index, new_value, verify);
		return -1;
	}

	printf("%s ok: index=%u old=0x%02x new=0x%02x\n",
	       name, index, expected_old, new_value);
	return 0;
}

int main(int argc, char *argv[])
{
	const char *device = "/dev/gatieme0";
	int fd;

	if (argc > 2) {
		fprintf(stderr, "usage: %s [device]\n", argv[0]);
		return 1;
	}
	if (argc == 2)
		device = argv[1];

	fd = open(device, O_RDWR);
	if (fd < 0) {
		perror(device);
		return 1;
	}

	printf("testing ioctl on %s\n", device);

	if (test_get_size(fd) < 0)
		goto fail;
	if (test_set_fill(fd, 0x5a) < 0)
		goto fail;
	if (test_clear(fd) < 0)
		goto fail;
	if (test_set_fill(fd, 0x11) < 0)
		goto fail;
	if (test_exchange(fd, GLOBALMEM_IOCTL_EXCHANGE_BYTE,
			  "EXCHANGE_BYTE", 8, 0xa5, 0x11) < 0)
		goto fail;
	if (test_exchange(fd, GLOBALMEM_IOCTL_RAW_EXCHANGE_BYTE,
			  "RAW_EXCHANGE_BYTE", 9, 0x3c, 0x11) < 0)
		goto fail;

	close(fd);
	printf("all ioctl tests passed\n");
	return 0;

fail:
	close(fd);
	return 1;
}
