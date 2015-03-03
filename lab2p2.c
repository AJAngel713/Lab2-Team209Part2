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
    writeLCD,
    enter,
    setMode
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
    moveCursorLCD(0,0);
    curState = enter;

    while(1){
        switch (curState)
        {
            //--------------WAIT------------------//
            case wait:
                break;

            //--------------KEYFIND---------------//
            case keyFind:
                keyToWrite = scanKeypad();
                curState = writeLCD;
                break;

            //--------------WRITELCD--------------//
            case writeLCD:
                if (keyToWrite != -1){
                    printCharLCD(keyToWrite);
                    inputs[inputsIndex] = keyToWrite;
                    inputsIndex++;
                }
                if (inputsIndex == 1 && !setMode){
                    if (stringEqual(setModeKeys, inputs, 2)) curState = setMode;
                }
                else if (inputsIndex == 3 && !setMode){
                    clearLCD();
                    if (isPassword()) printStringLCD("Good");
                    else printStringLCD("Bad");
                    delayMs(2000);
                }
                // Set Mode
                else if (setMode && inputsIndex == 3){
                    if (isValid()){
                        clearLCD();
                        printStringLCD("Valid");
                        delayMs(2000);
                    }
                    else {
                        clearLCD();
                        printStringLCD("Invalid");
                        delayMs(2000);
                    }
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
            case setMode:
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
    _CNIF = 0;
    delayMs(5);

    if (curState == wait){
        curState = keyFind;
    }

} // _CNInterrupt

/*
 * Compares two strings to see if they are exactly equal.
 * Parameters are the first string, second string, and the size of the array.
 * Note: string is a char* array.
 */
bool stringEqual(char* string1, char* string2, int size)
{
    bool result = true;

    for (int i = 0; int < size; i++){
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

    for (int i = 0; i < 4; i++){
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

    for (int i = 0; i < 4; i++){
        if (inputs[i] == '*' || inputs[i] == '#'){
            result = false;
        }
    }

    if (result){
        numOfPasswords = numOfPasswords%4 + 1;
        for (int i = 0; i < 4; i++){
            passwords[numOfPasswords-1] = inputs[i];
        }
    }
    
    return result;
}

