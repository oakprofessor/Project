#include <string>

class Question{
public:
    Question(std::string, std::string, std::string, std::string, std::string, int, int);
    void setQuiz(std::string, std::string, std::string, std::string, std::string, int, int);
    std::string getAnswers(int);
    bool checkAnswer(int);
    void changeOrder();	  

private:
    int order[4];
    int correct_answer;
public:
	std::string question;
    std::string answer[4];
    int question_score;
};