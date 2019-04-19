#include <string>
#include <vector>
#include "../questions/question.hpp"
class QuestionManager
{
public:
    QuestionManager();
    Question getNextQuestion();
    int getNumberOfQuestions();
    Question getQuestionAt(int a);
    int getMaxPlayer();
    int getWaitTime();
    int getAnswerTime();
private:
    int question_count;
    int maxPlayer;
    int waitTime;
    int answerTime;
    std::vector<Question> questions;
    void readQuestionsFromFile();
    void readConfigFromFile();
    void setMaxPlayer(int a);
    void setWaitTime(int a);
    void setAnswerTime(int a);
};