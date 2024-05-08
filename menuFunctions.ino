// execute menu actions based on ir recveiver input
void runMenu()
{
    if ( ( irrecv.decode( &results ) ) &&                  // if we have received an ir signal, and
         ( ( millis() - timeOfLastPress ) >= delayTime ) ) // if there has been long enough between button presses
    {
        timeOfLastPress = millis(); // 0 days since last accident

        switch( results.value ) // which button has been pressed?
        {
            case IR_UP:        // up button
                menuUp();      // go up to previous menu item
                break;

            case IR_DOWN:      // down button
                menuDown();    // go down to next menu item
                break;

            case IR_OK:        // ok button, or
            case IR_RIGHT:     // right button (either will execute this)
                menuConfirm(); // choose the current menu selection
                break;

            case IR_LEFT:      // left button
                menuReturn();  // go up 1 menu level
                break;
        }

        irrecv.resume(); // resume listening to ir receiver
    }
}

// go up to the previous menu item
void menuUp()
{
    lcdClearChar( 0, cursorPos ); // clear the cursor temporarily

    if ( cursorPos == 1 ) // if the cursor is at the bottom
    {
        itemPos--;   // go up 1 menu item, and
        cursorPos--; // move the cursor to the botom
    }
    else // if the cursor is already at the top
    {
        if ( itemPos > 0 ) // if there are still menu items further up (offscreen)
        {
            lcdPrintAtPos( menu[ menuLevel ][ itemPos - 1 ], 1, 0 ); // move the first offscreen menu item down onto the screen,
            lcdPrintAtPos( menu[ menuLevel ][ itemPos--   ], 1, 1 ); // move the top menu item down to the bottom, and change the current menu item to the formerly offscreen item
        }
        // if we are at the first menu item, do nothing
    }

    lcdPrintAtPos( ">", 0, cursorPos ); // re-print the cursor
}

// go down to the next menu item
void menuDown()
{
    lcdClearChar( 0, cursorPos ); // clear the cursor temporarily

    if ( cursorPos == 0 ) // if the cursor is at the top
    {
        itemPos++;   // go down 1 menu item, and
        cursorPos++; // move the cursor to the bottom
    }
    else // if the cursor is already at the bottom
    {
        if ( ( itemPos < ( menuSize - 1 ) ) &&              // if there are still menu items further down (offscreen), and
             ( menu[ menuLevel ][ itemPos + 1 ] != "\0" ) ) // if those menu items are not null
        {
            lcdPrintAtPos( menu[ menuLevel ][   itemPos ], 1, 0 ); // move the bottom menu item up to the top,
            lcdPrintAtPos( menu[ menuLevel ][ ++itemPos ], 1, 1 ); // move the first offscreen menu item up onto the screen, and change the current menu item to that item
        }
        // if we are at the last menu item, do nothing
    }

    lcdPrintAtPos( ">", 0, cursorPos ); // re-print the cursor
}

// execute the current menu selection
void menuConfirm()
{
    if ( ( menu[ menuLevel ][ itemPos ] ) == "Start" ) // if the user wants to start a run
    {
        if ( runState == RUN_OFF ) // if we are not currently in a run. this may not be necessary now that we have a dedicated "cancel run" menu item
            startRun(); // start a new run and go to the run menu
    }
    else if ( ( menu[ menuLevel ][ itemPos ] ) == "Set duration" ) // if user wants to set the next run's duration
    {
        menuLevel++;  // enter the duration submenu
        updateMenu(); // display the duration menu's options
    }
    else if ( ( ( menu[ menuLevel ][ itemPos ] ) == "3 min" ) || // if the item chosen is one of the four duration options
              ( ( menu[ menuLevel ][ itemPos ] ) == "4 min" ) ||
              ( ( menu[ menuLevel ][ itemPos ] ) == "5 min" ) ||
              ( ( menu[ menuLevel ][ itemPos ] ) == "6 min" ) )
    {
        chosenRunTime = itemPos + 3; // set the run time based on the menu item index (works out to 3, 4, 5, or 6 minutes),
        m = chosenRunTime;           // update the displayed minutes, and
        menuReturn();                // go back to the main menu
    }
    else if ( ( menu[ menuLevel ][ itemPos ] ) == "Cancel run" ) // if the
    {
        if ( runState == RUN_ON ) // if we are currently in a run. this may not be necessary now that we have a dedicated "start" menu item
            endRun();             // end the run and go back to the main menu
    }
}

// return to the previous menu
void menuReturn()
{
    if ( menuLevel > mainMenuLevel ) // if we aren't at the main menu. this also prevents us from accidentally going into the run menu
    {
        menuLevel--;  // go up 1 menu level, and 
        updateMenu(); // display the new menu's options
    }
}

// update menu and the display the new menu's items
void updateMenu()
{
    itemPos = 0;                                   // go to the first item of the new menu,
    cursorPos = 0;                                 // move the cursor to match,
    lcd.clear();                                   // clear the display,
    lcdPrintAtPos( ">", 0, cursorPos );            // print the cursor,
    lcdPrintAtPos( menu[ menuLevel ][ 0 ], 1, 0 ); // display the new menu's first item, and
    lcdPrintAtPos( menu[ menuLevel ][ 1 ], 1, 1 ); // display the new menu's second item

    if  ( menuLevel == mainMenuLevel ) // if the new menu is the main menu
        displayRunTime(); // display the currently chosen run time
}

// update and display the time remaining every second
void updateTimer()
{
    if ( ( millis() - secondStart ) >= ( 1000ul ) ) // if one second has elapsed
    {
        secondStart = millis(); // start a new second

        if ( s > 0 ) // if we have at least 1 second left in the current minute
        {
            s--; // decrement seconds
        }
        else if ( s == 0 ) // if we have 0 seconds left in the current minute
        {
            if ( m > 0 ) // if we have at least 1 minute in the current run
            {
                s = 59; // roll seconds over to 59, and
                m--;    // decrement minutes
            }
            else // if the run is over
            {
                s = 0;             // reset the timer display
                m = chosenRunTime; //
            }
        }

        displayRunTimer();
    }
}

// display the time remaining in the current timer
void displayRunTimer()
{        
    lcd.setCursor( 10, 0 );
    if ( s >= 10 )
        lcd.print( ( String ) m + ":"  + s );
        //Serial.println( ( String ) m + ":"  + s );
    else
        lcd.print( ( String ) m + ":0" + s );
        //Serial.println( ( String ) m + ":0" + s );
}

// display the currently chosen run time as "(X:00)"
void displayRunTime()
{
    lcd.setCursor( 7, 0 );
    lcd.print( ( String ) "(" + chosenRunTime + ":00)" );
    /*
    lcdPrintAtPos( "(", 7, 0 );
    lcdPrintAtPos( menu[ 1 ][ chosenRunTime - 3 ], 8, 0 ); // convert chosenRunTime back to the appropriate menu item index to get the string value
    lcdPrintAtPos( ")", 13, 0 );
    */
}
