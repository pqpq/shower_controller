void write7seg(int);


// the setup function runs once when you press reset or power the board
void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);

  // 7-seg data lines
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  // latch enables
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);

  write7seg(0);

  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
}

//      3
//    *****
//   *     *
// 2 *     * 4
//   *     *
//    *****
//   *  5  *
// 6 *     * 0
//   *     *
//    *****
//      1
const int digitMapping[10] =
{
  0b1011111,    // 0
  0b0010001,    // 1
  0b1111010,    // 2
  0b0111011,    // 3
  0b0110101,    // 4
  0b0101111,    // 5
  0b1101111,    // 6
  0b0011001,    // 7
  0b1111111,    // 8
  0b0111111     // 9
};

//     _  _     _  _     _  _     _  _     _  _     _  _ 
//    |   _|   |  | |   | |  |   |_   |   |    |   |    |
//    |_  _|   |_  _|   |_  _|   |_  _|   |_| _|   |_ |_|
const int spinnerMapping[]=
{
  
};

void write7seg(int bits)
{
  digitalWrite(0, bits & 0b0000001 ? LOW : HIGH);
  digitalWrite(1, bits & 0b0000010 ? LOW : HIGH);
  digitalWrite(2, bits & 0b0000100 ? LOW : HIGH);
  digitalWrite(3, bits & 0b0001000 ? LOW : HIGH);
  digitalWrite(4, bits & 0b0010000 ? LOW : HIGH);
  digitalWrite(5, bits & 0b0100000 ? LOW : HIGH);
  digitalWrite(6, bits & 0b1000000 ? LOW : HIGH);
}

void writeDigit(int digit, int value)
{
  if (digit == 0)
  {
    digitalWrite(7, HIGH);
  }
  else
  {
    digitalWrite(8, HIGH);
  }

  if (digit == 1 && value == 0)
  {
    write7seg(0);
  }
  else
  {
    write7seg(digitMapping[value]);
  }
  
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  write7seg(0b0100000);
}

  int n = 0;

// the loop function runs over and over again forever
void loop()
{
  writeDigit(0, n % 10);
  writeDigit(1, n / 10);
  ++n;
  if (n > 99)
  {
    n = 0;
  }

  digitalWrite(LED_BUILTIN, n % 2 ? HIGH : LOW);

  delay(100);
}
