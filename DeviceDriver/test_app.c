#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int fd;

int main(int argc, char **argv)
{
	int fd = open("/dev/test_dd", O_RDWR);
	if(fd < 3) {
		fprintf(stderr, "/dev/test_dd device file open error!! .. %s\n",strerror(errno));
		return 0;
	}

	read(fd, 0, 0);
	write(fd, 0,0);
	close(fd);
	return 0;
}

