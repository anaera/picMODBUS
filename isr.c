#include "const.h"
#include "modbus.h"
#include "isr.h"
//------------------------------------------------------------------------------
// Обработчик прерываний
//------------------------------------------------------------------------------
//  Поскольку неизвестна длина принимаемого пакета данных МОДБАСА,
// окончание пакета определяется отсутствием новых данных RxUSART в течении
//  интервала GAPTIME, который считается в переменной MODBUS.gapTime
// от GAPTIME до нуля.

void __interrupt() isr(void) {
    //------------------------------------------------------------------------------
    //Обработка прерывания от таймера 0, для формирования системных тиков
    if (INTCONbits.T0IF && INTCONbits.T0IE) {
        TMR0 = 6;
        if (MODBUS.stpTime > 0) {
            MODBUS.stpTime--;
        }
        //если находимся в режиме приема пакета(разешены прерывания PIE1bits.RCIE)
        // то каждый тик уменьшаем интервал GAP, если он уменьшился до 0,
        // и больше данных в RxUSART не поступает -> пакет принят полностью
        if (PIE1bits.RCIE) {
            if (MODBUS.gapTime > 0) {
                MODBUS.gapTime--;
            } else {
                PORTAbits.RA0 = 0;
                MODBUS.gapTime = GAP_TIME;
                if (MODBUS.cntByte > 0) { //принято что-то?
                    //принятая последовательность байт в размерах максимального пакета?
                    //                   if (MODBUS.cntByte < MAX_PACK_SIZE+1) {
                    RCSTAbits.CREN = 0; //выключаем приемник (чтоб был 0 в буф)
                    PIE1bits.RCIE = 0; //запрещаем прерывания приема
                    //идем на обработку принятого пакета в основном цикле
                    //                        doStepHandler = toSlaveStepHandler;
                    MODBUS.stpFunc = 2;
                    //PORTAbits.RA0 = 0;
                    //                    } else {
                    //НЕТ! Сбрасываем принятые данные, ждем нового приема.
                    //                        MODBUS.cntByte = 0;
                    //                   }
                }
            }
        }
        INTCONbits.T0IF = 0; //Сбрасываем флаг прерывания от таймера 0
    }
    //------------------------------------------------------------------------------
    //обработчик прерывания от приемника уарт
    //    if (INTCONbits.PEIE == 1) {
    if (PIE1bits.TXIE && PIR1bits.TXIF) {
        //следующий символ на передачу
        if (MODBUS.cntByte < MODBUS.sendLen) {
            TXREG = MODBUS.buffer[MODBUS.cntByte];
            MODBUS.cntByte++;
        }// все передано СТОП!
        else {
            PIE1bits.TXIE = 0; //запрещаем прерывание передачи
            //будем ждать освобождения буфера передачи USART
            //doStepHandler = toFinishSendHandler;
            MODBUS.stpFunc = 0;
        }
    }
    //------------------------------------------------------------------------------
    if (PIE1bits.RCIE && PIR1bits.RCIF) {
        //После приема каждого байта восстанавливаем защитный интервал приема         
        MODBUS.gapTime = GAP_TIME; //Установили интервал
        PORTAbits.RA0 = 1;
        PORTAbits.RA1 ^= 1;
        if (RCSTAbits.FERR || RCSTAbits.OERR) //Были ошибки?
        {
            //В случае ошибки приемника
            RCSTAbits.CREN = 0; //Отключаем приемник для сброса ошибки
            NOP();
            //Включаем приемник для продолжения работу УАРТ
            RCSTAbits.CREN = 1;
            //сбрасываем счетчик принятых байт
            //MODBUS.cntByte = 0;
        } else {
            while (PIR1bits.RCIF) {
                //Записываем очередной байт в пакет данных ModBus
                //Лишние данные сбрасываем
                if (MODBUS.cntByte < MAX_PACK_SIZE) {
                    MODBUS.buffer[MODBUS.cntByte] = RCREG;
                    MODBUS.cntByte++;
                }
            }
        }
    }
}
