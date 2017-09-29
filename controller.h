#pragma once

#include "actions.h"
#include "events.h"

class Controller : public Events
{
public:
    Controller(Actions& a) : actions(a) { idle(); }

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
            idle();
        }
    }

    void showerHot() override final
    {
        if (state == state_waterOn)
        {
            showerRunning();
        }
    }


private:

    enum State
    {
        state_idle,
        state_waterOn,
        state_showerRunning,
    };

    Actions& actions;
    State state;

    void idle()
    {
        state = state_idle;
        actions.greenLedOn();
        actions.valveClosed();
        actions.shortBeep();
        actions.showShowerTime();
        actions.displayDim();
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
};




