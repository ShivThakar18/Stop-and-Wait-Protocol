#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#define	MAX_FTA	128

// External functions
extern int swap_open(unsigned int addr, unsigned short port);
extern int swap_write(int sd, unsigned char buf[], int length);
extern void swap_close(int sd);

// These arguments must be passed to the program
// argv[1]: IP address of server, e.g., "127.0.0.1"
// argv[2]: port number of sever, e.g., "6789"

int main (int argc, char *argv[])
{
	int	sd;
	char	buf[MAX_FTA];
	int	n;
	int	i;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s IP_address port_number\n", argv[0]);
		exit(1);
	}

	// connect to the swap server
	sd = swap_open(inet_addr(argv[1]), htons(atoi(argv[2])));
	if (sd < 0)
	{
		fprintf(stderr, "%s cannot swap_open\n", argv[0]);
		exit(1);
	}
	else
		fprintf(stdout, "connected to the swap server\n");
	
	// send messages to the swap server
	buf[0] = '0';
	buf[1] = '1';
	buf[2] = '2';
	buf[3] = '3';
	buf[4] = '4';
	buf[5] = '5';
	buf[6] = '6';
	buf[7] = '7';
	buf[8] = '8';
	buf[9] = '9';

	n = 20;
	for (i = 0; i < n; i++)
		swap_write(sd, buf, 10);

	// close the connection to the swap server
	swap_close(sd);
}
