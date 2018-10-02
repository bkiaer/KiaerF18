#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/sysctl.h"
#include"driverlib/gpio.h"

uint8_t ui8PinData=2;
int freq;

int main(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_SYSDIV_2_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);



    while(1)
    {
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8PinData); //write output pins
        freq = SysCtlClockGet();
        SysCtlDelay(4047620); //delay
        freq = SysCtlClockGet();
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
        SysCtlDelay(4047620); //delay

        if(ui8PinData==14) {ui8PinData=2;} //if RGB then assign Red next
        else if(ui8PinData==2) {ui8PinData=8;} //if red then turn green
        else if(ui8PinData==8) {ui8PinData=4;} //if green then turn blue
        else if(ui8PinData==4) {ui8PinData=10;} //if blue then turn RG
        else if(ui8PinData==10) {ui8PinData=6;} //if RG then turn RB
        else if(ui8PinData==6) {ui8PinData=12;} //if RB then turn GB
        else if(ui8PinData==12) {ui8PinData=14;} //if GB then turn RGB

    }
}
