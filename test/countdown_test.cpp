#include "../countdown.h"
#include "../countdown.h"
//#include "../timer.h"

#include "catch.hpp"


size_t fiveMinutesCalls = 0;
void fiveMinutesCallback()
{
    fiveMinutesCalls++;
}

size_t oneMinuteCalls = 0;
void oneMinuteCallback()
{
    oneMinuteCalls++;
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
    fiveMinutesCalls = 0;
    oneMinuteCalls = 0;
    fiveSecondCalls = 0;
    oneSecondCalls = 0;
}

TEST_CASE("fiveMinutes triggers callback once each time we pass 5 minus")
{
    Countdown uut(fiveMinutesCallback, oneMinuteCallback, fiveSecondCallback, oneSecondCallback);

    clearAll();
    uut.secondsToGo(800);

    REQUIRE(fiveMinutesCalls == 0);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(700);

    REQUIRE(fiveMinutesCalls == 0);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    // late - should have been at 600, but should still trigger callback
    uut.secondsToGo(500);
    REQUIRE(fiveMinutesCalls == 1);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    clearAll();
    uut.secondsToGo(301);
    REQUIRE(fiveMinutesCalls == 0);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);

    uut.secondsToGo(300);
    REQUIRE(fiveMinutesCalls == 1);
    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(fiveSecondCalls == 0);
    REQUIRE(oneSecondCalls == 0);
}

TEST_CASE("oneMinute triggers callback only when time to go passes 60 seconds")
{
    Countdown uut(fiveMinutesCallback, oneMinuteCallback, ignoreCallback, ignoreCallback);

    clearAll();
    uut.secondsToGo(301);
    REQUIRE(fiveMinutesCalls == 0);
    REQUIRE(oneMinuteCalls == 0);

    uut.secondsToGo(300);
    REQUIRE(fiveMinutesCalls == 1);
    REQUIRE(oneMinuteCalls == 0);

    clearAll();
    uut.secondsToGo(61);
    REQUIRE(fiveMinutesCalls == 0);
    REQUIRE(oneMinuteCalls == 0);

    uut.secondsToGo(60);
    REQUIRE(fiveMinutesCalls == 0);
    REQUIRE(oneMinuteCalls == 1);

    clearAll();
    uut.secondsToGo(10);
    REQUIRE(fiveMinutesCalls == 0);
    REQUIRE(oneMinuteCalls == 0);

    uut.secondsToGo(1);
    REQUIRE(fiveMinutesCalls == 0);
    REQUIRE(oneMinuteCalls == 0);
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

