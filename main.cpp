#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
using namespace std;

#define BACKLOG 10 

struct Message {

};

class server {
private:
    struct addrinfo hints{}, *res, *p;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int status;
    int sockfd, new_fd;

    void initiate(char* port) {
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        status = getaddrinfo(NULL, port, &hints, &res);
        if(status != 0) {
            cout << "Error: getting getaddrinfo main: " << gai_strerror(status);
            return;
        }

        for(p = res; p != nullptr; p = p->ai_next){
            sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if(sockfd == -1){
                continue;
            }
            if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                closesocket(sockfd);
                continue;}
            break;
        }
        freeaddrinfo(res);
        listenAndAccept();
    }

    void listenAndAccept(){
        int lten = listen(sockfd, BACKLOG);
        if(lten == -1) return;

        addr_size = sizeof their_addr;

        while(true){
            new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
            if (new_fd == -1){
                continue;
            }
        }
    }

public:
    server(char* port){
        initiate(port);
    }
    
};

class client {
private:
    struct addrinfo hints{}, *res, *p;
    int status, sockfd;

    void initiate(char* node, char* port){
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        status = getaddrinfo(node, port, &hints, &res);
        if(status != 0) {
            cout << "Error: getting getaddrinfo main: " << gai_strerror(status);
            return;
        }

        for(p = res; p != nullptr; p = p->ai_next){
            sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if(sockfd == -1){
                continue;
            }
            if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                closesocket(sockfd);
                continue;}
            break;
        }

        if(p == nullptr){
            cout << "client failed to connect" << endl;
            return;
        }

        cout << "client connected" << endl;
        freeaddrinfo(res);
    }

public:
    client(char* node, char* port){
        initiate(node, port);
    }
};


int main(int argc, char* argv[]) {
    WORD wVersionRequest = MAKEWORD(2,2);
    WSADATA wsaData;
    if (WSAStartup(wVersionRequest, &wsaData) != 0) {
        cout << "WSAStartup failed\n";
        return 1;
    }

    if (argc != 5) {
        cout << "Usage: program server|client <host> <port>\n";
        WSACleanup();
        return 1;
    }

    char* mode = argv[2];
    char* host = argv[3];
    char* port = argv[4];

    cout << mode << " "<< host << " " << port << endl;


    if (strcmp(mode, "server") == 0) {
        server s(port);
    } else if (strcmp(mode, "client") == 0) {
        client c(host, port);
    } else {
        cout << "Unknown mode\n";
    }

    WSACleanup();
    return 0;
}

