#if defined(__linux__)
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<arpa/inet.h>

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

void Connect(int sockfd, const struct sockaddr* addr, socklen_t addrlen){
    int res = connect(sockfd, addr, addrlen);
    if(res == -1){
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
}

bool Write(int fd, void* buf, size_t count){
    int res = write(fd, buf, count);
    if(res == -1){
        perror("write failed"); 
        exit(EXIT_FAILURE);  
    }
    else if(strncmp(static_cast<const char*>(buf), "end", 3)){
        write(fd, buf, count);
        std::cout << "Client Exit";
        return true;
    }
    if(res >= 0){ 
        std::cout << "Data send to the server successfully!\n";
    }
    std::memset(buf, '\0', BUFER);
    return false;
}

void Read(int fd, void* buf, size_t count){
    size_t nread = read(fd, buf, count); 
    if(nread == -1){
        perror("read failed"); 
        exit(EXIT_FAILURE); 
    }
    else if(nread == 0){
        std::cout << "END OF FILE occured\n";
    }
    std::cout << "Data received from server: " << buf << '\n';
    std::memset(buf, '\0', BUFER);
}

int main(){
    int client = Socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in client_adr{0};
    client_adr.sin_family = AF_INET; 
    client_adr.sin_port = htons(PORT);
    client_adr.sin_addr.s_addr = inet_addr("127.0.0.1");

    Connect(client, (struct sockaddr*)&client_adr, sizeof(client_adr));

    char message[BUFER]{'\0'};
    while(true){
        std::cout << "Enter the message you want to send to the server: ";
        std::cin >> message;
        if(Write(client, message, sizeof(message))){
            break;
        }    
        Read(client, message, sizeof(message));    
    }

    close(client);

    return 0;
}
