# shower_controller
An Arduino based timer controller for a domestic electric shower.

## Why
Our eldest son is suffering from OCD and likes to spend a *long* time in the shower as part of his calming down process. This means we have to police his shower usage, whatever the time of day or night, to make sure he doesn't shower for too long. "Too long" can be an hour or more, if nobody is paying attention. Sometimes he is only ready for a shower at midnight. Or at 10am after everyone has left the house.

So we need an automatic system that can limit shower time, lock out the shower for a while, then automatically enable it again. All this has to be done kindly, so the user can see time passing and gets plenty of warnings so they are not surprised when the shower switches off.

Shower time has to be programmable so we can start with a longer time then slowly work down to what we think is reasonable. The programming has to be protected so you can't just open the cupboard and put it back to 1 hour when nobody is looking.

## Hardware
### Interface to existing shower
Solenoid water valve. NC (Normally Closed), so controller has to turn water on, then it can turn it off again when time’s up. If it was NO (Normally Open), when the controller decides to allow another shower, the shower would suddenly burst into life again with nobody there. Also NO would not be fail safe.

Hall effect sensor to detect current flow in shower cable? Can automatically tell when shower has ended so you don’t have to manually switch the unit off before it alarms. *This is a complicated extra that we don't need yet.*

When plumbed in, have a bypass valve in case timer fails. This should need tools to operate otherwise it would be too easy to defeat the timer.

Power must come from fuse box side of the existing shower switch. Otherwise the unit can simply be reset by turning off the shower. The shower MCB will still protect the timer.

### User facing

Waterproof momentary push button to initiate a shower.

5v Piezo buzzer for alerting user

2x7 segment display to display minutes, and show last minute count down somehow

Seven segment decimal point LED to indicate unit is on, and shower can start.

Reed switch to reset the unit using a magnet? Position this somewhere else? Will magnet work through tiles and plasterboard? This is a get out of jail free card, and also allows someone else to have a shower while the lockout is still active, if they have the magic touch.

### Back panel
+ and - buttons to change time.

5 pin DIN plug "dongle" with magic wiring to enable time change mode.

### Other
USB power supply for 5v.

Solid state relay to control the water valve. To keep the power supplies simple, the solenoid needs to be mains, and the SSR 5v so the Arduino can power it directly.

### Pin usage
Arduino Uno has:
* EEPROM (remember setting)
* 14 DIO
* 6 A/D in

Pin usage:
* 13 out
    * 8 for 7seg
    * 2 latch strobes
    * 1 output enable (PWM - dimmer)
    * 1 valve actuator
    * 1 PWM for buzzer

* 5 analogue in
    * 1 start button
    * 1 security
    * 2 change time (+, -)
    * 1 reset - reed relay

## “UI”

### Concepts
* Short beep to confirm every button press
* 3 quick beeps for negative
* Long beep to attract attention?
* Always have some LED on so we know the unit is functioning.
* If display is off, LED should blink.
* If display isn't changing rapidly, LED should blink

### Flow
Power on -> Idle
Idle:
* display off
* blinking LED
* Water valve closed
* short beep

Press button short -> Show time
* start show time timer
* show shower time
* display bright
* short beep

Show time timer expires -> Idle

Press button long -> Water On
* water valve open
* long beep
* show shower time
* display bright
* start shower timer
* blinking LED

Water On ->
* count down time in mins
* blinking LED
* short beep at 5 mins

Water On, button press -> 3 rapid beeps

Water On, long press -> long beep to confirm, carry on but no beeps

Water On, 1 minute to go ->
* display seconds
* blinking LED
* display alternating bright & dim
* long beep
* short beep every 5 seconds
* constant beeping last 10 seconds (rapid beeps every second)

Timer expires -> Lockout 

Lockout ->
* solid LED
* water valve closed
* count down lockout in mins
* display flashing
* any button press - 3 rapid beeps

Lockout complete -> Idle 

Dongle in -> Override

Override ->
* succession of short beeps so you know it has registered
* blinking LED
* show shower time
* display alternating bright & dim
* water valve open
* +/- buttons change time, short beep for each press

Reset-> Idle
* succession of short beeps so you know reset has occurred


### 7 Segments
* Show time in minutes.
* Count down when shower in progress.
* Show seconds in last minute
* Use PWM on latch output enable to control brightness.

## Links
Valve: http://www.solenoid-valve.world/by-industry/water-industry/115BC-RPE-solenoid-valve

Current sense: https://www.proto-pic.co.uk/acs712-breakout.html

## Ideas
Multiplex the 7-seg onto the same 7 DIO lines. Enable each in turn, 50% duty cycle. 
7 lines for data, 1 for select, 2 for other LEDs = 10 

or... 

2 for binary select, with 4 destinations for 7 data: 2 7seg, 2 LEDs, lots spare = 9 but more HW to demux. Probably unnecessary

“Front panel” behind smoked perspex, 15x15 so it can be tiled into the shower enclosure

Ribbon cable to front panel.

5-pin DIN “dongle” to enable override (always on, program times). This is just a fancy wiring jumble earthing an LED, and pulling an IO line low. Can also earth low side of +/- buttons so they just don’t work when dongle is out.

Glue buzzer to back of front panel - panel will be sound board. Use Arduino PWM channel, on 50% duty cycle, change frequency, to drive it?

Tune hall sensor so when shower is on but cold (i.e. heater isn’t drawing lots of amps), don’t trigger timer, so shower can be used for washing out etc.

Raise tone of beep as the last minute expires?!

Use PWM to generate tones, so we can free up DIO and/or CPU cycles? If not, tone library exists.


