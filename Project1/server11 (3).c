/* 
 * Lab 1.1 - server11c.c
 * Authors: Shanti Upadhyay and Jaeho Lee
*/

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

// Constants
#define SERVE_PORT 10010
#define MAXLINE 1024

int16_t getMsgLen(char *buf);
int32_t getSeqNum(char *buf);
long unsigned int getTimeStamp(char *buf);


int main() {
	// Variable declarations:
	struct sockaddr_in cliaddr, servaddr;
	int socketfd, n;
	socklen_t clilen;
	socketfd = socket(AF_INET, SOCK_DGRAM, 0);	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVE_PORT);
	
	// Bind socket to the server's IP Address:
	bind(socketfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	
	while (1) {
		char buf[MAXLINE];
		clilen = sizeof(cliaddr);
		if ((n = recvfrom(socketfd, (char *)buf, MAXLINE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &clilen)) < 0) {
			perror("Error in recvfrom");
			exit(1);
		}

		buf[n] = '\0';
		printf("From the client:\n");
		printf("Message: %s\n", buf + 14);
		printf("Packet Length: %d\n", getMsgLen(buf));
		printf("Sequence Number: %d\n", getSeqNum(buf));

		cliaddr.sin_family = AF_INET;
		
		printf("\nSending to client: %s\n\n", buf + 14);
		if (sendto(socketfd, (char *)buf, n, MSG_CONFIRM, (struct sockaddr *) &cliaddr, clilen) < 0) {
			perror("error in sendto");
			exit(2);
		}
		bzero(buf, MAXLINE);
	}

	return 0;
}

int16_t getMsgLen(char *buf) {
	int16_t len = 0;
	int i;
	for (i = 1; i >= 0; i--) {
		len += buf[i];
		if (i > 0) {
			len = len << 8;
		}
	}
	return len;
}

int32_t getSeqNum(char *buf) {
	int32_t sn = 0;
	int i;
	for (i = 5; i >= 2; i--) {
		sn += buf[i];
		if (i > 2) {
			sn = sn << 8;
		}
	}
	return sn;
}

long unsigned int getTimeStamp(char *buf) {
	long unsigned int timestamp = 0;
	int i;
	for (i = 13; i >= 6; i++) {
		timestamp |= (int64_t) buf[i];
		if (i > 6) {
			timestamp = timestamp << 8;
		}	
	}
	return timestamp;
}