/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LAMP_ENGINE_H
#define LAMP_ENGINE_H

#include <xc.h>
#include "const.h"
#include "modbus.h"

void initLamp(void);
void engineLamp(void);
void switchModbus(void);
void setLightLamp(void);
inline void upLightStep(void);
void secundStep(void);
inline void waitMove(void);
void longPress(void);
void quickPress(void);
//void switchON(void);
//void switchOFF(void);

#endif

