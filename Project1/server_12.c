/*
 * Lab 1.2 - server12c.c
 * Authors: Shanti Upadhyay and Jaeho Lee
*/
#include <stdlib.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#define SERV_PORT 10020
#define MAXLINE 4096 
#define OPSZ 4 

int calculate(int opnum, int operands[], char operator);

int main() {
    int ServerSock, ClientSock;//sockets 
    char message[MAXLINE];
    int  i;
    int result, opendCnt, recv_cnt, recv_len;


    socklen_t szClntAddr;


    struct sockaddr_in servAddr, clntAddr;

    //new socket
    ServerSock = socket(PF_INET, SOCK_STREAM, 0);
    if (ServerSock < 0)
    {
        fputs("socket error", stderr);
        fputc('\n', stderr);
        exit(1);
    }
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    servAddr.sin_port = htons(SERV_PORT);

    //address allocation
    if (bind(ServerSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0)
    {
        fputs("bind error", stderr);
        fputc('\n', stderr);
        exit(1);
    }
    if (listen(ServerSock, 3) < 0)
    {
        fputs("listen error", stderr);
        fputc('\n', stderr);
        exit(1);
    }
    printf("%s\n", "Server running... waiting for connections.");

    szClntAddr = sizeof(struct sockaddr_in);
    for (i = 0; i < 5; i++)
    {
        ClientSock = accept(ServerSock, (struct sockaddr*)&clntAddr, &szClntAddr);
        if (ClientSock == -1)
        {
            fputs("accpet error", stderr);
            fputc('\n', stderr);
            exit(1);
        }
        else
            printf("Connected client %d \n", i + 1);
        int flag;

        for (;;)
        {
            flag = 0;

            opendCnt = 0;
            recv_len = 0;

            read(ClientSock, &opendCnt, 1);
            while ((opendCnt * OPSZ + 1) > recv_len)
            {
                recv_cnt = recv(ClientSock, &message[recv_len], MAXLINE, 0);
                recv_len += recv_cnt;
                if (recv_cnt == -1 || recv_cnt == 0)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 1)
                break;

            //After calculation, then send the result to client
            result = calculate(opendCnt, (int*)message, message[recv_len - 1]);
            write(ClientSock, (char*)&result, sizeof(result));
            printf("result : %d \n", result);
        }
        printf("Client %d is disconnected \n", i + 1);
        close(ClientSock);
    }
    close(ServerSock);
}
int calculate(int opnum, int operands[], char operator) {
    int output = operands[0], i;
    switch (operator)
    {
    case '+':
        for (i = 1; i < opnum; i++) output += operands[i];
        break;
    case '-':
        for (i = 1; i < opnum; i++) output -= operands[i];
        break;
    case '*':
        for (i = 1; i < opnum; i++) output *= operands[i];
        break;
    case '/':
        for (i = 1; i < opnum; i++) output /= operands[i];
        break;
    }
    return output;
}

