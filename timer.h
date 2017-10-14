#pragma once

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
class Timer
{
public:
    typedef unsigned long Milliseconds;
    typedef Milliseconds TimeFn(void);
    typedef void EventCallbackFn();

    Timer(TimeFn& fn) : eventIndex(1), getTime(fn), lastUpdate(0) {}

    void every(Milliseconds t, EventCallbackFn & fn)
    {
        if (eventIndex < nEvents)
        {
            events[eventIndex++].init(t, fn);
        }
    }

    void start(Milliseconds t, EventCallbackFn & fn)
    {
        events[0].init(t, fn);
        lastUpdate = getTime();
    }

    Milliseconds remaining() const { return events[0].remaining; }

    void update()
    {
        const Milliseconds now = getTime();
        const Milliseconds delta = now - lastUpdate;
        lastUpdate = now;

        for (size_t i = 0; i < nEvents; ++i)
        {
            events[i].update(delta);
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
                    remaining += (delta > period) ? period * (delta / period) : period;
                    fn();
                }
            }
        }
    };

    static constexpr size_t nEvents = 10;
    Event events[nEvents];
    size_t eventIndex;

    TimeFn& getTime;
    Milliseconds lastUpdate;
};
