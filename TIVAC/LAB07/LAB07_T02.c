
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

void
InitConsole(void)
{
    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for UART0 functions on port A0 and A1.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    // Select the alternate (UART) function for these pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);

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

                ADCProcessorTrigger(ADC0_BASE, 3);
                while(!ADCIntStatus(ADC0_BASE, 3, false))
                {
                }

                ADCIntClear(ADC0_BASE, 3);
                ADCSequenceDataGet(ADC0_BASE, 3, ADCValues);
                TempValueC = (uint32_t)(147.5 - ((75.0*3.3 *(float)ADCValues[0])) / 4096.0);
                TempValueF = ((TempValueC * 9) + 160) / 5;
                UARTprintf("Temperature = %3d*C or %3d*F\n", TempValueC,
                        TempValueF); //print F and C for temperature
                SysCtlDelay(800000000 / 12);
            }

}


void UARTIntHandler(void)
{
    uint32_t ui32Status;
    ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
    UARTIntClear(UART0_BASE, ui32Status); //clear the asserted interrupts
    while(UARTCharsAvail(UART0_BASE)) //loop while there are chars
    {

       if(UARTCharGet(UART0_BASE) == 'R'){
            UARTCharPut(UART0_BASE, 'R');
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1); //turn on red LED

        }

        if(UARTCharGet(UART0_BASE) == 'r'){
            UARTCharPut(UART0_BASE, 'r');
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0); //turn off red
        }

        if(UARTCharGet(UART0_BASE) == 'G'){
            UARTCharPut(UART0_BASE, 'G');
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3); //turn on green LED

        }

        if(UARTCharGet(UART0_BASE) == 'g'){
            UARTCharPut(UART0_BASE, 'g'); //turn off green
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
        }

        if(UARTCharGet(UART0_BASE) == 'B'){
            UARTCharPut(UART0_BASE, 'B');
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //turn on blue LED

        }
        if(UARTCharGet(UART0_BASE) == 'b'){
            UARTCharPut(UART0_BASE, 'b'); //turn off blue
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
        }

        if(UARTCharGet(UART0_BASE) == 'T'){
        UARTCharPut(UART0_BASE, 'T');
        InitConsole(); //call function to run temperature
        }

    }
}
int main(void) {
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //enable pin for LED PF2
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntMasterEnable(); //enable processor interrupts
    IntEnable(INT_UART0); //enable the UART interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts
    UARTCharPut(UART0_BASE, 'E');
    UARTCharPut(UART0_BASE, 'n');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, 'r');
    UARTCharPut(UART0_BASE, ' ');
    UARTCharPut(UART0_BASE, 'T');
    UARTCharPut(UART0_BASE, 'e');
    UARTCharPut(UART0_BASE, 'x');
    UARTCharPut(UART0_BASE, 't');
    UARTCharPut(UART0_BASE, ':');
    UARTCharPut(UART0_BASE, ' ');
    while (1) //let interrupt handler do the UART echo function
    {
        // if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
    }
}
