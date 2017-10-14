#pragma once

#include "actions.h"
#include "events.h"

/// Finite State Machine that responds to Events, causing Actions, depending
/// on the internal state.
///
/// This pattern is described in https://accu.org/index.php/journals/1548
/// It allows us to write and test (or TDD) the FSM with no dependencies on the
/// embedded system that it will eventually control.
///
class Controller : public Events
{
public:
    Controller(Actions& a) : actions(a), silent(false) { goToIdleNormal(); }

    // Events
    void startButton() override final
    {
        if (state == idle)
        {
            state = showingTime;
            actions.showTimerStart();
            actions.showShowerTotalTime();
            actions.displayOn();
            actions.shortBeep();
        }
        else if (state == showingTime)
        {
            state = on;
            actions.longBeep();
            actions.valveOpen();
            actions.showerTimerStart();
            actions.showShowerTime();
            actions.ledFlashing();
        }
        else if (state == on || state == finalCountdown)
        {
            actions.longBeep();
            silent = !silent;
        }
        else
        {
            actions.rapidBeeps();
        }
    }

    virtual void plusButton()
    {
        if (state == override)
        {
            actions.shortBeep();
            actions.timeAdd();
        }
    }

    virtual void minusButton()
    {
        if (state == override)
        {
            actions.shortBeep();
            actions.timeRemove();
        }
    }

    virtual void dongleIn()
    {
        state = override;
        actions.rapidBeeps();
        actions.ledFlashing();
        actions.showShowerTotalTime();
        actions.displayPulse();
        actions.valveOpen();
    }

    virtual void dongleOut()
    {
        goToIdleSpecial();
    }

    virtual void reset()
    {
        if (state != override)
        {
            goToIdleSpecial();
        }
    }

    void showTimerExpired() override final
    {
        if (state == showingTime)
        {
            goToIdleNoBeep();
        }
    }

    virtual void showerTimerExpired()
    {
        if (state == on || state == finalCountdown)
        {
            state = lockout;
            actions.ledOn();
            actions.valveClosed();
            actions.showLockoutTime();
            actions.displayFlash();
        }
    }

    virtual void lockoutTimerExpired()
    {
        if (state == lockout)
        {
            goToIdleNoBeep();
        }
    }

    virtual void fiveMinutesToGo()
    {
        if (state == on && !silent)
        {
            actions.shortBeep();
        }
    }

    virtual void oneMinuteToGo()
    {
        if (state == on)
        {
            state = finalCountdown;
            actions.showFinalCountdown();
            actions.displayPulse();
            if (!silent)
            {
                actions.longBeep();
            }
        }
    }

    virtual void fiveSecondsPassed()
    {
        if (state == finalCountdown && !silent)
        {
            actions.shortBeep();
        }
    }

    virtual void oneSecondPassed()
    {
        if ((state == on || state == finalCountdown) && !silent)
        {
            actions.rapidBeeps();
        }
    }

private:

    enum State
    {
        idle,
        showingTime,
        on,
        finalCountdown,
        lockout,
        override
    };

    Actions& actions;
    State state;
    bool silent;

    void goToIdleNoBeep()
    {
        state = idle;
        actions.displayOff();
        actions.ledFlashing();
        actions.valveClosed();
    }

    void goToIdleNormal()
    {
        actions.shortBeep();
        goToIdleNoBeep();
    }

    void goToIdleSpecial()
    {
        if (state == override)
        {
            actions.timeSave();
        }
        actions.rapidBeeps();
        goToIdleNoBeep();
    }
};




