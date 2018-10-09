#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"

#include "inc/tm4c123gh6pm.h"


int clock; //check clock freq
uint32_t ui32ADC0Value[4];
volatile uint32_t ui32TempAvg;
volatile uint32_t ui32TempValueC;
volatile uint32_t ui32TempValueF;


void Timer1IntHandler(void)
{

    clock = SysCtlClockGet();
    SYSCTL_RCGCTIMER_R |= 2; //enable clock
    TIMER1_CTL_R = 0; //disable Timer before initialization
    TIMER1_CFG_R = 0x04; //16-bit option
    TIMER1_TAMR_R = 0x02; //periodic mode and down-counter
    TIMER1_TAILR_R = 80000-1; //TimerA interval load value reg
    TIMER1_TAPR_R = 250 - 1; //TimerA prescaler 2MHz/250 = 8000Hz => 1/8000 = .125ms => 1.5s/.125ms = 12000
    TIMER1_ICR_R = 0x01; //clear the TimerA timeout flag
    TIMER1_CTL_R |= 0x01; //enable Timer A after initialization

    // GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4) ;
    while((TIMER1_RIS_R & 0x1) == 0)
        ; //turn off red LED
    ADCIntClear(ADC0_BASE, 1);
    ADCProcessorTrigger(ADC0_BASE, 1);

    while(!ADCIntStatus(ADC0_BASE, 1, false))
    {
    }

    ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
    ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
    ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
    ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
    TIMER1_ICR_R = 0x1; //clear flag


}

int main(void)
{

    uint32_t ui32Period;


    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);


    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ui32Period = (SysCtlClockGet() / 10) / 2;
    TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period -1);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER1_BASE, TIMER_A);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCHardwareOversampleConfigure(ADC0_BASE, 32);

    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);

    while(1)
    {


    }
}
