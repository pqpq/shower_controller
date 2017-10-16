#pragma once

/// Issue callbacks at critical times, as we count down passing time.
///
/// secondsToGo() should be called regularly, and this class decides whether the
/// current time to go should trigger any callbacks.
///
class Countdown
{
public:
    typedef unsigned int Seconds;
    typedef void EventCallback();

    Countdown(EventCallback & oneMinuteCallback,
            EventCallback & lastMinuteCallback,
            EventCallback & fiveSecondCallback,
            EventCallback & oneSecondCallback)
    : oneMinuteCallback(oneMinuteCallback)
    , lastMinuteCallback(lastMinuteCallback)
    , fiveSecondsCallback(fiveSecondCallback)
    , oneSecondCallback(oneSecondCallback)
    , previous(0)
    {}

    void secondsToGo(Seconds s)
    {
        if (previous > 0)
        {
            if (60 < s && s <= 300 && (previous-1) / 60 != (s-1) / 60)
            {
                oneMinuteCallback();
            }

            if (previous > 60 && s <= 60)
            {
                lastMinuteCallback();
            }

            if (10 < s && s <= 55 && (previous-1) / 5 != (s-1) / 5)
            {
                fiveSecondsCallback();
            }

            if (s <= 10 && previous != s)
            {
                oneSecondCallback();
            }
        }

        previous = s;
    }

private:

    EventCallback & oneMinuteCallback;
    EventCallback & lastMinuteCallback;
    EventCallback & fiveSecondsCallback;
    EventCallback & oneSecondCallback;

    Seconds previous;
};
