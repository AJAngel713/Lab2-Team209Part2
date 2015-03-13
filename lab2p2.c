// ******************************************************************************************* //
// File:         lab2p2.c
// Date: 3/13/2015
// Authors: Angel Lopez, Rubi Ballesteros, Miguel Angel Garcia, Arminda estrada, Phillip Showers.
//
// Description: This programm can store 4 passwords. Initially the program contains only 1 password, which is 1 2 3 4.
//              When the user inputs the correct password the LCD will display "Good", otherwise it will display "Bad".
//              The user can enter setMode to enter a new password by pressing two *'s in a row and then entering a numbers only 4 digit password.
//              Up to four passwords can be stored.
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

#define bool int // define for a fake boolean data type. For readability.
#define true 1   // define true to use with bool data type.
#define false 0  // define false for bool data type.

typedef enum stateTypeEnum
{
    wait,
    keyFind,
    writeLCDState,
    enter,
    setModeState
}stateType;

volatile stateType curState; // For the current state.
bool setMode = false; // This is used to know if setMode is active.
char keyToWrite = -1; // Used to store the character received from the keypad.
int numOfPasswords = 1; // keeps traack of the number of password currently stored: goes up to 4.
char passwords[4][4] = {{'1','2','3','4'}, // array containing 4 passwords, which is an array of 4 characters.
                        {'x','x','x','x'},
                        {'x','x','x','x'},
                        {'x','x','x','x'}};
char inputs[4] = {'x','x','x','x'}; // This is what the user inputs.
char setModeKeys[2] = {'*','*'}; // These are the keys that will activate setMode.
int inputsIndex = 0; // to keep track to which input the program will write to next, also used for checking if statement conditions.

/*
 Helper Functions:
 */
bool stringEqual(char* string1, char* string2, int size); // Used to check if two character strings are equal, takes two char* and a int size.
bool isPassword(); // Used when the user presses 4 buttons, checks if the password is correct and used in an if statement to decide whether to display "good" or "bad".
bool isValid(); // Used when program is in setMode, check if the input password is valid, if it is it writes the password in the next available slot.


int main(void)
{
    
    initKeypad(); // Initialize keypad.
    initLCD(); // Initialize LCD.
    clearLCD(); // Clears the LCD and puts the cursor in spot 0,0
    curState = enter; // Enters the enter state.

    while(1){
        switch (curState)
        {
            //--------------WAIT------------------//
            case wait:
                // These four lines are used in case the rows were not set to default value after exiting the scanKeypad function.
                ROW1 = 0;
                ROW2 = 0;
                ROW3 = 0;
                ROW4 = 0;
                break;

            //--------------KEYFIND---------------//
            case keyFind:
                keyToWrite = scanKeypad(); // scans the keypad and writes the result in keyToWrite, if no keys or more than one key was pressed the function returns -1.
                curState = writeLCDState; // go to next state.
                break;

            //--------------WRITELCD--------------//
            case writeLCDState:
                if (keyToWrite != -1){ // check if the value read is valid, not valid when -1.
                    printCharLCD(keyToWrite); // if valid print the key to the LCD.
                    inputs[inputsIndex] = keyToWrite; // add the character to the inputs array to remember.
                    inputsIndex++; // increment the index of inputs array.
                    if (inputsIndex == 2 && !setMode){ // If two keys are pressed:
                        if (stringEqual(setModeKeys, inputs, 2)) curState = setModeState; // if the two keys are setModeKeys then go to setModeState.
                    }
                    else if (inputsIndex == 4 && !setMode){ // when the user pressed four keys:
                        clearLCD(); // clear the LCD.
                        if (isPassword()) {printStringLCD("Good");} // If password is correct display good.
                        else {printStringLCD("Bad");} // Otherwise if incorrect password display Bad.
                        delay2Sec(); // Wait for 2 seconds.
                        curState = enter; // go to enter state again to enter new numbers.
                    }
                    // Set Mode
                    else if (setMode && inputsIndex == 4){ // when in setMode and 4 keys are pressed.
                        clearLCD(); // Clear the LCD.
                        if (isValid()){ // Check if the password is valid (no '*' or '#')
                            printStringLCD("Valid"); // print Valid.
                            delay2Sec(); // show valid for 2 seconds.
                        }
                        else {
                            printStringLCD("Invalid"); // if not valid show Invalid and
                            delay2Sec();               // wait for 2 seconds.
                        }
                        curState = enter; // go to the enter state to enter new numbers.
                    }
                    if (inputsIndex > 1 && curState == writeLCDState && inputs[0] == '*'){ // used to check if the user pressed any key other than '*' after having pressed it for the first time.
                        clearLCD(); // clear the LCD.
                        printStringLCD("Bad"); // Display Bad.
                        delay2Sec(); // delay 2 seconds.
                        curState = enter; // enter the enter state to enter new numbers.
                    }
                    // Used for checking when the user inputs a '#' or a '*' after the first number.
                    else if ((keyToWrite == '#' || (keyToWrite == '*' && inputsIndex > 1)) && !setMode && curState == writeLCDState){
                        clearLCD(); // clear LCD.
                        printStringLCD("Bad"); // Print Bad.
                        delay2Sec(); // Delay 2 seconds.
                        curState = enter; // enter the enter state to enter new numbers.
                    }
                    else if ((keyToWrite == '#' || keyToWrite == '*') && setMode){ // Used to check when the user enters '*' or '#' while in set Mode.
                        clearLCD(); // clear LCD.
                        printStringLCD("Invalid"); // print Invalid.
                        delay2Sec(); // delay 2 seconds.
                        curState = enter; // Enter enter state.
                    }
                    if (curState == writeLCDState) {curState = wait;} // go to the wait state to start waiting for new key presses.
                }
                else curState = wait; // go to wait state to wait for new key presses.
                break;

            //--------------ENTER---------------//
            case enter:
                lcdEnterState(); // Prints "Enter" on the LCD and moves cursor to second row.
                setMode = false; // set the setMode flag.
                curState = wait; // set state variable to wait state.
                inputsIndex = 0; // reset inputs index.
                break;

            //--------------SETMODE-------------//
            case setModeState:
                lcdSetModeState(); // pront "Set Mode" to the LCD and move cursor to second row.
                setMode = true; // Set setMode flag to true.
                curState = wait; // enter wait state.
                inputsIndex = 0; // reset inputs index.
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
    bool result = true; // define result bool variable wich will be returned.
    int i = 0; // used to iterate an array with a for loop.

    for (i = 0; i < size; i++){ // check each index., go up to size.
        if (string1[i] != string2[i]){ // check if both characters at indeces are equal.
            result = false; // if the characters are NOT equal then update the result variable to false.
        }
    }
    return result; // return result. If the condition inside for loop is never true this result variable will be true.
}

/*
 * This function compares the contents of the inputs given by the user to the passwords array.
 * returns true if the input matches any of the stored password.
 */
bool isPassword()
{
    bool result = false; // define result bool variable wich will be returned.
    int i = 0; // used to iterate an array with a for loop.

    for (i = 0; i < 4; i++){ // check each index, go up to 4.
        if (stringEqual(inputs, passwords[i], 4)){ // check if the password and the input array are equal.
            result = true; // when the inputs are equal to one of the stored passwords update the result variable to true because it was found.
            break; // break because no need to check for more passwords.
        }
    }

    return result; // return the result variable.
}

/*
 * This function checks whether a password the user entered is valid and then stores it in the next password slot. <- Returns true
 * A valid password contains only numbers.
 * Invalid means the user entered '*' or '#'
 * When invalid the function returns false and does not store the password.
 */
bool isValid(){
    bool result = true; // define result bool variable wich will be returned.
    int i = 0; // used to iterate an array with a for loop.

    for (i = 0; i < 4; i++){ // check each index, go up to 4.
        if (inputs[i] == '*' || inputs[i] == '#'){ // check if an input was '*' or '#'
            result = false; // update the result variable to false.
        }
    }

    if (result){ // if the input was valid it will take this branch.
        numOfPasswords = numOfPasswords%4 + 1; // update the number of passwords variable.
        for (i = 0; i < 4; i++){ // used to traverse an array.
            passwords[numOfPasswords-1][i] = inputs[i]; // write the inputs in the next available location, if full it will write to the beginning.
        }
    }
    
    return result; // return the result variable.
}

