/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */
 
#ifndef UART_INIT_H
#define	UART_INIT_H

#include <xc.h>
#include "type_def.h"

/*Определяем констат скорости UART*/
#define BAUD_CALC    ((FOSC_UART-4L*BAUD_RATE)/4L*BAUD_RATE)

void initUSART(void);

#endif	/* UART_INIT_H */

