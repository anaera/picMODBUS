#include <xc.h>
#include "modbus.h"
#include "const.h"
//------------------------------------------------------------------------------
//Цикл работы протокольной части программы приема обратоки отсылки пакетов
//------------------------------------------------------------------------------
//
//0-Инициация режима приема очередного пакета МОДБАС
//  void toReceiveStepHandler(void)
//1-Ожиданиа завершения приема/передачи очередного пакета МОДБАС
//  void toIdleStepHandler(void) - Прием пакета МОДБАС
//2-Обработка принятого пакета МПДБАС и формирование ответа или пакета ошибки
//  void toSlaveStepHandler(void)
//3-Инициация режима передачи пакета ответа МОДБАС
//  void startSendFrame();
//4-Ожиданиа завершения приема/передачи очередного пакета МОДБАС
//  void toIdleStepHandler(void) - Передача пакета МОДБАС
//5-Завершение передачи (ожидание освобождения буфера передачи USART)
//  void toFinishSendHandler(void)
//6-Инициация режима приема очередного пакета МОДБАС
//  void toReceiveStepHandler(void) 
//далее пункт 1
//
//------------------------------------------------------------------------------

void engineModbus(void) {
    switch (MODBUS.stpFunc) {
        case(0):
        {
            toFinishSendHandler();
            break;
        }
        case(1):
        {
            toIdleStepHandler();
            break;
        }
        case(2):
        {
            toSlaveStepHandler();
            break;
        }
    }
}
//------------------------------------------------------------------------------
// Ожиданиа завершения приема/передачи очередного пакета МОДБАС
//------------------------------------------------------------------------------

inline void toIdleStepHandler(void) {
    NOP();
}
//------------------------------------------------------------------------------
// Функция обработки модбас фрейма
//------------------------------------------------------------------------------

inline void toSlaveStepHandler(void) {
    unsigned int crc;
    //   unsigned char tmp;
    /*если обращаются к нам или широковещательныйм адресом, и в буфере больше 5 байт*/
    if ((MODBUS.buffer[0] == MODBUS.addrDev) || (MODBUS.buffer[0] == 255)) {
        /*проверяем контрольную сумму*/
        crc = countFrameCRC16(MODBUS.buffer, MODBUS.cntByte - 2);
        /*если контрольная сумма совпала, обрабатываем сообщение*/
        if ((MODBUS.buffer[MODBUS.cntByte - 2] == (crc & 0x00FF)) && (MODBUS.buffer[MODBUS.cntByte - 1] == (crc >> 8))) {
            //обрабатываем прием и формируем ответ
            switch (MODBUS.buffer[1]) {
                case 1: //Чтение дискретного вывода
                    readCoilStatus01();
                    break;
                case 2: //Чтение дискретного ввода
                    readInputStatus02();
                    break;
                case 3: //Чтение нескольких аналоговоых выводов
                    readHoldingRegisters03();
                    break;
                case 4: //Чтение аналогового ввода
                    readInputRegisters04();
                    break;
                case 5: //Запись дискретного вывода
                    forceSingleCoil05();
                    break;
                case 6: //Запись одного аналогового вывода
                    presetSingleRegister06();
                    break;
                case 15: //Запись нескольких дискретных выводов
                    forceMultipleCoils15();
                    break;
                case 16: //Запись нескольких аналоговоых выводов
                    presetMultipleRegisters16();
                    break;
                default: //Пришла команда не поддерживаемая, отвечаем ошибкой
                    makeErrorFrame(0x01);
                    break;
            }
            //     добавляем контрольную сумму
            crc = countFrameCRC16(MODBUS.buffer, MODBUS.sendLen - 2);
            MODBUS.buffer[MODBUS.sendLen - 2] = crc;
            MODBUS.buffer[MODBUS.sendLen - 1] = crc >> 8;
            //  Инициация режима передачи пакета ответа МОДБАС
            MODBUS.cntByte = 0;
            MODBUS.stpFunc = 1;
            PORT_TxE = SND; //разрешенин передачи modbus
            TXSTAbits.TXEN = 1; //разрешение передачи (надо ли дергать??)
            PIE1bits.TXIE = 1; //разрешаем прерывание передачи
        }
    }
}
//------------------------------------------------------------------------------
// Завершение передачи (ожидание освобождения буфера передачи USART)
//------------------------------------------------------------------------------

inline void toFinishSendHandler(void) {
    //ждем пока последний байт выйдет из сдвигового регистра передатчика USART
    while (!TXSTAbits.TRMT) NOP();
    //переходим на прием
    // Инициация режима приема очередного пакета МОДБАС
    TXSTAbits.TXEN = 0; //запрещение передачи (надо ли дергать??)
    MODBUS.cntByte = 0; //указатель установить на начало пакета
    MODBUS.gapTime = GAP_TIME; //инициализируем интервал молчания
    MODBUS.stpFunc = 1;
    PORT_TxE = RCV; //шину МОДБАС переводим на прием   
    RCSTAbits.CREN = 1; //разрешение приема (надо ли дергать??) похоже да!
    PIE1bits.RCIE = 1; //разрешаем прерывание приема
}
//------------------------------------------------------------------------------
// Расчета контрольной суммы
//------------------------------------------------------------------------------

unsigned int countFrameCRC16(volatile unsigned char *ptrByte, int byte_cnt) {
    unsigned int w = 0;
    char shift_cnt;
    if (ptrByte) {
        w = 0xffffU;
        for (; byte_cnt > 0; byte_cnt--) {
            w = (unsigned int) ((w / 256U)*256U + ((w % 256U)^(*ptrByte++)));
            for (shift_cnt = 0; shift_cnt < 8; shift_cnt++) {
                if ((w & 0x1) == 1)
                    w = (unsigned int) ((w >> 1)^0xa001U);
                else
                    w >>= 1;
            }
        }
    }
    return w;
}

//------------------------------------------------------------------------------
//  Фукция формирования ошибки
//------------------------------------------------------------------------------

void makeErrorFrame(unsigned char error_type) {
    MODBUS.buffer[1] |= 128; //Отправляем ошибку
    MODBUS.buffer[2] = error_type; //код ошибки
    MODBUS.sendLen = ERR_PACK_SIZE; //длина пакета ошибки
}



