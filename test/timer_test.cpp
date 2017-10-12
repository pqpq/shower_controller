#include "../timer.h"
//#include "../beep.h"

#include "catch.hpp"

Timer::Milliseconds testTime;

Timer::Milliseconds getTestTime()
{
    return testTime;
}

void dummyCallback() {}

size_t calls = 0;
void testCallback()
{
    calls++;
}

size_t calls1 = 0;
void testCallback1()
{
    calls1++;
}

size_t calls2 = 0;
void testCallback2()
{
    calls2++;
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
    uut.start(t, dummyCallback);
    REQUIRE(uut.remaining() == t);
}

TEST_CASE("Update subtracts passed time from start time")
{
    Timer uut(getTestTime);

    const Timer::Milliseconds t = 99;
    const Timer::Milliseconds delta = 13;
    testTime = 10101;

    uut.start(t, dummyCallback);

    testTime += delta;
    uut.update();
    REQUIRE(uut.remaining() == t - delta);
}

TEST_CASE("If too much time has passed before update(), remaining returns zero")
{
    Timer uut(getTestTime);

    const Timer::Milliseconds t = 42;
    testTime = 10;

    uut.start(t, testCallback);

    testTime += t * 2;

    uut.update();
    REQUIRE(uut.remaining() == 0);
}

TEST_CASE("finished callback is called when duration gets to zero")
{
    Timer uut(getTestTime);

    const Timer::Milliseconds t = 8;
    testTime = 100;

    calls = 0;
    uut.start(t, testCallback);

    testTime += t - 1;
    uut.update();
    REQUIRE(calls == 0);

    testTime++;
    uut.update();
    REQUIRE(calls == 1);
}

TEST_CASE("Given callback is called every time given period has elapsed")
{
    Timer uut(getTestTime);

    const Timer::Milliseconds period = 13;
    uut.every(period, testCallback);

    testTime = 1;
    calls = 0;

    uut.start(999, dummyCallback);

    REQUIRE(calls == 0);

    testTime += period - 1;
    uut.update();
    REQUIRE(calls == 0);

    testTime += 1;
    uut.update();
    REQUIRE(calls == 1);

    testTime += period * 2 + 6; // lots of time has passed - we're late calling update()
    uut.update();
    REQUIRE(calls == 2);

    testTime += 6;
    uut.update();
    REQUIRE(calls == 2);

    testTime += 1;
    uut.update();
    REQUIRE(calls == 3);
}

TEST_CASE("Multiple callbacks are called correctly")
{
    Timer uut(getTestTime);

    const Timer::Milliseconds period1 = 3;
    uut.every(period1, testCallback1);

    const Timer::Milliseconds period2 = 4;
    uut.every(period2, testCallback2);

    testTime = 1;
    calls1 = 0;
    calls2 = 0;

    uut.start(999, dummyCallback);

    testTime++;
    testTime++;
    uut.update();

    REQUIRE(calls1 == 0);
    REQUIRE(calls2 == 0);

    testTime++;
    uut.update();

    REQUIRE(calls1 == 1);
    REQUIRE(calls2 == 0);

    testTime++;
    uut.update();

    REQUIRE(calls1 == 1);
    REQUIRE(calls2 == 1);

    testTime++;
    uut.update();

    REQUIRE(calls1 == 1);
    REQUIRE(calls2 == 1);

    testTime++;
    uut.update();

    REQUIRE(calls1 == 2);
    REQUIRE(calls2 == 1);

    testTime++;
    uut.update();

    REQUIRE(calls1 == 2);
    REQUIRE(calls2 == 1);

    testTime++;
    uut.update();

    REQUIRE(calls1 == 2);
    REQUIRE(calls2 == 2);

    testTime++;
    uut.update();

    REQUIRE(calls1 == 3);
    REQUIRE(calls2 == 2);
}
