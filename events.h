#pragma once

/// The events that drive our state machine.
///
/// Whether an event has occurred is outside the scope of the state machine, but
/// it is obviously done/calculated/sensed by the underlying hardware interface.
class Events
{
public:
    virtual ~Events() {}

    // Push buttons

    /// A press of the "start" pushbutton.
    virtual void startButton() = 0;

    /// The '+' pushbutton that adds time when the dongle is plugged in.
    virtual void plusButton() = 0;

    /// The '-' pushbutton that adds time when the dongle is plugged in.
    virtual void minusButton() = 0;


    // External hardware interface

    /// The override dongle has been plugged in.
    virtual void dongleIn() = 0;

    /// The override dongle has been pulled out.
    virtual void dongleOut() = 0;

    /// The reset switch/button/device has been triggered.
    virtual void reset() = 0;


    // Internal timers

    /// The "show time" timer has expired.
    /// This briefly shows the timer time, when the unit is idle.
    virtual void showTimerExpired() = 0;

    /// The shower timer has expired.
    /// Turn off the shower and prevent another shower for a while.
    virtual void showerTimerExpired() = 0;

    /// The lockout timer has expired.
    /// Allow another shower if the start button is pressed.
    virtual void lockoutTimerExpired() = 0;

    /// Mark each minute, in the last 5 minutes of the shower timer. Warn the user.
    virtual void oneMinutePassed() = 0;

    /// We're into the last minute of the shower timer. Warn the user.
    virtual void lastMinute() = 0;

    /// Five more seconds have passed.
    /// Depending on state we might want to beep or something.
    virtual void fiveSecondsPassed() = 0;

    /// One second has passed and we're in the last 10 seconds of the shower timer.
    /// We rely on the system to only call this when we're in the final stages.
    virtual void oneSecondPassed() = 0;

};
