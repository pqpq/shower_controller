#pragma once

#include <stddef.h>

#include <iostream>
using namespace std;

class Beep
{
public:
    typedef void Callback(bool);

    Beep(Callback & c, size_t size = queueSize)
        : callback(c)
        , write(0)
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

        int & head = queue[0];
        if (head > 0)
        {
            head--;
        }
        else if (head < 0)
        {
            head++;
        }

        if (head == 0 && !isEmpty())
        {
            advance();

            if (head > 0)
            {
                callback(true);
            }
            else if (head < 0)
            {
                callback(false);
            }
        }
    }

    void shortBeep()
    {
        if (spaceFor(2))
        {
            addOn(3);
            addOff(2);
        }
        dump("shortBeep");
    }

    void longBeep()
    {
        if (spaceFor(2))
        {
            addOn(6);
            addOff(2);
        }
        dump("longBeep");
    }

    void rapidBeeps()
    {
        if (spaceFor(6))
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

    int queue[queueSize];
    size_t write;
    const size_t max;

    void advance()
    {
        for (size_t i = 1; i < max; ++i)
        {
            queue[i-1] = queue[i];
        }
        queue[max-1] = 0;
        write--;
    }

    size_t space() const { return max - write; }
    bool spaceFor(size_t n) const { return space() >= n; }
    bool isEmpty() const { return write == 0; }

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
        if (isEmpty())
        {
            queue[write++] = 0;
        }

        queue[write++] = n;
    }

    void dump(const char* context) const
    {
        cout << context << " : [";
        for (size_t i = 0; i < queueSize-1; ++i)
        {
            cout << queue[i] << ", ";
        }
        cout << queue[queueSize-1] << ']' << endl;
        cout << "write=" << write << ", space=" << space() << '/' << max << endl;
    }
};
