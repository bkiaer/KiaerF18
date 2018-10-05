
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
#include <string.h>



#include "inc/tm4c123gh6pm.h"


int clock; //check clock freq
int i; //global variable

#define LED_PERIPH SYSCTL_PERIPH_GPIOF
#define LED_BASE GPIO_PORTF_BASE
#define RED_LED GPIO_PIN_1
#define BLUE_LED GPIO_PIN_2
#define GREEN_LED GPIO_PIN_3

#define Button_PERIPH SYSCTL_PERIPH_GPIOF
#define ButtonBase GPIO_PORTF_BASE
#define Button GPIO_PIN_0
#define ButtonInt GPIO_INT_PIN_0

volatile uint8_t value=0;

void Timer0IntHandler(void)
{

    while(1)
    {
        float delayTime = 1.5;
        uint32_t status=0;
        uint8_t state=0;

        status = GPIOIntStatus(ButtonBase,true);
        GPIOIntClear(ButtonBase,status);

        if(status & ButtonInt == ButtonInt){ //this means the button interrupt has been pressed


                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn off LED
                TIMER0_CTL_R = 0; //disable timer
                TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); //clear the timer interrupt
                timer1A_delaySec(); //run 1.5s delay function

        }
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

void timer1A_delaySec(void)
{

    clock = SysCtlClockGet();
    SYSCTL_RCGCTIMER_R |= 2; //enable clock
    TIMER1_CTL_R = 0; //disable Timer before initialization
    TIMER1_CFG_R = 0x04; //16-bit option
    TIMER1_TAMR_R = 0x02; //periodic mode and down-counter
    TIMER1_TAILR_R = 12000-1; //TimerA interval load value reg
    TIMER1_TAPR_R = 250 - 1; //TimerA prescaler 2MHz/250 = 8000Hz => 1/8000 = .125ms => 1.5s/.125ms = 12000
    TIMER1_ICR_R = 0x01; //clear the TimerA timeout flag
    TIMER1_CTL_R |= 0x01; //enable Timer A after initialization

       // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4) ;
        while((TIMER1_RIS_R & 0x1) == 0)
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 2) ; //delay and wait
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0); //turn off red LED
        TIMER1_ICR_R = 0x1; //clear flag




}







int main(void)
{
    uint32_t ui32Period;

    //Set the clock to 2Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_2|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_4MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(LED_PERIPH);
    SysCtlDelay(3);

    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY; //unlock the SW2 Pin

    HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;

//Button Interrupt
    GPIOPinTypeGPIOInput(ButtonBase, Button);
    GPIOPadConfigSet(ButtonBase ,Button,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_INT_PIN_0);

 //Timer Interrupt
    GPIOPinTypeGPIOOutput(LED_BASE, RED_LED| BLUE_LED | GREEN_LED);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ui32Period = SysCtlClockGet() ; //set to 2MHz
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);


    while(1){

    }

}
