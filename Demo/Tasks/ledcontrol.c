#include "FreeRTOS.h"
#include "task.h"

#include "ledcontrol.h"

#define Base_GPIO 0x3F200000 	// Base address for GPIO
#define High_GPIO *(gpio + 7)  	// GPIO pin high
#define Low_GPIO *(gpio + 10) 	// GPIO pin low

volatile unsigned int *gpio = (unsigned int *)Base_GPIO;

void vLEDON(int gpio_pin){
    	*(gpio + ((gpio_pin) / 10)) &= ~(7 << ((gpio_pin % 10) * 3));
    	*(gpio + ((gpio_pin) / 10)) |= (1 << ((gpio_pin % 10) * 3));
    	
    	High_GPIO = 1 << gpio_pin;
}

void vLEDOFF(int gpio_pin){
    	*(gpio + ((gpio_pin) / 10)) &= ~(7 << ((gpio_pin % 10) * 3));
    	*(gpio + ((gpio_pin) / 10)) |= (1 << ((gpio_pin % 10) * 3));
    	
    	Low_GPIO = 1 << gpio_pin;
}

