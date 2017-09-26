#pragma once

class Actions
{
public:
    virtual ~Actions() {}
    virtual void greenLedOn() = 0;
    virtual void valveClosed() = 0;
    virtual void showShowerTime() = 0;
    virtual void displayDim() = 0;
    virtual void shortBeep() = 0;
};
