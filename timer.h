
#include <stddef.h>

/// A simple Timer that calls back when time events have passed.
///
/// We track passing milliseconds by reading the HW time in update(), rather
/// than calling delay() from the main loop and suffering potential drift as a
/// result.
///
/// Blatantly ripped off the "every" concept from an Arduino timer library I
/// found on't web and now I've lost it so I can't include a reference.
///
/// @todo "finished" callback? Make this event 0?
///
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
