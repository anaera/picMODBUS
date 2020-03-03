#ifndef MODBUS_H
#define	MODBUS_H
#include "const.h"
/*Определим UART структура Modbus*/
typedef struct {
    volatile unsigned char stpFunc;
    volatile unsigned char gapTime; //счетчик защитного интервала
    volatile unsigned char stpTime; //счетчик 10мс
    volatile unsigned char secTime; //счетчик 1 сек
    volatile unsigned char cntByte; //счетчик байт приема - передачи
    volatile unsigned char sendLen; //размер отсылаемого пакета
    volatile unsigned char addrDev; //адрес устройства
    volatile unsigned char buffer[MAX_PACK_SIZE + 1]; //буфер приема/отправки пакета
} MODBUS_DATA;

MODBUS_DATA MODBUS;

/*Объявление прототипов функции*/


void readCoilStatus01(void);
void readInputStatus02(void);
void readDigitalReg(char,char,char);
void readHoldingRegisters03(void);
void readInputRegisters04(void);
void readAnalogReg(char,char,char);
void forceSingleCoil05(void);
void presetSingleRegister06(void);
void forceMultipleCoils15(void);
void presetMultipleRegisters16(void);
unsigned int countFrameCRC16(volatile unsigned char *ptrByte, int byte_cnt);

void makeErrorFrame(unsigned char error_type);
void startSendFrame(void);

//  Section: MODBUS  APIs
//void (*doStepHandler)(void);
void engineModbus(void);
inline void toIdleStepHandler(void);
inline void toSlaveStepHandler(void);
inline void toFinishSendHandler(void);

extern void checkButton(void);
extern void initButton(void);

#endif

