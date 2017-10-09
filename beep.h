#pragma once

#include <stddef.h>

class Beep
{
public:
    typedef void Callback(bool);

    Beep(Callback & c)
        : callback(c), on(false), duration(0), q(0)
    {
        callback(false);
    }

    void poll()
    {
        if (duration == 0 && on)
        {
            callback(false);
            on = false;
            if (q)
            {
                for (size_t i = 0; i < queueSize-1; ++i)
                {
                    queue[i] = queue[i+1];
                }
                q--;

                if (q)
                {
                    duration = queue[0];
                }
            }
        }

        if (duration > 0)
        {
            if (!on)
            {
                callback(true);
                on = true;
            }
            duration--;
        }
    }

    void shortBeep()
    {
        addOn(3);
        addOff(1);
    }

    void longBeep()
    {
        addOn(6);
        addOff(1);
    }

    void rapidBeeps()
    {
        addOn(1);
        addOff(1);
        addOn(1);
        addOff(1);
        addOn(1);
        addOff(1);
    }

private:

    enum
    {
        queueSize = 20,
        onBit    = 0x80,
        timeMask = 0x7f
    };

    Callback & callback;

    bool on;
    size_t duration;

    unsigned char queue[queueSize];
    size_t q;

    void addOn(size_t n)
    {
        enqueue(n, true);
    }

    void addOff(size_t n)
    {
        enqueue(n, false);
    }

    void enqueue(size_t n, bool on)
    {
        if (q < queueSize)
        {
            unsigned char x = n & timeMask;
            if (on)
            {
                x |= onBit;
            }
            queue[q] = x;
            q++;
        }

    }
};
