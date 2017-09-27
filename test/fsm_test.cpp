#include "../actions.h"
#include "../actions.h"
#include "../controller.h"
#include "../controller.h"

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_COLOUR_NONE
#include "catch.hpp"

#include <vector>
#include <string>

class TestActions : public Actions
{
public:
    TestActions() { reset(); }

    virtual void valveClosed()      { calls.push_back("valveClosed"); }
    virtual void valveOpen()        { calls.push_back("valveOpen"); }

    virtual void greenLedOn()       { calls.push_back("greenLedOn"); }
    virtual void greenLedFlashing() { calls.push_back("greenLedFlashing"); }

    virtual void showShowerTime()   { calls.push_back("showShowerTime"); }
    virtual void displayDim()       { calls.push_back("displayDim"); }
    virtual void displayBright()    { calls.push_back("displayBright"); }

    virtual void shortBeep()        { calls.push_back("shortBeep"); }
    virtual void longBeep()         { calls.push_back("longBeep"); }

    virtual void startColdTimer()   { calls.push_back("startColdTimer"); }
    virtual void stopColdTimer()    { calls.push_back("stopColdTimer"); }
    virtual void startShowerTimer() { calls.push_back("startShowerTimer"); }

    void reset()
    {
        calls.clear();
    }

    size_t n(const char* fn) const
    {
        return std::count(calls.begin(), calls.end(), fn);
    }

    size_t totalCalls() const { return calls.size(); }

private:
    std::vector<std::string> calls;
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
    REQUIRE(ta.n("startColdTimer") == 1);
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

    REQUIRE(ta.n("stopColdTimer") == 1);
    REQUIRE(ta.n("startShowerTimer") == 1);
    REQUIRE(ta.n("greenLedFlashing") == 1);
    REQUIRE(ta.n("longBeep") == 1);
    CHECK(ta.totalCalls() == 4);
}
