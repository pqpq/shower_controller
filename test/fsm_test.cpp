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

    virtual void valveClosed()      { _calls.push_back("valveClosed"); }
    virtual void valveOpen()        { _calls.push_back("valveOpen"); }

    virtual void greenLedOn()       { _calls.push_back("greenLedOn"); }
    virtual void greenLedOff()      { _calls.push_back("greenLedOff"); }
    virtual void greenLedFlashing() { _calls.push_back("greenLedFlashing"); }
    virtual void redLedFlashing()   { _calls.push_back("redLedFlashing"); }
    virtual void alternateLedsFlashing() { _calls.push_back("alternateLedsFlashing"); }

    virtual void showShowerTime()   { _calls.push_back("showShowerTime"); }
    virtual void showFinalCountdown(){ _calls.push_back("showFinalCountdown"); }
    virtual void showLockoutTime()  { _calls.push_back("showLockoutTime"); }
    virtual void displayDim()       { _calls.push_back("displayDim"); }
    virtual void displayBright()    { _calls.push_back("displayBright"); }
    virtual void displayFlash()     { _calls.push_back("displayFlash"); }

    virtual void shortBeep()        { _calls.push_back("shortBeep"); }
    virtual void longBeep()         { _calls.push_back("longBeep"); }
    virtual void rapidBeep()        { _calls.push_back("rapidBeep"); }

    virtual void coldTimerStart()   { _calls.push_back("coldTimerStart"); }
    virtual void coldTimerStop()    { _calls.push_back("coldTimerStop"); }
    virtual void showerTimerStart() { _calls.push_back("showerTimerStart"); }
    virtual void lockoutTimerStart(){ _calls.push_back("lockoutTimerStart"); }

    virtual void timeAdd()          { _calls.push_back("timeAdd"); }
    virtual void timeRemove()       { _calls.push_back("timeRemove"); }
    virtual void timeSave()         { _calls.push_back("timeSave"); }

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

    REQUIRE(ta.n("greenLedOn") == 1);
    REQUIRE(ta.n("valveClosed") == 1);
    REQUIRE(ta.n("showShowerTime") == 1);
    REQUIRE(ta.n("displayDim") == 1);
    REQUIRE(ta.n("shortBeep") == 1);
    CHECK(ta.calls().size() == 5);
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
    if (event == "startButton")         uut.startButton();
    if (event == "coldTimerExpired")    uut.coldTimerExpired();
    if (event == "showerHot")           uut.showerHot();
    if (event == "showerCold")          uut.showerCold();
    if (event == "fiveMinutesToGo")     uut.fiveMinutesToGo();
    if (event == "oneMinuteToGo")       uut.oneMinuteToGo();
    if (event == "fiveSecondsPassed")   uut.fiveSecondsPassed();
    if (event == "tenSecondsToGo")      uut.tenSecondsToGo();
    if (event == "showerTimerExpired")  uut.showerTimerExpired();
    if (event == "dongleIn")            uut.dongleIn();
    if (event == "dongleOut")           uut.dongleOut();
    if (event == "reset")               uut.reset();
    if (event == "plusButton")          uut.plusButton();
    if (event == "minusButton")         uut.minusButton();
    if (event == "lockoutTimerExpired") uut.lockoutTimerExpired();
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


const TestVector table[] =
{
    // Idle state
    {
        "Init (Idle) + Start -> Water On",
        { },
        { "startButton" },
        { "greenLedOn", "longBeep", "showShowerTime", "displayBright", "coldTimerStart", "valveOpen" }
    },
    {
        "Init (Idle) + reset -> stay in idle, feedback that reset occurred",
        { },
        { "reset" },
        { "rapidBeep", "greenLedOn", "valveClosed", "showShowerTime", "displayDim" }
    },
    {
        "Init (Idle) + dongle in -> Override",
        { },
        { "dongleIn" },
        { "rapidBeep", "alternateLedsFlashing", "valveOpen", "showShowerTime", "displayBright" }
    },
    {
        "Init (Idle) + events that should be ignored -> no effect",
        { },
        { "coldTimerExpired", "showerHot", "showerCold", "fiveMinutesToGo", "oneMinuteToGo",
          "fiveSecondsPassed", "tenSecondsToGo", "showerTimerExpired", "plusButton", "minusButton" },
        { }
    },

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

    // Shower running state
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
        "Shower running + 10 secs to go -> warnings",
        { "startButton", "showerHot" }, // get to test state
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
        "Shower running + then off + 10 secs to go -> warnings",
        { "startButton", "showerHot", "showerCold" }, // get to test state
        { "tenSecondsToGo" },
        { "displayFlash" }
    },
    {
        "Shower running + then off + 10 secs to go + 5 sec passed -> no beep",
        { "startButton", "showerHot", "showerCold", "tenSecondsToGo" }, // get to test state
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
        "Shower running + then off then on again, 10 secs to go -> warnings",
        { "startButton", "showerHot", "showerCold", "showerHot" }, // get to test state
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
        "Shower running + timer expired -> Lockout",
        { "startButton", "showerHot" }, // get to test state
        { "showerTimerExpired" },
        { "redLedFlashing", "valveClosed", "showLockoutTime" }
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
    /// @todo <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

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
    },
};

TEST_CASE("Table driven FSM test")
{
    for (const auto& v : table)
    {
        SECTION(v.testName)
        {
            apply(v);
        }
    }
}


/*

All events:

    "startButton",
    "coldTimerExpired",
    "showerHot",
    "showerCold",
    "fiveMinutesToGo",
    "oneMinuteToGo",
    "fiveSecondsPassed",
    "tenSecondsToGo",
    "showerTimerExpired",
    "dongleIn",
    "dongleOut",
    "reset",
    "plusButton",
    "minusButton",
    "lockoutTimerExpired",

 */
