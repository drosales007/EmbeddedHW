//*****************************************************************************
//
// David Rosales
// EID: dar542
// Lab Project 2
//
// Please use Board 1 of your Texas Instruments Stellaris EKK-LM3S8962
// Evaluation Kit to develop a stopwatch.  The specifications for the stopwatch
// are as follows:
//
// a. One switch on the board should start and stop the stopwatch. A second
//    switch should reset it.
// b. The stopwatch will employ the board’s OLED display to output the elapsed
//    time.  The display format should be mm:ss.t, with mm having a maximum
//    value of 59 minutes, ss displaying seconds, and t displaying tenths of
//    seconds.
//
// When the program is started a blinking message will be shown on the top of
// the screen telling the user to press start with the initial time displayed
// under. Once select is pressed, the message is cleared and the clock begins
// to run and update on the screen. The clock is stopable by pressing select
// again. The clock can be reset by pressing the down button but only if the
// timer is not running.
//
//*****************************************************************************

#include "inc/hw_gpio.h"
#include "utils/ustdlib.h"
#include "utils/ustdlib.c"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"

//*****************************************************************************
//
// Initialize global variables.
//
//*****************************************************************************
volatile unsigned long t = 0;
volatile int start = 0;
volatile int ts1 = 0;
volatile int s1 = 0;
volatile int s2 = 0;
volatile int m1 = 0;
volatile int m2 = 0;

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
// Interrupt handlers
//
//*****************************************************************************
void
SysTickIntHandler(void){
	// Handle incrementing the proper digits
	t++;
	// Increment tenth second if we counted to 10, reset count
	if (t==10){
		ts1++;
		t = 0;
	}
	// Increment 1st seconds column if ts counted to 10, reset ts1
	if (ts1==10){
		s1++;
		ts1 = 0;
	}
	// Increment 2nd seconds column if s1 counted to 10, reset s1
	if (s1==10){
		s2++;
		s1 = 0;
	}
	// Increment 1st minutes column if s2 counted to 6, reset s2
	if (s2==6){
		m1++;
		s2 = 0;
	}
	// Increment 2nd minutes column if m1 counted to 10, reset m1
	if (m1==10){
		m2++;
		m1 = 0;
	}
	// Wrap the clock if m2 counted to 6
	if (m2==6){
		m2 = 0;
	}
}

void
GPIOEIntHandler(void)
{
	// Clear the GPIO interrupt
	GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_1);

	// If the timer is not running we can reset the values
	if (!start){
		ms1 = 0;
		s1 = 0;
		s2 = 0;
		m1 = 0;
		m2 = 0;
	}
}

void
GPIOFIntHandler(void)
{
    // Clear the GPIO interrupt.
    GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);

    // If the watch is not running, enable SysTick and SysTick interrupt,
    // and set start to true
    if (!start){
    	start++;
    	t=0;
        SysTickIntEnable();
        SysTickEnable();
    }
    // If the watch is running, disable SysTick interrupts and set start to
    // false
    else {
    	SysTickIntDisable();
    	start--;
    }
}

static void DisplayNum(void){
	// Display the time based on the counters
	static char pcBuf[8];
	usprintf(pcBuf, "%d%d:%d%d.%d", m2, m1, s2, s1, ms1);
	RIT128x96x4StringDraw(pcBuf, 40, 40, 15);
}

int
main(void)
{
    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Initialize the OLED display and write status.
    RIT128x96x4Init(1000000);

    // Enable the peripherals used by this application
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    // Configure the down buttons as input and enable the pin to interrupt on
    // the falling edge (i.e. when the push button is pressed).
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
                         GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE);
    GPIOPortIntRegister(GPIO_PORTE_BASE, GPIOEIntHandler);
    GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_1);
    IntEnable(INT_GPIOE);

    // Configure the select buttons as input and enable the pin to interrupt on
    // the falling edge (i.e. when the push button is pressed).
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE);
    GPIOPortIntRegister(GPIO_PORTF_BASE, GPIOFIntHandler);
    GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_1);
    IntEnable(INT_GPIOF);

    // Initial start message
    static char pcStartMsg[] = {"Press Start"};

    // Initial time to display
    static char pcInitTime[8];
    usprintf(pcInitTime, "%d%d:%d%d.%d", m2, m1, s2, s1, ms1);
    RIT128x96x4StringDraw(pcInitTime, 40, 40, 15);

    // Set the time between SysTick interrupts and register the inerrupt handle
    SysTickPeriodSet(SysCtlClockGet() / 100);
    SysTickIntRegister(SysTickIntHandler);

    // Begin a blinking display of the initial start message
    while (!start){
    	int a;
        for (a=0; a<150000; a++){
        	if (a%10000==0){
        		RIT128x96x4StringDraw(pcStartMsg, 30, 20, a/10000);
        	}
        }
    }

    // The clock has started so we can clear the message
    RIT128x96x4Clear();

    // Loop forever
    while(1)
    {
    	// Display the updated time
    	DisplayNum();
    }
}
