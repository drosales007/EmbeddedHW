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

extern void drawUnderscore(int x, int y);
extern void drawPointer(int y);

char *alpha[26] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};
char *selected[26] = {"!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!","!"};
char *words[] = {"BOBCAT", "IRONMAN", "FOOTBALL", "CATFISH", "STOMACH", "TEXAS", "LONGHORNS", "CHRISTMAS", "SEAHORSE", "PEANUT"};
int wotd = -1;
int wordLen = 0;
volatile int position = 0;
volatile int position2 = 0;
volatile int score = 0;
volatile long x = 0;
volatile int state = 0;
volatile int try = 0;
volatile int pointer = 0;
volatile int pointer2 = 0;
volatile int correct = 0;
volatile int tick = 0;
volatile int done = 0;
volatile int classic = 0;
volatile int continuous = 0;
volatile int lose = 0;

static const unsigned char g_pucLogo1[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const unsigned char g_pucLogo2[] = {0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F};
static const unsigned char g_pucLogo3[] = {0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F};
static const unsigned char g_pucLogo4[] = {0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F};
static const unsigned char g_pucLogo5[] = {0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F};
static const unsigned char g_pucLogo6[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const unsigned char g_pucLogo7[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo8[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo9[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo10[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo11[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo12[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo13[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo14[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo15[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo16[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo17[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo18[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo19[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo20[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo21[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo22[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo23[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo24[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo25[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo26[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo27[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo28[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo29[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo30[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo31[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo32[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo33[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo34[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo35[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo36[] = {0x00, 0x00, 0xF0, 0x0F};
static const unsigned char g_pucLogo37[] = {0x00, 0x00, 0xF0, 0x0F, 0xF0};
static const unsigned char g_pucLogo38[] = {0x00, 0x00, 0xF0, 0x0F, 0x0F};
static const unsigned char g_pucLogo39[] = {0x00, 0x00, 0xF0, 0x0F, 0x00, 0xF0};
static const unsigned char g_pucLogo40[] = {0x00, 0x00, 0xF0, 0x0F, 0xF0, 0x0F};
static const unsigned char g_pucLogo41[] = {0x00, 0x00, 0xF0, 0x0F, 0x0F, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00};
static const unsigned char g_pucLogo42[] = {0x00, 0x00, 0xF0, 0x0F, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00};
static const unsigned char g_pucLogo43[] = {0x00, 0x00, 0xF0, 0x0F, 0x00, 0x0F, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00};
static const unsigned char g_pucLogo44[] = {0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00};
static const unsigned char g_pucLogo45[] = {0x00, 0x00, 0xF0, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static const unsigned char g_pucLogo46[] = {0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F};
static const unsigned char g_pucLogo47[] = {0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F};
static const unsigned char g_pucLogo48[] = {0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static const unsigned char g_pucHangman1[] = {0xFF, 0xF0, 0x0F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman2[] = {0xF0, 0xF0, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman3[] = {0xF0, 0xF0, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman4[] = {0xF0, 0xF0, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman5[] = {0xF0, 0xFF, 0xFF, 0x0F, 0x00, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xF0, 0x00, 0x00, 0xFF, 0xF0, 0xFF, 0xF0, 0xFF, 0xF0, 0xFF, 0x00, 0x0F, 0xF0, 0x00, 0x00, 0xFF, 0xF0, 0xFF, 0xF0, 0xFF, 0xF0, 0xFF, 0x00, 0x00};
static const unsigned char g_pucHangman6[] = {0xF0, 0x00, 0x00, 0x0F, 0x00, 0xF0, 0x00, 0xFF, 0x0F, 0x0F, 0x0F, 0xF0, 0x0F, 0x00, 0x0F, 0x00, 0x0F, 0xF0, 0xF0, 0xF0, 0xFF, 0x00, 0xFF, 0xF0, 0x0F, 0x00, 0x0F, 0x00, 0x0F, 0xF0, 0xF0, 0xF0, 0xFF, 0x00, 0xF0, 0x00};
static const unsigned char g_pucHangman7[] = {0xF0, 0xFF, 0xFF, 0x0F, 0x0F, 0x00, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0xF0, 0xF0, 0x0F, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x0F, 0x00, 0x00, 0xF0, 0xF0, 0x0F, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0x0F, 0x00};
static const unsigned char g_pucHangman8[] = {0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x0F, 0x0F, 0x00, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0x00, 0xF0, 0xF0, 0x00, 0xFF, 0x0F, 0x00};
static const unsigned char g_pucHangman9[] = {0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x00, 0xF0, 0x0F, 0x0F, 0x0F, 0x00, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x00, 0x00, 0xF0, 0xF0, 0x0F, 0xF0, 0x00, 0xFF, 0x00, 0xF0, 0xF0, 0x0F, 0x00, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0x00};
static const unsigned char g_pucHangman10[] ={0xF0, 0xF0, 0x0F, 0x0F, 0x00, 0xF0, 0x00, 0xFF, 0x0F, 0x0F, 0x0F, 0x00, 0xF0, 0xF0, 0x0F, 0x00, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x00, 0xF0, 0xF0, 0x0F, 0x00, 0x0F, 0xF0, 0xF0, 0xF0, 0xF0, 0x0F, 0x0F, 0x00};
static const unsigned char g_pucHangman11[] ={0xFF, 0xF0, 0x0F, 0xFF, 0x00, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x00, 0xFF, 0xF0, 0x00, 0xFF, 0xF0, 0xF0, 0xF0, 0xFF, 0xF0, 0x0F, 0xFF, 0x00, 0xFF, 0xF0, 0x00, 0xFF, 0xF0, 0xFF, 0xF0, 0xFF, 0xF0, 0x0F, 0xFF, 0x00};
static const unsigned char g_pucHangman12[] ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman13[] ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xF0, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman14[] ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x0F, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman15[] ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x0F, 0xF0, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman16[] ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman17[] ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const unsigned char g_pucHangman18[] ={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char g_pucHead1[] = {0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00};
static const unsigned char g_pucHead2[] = {0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00};
static const unsigned char g_pucHead3[] = {0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00};
static const unsigned char g_pucHead4[] = {0x00, 0xF0, 0x00, 0xFF, 0xFF, 0xF0, 0x00, 0xF0, 0x00};
static const unsigned char g_pucHead5[] = {0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00};
static const unsigned char g_pucHead6[] = {0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00};
static const unsigned char g_pucHead7[] = {0x0F, 0x00, 0xF0, 0xF0, 0x00, 0xF0, 0xF0, 0x0F, 0x00};
static const unsigned char g_pucHead8[] = {0x0F, 0x00, 0x0F, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0x00};
static const unsigned char g_pucHead9[] = {0x0F, 0x00, 0xF0, 0xF0, 0x00, 0xF0, 0xF0, 0x0F, 0x00};
static const unsigned char g_pucHead10[] = {0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x00};
static const unsigned char g_pucHead11[] = {0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x00};
static const unsigned char g_pucHead12[] = {0x00, 0x00, 0xF0, 0x00, 0x00, 0x00, 0xF0, 0x00, 0x00};
static const unsigned char g_pucHead13[] = {0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00};

static const unsigned char g_pucUnderscoreClear[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
													 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
													 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
													 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
													 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
													 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
													 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
													 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const unsigned char g_pucPointer1[] = {0xF0, 0x00};
static const unsigned char g_pucPointer2[] = {0xFF, 0x00};
static const unsigned char g_pucPointer3[] = {0xFF, 0xF0};
static const unsigned char g_pucPointer4[] = {0xFF, 0xFF};
static const unsigned char g_pucPointer5[] = {0xFF, 0xF0};
static const unsigned char g_pucPointer6[] = {0xFF, 0x00};
static const unsigned char g_pucPointer7[] = {0xF0, 0x00};

static const unsigned char g_clearPointer[] = {0x00, 0x00};

static const unsigned char g_pucLeftArm[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xF0};
static const unsigned char g_pucRightArm[] = {0xFF, 0xFF, 0xFF, 0xFF};

static const unsigned char g_pucUnderscore[] = {0xFF, 0xFF, 0xF0};

static const unsigned char g_pucBody[] = {0xF0};

static const unsigned char g_pucLeg1[] = {0xF0};
static const unsigned char g_pucLeg2[] = {0x0F};

static void drawLogo(void){
	// Method for displaying the Hangman logo
	int x0 = (128-72)/2;
	RIT128x96x4ImageDraw(g_pucHangman1, x0, 20, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman2, x0, 21, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman3, x0, 22, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman4, x0, 23, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman5, x0, 24, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman6, x0, 25, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman7, x0, 26, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman8, x0, 27, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman9, x0, 28, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman10, x0, 29, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman11, x0, 30, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman12, x0, 31, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman13, x0, 32, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman14, x0, 33, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman15, x0, 34, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman16, x0, 35, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman17, x0, 36, 72, 1);
	RIT128x96x4ImageDraw(g_pucHangman18, x0, 37, 72, 1);
}

static void displayAlphabet(void){
	// Method for displaying the alphabet on the bottom of the OLED
	RIT128x96x4StringDraw("A", 0, 75, 15);
	RIT128x96x4StringDraw("B", 10, 75, 15);
	RIT128x96x4StringDraw("C", 20, 75, 15);
    RIT128x96x4StringDraw("D", 30, 75, 15);
	RIT128x96x4StringDraw("E", 40, 75, 15);
    RIT128x96x4StringDraw("F", 50, 75, 15);
	RIT128x96x4StringDraw("G", 60, 75, 15);
    RIT128x96x4StringDraw("H", 70, 75, 15);
	RIT128x96x4StringDraw("I", 80, 75, 15);
    RIT128x96x4StringDraw("J", 90, 75, 15);
	RIT128x96x4StringDraw("K", 100, 75, 15);
    RIT128x96x4StringDraw("L", 110, 75, 15);
	RIT128x96x4StringDraw("M", 120, 75, 15);
    RIT128x96x4StringDraw("N", 0, 87, 15);
	RIT128x96x4StringDraw("O", 10, 87, 15);
    RIT128x96x4StringDraw("P", 20, 87, 15);
	RIT128x96x4StringDraw("Q", 30, 87, 15);
    RIT128x96x4StringDraw("R", 40, 87, 15);
	RIT128x96x4StringDraw("S", 50, 87, 15);
    RIT128x96x4StringDraw("T", 60, 87, 15);
    RIT128x96x4StringDraw("U", 70, 87, 15);
	RIT128x96x4StringDraw("V", 80, 87, 15);
    RIT128x96x4StringDraw("W", 90, 87, 15);
    RIT128x96x4StringDraw("X", 100, 87, 15);
    RIT128x96x4StringDraw("Y", 110, 87, 15);
    RIT128x96x4StringDraw("Z", 120, 87, 15);
}

static void drawGallow(void){
	// method for displaying the gallow
	RIT128x96x4ImageDraw(g_pucLogo1, 10, 50, 32, 1);
	RIT128x96x4ImageDraw(g_pucLogo2, 10, 49, 32, 1);
	RIT128x96x4ImageDraw(g_pucLogo3, 10, 48, 32, 1);
    RIT128x96x4ImageDraw(g_pucLogo4, 10, 47, 32, 1);
    RIT128x96x4ImageDraw(g_pucLogo5, 10, 46, 32, 1);
    RIT128x96x4ImageDraw(g_pucLogo6, 10, 45, 32, 1);
    RIT128x96x4ImageDraw(g_pucLogo7, 10, 44, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo8, 10, 43, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo9, 10, 42, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo10, 10, 41, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo11, 10, 40, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo12, 10, 39, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo13, 10, 38, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo14, 10, 37, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo15, 10, 36, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo16, 10, 35, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo17, 10, 34, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo18, 10, 33, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo19, 10, 32, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo20, 10, 31, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo21, 10, 30, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo22, 10, 29, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo23, 10, 28, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo24, 10, 27, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo25, 10, 26, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo26, 10, 25, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo27, 10, 24, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo28, 10, 23, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo29, 10, 22, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo30, 10, 21, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo31, 10, 20, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo32, 10, 19, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo33, 10, 18, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo34, 10, 17, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo35, 10, 16, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo36, 10, 15, 8, 1);
    RIT128x96x4ImageDraw(g_pucLogo37, 10, 14, 10, 1);
    RIT128x96x4ImageDraw(g_pucLogo38, 10, 13, 10, 1);
    RIT128x96x4ImageDraw(g_pucLogo39, 10, 12, 12, 1);
    RIT128x96x4ImageDraw(g_pucLogo40, 10, 11, 12, 1);
    RIT128x96x4ImageDraw(g_pucLogo41, 10, 10, 28, 1);
    RIT128x96x4ImageDraw(g_pucLogo42, 10, 9, 28, 1);
    RIT128x96x4ImageDraw(g_pucLogo43, 10, 8, 28, 1);
    RIT128x96x4ImageDraw(g_pucLogo44, 10, 7, 28, 1);
    RIT128x96x4ImageDraw(g_pucLogo45, 10, 6, 32, 1);
    RIT128x96x4ImageDraw(g_pucLogo46, 10, 5, 32, 1);
    RIT128x96x4ImageDraw(g_pucLogo47, 10, 4, 32, 1);
    RIT128x96x4ImageDraw(g_pucLogo48, 10, 3, 32, 1);
}

static void drawHead(void){
	// Method for displaying the head of the hangman
	RIT128x96x4ImageDraw(g_pucHead13, 28, 11, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead12, 28, 12, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead11, 28, 13, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead10, 28, 14, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead9, 28, 15, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead8, 28, 16, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead7, 28, 17, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead6, 28, 18, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead5, 28, 19, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead4, 28, 20, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead3, 28, 21, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead2, 28, 22, 18, 1);
	RIT128x96x4ImageDraw(g_pucHead1, 28, 23, 18, 1);
}

static void drawBody(void){
	// Method for displaying the body of the hangman
	RIT128x96x4ImageDraw(g_pucBody, 36, 24, 2, 1);
	RIT128x96x4ImageDraw(g_pucBody, 36, 25, 2, 1);
	RIT128x96x4ImageDraw(g_pucBody, 36, 26, 2, 1);
	RIT128x96x4ImageDraw(g_pucBody, 36, 27, 2, 1);
	RIT128x96x4ImageDraw(g_pucBody, 36, 28, 2, 1);
	RIT128x96x4ImageDraw(g_pucBody, 36, 29, 2, 1);
	RIT128x96x4ImageDraw(g_pucBody, 36, 30, 2, 1);
	RIT128x96x4ImageDraw(g_pucBody, 36, 31, 2, 1);
	RIT128x96x4ImageDraw(g_pucBody, 36, 32, 2, 1);
}

static void drawLeftLeg(void){
	// Method for displaying the left leg of the hangman
	RIT128x96x4ImageDraw(g_pucLeg2, 36, 33, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg1, 38, 34, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg2, 38, 35, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg1, 40, 36, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg2, 40, 37, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg1, 42, 38, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg2, 42, 39, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg1, 44, 40, 2, 1);
}

static void drawRightLeg(void){
	// Method for displaying the right leg of the hangman
	RIT128x96x4ImageDraw(g_pucLeg2, 34, 33, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg1, 34, 34, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg2, 32, 35, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg1, 32, 36, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg2, 30, 37, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg1, 30, 38, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg2, 28, 39, 2, 1);
	RIT128x96x4ImageDraw(g_pucLeg1, 28, 40, 2, 1);
}

static void drawLeftArm(void){
	// Method for displaying the left arm of the hangman
	RIT128x96x4ImageDraw(g_pucLeftArm, 36, 27, 10, 1);
}

static void drawRightArm(void){
	// Method for displaying the right arm of the handman
	RIT128x96x4ImageDraw(g_pucRightArm, 28, 27, 8, 1);
}

static void drawUnderscore(int x, int y){
	// Method for displaying an underline
	RIT128x96x4ImageDraw(g_pucUnderscore, x, y, 6, 1);
}

static void removeUnderscore(int x, int y){
	// Method for removing an underscore from the display
	RIT128x96x4ImageDraw(g_pucUnderscoreClear, x, y, 128, 1);
}

static void moveCursor(void){
	// Method for moving the cursor in the alphabet section.
	// Cursor go on line 83 or 95
	int a = position * 10;
	int b = 83;
	if (a>120){
		int a2 = position2 * 10;
		int b2 = 95;
		removeUnderscore(0, 83);
		removeUnderscore(0, 95);
		drawUnderscore(a2, b2);
	}
	else {
		removeUnderscore(0, 83);
		removeUnderscore(0, 95);
		drawUnderscore(a, b);
	}
	x = 0;
}

static void drawHiddenWord(void){
	// Method for displaying underscores for each letter in the word
	int num = abs(urand() % 10);
	char *word = words[num];
	int x = 10;
	int y = 60;
	int len = strlen(word);
	int i;
	for (i=0; i<len; i++){
		drawUnderscore(x,y);
		x += 10;
	}
	wotd = num;
	wordLen = len;
}

static void initMain(void){
	// Method for displaying the screen for the main menu
	drawLogo();
	RIT128x96x4StringDraw("Classic", 10, 50, 15);
	RIT128x96x4StringDraw("Continuous", 10, 60, 15);
	RIT128x96x4StringDraw("Instructions", 10, 70, 15);
	RIT128x96x4StringDraw("High Scores", 10, 80, 15);
	drawPointer(50);
}

static void drawPointer(int y){
	// Method for drawing a pointer
	RIT128x96x4ImageDraw(g_pucPointer1, 0, y, 4, 1);
	RIT128x96x4ImageDraw(g_pucPointer2, 0, y+1, 4, 1);
	RIT128x96x4ImageDraw(g_pucPointer3, 0, y+2, 4, 1);
	RIT128x96x4ImageDraw(g_pucPointer4, 0, y+3, 4, 1);
	RIT128x96x4ImageDraw(g_pucPointer5, 0, y+4, 4, 1);
	RIT128x96x4ImageDraw(g_pucPointer6, 0, y+5, 4, 1);
	RIT128x96x4ImageDraw(g_pucPointer7, 0, y+6, 4, 1);
}

static void clearPointer(int y){
	// Method for clearing a pointer
	int i;
	for (i=0; i<50; i++){
		RIT128x96x4ImageDraw(g_clearPointer, 0, y+i, 4, 15);
	}
}

static void resetClassic(void){
	// Method for resetting all the variables for the game
	int i;
	for (i=0; i<26; i++){
		selected[i] = "!";
	}
	try = 0;
	wotd = -1;
	correct = 0;
	position = 0;
	position2 = 0;
	RIT128x96x4Clear();
}

static void resetContinuous(void){
	// Method for resetting all the variables for the game
	int i;
	for (i=0; i<26; i++){
		selected[i] = "!";
	}
	wotd = -1;
	correct = 0;
	position = 0;
	position2 = 0;
	RIT128x96x4Clear();
}

static void startContinuous(void){
	// Method for continuing the Continuous mode
	resetContinuous();
	drawGallow();
	int i;
	for (i=0; i<try; i++){
		if (i==0){
			drawHead();
		}
		else if (i==1){
			drawBody();
		}
		else if (i==2){
			drawRightArm();
		}
		else if (i==3){
			drawLeftArm();
		}
		else if (i==4){
			drawRightLeg();
		}
	}
	static char puc_score[10];
	usprintf(puc_score, "Score: %d", score);
    RIT128x96x4StringDraw(puc_score, 69, 0, 15);
	displayAlphabet();
	drawUnderscore(0, 83);
	drawHiddenWord();
}

static void startClassic(void){
	// Method for starting the Classic mode
	resetClassic();
	drawGallow();
	static char puc_score[10];
	usprintf(puc_score, "Score: %d", score);
    RIT128x96x4StringDraw(puc_score, 69, 0, 15);
	displayAlphabet();
    drawUnderscore(0, 83);
	drawHiddenWord();
}

static void displayInstructions(void){
	// Method for displaying instructions to the user
	RIT128x96x4StringDraw("Uncover all the", 0, 0, 15);
	RIT128x96x4StringDraw("hidden characters to!", 0, 10, 15);
	RIT128x96x4StringDraw("complete each level!", 0, 20, 15);
	RIT128x96x4StringDraw("To move the cursor,", 0, 40, 15);
	RIT128x96x4StringDraw("press left or right.", 0, 50, 15);
	RIT128x96x4StringDraw("To select a letter,", 0, 70, 15);
	RIT128x96x4StringDraw("press select", 0, 80, 15);
}

static void displayScores(void){
	// Method for displaying high scores
	RIT128x96x4StringDraw("High Scores", 40, 0, 15);
	static char s1[24];
	usprintf(s1, "Classic Mode: %d", classic);
	RIT128x96x4StringDraw(s1, 0, 40, 15);
	static char s2[24];
	usprintf(s2, "Continuous Mode: %d", continuous);
	RIT128x96x4StringDraw(s2, 0, 60, 15);
}


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
	// Handle state changes
	if (done){
		tick++;
		if (tick>1){
			if (lose){
				// Turn off the noise
				PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, false);
				PWMGenDisable(PWM0_BASE, PWM_GEN_0);
				unsigned long ulPeriod = SysCtlClockGet() / 220;

				// Set the PWM period to 220 (A) Hz.
				PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
						PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
				PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);

				// Make some noise again
				PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);
				PWMGenEnable(PWM0_BASE, PWM_GEN_0);
			}
		}
		if (tick>2){
			if (lose){
				// Turn off the noise
				PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, false);
				PWMGenDisable(PWM0_BASE, PWM_GEN_0);

				unsigned long ulPeriod = SysCtlClockGet() / 440;
				// Set the PWM period to 440 (A) Hz. again
				PWMGenConfigure(PWM0_BASE, PWM_GEN_0,
						PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
				PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, ulPeriod);
				lose = 0;
			}
			if (state==1){
				startClassic();
				tick = 0;
				done = 0;
			}
			else if(state==2){
				if(tick>4){
					RIT128x96x4Clear();
					initMain();
					state = 0;
					pointer = 0;
					tick = 0;
					done = 0;
				}
			}
			else if (state==3){
				startContinuous();
				tick = 0;
				done = 0;
			}
		}
	}
}

void
GPIOEIntHandler(void)
{
	// Method for handling directional pad interrupts. The left and right
	// buttons allow the user to move the alphabet cursor left and right. This
	// behavior is handled here.

	// Clear the GPIO interrupt
	GPIOPinIntClear(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	// Disable Interrupts
	GPIOPinIntDisable(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	if (state==0){
		while(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0)==0 ||
			GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)==0){
			// Increment the counter
			x++;
			if (x>19999){
				if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_0)==0){
					if (pointer!=0){
						clearPointer(50+(pointer*10));
						pointer--;
						drawPointer(50+(pointer*10));
					}
					x = 0;
					break;
				}
				if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_1)==0){
					if (pointer!=3){
						clearPointer(50+(pointer*10));
						pointer++;
						drawPointer(50+(pointer*10));
					}
					x = 0;
					break;
				}
			}
		}
		x = 0;
	}
	else if (state==1 || state==3){
		// Count to 15000 so that we don't move the cursor too fast
		while(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)==0 ||
				GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3)==0){
			// Increment the counter
			x++;
			if (x>14999){
				// Check to see which button was pressed
				if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)==0){
					// Check if the cursor is not in the original position on the
					// top row. If it is, we do nothing
					if (position>0){
						// Check if the cursor is not in the original position on
						// the bottom row
						if (position2>0){
							// Move the cursor one spot to the left
							position2--;
						}
						else {
							// Move the cursor one spot to the left
							position--;
						}
					}
				}
				else if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_3)==0){
					// Check if the cursor is at the last possible position on the
					// bottom row. If it is, we do nothing
					if (position2<12){
						// Check if the cursor is at the last spot on the top row
						if (position<13){
							// Move the cursor one spot to the right
							position++;
						}
						else {
							// Move the cursor one spot to the right
							position2++;
						}
					}
				}
				// Update the position of the cursor
				moveCursor();
				break;
			}
		}
		x = 0;
	}

	// Enable Interrupts
	GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

void
GPIOFIntHandler(void){
	// Method for handling multiple functions for a select button press

	// Clear the GPIO interrupt
	GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_1);

	// Disable Interrupts
	GPIOPinIntDisable(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinIntDisable(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	// Check which state we are in
	if (state==0){
		// This state handles the main menu

		if (pointer==0){
			// Begin Classic Mode
			state = 1;
			startClassic();
		}
		else if (pointer==1){
			// Begin Continuous Mode
			state = 3;
			startClassic();
		}
		else if (pointer==2){
			// Show the instructions
			state = 2;
			RIT128x96x4Clear();
			displayInstructions();
			done = 1;
		}
		else if (pointer==3){
			// Show the high scores
			state = 2;
			RIT128x96x4Clear();
			displayScores();
			done = 1;
		}
		else if (pointer==4){
			state = 0;
			initMain();
		}
	}
	else if (state==1){
		// This state handles classic mode

		// Black out the letter that was selected
		int idx = position + position2;
		int pos = 0;
		char *puc_letter = alpha[idx];
		if (idx>12){
			pos = position2 * 10;
			RIT128x96x4StringDraw(puc_letter, pos, 87, 2);
		}
		else {
			pos = position * 10;
			RIT128x96x4StringDraw(puc_letter, pos, 75, 2);
		}

		// Add the letter to the list of selected letters
		int i;
		int wrong = 1;
		int used = 0;
		// Loop through the list until we find an empty spot to place the letter
		for (i=0; i<26; i++){
			if (strcmp(selected[i],"!")==0){
				selected[i] = puc_letter;
				break;
			}
			if (strcmp(selected[i],puc_letter)==0){
				wrong = 0;
				used = 1;
				break;
			}
		}

		// Check to see if the letter was already used
		if (!used){
			// Check the word to see if a letter matches the one selected. If it
			// does, we need to display the letters instead of an underscore
			for (i=0; i<strlen(words[wotd]); i++){
				char w_let = words[wotd][i];
				static char g[3];
				usprintf(g, "%d", w_let);
				char p_let = *puc_letter;
				if (w_let==p_let){
					wrong = 0;
					// Display the letter selected
					RIT128x96x4StringDraw(puc_letter, 10+i*10, 53, 15);
					correct++;
				}
			}
		}

		// Check to see if it was a wrong selection
		if (wrong==1){
			// Increment the number of wrong attempts
			try++;
			// If the selection was wrong, we need to draw a piece of the hangman
			if (try==1){
				drawHead();
			}
			else if (try==2){
				drawBody();
			}
			else if (try==3){
				drawRightArm();
			}
			else if (try==4){
				drawLeftArm();
			}
			else if (try==5){
				drawRightLeg();
			}
			else if (try==6){
				drawLeftLeg();
				RIT128x96x4StringDraw("                   ", 0, 53, 15);
				RIT128x96x4StringDraw(words[wotd], 20, 53, 2);
				RIT128x96x4StringDraw("!!!!GAME OVER!!!", 23, 30, 15);
				if (score > classic){
					classic = score;
				}
				score = 0;
				state = 2;
				done = 1;
				lose = 1;
				// Make some noise
				PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);
				PWMGenEnable(PWM0_BASE, PWM_GEN_0);
			}
		}
		else if (correct==wordLen){
			correct = 0;
			RIT128x96x4StringDraw("!!!!YOU WON!!!!", 26, 30, 15);
			score++;
			done = 1;
		}
	}
	else if (state==3){
		// This state handles Continuous mode

		// Black out the letter that was selected
		int idx = position + position2;
		int pos = 0;
		char *puc_letter = alpha[idx];
		if (idx>12){
			pos = position2 * 10;
			RIT128x96x4StringDraw(puc_letter, pos, 87, 2);
		}
		else {
			pos = position * 10;
			RIT128x96x4StringDraw(puc_letter, pos, 75, 2);
		}

		// Add the letter to the list of selected letters
		int i;
		int wrong = 1;
		int used = 0;
		// Loop through the list until we find an empty spot to place the letter
		for (i=0; i<26; i++){
			if (strcmp(selected[i],"!")==0){
				selected[i] = puc_letter;
				break;
			}
			if (strcmp(selected[i],puc_letter)==0){
				wrong = 0;
				used = 1;
				break;
			}
		}

		// Check to see if the letter was already used
		if (!used){
			// Check the word to see if a letter matches the one selected. If it
			// does, we need to display the letters instead of an underscore
			for (i=0; i<strlen(words[wotd]); i++){
				char w_let = words[wotd][i];
				static char g[3];
				usprintf(g, "%d", w_let);
				char p_let = *puc_letter;
				if (w_let==p_let){
					wrong = 0;
					// Display the letter selected
					RIT128x96x4StringDraw(puc_letter, 10+i*10, 53, 15);
					correct++;
				}
			}
		}

		// Check to see if it was a wrong selection
		if (wrong==1){
			// Increment the number of wrong attempts
			try++;
			// If the selection was wrong, we need to draw a piece of the hangman
			if (try==1){
				drawHead();
			}
			else if (try==2){
				drawBody();
			}
			else if (try==3){
				drawRightArm();
			}
			else if (try==4){
				drawLeftArm();
			}
			else if (try==5){
				drawRightLeg();
			}
			else if (try==6){
				drawLeftLeg();
				RIT128x96x4StringDraw("                   ", 0, 53, 15);
				RIT128x96x4StringDraw(words[wotd], 20, 53, 2);
				RIT128x96x4StringDraw("!!!!GAME OVER!!!", 23, 30, 15);
				if (score > continuous){
					continuous = score;
				}
				score = 0;
				state = 2;
				done = 1;
				lose = 1;
				// Make some noise
				PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT, true);
				PWMGenEnable(PWM0_BASE, PWM_GEN_0);
			}
		}
		else if (correct==wordLen){
			correct = 0;
			RIT128x96x4StringDraw("!!!!YOU WON!!!!", 26, 30, 15);
			score++;
			done = 1;
		}
	}

	// Enable Interrupts
	GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
}

//*****************************************************************************
//
// Main
//
//*****************************************************************************
int
main(void)
{
	unsigned long ulPeriod;

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);

    // Initialize the OLED display.
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
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA,
    		GPIO_PIN_TYPE_STD_WPU);

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
    GPIOIntTypeSet(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
    		GPIO_FALLING_EDGE);
    GPIOPortIntRegister(GPIO_PORTE_BASE, GPIOEIntHandler);
    GPIOPinIntEnable(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
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

    // Set the time between SysTick interrupts and register the interrupt handle
    SysTickPeriodSet(SysCtlClockGet()/1.2);
    SysTickIntRegister(SysTickIntHandler);
    SysTickEnable();

    initMain();

    while(1)
    {

    }
}
