#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/sysctl.h"
#include"driverlib/gpio.h"

uint8_t ui8PinData=4;
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

        if(ui8PinData==4) {ui8PinData=8;} //if blue then assign green next
        else if(ui8PinData==8) {ui8PinData=2;} //if green then turn red
        else if(ui8PinData==2) {ui8PinData=4;} //if red then turn blue

    }
}
