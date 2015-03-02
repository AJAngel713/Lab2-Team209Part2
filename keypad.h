/* 
 * File:   keypad.h
 * Author: user
 *
 * Created on February 5, 2015, 11:42 AM
 */

#ifndef KEYPAD_H
#define	KEYPAD_H

/*
 * Defines for use in keypad.c
 */
#define INPUT 1
#define OUTPUT 0
#define ROWS 4
#define COLUMNS 3

// ROWS   --------------------------- WRITE ONLY!!!!!
#define ROW1 LATAbits.LATA0 // Pin 2
#define ROW2 LATAbits.LATA1 // Pin 3
#define ROW3 LATBbits.LATB2 // Pin 6
#define ROW4 LATBbits.LATB3 // Pin 7

//COLUMNS --------------------------- READ ONLY!!!!!
#define COL1 PORTBbits.RB9  // Pin 18
#define COL2 PORTBbits.RB10 // Pin 21
#define COL3 PORTBbits.RB11 // Pin 22


void initKeypad(void);
char scanKeypad(void);



#endif	/* KEYPAD_H */

