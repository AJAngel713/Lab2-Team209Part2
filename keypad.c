#include "p24FJ64GA002.h"
#include "keypad.h"
#include "timer.h"

/* Initialize the rows as ODC outputs and the columns as inputs with pull-up
 * resistors. Don't forget about other considerations...
 */
void initKeypad(void){

    /*
     Inputs: RB9/Pin 18, RB10/Pin 21, RB11/Pin 22
     Ouputs: RA0/Pin 2, RA1/Pin 3, RB2/Pin 6, RB3/Pin 7
     */

    // ROWS
    TRISAbits.TRISA0 = OUTPUT; // row 1, output. Pin 2
    TRISAbits.TRISA1 = OUTPUT; // row 2, output. Pin 3
    TRISBbits.TRISB2 = OUTPUT; // row 3, output. Pin 6
    TRISBbits.TRISB3 = OUTPUT; // row 4, output. Pin 7

    ODCAbits.ODA0 = 1; // Open Drain collecctor for pins 2, 3, 6, 7
    ODCAbits.ODA1 = 1;
    ODCBbits.ODB2 = 1;
    ODCBbits.ODB3 = 1;

    // COLUMNS
    TRISBbits.TRISB9 = INPUT; // column 1, input. Pin 18
    TRISBbits.TRISB10 = INPUT; // column 2, input. Pin 21
    TRISBbits.TRISB11 = INPUT; // column 3, input. Pin 22

    CNPU2bits.CN21PUE = 1; // pull up resistor for columns. Pins 18, 21, 22
    CNPU2bits.CN16PUE = 1;
    CNPU1bits.CN15PUE = 1;

    IFS1bits.CNIF = 0;     // Interrupt Flag pulled down.

    CNEN2bits.CN21IE = 1; // Pin 18 Change Notification Interrupt Enable for each input pin.
    CNEN2bits.CN16IE = 1; // Pin 21
    CNEN1bits.CN15IE = 1; // Pin 22

    IEC1bits.CNIE = 1;     // Enable change notification interrupt.



}

/* This function will be called AFTER you have determined that someone pressed
 * SOME key. This function is to figure out WHICH key has been pressed.
 * This function should return -1 if more than one key is pressed or if
 * no key is pressed at all. Otherwise, it should return the ASCII character of
 * the key that is pressed.
 */
char scanKeypad(void){
    char key = -1; // initialize to -1 first.
    int numKeyPress = 0; // Presses so far are zero

    // Initialize row values.
    ROW1 = 0;
    ROW2 = 1;
    ROW3 = 1;
    ROW4 = 1;

    delayMs(10); // delay to set row values

    // Check each individual row. if one row is pressed then the branch is taken and key is updated.
    if (COL1 == 0){
        key = '1';
        numKeyPress++;
    }
    if(COL2 == 0){
        key = '2';
        numKeyPress++;
    }
    if(COL3 == 0){
        key = '3';
        numKeyPress++;
    }

    // Update to check next row.
    ROW1 = 1;
    ROW2 = 0;
    delayMs(10); // delay to set row value

    // Check each individual row. if one row is pressed then the branch is taken and key is updated.
    if (COL1 == 0){
        key = '4';
        numKeyPress++;
    }
    if(COL2 == 0){
        key = '5';
        numKeyPress++;
    }
    if(COL3 == 0){
        key = '6';
        numKeyPress++;
    }

    // Update to check next row.
    ROW2 = 1;
    ROW3 = 0;
    delayMs(10); // delay to set row value

    // Check each individual row. if one row is pressed then the branch is taken and key is updated.
    if (COL1 == 0){
        key = '7';
        numKeyPress++;
    }
    if(COL2 == 0){
        key = '8';
        numKeyPress++;
    }
    if(COL3 == 0){
        key = '9';
        numKeyPress++;
    }

    // Update to check next row.
    ROW3 = 1;
    ROW4 = 0;
    delayMs(10); // delay to set row value

    // Check each individual row. if one row is pressed then the branch is taken and key is updated.
    if (COL1 == 0){
        key = '*';
        numKeyPress++;
    }
     if(COL2 == 0){
        key = '0';
        numKeyPress++;
    }
    if(COL3 == 0){
        key = '#';
        numKeyPress++;
    }

    // Return Rows to their values.
    ROW1 = 0;
    ROW2 = 0;
    ROW3 = 0;
    ROW4 = 0;
    delayMs(10); // delay to set row value

    // if more than one key was pressed this branch will be taken.
    if(numKeyPress > 1){
        key = -1; // update return value.
    }

    return key; // return key of the set {-1,1,2,3,4,5,6,7,8,9,0,#,*}
}
