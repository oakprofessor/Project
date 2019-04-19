#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;

bool check_if_string_end(std::string a,std::string b){
    int a_len = a.size();
    int b_len = b.size();
    if (a_len >= b_len) {
        for(size_t i = 1; i <= b_len; i++)
        {
            char _a = a.at(a_len-i);
            char _b = a.at(b_len-i);
            if (_a != _b) 
            {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

//Client side
int main(int argc, char *argv[])
{
    //grab the IP address and port number 
    std::string ip = "127.0.0.1";
    const char *serverIp = ip.c_str();;
    int port = 12345; 
    //create a message buffer 
    char msg[1500]; 
    memset(&msg, 0, sizeof(msg));
    //setup a socket and connection tools 
    struct hostent* host = gethostbyname(serverIp); 
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);
    //try to connect...
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        std::cout<<"Error connecting to socket!"<<endl;
        return 0;
    }
    std::cout << "Connected to the server!" << endl;
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);

    // std::cout<<"Hello";

    bool register_status = true;
    std::string connect = "Welcome to Question! \nPlease enter your nickname:";
    std::string again = "Please enter a different nickname:";
    std::string start = "Type you answer below:";
    bool ingame = false;

    while(1)
    {
        std::string message = msg;
        // std::cout<<"Hello";
        if (message == "exit")
        {
            std::cout << "Server has quit the session" << endl;
            break;
        }

        // std::cout<<"Hello";
        if ( message == again || message == connect) 
        {
            register_status = false;
        }

        // std::cout<<"Hello";
        if ( check_if_string_end(message,start) ) 
        {
            ingame = true;
        }
        
        if (msg[0] != 0) {
            std::cout << "Server: " << msg << std::endl;
        }
        
        if (!register_status) 
        {
            memset(&msg, 0, sizeof(msg));
            std:: cout << ">";
            string data;
            getline(cin, data);
            strcpy(msg, data.c_str());
            bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
            register_status = true;
        }
        
        if (ingame) {
            memset(&msg, 0, sizeof(msg));
            std::cout << ">";
            string data;
            getline(cin, data);
            strcpy(msg, data.c_str());
            bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
            ingame = false;
        }
        
        memset(&msg, 0, sizeof(msg));
        //receive message from server
        bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);
    }
    gettimeofday(&end1, NULL);
    close(clientSd);
    std::cout << "********Session********" << endl;
    std::cout << "Bytes written: " << bytesWritten << 
    " Bytes read: " << bytesRead << endl;
    std::cout << "Elapsed time: " << (end1.tv_sec- start1.tv_sec) 
      << " secs" << endl;
    std::cout << "Connection closed" << endl;
    return 0;    
}
