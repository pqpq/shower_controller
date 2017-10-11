#include "../timer.h"
//#include "../beep.h"

#include "catch.hpp"

Timer::Milliseconds testTime;

Timer::Milliseconds getTestTime()
{
    return testTime;
}

size_t calls = 0;
void testCallback()
{
    calls++;
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

    const Timer::Milliseconds t = 123;
    uut.start(t);
    REQUIRE(uut.remaining() == t);
}

TEST_CASE("Update subtracts passed time from start time")
{
    Timer uut(getTestTime);

    const Timer::Milliseconds t = 99;
    const Timer::Milliseconds delta = 13;
    testTime = 10101;

    uut.start(t);

    testTime += delta;
    uut.update();
    REQUIRE(uut.remaining() == t - delta);
}

TEST_CASE("If too much time has passed before update(), remaining returns zero")
{
    Timer uut(getTestTime);

    const Timer::Milliseconds t = 42;
    testTime = 10;

    uut.start(t);

    testTime += t * 2;

    uut.update();
    REQUIRE(uut.remaining() == 0);
}

TEST_CASE("Given callback is called every time given period has elapsed")
{
    Timer uut(getTestTime);

    const Timer::Milliseconds period = 13;
    uut.every(period, testCallback);

    testTime = 1;
    calls = 0;

    uut.start(999);

    REQUIRE(calls == 0);

    testTime += period - 1;
    uut.update();
    REQUIRE(calls == 0);

    testTime += 1;
    uut.update();
    REQUIRE(calls == 1);

    testTime += period + 6; // lots of time has passed - we're late calling update()
    uut.update();
    REQUIRE(calls == 2);

    /// @todo something is going wrong here
    testTime += 6;
    uut.update();
    REQUIRE(calls == 2);

    testTime += 1;
    uut.update();
    REQUIRE(calls == 3);

}
