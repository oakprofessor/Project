#include "question.hpp"


void Question::Question(std::string question, std::string a_1, std::string a_2, std::string a_3, std::string a_4, int correct_answer, int plus_score, int minus_score)
{
	this->question = question;
	this->answer[0] = a_1;
	this->answer[1] = a_2;
	this->answer[2] = a_3;
	this->answer[3] = a_4;
	this->correct_answer = correct_answer;
	this->question_plus_score = plus_score;
    this->question_minus_score = minus_score;
}

void Question::setQuiz(std::string question, std::string a_1, std::string a_2, std::string a_3, std::string a_4, int correct_answer, int plus_score, int minus_score)
{
    this->question = question;
    this->answer[0] = a_1;
    this->answer[1] = a_2;
    this->answer[2] = a_3;
    this->answer[3] = a_4;
    this->correct_answer = correct_answer;
    this->question_plus_score = plus_score;
    this->question_minus_score = minus_score;
}

std::string Question::getAnswers(int index){
	return answer[order[index-1];
}

bool Question::checkAnswer(int index){
	return order[index-1] == correct_answer;
}

void Question::changeOrder(){
	vector<int> v(4); 
  
    // Fill the vector with the values 
    // 1, 2, 3, ..., n 
    for (int i = 0; i < 4; i++) 
        v[i] = i; 
  
    // While vector has elements 
    // get a random number from the vector and print it 
    while (v.size()) { 
        order[i] =  getNum(v) ;
    } 
}

int getNum(vector<int>& v) 
{ 
  
    // Size of the vector 
    int n = v.size(); 
  
    // Generate a random number 
    srand(time(NULL)); 
  
    // Make sure the number is within 
    // the index range 
    int index = rand() % n; 
  
    // Get random number from the vector 
    int num = v[index]; 
  
    // Remove the number from the vector 
    swap(v[index], v[n - 1]); 
    v.pop_back(); 
  
    // Return the removed number 
    return num; 
} 
  