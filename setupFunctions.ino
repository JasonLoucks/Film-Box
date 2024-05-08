// set up the lcd
void setUpLCD()
{
    lcd.init();      // initialize the lcd, and
    lcd.backlight(); // turn on the backlight
}

// set up the ir receiver
void setUpIR()
{
    irrecv.enableIRIn(); // turn on the receiver
}

// set up the motor
void setUpMotor()
{
    stepper.setMaxSpeed( maxSpeed ); // set the motor's max speed, and
    stepper.setSpeed( newSpeed );       // set its initial speed (0)
}

// set up the menu
void setUpMenu()
{
    menuLevel = mainMenuLevel; // set the current menu level to the main menu, and
    updateMenu();              // display the current menu's options
}