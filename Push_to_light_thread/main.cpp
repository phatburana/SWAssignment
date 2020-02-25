#include "mbed.h"
 
DigitalOut led(LED1);
DigitalIn button(PC_13);
Thread thread;
 
void push_to_light() {
    while (true) {
        if (button == 1){
            led = 0;
            }
        else{
            led = 1;
            }
    }
}

 
int main() {
    thread.start(push_to_light);
    }
