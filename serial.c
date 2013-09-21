#include "p33Fxxxx.h"
#include "serial.h"
#include "timers.h"

#define READTIMEOUT 1000

//UART1 functions

//read one byte from rx register, timeout after ~1second
//returns -1 if timeout
int readByte1(unsigned char *byte)
{
  //start timer
  timer(READTIMEOUT);
  while(getTimer() < READTIMEOUT){

	//get the data from register, if there
	if(U1STAbits.URXDA == 1){
		*byte = U1RXREG;
		return 1;
	}
	
  }
  //return -1 on timeout
  return -1;
}

//send byte, wait if buffer full
//returns -1 if buffer fails to clear
int writeByte1(const unsigned char byte)
{
  unsigned int i = 0;
  //tx buffer full? then wait up to 25000 cycles
  while( U1STAbits.UTXBF == 1 ){
	Nop();
	i++;
	if(i == 25000)
		return -1;	
  }
  
  //write to register
  U1TXREG = byte;

  return 1;
}

//receive bytes into a buffer
//returns number of bytes actually received
int readSerial1(unsigned char *buffer, const int count)
{
  int i;
  for(i = 0; i < count; i++){
	if( readByte1(buffer++) == -1 )
		break;
  }
  return i;
}

//send bytes from a buffer
//returns number of byte actually written
int writeSerial1(const unsigned char *buffer, const int count){
  int i;
  for(i = 0; i < count; i++){
	if( writeByte1(buffer[i]) == -1 )
		break;
  }
  return i;
}

void breakSerial1()
{
  U1STAbits.UTXBRK = 1;
  writeByte1('U');
}

/*#########################################*/
//UART2 functions

//read one byte from rx register, timeout after ~1second
int readByte2(unsigned char *byte)
{
  timer(READTIMEOUT);
  while(getTimer() < READTIMEOUT){
    
	//get the data from register, if there
	if(U2STAbits.URXDA == 1){
		*byte = U2RXREG;
		return 1;
	}

  }
  //return -1 on timeout
  return -1;
}

//send byte, wait if buffer full
int writeByte2(const unsigned char byte)
{
  unsigned int i = 0;
  //tx buffer full? then wait up to 25000 cycles
  while( U2STAbits.UTXBF == 1 ){
	Nop();
	i++;
	if(i == 25000)
		return -1;	
  }
  
  //write to register
  U2TXREG = byte;

  return 1;
}

//receive bytes into a buffer
int readSerial2(unsigned char *buffer, const int count)
{
  int i;
  for(i = 0; i < count; i++){
	if( readByte2(buffer++) == -1 )
		break;
  }
  return i;
}

//send bytes from a buffer
int writeSerial2(const unsigned char *buffer, const int count){
  int i;
  for(i = 0; i < count; i++){
	if( writeByte2(buffer[i]) == -1 )
		break;
  }
  return i;
}

void breakSerial2()
{
  U2STAbits.UTXBRK = 1;
  writeByte2('U');
}

/*#########################################*/
//Crc functions

//calculates crc for an array of a given length (count).
//requires address of integer variable to return crc value into
//crc integer variable should be set to 0 prior to 
//calling unless continuing a previous sequence
void crcCalc(const unsigned char *data, int count, int *crc)
 {
  unsigned char is_odd = 0;
  int i, cr;
  unsigned char *p;
  
  p = (unsigned char *)&CRCDAT; 
  CRCWDAT = *crc;      // Init. value
    
  if ((count % 2) != 0){
	is_odd = 1;
	count--;  // Make the byte count an even number
  }
  
  while(count > 0){
	*p = *data++;  //load data by 8 bit pointer
	count--;

	if(CRCCONbits.CRCFUL == 1){
		CRCCONbits.CRCGO = 1;  //start engine
	}
  }

  if(CRCCONbits.CRCGO != 1)
	  CRCCONbits.CRCGO = 1;  //start engine

  if(is_odd){
	*p = *data;
	*p = 0; //pad to fill in other 8 bits in register
  }

  else{
	CRCDAT=0x0000; //flush with 0 if bytes used is even
  }

  while(CRCCONbits.CRCMPT != 1) //wait until buffer empty
	Nop();
  CRCCONbits.CRCGO = 0;  //stop engine

  Nop();Nop();Nop();
  Nop();Nop();Nop();

  if(is_odd){ //manually shift last byte if odd number of bytes used
	for (i = 0; i < 8; i ++){
		cr = CRCWDAT & 0x8000;
		CRCWDAT <<= 1;
		if (cr != 0)
		CRCWDAT ^= 0x1021;
	}
  }
  
  //reverse order
  *crc = CRCWDAT;
  cr = *crc;
  cr = cr >> 8;
  cr = cr & 0x00FF;
  *crc = *crc << 8;

  *crc = (*crc | cr);
 }
 

