#pragma once

class Events
{
public:
    virtual ~Events() {}

    virtual void start() = 0;
    virtual void coldTimerExpired() = 0;
    virtual void showerHot() = 0;
    virtual void fiveMinutesToGo() = 0;
    virtual void oneMinuteToGo() = 0;
    virtual void fiveSecondsPassed() = 0;
    virtual void tenSecondsToGo() = 0;
    virtual void showerTimerExpired() = 0;
    virtual void dongleIn() = 0;
    virtual void dongleOut() = 0;
    virtual void blah() = 0;
};
