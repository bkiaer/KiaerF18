

#define PART_TM4C123GH6PM

#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_pwm.h"
#include "inc/hw_types.h"
#include "driverlib/adc.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/pwm.h"
#include "driverlib/ssi.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"
#include "utils/uartstdio.c"
#include <string.h>

#include "inc/tm4c123gh6pm.h"

#include "utils/Nokia5110.h"
#include "utils/Nokia5110.c"

void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}


int main(void)
{

    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysTick_Init();
	InitConsole();
	startSSI0();
	initialize_screen(BACKLIGHT_ON,SSI0);
	int i;
	char buf[11];
	int max=11,current_pos=0;
	set_buttons_up_down();
	unsigned char menu_elements[12][25];
	menu_elements[0][0]='1';
	menu_elements[0][1]=0x00;
	menu_elements[1][0]='2';
	menu_elements[1][1]=0x00;
	menu_elements[2][0]='3';
	menu_elements[2][1]=0x00;
	menu_elements[3][0]='4';
	menu_elements[3][1]=0x00;
	menu_elements[4][0]='5';
	menu_elements[4][1]=0x00;
	menu_elements[5][0]='6';
	menu_elements[5][1]=0x00;
	menu_elements[6][0]='7';
	menu_elements[6][1]=0x00;
	menu_elements[7][0]='8';
	menu_elements[7][1]=0x00;
	menu_elements[8][0]='9';
	menu_elements[8][1]=0x00;
	menu_elements[9][0]='1';
	menu_elements[9][1]='0';
	menu_elements[9][2]=0x00;
	menu_elements[10][0]='1';
	menu_elements[10][1]='1';
	menu_elements[10][2]=0x00;
	menu_elements[11][0]='1';
	menu_elements[11][1]='2';
	menu_elements[11][2]=0x00;
	set_menu(menu_elements);
	clear_screen(SSI0);
	screen_write("CPE\n403!",ALIGN_CENTRE_CENTRE,SSI0);
	SysTick_Wait50ms(100);

    uint32_t ADCValues[1];
       uint32_t TempValueC ;
       uint32_t TempValueF ;
       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
       SysCtlDelay(3);

       ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

       ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE |
                                ADC_CTL_END);
       ADCSequenceEnable(ADC0_BASE, 3);
       ADCIntClear(ADC0_BASE, 3);
	while(1)
	{

	    clear_screen(SSI0);


	           ADCProcessorTrigger(ADC0_BASE, 3);
	           while(!ADCIntStatus(ADC0_BASE, 3, false))
	           {
	           }

	           ADCIntClear(ADC0_BASE, 3);
	           ADCSequenceDataGet(ADC0_BASE, 3, ADCValues);
	           TempValueC = (uint32_t)(147.5 - ((75.0*3.3 *(float)ADCValues[0])) / 4096.0);
	           TempValueF = ((TempValueC * 9) + 160) / 5;
	           sprintf(buf, "Temperature = %3d*C or %3d*F\n", TempValueC, TempValueF);
	           screen_write(buf,ALIGN_CENTRE_CENTRE,SSI0);
	           SysTick_Wait50ms(100);

	}
	return 0;
}


// The delay parameter is in units of the 16 MHz core clock.
void SysTick_Wait(unsigned long delay){
  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
}


void SysTick_Wait50ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 50ms
  }
}


void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}
