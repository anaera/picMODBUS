#include <xc.inc>
    global		_checkButton, _initButton
    global		_quickPress, _longPress
    global		_COMMON
//	global	    bounceTimeSet,pressTimeSet

//--------------------------------------------------------------------------
// Если флаг установлен и совпадает 
// с состоянием кнопки то идем на увеличение счетчика удержания
// Если флаг сброшен и совпадает с состоянием кнопки и время
// удержания отлично от нуля - обрабатываем цикл нажате - отжатие
// Если флаг установлен и не совпадает  с состоянием кнопки 
// то начинаем цикл переходного процеса смены состояния кнопки -
// уменьшение счетчика времени установления нового состояния.
// Если удается досчитать до конца то идем на смену состония флага
// - далее состояние флага совпадает с состоянием кнопки и
// начинается один из циклов обработки нажатия-удержания
// ( подсчет времени) или отжатия - смены параметра.
//--------------------------------------------------------------------------
#define	    PORT_BT	14  //14- portC 12 - portA
#define	    PIN_BT	1
#define	    FLAG_BT	0

    psect	asm_data,global,class=BANK0,space=1,delta=1

_COMMON:
	   	DS  1;	// -1
		DS  1;	// -2 целевая яркость лампы
		DS  1;	// -3 шагов задержки одного приращения яркости
		DS  1;	// -4 шагов добавки яркости на один клик кнопки
pressSetTime:	DS  1;	// -5 время короткое/длинное нажатие
bounceSetTime:	DS  1;	// -6 шагов задержки дребезга (время между опросами)
		DS  1;	// -7 время таймаута датчика движения

		DS  1;	// -8 время таймаута блокировки режима AUTO
FLAGAbits:	DS  1;	// -9 

		DS  1;	// -10 текущая яркость лампы
		DS  1;	// -11 счетчик текущего таймаута
		DS  1;	// -12 счетчик текущей задержки яркости
bounceTimeCnt:	DS  1;	// -13 счетчик задержки дребезга
pressTimeCnt:	DS  1;	// -14 счетчик удержания кнопки короткое/длинное
		DS  1;	// -15
		DS  1;	// -16
	
	
	psect   asm_func,local,class=CODE, merge=1, delta=2 
_checkButton:
    banksel 0
//обнаруживаем нажатие и отсчитываем время дребезга
labON_up:
    btfss	FLAGAbits,FLAG_BT
    btfsc	PORT_BT,PIN_BT
    goto	labOFF_up
    decfsz	bounceTimeCnt,F
    return

    bsf	FLAGAbits,FLAG_BT	//принято,состяние сменилось на 1- нажата
    movf	bounceSetTime,W	//инициируем время дребезга отжатия
    movwf	bounceTimeCnt
    return
	
labOFF_up:
    btfsc	FLAGAbits,FLAG_BT	//состояние флага - удержание ->2
    btfss	PORT_BT,PIN_BT	//1-состояния различны - есть отжатие
    goto	labSET_up	//2-идем на обработку нулевого флага (нажатие завершено)
    decfsz	bounceTimeCnt,F	//3-кн отжата постоянно уменьшаем счетчик удержания	
    return			//ждем дальше
				//4-без дребезга, достаточно для смены состояния	
    bcf	FLAGAbits,FLAG_BT	//принято,состяние сменилось на 1- отжата
    movf	bounceSetTime,W
    movwf	bounceTimeCnt
    return
labSET_up:
    btfss	FLAGAbits,FLAG_BT	//если флаг PinUP установлен
    goto	labEXIT_up	//если флаг PinUP сброшен (отпустили кн)
//bsf	PORT_BT,0
    incfsz	pressTimeCnt,F	//добавляем (количество циклов удержания)
    return
    decf	pressTimeCnt,F
    return
	
//кнопку отпустили TimePress <> 0(она отпущена и была - TimePress = 0) 
labEXIT_up:
//bcf	PORT_BT,0
    movf	pressTimeCnt,F	//если время нажатия <> 0
		// идем на обработку нажатия (анализ длительности)
    btfsc	STATUS,STATUS_Z_POSITION
    goto	_initButton	//==0, то начинаем новый цикл

//анализ нажатия нопки по времени
// короткое нажатие при включеном CPC_BIT - выключить CPC_BIT
// короткое нажатие при выключеном CPC_BIT - включить CPC_BIT
//длинное нажатие при включеном CPC_BIT - Добавить 25ед яркости
//длинное нажатие при выключеном CPC_BIT - сбросить PosReg=0 
    movf	pressSetTime,W	//зацениваем время, короткое/длинное
    subwf	pressTimeCnt,W	
    btfsc	STATUS,STATUS_C_POSITION	//
    goto	lab_long
    call	_quickPress	//короткое нажатие
    goto	_initButton
lab_long:
    call	_longPress	//длинное нажатие	
//--------------------------------------------------------------------------			    
_initButton:			//инициализация нового цикла
    clrf	pressTimeCnt
    movf	bounceSetTime,W
    movwf	bounceTimeCnt
    return






