#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

#include <time.h>
#include <signal.h>

#include "sim_dev.h"
#include "disk.h"

#define oops(msg, errnum) { perror(msg); exit(errnum); }

#define BUFSIZE 4096

timer_t gTimerid;
int count = 5;

int main(void)
{
	(void) signal(SIGALRM, timer_callback);
    start_timer();
    while(count > 0);

	return 0;

	/*int i, fd, len, wlen, tmp, tmp2;
	
	// test message to send to the device
	char msg[] = "Skeleton Kernel Module Test";
	char receive_buffer[128];
	
	// variables for holding device control data
	int ioctl_control_data, ioctl_status_data;

   // open the I/O channel to the device
	fd = open("/dev/sim_dev", O_RDWR | O_SYNC);
	if( fd == -1)
	   oops("Unable to open device...", 1);

	// test device write function
	len = write(fd, msg, strlen(msg) + 1);
	if( len == -1 )
		oops("Unable to write to the simuated device.\n", 2);

	printf("'%s' written to /dev/sim_dev\n", msg);
	
	// test device read function
	len = read(fd, receive_buffer, 128);
	if( len == -1 )
		oops("Unable to read from the simulated device.\n", 3);

	printf("'%s' read from /dev/sim_dev\n", receive_buffer);

	// test ioctl with the device
	// this is a sample content to send to the device
	ioctl_control_data = 0xABCDEFEF;
	
	// send command to the device
	ioctl(fd, IOCTL_SIM_DEV_WRITE, &ioctl_control_data);
	
	// get a status from the device
	// we poll here, but that also can be in response to an interrupt
	ioctl(fd, IOCTL_SIM_DEV_READ, &ioctl_status_data);
	
	printf("IOCTL test: written: '%x' - received: '%x'\n", ioctl_control_data, ioctl_status_data);

	close(fd);*/
}

void start_timer(void)
{
    struct itimerspec value;

    value.it_value.tv_sec = 1;
    value.it_value.tv_nsec = 0;

    value.it_interval.tv_sec = 5;
    value.it_interval.tv_nsec = 0;

    timer_create (CLOCK_REALTIME, NULL, &gTimerid);

    timer_settime (gTimerid, 0, &value, NULL);

}

void stop_timer(void)
{
    struct itimerspec value;

    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = 0;

    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = 0;

    timer_settime (gTimerid, 0, &value, NULL);
}

void timer_callback(int sig)
{
	int fd, len;

	fd = open("/dev/sim_dev", O_RDWR | O_SYNC);
	if( fd == -1)
	   oops("Unable to open device...", 1);

	int test = 0;
	char *input;

	printf("Callback\n");

	input = malloc(sizeof(MAX_LOGICAL_SECTOR));

	sect *buffer;
	void *buff;

	//myDisk = malloc(sizeof(disk_t));

	/*myDisk[0][0][0][0] = 'a';
	myDisk[0][0][0][1] = 'b';*/

	scanf("%[^\n]%*c", input);	

	ioctl(fd, IOCTL_SIM_DEV_WRITE, &test);

	len = write(fd, input, strlen(input) + 1);
	if( len == -1 )
		oops("Unable to write to the simuated device.\n", 2);
	
	//write(test, 1, input);

	/*srand(time(NULL));

	test = rand() % MAX_LOGICAL_SECTOR;
	printf("%d\n", test);*/

	//read(test, 1, &buff);
	//buffer = buff;

	//printf("%s\n", myDisk[0][0][0][0]);
	//printf("%s\n", *buffer);

	count--;

	close(fd);
}

