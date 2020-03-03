/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef INIT_H
#define	INIT_H
#include <xc.h>

void initPIC(void);
void initWDT(void);
void initPWM(void);
void initBTN(void);
void initPORTA(void);
void initPORTC(void);
void initCPU(void);
void initTMR0(void);
#endif	/* INIT_H */



