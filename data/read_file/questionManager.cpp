#include "./questionManager.hpp"
#include <iostream>
#include <fstream>

void QuestionManager::setMaxPlayer(int a){
    maxPlayer = a;
}

void QuestionManager::setWaitTime(int a){
    waitTime = a;
}

void QuestionManager::setAnswerTime(int a){
    answerTime = a;
}

int QuestionManager::getNumberOfQuestions(){
    return questions.size();
}

int QuestionManager::getMaxPlayer(){
    return maxPlayer;
}

int QuestionManager::getWaitTime(){
    return waitTime;
}

int QuestionManager::getAnswerTime(){
    return answerTime;
}

Question QuestionManager::getNextQuestion()
{
    question_count++;
    return questions.at(question_count-1);
}

Question QuestionManager::getQuestionAt(int a){
    return questions.at(a);
}

void QuestionManager::readQuestionsFromFile(){
    std::string line;
    std::ifstream myfile("../text_file/questions.txt");
    if (myfile.is_open())
    {
        while(getline (myfile,line)){
            Question qs;
            //Question
            qs.setQuestion(line);
            //Option
            for(size_t i = 0; i < 4; i++)
            {
                getline (myfile,line);
                qs.setAnswer(line);
            }
            //Right answers
            getline (myfile,line);
            qs.setRightAnswer(line);
            //Plus point
            getline (myfile,line);
            int number = std::atoi (line.c_str());
            qs.setPlusPoint(number);
            //Minus point
            getline (myfile,line);
            number = std::atoi (line.c_str());
            qs.setMinusPoint(number);
            //Add to vector
            questions.push_back(qs);
        }
        myfile.close();
    }
    else {
        std::cout << "Unable to open file"; 
    }
}

void QuestionManager::readConfigFromFile(){
    std::string line;
    std::ifstream myfile("../text_file/config.txt");
    if (myfile.is_open())
    {
        //Max player
        getline (myfile,line);
        getline (myfile,line);
        int number = std::atoi (line.c_str());
        setMaxPlayer(number);
        //Wait time
        getline (myfile,line);
        getline (myfile,line);
        number = std::atoi (line.c_str());
        setWaitTime(number);
        //Wait time
        getline (myfile,line);
        getline (myfile,line);
        number = std::atoi (line.c_str());
        setAnswerTime(number);
        //Close file
        myfile.close();
    }
    else {
        std::cout << "Unable to open file"; 
    }
}

QuestionManager::QuestionManager()
{
    question_count = 0;
    readQuestionsFromFile();
    readConfigFromFile();
}