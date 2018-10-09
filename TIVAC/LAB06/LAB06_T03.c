#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"


int main(void)
{

    int i,j,k; //set temporary values for nested for loops
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIOF
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);  //enable PWM1

    GPIOPinConfigure(GPIO_PF1_M1PWM5); //assign PWM pins to Output LED pins
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);

    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //assign pins to PWM

    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY; //unlcok PortF
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);

    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, 400); //set period to 400
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 400);

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 400); //initialize all LED output pins
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 400);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 400);

    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    PWMOutputState(PWM1_BASE, (PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT), true);


    while(1)
    {

        for(i = 360; i > 40; i-- ){ //90 to 10%
            for(j = 360; j > 40; j--){ //90% to 10%
                for(k = 360; k > 40; k--){ //90%-10% nested for loops
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, i); //red
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, j); //blue
                    ROM_PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, k); //green
                    SysCtlDelay(100000);
                }
            }

        }

    }

}
