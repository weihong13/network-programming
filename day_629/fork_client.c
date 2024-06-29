#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>


// IP 端口号
#define SERIP "192.168.3.160" 
#define SERPORT 8001

// // socket简单实现 --- 客户端 client
int main(int argc, char* argv[])
{
    // 创建socket，ip协议，套接字类型（流式套接字），流式套接字默认TCP协议
    int cfd = socket(AF_INET,SOCK_STREAM,0);
    if(cfd<0)
    {
        perror("socket error");
        exit(1);
    }

    // 客户端是不需要绑定的，他只需要知道给谁打电话就行

    // 绑定ip 端口号
    struct sockaddr_in serAddr;

    // 协议族，在socket编程中只能是AF_INET
    serAddr.sin_family = AF_INET;
    // 存要连接服务器的端口号
    serAddr.sin_port = htons(SERPORT);

    // 存要连接的服务器的ip地址
    // int inet_pton(int af, const char *src, void *dst);
    // 将 "192.168.10.128" 点分十进制的字符串 转为正确格式
    
    inet_pton(AF_INET,SERIP,&serAddr.sin_addr.s_addr);
   
    // 客户端也不需要 accept，但是需要另一个函数 connect
    // 用来申请与服务器进行连接--进行三次握手
    connect(cfd,(struct sockaddr*)&serAddr,sizeof(serAddr));
    
    printf("connect successful\n");
    
    // 发送消息
    char buf[1024];
    while(1)
    {
        // 读取终端的数据到buf  
        int read_count = read(STDIN_FILENO,buf,sizeof(buf));
        // 发给到服务器
        write(cfd,buf,read_count);

        // 读取服务器发回的数据
        int read_count1 = read(cfd,buf,sizeof(buf));
        write(STDOUT_FILENO,buf,read_count1);
    }

    return 0;
}
