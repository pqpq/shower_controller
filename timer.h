
#include <iostream>
using namespace std;


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

    Timer(TimeFn& fn)
        : get(fn), startTime(0), lastUpdate(0), duration(0), eventIndex(1)
    {}

    void every(Milliseconds t, EventCallbackFn& fn)
    {
        if (eventIndex < nEvents)
        {
            events[eventIndex++].init(t, fn);
        }
    }

    void start(Milliseconds t, EventCallbackFn & fn)
    {
        startTime = get();
        lastUpdate = startTime;
        duration = t;

        events[0].init(t, fn);

        dump("start():");
    }

    Milliseconds remaining() const { return duration; }

    void update()
    {
        dump("update():");
        const Milliseconds now = get();
        const Milliseconds delta = now - lastUpdate;
        lastUpdate = now;

        for (size_t i = 0; i < nEvents; ++i)
        {
            events[i].update(delta);
        }

        cout << "delta = " << delta << endl;

        if (duration > delta)
        {
            duration -= delta;
        }
        else
        {
            duration = 0;
        }
        dump("update() done:");
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

    static constexpr size_t nEvents = 3;//10;

    TimeFn& get;
    Milliseconds startTime;
    Milliseconds lastUpdate;
    Milliseconds duration;
    Event events[nEvents];
    size_t eventIndex;

    void dump(const char* context) const
    {
        cout << context << endl;
        cout << "D=" << duration << " start=" << startTime << " lastUpdate=" << lastUpdate << endl;
        for (size_t i = 0; i < nEvents; ++i)
        {
            cout << '[' << i << "] p=" << events[i].period << " r=" << events[i].remaining << endl;
        }
        cout << endl;
    }

};
