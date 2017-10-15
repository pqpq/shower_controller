#include "button.h"
#include "display.h"
#include "timer.h"
#include "beep.h"
#include "countdown.h"
#include "controller.h"

#include <EEPROM.h>

#define SEG_A         (3)
#define SEG_B         (4)
#define SEG_C         (0)
#define SEG_D         (1)
#define SEG_E         (7)
#define SEG_F         (2)
#define SEG_G         (5)
#define SEG_DOT       (8)

const int sevenSegmentPins[8] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DOT };

#define LATCH0_ENABLE (9)   // Latch for units 7-seg
#define LATCH1_ENABLE (10)  // Latch for tens 7-seg

#define LEDS_OE       (11)  // Latch OE -> LED brightness if PWMed

#define RELAY         (12)

#define BUZZER_PWM    (6)   // 980 Hz PWM output on this pin

#define EEPROM_ADDR   (0)


constexpr unsigned int maxShowerTime_mins = 30;
constexpr unsigned int minShowerTime_mins = 2;//5;
unsigned int showerTime_mins = 15;

void setShowerTime(int mins)
{
  if (mins > maxShowerTime_mins)
  {
    mins = maxShowerTime_mins;
  }
  if (mins < minShowerTime_mins)
  {
    mins = minShowerTime_mins;
  }
  showerTime_mins = mins;
}

// How long to show the shower time on the first press of the start button
constexpr unsigned int showTime_sec = 5;

// How long to lock out the shower after it has finished
constexpr unsigned int lockoutTime_mins = 3;//30;



void beepCallback(bool on)
{
  analogWrite(BUZZER_PWM, on ? 50 : 0);
}

Beep beep(beepCallback);

void beepPoll()
{
  beep.poll();
}

Timer::Milliseconds getTimeNow() 
{
  return millis(); 
}

Timer showTimer(getTimeNow);
Timer showerTimer(getTimeNow);
Timer lockoutTimer(getTimeNow);
Timer systemTimer(getTimeNow);


Display display(sevenSegmentPins, LATCH1_ENABLE, LATCH0_ENABLE, LEDS_OE);

void displayFlashPoll()
{
  display.tick();
}

enum ShowOnDisplay
{
  StoredShowerTimeMins,
  ShowerTimeMins,
  ShowerTimeSecs,
  LockoutTimeMins
};

ShowOnDisplay showOnDisplay = StoredShowerTimeMins;

void updateDisplay()
{
  switch (showOnDisplay)
  {
  default:
  case StoredShowerTimeMins:
    display.showNumber(showerTime_mins);
    break;

  case ShowerTimeMins:
    display.showNumber(1 + showerTimer.remaining() / (60 * 1000UL));
    break;

  case ShowerTimeSecs:
    display.showNumber(showerTimer.remaining() / 1000UL);
    break;

  case LockoutTimeMins:
    display.showNumber(1 + lockoutTimer.remaining() / (60 * 1000UL));
    break;
  }
}


Countdown countdown(fiveMinutesToGo, oneMinuteToGo, fiveSecondsPassed, oneSecondPassed);

void updateCountdown()
{
  const Timer::Milliseconds ms = showerTimer.remaining();
  const unsigned long secondsToGo = ms / 1000UL;
  countdown.secondsToGo(secondsToGo);
}


class RealActions : public Actions
{
    void valveClosed() override { digitalWrite(RELAY, LOW);  digitalWrite(LED_BUILTIN, LOW);  }
    void valveOpen() override   { digitalWrite(RELAY, HIGH); digitalWrite(LED_BUILTIN, HIGH); }

    void ledOn() override       { display.setDot(Display::DotOn); }
    void ledFlashing() override { display.setDot(Display::DotFlash); }

    void showShowerTotalTime() override { showOnDisplay = StoredShowerTimeMins; }
    void showShowerTime() override      { showOnDisplay = ShowerTimeMins; }
    void showFinalCountdown() override  { showOnDisplay = ShowerTimeSecs; }
    void showLockoutTime() override     { showOnDisplay = LockoutTimeMins; }

    void displayOff() override   { display.setMode(Display::Off); }
    void displayOn() override    { display.setMode(Display::Bright); }
    void displayFlash() override { display.setMode(Display::Flash); }
    void displayPulse() override { display.setMode(Display::Pulse); }

    void shortBeep() override  { beep.shortBeep(); }
    void longBeep() override   { beep.longBeep(); }
    void rapidBeeps() override { beep.rapidBeeps(); }

    void showTimerStart() override    { showTimer.start(showTime_sec * 1000UL, showTimerExpired); }
    void lockoutTimerStart() override { lockoutTimer.start(lockoutTime_mins * 60 * 1000UL, lockoutTimerExpired); }
    void showerTimerStart() override
    {
      showerTimer.start(showerTime_mins * 60 * 1000UL, showerTimerExpired);
      systemTimer.synch();
    }

    void timeAdd() override    { setShowerTime(showerTime_mins + 1); }
    void timeRemove() override { setShowerTime(showerTime_mins - 1); }
    void timeSave() override   { EEPROM.write(EEPROM_ADDR, showerTime_mins); }
};

RealActions actions;

Controller controller(actions);

// timer expiry callbacks -> controller
void showTimerExpired()   { controller.showTimerExpired(); }
void showerTimerExpired() { controller.showerTimerExpired(); }
void lockoutTimerExpired(){ controller.lockoutTimerExpired(); }

// countdown callbacks -> controller
void fiveMinutesToGo()    { controller.fiveMinutesToGo(); }
void oneMinuteToGo()      { controller.oneMinuteToGo(); }
void fiveSecondsPassed()  { controller.fiveSecondsPassed(); }
void oneSecondPassed()    { controller.oneSecondPassed(); }


Button startButton(A0);
Button plusButton(A1);
Button minusButton(A2);
Button dongle(A3);
Button reset(A4);

void checkButtons()
{
  if (startButton.check())
  {
    controller.startButton();
  }

  if (plusButton.check())
  {
    controller.plusButton();
  }

  if (minusButton.check())
  {
    controller.minusButton();
  }

  if (dongle.check())
  {
    controller.dongleIn();
  }

  if (dongle.released())
  {
    controller.dongleOut();
  }

  if (reset.check())
  {
    controller.reset();
  }
}


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  display.setup();

  setShowerTime(EEPROM.read(EEPROM_ADDR));

  systemTimer.every(50, beepPoll);
  systemTimer.every(500, displayFlashPoll);
  systemTimer.every(1000, updateCountdown);
}


void loop()
{
  checkButtons();

  showTimer.update();
  showerTimer.update();
  lockoutTimer.update();
  systemTimer.update();

  updateDisplay();

  delay(10);
}
