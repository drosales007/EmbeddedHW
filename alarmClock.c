//*****************************************************************************
//
// David Rosales
// EID: dar542
// Lab Project 3
//
// A simple program to use as an alarm clock. To start the clock, you must
// first initialize the time by holding down the left directional pad button.
// While holding the button, a display lets the user know they are in the
// set clock mode. The use can press up or down on the directional pad to
// increase or decrease the time. There is a switch from a.m. to p.m. when
// you cycle through twelve hours.
//
// To set the alarm, you do all of the above but instead hold down the right
// directional pad button instead of the left.
//
// When the time reaches the
// time set for the alarm, an LED is lit on the board and the alarm noise
// sounds. The LED and sound are turned off by pressing the Select button
// on the device.
//
// A snooze mode is also implemented where instead of just pressing the
// select button to turn off the LED, you can hold it for ~1 second. This
// turns off the LED and sound and increases the alarm time by 5 minutes.
//
//*****************************************************************************
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "drivers/rit128x96x4.h"
#include "inc/lm3s8962.h"
#include "inc/hw_gpio.h"
#include "utils/ustdlib.h"
#include "utils/ustdlib.c"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"

//*****************************************************************************
//
// Initialize global variables.
//
//*****************************************************************************
volatile unsigned long t = 0;
volatile unsigned long x = 0;
volatile unsigned long y = 0;
volatile int alarmSet = 0;
volatile int toggleVal = 0;
volatile int blink = 0;
volatile int start = 0;
volatile int ampm = 0;
volatile int m1 = 0;
volatile int m2 = 0;
volatile int h1 = 2;
volatile int h2 = 1;
volatile int aampm = 0;
volatile int am1 = 0;
volatile int am2 = 0;
volatile int ah1 = 2;
volatile int ah2 = 1;


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
// Helper Functions
//
//*****************************************************************************
static void DisplayTime(void){
	// Method for displaying the time based on the counters
	static char pcTime[10];
	if (ampm==0){
		usprintf(pcTime, "%d%d:%d%d AM", h2, h1, m2, m1);
	}
	else {
		usprintf(pcTime, "%d%d:%d%d PM", h2, h1, m2, m1);
	}
	RIT128x96x4StringDraw(pcTime, 40, 40, 15);
	if (alarmSet){
		RIT128x96x4StringDraw("Alarm ON", 40, 80, 15);
	}
}

static void DisplayTimeA(void){
	// Method for displaying the alarm based on the counters
	static char pcTime[10];
	if (aampm==0){
		usprintf(pcTime, "%d%d:%d%d AM", ah2, ah1, am2, am1);
	}
	else {
		usprintf(pcTime, "%d%d:%d%d PM", ah2, ah1, am2, am1);
	}
	RIT128x96x4StringDraw(pcTime, 40, 40, 15);
	if (alarmSet){
		RIT128x96x4StringDraw("Alarm ON", 40, 80, 15);
	}
}

static void IncrementTime(void){
	// Method for handling the increments of the time appropriately
	m1++;
	if (m1>9){
		// We increment the 2nd minute place if minute place 1 is 10
		m1 = 0;
		m2++;
	}
	if (m2>5){
		// We increment the 1st hour place if minute place 2 is 6
		m2 = 0;
		h1++;
		if (h1==2 && h2==1){
			// We switched between am and pm here so we need to handle
			if (ampm){
				ampm = 0;
			}
			else {
				ampm = 1;
			}
		}
	}
	if (h1>9){
		// We increment the 2nd hour place if hour place 1 is 10
		h1 = 0;
		h2++;
	}
	if (h2>0){
		if(h1>2){
			// If the 2nd hour place is 1, hour place 1 can only increase to 2
			// until we have to reset
			h1 = 1;
			h2 = 0;
		}
	}
}

static void IncrementTimeA(void){
	// Method for handling increments of the alarm appropriately
	am1++;
	if (am1>9){
		// We increment the 2nd minute place if minute place 1 is 10
		am1 = 0;
		am2++;
	}
	if (am2>5){
		// We increment the 1st hour place if minute place 2 is 6
		am2 = 0;
		ah1++;
		if (ah1==2 && ah2==1){
			// We switched between am and pm here so we need to handle
			if (aampm){
				aampm = 0;
			}
			else {
				aampm = 1;
			}
		}
	}
	if (ah1>9){
		// We increment the 2nd hour place if hour place 1 is 10
		ah1 = 0;
		ah2++;
	}
	if (ah2>0){
		if(ah1>2){
			// If the 2nd hour place is 1, hour place 1 can only increase to 2
			// until we have to reset
			ah1 = 1;
			ah2 = 0;
		}
	}
}

static void DecrementTime(void){
	// Method for handling decrements of the time appropriately

	// If minute place 1 > 0 we can just decrement
	if (m1>0){
		m1--;
	}
	else {
		// Minute place 1 is 0 so it decreases to 9
		m1 = 9;
		if (m2==0){
			// Minute place 2 is 0 so it decreases to 5
			m2 = 5;
			if (h1==0){
				// If hour place 1 is 0, then it is 10:XX and we set to 09:XX
				h1 = 9;
				h2 = 0;
			}
			else if (h1==1){
				// Hour place 1 can be 1 in two instances 11:00 and 01:00 so we
				// handle this here
				if (h2>0){
					// If hour place 2 is not 0, then it is 11:00 and we can
					// just decrement
					h1--;
				}
				else {
					// It is 01:00 so we set to 12:59
					h1 = 2;
					h2 = 1;
				}
			}
			else {
				// We can just decrement
				h1--;
				if (h1==1 && h2==1){
					// We switched between am and pm here so we need to handle
					if (ampm){
						ampm = 0;
					}
					else {
						ampm = 1;
					}
				}
			}
		}
		else {
			// We can just decrement
			m2--;
		}
	}
}

static void DecrementTimeA(void){
	// Method for handling decrements of the alarm appropriately

	// If minute place 1 > 0 we can just decrement
	if (am1>0){
		am1--;
	}
	else {
		// Minute place 1 is 0 so it decreases to 9
		am1 = 9;
		if (am2==0){
			// Minute place 2 is 0 so it decreases to 5
			am2 = 5;
			if (ah1==0){
				// If hour place 1 is 0, then it is 10:XX and we set to 09:XX
				ah1 = 9;
				ah2 = 0;
			}
			else if (ah1==1){
				// Hour place 1 can be 1 in two instances 11:00 and 01:00 so we
				// handle this here
				if (ah2>0){
					// If hour place 2 is not 0, then it is 11:00 and we can
					// just decrement
					ah1--;
				}
				else {
					// It is 01:00 so we set to 12:59
					ah1 = 2;
					ah2 = 1;
				}
			}
			else {
				// We can just decrement
				ah1--;
				if (ah1==1 && ah2==1){
					// We switched between am and pm here so we need to handle
					if (aampm){
						aampm = 0;
					}
					else {
						aampm = 1;
					}
				}
			}
		}
		else {
			// We can just decrement
			am2--;
		}
	}
}

static void DisplayIncrementedTime(void){
	// Method for incrementing the time by 1 minute and then displaying it
	IncrementTime();
	DisplayTime();
}

static void DisplayDecrementedTime(void){
	// Method for decrementing the time by 1 minute and then displaying it
	DecrementTime();
	DisplayTime();
}

static void DisplayIncrementedAlarm(void){
	// Method for incrementing the alarm by 1 minute and then displaying it
	IncrementTimeA();
	DisplayTimeA();
}

static void DisplayDecrementedAlarm(void){
	// Method for decrementing the alarm by 1 minute and then displaying it
	DecrementTimeA();
	DisplayTimeA();
}

static void TisA(void){
	// Method to check determine when the alarm will activate
	// If all digits of the clock are equal, we activate
	if (m1==am1){
		if (m2==am2){
			if (h1==ah1){
				if (h2==ah2){
					if (ampm==aampm){
						// Turn on the LED
						GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_0);
						// Turn on the alarm
						PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);
						PWMGenEnable(PWM0_BASE, PWM_GEN_0);
						// Display text on screen
						RIT128x96x4StringDraw("WAKE UP!", 40, 10, 15);
						// Enable the interrupt so we can turn off the alarm
						// or use the snooze button
						GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_1);
					}
				}
			}
		}
	}
}


//*****************************************************************************
//
// Interrupt handlers
//
//*****************************************************************************
void
SysTickIntHandler(void){
	// Handle incrementing the proper digits
	t++;
	if (t==60){
		IncrementTime();
		t = 0;
	}
}

void
GPIOEIntHandler(void)
{
	// Clear the GPIO interrupt
	GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);

	// Disable Interrupts
	GPIOPinIntDisable(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinIntDisable(GPIO_PORTF_BASE, GPIO_PIN_0);

	// If the clock has been set before, we need to disable SysTickInt
	if (start==1){
		SysTickIntDisable();
	}

	// Clear the OLED
	RIT128x96x4Clear();

	// If the left button got us here, display that we are in set clock mode
	// If the right button got us here, display that we are in set alarm mode
	if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)==0){
		RIT128x96x4StringDraw("Set Clock", 40, 0, 15);
		DisplayTime();
	}
	else if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3)==0){
		RIT128x96x4StringDraw("Set Alarm", 40, 0, 15);
		DisplayTimeA();
	}

	// If the left button is held down enable the time to be set
	while (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)==0){
		// While the up button is being pressed, the clock will increment
		if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0)==0){
			// In order to slow the incrementing we count to 10000 first
			x++;
			if (x>9999){
				// Show the increment
				DisplayIncrementedTime();
				x = 0;
			}
		}
		// While the down button is being pressed, the clock will decrement
		if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)==0){
			// In order to slow the decrementing we count to 10000 first
			x++;
			if (x>9999){
				// Show the decrement
				DisplayDecrementedTime();
				x = 0;
			}
		}
	}

	// If the right button is held down enable the alarm to be set
	while (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3)==0){
		// While the up button is being pressed, the clock will increment
		if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0)==0){
			// In order to slow the decrementing we count to 10000 first
			x++;
			if (x>9999){
				// Show the increment
				DisplayIncrementedAlarm();
				x = 0;
			}
		}
		// While the down button is being pressed, the clock will decrement
		if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)==0){
			// In order to slow the decrementing we count to 10000 first
			x++;
			if (x>9999){
				// Show the decrement
				DisplayDecrementedAlarm();
				x = 0;
			}
		}
		alarmSet = 1;
	}

	RIT128x96x4Clear();

	// Enable the interrupts
	GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0);

	// If the start variable is not set, we set it and start SysTick
	if (!start){
		start++;
		t = 0;
		SysTickIntEnable();
		SysTickEnable();
	}
	else {
		SysTickIntEnable();
	}
}

void
GPIOFIntHandler(void)
{
    // Clear the GPIO interrupt.
    GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);

    y = 0;
    // Counter for how long the snooze button was pressed
    while (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_1)==0){
    	y++;
       }
    // If the snooze button was held long enough, add 5 minutes to the alarm
    if (y>500000){
    	int z;
    	for (z=0; z<5; z++){
    		IncrementTimeA();
        }
    }
    // Clear the screen
    RIT128x96x4Clear();
    // Turn off the LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
    // Turn off the alarm
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, false);
    PWMGenDisable(PWM0_BASE, PWM_GEN_0);
    // Disable the interrupt so that snooze and turn off alarm cannot be used
    GPIOPinIntDisable(GPIO_PORTF_BASE, GPIO_PIN_1);
}


//*****************************************************************************
//
// Main Program
//
//*****************************************************************************
int
main(void)
{
	unsigned long ulPeriod;

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Initialize the OLED display and write status.
    RIT128x96x4Init(1000000);

    // Enable the peripherals used by this application
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);

    // Set GPIO F0 and G1 as PWM pins.  They are used to output the PWM0 and
    // PWM1 signals.
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1);

    // Compute the PWM period based on the system clock.
    ulPeriod = SysCtlClockGet() / 440;

    // Set the PWM period to 440 (A) Hz.
    PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
    		PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);

    // Set PWM0 to a duty cycle of 25% and PWM1 to a duty cycle of 75%.
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, ulPeriod / 4);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_1, ulPeriod * 3 / 4);

    // Enable the PWM0 and PWM1 output signals.
    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);

    // Configure the 'up' button as input and enable the pin to interrupt on
    // the falling edge (i.e. when the push button is pressed).
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0,
    		GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    // Configure the 'down' button as input and enable the pin to interrupt on
    // the falling edge (i.e. when the push button is pressed).
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
                         GPIO_PIN_TYPE_STD_WPU);

    // Configure the 'left' button as input and enable the pin to interrupt on
    // the falling edge (i.e. when the push button is pressed).
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA,
                         GPIO_PIN_TYPE_STD_WPU);

    // Configure the 'right' button as input and enable the pin to interrupt on
    // the falling edge (i.e. when the push button is pressed).
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA,
                         GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3,
        		GPIO_FALLING_EDGE);
    GPIOPortIntRegister(GPIO_PORTE_BASE, GPIOEIntHandler);
    GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    IntEnable(INT_GPIOE);

    // Configure the LED
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2);

    // Configure the select buttons as input and enable the pin to interrupt on
    // the falling edge (i.e. when the push button is pressed).
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_FALLING_EDGE);
    GPIOPortIntRegister(GPIO_PORTF_BASE, GPIOFIntHandler);
    GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_1);
    IntEnable(INT_GPIOF);

    // Initial time to display
    static char pcInitTime[10];
    usprintf(pcInitTime, "%d%d:%d%d AM", h2, h1, m2, m1);

    // Set the time between SysTick interrupts and register the interrupt handle
    SysTickPeriodSet(SysCtlClockGet());
    SysTickIntRegister(SysTickIntHandler);

    // Begin a blinking display of the initial start message
    while (!start){
    	int a;
        for (a=0; a<300000; a++){
        	if (a%20000==0){
        		RIT128x96x4StringDraw(pcInitTime, 40, 40, a/20000);
        	}
        }
    }

    // The clock has started so we can clear the message
    RIT128x96x4Clear();

    // Loop forever
    while(1)
    {
    	// Display the updated time
    	DisplayTime();
    	// Check if we should sound the alarm
    	TisA();
    }
}
