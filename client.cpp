#include <iostream> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <sstream>

// ../bin/client 119.3.70.188 80 测试联通b站

using namespace std;

inline string GetIPAddrStr(char* argv[]) {
    return string(argv[1]);
}

inline string GetPortStr(char* argv[]) {
    return string(argv[2]);
}

inline bool Check3Para(int argc) {
    if(argc != 3) {
        return false;       // 非法
    }
    return true;            // 合法
}

// helper to prefix cout messages
inline ostream& cout_info() {
    cout << "[COUT INFO] ";
    return cout;
}

int main(int argc, char* argv[]) {
    // 期望有 3 个参数：程序名、服务器IP地址、端口号
    if (! Check3Para(argc)) {
        cerr << "Usage: " << argv[0] << " <Server_IPv4_Address> <Server_Port_Number>" << endl;
        cerr << "Example: " << argv[0] << " 127.0.0.1 8080" << endl;
        return 1; // 参数错误，退出
    }

    // 从命令行参数获取服务器 IP 地址和端口号
    string server_ip_str = GetIPAddrStr(argv);
    int server_port = atoi(GetPortStr(argv).c_str());   // 将字符串端口号转换为整数
    if (server_port <= 0 || server_port > 65535) {
        cerr << "Invalid port number: " << server_port << ". Port must be between 1 and 65535." << endl;
        return 1;
    }
    cout_info() << "Client is starting..." << endl;
    cout_info() << "Attempting to connect to: " << server_ip_str << ":" << server_port << endl;

    // 1. 创建socket
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock < 0) {
        cerr << "Error creating socket : " << strerror(errno) << endl;
        return 1;
    }
    cout_info() << "Creating socket success, client_sock : " << client_sock << endl;

    // 2. 为客户端设置将要访问的 IPv4 地址和端口号
    sockaddr_in server_addr = {};                
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);         // 与 server.cpp 使用相同端口
    // 将ipv4地址127.0.0.1转换为网络字节序存入server_addr.sin_addr，是client要访问的ip地址
    if (inet_pton(AF_INET, server_ip_str.c_str(), &server_addr.sin_addr) <= 0) {
        cerr << "Invalid address / Address not supported" << endl;
        close(client_sock);
        return 1;
    }

    // 3. 连接到服务器
    if (connect(client_sock, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error connecting to server: " << strerror(errno) << endl;
        close(client_sock);
        return 1;
    }
    cout_info() << "Connected to server " << server_ip_str << ": " << server_port << endl;

    // 4. 发送数据到服务器
    string send_msg = "Hello server, this is client !";
    ssize_t sent = send(client_sock, send_msg.c_str(), send_msg.length(), 0);
    if (sent < 0) {
        cerr << "Error sending data: " << strerror(errno) << endl;
        close(client_sock);
        return 1;
    }
    cout_info() << "Sent message (" << sent << " bytes) to server." << endl;

    // 5. 接收服务器回传的数据
    char buffer[1024] = {0};
    cout_info() << "Waiting to receive data from server..." << endl;
    ssize_t recvd = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (recvd < 0) {
        cerr << "Error receiving data: " << strerror(errno) << endl;
    } else if (recvd == 0) {
        cout << "Server closed connection without sending data." << endl;
    } else {
        buffer[recvd] = '\0';
        cout_info() << "Received from server: " << buffer;
        cout_info() << "Total " << recvd << " Bytes" << endl;
    }

    // 6. 关闭 socket
    close(client_sock);
    cout_info() << "Closing client !" << endl;

    return 0;
}