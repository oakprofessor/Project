#include "player.hpp"

int Player::getScore(){
    return score;
}

bool Player::getAnswerState(){
    return answer_state;
}

void Player::resetAnswerState(){
    answer_state = false;
}

void Player::setScore(int a){
    score += a;
    answer_state = true;
}

std::string Player::getNickname(){
    return nickname;
}

Player::Player(std::string name){
    nickname = name;
    score = 0;
    answer_state = false;
}