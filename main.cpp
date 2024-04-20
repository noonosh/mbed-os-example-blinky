/*
Author of the project: Nuriddin Islamov
Student ID: 2117032
Module: Embedded Computing
Description:
    The program is an instruction set
    for mbed NXP LPC 1768 microcontroller,
    where keypad and LEDs represent an embedded system
    in the door. The user is required to input the pin code
    for the door to be authenticated and granted access.
    The door stays open for a while and then automatically locks.
    LEDs represent the indication of the user input for each keystroke.
    The main logic checks the memory for the stored correct password, and will
    not unlock the door unless the user enters the correct one.
*/


#include "mbed.h"
#include "ThisThread.h"
#include <cctype>  // For isdigit function
#include <cstring> // For strcmp function

//--- Define the connections to the keypad ---//
BusOut rows(p5, p6, p7, p8);
BusIn cols(p9, p10, p11, p12);

DigitalOut led1(p22);
DigitalOut led2(p25);
DigitalOut led3(p23);
DigitalOut led4(p24);

// Array to represent keys on the keypad
const char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

// Predefined PIN code
const char correctPin[5] = "1234";  // Including null-terminator
char enteredPin[5] = "";            // To store user input

void updateLEDs(int pinIndex) {
    led1 = pinIndex > 0 ? 1 : 0;
    led2 = pinIndex > 1 ? 1 : 0;
    led3 = pinIndex > 2 ? 1 : 0;
    led4 = pinIndex > 3 ? 1 : 0;
}

int main() {
    printf("The door is locked. To unlock it, please enter the four-digit password and then press #.\n");

    cols.mode(PullUp);
    int pinIndex = 0;

    while (true) {
        for (int row = 0; row < 4; row++) {
            rows = ~(1 << row);
            ThisThread::sleep_for(10ms);

            for (int col = 0; col < 4; col++) {
                if (!(cols.read() & (1 << col))) {
                    char key = keys[row][col];
                    printf("%c\n", key); // print line of  pressed key
                    ThisThread::sleep_for(50ms);

                    if (key == '#') {  // submit
                        if (strcmp(enteredPin, correctPin) == 0) {
                            printf("Access Granted. Door is unlocked for 10 seconds.\n");
                            ThisThread::sleep_for(1000ms); // door remains unlocked for 10 seconds
                            printf("The door is now locked again. To unlock it, please enter the four-digit password and then press #.\n");
                        } else {
                            printf("Incorrect PIN. Please try again.\n");
                        }
                        pinIndex = 0;
                        memset(enteredPin, 0, sizeof(enteredPin));// clear input
                        updateLEDs(pinIndex);  // Reset LEDs
                    } else if (isdigit(key)) {
                        if (pinIndex < 4) {
                            enteredPin[pinIndex++] = key;  // Store digit
                            enteredPin[pinIndex] = '\0';   // Null-terminate string
                            updateLEDs(pinIndex);  // Update LED states
                        } else {
                            printf("The code should be no more than 4 digits. Please try again.\n");
                            pinIndex = 0;  // Reset index
                            memset(enteredPin, 0, sizeof(enteredPin));
                            updateLEDs(pinIndex);  // Reset LEDs
                            printf("The door is locked. To unlock it, please enter the four-digit password and then press #.\n");
                        }
                    }

                    while (!(cols.read() & (1 << col)));
                    ThisThread::sleep_for(50ms);
                }
            }
        }
        rows = 0xF;
    }
}
