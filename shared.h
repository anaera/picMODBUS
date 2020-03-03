#ifndef SHARED_H
#define	SHARED_H
#include "const.h"

typedef union {
    unsigned char flag;
    struct {
        unsigned MANUAL : 1;        //младший бит
        unsigned ALWAYS : 1;
        unsigned NIGHT : 1;
        unsigned ON : 1;        //старший бит
        unsigned OFF:1;
        unsigned RESET:1;
    };
} FLAGAbits_tw;
typedef union {
    unsigned char flag;
    struct {
        unsigned BUTTON : 1;    //младший бит
        unsigned SWITCH : 1;        
        unsigned LOCKUP : 1;
        unsigned LOCKDN : 1;      //старший бит
    };
} FLAGAbits_tr;
union {
    unsigned char registrTable[ALL_REG];
    struct {
        FLAGAbits_tw FLAGbitsDO; // -1
        unsigned char lightTargetSet; // целевая яркость лампы -3
        unsigned char lightStepWait; //шагов задержки одного приращения яркости
        unsigned char lightStepSet; //шагов добавки яркости на один клик кнопки
        unsigned char pressTimeSet; //время короткое/длинное нажатие
        unsigned char bounceTimeSet; //шагов задержки дребезга (время между опросами)
        unsigned char moveTimeWait; //время таймаута датчика движения
        unsigned char lockDnTimeWait; //время таймаута блокировки режима AUTO
        FLAGAbits_tr FLAGbitsDI; // -2
        unsigned char lightCurrentSet; //текущая яркость лампы
        unsigned char waitTimeCnt; //счетчик текущего таймаута
        unsigned char lightTimeCnt; //счетчик текущей задержки яркости
        unsigned char bounceTimeCnt; //счетчик задержки дребезга
        unsigned char pressTimeCount; //счетчик удержания кнопки
        unsigned char reserveByte1;
        unsigned char reserveByte2;
    };
}volatile extern COMMON;

#include <xc.h>
#endif