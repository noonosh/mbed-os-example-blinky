#include "mbed.h"
#include "ThisThread.h"
#include <cctype>  // For isdigit function
#include <cstring> // For strcmp function

// Define the connections to the keypad
BusOut rows(p5, p6, p7, p8);   // Rows connected to these pins
BusIn cols(p9, p10, p11, p12); // Columns connected to these pins

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

int main() {
    printf("The door is locked. To unlock it, please enter the four-digit password and then press #.\n");

    // Set column read pins as inputs with pull-up resistors
    cols.mode(PullUp);
    int pinIndex = 0; // Index to track input length

    while (true) {
        for (int row = 0; row < 4; row++) {
            // Activate one row at a time by setting it low
            rows = ~(1 << row);
            ThisThread::sleep_for(10ms); // Stabilization delay

            for (int col = 0; col < 4; col++) {
                // Check each column by testing each bit if it's low
                if (!(cols.read() & (1 << col))) {
                    char key = keys[row][col];
                    printf("%c\n", key); // Echo pressed key
                    ThisThread::sleep_for(50ms); // Debounce delay

                    if (key == '#') {  // Submit button
                        if (strcmp(enteredPin, correctPin) == 0) {
                            printf("Access Granted. Door is unlocked for 10 seconds.\n");
                            ThisThread::sleep_for(1000ms); // Door remains unlocked for 10 seconds
                            printf("The door is now locked again. To unlock it, please enter the four-digit password and then press #.\n");
                        } else {
                            printf("Incorrect PIN. Please try again.\n");
                            printf("The door is locked. To unlock it, please enter the four-digit password and then press #.\n");
                        }
                        pinIndex = 0;  // Reset index
                        memset(enteredPin, 0, sizeof(enteredPin));  // Clear input
                    } else if (isdigit(key)) {
                        if (pinIndex < 4) {
                            enteredPin[pinIndex++] = key;  // Store digit
                            enteredPin[pinIndex] = '\0';   // Null-terminate string
                        } else {
                            printf("The code should be no more than 4 digits. Please try again.\n");
                            pinIndex = 0;  // Reset index
                            memset(enteredPin, 0, sizeof(enteredPin));  // Clear input
                            printf("The door is locked. To unlock it, please enter the four-digit password and then press #.\n");
                        }
                    }

                    // Wait for key release
                    while (!(cols.read() & (1 << col)));
                    ThisThread::sleep_for(50ms); // Additional debounce after release
                }
            }
        }
        rows = 0xF;  // Disable all rows before next iteration
    }
}
