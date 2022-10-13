/* 
 * Lab 1.1 - client11b.c
 * Authors: Shanti Upadhyay and Jaeho Lee
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdint.h>
#include <math.h>

#define SERV_PORT 10010
#define MAXLINE 1024
#define MAXPKTSZ 1038
#define MAX 2

void numToByteArray(long num, int size, char *bArray);
void buildPacket(int16_t msgLength, int32_t seqNum, long unsigned int timestamp, char *msg, char *sendPacket);
void decodePacket(char *receivePacket);

int main(int argc, char **argv) {
	char message[MAXLINE];
	char sendPacket[MAXPKTSZ];
	char receivePacket[MAXPKTSZ];

	// Check for proper usage:
	if (argc != 2) {
		perror("Usage: ./client11b <server hostname>");
		exit(1);
	}
	// Variable declarations:
	int socketfd;
	int n;
	socklen_t servlen;
	struct sockaddr_in servaddr;
	struct timeval start, end;
	struct hostent *he;
	int rtt;

	int16_t msgLength;
	int32_t seqNum = 0;
   
	uint64_t timestamp;
	const unsigned int MAXSEQNUM = (long unsigned int) pow(2, 32) - 1;
	if ((he = gethostbyname(argv[1])) == NULL) {
		perror("Error in resolving server IP Address.");
		exit(2);
	}	
	do {
		if ((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("Problem in creating the socket.");
			exit(3);
		}
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr = *((struct in_addr *)he->h_addr);
		servaddr.sin_port = htons(SERV_PORT);
		printf("\nEnter message to be sent to server: ");
		
		fgets(message, MAXLINE, stdin);
		message[strlen(message) - 1] = '\0';
		
		
		if (strlen(message) > MAXLINE) {
			printf("Message too long. try again.\n");
		}
		else {
			gettimeofday(&start, NULL);
			timestamp = start.tv_sec * 1000 + start.tv_usec / 1000;
			if (seqNum < MAXSEQNUM) {
				seqNum++;
			}
			else {
				seqNum = 1;
			}
			msgLength = 2 + sizeof(seqNum) + sizeof(timestamp) + strlen(message);
			buildPacket(msgLength, seqNum, timestamp, message, sendPacket);

			printf("\nFrom the client:\n");
			printf("Message: %s\n", message);
			printf("Packet Length: %d\n", msgLength);
			printf("Timestamp: %ld\n", timestamp);
			printf("Sequence Number: %d\n", seqNum);

			if (sendto(socketfd, (char *) sendPacket, sizeof(sendPacket), MSG_CONFIRM, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
				perror("Problem in sending the packet.");
				exit(4);
			}
			servlen = sizeof(servaddr); 
			if ((n = recvfrom(socketfd, (char *) receivePacket, MAXPKTSZ, MSG_WAITALL, (struct sockaddr *) &servaddr, &servlen)) < 0) {
				perror("Problem in receiving the packet.");
				exit(5);
			}
			decodePacket(receivePacket);
			gettimeofday(&end, NULL);
			rtt = (end.tv_sec * 1000 + end.tv_usec / 1000) - (start.tv_sec * 1000 + start.tv_usec / 1000); // Calculate RTT	
			printf("\nRound Trip Time (RTT) = %d ms\n", rtt);
			
			bzero(message, MAXLINE);
			bzero(sendPacket, MAXPKTSZ);
			bzero(receivePacket, MAXPKTSZ);

			close(socketfd);
		}
      
	} while (1);

	return 0;
}

void numToByteArray(long num, int size, char *bArray) {
	int i;
	for (i = 0; i < size; i++) {
		bArray[i] = (num >> (i * 8)) & 0xFF;
	}
}


void buildPacket(int16_t msgLength, int32_t seqNum, uint64_t timestamp, char *msg, char *sendPacket) {
	// Create byte array variables:
	char msgLengthArr[2];
	char seqNumArr[4];
	char timeStampArr[8];

	// Convert each part into its own byte array:
	numToByteArray(msgLength, sizeof(msgLength),  msgLengthArr); 
	numToByteArray(seqNum, sizeof(seqNum), seqNumArr); 
	numToByteArray(timestamp, sizeof(timestamp), timeStampArr);
 
	// Combine all byte arrays (and the message) into a packet:
	memcpy(sendPacket, msgLengthArr, sizeof(msgLengthArr));
	memcpy(sendPacket + sizeof(msgLengthArr), seqNumArr, sizeof(seqNumArr));
	memcpy(sendPacket + sizeof(seqNumArr) + sizeof(msgLengthArr), timeStampArr, sizeof(timeStampArr));
	memcpy(sendPacket + sizeof(timeStampArr) + sizeof(seqNumArr) + sizeof(msgLengthArr), msg, strlen(msg));

}

void decodePacket(char *receivePacket) {
	int16_t msgLength = 0;
	int32_t seqNum = 0;
	int i;

	for (i = 1; i >= 0; i--) {
		msgLength += receivePacket[i];
		if (i > 0) {
			msgLength = msgLength << 8;	
		}
	}

	for (i = 5; i >= 2; i--) {
		seqNum += receivePacket[i];
		if (i > 2) {
			seqNum = seqNum << 8;
		}
	}

	printf("\nFrom the server:\n");
	printf("Message: %s\n", receivePacket + 14);
	printf("Packet Length: %d\n", msgLength);
	printf("Sequence Number: %d\n", seqNum);
	
}