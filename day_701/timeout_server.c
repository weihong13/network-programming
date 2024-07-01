#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/select.h>
// 端口号
#define SOCKPORT 8001

// select 简单实现-timeout
int main(int argc, char* argv[])
{

    int lfd = socket(AF_INET,SOCK_STREAM,0);

    if(lfd < 0)
    {
        perror("socket error");
        exit(1);
    }

    // 绑定
    struct sockaddr_in serArr;
    
    serArr.sin_port = htons(SOCKPORT);
    serArr.sin_family = AF_INET;
    serArr.sin_addr.s_addr = INADDR_ANY;

    int bret =  bind(lfd,(struct sockaddr*)&serArr,sizeof(serArr));
    if(bret < 0)
    {
        perror("bind error");
        exit(1);
    }

    // 监听
    listen(lfd,64);
    printf("listening...\n");
    
    struct sockaddr_in cliArr;
    int len = sizeof(cliArr);

    // 设置读集合
    fd_set read_set,all_set;
    FD_ZERO(&all_set);
    FD_SET(lfd,&all_set);

    int maxfd = lfd; //当前最大的文件描述符

    char buf[1024];
    
    struct timeval timeout;
    // 设置超时时间为，5秒，10000微秒（1秒=1000毫秒，1毫秒=1000微秒）
    timeout.tv_sec = 5;
    timeout.tv_usec = 10000;

    

    while(1)
    {
        // 第一次只监听lfd，如果没有触发事件，select会一直阻塞
        read_set = all_set; // all_set是所有需要监听的文件描述符，read_set是触发事件的文件描述符
        // select只等待 5.01秒。没有事件发生就返回
        // timeout是一个传入传出的参数，传出的是剩余的时间，
        // 没有事件发生，剩余0了，就查看一下有没有发生，就一点也不等了，
        // 因此要在循环内设置时间
        timeout.tv_sec = 5;
        timeout.tv_usec = 10000;

        int sret = select(maxfd+1,&read_set,NULL,NULL,&timeout);
        if(sret ==-1)
        {
            perror("select error");
            exit(1);
        }
        else if(sret == 0)
        {
            printf("select ret = %d\n",sret);
            continue;
        }
        
        // lfd的读事件是否发生
        if(FD_ISSET(lfd,&read_set) == 1)
        {
            int cfd = accept(lfd,(struct sockaddr*)&cliArr,&len);
            if(cfd<0)
            {
                perror("accept error");
                exit(1);
            }
            FD_SET(cfd,&all_set);
            if(maxfd<cfd)
            {
                maxfd = cfd;
            }
            
            int port = ntohs(cliArr.sin_port);
            char bst[64];
            inet_ntop(AF_INET,&cliArr.sin_addr.s_addr,bst,sizeof(bst));
            printf("accept successful!\n");
            printf("client IP: %s\n",bst);
            printf("client Port: %d\n",port);
            if(--sret == 0)
            {
                continue;
            }
        }

        for(int i = lfd+1;i<maxfd+1;i++)
        {
            if(FD_ISSET(i,&read_set) == 1)
            {
                int read_count = read(i,buf,sizeof(buf));
                write(i,buf,read_count);
                write(STDOUT_FILENO,buf,read_count);
                if(--sret == 0)
                {
                    break;
                }
            }
        }
    }
    return 0;
}


