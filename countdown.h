
/// Deals with the logic of time passing, as we count down the minutes and seconds.
/// The various "time has passed" methods should be called at regular intervals,
/// and this class decides whether they trigger callbacks.
///
class Countdown
{
public:
    typedef unsigned int Seconds;
    typedef Seconds TimeToGoFn();
    typedef void EventCallback();

    Countdown(TimeToGoFn & timeToGo,
            EventCallback & fiveMinutesCallback,
            EventCallback & oneMinuteCallback,
            EventCallback & fiveSecondCallback,
            EventCallback & oneSecondCallback)
    : timeToGo(timeToGo)
    , fiveMinutesCallback(fiveMinutesCallback)
    , lastTime5Mins(0)
    , oneMinuteCallback(oneMinuteCallback)
    , lastTime1Min(0)
    , fiveSecondsCallback(fiveSecondCallback)
    , oneSecondCallback(oneSecondCallback)
    {}

    void start()
    {
        const Seconds timeNow = timeToGo();
        lastTime5Mins = timeNow;
        lastTime1Min = timeNow;
    }

    void fiveMinutes()
    {
        const Seconds timeNow = timeToGo();
        if (timeNow < 300 && lastTime5Mins >= 300)
        {
            fiveMinutesCallback();
        }

        lastTime5Mins = timeNow;
    }

    void oneMinute()
    {
        const Seconds timeNow = timeToGo();
        if (timeNow < 60 && lastTime1Min >= 60)
        {
            oneMinuteCallback();
        }

        lastTime1Min = timeNow;
    }

    void fiveSeconds()
    {
        if (timeToGo() < 60)
        {
            fiveSecondsCallback();
        }
    }

    void oneSecond()
    {
        if (timeToGo() < 10)
        {
            oneSecondCallback();
        }
    }

private:

    TimeToGoFn & timeToGo;
    EventCallback & fiveMinutesCallback;
    Seconds lastTime5Mins;
    EventCallback & oneMinuteCallback;
    Seconds lastTime1Min;
    EventCallback & fiveSecondsCallback;
    EventCallback & oneSecondCallback;
};
