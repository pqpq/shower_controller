
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

//       a/3
//      *****
//     *     *
// f/2 *     * b/4
//     *     *
//      *****
//     * g/5 *
// e/6 *     * c/0
//     *     *
//      *****
//       d/1
const int digitMapping[10] =
{
  //gfedcba
  0b0111111,    // 0
  0b0000110,    // 1
  0b1011011,    // 2
  0b1001111,    // 3
  0b1100110,    // 4
  0b1101101,    // 5
  0b1111101,    // 6
  0b0000111,    // 7
  0b1111111,    // 8
  0b1101111     // 9
};

void write7seg(int bits, bool showDot)
{
  digitalWrite(SEG_A, bits & 0b0000001 ? LOW : HIGH);
  digitalWrite(SEG_B, bits & 0b0000010 ? LOW : HIGH);
  digitalWrite(SEG_C, bits & 0b0000100 ? LOW : HIGH);
  digitalWrite(SEG_D, bits & 0b0001000 ? LOW : HIGH);
  digitalWrite(SEG_E, bits & 0b0010000 ? LOW : HIGH);
  digitalWrite(SEG_F, bits & 0b0100000 ? LOW : HIGH);
  digitalWrite(SEG_G, bits & 0b1000000 ? LOW : HIGH);
  digitalWrite(SEG_DOT, showDot ? LOW : HIGH);
}

void writeDigit(int digit, int value, bool showDot)
{
  if (digit == 0)
  {
    digitalWrite(LATCH0_ENABLE, HIGH);
  }
  else
  {
    digitalWrite(LATCH1_ENABLE, HIGH);
  }

  if (digit == 1 && (value == 0 || value > 9))
  {
    write7seg(0, false);
  }
  else
  {
    const int bits = (value < 10) ? digitMapping[value] : 0;
    write7seg(bits, showDot);
  }
  
  digitalWrite(LATCH0_ENABLE, LOW);
  digitalWrite(LATCH1_ENABLE, LOW);
}

void displayNumber(int n, bool showDot)
{
  const int digitOff = 99;
  
  const int tens = n < 100 ? n / 10 : digitOff;
  const int units = n < 100 ? n % 10 : digitOff;
  writeDigit(1, tens, showDot);
  writeDigit(0, units, showDot);
}

void displayDim()
{
  // ~OE so high PWM duty cycle = low duty cycle for LEDs
  analogWrite(LEDS_OE, 220);
}

void displayBright()
{
  digitalWrite(LEDS_OE, LOW);
}


void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  // 7-seg data lines
  pinMode(SEG_A, OUTPUT);
  pinMode(SEG_B, OUTPUT);
  pinMode(SEG_C, OUTPUT);
  pinMode(SEG_D, OUTPUT);
  pinMode(SEG_E, OUTPUT);
  pinMode(SEG_F, OUTPUT);
  pinMode(SEG_G, OUTPUT);
  pinMode(SEG_DOT, OUTPUT);

  // latch enables
  pinMode(LATCH0_ENABLE, OUTPUT);
  pinMode(LATCH1_ENABLE, OUTPUT);

  // output enable (LED dimmer)
  pinMode(LEDS_OE, OUTPUT);

  write7seg(0, false);

  digitalWrite(LATCH0_ENABLE, LOW);
  digitalWrite(LATCH1_ENABLE, LOW);
  digitalWrite(LEDS_OE, LOW);
}


int n = 0;
int d = 0;
bool dot = true;

void loop()
{
  displayNumber(n, dot);
  ++n;
  if (n > 110)
  {
    n = 0;
  }

  // demonstrate separate control of the dot
  if (d++ > 10)
  {
    d = 0;
    dot = !dot;
    digitalWrite(LED_BUILTIN, dot ? HIGH : LOW);
  }

  // demonstrate PWM dimming
  if (n/10 % 2)
  {
    displayDim();
  }
  else
  {
    displayBright();
  }

  delay(300);
}
