/*
 * Lab 1.2 - client12c.c
 * Authors: Shanti Upadhyay and Jaeho Lee
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define MAXLINE 4096
#define DATASIZE 4
#define OPSZ 4
#define SERV_PORT 10020

int main(int argc, char* argv[])
{
    int socketfd;
        struct sockaddr_in serveraddr;
    char opmsg[MAXLINE];
        int result, opndCnt, i;
        unsigned char flag;
        int recv_len, recv_cnt;

        if(argc!=3)    {
                printf("you need to enter: %s <IPaddress> <port>\n", argv[0]);
                exit(1);
        }

        socketfd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

   if(socketfd == -1)
                        {
                fputs("socket error", stderr);
                fputc('\n', stderr);
                exit(1);
                }

        memset(&serveraddr, 0, sizeof(serveraddr));
        serveraddr.sin_family=AF_INET; //save the IPaddress
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
        serveraddr.sin_port=htons(SERV_PORT);

        if(connect(socketfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1)
                {
                fputs("connect error", stderr);
                fputc('\n', stderr);
           exit(1);
                }
        else
                puts("Connected!");

        for( ;; )
                {
                flag =' ';
                fputs("enter 2 to start (any other number to quit): ",stdout);
                scanf(" %c",&flag);

        if(flag != '2')
       break;
        else
                opndCnt = (int)flag-48;

                opmsg[0] = (char)opndCnt;

        for(i = 0; i < opndCnt; i++) {
                printf("Enter the Operand %d: ", i+1);
                scanf("%d", (int*)&opmsg[i*OPSZ+1]);
        }

        fgetc(stdin);
        fputs("Enter the operator: ", stdout);
        scanf("%c", &opmsg[opndCnt*OPSZ+1]);
        write(socketfd, opmsg, opndCnt*OPSZ+2);
        read(socketfd, &result, DATASIZE);
        printf("Operation result is  %d\n", result);

        close(socketfd);

        return 0;
        }
        }

        



