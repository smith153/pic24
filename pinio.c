#include "p33Fxxxx.h"
#include "pinio.h"
#include "pinio_s.h"

#define TXPIN RA0
#define RXPIN 2 //RP#

//public functions:
int sendRfBuf(const char *buf, const int count)
{
  int i;
  for(i = 0; i < count; i++){
	if( sendRfByte(buf[i] ) == -1 )
		break;
  }
  //send dummy bit
  PORTAbits.TXPIN = 1;
  unsigned int j;for(j=0;j<60000;j++){}
  PORTAbits.TXPIN = 0;
  return i;
}

int readRfBuf(char *buffer, const int count)
{
  int i;
  for(i = 0; i < count; i++){
	if( readRfByte(buffer++) == -1 )
		break;
  }
  return i;
}





/*############################################################################*/
//non public, ie. called by the above functions only:

static int sendRfByte(char byte)
{
  char i = 0;
  while( i < 8 ){
  //if 1, on for 75%, else off for 75%

	//is the lsb a 1 or 0?
	if(byte & 1)
	{
		pulseMod75();
		//set pin to 1
		PORTAbits.TXPIN = 1;

		//hold at 1 for 75%
		while(timerFlag75 != 1){
			Nop();
		}

		stopPulseMod75();
		pulseMod25();
		//set pin to 0
		PORTAbits.TXPIN = 0;

		//hold at 0 for 25%
		while(timerFlag25 != 1){
			Nop();
		}

		stopPulseMod25();
	}

	else
	{
		pulseMod25();
		PORTAbits.TXPIN = 1;

		//hold at 1 for 25%
		while(timerFlag25 != 1){
			Nop();
		}

		stopPulseMod25();
		pulseMod75();
		PORTAbits.TXPIN = 0;

		//hold at 0 for 75%
		while(timerFlag75 != 1){
			Nop();
		}

		stopPulseMod75();
	}
  //shift byte to get next bit
  byte = byte >> 1;
  //increment i
  i++;

  }//end while

  return 1;
}


/*############################################################################
pulseMod functions will use timers to hold 
pin at 1 or 0 for a length of time

numbers for duty cycles:
Fcy = 39920833hz
full toggle cycle,1khz: fcy/1000=39920
25% duty cycle: 39920/4=9980
75% duty cycle: 39920-9980=29941
half cycle: 39920/2=19960
*/

//used for 75% duty cycle
static void pulseMod75()
{
  //from: Section 11. Timers - PIC24H FRM 
  T2CONbits.TON = 0; //stop timer
  T2CONbits.TCS = 0; //use FCY
  T2CONbits.TGATE = 0; //no tgate
  T2CONbits.TCKPS = 0; //1:1 divisor
  
  TMR2 = 0; //timer2 count to 0

  //set interrupt period
  // 75% duty cycle: 39920-9980=29941
  PR2 = 29941;

  T2CONbits.TON = 1; //turn on timer;
  timerFlag75 = 0;
}

static void stopPulseMod75()
{
  T2CONbits.TON = 0;
}


//used for 25% duty cycle
static void pulseMod25()
{
  T3CONbits.TON = 0; //stop timer
  T3CONbits.TCS = 0; //use FCY
  T3CONbits.TGATE = 0; //no tgate
  T3CONbits.TCKPS = 0; //1:1 divisor
  
  TMR3 = 0; //timer3 count to 0

  //set interrupt period
  // 25% duty cycle: 39920/4=9980
  PR3 = 9980;  

  T3CONbits.TON = 1; //turn on timer;
  timerFlag25 = 0;
}

static void stopPulseMod25()
{
  T3CONbits.TON = 0;
}

/*############################################################################
used for demodulate, waveTimer will use a timer and rising edge/falling edge 
interrupts to determine whether a period is a 1 or 0
*/

static void startWaveTimer()
{
  T4CONbits.TON = 0; //stop timer
  T4CONbits.TCS = 0; //use FCY
  T4CONbits.TGATE = 0; //no tgate
  T4CONbits.TCKPS = 0; //1:1 divisor
  
  TMR4 = 0; //timer4 count to 0
  timerRollover = 0;

  //set interrupt period
  PR4 = 64000;  

  T4CONbits.TON = 1; //turn on timer;
}

static long int stopWaveTimer()
{
  T4CONbits.TON = 0;
  return ((timerRollover * 64000) + TMR4); 
}

/*############################################################################*/

/*###Interrupt functions for Timers###*/
void __attribute__((interrupt,auto_psv)) _T2Interrupt(void)
{
  timerFlag75 = 1;
  IFS0bits.T2IF = 0;  //Clear Timer2 interrupt flag
}

void __attribute__((interrupt,auto_psv)) _T3Interrupt(void)
{
  timerFlag25 = 1;
  IFS0bits.T3IF = 0;  //Clear Timer3 interrupt flag
}

void __attribute__((interrupt,auto_psv)) _T4Interrupt(void)
{
  timerRollover++;  //keep track of timer roll overs
  IFS1bits.T4IF = 0;  //Clear Timer4 interrupt flag
}
/*######*/


/*############################################################################
EdgeDetc functions will interrupt on rising or falling edges of a pin and
set a flag to messure what the duty cycle is*/

//for rising edge notification
static void enableRisingEdgeDetc()
{
  waitHigh = 0;  //make sure flag is zero before starting
  RPINR0bits.INT1R = RXPIN;  //set to pin rp#
  INTCON2bits.INT1EP = 0; //interrupt on rising edge
  IPC5bits.INT1IP = 1;  //interrup priority
  IFS1bits.INT1IF = 0;  //clear interrupt
  IEC1bits.INT1IE = 1; //int1 enable
}


//for falling edge notification
static void enableFallingEdgeDetc()
{
  waitLow = 0;  //make sure flag is zero before starting
  RPINR1bits.INT2R = RXPIN;  //set to pin rp#
  INTCON2bits.INT2EP = 1; //interrupt on falling edge
  IPC7bits.INT2IP = 1;  //interrup priority
  IFS1bits.INT2IF = 0;  //clear interrupt
  IEC1bits.INT2IE = 1; //int2 enable
}

static int readRfByte(char * byte)
{
  long int i = 0;
  long int max = 500000; //timeout value
  long int highTime = 0;
  long int lowTime = 0;
  char j = 0;

  while(1){
	i = 0;
	enableRisingEdgeDetc();
	//wait for pin to rise or exit at timeout
	while(!waitHigh){
		if(i > max)
			return -1;
		Nop();
		i++;
	}
 
	//skip this on first loop
	if(j > 0){
		lowTime = stopWaveTimer();
		//if 0, write 0 to msb
		if(lowTime > highTime)
			*byte = *byte & 127;
		else
			*byte = *byte | 128;

		if(j == 8)
			return 1;

		//shift to get next position
		*byte = *byte >> 1;
	}

	//time how long pin is high
	startWaveTimer();
	enableFallingEdgeDetc();

	//wait for pin to fall or exit at timeout
	while(!waitLow){
		if(i > max)
			return -1;
		Nop();
		i++;
	}
 
	highTime = stopWaveTimer();
	startWaveTimer();
	j++;  
  }

  return 1;
}

/*###Interrupt functions for edge detect###*/
void __attribute__((interrupt, auto_psv)) _INT2Interrupt(void)
{
  waitLow = 1;  //set flag, pin is transitioning to 0
  IFS1bits.INT2IF = 0;  //clear interrupt
  IEC1bits.INT2IE = 0; //disable interrupt
}

void __attribute__((interrupt, auto_psv)) _INT1Interrupt(void)
{
  waitHigh = 1;  //set flag, pin is transitioning to 1
  IFS1bits.INT1IF = 0;  //clear interrupt
  IEC1bits.INT1IE = 0; //disable interrupt
}
/*######*/



