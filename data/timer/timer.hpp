#include <time.h>

class Timer
{
	private:
    	time_t start;

    public:
        Timer();
        Timer(time_t);
        int getTimeRange();
        void resetTimer();
        ~Timer();
};
