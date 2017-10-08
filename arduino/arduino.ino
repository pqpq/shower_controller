#include "Button.h"
#include "Display.h"

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

Button startButton(A0);
Button plusButton(A1);
Button minusButton(A2);

const int pins[8] = { SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DOT };

Display display(pins, LATCH1_ENABLE, LATCH0_ENABLE, LEDS_OE);

  
void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  display.setup();
}


int n = 50;

void loop()
{
  const bool start = startButton.check();
  const bool plus = plusButton.check();
  const bool minus = minusButton.check();

  if (plus) n++;
  else if (minus) n--;

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
}
