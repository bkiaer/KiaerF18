#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

int clock; //check clock freq
int i; //global variable

int main(void)
{
    uint32_t ui32Period;
    SysCtlClockSet(SYSCTL_SYSDIV_2|SYSCTL_USE_PLL|SYSCTL_XTAL_4MHZ|SYSCTL_OSC_MAIN); //4MHz/2 = 2MHz
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ui32Period = (SysCtlClockGet() / 10) / 2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);
    while(1)
    {
    }
}


void Timer0IntHandler(void)
{

    while(1)
    {
        clock = SysCtlClockGet(); //double check clock freq at 2MHz
        TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); //clear the timer interrupt
        SYSCTL_RCGCTIMER_R |= 0x01;  //Activate Timer0
        TIMER0_CTL_R = 0;               //disable timer before initialization
        TIMER0_CFG_R = 0x04;            //16 - bit option
        TIMER0_TAMR_R = 0x02;           //periodic mode and down-counter
        if(i == 1)                      //check if ON or OFF, if 1 then LED is ON, if 0 then LED is OFF
            TIMER0_TAILR_R = 3000 - 1;      //ON calculations -> 2MHz/250 = 8000Hz -> 1/8000Hz = .125ms -> .375s (75% duty cycle) / .125ms = 3000
        else
            TIMER0_TAILR_R = 1000 - 1;      //OFF calculations -> 2MHz/250 = 8000Hz -> 1/8000Hz = .125ms -> .125s (75% duty cycle) / .125ms = 1000
        TIMER0_TAPR_R = 250-1;          //prescaler value at 250
        TIMER0_ICR_R = 0x01;            //clear the TimerA timeout flag
        TIMER0_CTL_R |= 0x01;           //enable timer after initialization

        while((TIMER0_RIS_R& 0x01) == 0) //wait for timerA timeout flag
        {
            if(i == 1)
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4); //turn ON
            else
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn OFF at different time then ON depending on above calcs
        }
        TIMER0_ICR_R = 0x01; //clear timeraA flag

        if(i == 1){ //toggle
            i = 0;
        }
        else { //toggle
            i = 1;
        }

    }
}


