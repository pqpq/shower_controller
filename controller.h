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
    void startButton() override final
    {
        if (state == state_idle)
        {
            waterOn();
        }
    }

    void coldTimerExpired() override final
    {
        if (state != state_idle && state != state_override && state != state_lockout)
        {
            goToIdleNormal();
        }
    }

    void showerHot() override final
    {
        if (state == state_waterOn || state == state_showerOffStillTiming)
        {
            showerRunning();
        }
    }

    virtual void showerCold()
    {
        if (state == state_showerRunning)
        {
            state = state_showerOffStillTiming;
        }
    }

    virtual void fiveMinutesToGo()
    {
        if (state == state_showerRunning)
        {
            actions.shortBeep();
        }
    }

    virtual void oneMinuteToGo()
    {
        if (state == state_showerRunning)
        {
            actions.longBeep();
            actions.showFinalCountdown();
            state = state_showerRunningFinalCountdown;
        }
        if (state == state_showerOffStillTiming)
        {
            actions.showFinalCountdown();
            state = state_showerOffStillTimingFinalCountdown;
        }
    }

    virtual void fiveSecondsPassed()
    {
        if (state == state_showerRunningFinalCountdown)
        {
            actions.shortBeep();
        }
        if (state == state_showerRunningVeryEnd)
        {
            actions.rapidBeep();
        }
    }

    virtual void tenSecondsToGo()
    {
        if (state == state_showerRunning)
        {
            actions.rapidBeep();
            actions.displayFlash();
            state = state_showerRunningVeryEnd;
        }
        if (state == state_showerOffStillTiming)
        {
            actions.displayFlash();
            state = state_showerOffStillTimingVeryEnd;
        }
    }

    virtual void showerTimerExpired()
    {
        if (state == state_showerRunning ||
            state == state_showerOffStillTiming ||
            state == state_showerRunningFinalCountdown ||
            state == state_showerOffStillTimingFinalCountdown ||
            state == state_showerRunningVeryEnd ||
            state == state_showerOffStillTimingVeryEnd)
        {
            state = state_lockout;
            actions.redLedFlashing();
            actions.valveClosed();
            actions.showLockoutTime();
        }
    }

    virtual void dongleIn() { override(); }
    virtual void dongleOut() { goToIdleSpecial(); }

    virtual void reset()
    {
        if (state != state_override)
        {
            goToIdleSpecial();
        }
    }

    virtual void plusButton()
    {
        if (state == state_override)
        {
            actions.shortBeep();
            actions.timeAdd();
        }
    }

    virtual void minusButton()
    {
        if (state == state_override)
        {
            actions.shortBeep();
            actions.timeRemove();
        }
    }

    virtual void lockoutTimerExpired()
    {
        if (state == state_lockout)
        {
            goToIdleNoBeep();
        }
    }

private:

    enum State
    {
        state_idle,
        state_waterOn,
        state_showerRunning,
        state_showerOffStillTiming,
        state_showerRunningFinalCountdown,
        state_showerOffStillTimingFinalCountdown,
        state_showerRunningVeryEnd,
        state_showerOffStillTimingVeryEnd,
        state_lockout,
        state_override
    };

    Actions& actions;
    State state;

    void goToIdleNoBeep()
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
        goToIdleNoBeep();
    }

    void goToIdleSpecial()
    {
        if (state == state_override)
        {
            actions.timeSave();
        }
        actions.rapidBeep();
        goToIdleNoBeep();
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
        actions.shortBeep();
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




