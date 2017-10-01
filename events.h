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

    /// The "start" pushbutton that tells us the user wants to start the shower.
    virtual void startButton() = 0;

    /// The '+' pushbutton that adds time when the dongle is plugged in.
    virtual void plusButton() = 0;

    /// The '-' pushbutton that adds time when the dongle is plugged in.
    virtual void minusButton() = 0;


    // External hardware interface

    /// Significant current has been detected, so the shower heater is on.
    virtual void showerHot() = 0;

    /// Shower heater current has stopped, so the shower is off or cold.
    virtual void showerCold() = 0;

    /// The override dongle has been plugged in.
    virtual void dongleIn() = 0;

    /// The override dongle has been pulled out.
    virtual void dongleOut() = 0;

    /// The reset switch/button/device has been triggered.
    virtual void reset() = 0;


    // Internal timers

    /// The cold timer has expired. This allows the shower to be run cold for
    /// cleaning etc. without risking a lockout.
    virtual void coldTimerExpired() = 0;

    /// The shower timer has expired.
    /// Turn off the shower and prevent another shower for a while.
    virtual void showerTimerExpired() = 0;

    /// The lockout timer has expired.
    /// Allow another shower if the start button is pressed.
    virtual void lockoutTimerExpired() = 0;

    /// We're into the last 5 minutes of the shower timer. Warn the user.
    virtual void fiveMinutesToGo() = 0;

    /// We're into the last minute of the shower timer. Warn the user.
    virtual void oneMinuteToGo() = 0;

    /// We're into the last 10 seconds of the shower timer. Warn the user.
    virtual void tenSecondsToGo() = 0;

    /// Five more seconds have passed.
    /// Depending on state we might want to beep or something.
    virtual void fiveSecondsPassed() = 0;
};
