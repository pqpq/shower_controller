#pragma once

#include "actions.h"
#include "events.h"

class Controller : public Events
{
public:
    Controller(Actions& a) : actions(a) { idle(); }

    // Events
    void start() override final { waterOn(); }

private:

    Actions& actions;

    void idle()
    {
        actions.greenLedOn();
        actions.valveClosed();
        actions.shortBeep();
        actions.showShowerTime();
        actions.displayDim();
    }

    void waterOn()
    {
        actions.greenLedOn();
        actions.longBeep();
        actions.showShowerTime();
        actions.displayBright();
        actions.startColdTimer();
        actions.valveOpen();
    }
};




