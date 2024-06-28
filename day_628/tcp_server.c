#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>



// 端口号
#define SOCKPORT 8002

// socket简单实现--服务器 server
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
    int cfd = accept(lfd,(struct sockaddr*)&cliAddr,&len);
    if(cfd < 0)
    {
        perror("accept error");
        exit(1);
    }
    printf("accept return\n");
    char buf[1024];
    char buf1[1024];
    while(1)
    {
        // 读取客户端数据
        int read_count = read(cfd,buf,sizeof(buf));
        // 写到终端
        write(STDOUT_FILENO,buf,read_count);
      
        // 读取终端数据
        int read_count1 = read(STDIN_FILENO,buf1,sizeof(buf1));
        // 发回客户端
        write(cfd,buf1,read_count1);
    }

    return 0;
}


// // 没有accpet，也能完成三次握手
// int main(int argc, char* argv[])
// {
//     // 创建socket，ip协议，套接字类型（流式套接字），流式套接字默认TCP协议
//     int lfd = socket(AF_INET,SOCK_STREAM,0);
//     if(lfd<0)
//     {
//         perror("socket error");
//         exit(1);
//     }

//     // 绑定ip 端口号
//     struct sockaddr_in serAddr,cliAddr;

//     // 协议族，在socket编程中只能是AF_INET
//     serAddr.sin_family = AF_INET;
//     // 端口号
//     serAddr.sin_port = htons(SOCKPORT);
//     // ip地址，本机任意一个ip
//     serAddr.sin_addr.s_addr = INADDR_ANY;
//     int bret = bind(lfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
//     if(bret < 0)
//     {
//         perror("bind error");
//         exit(1);
//     }

//     // 监听
//     listen(lfd,64);

//     while(1);

//     return 0;
// }

// //// accept 读取全连接队列（为空时吗，会阻塞）
// int main(int argc, char* argv[])
// {
//     // 创建socket，ip协议，套接字类型（流式套接字），流式套接字默认TCP协议
//     int lfd = socket(AF_INET,SOCK_STREAM,0);
//     if(lfd<0)
//     {
//         perror("socket error");
//         exit(1);
//     }

//     // 绑定ip 端口号
//     struct sockaddr_in serAddr,cliAddr;

//     // 协议族，在socket编程中只能是AF_INET
//     serAddr.sin_family = AF_INET;
//     // 端口号
//     serAddr.sin_port = htons(SOCKPORT);
//     // ip地址，本机任意一个ip
//     serAddr.sin_addr.s_addr = INADDR_ANY;
//     int bret = bind(lfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
//     if(bret < 0)
//     {
//         perror("bind error");
//         exit(1);
//     }

//     // 监听
//     listen(lfd,64);
//     printf("listen is return\n");
//     // 接收
//     socklen_t len = sizeof(cliAddr);
//     int cfd = accept(lfd,(struct sockaddr*)&cliAddr,&len);
//     if(cfd < 0)
//     {
//         perror("accept error");
//         exit(1);
//     }
//     printf("accept is return\n");
//     char buf[1024];
//     while(1)
//     {
//         // 读取
//         int read_count = read(cfd,buf,sizeof(buf));
//         // 写到终端
//         write(STDOUT_FILENO,buf,read_count);
//         // 再次发回
//         write(cfd,buf,read_count);
//     }

//     return 0;
// }