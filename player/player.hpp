#include <string>
#include <vector>

class Player
{
public:
    std::string getNickname();
    int getScore();
    Player(std::string nickname);
    void setScore(int score);
    bool getAnswerState();
    void resetAnswerState();
private:
    std::string nickname;
    int score;
    bool answer_state;
};