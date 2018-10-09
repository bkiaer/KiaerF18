#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include"driverlib/gpio.h"

 #ifdef DEBUG
void__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif



int main(void)
{

    uint32_t ui32ADC0Value[4]; //array for storing the data read
    volatile uint32_t ui32TempAvg; //avg Temp
    volatile uint32_t ui32TempValueC; //celsius
    volatile uint32_t ui32TempValueF; //F

    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); //enable ADC0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIOF
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3); //setoutput

    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0); //SS3

    ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END); //configure step in ADC sequencer

    ADCSequenceEnable(ADC0_BASE, 3); //enable ADC sequencer 3

    while(1)
    {
        ADCIntClear(ADC0_BASE, 3);
        ADCProcessorTrigger(ADC0_BASE, 3);
        while(!ADCIntStatus(ADC0_BASE, 3, false))
        {
        }
        ADCSequenceDataGet(ADC0_BASE, 3, ui32ADC0Value);
        ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
        ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
        ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

        if(ui32TempValueF > 72){ //if Farenheit is over 72 then turn on blue LED (PF2)
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);
        }
           else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
            }
    }



}
