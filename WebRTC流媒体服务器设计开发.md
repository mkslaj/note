

# 安装信号

## 简单方式

* ```cpp
  signal(int sig , void(*func)(int));
  
  void sighandle(int sig)
  {
     std::cout<<"received signal"
      <<sig
         <<std::endl;
  }
  int main(int argc,char* argv[])
  {
      signal(SIGINT,sighandle);//
      signal(SIGQUIT,sighandle);
      signal(SIGHUP,sighandle);
      pause();
  }
  ```

  

## 高级方式

```cpp
#include <iostream>
#include<signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <fcntl.h>
int sigaction(int sig , const struct sigaction * act, struct sigaction *oact);//常用方式
//实现
void sighandlder(int sig)
{
    std::cout<<"received signal"
    	<<sig
        <<std::endl;
}
int main(int argc,char* argv[])
{
struct sigaction act,oact;
act.sa_handler= sighandlder;
sigfillset(&act.sa_mask);
act.sa_flags=0;
sigaction(SIGINT,&act,&oact);
}
```

# 创建后台进程

## fork方式

设置**守护进程**

* fork一个子进程，父进程退出，子进程成为孤儿进程，被init进程接管。

* 调用setsid建立新的进程会话

* 将当前工作目录切换的根目录

* 将标准输入，输出，错误信息重定向到 /dev/null

  ```cpp
  void daemonize()
  {
      int fd;
      pid_t pid;
      if (pid=fork()<0)
      {
          perror("fork son error");
      }
      else if (pid!=0)
      {
          exit(0);
      }
      //重定向
      setsid();
      //改变工作目录
      if (chdir("/") < 0)
      {
          perror("change dir !\n");
          exit(-1);
      }
      //重定向错误信息输出
      fd= open("/dev/null",O_RDWR);
      dup2(fd,STDIN_FILENO);
      dup2(fd,STDOUT_FILENO);
      dup2(fd,STDERR_FILENO);
  }
  ```

  

# 以 Daemon方式切换后台

## Daemon API

```cpp
if (daemon(0,0)==-1)
{
    std::cout<<"error\n"<<std::endl;
    exit(-1);
}
```

# TCP Server实现原理

## read write 和 recv send的区别

`read/write` 是标准的 POSIX I/O 函数，它们用于从文件描述符读取或写入数据。Socket 在 Linux 中也被视为文件描述符，因此可以使用 `read/write` 函数进行数据传输。这些函数是相对简单的，可以在任何类型的文件描述符上使用，但是它们不能像 `recv/send` 一样提供更多的控制选项。

`recv/send` 是专门用于网络数据传输的函数，它们提供了更多的控制选项，例如可以指定传输的数据类型、传输的标志位等。`recv/send` 函数是面向连接的，必须在已经建立连接的套接字上使用。此外，`recv/send` 函数还提供了超时控制和错误处理机制。

总的来说，`read/write` 和 `recv/send` 都可以用于网络数据传输，但使用 `recv/send` 更为常见，因为它们提供了更多的控制选项和更好的可靠性。但是，在某些特定的场景下，`read/write` 也可以使用，例如在非阻塞 Socket 编程中。

## 创建socket，指定使用tcp协议

```CPP
int socket(int domain, int type, int protocol);
//设置端口复用
	int opt = 1;
	setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

//函数描述: 创建socket
//参数说明:
//	domain: 协议版本
//	AF_INET IPV4
//	AF_INET6 IPV6
//	AF_UNIX AF_LOCAL本地套接字使用
//	type:协议类型
//	SOCK_STREAM 流式, 默认使用的协议是TCP协议
//	SOCK_DGRAM  报式, 默认使用的是UDP协议
//	protocal: 一般填0, 表示使用对应类型的默认协议.
//返回值: 
//      成功: 返回一个大于0的文件描述符
//      失败: 返回-1, 并设置errno
```

## 将socket与地址和端口绑定

```cpp
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
函数描述: 将socket文件描述符和IP,PORT绑定
参数说明:
	socket: 调用socket函数返回的文件描述符
	addr: 本地服务器的IP地址和PORT, 
		struct sockaddr_in serv ,clientaddr;
	std::memset(&serv, 0, sizeof(serv));
		serv.sin_family = AF_INET;
		serv.sin_port = htons(8888);
		serv.sin_addr.s_addr = htonl(INADDR_ANY);//htons 函数用于将 16 位的主机字节序转换为网络字节序
		//INADDR_ANY: 表示使用本机任意有效的可用IP
		// inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);
		//addrlen: addr变量的占用的内存大小 
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int ret=bind(socket_fd,  (struct sockaddr *)&serv, sizeof(struct sockaddr));
     返回值: 
	成功: 返回0
	失败: 返回-1, 并设置errno
             if (ret == -1) 
       {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    	}
```

## 监听端口

```cpp
int listen(int sockfd, int backlog);
函数描述: 将套接字由主动态变为被动态
参数说明:
	sockfd: 调用socket函数返回的文件描述符
	backlog: 同时请求连接的最大个数(还未建立连接)【并发数量 】
返回值:
	成功: 返回0
	失败: 返回-1, 并设置errno
int ret= listen( sockfd,  backlog);
if(ret==-1)
{
    std::cerr<<"Failed to listen socket"<<std::endl;
    exit(-1);
}
	for(;;)//死循环接受连接
    {
        int accept_fd=accept( sockfd, (struct sockaddr *)&clientaddr, sizeof(struct sockaddr));
        for(;;)//死循环收数据
        {
            int ret =recv(accept_fd,(void*)buf,sizeof(buf),0);
            if(ret==0)
            {
                break;
            }
            std::cout<<buf<<std::endl;
            send(accept_fd,(void*)buf,sizeof(buf),0);
        }
        close(accept_fd);
    }
		close(socket_fd);
		return 0;

```

## 接受连接请求

```c++

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);	
// 需要用struct sockaddr_in clinetaddr; 定义
函数说明:获得一个连接, 若当前没有连接则会阻塞等待.
函数参数:
	sockfd: 调用socket函数返回的文件描述符
	clientaddr: 传出参数, 保存客户端的地址信息
	addrlen: 传入传出参数,  addr变量所占内存空间大小
返回值:
	成功: 返回一个新的文件描述符,用于和客户端通信
	失败: 返回-1, 并设置errno值.
        accept函数是一个阻塞函数, 若没有新的连接请求, 则一直阻塞.
	从已连接队列中获取一个新的连接, 并获得一个新的文件描述符, 该文件描述符用于和客户端通信.  (内核会负责将请求队列中的连接拿到已连接队列中

```



## 使用recv接受数据

```cpp
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
char* buffer = new char[1024];
int buffer_size = sizeof(buffer)
recv(accept_fd,(void*)buf,sizeof(buf),0);
//对应recv和send这两个函数flags直接填0就可以了。
```



## 使用send发送数据

```c++
 ssize_t send(int sockfd, const void *buf, size_t len, int flags);	
 send(accept_fd,(void*)buf,sizeof(buf),0);
```



## 使用close关闭数据

```cpp
for(;;)//死循环接受连接
    {
        int accept_fd=accept( sockfd, (struct sockaddr *)&clientaddr, sizeof(struct sockaddr));
        for(;;)//死循环收数据
        {
            int ret =recv(accept_fd,(void*)buf,sizeof(buf),0);
            if(ret==0)
            {
                break;
            }
            std::cout<<toupper(buf)<<std::endl;
            send(accept_fd,(void*)buf,sizeof(buf),0);
        }
        close(accept_fd);
    }
	close(socket_fd);

```



## TCP常见套接字选项

* **SO_REUSEADDR** ：端口处于WAIT_TIME仍然可以启动
* **SO_RCVBUF**：调整缓冲区大小
* **SO_SNDBUF**：缓冲区太大会导致发送有延迟过大和过小都不好

# 没有封装的服务端TCP

```c++
int main(int argc,char* argv[])
{
    char buf[1024];
    int socket_fd= socket(AF_INET,SOCK_STREAM,0);
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    struct sockaddr_in serv,clientaddr;
    int addr_len =sizeof(struct sockaddr);
    std::memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(8888);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);//htons 函数用于将 16 位的主机字节序转换为网络字节序
    //INADDR_ANY: 表示使用本机任意有效的可用IP
    // inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);
    //addrlen: addr变量的占用的内存大小


    int ret;
    ret = bind(socket_fd, (struct sockaddr *) &serv, sizeof(struct sockaddr));
   
    if (ret == -1)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }
    int backlog=5;
    int tet;
    tet = listen(socket_fd,backlog);
    if(tet==-1)
    {
        std::cerr<<"Failed to listen socket"<<std::endl;
        exit(-1);
    }
    for(;;)//死循环接受连接
    {
        int accept_fd;
        accept_fd = accept(socket_fd, (struct sockaddr *) &clientaddr, reinterpret_cast<socklen_t *>(&addr_len));
        for(;;)//死循环收数据
        {
            int ret =recv(accept_fd,(void*)buf,sizeof(buf),0);
            if(ret==0)
            {
                break;
            }
            std::cout<<buf<<std::endl;
            send(accept_fd,(void*)buf,sizeof(buf),0);
        }
        close(accept_fd);
    }
```

# 封装的TCP服务端

## 头文件

```cpp
class TCPServer {
public:
    TCPServer(int port);
    bool start();
    void run();
    ~TCPServer();

private:
    int m_port;
    int m_server_fd;
};
```

## 源文件

```c++
#include "socket.h"
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

TCPServer::TCPServer(int port) : m_port(port), m_server_fd(-1) {}

bool TCPServer::start() {
    // 创建 Socket
    m_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    // 设置 Socket 选项，允许地址重用
    int opt = 1;
    if (setsockopt(m_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == -1) {
        std::cerr << "Failed to set socket option" << std::endl;
        return false;
    }

    // 绑定 IP 地址和端口号
    struct sockaddr_in server_addr;
    std::memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(m_port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(m_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }

    // 监听连接请求
    int backlog = 5;
    if (listen(m_server_fd, backlog) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return false;
    }

    std::cout << "Server started on port " << m_port << std::endl;

    return true;
}

void TCPServer::run() {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    for (;;) {
        // 接受连接请求
        int accept_fd = accept(m_server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (accept_fd == -1) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        std::cout << "Accepted new connection" << std::endl;

        // 接收和发送数据
        char buffer[1024];
        for (;;) {
            int ret = recv(accept_fd, buffer, sizeof(buffer), 0);
            if (ret == 0) {
                // 连接断开
                break;
            } else if (ret == -1) {
                // 接收数据出错
                std::cerr << "Failed to receive data" << std::endl;
                break;
            }

            std::cout << "Received data: " << buffer << std::endl;

            if (send(accept_fd, buffer, sizeof(buffer), 0) == -1) {
                // 发送数据出错
                std::cerr << "Failed to send data" << std::endl;
                break;
            }
        }

        // 关闭连接
        close(accept_fd);
        std::cout << "Connection closed" << std::endl;
    }
}

TCPServer::~TCPServer() {
    if (m_server_fd != -1) {
        close(m_server_fd);
    }
}
```

## main函数

```cpp
int main(int argc,char* argv[])
{
        TCPServer server(8888);
        if (!server.start()) {
            std::cerr << "Failed to start server" << std::endl;
            return 1;
        }
        server.run();
        return 0;
    }
```

# TCP  Client 实现步骤

## 创建socket

```cpp
int client_fd =socket(AF_INET,SOCK_STREAM,0);
    if (client_fd<0)
    {
        std::cout<<"Failed to create socket"<<std::endl;
        exit(-1);
    }
```



## **将socket与地址和端口绑定**

```cpp
    serv.sin_family=AF_INET;
    serv.sin_port= htons(8888);//端口号和服务端一致
    serv.sin_addr.s_addr= inet_addr("192.168.88.130");//inet_addr在#include <arpa/inet.h> ide自动识别没出来
```

## 连接服务端

```c++
 int conn_fd;
    conn_fd = connect(client_fd, ( struct sockaddr *)&serv, sizeof(struct sockaddr));
    if (conn_fd<0)
    {
        std::cout<<"Failed to connect"<<std::endl;
        exit(-1);
    }
```

## 循环发送数据和读取服务端返回数据

```cpp
char buf[1024];
    while(1)

    {
        memset(buf,0,sizeof (buf));
        gets(buf);//获取buf存入缓冲区在输入回车前阻塞函数（不安全可能缓冲区溢出）
        int ret=  send(client_fd,buf, sizeof(buf),0);
        if (ret<=0)
        {
            std::cout<<"Failed to send "<<std::endl;
        }
        if(strcmp(buf,"quit")==0)//如果收到quit就退出
        {
            break;
        }
        ret =recv(client_fd,buf, sizeof(buf),0);
        buf[ret]='\0';
        std::cout<<"recv:"<<buf<<std::endl;
    }
    close(client_fd);
    return 0;
```

## 退出

# 没有封装的客户端TCP

```cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
int main(int argc,char * argv[])
{
    struct sockaddr_in serv ;
    int client_fd =socket(AF_INET,SOCK_STREAM,0);
    if (client_fd<0)
    {
        std::cout<<"Failed to create socket"<<std::endl;
        exit(-1);
    }


    serv.sin_family=AF_INET;
    serv.sin_port= htons(8888);
    serv.sin_addr.s_addr= inet_addr("192.168.88.130");
    int conn_fd;
    conn_fd = connect(client_fd, ( struct sockaddr *)&serv, sizeof(struct sockaddr));
    if (conn_fd<0)
    {
        std::cout<<"Failed to connect"<<std::endl;
        exit(-1);
    }
    char buf[1024];
    while(1)

    {
        memset(buf,0,sizeof (buf));
        gets(buf);//获取buf存入缓冲区在输入回车前阻塞函数（不安全可能缓冲区溢出）
        int ret=  send(client_fd,buf, sizeof(buf),0);
        if (ret<=0)
        {
            std::cout<<"Failed to send "<<std::endl;
        }
        if(strcmp(buf,"quit")==0)//如果收到quit就退出
        {
            break;
        }
        ret =recv(client_fd,buf, sizeof(buf),0);
        buf[ret]='\0';
        std::cout<<"recv:"<<buf<<std::endl;
    }
    close(client_fd);
    return 0;
}
```

# 封装的客户端TCP

## 头文件

```cpp
#include <netinet/in.h>

class TCPClient {
public:
    TCPClient(const char* server_ip, int server_port);
    ~TCPClient();
    void run();

private:
    int client_fd_;
    struct sockaddr_in serv_;
};
```

## 源文件

```cpp
#include "Socket.h"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>

TCPClient::TCPClient(const char* server_ip, int server_port)//构造函数中创建socket初始化地址连接服务器
{
    // 创建客户端socket
    client_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd_ < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(-1);
    }

    // 设置服务器地址
    memset(&serv_, 0, sizeof(serv_));
    serv_.sin_family = AF_INET;
    serv_.sin_port = htons(server_port);
    serv_.sin_addr.s_addr = inet_addr(server_ip);//#include <arpa/inet.h>

    // 连接服务器
    int conn_fd = connect(client_fd_, (struct sockaddr*)&serv_, sizeof(serv_));
    if (conn_fd < 0) {
        std::cerr << "Failed to connect" << std::endl;
        exit(-1);
    }
}

TCPClient::~TCPClient()//在析构函数中退出连接
{
    close(client_fd_);
}

void TCPClient::run()//将循环发送读取封装到run函数
{
    char buf[1024];
    while (true) {
        memset(buf, 0, sizeof(buf));//初始化
        std::cout << "Please input message: ";
        std::cin.getline(buf, sizeof(buf));  // 读取用户输入

        int ret = send(client_fd_, buf, strlen(buf), 0);
        if (ret <= 0) 
        {
            std::cerr << "Failed to send" << std::endl;
        }

        if (strcmp(buf, "quit") == 0)//输入quit退出连接
        {
            break;
        }

        ret = recv(client_fd_, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            std::cerr << "Failed to receive" << std::endl;
        }
        else {
            buf[ret] = '\0';//buf[ret] = '\0' 的作用是在接收到的数据末尾添加一个 NULL 终止符，将接收到的数据转换为以 NULL 结尾的字符串。这样，当我们使用 std::cout 输出接收到的数据时，就可以确保输出的是一个完整的字符串，而不是一段未知的数据。
//需要注意的是，在将接收到的数据转换为字符串时，必须确保字符串的长度不超过 buf 的大小。因此，在接收数据之前，需要确保 buf 的大小足够大，以避免缓冲区溢出的问题
            std::cout << "Received message: " << buf << std::endl;
        }
    }
}
```

## main函数

### 在项目中设置端口IP

```cpp
#include <iostream>
#include <cstdlib>
#include "Socket.h"

int main(int argc, char* argv[])
{
    const char* server_ip = "192.168.88.130";  // 设置服务器 IP 地址
    int server_port = 8888;  // 设置服务器端口号
	TCPClient client(server_ip, server_port);//在项目中设置端口IP
     client.run();
```

### 在命令行启动程序时输入 端口IP 并且输入少于三个命令时报错

```cpp

    
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << std::endl;
        return -1;
    }//在命令行启动程序时输入 端口IP 并且输入少于三个命令时报错

    TCPClient client(argv[1], atoi(argv[2]));//argv[1] 表示程序启动时第二个命令行参数，即服务器 IP 地址。argv[2] 表示程序启动时第三个命令行参数，即服务器端口号。atoi() 函数用于将一个字符串转换为整数类型。因此，atoi(argv[2]) 将字符串类型的端口号转换为整数类型。
//综上所述，TCPClient client(argv[1], atoi(argv[2])) 的作用是创建一个 TCPClient 类型的对象 client，并将服务器的 IP 地址和端口号作为参数传递给它的构造函数，以便客户端能够连接到服务器
    client.run();

    return 0;
}
```

# 动态配置端口IP的办法

从外部配置文件中读取服务器的 IP 地址和端口号可以让您更轻松地配置和管理客户端程序。以下是一种读取配置文件的方法：

1. ## 创建一个配置文件，例如 `config.txt`，在其中添加以下内容：

```cpp
server_ip=192.168.0.100
server_port=8888
```

其中，`server_ip` 和 `server_port` 是用于指定服务器 IP 地址和端口号的配置项。

1. ## 在程序中添加代码，读取配置文件并解析其中的配置项：

```cpp
#include <fstream>
#include <string>
#include <map>

int main(int argc, char* argv[])
{
    std::string config_file = "config.txt";
    std::map<std::string, std::string> config;

    // 打开配置文件
    std::ifstream ifs(config_file);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open config file: " << config_file << std::endl;
        return -1;
    }

    // 解析配置文件
    std::string line;
    while (std::getline(ifs, line)) {
        std::size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            config[key] = value;
        }
    }

    // 从配置项中读取服务器 IP 地址和端口号
    std::string server_ip = config["server_ip"];
    int server_port = std::stoi(config["server_port"]);

    TCPClient client(server_ip.c_str(), server_port);
    client.run();

    return 0;
}
```

在这个示例中，我们使用 `std::ifstream` 类读取配置文件，并使用 `std::map` 类型的变量 `config` 存储配置项和其对应的值。在读取每一行配置项时，我们使用 `std::string::find()` 函数查找等号的位置，并使用 `std::string::substr()` 函数提取配置项名称和值。然后，我们将配置项和其对应的值存储到 `config` 变量中，以便后续使用。

最后，我们从 `config` 变量中读取服务器的 IP 地址和端口号，并将它们作为参数传递给 `TCPClient` 类的构造函数，以便客户端能够连接到服务器。

需要注意的是，这种方法需要确保配置文件中的配置项名称和程序中使用的名称相匹配，以避免出现解析错误的问题。另外，还需要确保配置文件的格式正确，并且配置项的值可以正确地转换为所需的数据类型，否则可能会导致程序崩溃或发生意外行为。

# UDP服务端

```cpp
//udp服务端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

int main()
{
	//创建socket
	int cfd = socket(AF_INET, SOCK_DGRAM, 0);//选择DGRAM
	if(cfd<0)
	{
		perror("socket error");
		return -1;
	}

	//绑定
	struct sockaddr_in serv;
	struct sockaddr_in client;
	bzero(&serv, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8888);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(cfd, (struct sockaddr *)&serv, sizeof(serv));

	int i;
	int n;
	socklen_t len;
	char buf[1024];
	while(1)
	{
		//读取数据
		memset(buf, 0x00, sizeof(buf));
		len = sizeof(client);
		n = recvfrom(cfd, buf, sizeof(buf), 0, (struct sockaddr *)&client, &len);

		//将大写转换为小写
		for(i=0; i<n; i++)
		{
			buf[i] = toupper(buf[i]);
		}
		printf("[%d]:n==[%d], buf==[%s]\n", ntohs(client.sin_port), n, buf);
		//发送数据
		sendto(cfd, buf, n, 0, (struct sockaddr *)&client, len);
	}

	//关闭套接字
	close(cfd);

	return 0;
}
```

# UDP客户端

```cpp
//udp客户端
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <ctype.h>

int main()
{
	//创建socket
	int cfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(cfd<0)
	{
		perror("socket error");
		return -1;
	}

	int n;
	char buf[1024];
	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(8888);
	inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);//serv.sin_addr.s_addr=inet_addr("192.168.88.130");

	while(1)
	{
		//读标准输入数据
		memset(buf, 0x00, sizeof(buf));
		n = read(STDIN_FILENO, buf, sizeof(buf));

		//发送数据
		sendto(cfd, buf, n, 0, (struct sockaddr *)&serv, sizeof(serv));

		//读取数据
		memset(buf, 0x00, sizeof(buf));
		n = recvfrom(cfd, buf, sizeof(buf), 0, NULL, NULL);
		printf("n==[%d], buf==[%s]\n", n, buf);
	}

	//关闭套接字
	close(cfd);

	return 0;
}
```

# 高性能网络服务器

## epoll实现高性能网络服务器

### epoll的好处

* 没有文件描述符的限制
* 工作效率不会随着文件描述符的增加而下降
* epoll经过系统优化更高效

### epoll的事件触发模式

* Level Trigger (水平触发)没有处理反复发送 

* Edge Trigger (边缘触发)只发送一次  #效率更高，编写困难 

### epoll的重要API

```cpp
int epoll_create();//参数无意义可忽略
int epoll_ctl(epfd,op,fd,struct epoll_event*event);// OP：EPOLL_CTL_ADD添加描述符   EPOLL_CTL_MOD修改描述符  EPOLL_CTL_DEL删除描述符
int epoll_wait(epfd,events,maxevents,timeout);//等待监听的所有fd相应事件的产生
```

### epoll事件（epoll_event）

* EPOLLET(设置边缘模式)

* EPOLLIN(收事件)

* EPOLLOUT(写事件)

* EPOLLPRI(中断事件)

* EPOLLERR(出现错误)

* EPOLLHUP(事件挂起)

### epoll重要结构体

```cpp
     typedef union epoll_data {   //union只能用其中一项
               void        *ptr;
               int          fd;
               uint32_t     u32;
               uint64_t     u64;
           } epoll_data_t;
 
           struct epoll_event {
               uint32_t     events;      /* Epoll events */     
               epoll_data_t data;        /* User data variable */  //文件描述符
           };

```

### epoll服务器代码

```c++
//略
```

## epoll+fork实现高性能服务器

## 异步事件**会出现惊群现象**：唤醒多个进程但只有一个进程运行

**解决：**

1.  实际情况中，在发生惊群时，并非**全部**子进程都会被唤醒，而是**一部分**子进程被唤醒。但被唤醒的进程仍然只有1个成功accept，其他皆失败。
2. 所有基于linux epoll机制的服务器程序在多进程时都受惊群问题的困扰，包括 lighttpd 和[nginx](http://www.haodaima.net/tag/nginx) 等程序，各家程序的处理办法也不一样。
3. lighttpd的解决思路：**无视惊群**。采用Watcher/Workers模式，具体措施有[优化](http://www.haodaima.net/tag/优化)fork()与epoll_create()的位置（让每个子进程自己去epoll_create()和epoll_wait()），捕获accept()抛出来的错误并忽视等。这样子一来，当有新accept时仍将有多个lighttpd子进程被唤醒。
4. nginx的解决思路：**避免惊群**。具体措施有使用全局互斥锁，每个子进程在epoll_wait()之前先去申请锁，申请到则继续处理，获取不到则等待，并设置了一个负载均衡的[算法](http://www.haodaima.net/tag/算法)（当某一个子进程的任务量达到总设置量的7/8时，则不会再尝试去申请锁）来均衡各个进程的任务量。
5. 一款国内的优秀商业MTA服务器程序（不便透露名称）：采用Leader/Followers线程模式，各个线程地位平等，轮流做Leader来响应请求。
6. 对比lighttpd和nginx两套方案，前者实现方便，逻辑简单，但那部分无谓的进程唤醒带来的资源浪费的代价如何仍待商榷（有网友测试认为这部分开销不大 http://www.iteye.[com](http://www.haodaima.net/tag/com)/topic/382107）。后者逻辑较复杂，引入互斥锁和负载均衡算分也带来了更多的程序开销。所以这两款程序在解决问题的同时，都有其他一部分计算开销，只是哪一个开销更大，未有[数据](http://www.haodaima.net/tag/数据)对比。
7. 坊间也流传Linux 2.6.x之后的[内核](http://www.haodaima.net/tag/内核)，就已经解决了accept的惊群问题，论文地址 http://static.usenix.org/event/usenix2000/freenix/full_papers/molloy/molloy.pdf 。
8. 但其实不然，这篇论文里提到的改进并未能彻底解决实际生产环境中的惊群问题，因为大多数多进程服务器程序都是在fork()之后，再对epoll_wait(listen_fd,...)的事件，这样子当listen_fd有新的accept请求时，进程们还是会被唤醒。论文的改进主要是在内核级别让accept()成为原子操作，避免被多个进程都调用了。

 **【采用方案】**

  多方考量，最后选择参考lighttpd的Watcher/Workers模型，实现了我需要的那款多进程epoll程序，核心流程如下：

1. 主进程先监听端口， listen_fd = socket(...); ，setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR,...)，setnonblocking(listen_fd)，listen(listen_fd,...)。

2. 开始fork()，到达子进程数上限（建议根据服务器实际的CPU核数来配置）后，主进程变成一个Watcher，只做子进程维护和信号处理等全局性工作。

3. 每一个子进程（Worker）中，都创建属于自己的epoll，epoll_fd = epoll_create(...);，接着将listen_fd加入epoll_fd中，然后进入大循环，epoll_wait()等待并处理事件。千万注意， epoll_create()这一步**一定要在fork()之后**。

4. 大胆设想（未实现）：每个Worker进程采用[多线程](http://www.haodaima.net/tag/多线程)方式来提高大循环的socket fd处理速度，必要时考虑加入互斥锁来做同步，但也担心这样子得不偿失（进程+线程频繁切换带来的额外操作系统开销），这一步尚未实现和测试，但看到nginx源码中貌似有此逻辑。

   

   ## 未解决的代码

```cpp
pid_t pid=-1;//初始化为-1
for(int i=0;i<max;i++)
{
    if(pid!=0)
    {
        pid=fork();
    }
}
if(pid==0)
{
    tcp_epoll  te;//创建tcpeopll类对象
    te.create();//每个子进程创建epoll连接
    te.run();  //调用它的run函数
}
else
{
  do
  {
   pid=waitpid(-1,NULL,0);   
  } while(pid!=-1);
}
```



# 利用I/O事件处理库实现高性能网络服务器

## 比较有名的异步事件处理库

### libevent

### linux：epoll

#### mac：kqueue

#### windows：iopc

### nginx：http服务器

### libevthp：使用libevent制作的http基础库

### libuv：Node.js 内部使用libuv，js用v8引擎解析

### libev：改善libevent代码不利于阅读，只支持Linux

## libevent重要函数

```cpp
event_base_new ==  epoll_create  //常用

event_base_dispatch == epoll_wait   //常用

event_new  + event_add   +  event_del  + event_free  ==  epoll_ctl   

```

## libevebt中的 evconnlistener_new_bind   ：把socket和事件侦听事件触发封装到一起 

```cpp
//重要结构
typedef
    void(*evconnlistener_cb)
    (struct evconnlistener*,
     evutil_socket_t,
     struct sockaddr*,
     int socklen,void*);
struct evconnlistener * evconnlistener_new_bind(struct event base *base,
                                                 evconnlistener_cb cb,
                                               		void *ptr,
                                               		unsigned flags,
                                               		int backlog ,
                                               	  const struct sockaddr* sa,
                                                	int socklen);


```

# libevent实现高性能服务器   

## **libevent下载 linux：** 

wget  -c  https://github.com/libevent/libevent/releases/download/release-2.1.11-stable/libevent-2.1.11-stable.tar.gz --no-check-certificate	

## bufferevent的作用

从外面看就是一个缓冲区，可以与socket绑定

内部由输入和输出缓冲区组成

每一个socket对应一个bufferevent

当socket有事件触发，可以设置回调函数

## 没封装的libevent的代码：

```cpp
#include <iostream>
#include <event2/event.h>
#include <arpa/inet.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

void on_read_cb(struct bufferevent *bev,void *ctx )//bufferevent_setcb的readcb的回调函数
{
    struct evbuffer *input,*output = NULL;
    input= bufferevent_get_input(bev);
    output=bufferevent_get_output(bev);
    evbuffer_add_buffer(output,input);
}
void on_accept_cb(struct evconnlistener* listener,//evconnlistener* listener=evconnlistener_new_bind  cb的回调函数
                    evutil_socket_t fd,
                    struct sockaddr*addr,
                    int socklen,
                    void *ctx)
{
    struct event_base*base=NULL;//初始化base
    struct bufferevent *bev=NULL;//初始化bufferevent
    base= evconnlistener_get_base(listener);//获取base
    bev= bufferevent_socket_new(base,fd,0);//创建bufferevent
    bufferevent_enable(bev,EV_READ|EV_WRITE);//给bufferevent设置事件 读|写
    bufferevent_setcb(bev, on_read_cb,NULL,NULL,NULL);//设置回调函数
}


int main(int argc,char* argv[])
{
    struct evconnlistener* listener=NULL;
    struct sockaddr_in serveraddr;
    struct event_base *base =NULL;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port= htons(9999);
    serveraddr.sin_addr.s_addr=INADDR_ANY; //设置任意IP可用
    base=event_base_new();
    listener=evconnlistener_new_bind(base,on_accept_cb,NULL,LEV_OPT_REUSEABLE,10,(struct sockaddr*)&serveraddr, sizeof(serveraddr));
    event_base_dispatch(base);
    return 0;
}
```

## 封装的libevent代码

### 头文件

```cpp
#include <event2/event.h>
#include <arpa/inet.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

class LibeventServer {
public:
    LibeventServer();
    ~LibeventServer();
    void run();

private:
    static void OnRead(struct bufferevent* bev, void* ctx);
    static void OnAccept(struct evconnlistener* listener, evutil_socket_t fd,
                         struct sockaddr* addr, int socklen, void* ctx);

    struct event_base* base_;
    struct evconnlistener* listener_;
};
```

### 源文件

```cpp
#include <cstring>
#include <iostream>
#include "libeventServer.h"
LibeventServer::LibeventServer() {
    base_ = event_base_new();
}

LibeventServer::~LibeventServer() {
    if (listener_) {
        evconnlistener_free(listener_);
    }
    if (base_) {
        event_base_free(base_);
    }
}

void LibeventServer::run() {
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(9999);
    serveraddr.sin_addr.s_addr = INADDR_ANY; //设置任意IP可用

    listener_ = evconnlistener_new_bind(base_, OnAccept, this,
                                        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
                                        -1, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (!listener_) {
        std::cerr << "Failed to create listener\n";
        return;
    }

    event_base_dispatch(base_);
}

void LibeventServer::OnRead(struct bufferevent* bev, void* ctx) {
    struct evbuffer* input = bufferevent_get_input(bev);
    struct evbuffer* output = bufferevent_get_output(bev);
    evbuffer_add_buffer(output, input);
}

void LibeventServer::OnAccept(struct evconnlistener* listener, evutil_socket_t fd,
                              struct sockaddr* addr, int socklen, void* ctx) {
    LibeventServer* server = static_cast<LibeventServer*>(ctx);
    struct event_base* base = evconnlistener_get_base(listener);
    struct bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, OnRead, nullptr, nullptr, nullptr);
    bufferevent_enable(bev, EV_READ | EV_WRITE);
}
```

# 网络协议

## ip协议头

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230515154610968.png" alt="image-20230515154610968" />

### 1.版本(Verson)

由 4 比特构成，用来标识 IP 首部的版本号。IPv4 版本号就是 4。关于 IP 所有版本如下图所示：

| 版本 | 简称  | 协议                        |
| ---- | ----- | --------------------------- |
| 4    | IPV4  | Internet Protocol           |
| 5    | ST    | ST Datagram Mode            |
| 6    | IPV6  | Internet Protocol version 6 |
| 7    | TP/IX | The Next Internet           |
| 8    | PIP   | The P Internet Protocol     |
| 9    | TUBA  | TUBA                        |



### 2.首部长度(IHL)

表示 IP 首部的大小，单位是 4字节(32bit)。IP 头部的长度也就是 length * 4 的字节数. 当没有可选项时，length 是 5，也就是20字节。

### 3.区分服务(TOS)

用来表示服务的质量。可划分为 DSCP 和 ECN，DSCP 用来进行质量控制，ECN 用来报告网络拥堵情况。

### 4.总长度(Total Length)

表示的是 IP 首部 与数据部分合起来的总字节数，最大长度是 65535 字节。

### 5.标识(Identification)

用于分片重组。同一个分片的标识值是相同的，不同分片的标识值不同。每发送一个 IP 包，它的值也会逐渐递增。

### 6.标志(Flags)

表示包被分片的相关信息。每一位的具体含义如下图。

| 比特 | 含义                                                         |
| ---- | ------------------------------------------------------------ |
| 0    | 未使用。现在必须是0.                                         |
| 1    | 指示是否进行分片（Don’t Fragment）   0-可以分片    1-不能分片 |
| 2    | 包被分片的情况下，表示是否为最后一个包    0-最后一个分片的包  1-分片中段的包 |

### 7.片偏移(Fragment Offset)

用来标识被分片的每一个分段相对于原始数据的位置。

### 8.生存时间(TTL)

这个不是一个时间的概念，实际上是指可以中转多少个路由器的意思，每经过一个路由器，TTL会减少 1，直到变成0 则丢弃该包。

### 9.协议(Protocol)

表示 IP 首部的下一个首部属于哪个协议。

### 10.首部校验和(Header Checksum)

该字段只会校验数据包的首部，不会去校验数据部分。这个字段主要目的是用来确保 IP 数据包不被破坏。

### 11.源地址(Source Address)

表示发送端的 IP 地址。

### 12.目标地址(Destination Address)

表示接收端的 IP 地址。

### 13.可选字段(Options)

长度可变，通常只在进行实验或诊断时使用。

### 14.填充(Padding)

在有可选字段的情况下，首部长度可能不是 32 比特的整数倍。通过向字段填充 0，调整为 32 比特的整数倍。

### 15.数据(Data)

用来存入实际要传输的数据，同时将 IP 上层协议的首部也作为数据进行处理。



## MTU

Maximum Transmission Unit，指在网上可传输数据的最大尺寸。

可通过ICMP查询最大传输单元。它通过设置IP层的DF【Don’t Fragment】不分片为，如果将这一bit置为1，IP层将不对数据报进行分片。

获取MTU的好处是传输过程中不拆包，提高传输效率。以太网默认是1500B。

## Tcp协议头

![在这里插入图片描述](https://img-blog.csdnimg.cn/1ad5686d0b7b44959f515b7b336dcb7f.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBAZHlpbmcg5pCB5rWF,size_20,color_FFFFFF,t_70,g_se,x_16)

### 源端口号 Source Port： 

一般来说就是客户端发起请求时使用的端口号，这个在浏览其发起请求时会自动分配一个。比如这里我被分配的端口号就是 58173。这个作用是服务端给你返回数据的时候能找到你客户端的端口

### 目的端口号 Destintion Port： 

这个就是你要访问的服务的端口号，比较常用的如 8080 ，我这里是 18080。同理，你请求到了服务端总要知道服务是由哪个端口提供的吧。

### 序号 Sequence Number：

 TCP 为了保证包的顺序到达，就需要给每个包一个需要来标识它的顺序，它解决了乱序的问题，当然这个逻辑也是在两端程序逻辑控制的，真正的包到了网络中是怎么样的，那对你来说可就是鞭长莫及干瞪眼了。（祈祷他能平安到达吧）

关于序号的生成规则

```
TCP 是面向字节流的，这意味着 TCP 在传输数据的过程中会为每个字节按照顺序进行编号。例如对于传输 10 kb 的 HTML 文档                  10 kb = 10 * 1024 byte = 10240 byte 一共 10240 个字节。
序号是 32 位 是 2^32 是 4 294 967 296 = 4G 所以 TCP 可以对 4G 的数据进行编号，超过之后回 0 重新计数那对于上面的 10 kb 其编号范围就是 [0,10239]
这里的序号标识的是此报文段中，数据的第一个字节的序号，如上述 10 kb 数据我们分成两等分传输
第一段： 5 kb Sequence Number = 0 ； 其范围是 [0,5119]
第二段： 5 kb Sequence Number = 5120 ； 其范围是 [5120,10239]
相信大家可能注意到了一点，超出 4G 重复使用编号的问题，这种情况其实基本不用担心，一般情况，旧的序号早就到达终点，或者丢失了。
```



### 确认序号 Acknowledgement Number： 

为了保证可靠性，发出去的包需要确认到达，就有了确认序号，这个解决了丢包的问题，一定时间没确认收到就重发嘛。我们 TCP 能做啥呢，就是不断的重传，寄希望于能得到对端的收到确认，卑微也就至此了。

```
同样的 32 位确认序号，表示的是 期望收到对方下一个报文的序号值。
TCP 的可靠就是基于此，我们要确认每个发送出的报文都被确认收到了。
通讯双方在接到对方的报文后，都需要发送一个对应的确认报文，要告知已经收到，有确认报文就需要确认好
如第一段： 5 kb Sequence Number = 0 ； 其范围是 [0,5119] 发送过来，此时回 ACK 确认报文 Acknowledgement Number = 5120
```

### 标志位 Flags： 标志位共 16 位，首部占 4 位，保留 6 位，剩余 6 位每没一位标志不同的意思

#### 1.首部 Header Length 4 位:

表示 TCP 报文段首部长度，看了下有的地方也叫它数据偏移，可以理解为 TCP 报文起始位置 到 数据部分 payload 的起始位置 也就是上图中 [源端口号 , 选项, 填充 ] 这部分数据的大小，4 位 0000 最大为 1111 = 15 ，这里每个数代表 4 byte，所以首部最大是 15 * 4 byte = 60 byte

#### 2.保留 6 位 保留 6 位默认给 0，用于之后的拓展

### 3.标志位 (Flags)目前 6 位下图，0 表示没有，1 表示标识，每位代表不同含义

#### 1.紧急 Urgent URG 该位为 1 表示后面要说到的 紧急指针 有效，用于告诉系统，此报文段中包含紧急数据，应该尽快传送，而不是按原有顺序排队传，紧急吗，就是要加急传输。

#### 2.确认 Acknowledgment ACK 该位为 1 表示确认序号 Acknowledgement Number 有效，一般称携带 ACK 标志的 TCP 报文段称为确认报文段，TCP 规定在建立连接后所有的传输报文都必须报 ACK 设置为 1

#### 3.推送 Push PUS 该位为 1 表示该报文高优先级，接收方 TCP 应该尽快将其推送给应用服务，而不用等到 TCP 缓存填满后再一起发送

#### 4.复位 Reset RST 该位为 1 表示在 TCP 连接中出现了严重的错误，需要释放并重新建立连接，一般携带 RST 标志的报文段称之为 复位报文段

#### 5.同步 Syn SYN 该位用于标识此报文为同步报文，SYN = 1 ACK = 0 表示此次是 请求建立连接报文段，如果对方同意建立连接，则会返回一个 SYN = 1 ACK = 1 的响应报文。

#### 6.终止 Fin FIN 用于标识断开连接，为 1 是表示此报文的数据发送方数据已发送完毕，请求断开释放连接，这个在 ”四次挥手“ 时会用到。

### 窗口大小 Window ： 

```
16 位，该字段指定此时允许对方发送的最大数据量，也就是说此数据是我方缓冲区剩余大小，用于控制发送数据的速度
窗口大小是指，从本端报文的 确认序号 Acknowledgment Number 开始，还允许对方发送的数据量。比如 Acknowledgment Number = 100 Window = 60 ，那么标识报文发送方还有 60 字节的接收空间，即序号范围是 [100,159] 的数据。
```

### 校验和 CheckSum： 

16 位 用于检测 TCP 报文段在传输过程中有没有损坏，如果损耗则丢弃，采用 CRC 算法，这个是保证 TCP 可靠传输的重要一环。 具体生成校验规则，感兴趣同学可以自行搜索算法。

### 紧急指针 Urgent Pointer： 

16 位 ，当标志位 URG 为 1 时有意义，用于指出报文段中紧急数据的字节数。发送方会将紧急数据插入到本次报文段的最前面，而后面的仍然是普通数据，紧急指针指示的是紧急数据的末尾在本段报文数据的位置。

# RTP

## RTP协议头

![img](https://img-blog.csdnimg.cn/img_convert/ee2833d72c436bda962b2fa36f420332.png)

```CPP
版本号（V）：2比特，用来标志使用的RTP版本。
填充位（P）：1比特，如果该位置位，则该RTP包的尾部就包含附加的填充字节。
扩展位（X）：1比特，如果该位置位的话，RTP固定头部后面就跟有一个扩展头部。
CSRC计数器（CC）：4比特，含有固定头部后面跟着的CSRC的数目。
标记位（M）：1比特,该位的解释由配置文档（Profile）来承担.
载荷类型（PT）：7比特，标识了RTP载荷的类型。
//序列号（SN）：16比特，发送方在每发送完一个RTP包后就将该域的值增加1，接收方可以由该域检测包的丢失及恢复包序列。序列号的初始值是随机的。
//时间戳：32比特，记录了该包中数据的第一个字节的采样时刻。在一次会话开始时，时间戳初始化成一个初始值。即使在没有信号发送时，时间戳的数值也要随时间而不断地增加（时间在流逝嘛）。时间戳是去除抖动和实现同步不可缺少的。
同步源标识符(SSRC)：32比特，同步源就是指RTP包流的来源。在同一个RTP会话中不能有两个相同的SSRC值。该标识符是随机选取的 RFC1889推荐了MD5随机算法。
贡献源列表（CSRC List）：0～15项，每项32比特，用来标志对一个RTP混合器产生的新包有贡献的所有RTP包的源。由混合器将这些有贡献的SSRC标识符插入表中。SSRC标识符都被列出来，以便接收端能正确指出交谈双方的身份。
```

# RTCP

一. 在[音视频](https://so.csdn.net/so/search?q=音视频&spm=1001.2101.3001.7020)通话中，我们常常使用 RTP 协议包承载音视频负载数据，使用 RTCP 协议包反馈当前传输统计数据，控制传输特性以保证提供较高的服务质量，本文将介绍 RTCP 协议以及 RTCP 常用报文的应用。

## 二. RTCP协议报文

​        RTCP（Realtime Transport Control Protocol）称为传输控制报文协议，它是基于 UDP 协议之上的应用协议。在网络中传输 RTP 数据难免存在丢包，乱序等问题，当网络链路质量变差时接收端如何快速能够通知到源端并作出响应动作，以保证提供较好的传输质量，这便是 RTCP 报文的作用。

        RTCP 报文格式如下所示，不同类型的报文有不同的 Data 结构。

![img](https://img-blog.csdnimg.cn/7db10bc2d7194144ba0fc21c26cf6497.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5qKm5Yir5YGc,size_20,color_FFFFFF,t_70,g_se,x_16)

Version：RTCP 报文版本

P：填充标识，如果该位为 1 表示 RTCP 报文最后有填充字节

Count：报文类型不同时含义不同，见具体报文类型解释

Type：报文类型，常见类型有 SR（发送报告报文），RR（接收报告报文），SDES（源端描述信息报文），BYE（会话结束报文），APP（应用自定义报文）

Length：表示 RTCP 报文长度，报文长度为 (Length+1) * 4

## **RR 报文协议格式（Receiver Report）**

<img src="https://img-blog.csdnimg.cn/61b669f4a33b4a868c81069765343fef.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5qKm5Yir5YGc,size_20,color_FFFFFF,t_70,g_se,x_16" alt="img" style="zoom:67%;" /> 

用户会发送所接收流的接收报告，发送端才能知道当前接收端的接收状态，接收报告通常包含多个 report block。

SSRC_n：流标识

fraction lost：从上一次 RR 报文发送之后的丢包率

cumulative number of packets lost：从开始接收 SSRC_n 数据包到当前时刻总共接收丢失的包数 

extend highest sequence number received：从 SSRC_n 收到的 RTP 数据包中最大的序列号，其中低 16 位表示收到的最大 seq，高 16 位表示 seq 循环次数

interarrival jitter：接收抖动

last SR (LSR)：上一次接收到 SSRC_n SR 包时间戳（取 SR 报文 NTP 时间戳的中间 32 位）

delay since last SR (DLSR)：上一次接收到 SSRC_n SR 报文到本 RR 报文发送的延时

结合 LSR 和 DLSR，发送端可以测算 RTT 时间，RTT 为发送端收到 RR 报文的时间减去 LSR 再减去 DLSR 时间。

![image-20230520152110010](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520152110010.png)

## **SR 报文协议格式（Sender Report）**

<img src="https://img-blog.csdnimg.cn/1661b902912a41dbb3800e02c7b6a3f7.png" alt="img" style="zoom: 67%;" />

流的生产者需要发送 SR 报文，SR 报文包含该流的发送包数，发送字节数，流的接收者可以根据发送者报文结合自己的接收情况计算丢包率等信息。

1. SSRC：流标识
2. NTP timestamp：NTP 时间戳，64 bit，表示绝对时间戳
3. 高 32 位 seconds 表示从 1900 年 1 月 1 日到现在经历的秒数，ntp.seconds = ms / 1000
4. 低 32 位 fractions 用于表示剩余微秒部分，其值为剩余的微秒部分乘以 2^32 后四舍五入的结果值
5. 假设目前得到的时间戳是 3877076839.123456s，则 seconds = 3877076839，fractions = 0.123456 * 2^32 + 0.5（530239482）

```cpp
static constexpr uint64_t NtpFractionalUnit{ 1LL << 32 };
 
struct Ntp {
 
        uint32_t seconds;
 
        uint32_t fractions;
 
}
```


RTP timestamp：相对时间戳。它的单位表示经过一个采样间隔的时间，例如对于音频 8K 的采样频率，采样间隔为 1/8 ms，timestamp 加一表示经过了 1/8 ms，其初始值随机产生。

sender's packet count：发送的总包数

sender's octet count：发送的总字节数

SR 报文内容也可以同时包含接收报告。
## SDES 报文协议格式（Source Description）

<img src="https://img-blog.csdnimg.cn/7161da0edd574bd6be6931be7cb9d5a3.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5qKm5Yir5YGc,size_20,color_FFFFFF,t_70,g_se,x_16" alt="img" style="zoom:50%;" />

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520154604398.png" alt="image-20230520154604398" style="zoom: 33%;" />

该报文用于描述 SSRC 信息，PT = 202，SC 表示描述的 SSRC SDES 个数，SDES items 是TLV 格式数。

**SDES item：**<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520155048233.png" alt="image-20230520155048233" style="zoom:33%;" />

可描述信息有：流对应的 NAME，关联的 EMAIL，PHONE，LOC 等信息。

WebRTC 在媒体协商时会为每个 SSRC 设置 CNAME，然后通过 RTCP SDES 报文确认 CNAME 与 SSRC 的关系

## BYE 报文协议格式（Goodbye）

<img src="https://img-blog.csdnimg.cn/7c0b6edca6a14940a4308189f62cc942.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5qKm5Yir5YGc,size_20,color_FFFFFF,t_70,g_se,x_16" alt="img" style="zoom: 50%;" />

该报文用于 SSRC 流离开/结束通知，PT = 203，SC 表示 SSRC 个数。

## APP：应用程序定义的RTCP包

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520155647129.png" alt="image-20230520155647129" style="zoom:33%;" />

ST：5位，应用自定义的在同一个名字下的子类型

name：4字节，应用自定义的名字

最后一类RTCP包（APP）允许应用程序来自己定义扩展。它的包类型为204，由4个字符组成唯一的标识，每个字符都得从ASCII字符集中选择，并区分大小写。建议选择包名称来匹配它所代表的应用程序，并由应用程序来协调子类型值的选 择。包其余部分被用于应用程序的特定需求。

应用程序自定义的包用于RTCP的非标准扩展和验证新特性。目的是，验证者首先使用APP 来验证新特性，然后如果新特性有广泛的用途，那么就注册为新的包类型。一些应用程序 生成的包或实现方案，应该忽略识别不出来的应用程序包
## RTPFB / PSFB 报文协议格式（Feedback）

<img src="https://img-blog.csdnimg.cn/98654fa88b8643d78713e2b60a9be416.png?x-oss-process=image/watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBA5qKm5Yir5YGc,size_20,color_FFFFFF,t_70,g_se,x_16" alt="img" style="zoom:50%;" />

SSRC of media source：表示是对哪个媒体源的反馈报告

Feedback Control Information：反馈内容，需要根据具体反馈类型确定内容含义

RTPFB 称为传输层反馈包，PT = 205，主要用于传输控制，FMT 表示反馈的具体子类型。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520160440572.png" alt="image-20230520160440572" style="zoom: 33%;" />

PT	FMT	RTPFB Type	说明
       205	1	          NACK	   丢包反馈
       205	3	        TMMBR	  临时最大码率设置请求
       205	4	        TMMBN	  临时最大码率设置响应
       205	15       	TFB        	传输带宽反馈（收包状况的反馈信息）
       PSFB 称为负载反馈包，PT = 206。

PT	FMT	PSFB Type        	说明
       206	1	            PLI   	 图像丢失指示
       206	2	            SLI	    片丢失指示
       206	3	          RPSI	   参考图像丢失
       206	4	           FIR     	关键帧请求
       206	5	         TSTR	   时空交换请求
       206	6         	TSTN	   时空交换响应

PLI 和 FIR 很类似，发送端收到 PLI 或者 FIR 报文时都会触发生成关键帧的动作，但 PLI 通常在接收端无法解码图像时候使用（例如因丢包无法收到帧的完整 RTP 包），而 FIR 主要用于主动申请关键帧（例如会议场景，新加入会议的用户需要主动请求其他用户的关键帧才能看到画面，如果一直收到参考帧是无法单独解码的）。

## XR 报文协议格式（Extended Report）

<img src="https://img-blog.csdnimg.cn/5e53c3ea0cdf4e2eb21f46d365486aa7.png" alt="img" style="zoom:50%;" />

XR 报文 PT=207，主要用来提供更详细的时间，丢包等统计信息，其包体由 report blocks 组成。

<img src="https://img-blog.csdnimg.cn/0f81101546bb43518e357b2f784bd94c.png" alt="img" style="zoom:150%;" />

report block 构成如上所示，BT 指示该 block 的类型，block 有 7 种类型，分别是 LRLE(BT=1)，DRLE(BT=2)，PRT(BT=3)，RRT(BT=4)，DLRR(BT=5)，SS(BT=6)，VM(BT=7)，block length 指示块长度，以及跟 BT 类型相关的特定含义的字段，比较常用的有 RRT 和 DLRR。

<img src="https://img-blog.csdnimg.cn/73f78a22fb464da7a3f1b1ed221d12bf.png" alt="img" style="zoom:150%;" />

RRT (Receiver Reference Time) 报告块格式如上，主要包含发送该报告块的 NTP 时间，发送该报告块主要用于接收端结合 DLRR 报告块测算 RTT 时间。

<img src="https://img-blog.csdnimg.cn/44b966c40fa74734a1fa0f6450d7c2c6.png" alt="img" style="zoom:150%;" />

DLRR (Delay Since Last RR) 报告块格式如上，LRR 表示上次收到的 SSRC_1 RRT 报告块里的 NTP 时间的中间 32 位，DLRR 表示上次接收到 RRT 报告块到发送端发送此 DLRR 报告块的时间差。

当接收端收到此报告块时，就可以测算 RTT = 当前时间 - RRT 时间 - DLRR 时间。

mediasoup 中 NACK 模块的 RTT 便是用此方式计算得到的。

# STUN协议

## 1.概述

STUN(session traversal utilities for NAT),一种处理NAT传输的协议，主要作为工具来服务其他协议。它允许位于NAT（或多重NAT后的客户端找出自己的公网地址，查出自己位于那种类型的NAT之后以及NAT为某一个本地端口所绑定的internet端口），这些信息被用来两个同时处于NAT路由器之后的主机之间建立UDP通信。目的就是找到外界连接内部地址所需的信息。

## 2.STUN协议架构

1.请求/响应（request/response）类型，由客户端给服务器发送请求，并等待服务端返回响应，用于确定一个NAT给客户端分配的具体绑定。客户端通过事务ID将请求响应连接起来。

2.指示类型（indication transaction），由服务器或者客户端发送指示，另一方不产生响应，用于保持绑定的激活状态。事务ID通常作为debugging aid使用。

所有的STUN报文信息都包含有一个固定头部，包含了方法，类和事务ID。方法表示是具体哪一种传输类型。STUN中只定义了一种方法，即binding（绑定），其他方法可以由使用者自行扩展；Binding方法可以用于请求/响应类型和指示类型。

STUN基于客户机-服务器协议。如一个VoIP电话或者软件可能会包含一个STUN客户端。这个客户端向STUN服务器发送请求，之后，服务器就会向STUN客户端报告NAT路由器的公网IP地址以及NAT为允许传入流量传回内网而开通的端口。同时还使得STUN客户端能够确定正在使用的NAT类型——因为不同的NAT类型处理传入的UDP分组的方式不同。

## 3.RFC STUN 规范

## RFC3489

STUN的全称是Simple Traversal of User Datagram Protocol (UDP) Through Network Address Translators (NATs)，即穿越NAT的简单UDP传输，是一个轻量级的协议，允许应用程序发现自己和公网之间的中间件类型，同时也能允许应用程序发现自己被NAT分配的公网IP。

它就是将STUN定义成简单的通过UDP进行NAT穿越的一套规范，也就是告诉你如何一步一步通过UDP进行穿越，但是这套规范在穿越的过程中还是存在很多问题，尤其是现在的网络

路由器对UDP的限制比较多，有的路由器甚至不允许进行UDP传输，所以这就导致了我们通过RFC3489这套规范进行NAT穿越的时候它的失败率会非常高。所以为了解决这个问题，又定义了另一套标准，RFC5389

## RFC5389

1.RFC5389中，STUN的全称为Session Traversal Utilities for NAT，即NAT环境下的会话传输工具，是一种处理NAT传输的协议，但主要作为一个工具来服务于其他协议。

和STUN/RFC3489类似，可以被终端用来发现其公网IP和端口，同时可以检测端点间的连接性，也可以作为一种保活（keep-alive）协议来维持NAT的绑定。

和RFC3489最大的不同点在于，STUN本身不再是一个完整的NAT传输解决方案，而是在NAT传输环境中作为一个辅助的解决方法，同时也增加了TCP的支持。

RFC5389废弃了RFC3489，因此后者通常称为classic STUN，但依旧是后向兼容的。而完整的NAT传输解决方案则使用STUN的工具性质， ICE就是一个基于 offer/answer方法的完整NAT传输方案，如 SIP。

2.RFC5389是在RFC3489的基础上又增加了一些功能，但是它对整个STUN的描述就不一样了, 它是把STUN描述成一系列穿越NAT的工具，所以都叫STUN，但是他们的含义完全就不一样了。RFC5389在UDP尝试可能失败的情况下，尝试使用TCP，也就是说RFC5389是包括UDP和TCP的两种协议进行NAT穿越的，这是两套规范最本质的区别。当然在协议的具体内容上，包括协议头还有协议体中的属性都有很多的变化，但是那些都不是最关键的，最关键的是RFC5389里面将TCP纳入进来。你可以通过TCP进行穿越。

这是STUN 的RFC的两套规范。大家一定要清楚，尤其是查文档的 时候一定要清楚查询的是哪一套规范。

## 4.STUN报文格式

###    包括20字节的STUN header

​    Body中可以有0个或多个attribute

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520163919775.png" alt="image-20230520163919775" style="zoom:33%;" />

1）最高的2位必须置零，这可以在当STUN和其他协议复用的时候，用来区分STUN包和其他数据包。

2）STUN Message Type 字段定义了消息的类型（请求/成功响应/失败响应/指示）和消息的主方法。 虽然我们有4个消息类别，但在STUN中只有两种类型的事务，即请求/响应类型和指示类型。响应类型分为成功和出错两种，用来帮助快速处理STUN信息。Message Type字段又可以进一步分解为如下结构：

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/a666070e36ace2897943ffa1bea3b24f.png?imageView2/2/w/1200)

其中显示的位为从最高有效位M11到最低有效位M0，M11到M0表示方法的12位编码。C1和C0两位表示类的编码。

因此：MessageType=MessageClass | MessageMethod

例如对于binding方法来说，

### MessageClass定义：

* 0b00表示request（请求），0b01表示indication（指示），

* 0b10表示success response（成功的响应），0b11表示error response（失败的响应）

Message Method定义为0x001

所以binding的MessageType的定义如下枚举值：

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520214056430.png" alt="image-20230520214056430" style="zoom:67%;" />

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520231051462.png" alt="image-20230520231051462" style="zoom:50%;" />

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/c42b57d2aacd7eef8d49bdb54a1049d7.png?imageView2/2/w/1200)

每一个method都有可能对应不同的传输类别。方法和类是正交的，因此对于每种方法，对该方法的请求，成功响应，错误响应和指示都是可能的。拓展定义新方法的时候注意要指定该方法允许哪些类型的消息。

### 根据RFC5766定义，音视频通讯经常使用的Stun Methods定义如下：

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/09a4ee4ee706083b146e3b2d7ce6663c.png?imageView2/2/w/1200)

3）Message Length 字段存储了信息的长度，以字节为单位，不包括20字节的STUN头部。由于所有的STUN属性都是都是4字节对齐（填充）的，因此这个字段最后两位应该恒等于零，这也是辨别STUN包的一个方法之一。

4）Magic Cookie 字段包含固定值0x2112A442，这是为了前向兼容RFC3489，因为在classic STUN中，这一区域是事务ID的一部分。另外选择固定数值也是为了[服务器](https://cloud.tencent.com/product/cvm?from=20065&from_column=20065)判断客户端是否能识别特定的属性。还有一个作用就是在协议多路复用时候也可以将其作为判断标志之一

5）Transaction ID 字段是个96位的标识符，用来区分不同的STUN传输事务。对于request/response传输，事务ID由客户端选择，服务器收到后以同样的事务ID返回response；对于indication则由发送方自行选择。事务ID的主要功能是把request和response联系起来，同时也在防止攻击方面有一定作用。服务端也把事务ID当作一个Key来识别不同的STUN客户端，因此必须格式化且随机在0~2^(96-1)之间。重发同样的request请求时可以重用相同的事务ID，但是客户端进行新的传输时，必须选择一个新的事务ID。

## 5、STUN报文属性（attributes）STUN Message Body

在STUN报文头部之后，

通常跟着0个或者多个属性，

每个属性必须是TLV编码的（Type-Length-Value）。其中Type字段和Length字段都是16位，Value字段为为32位表示，如下：

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/55f15e162b1370e0372d251b263ec8bc.png?imageView2/2/w/1200)

- attributes的Type定义如下：

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230520232155042.png" alt="image-20230520232155042" style="zoom: 67%;" />

- webrtc的代码定义：

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/eb844b9d94df32d719ffc5048b51943a.png?imageView2/2/w/1200)

- wireshark抓包示例：

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/e239f4c66ef8b6fa151cddc9a371c38d.png?imageView2/2/w/1200)

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/196215ec43d20bffe4803aa0a8cde8ff.png?imageView2/2/w/1200)

下面简要介绍几个常见属性的定义：

### 1）MAPPED-ADDRESS

MAPPED-ADDRESS用来表示NAT客户端的反射地址。

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/f43dcd0b452023793afce25d9f754556.png?imageView2/2/w/1200)

Family为IP类型，即IPV4(0x01)或IPV6(0x02)，Port为端口，Address为32位或128位的IP地址。注意Family前面的高8位必须全部置零，而且接收端必须要将其忽略掉。

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/5b156d1bd5f3db6f8b4b89623900753f.png?imageView2/2/w/1200)

### 2）XOR-MAPPED-ADDRESS

和MAPPED-ADDRESS基本相同，不同点是反射地址部分经过了一次异或（XOR）处理。

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/fbf0890f2a4b4b7351fea1f2fb39daac.png?imageView2/2/w/1200)

对于X-Port字段，是将NAT的映射端口以小端形式与magic cookie的高16位进行异或，再将结果转换成大端形式而得到的，X-Address也是类似。

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/e26734236fab825721534c8524508dd6.png?imageView2/2/w/1200)

之所以要经过这么一次转换，是因为在实践中发现很多NAT会修改payload中自身[公网IP](https://cloud.tencent.com/product/eip?from=20065&from_column=20065)的32位数据，从而导致NAT打洞失败。

### 3）ERROR-CODE

该属性用于error response报文中。包含了300-699表示的错误代码，以及一个UTF-8格式的文字出错信息（Reason Phrase）。

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/fa2892144763ecb59ea677107c0f5241.png?imageView2/2/w/1200)

其中返回码定义：

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/86241fcdaad1746a9389d07562bf7e22.png?imageView2/2/w/1200)

300：尝试代替(Try Alternate)，客户端应该使用该请求联系一个代替的服务器。这个错误响应仅在请求包括一个 USERNAME属性和一个有效的MESSAGE-INTEGRITY属性时发送；否则它不会被发送，而是发送错误代码为400的错误响应； 400：错误请求(Bad Request)，请求变形了，客户端在修改先前的尝试前不应该重试该请求。 401：未授权(Unauthorized)，请求未包括正确的资格来继续。客户端应该采用一个合适的资格来重试该请求。 420：未知属性(Unknown Attribute)，服务器收到一个STUN包包含一个强制理解的属性但是它不会理解。服务器必须将不认识的属性放在错误响应的UNKNOWN-ATTRIBUTE属性中。 438：过期Nonce(Stale Nonce)，客户端使用的Nonce不再有效，应该使用响应中提供的Nonce来重试。 500：服务器错误(Server Error)，服务器遇到临时错误，客户端应该再次尝试。

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/50ef2b84640f025463d5776df2f23f3b.png?imageView2/2/w/1200)

其余的还有：

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/9aaef2898f02d4881d5b2071721fe2f5.png?imageView2/2/w/1200)

![img](https://ask.qcloudimg.com/http-save/yehe-8223537/28c467ebf480e07edd26197a23587252.png?imageView2/2/w/1200)

可以翻阅rfc5389协议15小节、RFC5766定义。

# ICE（穿越NAT/防火墙方案的实现）

ICE,Interactive Connectivity Establishemnt

需要两端进行交互才能创建连接

每个Candidate是一个地址：每个候选者包括：协议,IP,端口，类型

## Candidate类型

主机候选者：是本地使用的 IP 地址和端口，例如通过 ifconfig/ipconfig 查看到 WLAN 网卡的 IP 地址是 192.168.0.105，并且准备使用 51417 端口。

反射候选者：是 NAT 映射后使用的 IP 地址和端口。

中继候选者： 是 TURN 服务器开辟的 IP 地址和端口，TURN 服务器是用来应对当通信对端 NAT 穿越失败无法建立 P2P 通信时，通过 TURN 服务器转发数据包，TURN 服务器一般都部署在带公网 IP 的机器上。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230521004011458.png" alt="image-20230521004011458" style="zoom:50%;" />

## 收集Candidate

Host Candidate：本机所有IP和指定端口

Reflexive Candidate ：  STUN/TURN

Relay  Candidate ：TURN（TURN依赖于STUN）

## ICE具体做什么

收集 Candidate 

对 Candidate  Pair 排序

- 第一种通路，本机IP，如果两个终端在一个服务下，直接通过本机IP连通
- 第二种通路，穿越NAT，双方终端访问STUN服务，都拿到外网IP，尝试P2P穿越，成功既可以连接
- 第三条通路，P2P不成功，通过中继服务器Relay server

连通性检查

# DTLS协议（数据安全传输）

## 非对称加密

非对称密钥加密，又称公开密钥加密（Public-Key Encryption），加密和解密使用不同的密钥。公开密钥所有人都可以获得，通信发送方获得接收方的公开密钥之后，就可以使用公开密钥进行加密，接收方 收到通信内容后使用私有密钥解密。非对称密钥除了用来加密，还可以用来进行签名。因为私有密钥无法被其他人获取，因此通信发送方使用其私有密钥进行签名，通信接收方使用发送方的公开密钥对签名进行解密，就能判断这个签名是否正确。

公钥上锁私钥可以打开，公钥上锁公钥打不开

私钥上锁公钥可以打开 ，私钥上锁私钥打不开

优点：可以更安全地将公开密钥传输给通信发送方；  

缺点：运算速度慢。 

## 数字签名

### 数字签名的原理：

数字签名是基于[非对称密钥](https://www.zhihu.com/search?q=非对称密钥&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra={"sourceType"%3A"answer"%2C"sourceId"%3A2378587006})加密技术与数字摘要技术的应用，是一个包含电子文件信息以及发送者身份，并能够鉴别发送者身份以及发送信息是否被篡改的一段数字串。一段数字签名数字串，包含了电子文件经过[Hash编码](https://www.zhihu.com/search?q=Hash编码&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra={"sourceType"%3A"answer"%2C"sourceId"%3A2378587006})后产生的数字摘要，**即一个Hash函数值以及发送者的公钥和私钥三部分内容**。发送方通过[私钥加密](https://www.zhihu.com/search?q=私钥加密&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra={"sourceType"%3A"answer"%2C"sourceId"%3A2378587006})后发送给接收方，接收方使用[公钥解密](https://www.zhihu.com/search?q=公钥解密&search_source=Entity&hybrid_search_source=Entity&hybrid_search_extra={"sourceType"%3A"answer"%2C"sourceId"%3A2378587006})，通过对比解密后的Hash函数值确定数据电文是否被篡改。

## 证书

通过使用 证书 来对通信方进行认证。  数字证书认证机构（CA，Certificate Authority）是客户端与服务器双方都可信赖的第三方机构。  服务器的运营人员向 CA 提出公开密钥的申请，CA 在判明提出申请者的身份之后，会对已申请的公 开密钥做数字签名，然后分配这个已签名的公开密钥，并将该公开密钥放入公开密钥证书后绑定在一 起。  进行 HTTPS 通信时，服务器会把证书发送给客户端。客户端取得其中的公开密钥之后，先使用数字 签名进行验证，如果验证通过，就可以开始通信了。

## 私有证书 

也可以制作私有证书HTTPS不认私有证书，自己的程序可以用。通过openssl制作![image-20230521012359160](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230521012359160.png)

## 常用加密算法

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230521012656763.png" alt="image-20230521012656763" style="zoom:67%;" />

## DTLS协议详解

TLS是基于TCP协议

DTLS是基于UDP协议

## DTLS握手协议

DTLS协议在UDP提供的socket之上实现了客户机与服务器双方的握手连接，并且在握手过程中通过使用PSK或ECC实现了加密，并且利用cookie验证机制和证书实现了通信双方的身份认证，并且用在报文段头部加上序号，缓存乱序到达的报文段和重传机制实现了可靠传送。

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230521201321226.png" alt="image-20230521201321226"  />

握手过程如上图所示，大体来说分成三个过程：明文通信过程、非对成加密通信过程、对称加密通信过程；

**明文通信过程**：在通信两端首次向对方发送 Hello 消息时，由于双方都没有协商好要使用哪种加密方式，因此这个过程中的消息都是使用明文进行发送的。

a. Client Hello：客户端首先向服务端发起握手，在握手消息中告诉对方自己支持的 SSL/TLS 版本、加密套件（包括非对称加密时使用的算法与、非对称加密时使用的算法、产生密钥的伪随机函数 PRF）与数据压缩算法（TLS1.3之后就已经没有这个字段）等；还会携带一个 Session ID，因为握手流程的开销比较大，使用 Session ID 可以在下一次与 TLS 握手的过程跳过后续繁琐的握手流程，重用之前的握手结果（如版本号、加密算法套件、master-key 等）；并产生一个随机数 A，也告诉给对方；

b. Server Hello：服务端响应一个 Server Hello 消息，携带协商出来的 TLS/SSL 版本号、加密套件和数据压缩算法，如果服务端同意客户端重用上次的会话，就返回一个相同的 Session ID，否则就填入一个全新的 Session ID；

c. Server Certificate（可选）：**携带服务端数字证书（CA）以验证服务端身份，里面携带了服务端非对称加密所使用的公钥；这步虽然是可选的，但是一般来说客户端都会要求验证服务端的身份，在大多数情况下这步都会执行；**

d. Server Key Exchange（可选）：在使用某些非对称加密算法（例如 DH 算法）的情况下，Server Certificate 里的信息是不足够的，或者 Server Certificate 在某些通信过程中直接被省略了（没有验证服务端身份），需要 Server Key Exchange 里的额外信息来帮助客户端生成 pre-master key；

e. Client Sertificate Request（可选）：**在有些安全性要求高的场景，例如银行支付等，不仅需要验证服务端的身份，还需要验证客户端的身份，这时候服务端就会要求客户端提供客户端的身份证书；**

f. Server Hello Done：表明 Server Hello 结束；

g. Client Certificate（可选）：如果服务端要求客户端提供数字证书以验证身份，则客户端发送自己的身份证书给服务端；

**非对称加密通信过程**：**由于非对称加密通信的性能较差，在实际的通信过程中其实使用的是对称加密通信，为了保证对称加密通信过程的安全性，也就是需要避免对称加密密钥被窃取，这个密钥在协商过程中使用非对称加密来进行加密。**

a. Client Key Exchange：客户端在验证服务端的身份证书后，会取出其中的服务端公钥，产生一个随机数 C，作为 pre-master key，在本地使用之前的随机数 A、B 和这次生成的 C 共同生成对称加密密钥 master-key；使用服务端公钥对 pre-master key 加密后发送给服务端；

b. Certificate Verify（可选）：如果服务端要求客户端提供客户端证书，那么客户端在发送 Client Key Exchange 之后必须马上发送 Certificate Verify，其中的内容是客户端使用自己的私钥加密的一段数据，提供给服务端用客户端的公钥来进行解密验证。之所以需要这一步是为了确保客户端发送的证书确实是它自己的证书；

c. Client Change Cipher Spec：提示服务端随后使用 master key 来进行对称加密通信；

d. Client Handshake Finished: 表明客户端侧 SSL/TLS 握手结束；

e. Server Change Cipher Spec：提示客户端随后使用 master key 来进行对称加密通信；

f. Server Handshake Finished：表明服务端侧 SSL/TLS 握手结束；

**对称加密通信过程**：通过上述握手过程协商出对称加密算法及使用的对称加密密钥之后，随后的通信过程，也就是实际的应用通信过程，都使用的是对称加密。

### **DTLS时序图：**

<img src="https://pic3.zhimg.com/80/v2-78dcdb8861ff10c5630027425f8e4342_1440w.webp" alt="img" style="zoom:80%;" />

**1.SDP交换，媒体协商。SDP内容十分重要，交换了ICE需要的Username，Password，以及后面的DTLS需要的证书的验证指纹，用来验证证书是否被中间人替换。部分SDP内容如下：**

<img src="https://pic2.zhimg.com/80/v2-11f64045dbaef021601a79d4a1683e89_1440w.webp" alt="img" style="zoom: 67%;" />

### **a=fingerprint也就是指纹，那指纹是用来干什么的呢？**

**指纹就是用来我们进行数据加密的时候，来验证这个证书的**。那它首先通过信令层将SDP中的**证书的指纹**下发给对方，那么下次对数据加密前的它进行一下数据证书的交换，交换证书是通过DPLS进行，那么通过DPLS进行证书交换的时候，**通过这个指纹去验证你这个证书的有效性**，那如果这个证书验证是有效性的，然后后面你才能进行数据加密然后进行传输。如果通过指纹这个证书不匹配，那说明你这个连接也是有问题的。那这个时候就不能进行传输。通过以上这个种种方式呢，**在打通的时候进行一次验证**，**在传数据的时候在交换证书的时候也要进行验证**，那么通过这个层层的安全的验证，才能保证整个webRTC传输的安全性。以上就是安全性相关的一些描述。当然**最后进行算法加密的时候你可以使用这个a=crypto指定的加密算法，也可以通过DPLS交换的证书里的指定的加密算法进行加密**。

### **2.STUN/TURN服务，获取Candidate（TURN服务需要username与password），验证连通性，获取Candidate对**

### **3.DTLS握手**

### **4.传输加密后的数据**

## DTLS   SRTP

**SRTP：基于DTLS获取了对称加密所需要的密钥和对应的加密算法(默认使用长度128AES)**

**使用在SRTP中，以此来对RTP数据进行加密保证安全和完整性**

![img](https://pic2.zhimg.com/80/v2-0e260ae8d896d178d67f886fed206bb1_1440w.webp)

**头部与普通RTP头一样，头部不进行加密，只有数据进行加密（黄色部分），最后的部分：**

**SRTP MKI：主键标识符（可选），webrtc不使用，为0**

**Authentication Tag：完整性验证，RTP头与加密数据进行做一次hash函数运算，得到摘要。对端通过同样方式进行验证，用来保证完整性**

## **五）libsrtp：开源库，可以解决大数据吞吐**

<img src="https://pic1.zhimg.com/80/v2-ca2615bfb3cdeb5249b7867d0e0028c8_1440w.webp" alt="img" style="zoom: 80%;" />

**创建Session：是在DTLS握手之后，获取了双方信息，比如协商后的加密算法、密钥...，之后可以进行加解密。**

**另外，session包含两个，分别对于输出流和输入流，不能共用。**

# OpenSSL

SSL： Secure Sockets Layer

TLS： Transport Layer Security

## TLS协议

 **TLS握手协议：**是一种实现身份鉴别和安全参数协商的协议。客户端和服务器端通过TLS记录协议传输数据前，需要通过TLS握手协议完成双向身份鉴别过程，并约定压缩算法、加密算法、消息鉴别MAC算法、加密密钥、MAC密钥等安全参数通信双方确定新的安全参数后，通过TLS改变密码规范协议通知对方开始使用新约定的安全参数。

 **报警协议：**用于传输出错消息，如解密失败、无法确认证书等

 **通信双方第一次启动握手协议时，初始安全参数为不压缩、不加密、不计算MAC**

**TLS记录协议：**使用[握手](https://learn.microsoft.com/zh-cn/windows/win32/secauthn/tls-handshake-protocol)期间创建的密钥保护应用程序数据。 记录协议负责保护应用程序数据并验证其 [*完整性*](https://learn.microsoft.com/zh-cn/windows/win32/secgloss/i-gly) 和来源。 它管理以下内容：

- 将传出消息划分为可管理块，并重新组合传入消息。
- 压缩传出块并解压缩传入块 (可选) 。
- 将 [*消息身份验证代码*](https://learn.microsoft.com/zh-cn/windows/win32/secgloss/m-gly) (MAC) 应用于传出消息，并使用 MAC 验证传入消息。
- 加密传出消息并解密传入消息。

记录协议完成后，传出加密数据将传递到传输控制协议 (TCP) 层进行传输。

### 

## OpenSSL原理

SSL_CTX

SSL:代表一个SSL连接

SSL_Write/SSL_Read



使用步骤<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230521014011375.png" alt="image-20230521014011375" style="zoom:67%;" />

# SDP协议

SDP（Session Description Protocol）他只是一种信息格式的描述标准，本身不属于传输协议，但是可以被其他传输协议用来交换必要信息。（json/xml）

## SDP规范

多个媒体级描述

一个会话级描述

有多个<type>=<value>组成

## 会话层

会话名称和目的

会话存活时间

会话中包括多个媒体信息

## 媒体层

媒体格式

传输协议

传输IP和端口

媒体负载类型

SDP描述信息

## SDP描述信息

### **Session Description**

v=（protocol version）

* Version必选 v=0 SDP的版本号，不包括次版本号

o=（owner/create and session identifier）

* 必选  origion/owner
* o=<username><session id><version><network type><adress type><adress> 
* 例： o=  -   7017624586836067756 2 IN IP4 127.0.0。1

s=（session name）

* Session name必选 s=<session name>会话名，s= - 表示忽略会话名

c=*（conn info - optional if included at session-level）

* Connetion可选  c=<network type><adress type><connection address>
* c= IN IP4 0.0.0.0

t=（time the session is active）

a=*（zero or more session attribute lines）

* Attributes可选
* a=<TYPE>或a=<TYPE>:<VALUES>
* 例：a=framerate：<帧速率>
* rtpmap可选
* a=rtpmap：<fmt/payload type><encoding name>/<clock rate>[/<encodingparameters>]
* 例：a=rtpmap:103 ISAC/16000
* fmtp可选
* a=fmtp:<format/payload type> parameters
* 例： a=fmtp:103  apt=106

##      Media Description

m=（media name and transport address）

* Media重要必选
* m=<media><port><transport><fmt/payload type list>
* 例：m=audio 1024 UDP/TLS/RTP/SAVPF 111 103 104 9 0 8 106 105 13 126

c=*（conn info - optional if included at session-level）

b*（bandwidth information）

a=*（zero or more session attribute lines）

# 媒体协商

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230522010851976.png" alt="image-20230522010851976" style="zoom:50%;" />

媒体协商方法：

createOffer

createAnswer

setLocalDescription

setRemoteDescription

# WebRTC中的SDP

<img src="C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\image-20230523000728928.png" alt="image-20230523000728928" style="zoom: 80%;" />

# 多人音视频架构

## Mesh架构



## MCU架构



## SFU架构

