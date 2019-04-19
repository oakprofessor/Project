#include "../questionManager.hpp"
#include <iostream>
int main(){
    QuestionManager qm;
    std::cout<<qm.getMaxPlayer()<<std::endl;
    std::cout<<qm.getAnswerTime()<<std::endl;
    std::cout<<qm.getWaitTime()<<std::endl;
    qm.getNextQuestion().toString();
    qm.getNextQuestion().toString();
    qm.getNextQuestion().toString();
    qm.getNextQuestion().toString();
}