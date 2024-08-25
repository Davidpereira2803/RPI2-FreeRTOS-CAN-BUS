#include "FreeRTOS.h"
#include "task.h"

#include "ledcontrol.h"

#define Base_GPIO 0x3F200000 	// Base address for GPIO
#define High_GPIO *(led_gpio + 7)  	// GPIO pin high
#define Low_GPIO *(led_gpio + 10) 	// GPIO pin low

volatile unsigned int *led_gpio = (unsigned int *)Base_GPIO;

// Function to turn on the LED on the GPIO passed as parameter
void vLEDON(int gpio_pin){

	// Configure the GPIO pin passed as parameter
    *(led_gpio + ((gpio_pin) / 10)) &= ~(7 << ((gpio_pin % 10) * 3));
    *(led_gpio + ((gpio_pin) / 10)) |= (1 << ((gpio_pin % 10) * 3));
    	
	// Turn the LED on
    High_GPIO = 1 << gpio_pin;
}

void vLEDOFF(int gpio_pin){
	
	// Configure the GPIO pin passed as parameter
    *(led_gpio + ((gpio_pin) / 10)) &= ~(7 << ((gpio_pin % 10) * 3));
    *(led_gpio + ((gpio_pin) / 10)) |= (1 << ((gpio_pin % 10) * 3));

	// Turn the LED off	
    Low_GPIO = 1 << gpio_pin;
}

