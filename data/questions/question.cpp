#include "question.hpp"
#include <iostream>

void Question::setAnswer(std::string a){
    answer.push_back(a);
}

void Question::setRightAnswer(std::string a){
    rightAnswer = a;
}

void Question::setQuestion(std::string a){
    question = a;
}

void Question::setPlusPoint(int a){
    plus_point = a;
}

void Question::setMinusPoint(int a){
    minus_point = a;
}

void Question::toString(){
    std::cout<<getQuestion()<<std::endl;
    for(size_t i = 0; i < answer.size(); i++)
    {
        std::cout<<getAnswer(i)<<std::endl;
    }
    std::cout<<getRightAnswer()<<std::endl;
    std::cout<<getPlusPoint()<<std::endl;
    std::cout<<getMinusPoint()<<std::endl;
}

std::string Question::getAnswer(int i){
    return answer.at(i);
}

std::string Question::getQuestion(){
    return question;
}

std::string Question::getRightAnswer(){
    return rightAnswer;
}

int Question::getPlusPoint(){
    return plus_point;
}

int Question::getMinusPoint(){
    return minus_point;
}