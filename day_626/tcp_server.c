#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/ip.h>
// socket简单实现 

// 自定义端口号
// 需要考虑大小端的问题，也就是网络字节序和主机的大小端是否相同（一般不同）
// 一般主机的字节序是小端存储、网络字节序是大端存储
// 因此，需要自己转换字节序
#define SOCKPORT 8002

int main(int argc, char* argv[])
{
    // 创建socket 
    // 参数 
    // AF_INET: 套接字协议域为IPV4协议
    // SOCT_STREAM: 套接字类型为 流套接字（安全，稳定，不丢包，对应TCP协议）
    // 0 : 套接字使用默认协议（流式套接字为TCP，报式套接字为UDP协议）
    // TPC协议参数 IPPROTO_TCP

    int lfd = socket(AF_INET,SOCK_STREAM,0);  // 创建一个流式套接字
    if(lfd<0)
    {
        perror("socket error");
    }
    int lfd1 = socket(AF_INET,SOCK_DGRAM,0);  // 创建一个报式套接字
    int lfd2 = socket(AF_INET,SOCK_RAW,0);  // 创建一个原始套接字

    // 返回一个 socket描述符

    // SOCK_STREAM 是流式协议
    // SOCK_DGRAM  是报式协议
    // TCP是流式的代表
    // UDP是报式的代表

    // 使用 bind()函数，绑定IP地址、端口号
    struct sockaddr_in serAddr;
    /*
    sockaddr_in 结构体的参数
    in_family指代协议族，在socket编程中只能是AF_INET
	sin_port存储端口号（使用网络字节顺序），2个字节（16位，0-65535）
    sin_addr存储IP地址，使用in_addr这个数据结构
    */
    serAddr.sin_family  = AF_INET;
    // 端口号一般大于1000，找个没人用的端口号
    // 一般主机的字节序是小端存储、网络字节序是大端存储
    // 因此，需要自己转换字节序
    serAddr.sin_port = htons(SOCKPORT);
    // htons 将主机字节序转为网络字节序（短整型）
    // host to network short
    // 主机 向  网络    短整型

    // ip 地址 192.168.10.128 --- 主机（本地）字节序的字符串
    // uint32_t 应该是一个32位（4个字节的）整型
    // 因此要将 上面 本机字节序的字符串 转换为 一个 网络字节序的整型
    serAddr.sin_addr.s_addr = INADDR_ANY;
    // 使用 inet_pton()函数转换
	// 或者直接传入 INADDR_ANY 这是一个宏定义，值为0，传入之后可以是本机的任意一个IP地址

    // 给套接字绑定 IP地址、端口号
    int bret = bind(lfd,(struct sockaddr *)&serAddr,sizeof(serAddr));
    if(bret<0)
    {
        perror("bind error");
    }
    // 得有一个接收方
    // 监听，等着 lfd发送内容，第二个参数传一个大于0的数
    listen(lfd,64);

    // 接收
    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    struct sockaddr_in cliAddr;
    socklen_t addrlen = sizeof(cliAddr);
    int cfd = accept(lfd,(struct sockaddr*)&cliAddr,&addrlen);
    if(cfd<0)
    {
        perror("accept errot");
    }
    char buf[1024];
    // 进行通信
    while(1)
    {
        // 读取
        int read_count = read(cfd,buf,sizeof(buf));
        // 写到终端
        int write_count = write(STDOUT_FILENO,buf,read_count);
        // 再发回去
        write(cfd,buf,read_count);
    }

    return 0;
}

网络编程_创建网络应用程序

// // 联合体检测大小端
// union Un
// {
//     int a;
//     char c;
// }un;
// int main()
// {
//     un.a = 1;
    
//     if(un.c)
//     {
//         printf("小端\n");
//     }
//     else
//     {
//         printf("大端\n");
//     }

//     return 0;
// }