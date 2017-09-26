#pragma once

class Events
{
public:
    virtual ~Events() {}

    virtual void start() = 0;
    virtual void coldTimerExpired() = 0;
    virtual void showerHot() = 0;
};
