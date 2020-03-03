/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef TYPE_DEF_H
#define	TYPE_DEF_H

#include <xc.h>

typedef union {
    char Val;

    struct {
        char b0 : 1;
        char b1 : 1;
        char b2 : 1;
        char b3 : 1;
        char b4 : 1;
        char b5 : 1;
        char b6 : 1;
        char b7 : 1;
    } bits;
} BYTE_VAL, BYTE_BITS;

typedef union {
    unsigned int Val;
    char v[2];

    struct {
        char LB;
        char HB;
    } byte;

    struct __PACKED {
        char b0 : 1;
        char b1 : 1;
        char b2 : 1;
        char b3 : 1;
        char b4 : 1;
        char b5 : 1;
        char b6 : 1;
        char b7 : 1;
        char b8 : 1;
        char b9 : 1;
        char b10 : 1;
        char b11 : 1;
        char b12 : 1;
        char b13 : 1;
        char b14 : 1;
        char b15 : 1;
    } bits;
} WORD_VAL, WORD_BITS;

#endif	/* TYPE_DEF_H */



