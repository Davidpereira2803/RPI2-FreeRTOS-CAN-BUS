#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/rpi_gpio.h"
#include "Drivers/rpi_irq.h"

#include "Tasks/ledcontrol.h"
#include "Tasks/binaryblink.h"

#include "Tasks/Bench/countnegative/countnegative.h"
#include "Tasks/Bench/gsm_enc/gsm_enc.h"


TickType_t exect;

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


void displayBinaryOnLEDs(uint32_t decimal) {
    // GPIO pins mapped to LEDs (from LSB to MSB)
    uint8_t ledPins[11] = {19,26,13,5,6,16,20, 8,7,12};

    for(int i = 0; i < 11; i++) {
        // Extract the ith bit (from LSB to MSB)
        uint8_t bit = (decimal >> i) & 0x01;
        
		if(bit == 1){
			vLEDON(ledPins[i]);
		}else{
			vLEDOFF(ledPins[i]);
		}
    }
}



/*
	Task to run the benchmark comples_updates
	Calls the function "comrun(NULL)", to execute the benchmark
	Computes the time recuired for the execution
*/
void benchtask(void *pvParameters)
{
	int count = 0;
	// Tick values startt and endt -> count ticks 1 tick = 1ms (1000hz)
	while (count < 100)
	{
		count++;

		TickType_t startt, endt;

		startt = xTaskGetTickCount();

		countrun(NULL);
		//gsmrun(NULL);

		endt = xTaskGetTickCount();

		//exect = endt - startt;


        if (exect < endt - startt)
        {
            exect = endt - startt;
        }else {
            exect = exect;
        }
		
		displayBinaryOnLEDs(exect);

		vTaskDelay(pdMS_TO_TICKS(2000));

	}
	
    vLEDON(4);

    vTaskDelete(NULL);
}



/**
 *	This is the systems main entry, some call it a boot thread.
 *
 *	-- Absolutely nothing wrong with this being called main(), just it doesn't have
 *	-- the same prototype as you'd see in a linux program.
 **/
int main(void) {
	/*
		(start) Default
	*/
	rpi_cpu_irq_disable();

	rpi_gpio_sel_fun(47, 1);			
	rpi_gpio_sel_fun(35, 1);
			
	// xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
	// xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);

	/*
		(end) Default
	*/

	// Create and call task to run the benchmark
	xTaskCreate(benchtask, "Complex Updates Bench", 512, NULL, 1 , NULL);
	// save the execution time and execute the binary blink to output the result
	//int decimalnum = exect;

	//xTaskCreate(decimaltobinaryblinktask, "Blink Binary", 128, (void*)&decimalnum, 1, NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}