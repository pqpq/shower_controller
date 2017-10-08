#pragma once

/// The actions that come from our state machine.
///
/// The implementation details of each action are outside the scope of the state
/// machine, but are obviously carried out by the underlying hardware.
class Actions
{
public:
    virtual ~Actions() {}

    virtual void valveClosed() = 0;
    virtual void valveOpen() = 0;

    virtual void ledOn() = 0;
    virtual void ledFlashing() = 0;

    virtual void showShowerTime() = 0;
    virtual void showFinalCountdown() = 0;
    virtual void showLockoutTime() = 0;
    virtual void displayOff() = 0;
    virtual void displayOn() = 0;
    virtual void displayFlash() = 0;
    virtual void displayPulse() = 0;

    virtual void shortBeep() = 0;
    virtual void longBeep() = 0;
    virtual void rapidBeeps() = 0;

    virtual void showTimerStart() = 0;
    virtual void showerTimerStart() = 0;
    virtual void lockoutTimerStart() = 0;

    virtual void timeAdd() = 0;
    virtual void timeRemove() = 0;
    virtual void timeSave() = 0;

};
