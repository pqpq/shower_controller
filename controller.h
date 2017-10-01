#pragma once

#include "actions.h"
#include "events.h"

class Controller : public Events
{
public:
    Controller(Actions& a) : actions(a) { goToIdleNormal(); }

    // Events
    void start() override final
    {
        if (state == state_idle)
        {
            waterOn();
        }
    }

    void coldTimerExpired() override final
    {
        if (state != state_idle)
        {
            goToIdleNormal();
        }
    }

    void showerHot() override final
    {
        if (state == state_waterOn)
        {
            showerRunning();
        }
    }

    virtual void fiveMinutesToGo() {}
    virtual void oneMinuteToGo() {}
    virtual void fiveSecondsPassed() {}
    virtual void tenSecondsToGo() {}
    virtual void showerTimerExpired() {}
    virtual void dongleIn() { override(); }
    virtual void dongleOut() { goToIdleSpecial(); }
    virtual void reset() { goToIdleSpecial(); }
    virtual void plusButton() {}
    virtual void minusButton() {}

private:

    enum State
    {
        state_idle,
        state_waterOn,
        state_showerRunning,
        state_override
    };

    Actions& actions;
    State state;

    void idleState()
    {
        state = state_idle;
        actions.greenLedOn();
        actions.valveClosed();
        actions.showShowerTime();
        actions.displayDim();
    }

    void goToIdleNormal()
    {
        actions.shortBeep();
        idleState();
    }

    void goToIdleSpecial()
    {
        actions.rapidBeep();
        idleState();
    }

    void waterOn()
    {
        state = state_waterOn;
        actions.greenLedOn();
        actions.longBeep();
        actions.showShowerTime();
        actions.displayBright();
        actions.coldTimerStart();
        actions.valveOpen();
    }

    void showerRunning()
    {
        state = state_showerRunning;
        actions.coldTimerStop();
        actions.showerTimerStart();
        actions.greenLedFlashing();
        actions.longBeep();
    }

    void override()
    {
        state = state_override;
        actions.rapidBeep();
        actions.alternateLedsFlashing();
        actions.valveOpen();
        actions.showShowerTime();
        actions.displayBright();
    }
};




