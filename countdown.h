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

    Countdown(EventCallback & fiveMinutesCallback,
            EventCallback & lastMinuteCallback,
            EventCallback & fiveSecondCallback,
            EventCallback & oneSecondCallback)
    : fiveMinutesCallback(fiveMinutesCallback)
    , lastMinuteCallback(lastMinuteCallback)
    , fiveSecondsCallback(fiveSecondCallback)
    , oneSecondCallback(oneSecondCallback)
    , previous(0)
    {}

    void secondsToGo(Seconds s)
    {
        if (previous > 0)
        {
            if ((previous-1) / 300 != (s-1) / 300)
            {
                fiveMinutesCallback();
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

    EventCallback & fiveMinutesCallback;
    EventCallback & lastMinuteCallback;
    EventCallback & fiveSecondsCallback;
    EventCallback & oneSecondCallback;

    Seconds previous;
};
