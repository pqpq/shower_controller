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
    virtual void greenLedFlashing() { totalCalls++; greenLedFlashingCalls++; }

    virtual void showShowerTime()   { totalCalls++; showShowerTimeCalls++; }
    virtual void displayDim()       { totalCalls++; displayDimCalls++; }
    virtual void displayBright()    { totalCalls++; displayBrightCalls++; }

    virtual void shortBeep()        { totalCalls++; shortBeepCalls++; }
    virtual void longBeep()         { totalCalls++; longBeepCalls++; }

    virtual void startColdTimer()   { totalCalls++; startColdTimerCalls++; }
    virtual void stopColdTimer()    { totalCalls++; stopColdTimerCalls++; }
    virtual void startShowerTimer() { totalCalls++; startShowerTimerCalls++; }

    void reset()
    {
        valveClosedCalls = 0;
        valveOpenCalls = 0;

        greenLedOnCalls = 0;
        greenLedFlashingCalls = 0;

        showShowerTimeCalls = 0;
        displayDimCalls = 0;
        displayBrightCalls = 0;

        shortBeepCalls = 0;
        longBeepCalls = 0;

        startColdTimerCalls = 0;
        stopColdTimerCalls = 0;
        startShowerTimerCalls = 0;

        totalCalls = 0;
    }

    size_t valveClosedCalls;
    size_t valveOpenCalls;

    size_t greenLedOnCalls;
    size_t greenLedFlashingCalls;

    size_t showShowerTimeCalls;
    size_t displayDimCalls;
    size_t displayBrightCalls;

    size_t shortBeepCalls;
    size_t longBeepCalls;

    size_t startColdTimerCalls;
    size_t stopColdTimerCalls;
    size_t startShowerTimerCalls;

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

    uut.start();
    CHECK(ta.totalCalls == 0);
}


TEST_CASE("Cold timer expired in idle -> no actions")
{
    TestActions ta;
    Controller uut(ta);
    ta.reset();

    uut.coldTimerExpired();

    CHECK(ta.totalCalls == 0);
}

TEST_CASE("Cold timer expired in water on -> idle")
{
    TestActions ta;
    Controller uut(ta);

    // get to 'water on' state
    uut.start();
    ta.reset();

    uut.coldTimerExpired();

    REQUIRE(ta.greenLedOnCalls == 1);
    REQUIRE(ta.valveClosedCalls == 1);
    REQUIRE(ta.showShowerTimeCalls == 1);
    REQUIRE(ta.displayDimCalls == 1);
    REQUIRE(ta.shortBeepCalls == 1);
    CHECK(ta.totalCalls == 5);
}


TEST_CASE("Shower hot in water on -> shower running")
{
    TestActions ta;
    Controller uut(ta);

    // get to 'water on' state
    uut.start();
    ta.reset();

    uut.showerHot();

    REQUIRE(ta.stopColdTimerCalls == 1);
    REQUIRE(ta.startShowerTimerCalls == 1);
    REQUIRE(ta.greenLedFlashingCalls == 1);
    REQUIRE(ta.longBeepCalls == 1);
    CHECK(ta.totalCalls == 4);
}
