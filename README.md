# Film-Box

## Description
This is the code for The Yard's ME 138 group project, Film Box. Film Box is intended to be a 3D printed film development tank with automated agitation.

While there is no active run, the user can select from four preset run durations (3, 4, 5, or 6 minutes - default of 4 minutes). While there is an active run, a countdown timer with the time remaining in the run is displayed, as well as an option to cancel the run.

During each run, the motor performs an agitation cycle at the top of every minute. The first cycle is 30 seconds, and all subsequent cycles are 10 seconds. For example, a 4-minute run would agitate at the following times: 0:00-0:30, 1:00-1:10, 2:00-2:10, and 3:00-3:10.

After a run ends or is canceled, the user is sent back to the main menu and all timers are reset.

## How to use:
### Controls
Controlled using a standard IR remote:
- OK/Right: Confirm current selection
- Left: Return to previous menu (without confirming)
- Up: Previous menu item
- Down: Next menu item
### Running the code yourself
Download all the files (Code -> Download ZIP or from [Releases](https://github.com/JasonLoucks/Film-Box/releases)) and extract to a location of your choosing. All five .ino files must be present inside the FilmBox folder. All required libraries can be installed from the Arduino IDE's Library Manager (Ctrl+Shift+I or Sketch -> Include Library -> Manage Libraries). Currently, the installed IRRemote library MUST be version 2.6.0 or below. The IR sensor will not work if you attempt to use the current version (4.3.1).

Set up the circuit according to the diagram/photos (todo: add diagram/photos), then compile using the Arduino IDE and upload to your Arduino (currently, it has only been tested on an Uno Rev3). The program will automatically start with the Arduino.

## Todo:
- Add hardware requirements/circuit diagram/setup photos
- Update LiquidCrystal_I2C to current version
- General code cleanup/optimization
  - Do there need to be so many global variables?
- Create a Menu class to replace the current array system
- Add automatic film loading - probably needs another sensor
- Add indicator LEDs/buzzers
- Add license?
