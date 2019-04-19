//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux  
#include <stdio.h>  
#include <string.h>   //strlen  
//#include <stdlib.h>  
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
#include "../data/timer/timer.hpp"
#include <iostream>     
#include <cstdlib>
#define TRUE   1  
#define FALSE  0  
#define PORT 12345
     
int main(int argc , char *argv[])   
{   
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[30] ,  
          max_clients = 30 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in address;   
    Timer timer;
    bool reset_timer = false, register_nickname = true;
    int number_of_question;
         
    char buffer[1024];  //data buffer of 1K  
         
    //set of socket descriptors  
    fd_set readfds;   
         
    //a message  
    char *message = "Welcome to Question! \n Please enter your nickname:";   
     
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

    // Map to control nickname
    std::map<std::pair<std::string,int>,int> nickname;
    // Count number of ppl join
    int player_count = 0;
    // Question manager
    QuestionManager question_manager;
    number_of_question = question_manager.getNumberOfQuestions();
    printf("Max %d player\n", question_manager.getMaxPlayer());

    bool answer[question_manager.getMaxPlayer()];
    int score[question_manager.getMaxPlayer()];

    // The current question
    Question current_question;
    // Control so the server only sent the question once
    bool sentQuestion = false;
    // Sent game start
    bool sentStartNoti = false;
    //Player answer

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
                    getpeername(sd , (struct sockaddr*)&address ,  
                        (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    //Close the socket and mark as 0 in list for reuse  
                    close(sd);   
                    client_socket[i] = 0; 
                    //Delete the nick name from nickname map
                    std::map<std::pair<std::string, int>, int>::iterator itr; 
                    for (itr = nickname.begin(); itr != nickname.end(); ++itr) { 
                        if(itr->second == i)
                        {
                            itr = nickname.erase(itr);
                        }
                    } 
                    printf("Current have %d player\n", player_count);
                }   

                if(!strcmp(buffer, "exit"))
		        {
		        	if (register_nickname){
			            std::cout << "Client has quit the session" << std::endl;
			            break;
		        	} else {
		        		std::string noti = "Player "; 
                        // noti += std::to_string(player_count);
                        // insert name of current player
                        noti += " has been disconnected!";
                        char noti_mes[noti.size()+1];
                        strcpy(noti_mes, noti.c_str());
                        for (int j = 0; j < max_clients; j++)   
                        {
                            int sd2 = client_socket[j];
                            // if exist socket
                            if (sd2 != 0)
                            {
                            	send(sd2, noti_mes, strlen(noti_mes), 0);
                        	}
                        }
                        break;
		        	}
		        }


                //If player count is not enough or countdown is still running then
                //its register message 
                //reset Timer to count from when start first connect and begin register nickname
                if (! reset_timer && register_nickname)
                {
                	timer.resetTimer();
                	reset_timer = true;
                }

                if (register_nickname && question_manager.getMaxPlayer() > player_count && timer.getTimeRange()<60*5)
                {   
                    //Found the nickname in the nickname map
                    bool found = false;
                    std::map<std::pair<std::string, int>, int>::iterator itr; 
                    for (itr = nickname.begin(); itr != nickname.end(); ++itr) { 
                        std::pair<std::string, int> name_score = itr->first;
                        std::string name = name_score.first;
                        char name_arr[name.size()+1];
                        strcpy(name_arr, name.c_str());
                        if(!strcmp(buffer, name_arr))
                        {
                            bool found = true;
                            break;
                        }
                    } 
                    //If we cant find the index in the nickname map then 
                    //he havent register so this is the register message
                    if(!found){
                        //Add to nickname map
                        std::string name = buffer;
                        //Nickname and score
                        std::pair<std::string, int> name_score = std::pair<std::string, int>(name,0);
                        nickname.insert(std::pair<std::pair<std::string, int>, int>(name_score,i));
                        //Make string to send back
                        std::string mess = "Register as "+name;
                        char register_msg[mess.size()+1];
                        strcpy(register_msg, mess.c_str());
                        //Send back to client
                        send(sd , register_msg , strlen(register_msg) , 0 );
                        //Send message to all other client (broadcast)
                        for (int j = 0; j < max_clients; j++)   
                        {
                            if ( i!=j ) {
                                int sd1 = client_socket[j];
                                //Make string to send back
                                std::string mess = name + " join the fight!";
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
                        //Notify all ppl about player count
                        std::string noti = "We now have "; 
                        noti += std::to_string(player_count);
                        noti += " players in the game!";
                        char noti_mes[noti.size()+1];
                        strcpy(noti_mes, noti.c_str());
                        for (int j = 0; j < max_clients; j++)   
                        {
                            int sd2 = client_socket[j];
                            send(sd2, noti_mes, strlen(noti_mes), 0);
                            if (question_manager.getMaxPlayer() == player_count) {
                                std::string full = "We have enough player! The game will start now!";
                                char full_msg[full.size()+1];
                                strcpy(full_msg, full.c_str());
                                send(sd2,full_msg,strlen(full_msg) , 0 );

                                // end registry
                                register_nickname = false;
			                	reset_timer = false;
			                	std::fill_n(score, question_manager.getMaxPlayer(), 0);
                            }   
                        }
                        memset(&buffer, 0, sizeof(buffer));
                    //If found the nickname request to change
                    } else {
                        std::string name = buffer;
                        std::string mess = name + " already taken\n";
                        std::string again = "Please enter a different nickname:";
                        char register_msg[mess.size()+1];
                        strcpy(register_msg, mess.c_str());
                        char again_msg[again.size()+1];
                        strcpy(again_msg, again.c_str());
                        //Send back to client
                        send(sd , register_msg , strlen(register_msg) , 0 );
                        memset(&buffer, 0, sizeof(buffer));//clear the buffer
                        send(sd , again_msg , strlen(again_msg) , 0 );
                    }
                } else {
                	register_nickname = false;
                	reset_timer = false;
                	std::fill_n(score, question_manager.getMaxPlayer(), 0);
                	// Close register nickname
                }

                std::fill_n(answer, question_manager.getMaxPlayer(), false);

                //receive answer
	        	if (!register_nickname && sentQuestion)
	        	{
        			// if input is answer then process, else ignore other input
        			if (!strcmp(buffer,"1") || !strcmp(buffer,"2") || !strcmp(buffer,"3")|| !strcmp(buffer,"4"))
        			{
        				if (!answer[i]){
        					answer[i] = true;
        					// if (QuestionManager.checkAnswer(buffer)){
        					// 	score[i] += current_question.getPlusPoint();
        					// } else {
        					// 	score[i] += current_question.getMinusPoint();
        					// }
        				}
        			}
	        		
	        	}

            }   
        }  

        // begin to sent question and close register nickname
        // only need to reset to sent first question
        if (!reset_timer && !register_nickname){
        	timer.resetTimer();
        	reset_timer = true;
        }
        //If the player count is equal to the max player let the game begin
        if (!register_nickname)
        {
            // if (!sentQuestion) {
            //     //Send the question
            //     current_question = question_manager.getNextQuestion();
            //     std::string question = "";
            //     question += current_question.getQuestion();
            //     question += "\n" + current_question.getAnswer(0);
            //     question += "\n" + current_question.getAnswer(1);
            //     question += "\n" + current_question.getAnswer(2);
            //     question += "\n" + current_question.getAnswer(3);
            //     question += "\n";
            //     char question_msg[question.size()+1];
            //     strcpy(question_msg, question.c_str());
            //     for (i = 0; i < max_clients; i++)   
            //     {   
            //         sd = client_socket[i]; 
            //         send(sd , question_msg , strlen(question_msg) , 0 );
            //         //Send the type answer so the client can type answer
            //         std::string question_start = "Type you answer below:";
            //         char start[question.size()+1];
            //         strcpy(start, question_start.c_str());
            //         send(sd , start , strlen(start) , 0 );
            //     }
            //     sentQuestion = true;
            // }

        	//sent question
        	while (number_of_question){
        		if (!sentQuestion){
        			sentQuestion = true;
	        		current_question = question_manager.getNextQuestion();
	                std::string question = "";
	                question += current_question.getQuestion();
	                question += "\n1. " + current_question.getAnswer(0);
	                question += "\n2. " + current_question.getAnswer(1);
	                question += "\n3. " + current_question.getAnswer(2);
	                question += "\n4. " + current_question.getAnswer(3);
	                question += "\n";
	                char question_msg[question.size()+1];
	                strcpy(question_msg, question.c_str());
	                for (i = 0; i < max_clients; i++)   
	                {   
	                    sd = client_socket[i]; 
	                    send(sd , question_msg , strlen(question_msg) , 0 );
	                    //Send the type answer so the client can type answer
	                    std::string question_start = "Type your answer below:";
	                    char start[question.size()+1];
	                    strcpy(start, question_start.c_str());
	                    send(sd , start , strlen(start) , 0 );
	                }
        		}
        		
                if (timer.getTimeRange() > 2){
                	// print next question
                	sentQuestion = false;
                	timer.resetTimer();
                	number_of_question--;
                }
        	}

        	if (!number_of_question){
				std::string noti = "Total score = ";
			        
		    	for (int i = 0; i < max_clients; i++)   
			    {
			        if (client_socket[i] != 0){
			        	noti += score[i] + "\t";
			        // send(sd, noti_mes, strlen(noti_mes), 0);
			    	}
			    }
			    noti += "\n";

			    char noti_mes[noti.size()+1];
			    strcpy(noti_mes, noti.c_str());
			    int sd;
			    for (int j = 0; j < max_clients; j++)   
			    {
			        if ((sd = client_socket[j]) != 0 )
			        	send(sd, noti_mes, strlen(noti_mes), 0);
			    }
			    break;
		    }			
        } 
    } 

   
    //Notify all ppl about exit
    std::string noti = "exit";
    char noti_mes[noti.size()+1];
    strcpy(noti_mes, noti.c_str());
    for (int j = 0; j < max_clients; j++)   
    {
        int sd = client_socket[j];
        send(sd, noti_mes, strlen(noti_mes), 0);
    }
    return 0;   
}   