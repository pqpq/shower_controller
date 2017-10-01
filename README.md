# shower_controller
An Arduino based timer controller for a domestic electric shower.

## Why
Our eldest son is suffering from OCD and likes to spend a *long* time in the shower as part of his calming down process. This means we have to police his shower usage, whatever the time of day or night, to make sure he doesn't shower for too long. "Too long" can be an hour or more, if nobody is paying attention. Sometimes he is only ready for a shower at midnight. Or at 10am after everyone has left the house.

So we need an automatic system that can limit shower time, lock out the shower for a while, then automatically enable it again. All this has to be done kindly, so the user can see time passing and gets plenty of warnings so they are not surprised when the shower switches off.

Shower time has to be programmable so we can start with a longer time then slowly work down to what we think is reasonable. The programming has to be protected so you can't just open the cupboard and put it back to 1 hour when nobody is looking.

## Hardware
### Interface to existing shower
Solenoid water valve. NC (Normally Closed), so controller has to turn water on, then it can turn it off again when time’s up. If it was NO (Normally Open), when the controller decides to allow another shower, the shower would suddenly burst into life again with nobody there. Also NO would not be fail safe.

Hall effect sensor to detect current flow in shower cable? Can automatically tell when shower has ended so you don’t have to manually switch the unit off before it alarms

When plumbed in, have a bypass valve in case timer fails. This should need tools to operate otherwise it would be too easy to defeat the timer.

Power must come from fuse box side of the existing shower switch. Otherwise the unit can simply be reset by turning off the shower. The shower MCB will still protect the timer.

### User facing

Waterproof momentary push button to initiate a shower.

5v Piezo buzzer for alerting user

2x7 segment display to display minutes, and show last minute count down somehow

Green LED to indicate unit is on, and shower can start

Red LED to indicate no shower allowed

Reed switch to reset the unit using a magnet? Position this somewhere else? Will magnet work through tiles and plasterboard? This is a get out of jail free card, and also allows someone else to have a shower while the lockout is still active, if they have the magic touch.

### Back panel
+ and - buttons to change time? Simpler. Possible to use analogue I/P for these to free DIO.
_Old idea: Pot to dial in time. Analogue input? Probably OK. Display new time whenever this is being turned._

5 pin DIN plug with magic wiring to enable time change? In series with + and - switches to avoid a third input?
_Old ideas:_
* _Push to hold button while changing time, or push once after changing time?_
* _Reed switch so you can’t just change the time willy-nilly?_

Duplicate 7-seg to show time when programming? Only comes on when programming key enabled? Power it via a link in programming key? Bit of a luxury, and complex to build.

### Other
USB power supply for 5v?
Solid state relay to control the water valve?

### Pin usage
Arduino Uno:
EEPROM (remember setting)
14 DIO
6 A/D in

LEDs: 
* 10 out: 7seg + 1select (L not R) + 1 red + 1 green
* 9 out: 7seg + 2 mux select
Start button: 1 (in)
Security: 1 (in) 
Program: 2, or 3 (one toggle between shower time and lockout time?) (in)
Hall sensor: 1 analogue in
Reed relay (override): 1 adc or 1 digital in
Shower valve actuate: 1 out
Buzzer: 1 PWM out

**Total: 12 out, 5 or 6 Digital in, 1 Analogue in**

## “UI”

### Flow
Power on -> Idle
Idle:
* Solid green LED
* Water valve closed
* Show time, dim
* beep

Press button -> Water On
* water valve open
* beep (long?)
* Display initial time in mins
* display bright
* start "cold" timer
* Solid green LED

Cold timer expires -> idle

Detect shower current -> Shower running
* stop "cold" timer
* start timer
* flashing green LED
* beep

Shower running ->
* Count down time in mins
* Flashing green LED
* Short beep at 5 mins

Shower running, 1 minute to go ->
* Run countdown animation
* Flashing green LED
* Long beep(1s?) then short beep every 5 seconds
* Constant beeping last 10 seconds

Shower stops within time ->
* As shower running, but no beeps

Timer expires -> Lockout 
* (doesn’t matter if shower is on or off)

Lockout ->
* Flashing red LED
* Water valve closed
* Count down lockout in mins

Lockout complete -> Idle 

Dongle in -> Override

Override ->
* Succession of short beeps so you know it has registered
* Alternately flashing red/green LEDs
* Water valve open
* +/- buttons change time, short beep for each press
* Display time on 7seg, bright

Reset-> Idle
* Succession of short beeps so you reset has occurred


### 7 Segments
Show time in minutes.
Count down when shower in progress
Flash the dot on one to show system is live, or flash the green/red LED?

```
     _  _
    |_||_|
    |_||_|
```

At 1 minute:

```
     _  _     _  _     _  _     _  _     _  _     _  _ 
    |   _|   |  | |   | |  |   |_   |   |    |   |    |
    |_  _|   |_  _|   |_  _|   |_  _|   |_| _|   |_ |_|
```

Then
 
```
     _  _     _  _     _  _     _  _     _  _     _  _ 
    |   _|   |  | |   | |  |   |_   |   |    |   |    |
    |_  _    |_  _    |_  _    |_  _    |_| _    |_ |_ 
```

Etc., with 6 spoke windmill in the middle doing the seconds, and the outer ring counting down thusly:

```
     _  _     _  _     _  _     _                       
    |    |   |    |   |        |        |               
    |_  _|   |_  _    |_  _    |_  _    |_  _    |_  _    _  _       _ 
```

8 outer steps x 6 inner = 48 steps
Maybe...
Then one with entire outer ring flashing slow = 54 seconds
Then one with outer ring flashing fast = 60 seconds
Or. count down 10-0 in last 10 seconds whilst beeping angrily

Could use PWM to show full outline for part of cycle, dim, then flash the animated part for a bit.

OR ….
Just switch over to displaying seconds for last minute? Not so much fun though!

## Links
Valve: http://www.solenoid-valve.world/by-industry/water-industry/115BC-RPE-solenoid-valve

Current sense: https://www.proto-pic.co.uk/acs712-breakout.html

## Ideas
Multiplex the 7-seg onto the same 7 DIO lines. Enable each in turn, 50% duty cycle. 
7 lines for data, 1 for select, 2 for other LEDs = 10 

or... 

2 for binary select, with 4 destinations for 7 data: 2 7seg, 2 LEDs, lots spare = 9 but more HW to demux. Probably unnecessary

“Front panel” behind smoked perspex, 15x15 so it can be tiled into the shower enclosure

Hard code LED animation, don’t work it out programmatically!
Hard code PWM or flashing - just toggle between 2 arrays
Use spreadsheet to calculate byte values for this

Ribbon cable to front panel.

5-pin DIN “dongle” to enable override (always on, program times). This is just a fancy wiring jumble earthing an LED, and pulling an IO line low. Can also earth low side of +/- buttons so they just don’t work when dongle is out.

Glue buzzer to back of front panel - panel will be sound board. Use Arduino PWM channel, on 50% duty cycle, change frequency, to drive it?

Tune hall sensor so when shower is on but cold (i.e. heater isn’t drawing lots of amps), don’t trigger timer, so shower can be used for washing out etc.

Raise tone of beep as the last minute expires?!

Use PWM to generate tones, so we can free up DIO and/or CPU cycles? If not, tone library exists.



