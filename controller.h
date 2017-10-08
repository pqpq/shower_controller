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
            state = waterOn;
            actions.displayOff();
            actions.ledFlashing();
            actions.showShowerTime();
            actions.valveOpen();
        }
    }

    void startButtonLong() override final
    {
        if (state == idle)
        {
            state = waterOn;
            actions.displayOff();
            actions.ledFlashing();
            actions.showShowerTime();
            actions.valveOpen();
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
        actions.valveOpen();
        actions.showShowerTime();
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
        if (state != idle && state != override && state != lockout)
        {
            goToIdleNormal();
        }
    }

    virtual void showerTimerExpired()
    {
        if (state == showerRunning ||
            state == showerOffStillTiming ||
            state == showerRunningFinalCountdown ||
            state == showerOffStillTimingFinalCountdown ||
            state == showerRunningVeryEnd ||
            state == showerOffStillTimingVeryEnd)
        {
            state = lockout;
            actions.valveClosed();
            actions.showLockoutTime();
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
        if (state == showerRunning)
        {
            actions.shortBeep();
        }
    }

    virtual void oneMinuteToGo()
    {
        if (state == showerRunning)
        {
            actions.longBeep();
            actions.showFinalCountdown();
            state = showerRunningFinalCountdown;
        }
        if (state == showerOffStillTiming)
        {
            actions.showFinalCountdown();
            state = showerOffStillTimingFinalCountdown;
        }
    }

    virtual void tenSecondsToGo()
    {
        if (state == showerRunning || state == showerRunningFinalCountdown)
        {
            actions.rapidBeeps();
            actions.displayFlash();
            state = showerRunningVeryEnd;
        }
        if (state == showerOffStillTiming || state == showerOffStillTimingFinalCountdown)
        {
            actions.displayFlash();
            state = showerOffStillTimingVeryEnd;
        }
    }

    virtual void fiveSecondsPassed()
    {
        if (state == showerRunningFinalCountdown)
        {
            actions.shortBeep();
        }
        if (state == showerRunningVeryEnd)
        {
            actions.rapidBeeps();
        }
    }

private:

    enum State
    {
        idle,
        waterOn,
        showerRunning,
        showerRunningFinalCountdown,
        showerRunningVeryEnd,
        showerOffStillTiming,
        showerOffStillTimingFinalCountdown,
        showerOffStillTimingVeryEnd,
        lockout,
        override
    };

    Actions& actions;
    State state;

    void goToIdleNoBeep()
    {
        state = idle;
        actions.valveClosed();
        actions.showShowerTime();
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




