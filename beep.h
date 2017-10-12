#pragma once

#include <stddef.h>


/// Manage a queue of beep requests.
///
/// poll() should be called at regular intervals to bump the queue along.
/// 20Hz is about right, because this makes the rapid beeps sound right.
/// Beep durations will have to be tweaked if the poll frequency is changed.
/// The callback function should turn a beeper on and off.
///
/// This code is NOT thread safe: it is assumed that beeps are added between
/// calls to poll().
///
/// The class manages a simple queue of times. Positive times are for beep on,
/// and negative times for beep off. Complex beep patterns can be built up from
/// sequences of ons and offs.
/// When the queue is empty, it is primed with a zero to cause poll() to step to
/// the next slot. This is simpler than some special code to handle the 'start'
/// condition.
///
class Beep
{
public:
    typedef void Callback(bool);

    Beep(Callback & c, size_t size = maxQueueSize)
        : callback(c)
        , write(0)
        , size(size > maxQueueSize ? maxQueueSize : size)
    {
        queue[0] = 0;
        callback(false);
    }

    void poll()
    {
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
            addOn(5);
            addOff(5);
        }
    }

    void longBeep()
    {
        if (spaceFor(2))
        {
            addOn(20);
            addOff(5);
        }
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
            addOff(5);
        }
    }

private:

    enum
    {
        maxQueueSize = 20
    };

    Callback & callback;

    int queue[maxQueueSize];
    size_t write;
    const size_t size;

    void advance()
    {
        for (size_t i = 1; i < size; ++i)
        {
            queue[i-1] = queue[i];
        }
        queue[size-1] = 0;
        write--;
    }

    size_t space()            const { return size - write; }
    bool   spaceFor(size_t n) const { return space() >= n; }
    bool   isEmpty()          const { return write == 0; }

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
};
