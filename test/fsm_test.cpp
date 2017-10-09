// Include all UUTs twice to test include guard
// Include them before all other includes to ensure they stand alone.
#include "../controller.h"
#include "../controller.h"
#include "../actions.h"
#include "../actions.h"
#include "../events.h"
#include "../events.h"

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_COLOUR_NONE
#include "catch.hpp"

#include <vector>
#include <string>

using namespace std;


/// Test implementation of the Actions interface that allows us to interrogate
/// it after a test and work out which Actions have been called, and in what
/// order.
class TestActions : public Actions
{
public:
    TestActions() { reset(); }

    virtual void valveClosed()          { _calls.push_back("valveClosed"); }
    virtual void valveOpen()            { _calls.push_back("valveOpen"); }

    virtual void ledOn()                { _calls.push_back("ledOn"); }
    virtual void ledFlashing()          { _calls.push_back("ledFlashing"); }

    virtual void showShowerTime()       { _calls.push_back("showShowerTime"); }
    virtual void showFinalCountdown()   { _calls.push_back("showFinalCountdown"); }
    virtual void showLockoutTime()      { _calls.push_back("showLockoutTime"); }
    virtual void displayOff()           { _calls.push_back("displayOff"); }
    virtual void displayOn()            { _calls.push_back("displayOn"); }
    virtual void displayFlash()         { _calls.push_back("displayFlash"); }
    virtual void displayPulse()         { _calls.push_back("displayPulse"); }

    virtual void shortBeep()            { _calls.push_back("shortBeep"); }
    virtual void longBeep()             { _calls.push_back("longBeep"); }
    virtual void rapidBeeps()           { _calls.push_back("rapidBeeps"); }

    virtual void showTimerStart()       { _calls.push_back("showTimerStart"); }
    virtual void showerTimerStart()     { _calls.push_back("showerTimerStart"); }
    virtual void lockoutTimerStart()    { _calls.push_back("lockoutTimerStart"); }

    virtual void timeAdd()              { _calls.push_back("timeAdd"); }
    virtual void timeRemove()           { _calls.push_back("timeRemove"); }
    virtual void timeSave()             { _calls.push_back("timeSave"); }

    void reset()
    {
        _calls.clear();
    }

    size_t n(const char* fn) const
    {
        return count(_calls.begin(), _calls.end(), fn);
    }

    const vector<string>& calls() const { return _calls; }

private:

    /// Remember which calls have been made.
    /// By simply remembering the function names in the order they were called
    /// we can test for:
    /// 1) whether a function has been called (is it in the container?)
    /// 2) how many times it has been called (count instances in the container)
    /// 3) order of calls (compare container with expected order)
    vector<string> _calls;
};


TEST_CASE("Check a new instance puts the hardware in the right state")
{
    TestActions ta;
    const Controller uut(ta);

    REQUIRE(ta.n("shortBeep") == 1);
    REQUIRE(ta.n("displayOff") == 1);
    REQUIRE(ta.n("ledFlashing") == 1);
    REQUIRE(ta.n("valveClosed") == 1);
    // Check no other calls
    CHECK(ta.calls().size() == 4);
}


struct TestVector
{
    std::string testName;
    std::vector<std::string> eventsToGetToTestState;
    std::vector<std::string> eventsToApply;
    std::vector<std::string> expectedActions;
};

void apply(Controller& uut, std::string event)
{
    if (event == "startButton")         { uut.startButton(); return; }
    if (event == "plusButton")          { uut.plusButton(); return; }
    if (event == "minusButton")         { uut.minusButton(); return; }

    if (event == "dongleIn")            { uut.dongleIn(); return; }
    if (event == "dongleOut")           { uut.dongleOut(); return; }
    if (event == "reset")               { uut.reset(); return; }

    if (event == "showTimerExpired")    { uut.showTimerExpired(); return; }
    if (event == "showerTimerExpired")  { uut.showerTimerExpired(); return; }
    if (event == "lockoutTimerExpired") { uut.lockoutTimerExpired(); return; }
    if (event == "fiveMinutesToGo")     { uut.fiveMinutesToGo(); return; }
    if (event == "oneMinuteToGo")       { uut.oneMinuteToGo(); return; }
    if (event == "fiveSecondsPassed")   { uut.fiveSecondsPassed(); return; }
    if (event == "oneSecondPassed")     { uut.oneSecondPassed(); return; }

    FAIL("unknown event: " + event);
}

void apply(const TestVector& v)
{
    TestActions ta;
    Controller uut(ta);

    for (const auto& e : v.eventsToGetToTestState)
    {
        apply(uut, e);
    }

    ta.reset();     // Clear side effects of getting to the test state

    for (const auto& e : v.eventsToApply)
    {
        apply(uut, e);
    }

    REQUIRE(ta.calls() == v.expectedActions);
}


const TestVector stateTests[] =
{
    // Idle state
    {
        "Init (Idle) + start button -> Show time",
        { },
        { "startButton" },
        { "showTimerStart", "showShowerTime", "displayOn", "shortBeep" }
    },
    {
        "Init (Idle) + dongle in -> Override",
        { },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTime", "displayPulse", "valveOpen" }
    },
    {
        "Init (Idle) + reset -> stay in idle, feedback that reset occurred",
        { },
        { "reset" },
        { "rapidBeeps", "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Init (Idle) + events that should be ignored -> no effect",
        { },
        {
            "plusButton", "minusButton", "showTimerExpired", "showerTimerExpired",
            "lockoutTimerExpired", "fiveMinutesToGo", "oneMinuteToGo",
            "fiveSecondsPassed", "oneSecondPassed"
        },
        { }
    },

    // Showing time state
    {
        "Showing time + show timer expired -> Idle",
        { "startButton" },
        { "showTimerExpired" },
        { "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Showing time + start button -> Water On",
        { "startButton" },
        { "startButton" },
        { "longBeep", "valveOpen", "showShowerTime", "displayOn", "showerTimerStart", "ledFlashing" }
    },
    {
        "Showing time + dongle -> Override",
        { "startButton" },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTime", "displayPulse", "valveOpen" }
    },
    {
        "Showing time + reset -> Idle",
        { "startButton" },
        { "reset" },
        { "rapidBeeps", "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Showing time + events that should be ignored -> no effect",
        { "startButton" },
        {
            "plusButton", "minusButton", "showerTimerExpired", "lockoutTimerExpired",
            "fiveMinutesToGo", "oneMinuteToGo", "fiveSecondsPassed", "oneSecondPassed"
        },
        { }
    },

    // Water On state
    {
        "Water On + start button -> Silent",
        { "startButton", "startButton" },
        { "startButton" },
        { "longBeep" }
    },
    {
        "Water On + dongle in -> Override",
        { "startButton", "startButton" },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTime", "displayPulse", "valveOpen" }
    },
    {
        "Water on + reset -> idle",
        { "startButton", "startButton" },
        { "reset" },
        { "rapidBeeps", "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Water on + 5 mins to go -> beep",
        { "startButton", "startButton" },
        { "fiveMinutesToGo" },
        { "shortBeep" }
    },
    {
        "Water on + 5 mins to go + 5 sec passed -> nothing",
        { "startButton", "startButton", "fiveMinutesToGo" },
        { "fiveSecondsPassed" },
        { }
    },
    {
        "Water on + 1 min to go -> warnings",
        { "startButton", "startButton" },
        { "oneMinuteToGo" },
        { "showFinalCountdown", "displayPulse", "longBeep" }
    },
    {
        "Water on + 1 min to go + 5 sec passed -> beep",
        { "startButton", "startButton", "oneMinuteToGo" },
        { "fiveSecondsPassed" },
        { "shortBeep" }
    },
    {
        "Water on, 1 min to go + 10 secs to go -> rapid beeps",
        { "startButton", "startButton", "oneMinuteToGo" },
        { "oneSecondPassed" },
        { "rapidBeeps" }
    },
    {
        "Water on, 1 min to go + 9 secs to go -> rapid beeps",
        { "startButton", "startButton", "oneMinuteToGo" },
        { "oneSecondPassed", "oneSecondPassed" },
        { "rapidBeeps", "rapidBeeps" }
    },
    {
        "Water on, 1 min to go + long start button -> Silent",
        { "startButton", "startButton", "oneMinuteToGo" },
        { "startButton", "oneSecondPassed" },
        { "longBeep" }
    },
    {
        "Water on + timer expired -> Lockout",
        { "startButton", "startButton" },
        { "showerTimerExpired" },
        { "ledOn", "valveClosed", "showLockoutTime", "displayFlash" }
    },
    {
        "Water on, usual sequence + timer expired -> Lockout",
        { "startButton", "startButton", "fiveMinutesToGo", "oneMinuteToGo", "fiveSecondsPassed", "oneSecondPassed" },
        { "showerTimerExpired" },
        { "ledOn", "valveClosed", "showLockoutTime", "displayFlash" }
    },
    {
        "Water on + events that should be ignored -> no effect",
        { "startButton", "startButton" },
        { "plusButton", "minusButton", "showTimerExpired", "lockoutTimerExpired" },
        { }
    },

    // Silent state
    {
        "Silent + long start button -> Water on",
        { "startButton", "startButton", "startButton" },
        { "startButton" },
        { "longBeep" }
    },
    {
        "Silent + dongle in -> Override",
        { "startButton", "startButton", "startButton" },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTime", "displayPulse", "valveOpen" }
    },
    {
        "Silent + reset -> idle",
        { "startButton", "startButton", "startButton" },
        { "reset" },
        { "rapidBeeps", "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Silent + 5 mins to go -> no beep",
        { "startButton", "startButton", "startButton" },
        { "fiveMinutesToGo" },
        { }
    },
    {
        "Silent + 1 min to go -> no beep",
        { "startButton", "startButton", "startButton" },
        { "oneMinuteToGo" },
        { "showFinalCountdown", "displayPulse" }
    },
    {
        "Silent, 1 min to go + long start button -> Water on",
        { "startButton", "startButton", "startButton", "oneMinuteToGo" },
        { "startButton" },
        { "longBeep" }
    },
    {
        "Silent, 1 min to go + 5 sec passed -> no beep",
        { "startButton", "startButton", "startButton", "oneMinuteToGo" },
        { "fiveSecondsPassed" },
        { }
    },
    {
        "Silent, 1 min to go + final 10 seconds -> no beep",
        { "startButton", "startButton", "startButton", "oneMinuteToGo" },
        { "oneSecondPassed" },
        { }
    },
    {
        "Silent, 1 min to go + final 9 seconds -> no beep",
        { "startButton", "startButton", "startButton", "oneMinuteToGo" },
        { "oneSecondPassed", "oneSecondPassed" },
        { }
    },
    {
        "Silent, usual sequence + timer expired -> Lockout",
        { "startButton", "startButton", "startButton", "fiveMinutesToGo", "oneMinuteToGo", "fiveSecondsPassed", "oneSecondPassed" },
        { "showerTimerExpired" },
        { "ledOn", "valveClosed", "showLockoutTime", "displayFlash" }
    },
    {
        "Silent + events that should be ignored -> no effect",
        { "startButton", "startButton", "startButton" },
        {
            "plusButton",
            "minusButton",
            "showTimerExpired",
            "lockoutTimerExpired",
            "fiveMinutesToGo",
            "fiveSecondsPassed",
            "oneSecondPassed",

        },
        { }
    },

    // Lockout state
    {
        "Lockout + long start button -> error beeps",
        { "startButton", "startButton", "showerTimerExpired" },
        { "startButton" },
        { "rapidBeeps" }
    },
    {
        "Lockout + lockout timer expired -> idle",
        { "startButton", "startButton", "showerTimerExpired" },
        { "lockoutTimerExpired" },
        { "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Lockout + dongle in -> Override",
        { "startButton", "startButton", "showerTimerExpired" },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTime", "displayPulse", "valveOpen" }
    },
    {
        "Lockout + reset -> idle",
        { "startButton", "startButton", "showerTimerExpired" },
        { "reset" },
        { "rapidBeeps", "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Lockout + events that should be ignored -> no effect",
        { "startButton", "startButton", "showerTimerExpired" },
        {
            "plusButton", "minusButton", "showTimerExpired", "showerTimerExpired",
            "fiveMinutesToGo", "oneMinuteToGo", "fiveSecondsPassed",
            "oneSecondPassed"
        },
        { }
    },

    // Override state
    {
        "Override + dongle out -> Idle",
        { "dongleIn" },
        { "dongleOut" },
        { "timeSave", "rapidBeeps", "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Override + long start button -> error beep",
        { "dongleIn" },
        { "startButton" },
        { "rapidBeeps" }
    },
    {
        "Override + plus Button -> increase time",
        { "dongleIn" },
        { "plusButton" },
        { "shortBeep", "timeAdd" }
    },
    {
        "Override + minus Button -> decrease time",
        { "dongleIn" },
        { "minusButton" },
        { "shortBeep", "timeRemove" }
    },
    {
        "Override + events that should be ignored -> no effect",
        { "dongleIn" },
        {
            "reset", "showTimerExpired", "showerTimerExpired",
            "lockoutTimerExpired", "fiveMinutesToGo", "oneMinuteToGo",
            "fiveSecondsPassed", "oneSecondPassed"
        },
        {  }
    }
};

TEST_CASE("Table driven FSM test")
{
    for (const auto& v : stateTests)
    {
        SECTION(v.testName)
        {
            apply(v);
        }
    }
}


const TestVector useCaseTests[] =
{
    {
        "Show time, back to idle",
        { },
        {
            "startButton",
            "showTimerExpired"
        },
        {
            "showTimerStart", "showShowerTime", "displayOn", "shortBeep",
            "displayOff", "ledFlashing", "valveClosed"
        }
    },
    {
        "Normal use, timing out",
        { },
        {
            // start
            "startButton",
            "startButton",
            // time passes during shower
            "fiveMinutesToGo",
            "oneMinuteToGo",
            "fiveSecondsPassed", "fiveSecondsPassed", "fiveSecondsPassed",
            "oneSecondPassed", "oneSecondPassed", "oneSecondPassed",
            // end
            "showerTimerExpired",
            "lockoutTimerExpired"
        },
        {
            "showTimerStart", "showShowerTime", "displayOn", "shortBeep",
            "longBeep", "valveOpen", "showShowerTime", "displayOn", "showerTimerStart", "ledFlashing",
            "shortBeep",
            "showFinalCountdown", "displayPulse", "longBeep",
            "shortBeep", "shortBeep", "shortBeep",
            "rapidBeeps", "rapidBeeps", "rapidBeeps",
            "ledOn", "valveClosed", "showLockoutTime", "displayFlash",
            "displayOff", "ledFlashing", "valveClosed"
        }
    },
/*
    {
        "Normal use, finishing just before timer expires",
        { },
        {
            // start
            "startButton",
            // time passes during shower
            "fiveMinutesToGo", "oneMinuteToGo",
            "fiveSecondsPassed", "fiveSecondsPassed", "fiveSecondsPassed",
            // stop
            "showerCold",
            // more time passes
            "fiveSecondsPassed", "fiveSecondsPassed", "fiveSecondsPassed",
            "tenSecondsToGo", "fiveSecondsPassed",
            "showerTimerExpired", "lockoutTimerExpired"
        },
        {
            // start
            "greenLedOn", "longBeep", "showShowerTime", "displayBright", "coldTimerStart", "valveOpen",
            // showering
            "coldTimerStop", "showerTimerStart", "greenLedFlashing", "shortBeep",
            "shortBeep",
            "longBeep", "showFinalCountdown",
            "shortBeep", "shortBeep", "shortBeep",
            "displayFlash",
            "redLedFlashing", "valveClosed", "showLockoutTime",
            "greenLedOn", "valveClosed", "showShowerTime", "displayDim"
        }
    },
    {
        "Turn on cold for a few minutes",
        { },
        { "startButton", "coldTimerExpired" },
        {
            // start
            "greenLedOn", "longBeep", "showShowerTime", "displayBright", "coldTimerStart", "valveOpen",
            // cold timer expired
            "shortBeep", "greenLedOn", "valveClosed", "showShowerTime", "displayDim"
        }
    },
    {
        "Normal use, time set for less than 5 mins, timing out",
        { },
        {
            // start
            "startButton",
            // time passes during shower
            "oneMinuteToGo",
            "fiveSecondsPassed", "fiveSecondsPassed", "fiveSecondsPassed",
            "tenSecondsToGo", "fiveSecondsPassed",
            "showerTimerExpired", "lockoutTimerExpired"
        },
        {
            // start
            "greenLedOn", "longBeep", "showShowerTime", "displayBright", "coldTimerStart", "valveOpen",
            // showering
            "coldTimerStop", "showerTimerStart", "greenLedFlashing", "shortBeep",
            "longBeep", "showFinalCountdown",
            "shortBeep", "shortBeep", "shortBeep",
            "rapidBeep", "displayFlash", "rapidBeep",
            "redLedFlashing", "valveClosed", "showLockoutTime",
            "greenLedOn", "valveClosed", "showShowerTime", "displayDim"
        }
    },
*/
};

TEST_CASE("Table driven use case tests")
{
    for (const auto& v : useCaseTests)
    {
        SECTION(v.testName)
        {
            apply(v);
        }
    }
}


// all events
/*

"startButtonShort",
"startButtonLong",
"plusButton",
"minusButton",
"dongleIn",
"dongleOut",
"reset",
"showTimerExpired",
"showerTimerExpired",
"lockoutTimerExpired",
"fiveMinutesToGo",
"oneMinuteToGo",
"fiveSecondsPassed"
"oneSecondPassed",

 */
