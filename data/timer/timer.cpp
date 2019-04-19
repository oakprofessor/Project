#include "timer.hpp"

// constructor
Timer::Timer()
{
    time(&this->start);
}

// constructor with param
Timer::Timer(time_t time)
{
    start = time;
}

//deconstructor
Timer::~Timer()
{
    // nothing .-.
}

void Timer::resetTimer()
{
    time(&start);    
}

int Timer::getTimeRange()
{
    return int(time(NULL) - start);
}
