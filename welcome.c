#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

//*****************************************************************************
//
// David Rosales
// EID: dar542
// Lab Project 1
//
// When an appropriate Board 1 switch is pressed, the string,
// “Welcome to ESE 382N.4  - ADVANCED EMBEDDED MICROCONTROLLER SYSTEMS”,
// will scroll across the OLED display.
//
// When the program is started a message will be shown on the top of the
// screen telling the user to press the select button. Once pressed, the
// message is cleared and the required message is scrolled across the
// screen. When it is finished scrolling, the original message is displayed
// again.
//
//*****************************************************************************

volatile unsigned long ulCol = -1;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// The interrupt handler for the PF1 pin interrupt.  When triggered, this will
// start the scroll.
//
//*****************************************************************************
void
GPIOFIntHandler(void)
{
    // Clear the GPIO interrupt.
    GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);

    // Set ulCol to 0 to being scrolling
    if(ulCol==-1){
    	ulCol++;
    }
}

//*****************************************************************************
//
// Display a scrolling welcome message after the select push button is pressed
//
//*****************************************************************************
int
main(void)
{
    volatile int iDelay;
    static char pucPressBut[] =
    {
    	"press select button"
    };
    static char pucWelcome[] =
    {
        "                      "
        "Welcome to ESE 382N.4 - ADVANCED EMBEDDED MICROCONTROLLER SYSTEMS"
        "                      "
    };

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Enable the peripherals used by this application.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure the push button as an input and enable the pin to interrupt on
    // the falling edge (i.e. when the push button is pressed).
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE);
    GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_1);
    IntEnable(INT_GPIOF);

    // Initialize the OLED display
    RIT128x96x4Init(1000000);

    // Loop forever
    while(1)
    {
    	// ulCol initially is set to -1
    	if(ulCol==-1){
    		// We generate the push button message
    		RIT128x96x4StringDraw(&pucPressBut, 8, 0, 15);
    	}
    	// We wait for the select button to be pushed
    	while(ulCol==-1){
    		//noop
    	}
    	// We clear the push button message
        if(ulCol==0){
        	RIT128x96x4StringDraw("                   ", 8, 0, 15);
        }

        // Display the scrolling text
        RIT128x96x4StringDraw(&pucWelcome[ulCol++], 0, 40, 11);

        // Delay for a bit
        for(iDelay = 0; iDelay < 100000; iDelay++)
        {
        }

        // Set index back to -1 to start process over again
        if(ulCol > 87)
        {
            ulCol = -1;
        }
    }
}
