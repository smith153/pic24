#ifndef TIMER_H
#define TIMER_H

void sleep(const int count);

void timer(const int count);

void usleep(const long int count);

void utimer(const long int count);

int getTimer(); 

long int getuTimer();

#endif

/*
sleep(), usleep(), and timer() all call utimer(). Calls to any of these functions will restart
the timer. utimer() counts microseconds and interrupts every 80 clock cycles. getTimer() returns 
the microseconds divided by 1000 (therefore the returned value is in milliseconds), getuTimer() 
returns microseconds. getTimer() and getuTimer() are used for polling when non-blocking functions
are needed.



example use:

block and sleep for 3/4 a second:
int i = 750;
sleep(i);
or
sleep(750);

or

long int i = 750000;
usleep(i);
or
usleep(750000);

##########
start timer for 3/4 a second and don't block:

int i = 750;
timer(i);

while( getTimer() != i ){
  //do stuff  
}

if( getTimer() == i ){
  //do stuff  
}

same thing with microtimer:

long int i = 750000;
utimer(i);

while( getuTimer() != i ){
  //do stuff  
}

if( getuTimer() == i ){
  //do stuff  
}


*/

