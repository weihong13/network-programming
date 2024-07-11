#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
// 端口号
#define SOCKPORT 8001

// epoll 简单实现
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

    // 用来读取的缓冲区
    char buf[1024];

    // ****** epoll ******
    
    // 先创建一个epoll 
    int epfd = epoll_create(64);
    if(epfd == -1)
    {
        perror("epoll_create error");
    }

    // 修改epoll -- 添加lfd
    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    struct epoll_event event; // 创建一个结构体，用来传入ctl
    event.events = EPOLLIN; // 表示监听读事件
    event.data.fd = lfd;    // 传入文件描述符，方便后续判断
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&event);

    // 创建传入 epoll_wait的结构体数组
    struct epoll_event events[1025];

    while(1)
    {
        // 创建事件等待函数，timeout设置为-1，就绪链表中没有事件，一直阻塞
        // timeout 设置为 500，阻塞500毫秒后返回
        int wret = epoll_wait(epfd,events,1025,-1); // 返回所有就绪态的事件个数
        if(wret ==-1) 
        {   // epoll_wait返回-1，出错提示
            perror("epoll_wait error");
            exit(1);
        }
        else if(wret == 0)
        {   // epoll_wait返回0，代表阻塞500毫秒期间，没有事件触发
            printf("epoll_wait ret = %d\n",wret);
            continue;
        }
        else 
        {
            // 处理wret大于0的情况，即有事件触发
            // wret记录的就是触发事件的个数，因此我们直接遍历wret即可，不用遍历1025个
            for(int i = 0; i < wret;i++) 
            {
                if(events[i].data.fd == lfd) // 如果是lfd触发的事件，需要进行accept连接
                {
                    if(events[i].events & EPOLLIN) // 判断是否为lfd的读事件发生，这里使用按位与，而不是==
                    {
                        int cfd = accept(lfd,(struct sockaddr*)&cliArr,&len); // 创建新的文件描述符
                        if(cfd < 0)
                        {
                            perror("accept error");
                            exit(1);
                        }
                        // 打印建立连接的客户端ip地址和端口号
                        int port = ntohs(cliArr.sin_port);
                        char bst[64];
                        inet_ntop(AF_INET,&cliArr.sin_addr.s_addr,bst,sizeof(bst));
                        printf("accept successful!\n");
                        printf("client IP: %s\n",bst);
                        printf("client Port: %d\n",port);
                        // 将新连接的客户端添加到epoll对象中。
                        event.data.fd = cfd;
                        event.events = EPOLLIN;
                        epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&event);

                    }
                }
                else // cfd，客户端触发事件
                {   
                    if(events[i].events & EPOLLIN) // 判断是否为客户端的读事件触发
                    {   
                        int read_count = read(events[i].data.fd,buf,sizeof(buf));
                        if(read_count == -1)
                        {   // 读取失败
                            perror("read error");
                            exit(1);
                        }
                        else if(read_count == 0)
                        {
                            // 表示客户端断开连接了
                            printf("客户端已断开连接\n");
                            // 将该文件描述符对应的epoll对象删除
                            epoll_ctl(epfd,EPOLL_CTL_DEL,events[i].data.fd,NULL); // 删除的时候，也就不用关注监听啥状态了，传NULL
                            sleep(10);
                            printf("server cfd close\n");
                            close(events[i].data.fd);
                        }
                        else
                        {   // 读取成功
                            // 写到终端
                            write(STDOUT_FILENO,buf,read_count);
                            // 发回客户端
                            write(events[i].data.fd,buf,read_count);
                        }   
                    }
                }

            }
           
        }
        
    }
    return 0;
}


