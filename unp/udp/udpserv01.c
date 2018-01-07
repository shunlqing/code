#include "unp.h"

void dg_echo(int sockfd, SA* pcliaddr, socklen_t clilen);

int main(int argc, char* argv[])
{
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(sockfd, (SA*) &servaddr, sizeof(servaddr));

    dg_echo(sockfd, (SA*) &cliaddr, sizeof(cliaddr));
}

void dg_echo(int sockfd, SA* pcliaddr, socklen_t clilen) //协议无关的
{
    int n;
    socklen_t len;
    char msg[MAXLINE];

    for(;;) {
        len = clilen;
        n = recvfrom(sockfd, msg, MAXLINE, 0, pcliaddr, &len);

        sendto(sockfd, msg, n, 0, pcliaddr, len);
    }
}