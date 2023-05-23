#if defined (__linux__)
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<cstring>

#elif defined(_WIN32)
#include<winsock2.h>

#elif defined(_WIN64)
#include<winsock2.h>

#endif
#define PORT 55555
#define BUFER 1024

int Socket(int domain, int type, int protocol){
    int res = socket(AF_INET, SOCK_STREAM, 0);
    if(res == -1){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen){
    int res = bind(sockfd, addr, addrlen);
    if(res == -1){
        perror("bins failed");
        exit(EXIT_FAILURE);
    }
}

void Listen(int sockfd, int backlog){
    int res = listen(sockfd, backlog);
    if(res == -1){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t* addrlen){
    int res = accept(sockfd, addr, addrlen);
    if(res == -1){
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    return res;
}

bool Read(int fd, void* buf, size_t count){
    size_t nread = read(fd, buf, count);
    if(nread == -1){
        perror("read failed");
        exit(EXIT_FAILURE);
    }
    else if(nread == 0){
        std::cout << "END OF FILE occured\n";
    }
    else if(std::strncmp("end",static_cast<const char*>(buf), 3) == 0){
        std::cout << "Client exited\n";
        std::cout << "Server is exiting...\n";
        return false;
    }
    std::cout << "Data received from client: " << buf << '\n';
    std::memset(buf, '\0', count);
    return true;
}

void Write(int fd, void* buf, size_t count){
    int res = write(fd, buf, count);
    if(res == -1){
        perror("write failed");
        exit(EXIT_FAILURE);
    }
    if(res >= 0){
        std::cout << "Data successfully sent to the client!\n";
    }
    std::memset(buf, '\0', BUFER);
}

int main(){
    int server = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_adr{0};
    server_adr.sin_family = AF_INET;
    server_adr.sin_port = htons(PORT);
    server_adr.sin_addr.s_addr = htonl(INADDR_ANY);

    Bind(server, (struct sockaddr*)&server_adr, sizeof(server_adr));
    Listen(server, 10);

    socklen_t adrlen = sizeof(server_adr);
    int fd = Accept(server, (struct sockaddr*)&server_adr, &adrlen);
    char message[BUFER]{'\0'};

    while(Read(fd, message, BUFER)){
        std::cout << "Enter the message you want to send to the client: ";
        std::cin >> message;
        Write(fd, message, sizeof(message));
    }

    close(server);
    return 0;
}
