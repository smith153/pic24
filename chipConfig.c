#include "p33Fxxxx.h"
#include "chipConfig.h"


//macros for baudrate setup
#define FCY 39920833
#define BAUDRATE 9600
#define BRGVAL (((FCY/BAUDRATE)/16)-1)+1

//wrapper function to config chip clock and ports
void chipConfig()
{
  initClock();
  setSerialPorts();
  confSerials();
  setPinTris();
  enableCrc();
  
  //enable interrupts for timers
  setInterrupts();

  
}

//sets system clock to 80mhz
void initClock()
{
  //system clock (FOSC) = 7.37 * M/(N1 * N2)

  PLLFBD = 63;	// M = 65
  CLKDIVbits.PLLPOST = 0;  // N1 = 2
  CLKDIVbits.PLLPRE = 1;  // N2 = 3
  CLKDIVbits.ROI = 0;  //disable reduced clock powersaving mode
  CLKDIVbits.FRCDIV = 0;
  CLKDIVbits.DOZEN = 0;
  CLKDIVbits.DOZE = 0;
  OSCTUN = 0;  
  RCONbits.SWDTEN = 0; //disable watchdog

  // Clock switch to incorporate PLL
  __builtin_write_OSCCONH(0x01);	// Initiate Clock Switch to // FRC with PLL (NOSC=0b001)
  __builtin_write_OSCCONL(0x01);	// Start clock switching
  while (OSCCONbits.COSC != 0x001);	// Wait for Clock switch to occur	
  while(OSCCONbits.LOCK != 1) {};

}

//sets serial ports to rp6,rp7,pr8,rp9
void setSerialPorts()
{
  //from: Section 30. I/O Ports With Peripheral Pin Select

  // Unlock pin Registers
 ///////// __builtin_write_OSCCONL(OSCCON & ~(1<<6));

  //turn off ADC
  AD1PCFGL = 0xFFFF;

  // Assign U1Rx To Pin RP7
  RPINR18bits.U1RXR = 7;

  // Assign U1Tx To Pin RP6
  RPOR3bits.RP6R = 3;

  // Assign U2Rx To Pin RP9
  RPINR19bits.U2RXR = 9;

  // Assign U2Tx To Pin RP8
  RPOR4bits.RP8R = 5;

  // Lock pin Registers
  ///////__builtin_write_OSCCONL(OSCCON | (1<<6));
}
 
//sets serial params
void confSerials()
{
  //from: Section 17. UART

  //loopback
  //U1MODEbits.LPBACK = 1;

  U1BRG = BRGVAL;  //U1 rate = macro defined baud rate

  U1MODEbits.STSEL = 0;  //1-stop bit
  U1MODEbits.PDSEL = 0;  //no Parity, 8 data bits
  U1MODEbits.ABAUD = 0;  // auto-baud disable
  U1MODEbits.BRGH = 0;  //high speed mode
  U1MODEbits.UARTEN = 1;  //enable uart
  U1MODEbits.UEN = 0;  //enable tx,rx

  //set interrupt events, interrupts not enabled though
  U1STAbits.URXISEL = 0;  //send interrupt for every char received
  U1STAbits.UTXISEL1 = 0;  //send interrupt for every char sent
  U1STAbits.UTXISEL0 = 0;  //send interrupt for every char sent
  U1STAbits.UTXEN = 1;  //enable uart tx

  //UART2

  //loopback
  //U2MODEbits.LPBACK = 1;

  U2BRG = BRGVAL;  //U1 rate = macro defined baud rate

  U2MODEbits.STSEL = 0;  //1-stop bit
  U2MODEbits.PDSEL = 0;  //no Parity, 8 data bits
  U2MODEbits.ABAUD = 0;  // auto-baud disable
  U2MODEbits.BRGH = 0;  //high speed mode
  U2MODEbits.UARTEN = 1;  //enable uart
  U2MODEbits.UEN = 0;  //enable tx,rx

  //set interrupt events, interrupts not enabled though
  U2STAbits.URXISEL = 0;  //send interrupt for every char received
  U2STAbits.UTXISEL1 = 0;  //send interrupt for every char sent
  U2STAbits.UTXISEL0 = 0;  //send interrupt for every char sent
  U2STAbits.UTXEN = 1;  //enable uart tx
}

//set RA0 for output for modulation
void setPinTris()
{
  //turn off ADC
  AD1PCFGL = 0xFFFF;

  TRISAbits.TRISA0 = 0;
}

void setInterrupts()
{
  IPC1bits.T2IP = 2; // Set Timer2 Interrupt Priority Level
  IFS0bits.T2IF = 0;  // Clear Timer2 Interrupt Flag
  IEC0bits.T2IE = 1;   // Enable timer 2 interrupt

  IPC2bits.T3IP = 2; // Set Timer3 Interrupt Priority Level
  IFS0bits.T3IF = 0;  // Clear Timer3 Interrupt Flag
  IEC0bits.T3IE = 1;   // Enable timer 3 interrupt

  IPC6bits.T4IP = 2;  //Set Timer4 priority
  IFS1bits.T4IF = 0;  //Clear Timer4 Interrupt Flag
  IEC1bits.T4IE = 1;  //Enable timer4 interrupt

  IPC7bits.T5IP = 2;  //Set Timer5 priority
  IFS1bits.T5IF = 0;  //Clear Timer5 Interrupt Flag
  IEC1bits.T5IE = 1;  //Enable timer5 interrupt
}

//enable CRC
void enableCrc()
{
  CRCCONbits.PLEN = 0x0F; //enable 16 bit mode
  CRCXOR = 0x1020; //crc xmodem mode
}



