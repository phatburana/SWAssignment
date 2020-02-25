#include "mbed.h"
DigitalOut led1(LED1); //le1 queue
DigitalOut led2(LED2); //led2 blink
DigitalOut led3(LED3); //led3 blink
DigitalOut led4(LED4); //led4 blink
InterruptIn sw(PC_13);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void rise_toggle(void) {
    // Toggle LED1
    led1 = !led1;}
void fall_toggle(void) {
    // Toggle LED1
    led1 = !led1;}

int main() {
    // Start the event queue
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    sw.rise(rise_toggle);  
    sw.fall(queue.event(fall_toggle));
    while (true) {   //led2,led3,led4 blink
    	led3 = 1;
    	led4 = 1;
        led2 = 1; // LED2 is ON
        wait(0.5); // 500 ms
        led3 = 0;
    	led4 = 0;
        led2 = 0; // LED2 is OFF
        wait(0.5); // 500 ms
    }
}
