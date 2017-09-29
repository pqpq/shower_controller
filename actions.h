#pragma once

class Actions
{
public:
    virtual ~Actions() {}

    virtual void valveClosed() = 0;
    virtual void valveOpen() = 0;

    virtual void greenLedOn() = 0;
    virtual void greenLedOff() = 0;
    virtual void greenLedFlashing() = 0;
    virtual void redLedFlashing() = 0;

    virtual void showShowerTime() = 0;
    virtual void countDownTime() = 0;
    virtual void countDownLockout() = 0;
    virtual void displayDim() = 0;
    virtual void displayBright() = 0;

    virtual void shortBeep() = 0;
    virtual void longBeep() = 0;
    virtual void rapidBeep() = 0;

    virtual void coldTimerStart() = 0;
    virtual void coldTimerStop() = 0;
    virtual void showerTimerStart() = 0;
};
