/*
 * Retrokit_Firmware_RevA
 * Device: SAMD09C13A
 *
 * Created: 7/11/2016
 * Author : Dan Watson
 *			syncchannel.blogspot.com
 */

/* Calculations for demonstration LED blinks:
 *	- The four devices I have in mind for this demonstration:
 *		SAM D09, 48MHz, 48 DMIPS
 *		ATxmega256A3BU, 32MHz, 32 MIPS
 *		ATmega328P, 20MHz, 20 MIPS
 *		PIC 16F627A, 20MHz, 5 MIPS
 *
 *	- Blink rate and completion time are based on MIPS/DMIPS.
 *
 *	- Want blinks to take 10 seconds (10,000 milliseconds) in total. The slowest device is the PIC (5 MIPS).
 *	  The SysTick timer will interrupt every 1ms (1MHz clock / 1000).
 *	  So let's set the counter = 10000.
 *	  Every 1ms, the SAM D09 would increment the counter 48, the XMEGA increments 32, etc.
 *	  But we will increment a single counter and pre-computer the target completion values.
 *	  
 *	  SAM D: 1/10th complete = 111, complete at counter value = 1,110
 *	  XMEGA: 1/10th complete = 156, complete at counter value = 1,560
 *	    AVR: 1/10th complete = 250, complete at counter value = 2,500
 *	    PIC: 1/10th complete = 1000, complete at counter value = 10,000
*/

#include "samd09.h"
#include "clock.h"
#include "port.h"
#include "power.h"

#define SAMDLED PIN_PA09
#define XMEGALED PIN_PA08
#define AVRLED PIN_PA05
#define PICLED PIN_PA04

volatile uint16_t counter = 0;

void SysTick_Handler(void)
{
	counter++;
	
	// Check for updates on each of the four LEDs
	
	if (counter % 111 == 0)
		port_pin_set_output_level(SAMDLED,true);
    else if (counter % 131 == 0 && counter < 1110)
		port_pin_set_output_level(SAMDLED,false);
		
	if (counter % 156 == 0)
		port_pin_set_output_level(XMEGALED,true);
	else if (counter % 176 == 0 && counter < 1560)
		port_pin_set_output_level(XMEGALED,false);
		
	if (counter % 250 == 0)
		port_pin_set_output_level(AVRLED,true);
	else if (counter % 270 == 0 && counter < 2500)
		port_pin_set_output_level(AVRLED,false);
		
	if (counter % 1000 == 0)
		port_pin_set_output_level(PICLED,true);
	else if (counter % 1020 == 0 && counter < 10000)
		port_pin_set_output_level(PICLED,false);
		
	// Go to sleep after 5 seconds of all LEDs on
	if (counter == 15000)
	{
		port_group_set_output_level(&PORTA,0xFFFFFFFF,0);
		port_group_set_config(&PORTA,0xFFFFFFFF,0);
		system_sleep();
	}
}

// Configure the SysTick timer to interrupt every 1ms
static void configure_systick_handler(void)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 1000;
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

int main(void)
{
	SystemInit();
	system_clock_init(); // Internal RC, DIV8 (1MHz)
	configure_systick_handler(); // Interrupt every 1ms
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY); // Deep sleep mode
	
	
	// Configure output pins for LEDs
	struct port_config pin_conf_out;
	port_get_config_defaults(&pin_conf_out);
	pin_conf_out.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(SAMDLED, &pin_conf_out);
	port_pin_set_config(XMEGALED, &pin_conf_out);
	port_pin_set_config(AVRLED, &pin_conf_out);
	port_pin_set_config(PICLED, &pin_conf_out);
	
	while (1)
	{
		// Do nothing. Everything is handled in the SysTick timer interrupt.
	}
}