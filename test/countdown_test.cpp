#include "../countdown.h"
//#include "../timer.h"

#include "catch.hpp"

Countdown::Seconds testTimeToGo = 0;
Countdown::Seconds testGetTimeToGo()
{
    return testTimeToGo;
}

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


TEST_CASE("fiveMinutes triggers callback once when time to go passes 300 seconds")
{
    Countdown uut(testGetTimeToGo, fiveMinutesCallback, ignoreCallback, ignoreCallback, ignoreCallback);

    testTimeToGo = 1000;
    uut.start();

    fiveMinutesCalls = 0;
    uut.fiveMinutes();
    REQUIRE(fiveMinutesCalls == 0);

    testTimeToGo = 300;
    uut.fiveMinutes();
    REQUIRE(fiveMinutesCalls == 0);

    testTimeToGo = 299;
    uut.fiveMinutes();
    REQUIRE(fiveMinutesCalls == 1);

    testTimeToGo = 10;
    fiveMinutesCalls = 0;
    uut.fiveMinutes();
    REQUIRE(fiveMinutesCalls == 0);
}

TEST_CASE("oneMinute triggers callback once when time to go passes 60 seconds")
{
    Countdown uut(testGetTimeToGo, ignoreCallback, oneMinuteCallback, ignoreCallback, ignoreCallback);

    testTimeToGo = 100;
    uut.start();

    oneMinuteCalls = 0;
    uut.oneMinute();
    REQUIRE(oneMinuteCalls == 0);

    testTimeToGo = 60;
    uut.oneMinute();
    REQUIRE(oneMinuteCalls == 0);

    testTimeToGo = 59;
    uut.oneMinute();
    REQUIRE(oneMinuteCalls == 1);

    testTimeToGo = 10;
    oneMinuteCalls = 0;
    uut.oneMinute();
    REQUIRE(oneMinuteCalls == 0);
}

TEST_CASE("oneMinute and fiveMinutes called together triggers 5 minutes if time passes 300")
{
    Countdown uut(testGetTimeToGo, fiveMinutesCallback, oneMinuteCallback, ignoreCallback, ignoreCallback);

    testTimeToGo = 1000;
    uut.start();

    oneMinuteCalls = 0;
    fiveMinutesCalls = 0;

    testTimeToGo = 299;
    uut.oneMinute();
    uut.fiveMinutes();

    REQUIRE(oneMinuteCalls == 0);
    REQUIRE(fiveMinutesCalls == 1);
}

TEST_CASE("fiveSeconds only triggers callbacks in last 60 seconds")
{
    Countdown uut(testGetTimeToGo, ignoreCallback, ignoreCallback, fiveSecondCallback, ignoreCallback);

    testTimeToGo = 100;
    uut.start();

    fiveSecondCalls = 0;
    uut.fiveSeconds();
    REQUIRE(fiveSecondCalls == 0);

    testTimeToGo = 60;
    uut.fiveSeconds();
    REQUIRE(fiveSecondCalls == 0);

    testTimeToGo = 59;
    uut.fiveSeconds();
    REQUIRE(fiveSecondCalls == 1);

    testTimeToGo = 10;
    fiveSecondCalls = 0;
    uut.fiveSeconds();
    REQUIRE(fiveSecondCalls == 1);
}

TEST_CASE("oneSecond only triggers callbacks in last 10 seconds")
{
    Countdown uut(testGetTimeToGo, ignoreCallback, ignoreCallback, fiveSecondCallback, oneSecondCallback);

    testTimeToGo = 60;
    uut.start();

    oneSecondCalls = 0;
    uut.oneSecond();
    REQUIRE(oneSecondCalls == 0);

    testTimeToGo = 10;
    oneSecondCalls = 0;
    uut.oneSecond();
    REQUIRE(oneSecondCalls == 0);

    testTimeToGo = 9;
    oneSecondCalls = 0;
    uut.oneSecond();
    REQUIRE(oneSecondCalls == 1);

    testTimeToGo = 4;
    oneSecondCalls = 0;
    uut.oneSecond();
    REQUIRE(oneSecondCalls == 1);
}
