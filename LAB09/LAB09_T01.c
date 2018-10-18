
#define PART_TM4C123GH6PM //define the board
#define TARGET_IS_TM4C123_RB1 //define the target

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h" //include FPU file
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

#include "inc/tm4c123gh6pm.h"

#ifndef M_PI  //create PI
#define M_PI 3.14159265358979323846 //define the value for PI
#endif

#define SERIES_LENGTH 100 //depth of data buffer
float gSeriesData[SERIES_LENGTH]; //an array of floats

int32_t i32DataCount = 0; //counter
int main(void) { //main
    float fRadians; //float for calculation purposes
    ROM_FPULazyStackingEnable(); //enable Lazy Stacking, skips the stacking of floating point registers to avoid latency

    ROM_FPUEnable();//enable FPU (Floating Point Unit)

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); //enable Clock to 50MHz

    fRadians = ((2 * M_PI) / SERIES_LENGTH); //divide 2PI by the array size
    while(i32DataCount < SERIES_LENGTH) {
        gSeriesData[i32DataCount] = sinf(fRadians * i32DataCount); //calaculate sine wave value for each 100
        i32DataCount++; }
    while(1) {
    }
}
