
#include <stddef.h>

class Timer
{
public:
    typedef unsigned int Milliseconds;
    typedef Milliseconds TimeFn(void);
    typedef void EventCallbackFn();

    Timer(TimeFn& fn) : get(fn), startTime(0), duration(0) {}

    void every(Milliseconds t, void(&fn)())
    {
        events[0].init(t, fn);
    }

    void start(Milliseconds t)
    {
        startTime = get();
        duration = t;
    }

    Milliseconds remaining() const { return duration; }

    void update()
    {
        const Milliseconds now = get();
        const Milliseconds delta = now - startTime;
        for (size_t i = 0; i < nEvents; ++i)
        {
            events[i].update(delta);
        }

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

    struct Event
    {
        EventCallbackFn* fn = nullptr;
        Milliseconds period = 0;
        Milliseconds remaining = 0;

        void init(Milliseconds t, EventCallbackFn& callback)
        {
            fn = &callback;
            period = t;
            remaining = t;
        }

        bool valid() const { return period && fn; }

        void update(Milliseconds delta)
        {
            if (valid())
            {
                const Milliseconds oldRemaining = remaining;
                remaining -= delta;
                if (oldRemaining <= delta)
                {
                    remaining += period;
                    fn();
                }
            }
        }
    };

    static constexpr size_t nEvents = 10;

    TimeFn& get;
    Milliseconds startTime;
    Milliseconds duration;
    Event events[nEvents];

};
