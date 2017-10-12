#include "button.h"
#include "display.h"
#include "timer.h"
#include "beep.h"

#include <EEPROM.h>

#define SEG_A         (3)
#define SEG_B         (4)
#define SEG_C         (0)
#define SEG_D         (1)
#define SEG_E         (7)
#define SEG_F         (2)
#define SEG_G         (5)
#define SEG_DOT       (8)

#define LATCH0_ENABLE (9)
#define LATCH1_ENABLE (10)

#define LEDS_OE       (11)

#define BUZZER_PWM    (6)   // 980 Hz PWM output on this pin

#define EEPROM_ADDR   (0)

Button startButton(A0);
Button plusButton(A1);
Button minusButton(A2);

const int pins[8] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DOT };

Display display(pins, LATCH1_ENABLE, LATCH0_ENABLE, LEDS_OE);


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
Timer timer(getTimeNow);
void tenSecondCallback()
{
  static int n = 0;

  switch(n++)
  {
    case 0: beep.shortBeep(); break;
    case 1: beep.longBeep(); break;
    case 2: beep.rapidBeeps(); n = 0; break;
  }
}

void store(byte x)
{
  EEPROM.write(EEPROM_ADDR, x);
}

int n = 50;
const int max = 100;
  
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  display.setup();

  const byte x = EEPROM.read(EEPROM_ADDR);
  n = x;
  if (n > max)
  {
    n = max;
  }

  //timer.every(3000, tenSecondCallback);
  //timer.every(50, beepPoll);
}


void loop()
{
  const bool start = startButton.check();
  const bool plus = plusButton.check();
  const bool minus = minusButton.check();

  if (plus)
  {
    if (n < max)
    {
      n++;
      store(n);
      analogWrite(BUZZER_PWM, 50);
      delay(50);
      analogWrite(BUZZER_PWM, 0);
    }
  }
  else if (minus)
  {
    if (n > 0)
    {
      n--;
      store(n);
      analogWrite(BUZZER_PWM, 50);
      delay(50);
      analogWrite(BUZZER_PWM, 0);
    }
  }

  display.showNumber(n, start);

  // demonstrate PWM dimming
  if (n/10 % 2)
  {
    display.dim();
  }
  else
  {
    display.bright();
  }

  delay(10);

  timer.update();
}
