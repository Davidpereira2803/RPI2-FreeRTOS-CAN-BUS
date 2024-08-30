#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>

#include "Drivers/rpi_gpio.h"
#include "Drivers/rpi_irq.h"

#include "Tasks/ledcontrol.h"
#include "Tasks/binaryblink.h"
#include "Tasks/time.h"

// variables storing the start, end and the difference of the tickcount
uint32_t exect;
uint32_t startt, endt;

//  Input list and the  currentValue, select default 11 which is not in the list
volatile int input[10];
volatile int currentValue = 11;

// Start Default task1 and task2
const TickType_t xDelay = 500 * portTICK_PERIOD_MS;

void task1(void *pParam) {
	int i = 0;
	while(1) {
		i++;
		rpi_gpio_set_val(47, 1);
		rpi_gpio_set_val(35, 0);
		vTaskDelay(xDelay);
	}
}

void task2(void *pParam) {
	int i = 0;
	while(1) {
		i++;
		vTaskDelay(xDelay/2);
		rpi_gpio_set_val(47, 0);
		rpi_gpio_set_val(35, 1);
		vTaskDelay(xDelay/2);
	}
}
// End Default task1 and task2

/*
	Function to display a decimal number as binary on LEDs
*/
void displayBinaryOnLEDs(uint32_t decimal) {
	// List of LED pins, where the first pin represents bit 0
    uint8_t ledPins[11] = {19,26,13,5,6,16,20,8,7,12};

	// Loop through list and turn the LED on or OFF
    for(int i = 0; i < 11; i++) {
		// Get bit 0 or 1 from the decimal number
        uint8_t bit = (decimal >> i) & 0x01;
        
		if(bit == 1){
			vLEDON(ledPins[i]);
		}else{
			vLEDOFF(ledPins[i]);
		}
    }
}

/*
	Function to return the value of the list with the index passed as parameter
*/
int valuereturn(int index)
{
	return input[index];
}

/*
	Taskt1, which takes the list input,
	and outputs the values periodically,
	with a delay of 200ms
*/
void taskt1(void *pParam)
{
	// Put values in the list
	input[0] = 0;
	input[1] = 1;
	input[2] = 2;
	input[3] = 3;
	input[4] = 4;
	input[5] = 5;
	input[6] = 6;
	input[7] = 7;
	input[8] = 8;
	input[9] = 9;
	input[10] = 10;

	// Start tickcount
	startt = get_cycles();
	int count = 0;
	
	// Tick values startt and endt -> count ticks 1 tick = 1ms (1000hz)
	while (count < 11)
	{
		/*
			Set the currentValue to the valuereturn with index count
			increment count
			delay 200ms
		*/
		currentValue = valuereturn(count);
		count++;
		vTaskDelay(pdMS_TO_TICKS(200));
	}
	
	// Light up LED to tell that the loop has finished
	vLEDON(4);

	// Delete the task
    vTaskDelete(NULL);
}

/*
	Taskt2, is reading the currentValue periodically
	and waiting for a match with number 6 
*/
void taskt2(void *pParam)
{
	// notfound is 1 to keep the while loop running
	int notfound = 1;
	// number to find
	int number = 6;

	while (notfound)
	{	
		// Delay the task by 100ms at the start
		vTaskDelay(pdMS_TO_TICKS(200)/2);

		// Display the number that is beeing searched
		displayBinaryOnLEDs(number);

		// Read currentValue and check if it matches number
		if (currentValue == number)
		{
			// Set notfound = 0 to exit the loop
			notfound = 0;
			// end the tick cound
			endt = get_cycles();
			
			// Compute the cycles from startt to endt
			uint32_t total = endt - startt;

			// Cycles to microseconds, divide by the cpu clock speed Mhz
			exect = total / 900;
		}

		// Delay the task by 100ms at the end
		vTaskDelay(pdMS_TO_TICKS(200)/2);
	}

	/*
		Display the time from the
		first input to taskt1 to the first match of taskt2
	*/
	displayBinaryOnLEDs(exect/1000);	
	
	// Delete the task
	vTaskDelete(NULL);
}

/**
 *	This is the systems main entry, some call it a boot thread.
 *
 *	-- Absolutely nothing wrong with this being called main(), just it doesn't have
 *	-- the same prototype as you'd see in a linux program.
 **/
int main(void) {
	start_pmu_register();

	rpi_cpu_irq_disable();

	rpi_gpio_sel_fun(47, 1);			// RDY led
	rpi_gpio_sel_fun(35, 1);			// RDY led

	// xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
	// xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);

	// measure_time();

    xTaskCreate(taskt1, "TASK T1", 256, NULL, 0 , NULL);
	xTaskCreate(taskt2, "TASK T2", 256, NULL, 0 , NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}

