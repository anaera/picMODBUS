/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */
  
#ifndef CONST_H
#define	CONST_H

#include <xc.h>

#define _XTAL_FREQ      16000000L
#define FOSC_UART        16000000L

#define CHIP_1823

//Константы конфигурации устройства
#define BAUD_RATE    9600L  //Скорость UART modbus устройства
#define BAUD_RATE_INT   416 //0x01A0-> 0x01=1 0xA0=160 256+160=416 
#define  DELAY_CALC() 10*MOD_SYMBOL*FOSC_UART/(16000L*BAUD_RATE)

#define ADDRES_DEVICE   1   //Сетевой адрес modbus устройства
#define BAUD_RATE_HIGH  1   //Старший байт скорости UART modbus устройства
#define BAUD_RATE_LOW   160 //Младший байт скорости UART modbus устройства
//Интервалы заддаются в тиках таймера TO равного 1-ой миллисекунде
#define GAP_TIME 4           // 4 МС -задержка от 4 до 5 мс
#define STEP_TIME 10          // 10МС
#define WAIT_TIME 100       // 1C

//Константы конфигурации регистров устройства
//Для 8-битных контролеров для адресации целесообразно взять один байт,
//что позволит адресовать 256 аналоговых регистров, ALL_REG=AI_REG+AO_REG<256.
//Будем считать что этого достаточно.Пусть Пусть часть аналоговых регистров
//перекрываются с цифровыми.
//Битовых регистров (DI и DO)в устройстве до 256 штук (максимально) с адресами от 0 до 255).
//Они перекрывают байтовые регистры (AI и AO) с адресами 0 до 65 (максимально).
//Регистров (АI + АO)в устройстве так же может быть до 256 штук (максимально).
//  "++++++++ ++++++++ ++++++++ 
//  "-------- -------- -------- -------- -------- -------- -------- --------"
//Регистры (АI и АO) размером один байт. 

//В пакете MODBUS может быть 4 аналоговых регистра (8байт) или 8 цифровых (1байт) 
#define DI_REG  8
#define DO_REG  24  //8*3=24
#define AI_REG  8
#define AO_REG  8
#define ALL_REG  AO_REG+AI_REG  //Общее количество аналоговых регистров устройства

//Константы конфигурации пакета протокола устройства
#define AREG_CNT    4   //Количество аналоговых регистров в пакете ограничим -4
#define DREG_CNT    8   //Количество цифровых регистров в пакете ограничим -8
#define CTL_PACK_SIZE   9   //кол-во байт пакета без данных(см.presetMultipleRegisters16)
#define ERR_PACK_SIZE   5       //Количество байт пакета ошибки Modbus
//#define DAT_PACK_SIZE   2*REGISTER_IN_PACK //Количество байт данных в пакете Modbus
#define MAX_PACK_SIZE CTL_PACK_SIZE+2*AREG_CNT //Максимальный размер в байтах пакета Modbus

//время и задержки определяемые в интервалах времени STEP_TIME = 10мс
#define FLAG_CNT_BIT        1   //Флаги управления устройством
#define TARGET_LIGHT_SET    255 //значение (целевой) конечной яркости
#define TARGET_WAIT_SET     10  //шагов задержки одного приращения яркости
#define TARGET_PRESS_SET    25  //уровней добавки яркости на один клик кнопки
#define PRESS_TIME_SET      60 //время отсечки короткое/длинное нажатие
#define BOUNCE_TIME_SET     6  // время стабилизации нажатия (в течении которого не изменяется ПИН)

//ремя и задержки определяемые в интервалах времени SEC_TIME = 1с
#define LOCK_TIME_SET       6 //время блокировки включения лампы по датчику
#define WAIT_TIME_SET       24 //время свечения лампы после активации по датчику 

#ifdef CHIP_1822
#define TRIS_TxE    TRISAbits.TRISA0
#define TRIS_BTN    TRISAbits.TRISA3
#define TRIS_PWM    TRISAbits.TRISA2
#define TRIS_ACT    TRISAbits.TRISA1
#define TRIS_TxD    TRISAbits.TRISA4
#define TRIS_RxD    TRISAbits.TRISA5
#define PORT_TxE    PORTAbits.RA0
#define PORT_BTN    PORTAbits.RA3
#define PORT_PWM    PORTAbits.RA2
#define PORT_ACT    PORTAbits.RA1
#define PORT_TxD    PORTAbits.RA4
#define PORT_RxD    PORTAbits.RA5
#endif

#ifdef CHIP_1823
#define TRIS_ACT    TRISCbits.TRISC0
#define TRIS_BTN    TRISCbits.TRISC1
#define TRIS_PWM    TRISCbits.TRISC2
#define TRIS_TxE    TRISCbits.TRISC3
#define TRIS_TxD    TRISCbits.TRISC4
#define TRIS_RxD    TRISCbits.TRISC5
#define PORT_ACT    PORTCbits.RC0
#define PORT_BTN    PORTCbits.RC1
#define PORT_PWM    PORTCbits.RC2
#define PORT_TxE    PORTCbits.RC3
#define PORT_TxD    PORTCbits.RC4
#define PORT_RxD    PORTCbits.RC5
#endif

#define SND    1
#define RCV    0

// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#endif

