#include "../actions.h"
#include "../actions.h"
#include "../controller.h"
#include "../controller.h"

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_COLOUR_NONE
#include "catch.hpp"

#include <vector>
#include <string>

using namespace std;


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
    virtual void countDownTime()    { _calls.push_back("countDownTime"); }
    virtual void countDownLockout() { _calls.push_back("countDownLockout"); }
    virtual void displayDim()       { _calls.push_back("displayDim"); }
    virtual void displayBright()    { _calls.push_back("displayBright"); }

    virtual void shortBeep()        { _calls.push_back("shortBeep"); }
    virtual void longBeep()         { _calls.push_back("longBeep"); }
    virtual void rapidBeep()        { _calls.push_back("rapidBeep"); }

    virtual void coldTimerStart()   { _calls.push_back("coldTimerStart"); }
    virtual void coldTimerStop()    { _calls.push_back("coldTimerStop"); }
    virtual void showerTimerStart() { _calls.push_back("showerTimerStart"); }

    void reset()
    {
        _calls.clear();
    }

    size_t n(const char* fn) const
    {
        return count(_calls.begin(), _calls.end(), fn);
    }

    size_t totalCalls() const { return _calls.size(); }

    const vector<string>& calls() const { return _calls; }

private:
    vector<string> _calls;
};



TEST_CASE("Can create an instance")
{
    TestActions ta;
    const Controller uut(ta);

    REQUIRE(ta.n("greenLedOn") == 1);
    REQUIRE(ta.n("valveClosed") == 1);
    REQUIRE(ta.n("showShowerTime") == 1);
    REQUIRE(ta.n("displayDim") == 1);
    REQUIRE(ta.n("shortBeep") == 1);
    CHECK(ta.totalCalls() == 5);
}

TEST_CASE("Start causes no actions from water on")
{
    TestActions ta;
    Controller uut(ta);

    // get to 'water on' state
    uut.start();
    ta.reset();

    uut.start();
    CHECK(ta.totalCalls() == 0);
}

TEST_CASE("Cold timer expired in water on -> idle")
{
    TestActions ta;
    Controller uut(ta);

    // get to 'water on' state
    uut.start();
    ta.reset();

    uut.coldTimerExpired();

    REQUIRE(ta.n("greenLedOn") == 1);
    REQUIRE(ta.n("valveClosed") == 1);
    REQUIRE(ta.n("showShowerTime") == 1);
    REQUIRE(ta.n("displayDim") == 1);
    REQUIRE(ta.n("shortBeep") == 1);
    CHECK(ta.totalCalls() == 5);
}

TEST_CASE("Shower hot in water on -> shower running")
{
    TestActions ta;
    Controller uut(ta);

    // get to 'water on' state
    uut.start();
    ta.reset();

    uut.showerHot();

    REQUIRE(ta.n("coldTimerStop") == 1);
    REQUIRE(ta.n("showerTimerStart") == 1);
    REQUIRE(ta.n("greenLedFlashing") == 1);
    REQUIRE(ta.n("longBeep") == 1);
    CHECK(ta.totalCalls() == 4);
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
    if (event == "start")               uut.start();
    if (event == "coldTimerExpired")    uut.coldTimerExpired();
    if (event == "showerHot")           uut.showerHot();
    if (event == "fiveMinutesToGo")     uut.fiveMinutesToGo();
    if (event == "oneMinuteToGo")       uut.oneMinuteToGo();
    if (event == "fiveSecondsPassed")   uut.fiveSecondsPassed();
    if (event == "tenSecondsToGo")      uut.tenSecondsToGo();
    if (event == "showerTimerExpired")  uut.showerTimerExpired();
    if (event == "dongleIn")            uut.dongleIn();
    if (event == "dongleOut")           uut.dongleOut();
    if (event == "reset")               uut.reset();
}

void apply(const TestVector& v)
{
    TestActions ta;
    Controller uut(ta);

    for (const auto& e : v.eventsToGetToTestState)
    {
        apply(uut, e);
    }
    ta.reset();

    for (const auto& e : v.eventsToApply)
    {
        apply(uut, e);
    }

    REQUIRE(ta.calls() == v.expectedActions);
}


const TestVector table[] =
{
    {
        "Init (Idle) + Start -> Water On",
        { },
        { "start" },
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
        "Init (Idle) + dongle out -> Idle",
        { },
        { "dongleOut" },
        { "rapidBeep", "greenLedOn", "valveClosed", "showShowerTime", "displayDim" }
    },
    {
        "Init (Idle) + events that should be ignored -> no effect",
        { },
        { "coldTimerExpired", "showerHot", "fiveMinutesToGo", "oneMinuteToGo", "fiveSecondsPassed", "tenSecondsToGo", "showerTimerExpired", "plusButton", "minusButton" },
        { }
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
