#include "../timer.h"
//#include "../beep.h"

#include "catch.hpp"

Timer::Time testTime;

Timer::Time getTestTime()
{
    return testTime;
}

TEST_CASE("Fresh timer has 0 time remaining")
{
    const Timer uut(getTestTime);
    REQUIRE(uut.remaining() == 0);
}

TEST_CASE("update() on fresh timer has 0 time remaining")
{
    testTime = 99;
    Timer uut(getTestTime);
    uut.update();
    REQUIRE(uut.remaining() == 0);
}

TEST_CASE("Timer started with a time has that time remaining")
{
    Timer uut(getTestTime);

    const Timer::Time t = 123;
    uut.start(t);
    REQUIRE(uut.remaining() == t);
}

TEST_CASE("Update subtracts passed time from start time")
{
    Timer uut(getTestTime);

    const Timer::Time t = 99;
    const Timer::Time delta = 13;
    testTime = 10101;

    uut.start(t);

    testTime += delta;
    uut.update();
    REQUIRE(uut.remaining() == t - delta);
}

TEST_CASE("If too much time has passed before update(), remaining returns zero")
{
    Timer uut(getTestTime);

    const Timer::Time t = 42;
    testTime = 10;

    uut.start(t);

    testTime += t * 2;

    uut.update();
    REQUIRE(uut.remaining() == 0);
}
