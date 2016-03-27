//
// Created by Silvio Fragnani da Silva on 21/03/16.
//

#ifndef SDLTUTORIALS_TIMER_H
#define SDLTUTORIALS_TIMER_H

class Timer {
private:
    //The clock time when the timer started
    int startTicks;

    //The ticks stored when the timer was paused
    int pausedTicks;

    //The timer status
    bool paused;
    bool started;

public:
    //Initializes variables
    Timer();

    //The various clock actions
    void start();
    void stop();
    void pause();
    void unPause();

    //Gets the timer's time
    int getTicks();

    //Checks the status of the timer
    bool isStarted();
    bool isPaused();
};


#endif //SDLTUTORIALS_TIMER_H
