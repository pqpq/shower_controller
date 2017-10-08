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
        if (state == on)
        {
            actions.rapidBeeps();
        }
    }

    void startButtonLong() override final
    {
        if (state == on)
        {
            state = onNoBeeps;
            actions.longBeep();
        }
        if (state == idle || state == showingTime)
        {
            state = on;
            actions.valveOpen();
            actions.longBeep();
            actions.showShowerTime();
            actions.displayOn();
            actions.showerTimerStart();
            actions.ledFlashing();
        }
    }

    virtual void plusButton()
    {
//        if (state == override)
//        {
//            actions.shortBeep();
//            actions.timeAdd();
//        }
    }

    virtual void minusButton()
    {
//        if (state == override)
//        {
//            actions.shortBeep();
//            actions.timeRemove();
//        }
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
//        goToIdleSpecial();
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
        if (state == on ||
            state == finalCountdown )
//            state == showerRunningFinalCountdown ||
//            state == showerOffStillTimingFinalCountdown ||
//            state == showerRunningVeryEnd ||
//            state == showerOffStillTimingVeryEnd)
        {
//            state = lockout;
            actions.ledOn();
            actions.valveClosed();
            actions.showLockoutTime();
            actions.displayFlash();
        }
    }


    virtual void lockoutTimerExpired()
    {
//        if (state == lockout)
//        {
//            goToIdleNoBeep();
//        }
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
//        if (state == showerOffStillTiming)
//        {
//            actions.showFinalCountdown();
//            state = showerOffStillTimingFinalCountdown;
//        }
    }

    virtual void fiveSecondsPassed()
    {
        if (state == finalCountdown)
        {
            actions.shortBeep();
        }
//        if (state == showerRunningVeryEnd)
//        {
//            actions.rapidBeeps();
//        }
    }

    virtual void oneSecondPassed()
    {
        if (state == on || state == finalCountdown)
        {
            actions.rapidBeeps();
        }
//        if (state == showerOffStillTiming || state == showerOffStillTimingFinalCountdown)
//        {
//            actions.displayFlash();
//            state = showerOffStillTimingVeryEnd;
//        }
    }

private:

    enum State
    {
        idle,
        showingTime,
        on,
        onNoBeeps,
        //showerRunning,
        finalCountdown,
        //showerRunningVeryEnd,
        //showerOffStillTiming,
        //showerOffStillTimingFinalCountdown,
        //showerOffStillTimingVeryEnd,
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




