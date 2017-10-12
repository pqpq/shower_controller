// Analogue input button normally pulled low, with the button shorting
// the input to Vcc
// We convert it into a momentary action so each distinct press can
// be detected.
class Button
{
  // 0-5v reads 0 - 1024, so just need something reasonable to
  // tell when the press has pulled the input up
  static const int senseThreshold = 800;

  // Number of consecutive reads before declaring a press
  static const int debounceCount = 5;

  const int pin;
  int debounce;
  bool pressed;

public:

  Button(int pin) : pin(pin), debounce(0), pressed(false) {}

  bool check()
  {
    const bool wasPressed = pressed;
    pressed = debounce > debounceCount;
    const bool maybePressed = analogRead(pin) > senseThreshold;
    if (maybePressed)
    {
      if (!pressed)
      {
        debounce++;
      }
    }
    else
    {
      debounce = 0;
    }

    return pressed && !wasPressed;
  }
};

