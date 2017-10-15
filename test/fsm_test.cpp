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

    virtual void showShowerTotalTime()  { _calls.push_back("showShowerTotalTime"); }
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
        "Init (Idle) + start button -> Show total time",
        { },
        { "startButton" },
        { "showTimerStart", "showShowerTotalTime", "displayOn", "shortBeep" }
    },
    {
        "Init (Idle) + dongle in -> Override",
        { },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTotalTime", "displayPulse", "valveOpen" }
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
        { "longBeep", "valveOpen", "showerTimerStart", "showShowerTime", "ledFlashing" }
    },
    {
        "Showing time + dongle -> Override",
        { "startButton" },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTotalTime", "displayPulse", "valveOpen" }
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
        { "rapidBeeps", "ledFlashing", "showShowerTotalTime", "displayPulse", "valveOpen" }
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
        { "longBeep" }
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
        { "longBeep" }
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
        { "ledOn", "valveClosed", "lockoutTimerStart", "showLockoutTime", "displayFlash" }
    },
    {
        "Water on, usual sequence + timer expired -> Lockout",
        { "startButton", "startButton", "fiveMinutesToGo", "oneMinuteToGo", "fiveSecondsPassed", "oneSecondPassed" },
        { "showerTimerExpired" },
        { "ledOn", "valveClosed", "lockoutTimerStart", "showLockoutTime", "displayFlash" }
    },
    {
        "Water on + events that should be ignored -> no effect",
        { "startButton", "startButton" },
        { "plusButton", "minusButton", "showTimerExpired", "lockoutTimerExpired" },
        { }
    },

    // Silent state
    {
        "Silent + start button -> Water on",
        { "startButton", "startButton", "startButton" },
        { "startButton" },
        { "longBeep" }
    },
    {
        "Silent + dongle in -> Override",
        { "startButton", "startButton", "startButton" },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTotalTime", "displayPulse", "valveOpen" }
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
        { "ledOn", "valveClosed", "lockoutTimerStart", "showLockoutTime", "displayFlash" }
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
            "oneSecondPassed"
        },
        { }
    },

    // Lockout state
    {
        "Lockout + start button -> error beeps",
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
        { "rapidBeeps", "ledFlashing", "showShowerTotalTime", "displayPulse", "valveOpen" }
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
        "Override + start button -> error beep",
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
            "showTimerStart", "showShowerTotalTime", "displayOn", "shortBeep",
            "displayOff", "ledFlashing", "valveClosed"
        }
    },
    {
        "Normal use, timing out",
        { },
        {
            // start
            "startButton", "startButton",
            // time passes during shower
            "fiveMinutesToGo",
            "oneMinuteToGo",
            "fiveSecondsPassed", "fiveSecondsPassed", "fiveSecondsPassed",
            "oneSecondPassed", "oneSecondPassed", "oneSecondPassed",
            // end
            "showerTimerExpired", "lockoutTimerExpired"
        },
        {
            "showTimerStart", "showShowerTotalTime", "displayOn", "shortBeep",
            "longBeep", "valveOpen", "showerTimerStart", "showShowerTime", "ledFlashing",
            "longBeep",
            "showFinalCountdown", "displayPulse", "longBeep",
            "longBeep", "longBeep", "longBeep",
            "rapidBeeps", "rapidBeeps", "rapidBeeps",
            "ledOn", "valveClosed", "lockoutTimerStart", "showLockoutTime", "displayFlash",
            "displayOff", "ledFlashing", "valveClosed"
        }
    },
    {
        "Normal use, timing out, silent half way through",
        { },
        {
            // start
            "startButton", "startButton",
            // time passes during shower
            "fiveMinutesToGo",
            "oneMinuteToGo",
            "fiveSecondsPassed", "fiveSecondsPassed", "fiveSecondsPassed",
            "startButton",
            "fiveSecondsPassed", "fiveSecondsPassed", "fiveSecondsPassed",
            "oneSecondPassed", "oneSecondPassed", "oneSecondPassed",
            // end
            "showerTimerExpired",
            "lockoutTimerExpired"
        },
        {
            "showTimerStart", "showShowerTotalTime", "displayOn", "shortBeep",
            "longBeep", "valveOpen", "showerTimerStart", "showShowerTime", "ledFlashing",
            "longBeep",
            "showFinalCountdown", "displayPulse", "longBeep",
            "longBeep", "longBeep", "longBeep",
            "longBeep",
            "ledOn", "valveClosed", "lockoutTimerStart", "showLockoutTime", "displayFlash",
            "displayOff", "ledFlashing", "valveClosed"
        }
    },
    {
        "Normal use, time set for less than 5 mins, timing out",
        { },
        {
            // start
            "startButton", "startButton",
            // time passes during shower
            "oneMinuteToGo",
            "fiveSecondsPassed", "fiveSecondsPassed", "fiveSecondsPassed",
            "oneSecondPassed", "oneSecondPassed", "oneSecondPassed",
            // end
            "showerTimerExpired", "lockoutTimerExpired"
        },
        {
            // start
            "showTimerStart", "showShowerTotalTime", "displayOn", "shortBeep",
            "longBeep", "valveOpen", "showerTimerStart", "showShowerTime", "ledFlashing",
            // showering
            "showFinalCountdown", "displayPulse", "longBeep",
            "longBeep", "longBeep", "longBeep",
            "rapidBeeps", "rapidBeeps", "rapidBeeps",
            "ledOn", "valveClosed", "lockoutTimerStart", "showLockoutTime", "displayFlash",
            "displayOff", "ledFlashing", "valveClosed"
        }
    },
    {
        "Normal use, timed out, then try to have another shower -> error beeps because we're locked out",
        { },
        {
            // start
            "startButton", "startButton",
            // end
            "showerTimerExpired",
            // start again
            "startButton", "startButton",
        },
        {
            // start
            "showTimerStart", "showShowerTotalTime", "displayOn", "shortBeep",
            "longBeep", "valveOpen", "showerTimerStart", "showShowerTime", "ledFlashing",
            // end
            "ledOn", "valveClosed", "lockoutTimerStart", "showLockoutTime", "displayFlash",
            // start again
            "rapidBeeps", "rapidBeeps",
        }
    },
    {
        "Normal use, timed out, then reset so we can start again",
        { },
        {
            // start
            "startButton", "startButton",
            // end
            "showerTimerExpired",
            "reset",
            // start again
            "startButton", "startButton",
        },
        {
            // start
            "showTimerStart", "showShowerTotalTime", "displayOn", "shortBeep",
            "longBeep", "valveOpen", "showerTimerStart", "showShowerTime", "ledFlashing",
            // end
            "ledOn", "valveClosed", "lockoutTimerStart", "showLockoutTime", "displayFlash",
            // reset
            "rapidBeeps", "displayOff", "ledFlashing", "valveClosed",
            // start again
            "showTimerStart", "showShowerTotalTime", "displayOn", "shortBeep",
            "longBeep", "valveOpen", "showerTimerStart", "showShowerTime", "ledFlashing"
        }
    },
    {
        "Dongle in, change time",
        { },
        {
            "dongleIn",
            "plusButton", "plusButton", "plusButton", "minusButton",
            "plusButton", "minusButton", "minusButton",
            "dongleOut"
        },
        {
            "rapidBeeps", "ledFlashing", "showShowerTotalTime", "displayPulse", "valveOpen",
            "shortBeep", "timeAdd", "shortBeep", "timeAdd", "shortBeep", "timeAdd",
            "shortBeep", "timeRemove", "shortBeep", "timeAdd", "shortBeep", "timeRemove",
            "shortBeep", "timeRemove",
            "timeSave", "rapidBeeps", "displayOff", "ledFlashing", "valveClosed"
        }
    }
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
