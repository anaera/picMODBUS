/*
 *Функции нициализации МК
 */
#include <xc.h> 
#include "init.h"
#include "modbus.h"
//------------------------------------------------------------------------------

void initPIC(void) {
    initCPU();
    initPORTA();
    initPORTC();
    initTMR0();
    initPWM();
    initBTN();
    initWDT();
}
//------------------------------------------------------------------------------

void initCPU(void) {
    //init_OSC
    OSCCONbits.SCS = 3;
    OSCCONbits.IRCF = 15;
    OSCCONbits.SPLLEN = 0;
}
//------------------------------------------------------------------------------

void initPORTA(void) {
    //init_PORTA
    ANSELA = 0; //digital I/O      
    TRISA = 0; //all pins out
    LATA = 0; //all pins set 0
    WPUA = 0; //
}
//------------------------------------------------------------------------------

void initPORTC(void) {
#ifdef CHIP_1823
    //init_PORTC
    ANSELC = 0; //digital I/O
    TRISC = 0; //all pins out
    PORTC = 0; //all pins set 0
#endif
}
//------------------------------------------------------------------------------

void initTMR0(void) {
    //init_TMR0
    /*Настроим таймер 0 для обеспечения системного тика времени*/
    OPTION_REG = 0b10000011; //Прескалер 1:16, тактирование Fosc/4,
    //подтягивающие резисторы отключены
    //один тик 4000000/256 = 0,064мкс (us)
}
//------------------------------------------------------------------------------

void initPWM(void) {
    //init_PORT_PWM
    TRIS_PWM = 0; // TRISxbits.TRISCx = 0 output port
    //Начальные значения
    PORT_PWM = 0; //PORTxbits.Rxx = 0; //PWM = 0
    //init_PWM
    T2CONbits.TMR2ON = 0; //останавливаем PWM  
    PR2 = 255; //частота 1,9-1,2 кгц - разряддность 10
    //Single Output PWM (CCP1M<3:2>=11 & P1M<1:0>=00 of the CCP1CON register)
    CCP1CON = 0x0C;
#ifdef CHIP_1823 
    PSTR1CON = 0;
    PSTR1CONbits.STR1D = 1; //PWM-> active P1Dpin on RC2
    PSTR1CONbits.STR1SYNC = 1;
#endif        
    T2CONbits.T2CKPS = 2; //предделитель на 16
    CCPR1L = 0; //старшие  биты ->0
    T2CONbits.TMR2ON = 1; //пуск PWM
}
//------------------------------------------------------------------------------

void initBTN(void) {
    //init_PORT_BTN
    TRIS_BTN = 1; // TRISxbits.TRISCx = 1   input port
    TRIS_ACT = 1;
}
//------------------------------------------------------------------------------

void initWDT(void) {
    //init_WDT
    WDTCON = 0b00010010; //512ms
    //        WDTCONbits.SWDTEN=1;
}
