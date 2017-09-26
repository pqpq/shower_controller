#pragma once

class Actions
{
public:
    virtual ~Actions() {}

    virtual void valveClosed() = 0;
    virtual void valveOpen() = 0;

    virtual void greenLedOn() = 0;

    virtual void showShowerTime() = 0;
    virtual void displayDim() = 0;
    virtual void displayBright() = 0;

    virtual void shortBeep() = 0;
    virtual void longBeep() = 0;

    virtual void startColdTimer() = 0;
};
