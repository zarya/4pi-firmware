
#include <board.h>
#include <pio/pio.h>
#include <pio/pio_it.h>
#include <irq/irq.h>
#include <tc/tc.h>
#include <systick/systick.h>
#include <utility/trace.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parameters.h"
#include "serial.h"
#include "samadc.h"
#include "stepper_control.h"
#include "planner.h"
#include "gcode_parser.h"
#include "sdcard.h"
#include "hdlcd.h"

//#include "heaters.h"


//--------------------------
// EXTERN FUNCTIONS
//--------------------------
extern void adc_sample();
extern void samserial_init();

extern void motor_setup();
extern void motor_setopts(unsigned char axis, unsigned char ustepbits, unsigned char current);
extern void motor_enaxis(unsigned char axis, unsigned char en);
extern void motor_setdir(unsigned char axis, unsigned char dir);
extern void motor_step(unsigned char axis);
extern void motor_unstep();

extern void heaters_setup();
extern void manage_heaters(void);
//extern void heater_soft_pwm(void);
extern void ConfigureTc_1(void);


//extern void sprinter_mainloop();
extern void initadc(int);
extern void samserial_setcallback(void (*c)(unsigned char));


#ifndef AT91C_ID_TC0
    #define AT91C_ID_TC0 AT91C_ID_TC
#endif

//--------------------------
// GLOBAL VARIABLES
//--------------------------
/// Global timestamp in milliseconds since start of application.
volatile unsigned long timestamp = 1;

  
//----------------------------------------------------------
//SYSTICK --> INTERRUPT call every 1ms 
//----------------------------------------------------------
void SysTick_Handler(void)
{
	
	timestamp++;
	
    if(timestamp%10==0)
        adc_sample();

    if(timestamp%1000==0) {
        hdlcd_main();
    }
    
    
    //temp control goes in here
    //temp0 = chan 5 = adc_read(5) etc (returns unsigned absolute millivolt value).
    //temp1 = chan 3
    //temp2 = chan 1
    //temp3 = chan 2
    
    //if(timestamp%1000==0)//every 1 second
	//{
	//  for(i=1;i<9;i++)
    //  	printf("Channel %u : %u mV\n", i,adc_read(i));
    //	
	
	if(timestamp%250==0) //every 100 ms
    {
		manage_heaters();
    }
	    
}
unsigned long oldtimestamp=1;
void do_periodic()
{
	if (timestamp==oldtimestamp)
		return;
	oldtimestamp=timestamp;
	if (timestamp % 500 == 0)
	{
		sdcard_handle_state();
	}
	
}


int main()
{
	
    TRACE_CONFIGURE(DBGU_STANDARD, 115200, BOARD_MCK);
    printf("-- %s\n\r", BOARD_NAME);
    printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

    // If they are present, configure Vbus & Wake-up pins
    //PIO_InitializeInterrupts(0);
	
	//-------- Init parameters --------------
	printf("INIT Parameters\n\r");
	init_parameters();
	
	//-------- Load parameters from Flash --------------
	printf("Load parameters from Flash\n\r");
	FLASH_LoadSettings();
	
    //-------- Init UART --------------
	printf("USB Seriel INIT\n\r");
	samserial_init();
	
	//-------- Init ADC without Autostart --------------
	printf("Init ADC\n\r");
    initadc(0);
	
	//-------- Init Motor driver --------------
	printf("Init Motors\n\r");
    motor_setup();
	
	//-------- Init Heater I/O  --------------
	printf("Init Heaters\n\r");
    heaters_setup();
	
    //-------- Start SYSTICK (1ms) --------------
	printf("Configuring systick.\n\r");
	SysTick_Configure(1, BOARD_MCK/1000, SysTick_Handler);
	
	//-------- Timer 0 for Stepper --------------
	printf("Init Stepper IO\n\r");
    stepper_setup();	//Timer for Stepper


	//-------- Timer 0 for Stepper --------------
	printf("Configuring Timer 0 Stepper\n\r");
    ConfigureTc0_Stepper();	//Timer for Stepper
	
	//-------- Timer 1 for heater PWM --------------
	printf("Configuring Timer 1 PWM.\n\r");
	ConfigureTc_1();

	//-------- Init Planner Values --------------
	printf("Plan Init\n\r");
	plan_init();

    //-------- setup LCD driver ----------
    printf("LCD Init\n\r");
    hdlcd_setup();
	
	printf("G-Code parser init\n\r");
	gcode_init(usb_printf);
	
	//-------- Check for SD card presence -------
//	sdcard_handle_state();
	
	//motor_enaxis(0,1);
    //motor_enaxis(1,1);
	while (1)
	{
  		//uncomment to use//sprinter_mainloop();
    	//main loop events go here

		do_periodic();

		gcode_update();
/*    	
		if(buflen < (BUFSIZE-1))
			get_command();

    	if(buflen > 0)
		{
			
			//-------- Check and execute G-CODE --------------
			process_commands();

			//-------- Increment G-Code FIFO  --------------
			buflen = (buflen-1);
			bufindr++;
			if(bufindr == BUFSIZE) bufindr = 0;
			
		}
*/		  
    }
}

