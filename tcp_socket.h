#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

#include <iostream> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sstream>

class TcpClient {
private:
    int m_clientfd;             // client socket 句柄
    std::string m_ip;           // 服务端 IP/域名
    uint16_t m_port;            // 服务端端口 1~65535

public:
    TcpClient();
    TcpClient(const TcpClient&) = delete;
    ~TcpClient();

    // connection 
    bool connect_to_server(const std::string& ip, uint16_t port);
    void close_connection();

    // send and recv
    ssize_t send_data(const void* buf, size_t len);     // 全力发送len长度的字符串
    ssize_t recv_data(void* buf, size_t len);           
};

class TcpServer {
private:
    int m_listenfd;             // 监听的socket句柄
    int m_clientfd;             // 客户端连接上来的socket
    std::string m_clientip;     // 客户端ip
    unsigned short m_port;            // 客户端端口
public:
    TcpServer() : m_listenfd(-1), m_clientfd(-1), m_clientip(""), m_port(0) {}
    bool initserver(const unsigned short in_port);
};

#endif