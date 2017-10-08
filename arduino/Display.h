// The 7 segments are
//    a
//  f   b
//    g
//  e   c
//    d
static const int digitMapping[10] =
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


class Display
{
public:

  typedef int Pins [8];

private:

  const Pins & pins;
  int selectDigit0;
  int selectDigit1;
  int pwm;

public:

  Display(const Pins & pins, int selectDigit1, int selectDigit0, int pwm)
  : pins(pins)
  , selectDigit0(selectDigit0)
  , selectDigit1(selectDigit1)
  , pwm(pwm)
  {}

  void setup()
  {
    // 7-seg data lines
    pinMode(pins[0], OUTPUT);
    pinMode(pins[1], OUTPUT);
    pinMode(pins[2], OUTPUT);
    pinMode(pins[3], OUTPUT);
    pinMode(pins[4], OUTPUT);
    pinMode(pins[5], OUTPUT);
    pinMode(pins[6], OUTPUT);
    pinMode(pins[7], OUTPUT);

    // latch enables
    pinMode(selectDigit0, OUTPUT);
    pinMode(selectDigit1, OUTPUT);

    // output enable (LED dimmer)
    pinMode(pwm, OUTPUT);

    writeBits(0, false);

    digitalWrite(selectDigit0, LOW);
    digitalWrite(selectDigit1, LOW);
    digitalWrite(pwm, LOW);
  }

  void showNumber(int n, bool showDot)
  {
    const int digitOff = 99;

    const int tens = n < 100 ? n / 10 : digitOff;
    const int units = n < 100 ? n % 10 : digitOff;
    writeDigit(1, tens, showDot);
    writeDigit(0, units, showDot);
  }

  void dim()
  {
    // ~OE so high PWM duty cycle = low duty cycle for LEDs
    analogWrite(pwm, 220);
  }

  void bright()
  {
    digitalWrite(pwm, LOW);
  }

private:

  void writeBits(int bits, bool showDot)
  {
    digitalWrite(pins[0], bits & 0b0000001 ? LOW : HIGH);
    digitalWrite(pins[1], bits & 0b0000010 ? LOW : HIGH);
    digitalWrite(pins[2], bits & 0b0000100 ? LOW : HIGH);
    digitalWrite(pins[3], bits & 0b0001000 ? LOW : HIGH);
    digitalWrite(pins[4], bits & 0b0010000 ? LOW : HIGH);
    digitalWrite(pins[5], bits & 0b0100000 ? LOW : HIGH);
    digitalWrite(pins[6], bits & 0b1000000 ? LOW : HIGH);
    digitalWrite(pins[7], showDot ? LOW : HIGH);
  }

  void writeDigit(int digit, int value, bool showDot)
  {
    if (digit == 1 && (value == 0 || value > 9))
    {
      writeBits(0, false);
    }
    else
    {
      const int bits = (value < 10) ? digitMapping[value] : 0;
      writeBits(bits, showDot);
    }

    if (digit == 0)
    {
      digitalWrite(selectDigit0, HIGH);
    }
    else
    {
      digitalWrite(selectDigit1, HIGH);
    }

    digitalWrite(selectDigit0, LOW);
    digitalWrite(selectDigit1, LOW);
  }
};
