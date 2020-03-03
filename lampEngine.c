#include "lampEngine.h"
#include "shared.h"
//------------------------------------------------------------------------------
// Процедуры движка лампы
//------------------------------------------------------------------------------

void initLamp(void) {
    COMMON.lightTimeCnt = COMMON.lightStepWait; //инициализируем следующий цикл
}

void engineLamp(void) { //каждые 10 мс
    checkButton(); //управление с копки
    switchModbus(); //управление удаленно (МОДБАС))    
    setLightLamp(); //управление яркостью
    secundStep(); //управление с датчика движения
}

void secundStep(void) {
    if (MODBUS.secTime) {
        MODBUS.secTime--;
    } else {
        PORTAbits.RA2 = PORTAbits.RA2^1;
        MODBUS.secTime = WAIT_TIME;
        waitMove(); //каждую сек
    }
}
//------------------------------------------------------------------------------
// Процедуры управления плавного включения - выключения лампы (света)
//------------------------------------------------------------------------------

void setLightLamp(void) {
    if (COMMON.FLAGbitsDI.SWITCH) {//свет включен SWITCH = 1
        if (COMMON.lightTargetSet == COMMON.lightCurrentSet) { //установленная == текущей
            return; // ничего не делаем
        } else {
            if (COMMON.lightTargetSet < COMMON.lightCurrentSet) {//установленная < текущей
                COMMON.lightCurrentSet = COMMON.lightTargetSet; //текущая = установленной
                CCPR1L = COMMON.lightTargetSet; //загружаем в регистр PWM
            } else {
                upLightStep(); //плавно добавляем яркость
            }
        }
    } else {//свет выключен SWITCH = 0
        COMMON.lightCurrentSet = 0; //гасим лампу (текущая яркость не меняется)
        CCPR1L = 0; //загружаем в регистр PWM
    }
}

inline void upLightStep(void) {
    if (COMMON.lightTimeCnt) { // <> 0
        COMMON.lightTimeCnt--; // считаем
    } else { // ==0 
        COMMON.lightTimeCnt = COMMON.lightStepWait; //инициализируем следующий цикл
        //lightCurrentSet++ ;    //вариант
        COMMON.lightCurrentSet = COMMON.lightCurrentSet + COMMON.lightStepSet; //добавляем шаг
        if (STATUSbits.C) {
            COMMON.lightCurrentSet = COMMON.lightTargetSet;
        } //  если перескочили, то мах =255
        CCPR1L = COMMON.lightCurrentSet; //загружаем в регистр PWM
    }
}
//------------------------------------------------------------------------------
// Процедуры управления светом от датчика движения
//------------------------------------------------------------------------------

inline void waitMove(void) {
    if (COMMON.FLAGbitsDO.MANUAL) { //ручной режим MAHUAL = 1
        return;
    } //ничего не делаем
    if (COMMON.FLAGbitsDI.LOCKUP) { //блокировка по включению LOCKUP = 1
        return; //ничего не делаем
    }
    if (COMMON.FLAGbitsDI.LOCKDN) { //блокировка по выключению LOCKDN = 1
        if (COMMON.waitTimeCnt) { //ждем окончания интервала
            COMMON.waitTimeCnt--;
        } else {
            COMMON.FLAGbitsDI.LOCKDN = 0; //досчитали => разблокировали
        }
        return;
    }
    //Автоматизация всегда или сейчас ночь?
    if (COMMON.FLAGbitsDO.ALWAYS || COMMON.FLAGbitsDO.NIGHT) {
        if (!PORT_ACT) { //Есть движение?
            //ДА! -> Запускаем счетчик
            COMMON.waitTimeCnt = COMMON.moveTimeWait; //инициируем счетчик
            COMMON.FLAGbitsDI.SWITCH = 1; //включаем свет
            PORTAbits.RA2 = 1;
            return;
        }
        //НЕТ! -> Если интервал не закончился -> Считаем
        if (COMMON.waitTimeCnt) {
            COMMON.waitTimeCnt--;
        } else {
            //Досчитали -> Выключаем свет.
            COMMON.FLAGbitsDI.SWITCH = 0; //включаем свет
            PORTAbits.RA2 = 0;
        }
    }
}

//------------------------------------------------------------------------------
// Процедуры ручного управления светом кнопка или МОДБАС
//------------------------------------------------------------------------------

void switchON(void) {
    COMMON.FLAGbitsDI.SWITCH = 1; //включаем SWITCH => 1
    COMMON.FLAGbitsDI.LOCKUP = 1; //фиксируем включение LOCKUP => 1
    COMMON.FLAGbitsDI.LOCKDN = 0;
}

void switchOFF(void) {
    COMMON.FLAGbitsDI.SWITCH = 0; //выключаем SWITCH => 0
    COMMON.FLAGbitsDI.LOCKDN = 1; //фиксируем выключение LOCKDN => 1
    COMMON.FLAGbitsDI.LOCKUP = 0;
    COMMON.waitTimeCnt = COMMON.lockDnTimeWait; //время фиксации выключения
}
//------------------------------------------------------------------------------

void quickPress(void) {

    if (COMMON.FLAGbitsDI.SWITCH) { //свет включен = SWITCH=1
        if (COMMON.FLAGbitsDI.LOCKUP) { //второе нажатие LOCKUP = 1
            switchOFF(); //выключаем свет
        } else { //первое нажатие LOCKUP = 0
            switchON(); //включаем (фиксируем включение) 
        }
    } else { //свет выключен = SWITCH=0
        switchON(); //включаем + фиксируем включение
    }
}

void longPress(void) {
    if (COMMON.FLAGbitsDI.SWITCH) { //свет включен = SWITCH=1
        //циклически добавляем шаг яркости
        COMMON.lightTargetSet = COMMON.lightTargetSet + COMMON.lightStepSet;
        if (STATUSbits.C) {
            COMMON.lightTargetSet = 0;
        }
    } else { //свет выключен = SWITCH=0
        COMMON.lightTargetSet = 255; //устанавливаем максимум яркости
    }
}

//------------------------------------------------------------------------------

void switchModbus(void) {
    if (COMMON.FLAGbitsDO.ON) {
        COMMON.FLAGbitsDO.ON = 0;
        switchON();
        return;
    }
    if (COMMON.FLAGbitsDO.OFF) {
        COMMON.FLAGbitsDO.OFF = 0;
        switchOFF();
        return;
    }
}