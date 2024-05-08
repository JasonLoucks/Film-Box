////////////////////////////////////////////////////////////////////////////////
//
// FilmBox.ino
//
// This is the code for The Yard's ME 138 group project, Film Box. Film Box is
// intended to be a 3D printed film development tank with automated agitation.
//
// While there is no active run, the user can select from four preset run
// durations (3, 4, 5, or 6 minutes - default of 4 minutes). While there is an
// active run, a countdown timer with the time remaining in the run is displayed
// as well as an option to cancel the run.
//
// During each run, the motor performs an agitation cycle at the top of every
// minute. The first cycle is 30 seconds, and all subsequent cycles are 10
// seconds. For example, a 4-minute run would agitate at the following times:
// 0:00-0:30, 1:00-1:10, 2:00-2:10, and 3:00-3:10.
//
// After a run ends or is canceled, the user is sent back to the main menu and
// all timers are reset.
//
// Controlled using a standard IR remote. Controls:
// - OK/Right: Confirm current selection
// - Left: Return to previous menu (without confirming)
// - Up: Previous menu item
// - Down: Next menu item
//
// Todo:
// - Update LiquidCrystal_I2C to current version
// - General code cleanup/optimization
//   - Do there need to be so many global variables?
// - Create a Menu class to replace the current array system
// - Add automatic film loading - probably needs another sensor
// - Add indicator LEDs/buzzers
// - Add license?
//
////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <AccelStepper.h>

// set these based on your personal setup
const int irPin = 9; // ir receiver pin

const int IN1 = 10; // motor pins
const int IN2 = 11;
const int IN3 = 12;
const int IN4 = 13;

const int maxSpeed = 500; // max motor speed. motor seems to be unreliable above 500 and doesn't work at all above ~740

// lcd
LiquidCrystal_I2C lcd( 0x27, 16, 2 ); // lcd object

// motor
AccelStepper stepper( AccelStepper::FULL4WIRE, IN1, IN3, IN2, IN4 ); // motor object
int       currentSpeed =   0;                                        // current motor speed, used by runMotor() to detect changes in speed
int       newSpeed =       0;                                        // new motor speed in steps/sec, used by stepper.setSpeed() to set the motor's speed

// ir remote/receiver
IRrecv irrecv( irPin );
decode_results results;
const unsigned long IR_UP    = 0xFF629D;
const unsigned long IR_RIGHT = 0xFFC23D;
const unsigned long IR_DOWN  = 0xFFA857;
const unsigned long IR_LEFT  = 0xFF22DD;
const unsigned long IR_OK    = 0xFF02FD;
const unsigned long IR_1     = 0xFF6897;
const unsigned long IR_2     = 0xFF9867;
const unsigned long IR_3     = 0xFFB04F;
const unsigned long IR_4     = 0xFF30CF;
const unsigned long IR_5     = 0xFF18E7;
const unsigned long IR_6     = 0xFF7A85;
const unsigned long IR_7     = 0xFF10EF;
const unsigned long IR_8     = 0xFF38C7;
const unsigned long IR_9     = 0xFF5AA5;
const unsigned long IR_0     = 0xFF4AB5;
const unsigned long IR_STAR  = 0xFF42BD;
const unsigned long IR_POUND = 0xFF52AD;

// menu
int menuLevel; // array index of the current menu level
int itemPos;   // internal menu item index
int cursorPos; // cursor display position

const int menuSize = 4;          // max number of items in any submenu. needs to be manually set
const int runMenuLevel = 0;      // array index of the run menu level. needs to be manually set
const int mainMenuLevel = 1;     // array index of the main menu level. needs to be manually set
const char *menu[][ menuSize ] = // menu items. null items are not part of the menu - they are just there to make the coding easier
{
    {
        "Running: ",
        "Cancel run",
        "\0",
        "\0"

    },
    
    {
        "Start",        // main menu items
        "Set duration",
        "\0",
        "\0"
    },
    {
        "3 min",        // duration menu items
        "4 min",
        "5 min",
        "6 min"
    }
};

// timers, states, and other trackers
unsigned long timeOfLastPress;           // the last time a button input was received, compared to millis() and delayTime
const int     delayTime = 0.1 * 1000;    // only allow a button press every 0.1 sec

unsigned long runStart;                  // start time of the current run
unsigned long cycleStart;                // start time of the current agitation cycle
unsigned long minuteStart;               // start time of the current minute
unsigned long secondStart;               // start time of the current second

unsigned long runTime;                   // duration of the current run
unsigned long cycleTime;                 // duration of the current cycle, either 30 sec or 10 sec

int           chosenRunTime = 4;         // user's selected duration, default 4 min. this is always in minutes so must be converted when assigning its value to runTime
int           s = 0;                     // timer display, seconds component
int           m = chosenRunTime;         // timer display, minutes component, initially equal to chosenRunTime

bool          initCycleHasRun = false;   // first cycle is 30 sec, 10 sec every minute thereafter

enum { RUN_OFF, RUN_ON } runState;       // state of the current run
enum { CYCLE_OFF, CYCLE_ON } cycleState; // state of the current cycle

void setup()
{
    Serial.begin( 9600 );

    Serial.print( "Setting up LCD... " );
    setUpLCD(); 
    Serial.println( "LCD set up");

    Serial.print( "Setting up IR... " );
    setUpIR();
    Serial.println( "IR set up" );

    Serial.print( "Setting up menu... ");
    setUpMenu();
    Serial.println( "Menu set up" );

    Serial.print( "Setting up motor... " );
    setUpMotor();
    Serial.println( "Motor set up" );
}

void loop()
{
    runMenu();  // listen for ir input, and
    runMotor(); // run the motor based on user's choices
}