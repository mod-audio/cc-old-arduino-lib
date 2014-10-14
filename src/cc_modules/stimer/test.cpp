
#include <stdio.h>
#include <iostream>
// #include "stimer.h"

using namespace std;

#if defined __linux__
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "stimer.h"

void event_handler (int signum) // 1ms interrupt
{
    if (signum == SIGALRM)
    {
        STimer::static_timer_count++;        
    }
}
#endif


int main(void)
{
    struct sigaction sa;
    struct itimerval timer;

    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &event_handler;
    sigaction (SIGALRM, &sa, NULL);
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 1000;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 1000;
    setitimer (ITIMER_REAL, &timer, NULL);

    // how to use STimer

    STimer alarm(1000);

    alarm.start();

    while(1){
        if (alarm.check()){
            cout << "BOOYA" << endl;
        }   
    }

    return 0;
}

