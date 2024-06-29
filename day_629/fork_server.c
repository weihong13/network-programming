#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


// 端口号
#define SOCKPORT 8001

// socket简单实现--多进程服务器 server
int main(int argc, char* argv[])
{
    // 创建socket，ip协议，套接字类型（流式套接字），流式套接字默认TCP协议
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if(lfd<0)
    {
        perror("socket error");
        exit(1);
    }

    // 绑定ip 端口号
    struct sockaddr_in serAddr,cliAddr;

    // 协议族，在socket编程中只能是AF_INET
    serAddr.sin_family = AF_INET;
    // 端口号
    serAddr.sin_port = htons(SOCKPORT);
    // ip地址，本机任意一个ip
    serAddr.sin_addr.s_addr = INADDR_ANY;
    int bret = bind(lfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    if(bret < 0)
    {
        perror("bind error");
        exit(1);
    }

    // 监听
    listen(lfd,64);
    printf("listen return\n");
    // 接收
    socklen_t len = sizeof(cliAddr);
    while(1)
    {
        // 父进程进行连接
        int cfd = accept(lfd,(struct sockaddr*)&cliAddr,&len);
        if(cfd < 0)
        {
            perror("accept error");
            exit(1);
        }
        printf("accept return\n");
        // 读取客户端的端口号
        int port = ntohs(cliAddr.sin_port);
        // 读取客户端的IP地址
        char dst[64];
        // 将网络中的二进制IP地址转为点分十进制字符串
        inet_ntop(AF_INET,&cliAddr.sin_addr.s_addr,dst,sizeof(dst));
        printf("client ip: %s\n",dst);
        printf("client port: %d\n",port);
        
        int pid = fork();
        if(pid == 0) // 子进程发送消息
        {
            close(lfd); // 子进程，将无用的lfd关闭
            while(1)
            {   char buf[1024];
                // 读取客户端数据
                int read_count = read(cfd,buf,sizeof(buf));
                // 写到终端
                write(STDOUT_FILENO,buf,read_count);
            
                // 发回客户端
                write(cfd,buf,read_count);
            }
        }
        close(cfd); // 父进程用不到cfd，关闭

    }
    return 0;
}


