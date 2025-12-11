#include <iostream> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sstream>

using namespace std;

int main( ) {
    cout << "Server is starting..." << endl;
    // 1. 创建socket
    //  a.成功时：返回一个非负整数 ———— 一个文件描述符（fd），代表新创建的 socket。
    //    这个 fd 可以用于 bind/connect/listen/accept/send/recv/close 等系统调用。
    //  b.失败时：返回 -1，并设置全局 errno（可用 perror 或 strerror(errno) 查看原因）。
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        cerr << "Error creating socket : " << strerror(errno) << endl;
        return 1;
    }
    cout << "Creating socket success, server_sock : " << server_sock << endl;

    // 2. 为服务端设置 IPv4 地址和端口号。
    sockaddr_in server_addr = {};                // IPv4 地址/端口，
    server_addr.sin_family = AF_INET;                   // 指定地址族为 IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;           // 监听所有网络接口的请求
    server_addr.sin_port = htons(11451);                // 主机字节序的端口号 9999 转为网络字节

    // 3. 绑定socket和服务端口号
    // 在bind之前设置 SO_REUSEADDR，避免快速重启时报 "Address already in use"
    int opt = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        cerr << "Error setting SO_REUSEADDR: " << strerror(errno) << endl;
        close(server_sock);
        return 1;
    }

    // 绑定socket和服务端口号
    int bind_retval = bind(server_sock, (sockaddr*)&server_addr, sizeof(server_addr));
    if(bind_retval < 0) {
        cerr << "Error binding socket : " << strerror(errno) << endl;
        close(server_sock);
        return 1;
    }
    cout << "Socket bound successfully. " << endl;

    // 4. 监听socket
    // 第二个参数 5 指定了等待队列（Backlog Queue）的大小。
    // 当有新的连接请求到达时，如果服务器正在忙于处理其他连接（例如，正在执行 accept() 操作）
    // 这些新的请求会被放入一个等待队列。这个参数 5 告诉操作系统，最多可以有多少个等待被接受
    // （已建立连接但尚未被 accept() 处理）的连接请求排在这个队列里。
    if(listen(server_sock, 5) < 0) {
        cerr << "Error listening on socket: " << strerror(errno) << endl;
        close(server_sock);
        return 1;
    }
    cout << "Server is listening on port 11451 ..." << endl;

    // 5. 接收来自客户端的连接请求，并把其网络地址信息存放在client_addr结构体中
    sockaddr_in client_addr = {};
    socklen_t client_addr_len = sizeof(client_addr);
    cout << "Waiting for connection..." << endl;        // 添加提示，表示已执行到 accept()
    int client_sock = accept(server_sock, (sockaddr*)&client_addr, &client_addr_len);
    if (client_sock < 0) { 
        cerr << "Error accepting connection: " << strerror(errno) << endl;
        close(server_sock);
        return 1;
    }

    // 连接成功，打印客户端IP地址和端口
    cout << "Connection accepted from " << inet_ntoa(client_addr.sin_addr)
         << ":" << ntohs(client_addr.sin_port) << endl;
    cout << "client_sock : " << client_sock << endl;

    // 6. 从socket中读取字符
    char buffer[1024] = {0};
    stringstream ss;
    cout << "Waiting to receive data..." << endl;
    // 返回值是实际的字节数数量
    // para1 客户端的fd
    // para2 用于接收字符的缓冲区
    // para3 缓冲区大小
    // para4 0 表示使用默认行为（通常是阻塞模式，等待数据到达）。
    int real_read_size = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if(real_read_size < 0) {
        cerr << "Error receiving data: " << strerror(errno) << endl;
    } else if (real_read_size == 0) {
        cout << "Client closed connection without sending data." << endl;
    } else {
        buffer[real_read_size] = '\0';      // 确保以 null 结尾，将其视为 C 字符串
        ss << buffer;
        cout << "Received message : " << ss.str() << "total " << real_read_size << " Bytes"<< endl;
    }

    // 7. 给客户端回传消息
    string send_msg = "Hello client, I have receive u message";
    write(client_sock, send_msg.c_str(), send_msg.length());

    // 8. 关闭socket：先关闭与客户端的连接，再关闭监听 socket
    close(client_sock);
    close(server_sock);
    cout << "Closing server !" << endl;

    return 0;
}
