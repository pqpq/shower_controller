#pragma once

#include <stddef.h>

//#include <iostream>
//using namespace std;

class Beep
{
public:
    typedef void Callback(bool);

    Beep(Callback & c)
        : callback(c), on(false), duration(0), r(0), w(0)
    {
        //queue[0] = 0;
        callback(false);
    }

    void poll()
    {
//        cout << '[';
//        for (size_t i = 0; i < queueSize-1; ++i)
//        {
//            cout << queue[i] << ", ";
//        }
//        cout << queue[queueSize-1] << ']' << endl;
//        cout << "r=" << r << ", w=" << w << endl;

        int & n = queue[r];
        if (n > 0)
        {
            n--;
        }
        else if (n < 0)
        {
            n++;
        }

        if (n == 0 && r != w)
        {
            // advance
            r++;
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
        addOn(3);
        addOff(2);
    }

    void longBeep()
    {
        addOn(6);
        addOff(2);
    }

    void rapidBeeps()
    {
        addOn(1);
        addOff(1);
        addOn(1);
        addOff(1);
        addOn(1);
        addOff(2);
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
        if (w == 0) // empty
        {
            queue[w++] = 0;
        }
        if (w < queueSize)
        {
            queue[w++] = n;
        }
    }
};
