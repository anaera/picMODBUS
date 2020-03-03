#include "const.h"
#include "modbus.h"
#include "shared.h"
//------------------------------------------------------------------------------
// Функция чтении мультирегистров DO
//01 (0x01)	Чтение DO	Read Coil Status	Дискретное	Чтение
//------------------------------------------------------------------------------

void readCoilStatus01(void) {
    readDigitalReg(DREG_CNT, 0, DO_REG);
}
//------------------------------------------------------------------------------
// Функция чтении мультирегистров DI
//02 (0x02)	Чтение DI	Read Input Status	Дискретное	Чтение
//------------------------------------------------------------------------------

void readInputStatus02(void) {
    readDigitalReg(DREG_CNT, AO_REG, DI_REG);
}
//------------------------------------------------------------------------------

void readDigitalReg(char regInPack, char offInDevice, char regInDevice) {
    if (MODBUS.buffer[4] == 0) {
        if (MODBUS.buffer[2] == 0) {
            if (MODBUS.buffer[5] < regInPack + 1) {
                if ((MODBUS.buffer[3] + MODBUS.buffer[5]) < regInDevice + 1) {
                    char hiTag;
                    char loTag;
                    char off;
                    unsigned int val;
                    loTag = MODBUS.buffer[3] & 0x07; //младшая часть адреса первого регистра
                    hiTag = offInDevice + (MODBUS.buffer[3] >> 3); //старшая часть адреса первого регистра
                    val = COMMON.registrTable[hiTag + 1] << 8; //старшие биты облласти регистров
                    val = val | COMMON.registrTable[hiTag]; //младшие биты облласти регистров
                    val = val >> loTag; //смещаем к нулевому биту (вправо))
                    off = (MODBUS.buffer[5] & 0x07);
                    if (off == 0) {
                        off = 8;
                    }
                    MODBUS.buffer[3] = 0;
                    MODBUS.buffer[4] = (char) (val & (~(0xFF << off)));
                    MODBUS.buffer[2] = 2; //количество байт в пакете
                    MODBUS.sendLen = 7;
                    return;
                }
            }
        }
    }
    makeErrorFrame(0x02); //область памяти не поддерживается
    return;
}
//------------------------------------------------------------------------------
// Функция чтении мультирегистров AO
//03 (0x03)	Чтение AO	Read Holding Registers	16 битное	Чтение
//------------------------------------------------------------------------------

void readHoldingRegisters03(void) {
    readAnalogReg(AREG_CNT, 0, AO_REG);
}
//------------------------------------------------------------------------------
// Функция чтении мультирегистров AI
//04 (0x04)	Чтение AI	Read Input Registers	16 битное	Чтение
//------------------------------------------------------------------------------

void readInputRegisters04(void) {
    readAnalogReg(AREG_CNT, AO_REG, AI_REG);
}
//------------------------------------------------------------------------------

void readAnalogReg(char regInPack, char offInDevice, char regInDevice) {
    if (MODBUS.buffer[4] == 0) {
        if (MODBUS.buffer[2] == 0) {
            if (MODBUS.buffer[5] < regInPack + 1) {
                if ((MODBUS.buffer[3] + MODBUS.buffer[5]) < regInDevice + 1) {
                    char ptrPck; //номер регистра в пакете
                    char cntReg; //количество  регистров для записи/чтения
                    char tagReg; //адреес регистра в таблице
                    ptrPck = 3;
                    cntReg = MODBUS.buffer[5];
                    tagReg = MODBUS.buffer[3] + offInDevice;

                    while (cntReg) {
                        cntReg--;
                        MODBUS.buffer[ptrPck] = 0; //старший байт =0
                        ptrPck++;
                        MODBUS.buffer[ptrPck] = COMMON.registrTable[tagReg];
                        ptrPck++;
                        tagReg++;
                    }
                    MODBUS.buffer[2] = ptrPck - 0x03; //byte count
                    MODBUS.sendLen = ptrPck + 0x02; //responce length
                    return;
                }
            }
        }
    }
    makeErrorFrame(0x02); //область памяти не поддерживается
    return;
}
//------------------------------------------------------------------------------
// Функция записи одного регистра DO
//05 (0x05)	Запись одного DO	Force Single Coil	Дискретное	Запись
//------------------------------------------------------------------------------

void forceSingleCoil05(void) {
    if (MODBUS.buffer[2] == 0) {
        if (MODBUS.buffer[3] < DO_REG) {
            //в нашем адресном пространстве
            if (MODBUS.buffer[5] == 0) {
                unsigned char numBit, numByte;
                numBit = MODBUS.buffer[3]&7;
                numByte = MODBUS.buffer[3] >> 3;
                MODBUS.sendLen = 8;
                if (MODBUS.buffer[4] == 0) {
                    COMMON.registrTable[numByte] = COMMON.registrTable[numByte] & (~(1<<numBit));
                    return;
                }
                if (MODBUS.buffer[4] == 0xFF) {
                    COMMON.registrTable[numByte] = COMMON.registrTable[numByte] | (1<<numBit);
                    return;
                }
            }
            //недопустимое значение данных
            makeErrorFrame(0x03);
            return;
        }
    }
    //область памяти не поддерживается
    makeErrorFrame(0x02); //область памяти не поддерживается
}
//------------------------------------------------------------------------------
// Функция записи одного регистра AO
//06 (0x06)	Запись одного AO	Preset Single Register	16 битное	Запись
//------------------------------------------------------------------------------

void presetSingleRegister06(void) {
    if (MODBUS.buffer[2] == 0) {
        if (MODBUS.buffer[3] < AO_REG) {
            //в нашем адресном пространстве
            if (MODBUS.buffer[4] == 0) {
                //допустимые данные -> записать по адресу регистра
                COMMON.registrTable[MODBUS.buffer[3]] = MODBUS.buffer[5];
                MODBUS.sendLen = 8;
                return;
            } else {
                //недопустимое значение данных
                makeErrorFrame(0x03);
                return;
            }
        }
    }
    //область памяти не поддерживается
    makeErrorFrame(0x02);
    return;
}
//------------------------------------------------------------------------------
// Функция записи мультирегистров DO
//15 (0x0F)	Запись нескольких DO	Force Multiple Coils	Дискретное	Запись
//------------------------------------------------------------------------------

void forceMultipleCoils15(void) {
    if (MODBUS.buffer[4] == 0) {
        if (MODBUS.buffer[2] == 0) {
            if (MODBUS.buffer[5] < DREG_CNT + 1) {
                if ((MODBUS.buffer[3] + MODBUS.buffer[5]) < DO_REG + 1) {
                    char hiTag;
                    char loTag;
                    //                    unsigned int val;
                    //                    unsigned int mask;
                    unsigned int tag;

                    loTag = MODBUS.buffer[3] & 0x07; //младшая часть адреса первого регистра
 //                   if (loTag == 0) {
   //                     loTag = 8;
     //               }
                    //                    mask = ((~(0xFFFF << MODBUS.buffer[5])) << loTag);
                    //                    mask = mask<<loTag;
                    //                    val = MODBUS.buffer[7] << loTag;
                    //                    val = val<<loTag;

                    hiTag = MODBUS.buffer[3] >> 3; //старшая часть адреса первого регистра                    
                    tag = COMMON.registrTable[hiTag + 1]; //старшие биты области регистров
                    tag = tag << 8;
                    tag = tag + COMMON.registrTable[hiTag]; //младшие биты области регистров

                    //                    tag = (tag & (~mask)) | val;
                    tag = (tag & (~((~(0xFFFF << MODBUS.buffer[5])) << loTag))) | (MODBUS.buffer[7] << loTag);
                    COMMON.registrTable[hiTag + 1] = (unsigned char) (tag >> 8);
                    COMMON.registrTable[hiTag] = (unsigned char) tag;

                    MODBUS.sendLen = 8;
                    return;
                }
            }
        }
    }
    makeErrorFrame(0x02); //область памяти не поддерживается
    return;
}
//------------------------------------------------------------------------------
// Функция записи мультирегистров AO
//16 (0x10)	Запись нескольких AO	Preset Multiple Registers	16 битное	Запись
//------------------------------------------------------------------------------

void presetMultipleRegisters16(void) {
    if (MODBUS.buffer[4] == 0) {
        if (MODBUS.buffer[2] == 0) {
            if (MODBUS.buffer[5] < AREG_CNT + 1) {
                if ((MODBUS.buffer[3] + MODBUS.buffer[5]) < AO_REG + 1) {
                    //                   char ptrTab;
                    char ptrPck; //номер регистра в пакете
                    char cntReg; //количество  регистров для записи/чтения
                    char tagReg; //адреес регистра в таблице
                    tagReg = MODBUS.buffer[3];
                    ptrPck = 7; //первый байт данных в пакете

                    for (cntReg = MODBUS.buffer[5]; cntReg>0; cntReg--){
                          if (MODBUS.buffer[ptrPck] == 0) {
                            ptrPck++;
                           COMMON.registrTable[tagReg] = MODBUS.buffer[ptrPck];
                           ptrPck++;
                           tagReg++;
                        } else {
                           makeErrorFrame(0x03); //Недопустимое значение  данных
                            return;
                        }
                    }
                    MODBUS.sendLen = 8;
                    return;
                }
            }
        }
    }
    makeErrorFrame(0x02); //область памяти не поддерживается
    return;
}
