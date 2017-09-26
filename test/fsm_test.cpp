#include "../actions.h"
#include "../actions.h"
#include "../controller.h"
#include "../controller.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

class TestActions : public Actions
{
public:
    TestActions() { reset(); }

    virtual void valveClosed()      { totalCalls++; valveClosedCalls++; }
    virtual void valveOpen()        { totalCalls++; valveOpenCalls++; }

    virtual void greenLedOn()       { totalCalls++; greenLedOnCalls++; }

    virtual void showShowerTime()   { totalCalls++; showShowerTimeCalls++; }
    virtual void displayDim()       { totalCalls++; displayDimCalls++; }
    virtual void displayBright()    { totalCalls++; displayBrightCalls++; }

    virtual void shortBeep()        { totalCalls++; shortBeepCalls++; }
    virtual void longBeep()         { totalCalls++; longBeepCalls++; }

    virtual void startColdTimer()   { totalCalls++; startColdTimerCalls++; }


    void reset()
    {
        valveClosedCalls = 0;
        valveOpenCalls = 0;

        greenLedOnCalls = 0;

        showShowerTimeCalls = 0;
        displayDimCalls = 0;
        displayBrightCalls = 0;

        shortBeepCalls = 0;
        longBeepCalls = 0;

        startColdTimerCalls = 0;

        totalCalls = 0;
    }

    size_t valveClosedCalls;
    size_t valveOpenCalls;

    size_t greenLedOnCalls;

    size_t showShowerTimeCalls;
    size_t displayDimCalls;
    size_t displayBrightCalls;

    size_t shortBeepCalls;
    size_t longBeepCalls;

    size_t startColdTimerCalls;

    size_t totalCalls;
};



TEST_CASE("Can create an instance")
{
    TestActions ta;
    const Controller uut(ta);

    REQUIRE(ta.greenLedOnCalls == 1);
    REQUIRE(ta.valveClosedCalls == 1);
    REQUIRE(ta.showShowerTimeCalls == 1);
    REQUIRE(ta.displayDimCalls == 1);
    REQUIRE(ta.shortBeepCalls == 1);
    CHECK(ta.totalCalls == 5);
}


TEST_CASE("Start causes various actions from idle")
{
    TestActions ta;
    Controller uut(ta);
    ta.reset();

    uut.start();

    REQUIRE(ta.greenLedOnCalls == 1);
    REQUIRE(ta.valveOpenCalls == 1);
    REQUIRE(ta.showShowerTimeCalls == 1);
    REQUIRE(ta.displayBrightCalls == 1);
    REQUIRE(ta.longBeepCalls == 1);
    REQUIRE(ta.startColdTimerCalls == 1);
    CHECK(ta.totalCalls == 6);
}

TEST_CASE("Start causes no actions from water on")
{
    TestActions ta;
    Controller uut(ta);

    // get to 'water on' state
    uut.start();
    ta.reset();

    // test
    uut.start();
    CHECK(ta.totalCalls == 0);
}
