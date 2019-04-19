//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux  
#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <map>
#include <iterator> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#include "../data/read_file/questionManager.hpp"
#include "../player/player.hpp"
#include <iostream>     
#define TRUE 1  
#define FALSE 0  
#define PORT 12345
     
int main(int argc , char *argv[])   
{   
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[10] ,  
          max_clients = 10 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;   
         
    char buffer[1024];  //data buffer of 1K  
         
    //set of socket descriptors  
    fd_set readfds;   
         
    //a message  
    char message[] = "Welcome to Question! \nPlease enter your nickname:";   
     
    //initialise all client_socket[] to 0 so not checked  
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //create a master socket  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    //set master socket to allow multiple connections ,  
    //this is just a good habit, it will work without this  
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    //type of socket created  
    address.sin_family = AF_INET;   
    address.sin_addr.s_addr = INADDR_ANY;   
    address.sin_port = htons( PORT );   
         
    //bind the socket to localhost port PORT
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port %d \n", PORT);   
         
    //try to specify maximum of 10 pending connections for the master socket  
    if (listen(master_socket, 10) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }
         
    //accept the incoming connection  
    addrlen = sizeof(address);   
    puts("Waiting for connections ..."); 

    // Map to control player
    std::map<int,Player> player_manager;
    // Count number of ppl join
    int player_count = 0;
    // Question manager
    QuestionManager question_manager;
    printf("Max %d player\n", question_manager.getMaxPlayer());
    // The current question
    Question current_question;
    // Control so the server only sent the question once
    bool sentQuestion = false;
    // Sent game start
    bool sentStartNoti = false;
    // Number of player answered a question
    int number_answer = 0;
    // Number of question answerd
    int number_question = 0;

    while(TRUE)
    {   
        //clear the buffer
        memset(&buffer, 0, sizeof(buffer));
        //clear the socket set  
        FD_ZERO(&readfds);   
        //add master socket to set  
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
        //add child sockets to set  
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //socket descriptor  
            sd = client_socket[i];   
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);     
            //highest file descriptor number, need it for the select function  
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //wait for an activity on one of the sockets , timeout is NULL ,  
        //so wait indefinitely  
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //If something happened on the master socket ,  
        //then its an incoming connection  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket,  
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //inform user of socket number - used in send and receive commands  
            printf("New connection , socket fd is %d , ip is : %s , port : %d  \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs 
                  (address.sin_port));   
           
            //send new connection greeting message  
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )   
            {   
                perror("send");   
            }   

            //add new socket to array of sockets  
            for (i = 0; i < max_clients; i++)   
            {   
                //if position is empty  
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                    break;   
                }   
            }   
        }   
             
        //else its some IO operation on some other socket 
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
            if (FD_ISSET( sd , &readfds))   
            {   
                //Check if it was for closing , and also read the  
                //incoming message  

                if ((valread = read( sd , buffer, 1024)) == 0)   
                {   
                    player_count--;
                    //Somebody disconnected , get his details and print  
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    //Close the socket and mark as 0 in list for reuse  
                    close(sd);   
                    client_socket[i] = 0; 
                    std::map<int, Player>::iterator itr; 
                    for (itr = player_manager.begin(); itr != player_manager.end(); ++itr) { 
                        if(itr->first == i)
                        {
                            itr = player_manager.erase(itr);
                            break;
                        }
                    } 
                    printf("Current have %d player\n", player_count);
                }   
                //If player count is not enough or countdown is still running then its register message 
                if ( player_count < question_manager.getMaxPlayer())
                {   
                    //Found the nickname in the nickname map
                    bool found = false;
                    std::map<int, Player>::iterator itr; 
                    for (itr = player_manager.begin(); itr != player_manager.end(); ++itr) { 
                        Player _player = itr->second;
                        std::string name = _player.getNickname();
                        std::string buff = buffer;
                        if( buff == name)
                        {
                            found = true;
                            break;
                        }
                    } 
                    //If we cant find the index in the nickname map then 
                    //he havent register so this is the register message
                    if(!found){
                        //Add to player_manager map
                        std::string name = buffer;
                        //Nickname and score
                        Player _player = Player(name);
                        player_manager.insert(std::pair<int,Player>(i,_player));
                        //Make string to send back
                        std::string mess = "Register as "+name+"! ";
                        char register_msg[mess.size()+1];
                        strcpy(register_msg, mess.c_str());
                        //Send back to client
                        if (send(sd , register_msg , strlen(register_msg) , 0 ) == strlen(register_msg)) {
                            sleep(1);
                        }
                        //Send message to all other client (broadcast)
                        for (int j = 0; j < max_clients; j++)   
                        {
                            if ( i!=j ) {
                                int sd1 = client_socket[j];
                                //Make string to send back
                                std::string mess = name + " join the fight! ";
                                char register_msg[mess.size()+1];
                                strcpy(register_msg, mess.c_str());
                                //Send back to client
                                send(sd1 , register_msg , strlen(register_msg) , 0 );
                            }
                        }
                        //Increase the player count
                        player_count++;
                        //clear the buffer
                        memset(&buffer, 0, sizeof(buffer));
                        
                        for (int j = 0; j < max_clients; j++)   
                        {
                            int sd2 = client_socket[j];
                            //Notify all ppl about player count
                            std::string noti = "We now have "; 
                            noti += std::to_string(player_count);
                            noti += " players in the game!";
                            char noti_mes[noti.size()+1];
                            strcpy(noti_mes, noti.c_str());
                            if (send(sd2, noti_mes, strlen(noti_mes), 0) == strlen(noti_mes)) {
                                sleep(1);
                            }
                            
                            if (question_manager.getMaxPlayer() == player_count) {
                                std::string full = "We have enough player! The game will start now!";
                                char full_msg[full.size()+1];
                                strcpy(full_msg, full.c_str());
                                send(sd2,full_msg,strlen(full_msg) , 0 );
                            }   
                        }
                        memset(&buffer, 0, sizeof(buffer));
                    //If found the nickname request to change
                    } else {
                        std::string name = buffer;
                        std::string mess = name + " already taken";
                        std::string again = "Please enter a different nickname:";
                        char register_msg[mess.size()+1];
                        strcpy(register_msg, mess.c_str());
                        char again_msg[again.size()+1];
                        strcpy(again_msg, again.c_str());
                        //Send back to client
                        send(sd , register_msg , strlen(register_msg) , 0 );
                        memset(&buffer, 0, sizeof(buffer));//clear the buffer
                        sleep(1);
                        send(sd , again_msg , strlen(again_msg) , 0 );
                    }
                }  

                //If the game is still going then its the answer to the current question
                if ( player_count == question_manager.getMaxPlayer() ){
                    //Find the player by socket_id
                    std::map<int, Player>::iterator itr; 
                    for (itr = player_manager.begin(); itr != player_manager.end(); ++itr) { 
                        if(itr->first == i)
                        {
                            //Check if the player answered the question
                            if (!itr->second.getAnswerState()) {
                                std::string answer = buffer;
                                int score = current_question.giveScore(answer);
                                itr->second.setScore(score);
                                number_answer++;
                            }
                        }
                    } 

                    //If all player have answered begin a new question
                    if (number_answer == question_manager.getMaxPlayer()) {
                        for (itr = player_manager.begin(); itr != player_manager.end(); ++itr) { 
                            itr->second.resetAnswerState();
                        } 
                        number_question++;
                        sentQuestion = false;
                        number_answer = 0;
                    }
                    break;
                }
            }   
        }  

        //If the player count is equal to the max player let the game begin
        if (question_manager.getMaxPlayer() == player_count)
        {
            sleep(3);
            if (!sentQuestion) {
                //If we have no question end the game
                if (number_question == question_manager.getNumberOfQuestions()) {
                    //Disconnect all user
                    std::string noti = "exit";
                    char noti_mes[noti.size()+1];
                    strcpy(noti_mes, noti.c_str());
                    for (int j = 0; j < max_clients; j++)   
                    {
                        int sd = client_socket[j];
                        send(sd, noti_mes, strlen(noti_mes), 0);
                    }
                } else {
                    //Send the question
                    current_question = question_manager.getNextQuestion();
                    std::string question = "";
                    question += current_question.getQuestion();
                    question += "\n" + current_question.getAnswer(0);
                    question += "\n" + current_question.getAnswer(1);
                    question += "\n" + current_question.getAnswer(2);
                    question += "\n" + current_question.getAnswer(3);
                    char question_msg[question.size()+1];
                    strcpy(question_msg, question.c_str());
                    for (i = 0; i < max_clients; i++)   
                    {   
                        sd = client_socket[i]; 
                        if (send(sd , question_msg , strlen(question_msg) , 0 ) == strlen(question_msg)) {
                            sleep(1);
                        } 
                        //Send the type answer so the client can type answer
                        std::string question_start = "Type you answer below:";
                        char start[question.size()+1];
                        strcpy(start, question_start.c_str());
                        send(sd , start , strlen(start) , 0 );
                    }
                    sentQuestion = true;
                }
            }
        } 
    } 
    return 0;   
}   