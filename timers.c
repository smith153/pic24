#include "p33Fxxxx.h"
#include "timers.h"

static volatile long int timerCurMS;
static volatile long int timerMax; 


/*####################### Timer functions #####################################################*/

//millisecond sleep
void sleep(const int count)
{
  timer(count);
  while(timerCurMS < timerMax)
	Nop();
}

//non-blocking millisecond sleep
void timer(const int count)
{
  long int ucount = count;
  ucount = ucount * 1000;
  utimer(ucount);
}

//microsecond sleep
void usleep(const long int count)
{
  utimer(count);
  while(timerCurMS < timerMax)
	Nop();
}

//non-blocking microsecond sleep
void utimer(const long int count)
{
  T5CONbits.TON = 0; //stop timer
  T5CONbits.TCS = 0; //use FCY
  T5CONbits.TGATE = 0; //no tgate
  T5CONbits.TCKPS = 0; //1:1 divisor
  
  TMR5 = 0; //timer5 count to 0
  timerCurMS = 0;
  timerMax = count;
  

  //set interrupt period,1us
  PR5 = 40;  

  T5CONbits.TON = 1; //turn on timer;
}

//return milliseconds
int getTimer()
{
  return (int)(timerCurMS/1000);
}

//return microseconds
long int getuTimer()
{
  return timerCurMS;
}

/*############################################################################*/

/*###Interrupt functions for Timer###*/

void __attribute__((interrupt,auto_psv)) _T5Interrupt(void)
{
  timerCurMS++;  //keep track of timer roll overs

  if(timerCurMS >= timerMax){
	//turn off timer
	T5CONbits.TON = 0;
  }

  IFS1bits.T5IF = 0;  //Clear Timer5 interrupt flag
}
/*######*/

