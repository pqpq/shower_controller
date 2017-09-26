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

private:

    enum State
    {
        state_idle,
        state_waterOn
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
        actions.startColdTimer();
        actions.valveOpen();
    }
};




