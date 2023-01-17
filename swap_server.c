/*
*	swap_server.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define	MAXLINE	128	// maximum characters to receive and send at once
#define MAXFRAME 256

extern int swap_accept(unsigned short port);
extern int swap_disconnect(int sd);
extern int sdp_send(int sd, char *buf, int length);
extern int sdp_receive(int sd, char *buf);
extern int sdp_receive_with_timer(int sd, char *buf, unsigned int expiration);
extern unsigned short checksum(unsigned char buf[], int length);

int session_id = 0;
int R = 0;	// frame number to receive

int counter = 0;
int swap_wait(unsigned short port)
{
	/*
	*	if the session is already open, then return error
	*/

	if (session_id != 0)
		return -1;

	/*
	*	accept a connection
	*/

	session_id = swap_accept(port);	// in sdp.o

	/*
	*	return a ssession id
	*/

	return session_id;
}

int swap_read(int sd, char *buf)
{
	int send_bytes;
	int receive_bytes;
	char frame[MAXFRAME] ="";				// full frame
	char split_frame[MAXFRAME] = "";			// split frame and checksum 
	unsigned short server_checksum; 	// calculate checksum from frame received
	char str_serv_cs[6];
	char client_checksum[6]= ""; 	// split from data frame received from client
								// checksum received from client
	char ack[2] = "0";
	char client_seq[2] = "";

	if (session_id == 0 || sd != session_id)
		return -1;

	/*
	*	receive a frame without a timer
	*/

	//bzero(&frame,sizeof(frame));
	memset(frame,'\0',sizeof(frame));

	receive_bytes = sdp_receive(sd,frame);
	/*
	*	split frame into actual data and checksum 
	*/

	int checksum_len = 0;

	if(strstr(frame, "999") == NULL){
		checksum_len = 5;
	}else{
		checksum_len = 3;
	}


	int frame_len = strlen(frame) - checksum_len;

	if(frame_len < 0){ // if length of received frame is less than data frame

			/* 
			 * got an error where a 21st frame was received and help garbage 
			 * values
			 */

		return receive_bytes;
	}

	printf("\n\nReceived Frame: %s\n",frame);


	char *p = frame; 		// pointer for frame

	// split the string, actual data frame and checksum
	// 012345678911111 -> split_frame = 0123456789 , client_checksum = 11111
	memcpy(client_seq, &p[0],1);
	memcpy(client_checksum, &p[1], checksum_len);
	memcpy(split_frame,&p[checksum_len+1],frame_len);

	//printf("Data Frame Received: %s\n", split_frame);
	//printf("Client seqNo: %s  Client Checksum: %s\n",client_seq, client_checksum);

	server_checksum = checksum(split_frame,strlen(split_frame));	// calculate received frame checksum
	sprintf(str_serv_cs,"%d",server_checksum);	// save short as string

	/*
	*	several cases
	*/

	if(receive_bytes == -1){
		printf("General Errors have occurred\n");
	}else if(receive_bytes == -2){
		printf("Session was Disconnected\n");
		return receive_bytes;
	}else{ // if no errors were found, if receive_bytes < 0
		printf("Received: %s ",frame);
		printf("Data Frame: %s ",split_frame);
		printf("Client Checksum: %s ",client_checksum);
		printf("Server Checksum: %s\n",str_serv_cs);

		/*
		*	verify received checksum with calculated checksum 
		*/

		int compare = strcmp(client_checksum, str_serv_cs); //returns 0, equal

		char r[2];

		sprintf(r, "%d",R);

		if(strcmp(r,client_seq) == 0){ // new packet
			if(compare == 0){ // equal 
				R = (R + 1)%2; 	// mod 2 slude window forward
				printf("Checksum Matched\n");
				counter = counter + 1; // number of frames received
				printf("Frame Number: %d\n", counter);
			}else if(compare != 0){ // not equal
				printf("DISCARDED - Checksum Didn't Match; ");
				strcpy(split_frame, "");
			}
		}else{
			printf("DISCARDED - Duplicate Packet; ");
			strcpy(split_frame, "");
		}

		printf("Sending acknowledgement frame...\n");
		sprintf(ack,"%d",R);

		do{
			send_bytes = sdp_send(sd, ack,2);
		}while(send_bytes < 0);

		printf("Acknowledgement Frame Sent\n");

	}

	/*
	*	copy the data field in the frame into buf, and return the length
	*/

	strcpy(buf,split_frame);			// copy string  -> src = frame, dest = buf
	return receive_bytes;
}

void swap_close(int sd)
{
	if (session_id == 0 || sd != session_id)
		return;

	else
		session_id = 0;

	swap_disconnect(sd);	// in sdp.o
}
