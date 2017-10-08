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
    if (event == "startButtonShort")    { uut.startButtonShort(); return; }
    if (event == "startButtonLong")     { uut.startButtonLong(); return; }
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
    if (event == "tenSecondsToGo")      { uut.tenSecondsToGo(); return; }
    if (event == "fiveSecondsPassed")   { uut.fiveSecondsPassed(); return; }

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
        "Init (Idle) + short start button -> Show time",
        { },
        { "startButtonShort" },
        { "showTimerStart", "showShowerTime", "displayOn", "shortBeep" }
    },
    {
        "Init (Idle) + long start button -> Water On",
        { },
        { "startButtonLong" },
        { "valveOpen", "longBeep", "showShowerTime", "displayOn", "showerTimerStart", "ledFlashing" }
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
            "tenSecondsToGo", "fiveSecondsPassed"
        },
        { }
    },

    // Showing time state
    {
        "Showing time + show timer expired -> Idle",
        { "startButtonShort" },
        { "showTimerExpired" },
        { "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Showing time + long start button -> Water On",
        { "startButtonShort" },
        { "startButtonLong" },
        { "valveOpen", "longBeep", "showShowerTime", "displayOn", "showerTimerStart", "ledFlashing" }
    },
    {
        "Showing time + dongle -> Override",
        { "startButtonShort" },
        { "dongleIn" },
        { "rapidBeeps", "ledFlashing", "showShowerTime", "displayPulse", "valveOpen" }
    },
    {
        "Showing time + reset -> Idle",
        { "startButtonShort" },
        { "reset" },
        { "rapidBeeps", "displayOff", "ledFlashing", "valveClosed" }
    },
    {
        "Showing time + events that should be ignored -> no effect",
        { "startButtonShort" },
        {
            "startButtonShort", "plusButton", "minusButton", "showerTimerExpired",
            "lockoutTimerExpired", "fiveMinutesToGo", "oneMinuteToGo",
            "tenSecondsToGo", "fiveSecondsPassed"
        },
        { }
    }

    /*
    // Water On state
    {
        "Water On + cold timer expired -> idle",
        { "startButton" }, // get to test state
        { "coldTimerExpired" },
        { "shortBeep", "greenLedOn", "valveClosed", "showShowerTime", "displayDim" }
    },
    {
        "Water On + shower hot -> shower running",
        { "startButton" }, // get to test state
        { "showerHot" },
        { "coldTimerStop", "showerTimerStart", "greenLedFlashing", "shortBeep" }
    },
    {
        "Water On + dongle in -> Override",
        { "startButton" }, // get to test state
        { "dongleIn" },
        { "rapidBeep", "alternateLedsFlashing", "valveOpen", "showShowerTime", "displayBright" }
    },
    {
        "Water on + reset -> idle",
        { "startButton" }, // get to test state
        { "reset" },
        { "rapidBeep", "greenLedOn", "valveClosed", "showShowerTime", "displayDim" }
    },
    {
        "Water On + events that should be ignored -> no effect",
        { "startButton" }, // get to test state
        { "startButton", "showerCold", "fiveMinutesToGo", "oneMinuteToGo", "fiveSecondsPassed",
          "tenSecondsToGo", "showerTimerExpired", "plusButton", "minusButton", "lockoutTimerExpired" },
        { }
    },

    // Shower running state - normal sequence
    {
        "Shower running + 5 mins to go -> beep",
        { "startButton", "showerHot" }, // get to test state
        { "fiveMinutesToGo" },
        { "shortBeep" }
    },
    {
        "Shower running + 5 mins to go + 5 sec passed -> nothing",
        { "startButton", "showerHot", "fiveMinutesToGo" }, // get to test state
        { "fiveSecondsPassed" },
        { }
    },
    {
        "Shower running + 1 min to go -> warnings",
        { "startButton", "showerHot" }, // get to test state
        { "oneMinuteToGo" },
        { "longBeep", "showFinalCountdown" }
    },
    {
        "Shower running + 1 min to go + 5 sec passed -> beep",
        { "startButton", "showerHot", "oneMinuteToGo" }, // get to test state
        { "fiveSecondsPassed" },
        { "shortBeep" }
    },
    {
        "Shower running, 1 min to go + 10 secs to go -> warnings",
        { "startButton", "showerHot", "oneMinuteToGo" }, // get to test state
        { "tenSecondsToGo" },
        { "rapidBeep", "displayFlash" }
    },
    {
        "Shower running + 10 secs to go + 5 sec passed -> beep",
        { "startButton", "showerHot", "tenSecondsToGo" }, // get to test state
        { "fiveSecondsPassed" },
        { "rapidBeep" }
    },
    {
        "Shower running + timer expired -> Lockout",
        { "startButton", "showerHot" }, // get to test state
        { "showerTimerExpired" },
        { "redLedFlashing", "valveClosed", "showLockoutTime" }
    },

    // Shower running state - shower off before end
    {
        "Shower running + then off, 5 mins to go -> no beep",
        { "startButton", "showerHot", "showerCold" }, // get to test state
        { "fiveMinutesToGo" },
        { }
    },
    {
        "Shower running + then off, 1 min to go -> no beep",
        { "startButton", "showerHot", "showerCold" }, // get to test state
        { "oneMinuteToGo" },
        { "showFinalCountdown" }
    },
    {
        "Shower running + then off, 1 min to go + 5 sec passed -> no beep",
        { "startButton", "showerHot", "showerCold", "oneMinuteToGo" }, // get to test state
        { "fiveSecondsPassed" },
        { }
    },
    {
        "Shower running + then off, 1 min to go + 10 secs to go -> warnings",
        { "startButton", "showerHot", "showerCold", "oneMinuteToGo" }, // get to test state
        { "tenSecondsToGo" },
        { "displayFlash" }
    },
    {
        "Shower running + then off + 1 min to go + 10 secs to go + 5 sec passed -> no beep",
        { "startButton", "showerHot", "showerCold", "oneMinuteToGo", "tenSecondsToGo" }, // get to test state
        { "fiveSecondsPassed" },
        { }
    },
    {
        "Shower running + then off then on again, 5 mins to go -> warnings",
        { "startButton", "showerHot", "showerCold", "showerHot" }, // get to test state
        { "fiveMinutesToGo" },
        { "shortBeep" }
    },
    {
        "Shower running + then off then on again, 5 mins to go + 5 sec passed -> nothing",
        { "startButton", "showerHot", "showerCold", "showerHot", "fiveMinutesToGo" }, // get to test state
        { "fiveSecondsPassed" },
        { }
    },
    {
        "Shower running + then off then on again, 1 min to go -> warnings",
        { "startButton", "showerHot", "showerCold", "showerHot" }, // get to test state
        { "oneMinuteToGo" },
        { "longBeep", "showFinalCountdown" }
    },
    {
        "Shower running + then off then on again, 1 min to go + 5 sec passed -> beep",
        { "startButton", "showerHot", "showerCold", "showerHot", "oneMinuteToGo" }, // get to test state
        { "fiveSecondsPassed" },
        { "shortBeep" }
    },
    {
        "Shower running + then off then on again, 1 min to go + 10 secs to go -> warnings",
        { "startButton", "showerHot", "showerCold", "showerHot", "oneMinuteToGo" }, // get to test state
        { "tenSecondsToGo" },
        { "rapidBeep", "displayFlash" }
    },
    {
        "Shower running + then off then on again, 10 secs to go + 5 sec passed -> beep",
        { "startButton", "showerHot", "showerCold", "showerHot", "tenSecondsToGo" }, // get to test state
        { "fiveSecondsPassed" },
        { "rapidBeep" }
    },

    {
        "Shower running + dongle in -> Override",
        { "startButton", "showerHot" }, // get to test state
        { "dongleIn" },
        { "rapidBeep", "alternateLedsFlashing", "valveOpen", "showShowerTime", "displayBright" }
    },
    {
        "Shower running + reset -> idle",
        { "startButton", "showerHot" }, // get to test state
        { "reset" },
        { "rapidBeep", "greenLedOn", "valveClosed", "showShowerTime", "displayDim" }
    },
    {
        "Shower running + events that should be ignored -> no effect",
        { "startButton", "showerHot" }, // get to test state
        { "startButton", "plusButton", "minusButton", "lockoutTimerExpired" },
        { }
    },

    // Lockout state
    {
        "Lockout + lockout timer expired -> idle",
        { "startButton", "showerHot", "showerTimerExpired" }, // get to test state
        { "lockoutTimerExpired" },
        { "greenLedOn", "valveClosed", "showShowerTime", "displayDim" }
    },
    {
        "Lockout + dongle in -> Override",
        { "startButton", "showerHot", "showerTimerExpired" }, // get to test state
        { "dongleIn" },
        { "rapidBeep", "alternateLedsFlashing", "valveOpen", "showShowerTime", "displayBright" }
    },
    {
        "Lockout + reset -> idle",
        { "startButton", "showerHot", "showerTimerExpired" }, // get to test state
        { "reset" },
        { "rapidBeep", "greenLedOn", "valveClosed", "showShowerTime", "displayDim" }
    },
    {
        "Lockout + events that should be ignored -> no effect",
        { "startButton", "showerHot", "showerTimerExpired" }, // get to test state
        { "startButton", "coldTimerExpired", "showerHot", "showerCold", "fiveMinutesToGo",
          "oneMinuteToGo", "fiveSecondsPassed", "tenSecondsToGo", "showerTimerExpired",
          "plusButton", "minusButton" },
        { }
    },

    // Override state
    {
        "Override + dongle out -> Idle",
        { "dongleIn" }, // get to test state
        { "dongleOut" },
        { "timeSave", "rapidBeep", "greenLedOn", "valveClosed", "showShowerTime", "displayDim" }
    },
    {
        "Override + plus Button -> increase time",
        { "dongleIn" }, // get to test state
        { "plusButton" },
        { "shortBeep", "timeAdd" }
    },
    {
        "Override + minus Button -> decrease time",
        { "dongleIn" }, // get to test state
        { "minusButton" },
        { "shortBeep", "timeRemove" }
    },
    {
        "Override + events that should be ignored -> no effect",
        { "dongleIn" }, // get to test state
        { "startButton", "coldTimerExpired", "showerHot", "showerCold", "fiveMinutesToGo",
          "oneMinuteToGo", "fiveSecondsPassed", "tenSecondsToGo", "showerTimerExpired",
          "lockoutTimerExpired", "reset" },
        {  }
    }
    */
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
        "Normal use, timing out",
        { },
        {
            // start
            "startButton", "showerHot",
            // time passes during shower
            "fiveMinutesToGo", "oneMinuteToGo",
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
            "rapidBeep", "displayFlash", "rapidBeep",
            "redLedFlashing", "valveClosed", "showLockoutTime",
            "greenLedOn", "valveClosed", "showShowerTime", "displayDim"
        }
    },
    {
        "Normal use, finishing just before timer expires",
        { },
        {
            // start
            "startButton", "showerHot",
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
            "startButton", "showerHot",
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
};

TEST_CASE("Table driven use case tests")
{
    for (const auto& v : useCaseTests)
    {
        SECTION(v.testName)
        {
            //apply(v);
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
"tenSecondsToGo",
"fiveSecondsPassed"

 */
