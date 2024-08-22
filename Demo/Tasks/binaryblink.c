#include "FreeRTOS.h"
#include "task.h"

#include "binaryblink.h"

#define GPIO_BASE 0x3F200000 		// Base address for GPIO
#define GPIO_High *(blink_gpio + 7)	// GPIO pin high
#define GPIO_Low *(blink_gpio + 10) 	// GPIO pin low
#define GPIO_PIN_4 4 // Pin number 18 -> Red LED
#define GPIO_PIN_26 26 // Pin number 26 -> Yellow LED

volatile unsigned int *blink_gpio = (unsigned int *)GPIO_BASE;

// Function to set and select the register for the specific pin
void setgpio(uint32_t pin){
    *(blink_gpio + ((pin) / 10)) &= ~(7 << ((pin % 10) * 3));
    *(blink_gpio + ((pin) / 10)) |= (1 << ((pin % 10) * 3));
}

// Function to blink Pin 18 and Pin 26 according to the decimal value
void decimaltobinaryblink(int decimalnum, int bits) {
    // num -> decimal number
    // bits -> # of bytes for binary output
    
    // Configure pin 26 and 4 to be able to turn on and off the LED
    setgpio(GPIO_PIN_26);
    setgpio(GPIO_PIN_4);

    uint32_t gpio_pin_1 = GPIO_PIN_4;
    uint32_t gpio_pin_0 = GPIO_PIN_26; 

    for (int i = bits; i >= 0; i--) {
        // bit is either 0 or 1 depending on the bit shift of decimalnum by i
        int bit = (decimalnum >> i) & 1;

        if (bit) {
            // Blink once GPIO 18 (RED) for binary '1'
            GPIO_High = 1 << gpio_pin_1;    // Turn on
            vTaskDelay(pdMS_TO_TICKS(200)); // On duration 200ms
            GPIO_Low = 1 << gpio_pin_1;     // Turn off
        } else {
            // Blink once GPIO 26 (YELLOW) for binary '0'
            GPIO_High = 1 << gpio_pin_0;    // Turn on
            vTaskDelay(pdMS_TO_TICKS(200)); // On duration 200ms
            GPIO_Low = 1 << gpio_pin_0;     // Turn off
        }

        vTaskDelay(pdMS_TO_TICKS(250)); // Off duration, delay between each blink 200ms
    }
}

/*
    Function to call the task, to display the decimal number as binary on the LEDs
    Function defines decimalnum which represents the decimal number 
    bits represents how many bits the led will represent, as default a full byte
    is represented 8bits (-1 is required becaus of the for loops implementation )
*/
void decimaltobinaryblinktask(void *pParam) {

    int decimalnum = *((int *)pParam);  // Decimal number to convert and blink
    int bits = 8 - 1;		            // # of bits for binary output	

    while(1) {
        decimaltobinaryblink(decimalnum, bits);
        vTaskDelay(pdMS_TO_TICKS(3000)); // Delay between full binary displays 3s
    }
}


void blink( int num){
    decimaltobinaryblink(num, 7);
}