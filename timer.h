

class Timer
{
public:
    typedef unsigned int Time;
    typedef Time TimeFn(void);

    Timer(TimeFn& fn) : get(fn), startTime(0), duration(0) {}

    void start(Time t)
    {
        startTime = get();
        duration = t;
    }

    Time remaining() const { return duration; }

    void update()
    {
        const Time now = get();
        const Time delta = now - startTime;
        if (duration > delta)
        {
            duration -= delta;
        }
        else
        {
            duration = 0;
        }
    }

private:

    TimeFn& get;
    Time startTime;
    Time duration;
};
