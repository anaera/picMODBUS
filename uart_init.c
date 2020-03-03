/*
 * Подпрограмма настройки УАРТа МК
 * На вход получаем скорость в БОДах
 */
#include "const.h"
#include "type_def.h"
#include "uart_init.h"

void initUSART() {
    //Назначаем ноги Tx и Rx для USART
    APFCON = 0;
#ifdef CHIP_1822    
    APFCONbits.RXDTSEL = 1; //RxD: 0->RA1  1->RA5
    APFCONbits.TXCKSEL = 1; //TxD: 0->RA0  1->RA4
    //Направление передачи
    TRISAbits.TRISA5 = 1; //RxD -> RA5 input          
    TRISAbits.TRISA4 = 0; //TxD -> RA4 output  
    TRISAbits.TRISA0 = 0; //TxE -> RA0 output
    //Начальные значения
    PORTAbits.RA4 = 1; //TxD = 1
    PORTAbits.RA0 = 0; //TxE = 0 направление передачи - прием
#endif

#ifdef CHIP_1823    
    APFCONbits.RXDTSEL = 0; //RxD: 0->RC5  1->RA1
    APFCONbits.TXCKSEL = 0; //TxD: 0->RC4  1->RA0
    //Направление передачи
    TRIS_RxD = 1;
    TRIS_TxD = 0;
    TRIS_TxE = 0;
    /*
    TRISCbits.TRISC5 = 1; //RxD -> RC5 input          
    TRISCbits.TRISC4 = 0; //TxD -> RC4 output  
    TRISCbits.TRISC3 = 0; //TxE -> RC3 output
     */
    //Начальные значения
    PORT_TxD = 1;
    PORT_TxE = 0;
    /*
    PORTCbits.RC4 = 1; //TxD = 1
    PORTCbits.RC3 = 0; //TxE = 0 направление передачи - прием
     */
#endif    

    //SYNC=0 BRGH=1 BRG16=1   ->  Fosc/4*[N+1]    
    BAUDCON = 0;
    BAUDCONbits.BRG16 = 1; //BRG16 16-битный генератор скорости
    
//    WORD_VAL bRate;
//    bRate.Val = 416; /*Производим расчет регистров скорости передачи*/
//    SPBRGH = bRate.byte.HB; //Устанавливаем скорость
//    SPBRGL = bRate.byte.LB;
 
    SPBRGL = BAUD_RATE_LOW; //зададим скорость   
    SPBRGH = BAUD_RATE_HIGH;    //зададим скорость   
    TXSTA = 0;
    TXSTAbits.SYNC = 0; //TX Асинхронный режим SYNC
    TXSTAbits.BRGH = 1; //высокоскоростной BRGH
    //    TXSTAbits.TXEN = 1; //TX Включить TX передачу
    RCSTA = 0;
    //    RCSTAbits.CREN = 1; //CREN Включить RX прием
    RCSTAbits.SPEN = 1; //SPEN Включить УАРТ
}
