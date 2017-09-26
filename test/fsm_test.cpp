#include "../controller.h"
#include "../controller.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

class TestActions : public Actions
{
public:
    TestActions() { reset(); }

    virtual void greenLedOn() { greenLedOnCalls++; }
    virtual void valveClosed() { valveClosedCalls++; }
    virtual void showShowerTime() { showShowerTimeCalls++; }
    virtual void displayDim() { displayDimCalls++; }
    virtual void shortBeep() { shortBeepCalls++; }

    void reset()
    {
        greenLedOnCalls = 0;
        valveClosedCalls = 0;
        showShowerTimeCalls = 0;
        displayDimCalls = 0;
        shortBeepCalls = 0;
    }

    size_t greenLedOnCalls;
    size_t valveClosedCalls;
    size_t showShowerTimeCalls;
    size_t displayDimCalls;
    size_t shortBeepCalls;
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
