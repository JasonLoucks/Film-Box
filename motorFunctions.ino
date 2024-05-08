// controls the stepper motor
void runMotor()
{
    if ( runState == RUN_ON ) // if we are in an active run
    {
        updateTimer(); // update and display the time remaining in the run
        
        if ( ( millis() - runStart ) >= runTime ) // if the run time has elapsed
            endRun(); // end the run and return to the main menu
        else // if the run time hasn't elapsed
            agitate(); // perform intermittent agitation
    }

    if ( currentSpeed != newSpeed ) // if the speed has changed since the last step
    {
        stepper.setSpeed( newSpeed ); // update the speed, and
        currentSpeed = newSpeed;      // note the new speed
    }

    stepper.runSpeed(); // run the motor at the specified speed
}

// starts a new run
void startRun()
{
    newSpeed = maxSpeed;                   // start the motor,
    runTime = chosenRunTime * 60 * 1000ul; // set the run time to the user's chosen run time (converting minutes to ms), and
    cycleTime = 30 * 1000;                 // set the initial cycle time to 30 sec
    
    runStart = millis();                   // note the start time of this run, and
    runState = RUN_ON;                     // signal that the run is now active

    cycleStart = runStart;                 // note the start time of the initial cycle (setting it to runStart to ensure timers are synced),
    minuteStart = runStart;                // begin the 1-min timer for agitation,
    secondStart = runStart;                // begin the 1-sec timer for time display, and
    cycleState = CYCLE_ON;                 // signal that the inital cycle is now active

    menuLevel = runMenuLevel; // go to the run menu,
    updateMenu();             // update the display, and
    displayRunTimer();        // show the remaining time in the run

    Serial.print( "Starting run: " );
    Serial.print( runTime / 60 / 1000 );
    Serial.println( " min/" );
    Serial.print( "First cycle: " );
    Serial.print( cycleTime / 1000 );
    Serial.println( " sec" );
}

// ends the current run
void endRun()
{
    newSpeed = 0;           // stop the motor,
    runState = RUN_OFF;     // signal that the run is now inactive, and
    cycleState = CYCLE_OFF; // signal that the cycle is now inactive

    s = 0;             // reset the timer display
    m = chosenRunTime; //

    menuLevel = mainMenuLevel; // go to the main menu, and
    updateMenu();              // update the display

    Serial.println( "Ending run" );
}

// performs intermittent agitation cycles
void agitate()
{
    if ( cycleState == CYCLE_ON ) // if we are currently in an agitation cycle
    {
        if ( ( millis() - cycleStart ) >= cycleTime ) // if the cycle time has elapsed
        {
            newSpeed = 0;           // stop the motor,
            cycleState = CYCLE_OFF; // signal that we are no longer agitating, and
            
            Serial.print( cycleTime );
            Serial.print( " sec cycle has elapsed. Starting new cycle: " );
            Serial.print( cycleTime / 1000 );
            Serial.println( " sec" );
            
            if ( !initCycleHasRun ) // if this was the first agitation cycle
            {
                cycleTime = 10 * 1000;  // switch to 10-sec cycle, and
                initCycleHasRun = true; // signal that we are no longer on 30-sec cycle
            }
        }
    }
    else // if we are not currently in an agitation cycle
    {
        if ( ( millis() - minuteStart ) >= ( 60 * 1000ul ) ) // if one minute has elapsed
        {
            newSpeed = maxSpeed;      // start the motor,
            cycleStart = millis();    // note the new agitation cycle start time,
            minuteStart = cycleStart; // start a new timer for one minute, and
            cycleState = CYCLE_ON;    // signal that a cycle is now active

            Serial.print( "1 minute has elapsed. Starting new cycle: " );
            Serial.print( cycleTime / 1000 );
            Serial.println( " sec" );

            Serial.print( "Total time since start of run: " );
            Serial.print( ( millis() - runStart ) / 1000 );
            Serial.print( " sec. Running until ( millis() - runstart ) >= " );
            Serial.print( runTime / 1000 );
            Serial.println( " sec " );
        }
    }
}