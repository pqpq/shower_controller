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

#define LATCH0_ENABLE (9)
#define LATCH1_ENABLE (10)

#define LEDS_OE       (11)

#define BUZZER_PWM    (6)   // 980 Hz PWM output on this pin

#define EEPROM_ADDR   (0)

Button startButton(A0);
Button plusButton(A1);
Button minusButton(A2);
Button dongle(A3);

constexpr unsigned int maxShowerTime_mins = 30;
constexpr unsigned int minShowerTime_mins = 2;//5;
constexpr unsigned int showTime_sec = 5;
constexpr unsigned int lockoutTime_mins = 3;//30;
unsigned int showerTime_mins = 15;

Display display(sevenSegmentPins, LATCH1_ENABLE, LATCH0_ENABLE, LEDS_OE);

void displayFlashPoll()
{
  display.tick();
}

void beepCallback(bool on)
{
  analogWrite(BUZZER_PWM, on ? 50 : 0);
}

Beep beep(beepCallback);

void beepPoll()
{
  beep.poll();
}

Timer::Milliseconds getTimeNow() { return millis(); }

Timer showTimer(getTimeNow);
Timer showerTimer(getTimeNow);
Timer lockoutTimer(getTimeNow);
Timer systemTimer(getTimeNow);

void store(byte x)
{
  EEPROM.write(EEPROM_ADDR, x);
}

enum ShowOnDisplay
{
  StoredShowerTimeMins,
  ShowerTimeMins,
  ShowerTimeSecs,
  LockoutTimeMins
};

ShowOnDisplay showOnDisplay = StoredShowerTimeMins;

Countdown::Seconds timeToGo()
{
  const Timer::Milliseconds ms = showerTimer.remaining();
  return ms / 1000UL;
}

Countdown countdown(timeToGo, fiveMinutesToGo, oneMinuteToGo, fiveSecondsPassed, oneSecondPassed);

class RealActions : public Actions
{
    void valveClosed() override {}
    void valveOpen() override {}

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
    void showerTimerStart() override
    {
      showerTimer.start(showerTime_mins * 60 * 1000UL, showerTimerExpired);
      countdown.start();
    }
    void lockoutTimerStart() override { lockoutTimer.start(lockoutTime_mins * 60 * 1000UL, lockoutTimerExpired); }

    void timeAdd() override
    {
      if (showerTime_mins < maxShowerTime_mins)
      {
        showerTime_mins++;
      }
    }
    void timeRemove() override
    {
      if (showerTime_mins > minShowerTime_mins)
      {
        showerTime_mins--;
      }
    }
    void timeSave() override { store(showerTime_mins); }
};

RealActions actions;

Controller controller(actions);

// shower timer callbacks -> countdown
void showerFiveMinutesPassed()  { countdown.fiveMinutes(); }
void showerOneMinutePassed()    { countdown.oneMinute(); }
void showerFiveSecondsPassed()  { countdown.fiveSeconds(); }
void showerOneSecondPassed()    { countdown.oneSecond(); }

// timer expiry callbacks -> controller
void showTimerExpired()   { controller.showTimerExpired(); }
void showerTimerExpired() { controller.showerTimerExpired(); }
void lockoutTimerExpired(){ controller.lockoutTimerExpired(); }

// countdown callbacks -> controller
void fiveMinutesToGo()    { controller.fiveMinutesToGo(); }
void oneMinuteToGo()      { controller.oneMinuteToGo(); }
void fiveSecondsPassed()  { controller.fiveSecondsPassed(); }
void oneSecondPassed()    { controller.oneSecondPassed(); }
    
  
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  display.setup();

  const byte x = EEPROM.read(EEPROM_ADDR);
  showerTime_mins = x;
  if (showerTime_mins > maxShowerTime_mins)
  {
    showerTime_mins = maxShowerTime_mins;
  }
  if (showerTime_mins < minShowerTime_mins)
  {
    showerTime_mins = minShowerTime_mins;
  }

  showerTimer.every(5 * 60 * 1000UL, showerFiveMinutesPassed);
  showerTimer.every(1 * 60 * 1000UL, showerOneMinutePassed);
  showerTimer.every(     5 * 1000UL, showerFiveSecondsPassed);
  showerTimer.every(     1 * 1000UL, showerOneSecondPassed);

  systemTimer.every(50, beepPoll);
  systemTimer.every(500, displayFlashPoll);
}


void loop()
{
  const bool start = startButton.check();
  const bool plus = plusButton.check();
  const bool minus = minusButton.check();
  const bool dongleIn = dongle.check();
  const bool dongleOut = dongle.released();

  if (start)
  {
    controller.startButton();
  }
  if (plus)
  {
    controller.plusButton();
  }
  if (minus)
  {
    controller.minusButton();
  }
  if (dongleIn)
  {
    controller.dongleIn();
  }
  if (dongleOut)
  {
    controller.dongleOut();
  }

  /// @todo reset

  switch (showOnDisplay)
  {
  default:
  case StoredShowerTimeMins:
    display.showNumber(showerTime_mins);
    break;
  case ShowerTimeMins:
    display.showNumber(showerTimer.remaining() / (60 * 1000));
    break;
  case ShowerTimeSecs:
    display.showNumber(showerTimer.remaining() / 1000);
    break;
  case LockoutTimeMins:
    display.showNumber(lockoutTimer.remaining() / (60 * 1000));
    break;
  }

  showTimer.update();
  showerTimer.update();
  lockoutTimer.update();
  systemTimer.update();

  delay(10);
}
