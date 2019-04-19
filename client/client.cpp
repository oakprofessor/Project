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
#include <atomic>
using namespace std;
//Client side
int main(int argc, char *argv[])
{
    //grab the IP address and port number 
    char *serverIp = "127.0.0.1";
    int port = 12345; 
    //create a message buffer 
    char msg[1500]; 
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
        return 1;

    }
    std::cout << "Connected to the server!" << endl;
    int bytesRead, bytesWritten = 0;
    struct timeval start1, end1;
    gettimeofday(&start1, NULL);

    bool register_status = false;
    char again[1500] = "Please enter a different nickname:";
    char start[1500] = "Type you answer below:";
    bool ingame = false;

    while(1)
    {
        std::string a = msg;
        //clear the buffer
        memset(&msg, 0, sizeof(msg));
        //receive message from server
        bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);
        
        if (!strcmp(msg, "exit"))
        {
            std::cout << "Server has quit the session" << endl;
            break;
        }

        if (!strcmp(msg, again)) 
        {
            register_status = false;
        }

        if (!strcmp(msg, start)) 
        {
            ingame = true;
        }
        
        std::cout << "Server: " << msg << std::endl;

        if (!register_status) 
        {
            std:: cout << ">";
            string data;
            getline(cin, data);
            //clear the buffer
            memset(&msg, 0, sizeof(msg));
            strcpy(msg, data.c_str());
            bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
            register_status = true;
            memset(&msg, 0, sizeof(msg));
        }
        
        std::atomic<bool> flag{false};

        void listenToServer() {
            while (!flag)
            {
                std::cout << "Listening...\n"; // Sleep for one second
            }
        }

        void sendToServer() {
            std::cout << ">";
            string data;
            getline(cin, data);
            if (!strcmp(data, "exit")) {
                flag = true;
                close(clientSd);
            }
            //clear the buffer
            memset(&msg, 0, sizeof(msg));
            strcpy(msg, data.c_str());
            bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
            memset(&msg, 0, sizeof(msg));
        }

        if (ingame) {
            // std::thread sendS(sendToServer);
            std::thread listenS(listenToServer);
            flag=true;
            listenS.join();
        }
        memset(&msg, 0, sizeof(msg));
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
