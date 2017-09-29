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
    virtual void greenLedFlashing() { _calls.push_back("greenLedFlashing"); }

    virtual void showShowerTime()   { _calls.push_back("showShowerTime"); }
    virtual void displayDim()       { _calls.push_back("displayDim"); }
    virtual void displayBright()    { _calls.push_back("displayBright"); }

    virtual void shortBeep()        { _calls.push_back("shortBeep"); }
    virtual void longBeep()         { _calls.push_back("longBeep"); }

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

TEST_CASE("Start causes various actions from idle")
{
    TestActions ta;
    Controller uut(ta);
    ta.reset();

    uut.start();

    REQUIRE(ta.n("greenLedOn") == 1);
    REQUIRE(ta.n("valveOpen") == 1);
    REQUIRE(ta.n("showShowerTime") == 1);
    REQUIRE(ta.n("displayBright") == 1);
    REQUIRE(ta.n("longBeep") == 1);
    REQUIRE(ta.n("coldTimerStart") == 1);
    CHECK(ta.totalCalls() == 6);
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

TEST_CASE("Cold timer expired in idle -> no actions")
{
    TestActions ta;
    Controller uut(ta);
    ta.reset();

    uut.coldTimerExpired();

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
    std::string eventToApply;
    std::vector<std::string> expectedActions;
};

void apply(Controller& uut, std::string event)
{
    if (event == "start")               uut.start();
    if (event == "coldTimerExpired")    uut.coldTimerExpired();
    if (event == "showerHot")           uut.showerHot();
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

    apply(uut, v.eventToApply);

    REQUIRE(ta.calls() == v.expectedActions);
}


const TestVector table[] =
{
    {
        "Init (Idle) + Start -> Water On",
        { },
        "start",
        { "greenLedOn", "longBeep", "showShowerTime", "displayBright", "coldTimerStart", "valveOpen" }
    },
    {
        "Init (Idle) + Cold timer expired -> no effect",
        { },
        "coldTimerExpired",
        { }
    },
    {
        "Init (Idle) + Shower hot -> no effect",
        { },
        "showerHot",
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
