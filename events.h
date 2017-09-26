#pragma once

class Events
{
public:
    virtual ~Events() {}
    virtual void start() = 0;
};
