#include <xc.h>
#include "const.h"
#include "config.h"
#include "uart_init.h"
#include "init.h"
#include "modbus.h"
#include "shared.h"
#include "lampEngine.h"
//extern volatile PORTAbits_t PORTAbits __at(0x00C);
//extern volatile PORTCbits_t PORTCbits __at(0x00E);

__EEPROM_DATA(FLAG_CNT_BIT, TARGET_LIGHT_SET, TARGET_WAIT_SET, TARGET_PRESS_SET, PRESS_TIME_SET, BOUNCE_TIME_SET, WAIT_TIME_SET, LOCK_TIME_SET);
__EEPROM_DATA(ADDRES_DEVICE, BAUD_RATE_LOW, BAUD_RATE_HIGH, GAP_TIME, STEP_TIME, WAIT_TIME, 0, 0);

/*Создаем стурктуру ModBus*/
MODBUS_DATA MODBUS;

void initShared(void) {
    COMMON.FLAGbitsDI.flag = 0;
    unsigned char cnt;
    for (cnt = 0; cnt < 8; cnt++) {
        COMMON.registrTable[cnt] = eeprom_read(cnt);
    }
    COMMON.FLAGbitsDO.RESET = 1;
}

void main(void) {
    initPIC();
    initUSART();
    initShared();
    initButton();
    initLamp();
    MODBUS.addrDev = eeprom_read(8);
    MODBUS.addrDev = 1;
    MODBUS.gapTime = GAP_TIME;
    MODBUS.stpTime = STEP_TIME;
    MODBUS.secTime = WAIT_TIME;
    MODBUS.stpFunc = 0;

    INTCONbits.T0IE = 1; //Разрешаем прерывание от Таймера 0
    INTCONbits.PEIE = 1; //Разрешаем прерывания от переферии
    INTCONbits.GIE = 1; //Разрешаем все прерывания


    while (1) {
        engineModbus();

        if (MODBUS.stpTime == 0) //цикл 10мс
        {
            //PORTAbits.RA2=PORTAbita^1;
            CLRWDT();
            MODBUS.stpTime = STEP_TIME;
          engineLamp();
        }
    }
}
