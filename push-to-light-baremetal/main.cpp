/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */


#include "mbed.h"
 

DigitalOut led(LED2);
DigitalIn button(PC_13);

int main() {
    
    while (true) {
        if (button == 1){
            led = 0;
            }
        else{
            led = 1;
            }
        
    }
}