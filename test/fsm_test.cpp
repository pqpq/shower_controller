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

    virtual void valveClosed() { valveClosedCalls++; }
    virtual void valveOpen() { valveOpenCalls++; }

    virtual void greenLedOn() { greenLedOnCalls++; }

    virtual void showShowerTime() { showShowerTimeCalls++; }
    virtual void displayDim() { displayDimCalls++; }
    virtual void displayBright() { displayBrightCalls++; }

    virtual void shortBeep() { shortBeepCalls++; }
    virtual void longBeep() { longBeepCalls++; }

    virtual void startColdTimer() { startColdTimerCalls++; }


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

    /// @todo test all other calls == 0
}


TEST_CASE("Start causes various actions")
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

    /// @todo test all other calls == 0

}
