#pragma once

#include "actions.h"
#include "events.h"

/// Finite State Machine that responds to Events, causing Actions, depending
/// on the internal state.
/// This pattern is described in https://accu.org/index.php/journals/1548
/// It allows us to write and test the FSM with no dependencies on the embedded
/// system that it will eventually control.

class Controller : public Events
{
public:
    Controller(Actions& a) : actions(a) { goToIdleNormal(); }

    // Events
    void startButtonShort() override final
    {
        if (state == idle)
        {
            state = showingTime;
            actions.showTimerStart();
            actions.showShowerTime();
            actions.displayOn();
            actions.shortBeep();
        }
        if (state == on || state == lockout || state == silent || state == override)
        {
            actions.rapidBeeps();
        }
    }

    void startButtonLong() override final
    {
        if (state == lockout || state == override)
        {
            actions.rapidBeeps();
        }
        else
        {
            actions.longBeep();
        }

        if (state == on)
        {
            state = silent;
        }
        else if (state == silent)
        {
            state = on;
        }

        if (state == finalCountdown)
        {
            state = finalCountdownSilent;
        }
        else if (state == finalCountdownSilent)
        {
            state = finalCountdown;
        }

        if (state == idle || state == showingTime)
        {
            state = on;
            actions.valveOpen();
            actions.showShowerTime();
            actions.displayOn();
            actions.showerTimerStart();
            actions.ledFlashing();
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
        actions.showShowerTime();
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
        if (state == on || state == finalCountdown ||
            state == silent || state == finalCountdownSilent)
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
        if (state == on)
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
            actions.longBeep();
        }
        if (state == silent)
        {
            state = finalCountdownSilent;
            actions.showFinalCountdown();
            actions.displayPulse();
        }
    }

    virtual void fiveSecondsPassed()
    {
        if (state == finalCountdown)
        {
            actions.shortBeep();
        }
    }

    virtual void oneSecondPassed()
    {
        if (state == on || state == finalCountdown)
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
        silent,
        finalCountdown,
        finalCountdownSilent,
        lockout,
        override
    };

    Actions& actions;
    State state;

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




