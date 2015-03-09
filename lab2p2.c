// ******************************************************************************************* //
// File:         lab2p2.c
// Date:
// Authors:
//
// Description:
// ******************************************************************************************* //

#include "p24fj64ga002.h"
#include "lcd.h"
#include "timer.h"
#include "keypad.h"
#include <stdio.h>

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & BKBUG_ON & COE_OFF & ICS_PGx1 &
          FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )

_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
          IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )

#define bool int
#define true 1
#define false 0

typedef enum stateTypeEnum
{
    wait,
    keyFind,
    writeLCDState,
    enter,
    setModeState
}stateType;

volatile stateType curState;
bool setMode = false;
char keyToWrite = -1;
int numOfPasswords = 1;
char passwords[4][4] = {{'1','2','3','4'},
                        {'x','x','x','x'},
                        {'x','x','x','x'},
                        {'x','x','x','x'}};
char inputs[4] = {'x','x','x','x'};
char setModeKeys[2] = {'*','*'};
int inputsIndex = 0;

bool stringEqual(char* string1, char* string2, int size);
bool isPassword();
bool isValid();

/*
 * TODO: 
 *       2. Implement displaying "Bad" when '#' is pressed.
 *       4. Implement functionality for when '*' is the first key pressed
 *          a. (Allow second key to be pressed)
 *          b. (when any key other than a socond '*' is entered display Bad.
 */

int main(void)
{
    
    initKeypad();
    initLCD();
    clearLCD();
    curState = enter;

    while(1){
        switch (curState)
        {
            //--------------WAIT------------------//
            case wait:
                ROW1 = 0;
                ROW2 = 0;
                ROW3 = 0;
                ROW4 = 0;
                break;

            //--------------KEYFIND---------------//
            case keyFind:
                keyToWrite = scanKeypad();
                curState = writeLCDState;
                break;

            //--------------WRITELCD--------------//
            case writeLCDState:
                if (keyToWrite != -1){
                    printCharLCD(keyToWrite);
                    inputs[inputsIndex] = keyToWrite;
                    inputsIndex++;
                    if (inputsIndex == 2 && !setMode){
                        if (stringEqual(setModeKeys, inputs, 2)) curState = setModeState;
                    }
                    else if (inputsIndex == 4 && !setMode){
                        clearLCD();
                        if (isPassword()) {printStringLCD("Good");}
                        else {printStringLCD("Bad");}
                        delay2Sec();
                        curState = enter;
                    }
                    // Set Mode
                    else if (setMode && inputsIndex == 4){
                        clearLCD();
                        if (isValid()){
                            printStringLCD("Valid");
                            delay2Sec();
                        }
                        else {
                            printStringLCD("Invalid");
                            delay2Sec();
                        }
                        curState = enter;
                    }
                    if (inputsIndex > 1 && curState == writeLCDState && inputs[0] == '*'){
                        clearLCD();
                        printStringLCD("Bad");
                        delay2Sec();
                        curState = enter;
                    }
                    else if ((keyToWrite == '#' || (keyToWrite == '*' && inputsIndex > 1)) && !setMode && curState == writeLCDState){
                        clearLCD();
                        printStringLCD("Bad");
                        delay2Sec();
                        curState = enter;
                    }
                    else if ((keyToWrite == '#' || keyToWrite == '*') && setMode){
                        clearLCD();
                        printStringLCD("Invalid");
                        delay2Sec();
                        curState = enter;
                    }
                    if (curState == writeLCDState) {curState = wait;}
                }
                else curState = wait;
                break;

            //--------------ENTER---------------//
            case enter:
                lcdEnterState();
                setMode = false;
                curState = wait;
                inputsIndex = 0;
                break;

            //--------------SETMODE-------------//
            case setModeState:
                lcdSetModeState();
                setMode = true;
                curState = wait;
                inputsIndex = 0;
                break;

        } // switch
    } // while

    return 0;
}

void _ISR _CNInterrupt()
{
    _CNIF = 0;   // Always put the flag down first.
    delayMs(10); //press/release delay


    if (curState == wait){   // check what is the current state. if wait go ahead and scan keys.
        if (COL1 == 0 || COL2 == 0 || COL3 == 0){
//        curState = keyFind;
            /*
             NOTE: I can use the statement right above this, but the program breaks more often by pressing keys fast and simultaneously.
             * By scanning the keys here in the interrupt it no longer breaks.
             */
            keyToWrite = scanKeypad();   // Scan keys.
            curState = writeLCDState;         // Go to WriteLCD state.
        } // if (COL...
    }  // if (CurState...

} // _CNInterrupt

/*
 * Compares two strings to see if they are exactly equal.
 * Parameters are the first string, second string, and the size of the array.
 * Note: string is a char* array.
 */
bool stringEqual(char* string1, char* string2, int size)
{
    bool result = true;
    int i = 0;

    for (i = 0; i < size; i++){
        if (string1[i] != string2[i]){
            result = false;
        }
    }
    return result;
}

/*
 * This function compares the contents of the inputs given by the user to the passwords array.
 * returns true if the input matches any of the stored password.
 */
bool isPassword()
{
    bool result = false;
    int i = 0;

    for (i = 0; i < 4; i++){
        if (stringEqual(inputs, passwords[i], 4)){
            result = true;
            break;
        }
    }

    return result;
}

/*
 * This function checks whether a password the user entered is valid and then stores it in the next password slot. <- Returns true
 * A valid password contains only numbers.
 * Invalid means the user entered '*' or '#'
 * When invalid the function returns false and does not store the password.
 */
bool isValid(){
    bool result = true;
    int i = 0;

    for (i = 0; i < 4; i++){
        if (inputs[i] == '*' || inputs[i] == '#'){
            result = false;
        }
    }

    if (result){
        numOfPasswords = numOfPasswords%4 + 1;
        for (i = 0; i < 4; i++){
            passwords[numOfPasswords-1][i] = inputs[i];
        }
    }
    
    return result;
}

