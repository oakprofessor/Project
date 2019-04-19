#include <string>
#include <vector>

class Question
{
public:
    void setQuestion(std::string a);
    void setRightAnswer(std::string a);
    void setAnswer(std::string a);
    void setMinusPoint(int a);
    void setPlusPoint(int a);
    std::string getAnswer(int i);
    std::string getQuestion();
    std::string getRightAnswer();
    int getPlusPoint();
    int getMinusPoint();
    void toString();
private:
    std::string question;
    std::vector<std::string> answer;
    std::string rightAnswer;
    int minus_point;
    int plus_point;
};