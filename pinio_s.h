#ifndef PINIO_S_H
#define PINIO_S_H

/*##########################Local flags for interrupts########################*/
static volatile char timerFlag75;
static volatile char timerFlag25;
static volatile char timerFlagDeMod;
static volatile char waitLow;
static volatile char waitHigh;
static volatile int timerRollover;
/*####*/



static int sendRfByte(char byte);

static void pulseMod75();

static void stopPulseMod75();

static void pulseMod25();

static void stopPulseMod25();

static void startWaveTimer();

static long int stopWaveTimer();

static void enableRisingEdgeDetc();

static void enableFallingEdgeDetc();

static int readRfByte(char * byte); 

#endif
