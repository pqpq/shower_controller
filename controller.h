#pragma once

#include "actions.h"

class Controller
{
public:
    Controller(Actions& a) : actions(a) { idle(); }

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
};




