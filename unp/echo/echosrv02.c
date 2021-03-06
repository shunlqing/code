/**
 * 每客户单进程 echo服务器
*/

/**
 * 解决问题：
 *    当同时多个客户连接关闭，因为信号不排队，使用wait不能处理所有僵尸进程，改用非阻塞waitpid（WNOHANG选项）；
*/

#include <errno.h>
#include <string.h>
#include <signal.h>

#include "sock.h"
#include "wrapsock.h"
#include "error.h"
#include "helperfunc.h"

#define LISTENQ 5
#define SERV_PORT 9999

void str_echo(int sockfd);
void sig_chld(int signo);

int 
main(int argc, char * argv[])
{
    int listenfd, connfd;
    pid_t childpid;

    socklen_t chilen;
    struct sockaddr_in servaddr, cliaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    if((signal(SIGCHLD, sig_chld)) == SIG_ERR) {
        err_sys("signal error");
    }

    for(;;) {
        chilen = sizeof(cliaddr);
        if((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &chilen)) < 0) {
            if(errno == EINTR) { //处理accept被信号中断的情况
                continue;
            } else {
                err_sys("accept error");
            }
        }
        if((childpid = fork()) == 0) //child process
        {
            Close(listenfd);
            str_echo(connfd);
            exit(0);
        }
        Close(connfd);
    }
}

void
str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while((n = read(sockfd, buf, MAXLINE)) > 0) {
        Writen(sockfd, buf, n);
    }

    if(n < 0 && errno == EINTR) {
        goto again;
    } else if(n < 0) {
        err_sys("str_echo: read error");
    }
}

void sig_chld(int signo)
{
    pid_t pid;
    int stat;

    // pid = wait(&stat);
    // printf("child %d terminated\n", pid);

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0) { //使用非阻塞waitpid可以处理同时有多个子进程退出的情况
        printf("child %d terminated\n", pid);
    }
    return;
}