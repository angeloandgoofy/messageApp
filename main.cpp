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
    string username;
    char* message;
};

class server {
private:
    struct addrinfo hints{}, *res, *p;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int status;
    int sockfd, new_fd;
    char* buffer;
    int len;

    void initiate(char* node, char* port) {
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
            if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                closesocket(sockfd);
                continue;}

           
            break;
        }

        freeaddrinfo(res);
        listenAndAccept();
    }

    void listenAndAccept(){
        if(sockfd == -1) {
            cout << "unable to create socket " << endl;
            return;
        }
        
        int lten = listen(sockfd, BACKLOG);
        if(lten == -1) {
            cout << "Listening could connect" << endl;
            return;}
            
            
        addr_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        if (new_fd == -1){
            cout << "Error" << endl;
            perror("accpet");
            return;
        }
        cout << "Hello1" << endl;
        cout << "server accepting connections " << endl;
        char buf[1024];

        while (true) {
            int bytes = recv(new_fd, buf, sizeof(buf) - 1, 0);
            if (bytes <= 0) {
                break; 
            }

            buf[bytes] = '\0';
            cout << "Client says: " << buf << endl;

            send(new_fd, buf, bytes, 0); 
        }
        
    }

public:
    server(char* node, char* port){
        initiate(node, port);
    }
    
};

class client {
private:
    struct addrinfo hints{}, *res, *p;
    int status, sockfd;
    Message msg;

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
        sendRecv();
        freeaddrinfo(res);
    }

    void sendRecv(){
        char buf[1024];
        cout << "Enter username: ";
        getline (cin, msg.username);
        cout << "Enter username: ";
        getline (cin, msg.username);
        while(true){
            cout <<  msg.username << ": ";
            msg.message = new char[1000];
            cin.getline (msg.message, 1000);
            if (strcmp(msg.message,"quit") == 0){break;}
            send(sockfd, msg.message, strlen(msg.message), 0);

            delete[] msg.message;
        }
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
    char* ip = argv[3];
    char* port = argv[4];

    if(strcmp(mode, "server") == 0){
        server server(ip, port);
    }else if(strcmp(mode, "client") == 0){
        client client(ip, port);
    }else{
        cout << "Enter valid information" << endl;
    }

    WSACleanup();
    return 0;
}

