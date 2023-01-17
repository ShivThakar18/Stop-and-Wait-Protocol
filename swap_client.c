/*
*	swap_client.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define	MAXLINE 128	// maximum characters to receive and send at once
#define	MAXFRAME 256

extern int swap_connect(unsigned int addr, unsigned short port);
extern int swap_disconnect(int sd);
extern int sdp_send(int sd, char *buf, int length);
extern int sdp_receive(int sd, char *buf);
extern int sdp_receive_with_timer(int sd, char *buf, unsigned int expiration);
extern unsigned short checksum(unsigned char buf[], int length);

int session_id = 0;
int S = 0;	// frame number sent
int seqNo = 0; 
int counter = 0;

int swap_open(unsigned int addr, unsigned short port)
{
	int	sockfd;		// sockect descriptor
	struct	sockaddr_in	servaddr;	// server address
	char	buf[MAXLINE];
	int	len, n;

	/*
	*	if the session is already open, then return error
	*/

	if (session_id != 0)
		return -1;

	/*
	*	connect to a server
	*/

	session_id = swap_connect(addr, port);	// in sdp.o

	/*
	*	return the seesion id
	*/

	return session_id;
}

int swap_write(int sd, char *buf, int length)
{								// buf = frame
	int send_bytes; 
	int receive_bytes;
	char frame[MAXFRAME] = ""; // ack frame
	char ack[2] = "0";  // change to S
	unsigned short sum;
	char tempFrame[MAXFRAME] = "";
	char str_sum[6] = "0"; 	// initialize 
	char seq[2]; 
	int flag = 0;

	if (session_id == 0 || sd != session_id)
		return -1;

	seqNo = S; 

	//bzero(&frame,sizeof(frame));
	memset(frame,'\0',sizeof(frame));

	//bzero(&tempFrame,sizeof(tempFrame));
	memset(tempFrame,'\0',sizeof(tempFrame));

	sprintf(seq,"%d",seqNo);
	strcpy(tempFrame,seq);		

	// call checksum of buffer
	sum = checksum(buf,length); // unsigned short
	sprintf(str_sum,"%d",sum);	// save short as string

	strcpy(frame,buf); // copy buffer into frame
	//strcat(frame,str_sum); // concatenate the frame and the checksum
	
	strcat(tempFrame,str_sum);
	strcat(tempFrame,frame);

	strcpy(frame, tempFrame);
	
	printf("Buffer = %s   Frame = %s\n",buf,frame);


	frame[strlen(frame)] = '\0';

	/*
	*	send a DATA frame
	*/

	do{

		if(receive_bytes == -1 || receive_bytes == -3 || flag == 1){
			printf("Error, Sending Data Again\n");
		}

		flag = 0;
		
		do{
			if(send_bytes < 0){
				printf("Error sending data frame...\n");
			}

			printf("Sending Data Frame: %s\n",frame);

			send_bytes = sdp_send(sd, frame, strlen(frame));

		}while(send_bytes < 0);

		/*
		*	read a frame with a timer
		*/

		receive_bytes = sdp_receive_with_timer(sd,ack,5);

		/*
		*	several different cases including disconnection
		*/

		
		if(receive_bytes == -1){
			printf("General Errors Encountered\n");
			//swap_close(sd);		// close session 

		}else if(receive_bytes == -2){
			printf("Session was Disconnected\n");
			return send_bytes;		// close session

		}else if(receive_bytes == -3){
			printf("Time Expired\n");
			//swap_close(sd);		// close session
		}else{

			printf("Ack Frame: %s\n",ack);

			char s[2];
			int tempS = 0;
			tempS = (S + 1)%2;

			sprintf(s, "%d",tempS);

			if(strcmp(ack,s) == 0){
				S = (S + 1)%2;
				printf("Slide Window Forward\n");

			}else{
				printf("Packet was discarded\n");
				flag = 1;
			}

		}

	}while(receive_bytes == -1 || receive_bytes == -3 || flag == 1);

	/*
	*	return the length sent
	*/

	counter = counter + 1; 

	printf("Frame Number: %d\n\n",counter);

	return send_bytes; // return number of bytes sent
	// ...
}

void swap_close(int sd)
{
	if (session_id == 0 || sd != session_id)
		return;

	else
		session_id = 0;

	swap_disconnect(sd);	// in sdp.o
}
