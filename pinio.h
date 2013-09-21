#ifndef PINIO_H
#define PINIO_H

int sendRfBuf(const char *buf, const int count);

int readRfBuf(char *buffer, const int count);

#endif

/*
logic flow:
sendRfBuf() takes a pointer to a buffer and an amount to send. sendRfBuf() calls sendRfByte()
which in turn analyzes the lsb of the byte to determine if it is a one or not. The pulseMod75() and pulseMod25()
functions are then called to hold the chosen pin at a 25% or 75% duty cycle. The pulseModxx() functions set a flag 
when the chosen time limit is reached. This flag is checked to determine how long to hold the pin for. At the end
of this cycle the byte is shifted to the right to expose the next lsb and this cycle is repeated 7 more times.

readRfBuf() takes a pointer to a buffer and an amount to read. readRfBuf() calls readRfByte() for each byte 
that is read. readRfByte() uses the enableRisingEdgeDetc() and enableFallingEdgeDetc() functions to enable interrupts
once a changing condition is noticed on the chosen pin. These interrupts set a flag, waitHigh or waitLow.
The flags are monitored and once set a timer is started to measure the time to the next pin change condition.
The timer times are stored in highTime and lowTime variables. Comparing these times are used to write a 1 or a 0
to the msb of the byte. The byte is then shifted right and this process is repeated  7 more times.


*/
