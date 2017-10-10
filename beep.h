#pragma once

#include <stddef.h>

#include <iostream>
using namespace std;

class Beep
{
public:
    typedef void Callback(bool);

    Beep(Callback & c, size_t size = queueSize)
        : callback(c), on(false), duration(0), r(0), w(0)
        , max(size > queueSize ? queueSize : size)
    {
        queue[0] = 0;
        callback(false);

        for (size_t i = 1; i < queueSize; ++i)
        {
            queue[i] = 99;
        }
    }

    void poll()
    {
        dump("poll");

        int & n = queue[r];
        if (n > 0)
        {
            n--;
        }
        else if (n < 0)
        {
            n++;
        }

        if (n == 0 && !isEmpty())
        {
            // advance
            r++;
            if (r >= max)
            {
                r = 0;
            }

            if (queue[r] > 0)
            {
                callback(true);
            }
            else //if (queue[w] < 0)
            {
                callback(false);
            }
        }
    }

    void shortBeep()
    {
        if (space() >= 2)
        {
            addOn(3);
            addOff(2);
        }
        dump("shortBeep");
    }

    void longBeep()
    {
        if (space() >= 2)
        {
            addOn(6);
            addOff(2);
        }
        dump("longBeep");
    }

    void rapidBeeps()
    {
        if (space() >= 6)
        {
            addOn(1);
            addOff(1);
            addOn(1);
            addOff(1);
            addOn(1);
            addOff(2);
        }
        dump("rapidBeeps");
    }

private:

    enum
    {
        queueSize = 20
    };

    Callback & callback;

    bool on;
    size_t duration;

    int queue[queueSize];
    size_t r;
    size_t w;
    const size_t max;

    size_t space() const
    {
        if (w > r)
        {
            return max - (w - r);
        }
        if (w < r)
        {
            return r - w;
        }

        if (queue[r] == 0)
        {
            return max;
        }

        return 0;
    }

    bool isEmpty() const { return space() == max; }

    void addOn(unsigned int n)
    {
        enqueue(n);
    }

    void addOff(unsigned int n)
    {
        enqueue(-n);
    }

    void enqueue(int n)
    {
        if (w == r) // empty
        {
            queue[w++] = 0;
        }

        queue[w++] = n;
        if (w >= max)
        {
            w = 0;
        }
    }

    void dump(const char* context) const
    {
        cout << context << " : [";
        for (size_t i = 0; i < queueSize-1; ++i)
        {
            cout << queue[i] << ", ";
        }
        cout << queue[queueSize-1] << ']' << endl;
        cout << "r=" << r << ", w=" << w << ", space=" << space() << '/' << max << endl;
    }
};
