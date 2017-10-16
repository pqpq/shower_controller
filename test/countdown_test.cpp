#include "../countdown.h"
#include "../countdown.h"
//#include "../timer.h"

#include "catch.hpp"


size_t oneMinuteCalls = 0;
void oneMinuteCallback()
{
    oneMinuteCalls++;
}

size_t lastMinuteCalls = 0;
void lastMinuteCallback()
{
    lastMinuteCalls++;
}

size_t fiveSecondCalls = 0;
void fiveSecondCallback()
{
    fiveSecondCalls++;
}

size_t oneSecondCalls = 0;
void oneSecondCallback()
{
    oneSecondCalls++;
}

void ignoreCallback() {}

void clearAll()
{
    oneMinuteCalls = 0;
    lastMinuteCalls = 0;
    fiveSecondCalls = 0;
    oneSecondCalls = 0;
}

TEST_CASE("One minute callback triggered for each minute from 5 mins to 1 min")
{
    Countdown uut(oneMinuteCallback, lastMinuteCallback, fiveSecondCallback, oneSecondCallback);

    clearAll();
    uut.secondsToGo(800);

    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(700);

    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    // late - should have been at 300, but should still trigger callback
    uut.secondsToGo(290);
    REQUIRE(oneMinuteCalls == 1);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    clearAll();
    uut.secondsToGo(241);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(240);
    REQUIRE(oneMinuteCalls == 1);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    clearAll();
    uut.secondsToGo(190);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(170); // a bit late
    REQUIRE(oneMinuteCalls == 1);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    clearAll();
    uut.secondsToGo(121);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(120);
    REQUIRE(oneMinuteCalls == 1);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    clearAll();
    uut.secondsToGo(61);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(60);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 1);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);
}

TEST_CASE("last minute callback triggered only when time to go passes 60 seconds")
{
    Countdown uut(oneMinuteCallback, lastMinuteCallback, ignoreCallback, ignoreCallback);

    clearAll();
    uut.secondsToGo(301);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);

    uut.secondsToGo(300);
    REQUIRE(oneMinuteCalls == 1);
    REQUIRE(lastMinuteCalls == 0);

    uut.secondsToGo(62);
    clearAll();
    uut.secondsToGo(61);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);

    uut.secondsToGo(60);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 1);

    clearAll();
    uut.secondsToGo(10);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);

    uut.secondsToGo(1);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(lastMinuteCalls == 0);
}

TEST_CASE("fiveSeconds only triggers callbacks in last 55 - 11 seconds")
{
    Countdown uut(ignoreCallback, ignoreCallback, fiveSecondCallback, ignoreCallback);


    fiveSecondCalls = 0;
    uut.secondsToGo(100);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(61);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(60);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(56);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(55);
    REQUIRE(fiveSecondCalls == 1);

    uut.secondsToGo(42);
    fiveSecondCalls = 0;
    uut.secondsToGo(41);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(40);
    REQUIRE(fiveSecondCalls == 1);

    uut.secondsToGo(12);
    fiveSecondCalls = 0;
    uut.secondsToGo(11);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(10);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(9);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(6);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(5);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(4);
    REQUIRE(fiveSecondCalls == 0);

    uut.secondsToGo(0);
    REQUIRE(fiveSecondCalls == 0);
}

TEST_CASE("oneSecond only triggers callbacks in last 10 seconds")
{
    Countdown uut(ignoreCallback, ignoreCallback, ignoreCallback, oneSecondCallback);

    oneSecondCalls = 0;
    uut.secondsToGo(60);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(59);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(58);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(12);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(11);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(10);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(9);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(8);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(7);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(6);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(5);
    REQUIRE(oneSecondCalls == 1);

    // multiple calls in same second don't trigger
    oneSecondCalls = 0;
    uut.secondsToGo(5);
    REQUIRE(oneSecondCalls == 0);

    oneSecondCalls = 0;
    uut.secondsToGo(4);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(3);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(2);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(1);
    REQUIRE(oneSecondCalls == 1);

    oneSecondCalls = 0;
    uut.secondsToGo(0);
    REQUIRE(oneSecondCalls == 1);
}

