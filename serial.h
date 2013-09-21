#ifndef SERIAL_H
#define SERIAL_H

void crcCalc(const unsigned char *buf,int count, int *crc);

int readByte1(unsigned char *byte);

int writeByte1(const unsigned char byte);

int readSerial1(unsigned char *buffer, const int count);

int writeSerial1(const unsigned char *buffer, const int count);

int readByte2(unsigned char *byte);

int writeByte2(const unsigned char byte);

int readSerial2(unsigned char *buffer, const int count);

int writeSerial2(const unsigned char *buffer, const int count);

void breakSerial1();

void breakSerial2();


#endif
