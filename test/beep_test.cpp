#include "../beep.h"
#include "../beep.h"

#include "catch.hpp"

bool testValue = 0;
size_t testCalls = 0;

void testCallback(bool x)
{
    testValue = x;
    testCalls++;
}

void testClearTo(bool x)
{
    testValue = x;
    testCalls = 0;
}

void testClear()
{
    testClearTo(false);
}

TEST_CASE("Constructor takes callback function and calls it with zero")
{
    testClearTo(true);
    const Beep uut(testCallback);
    REQUIRE(testValue == false);
}

TEST_CASE("Calling poll on empty Beep does nothing")
{
    Beep uut(testCallback);
    testClear();
    uut.poll();
    REQUIRE(testCalls == 0);
}

TEST_CASE("shortBeep turns on for 3 polls")
{
    Beep uut(testCallback);

    // doesn't call back immediately
    testClear();
    uut.shortBeep();
    REQUIRE(testCalls == 0);

    // On on 1st poll
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);

    testClear();
    uut.poll();
    REQUIRE(testCalls == 0);
    uut.poll();
    REQUIRE(testCalls == 0);

    // Off on 4th poll
    testClear();
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == false);
}

TEST_CASE("longBeep turns on for 20 polls")
{
    Beep uut(testCallback);

    // Doesn't call back immediately
    testClear();
    uut.longBeep();
    REQUIRE(testCalls == 0);

    // On on 1st poll
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);

    testClear();

    for (size_t i = 0; i < 19; ++i)
    {
        uut.poll();
        REQUIRE(testCalls == 0);
    }

    // Off on 20th poll
    testClear();
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == false);
}

TEST_CASE("rapidBeeps beeps 3 times")
{
    Beep uut(testCallback);

    // Doesn't call back immediately
    testClear();
    uut.rapidBeeps();
    REQUIRE(testCalls == 0);

    // On on 1st poll
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);

    // then off
    testClearTo(true);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == false);

    // then on
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);

    // then off
    testClearTo(true);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == false);

    // then on
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);

    // then off
    testClearTo(true);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == false);
}

TEST_CASE("two short beeps are separated")
{
    Beep uut(testCallback);

    // doesn't call back immediately
    testClear();

    uut.shortBeep();
    uut.shortBeep();

    REQUIRE(testCalls == 0);

    // On on 1st poll
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);

    testClear();
    uut.poll();
    uut.poll();
    REQUIRE(testCalls == 0);

    // Off on 4th poll
    testClear();
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == false);

    // silent
    testClear();
    uut.poll();
    REQUIRE(testCalls == 0);

    // on for second beep
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);

    testClear();
    uut.poll();
    uut.poll();
    REQUIRE(testCalls == 0);

    // off again
    testClear();
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == false);

    // silent
    testClear();
    uut.poll();
    REQUIRE(testCalls == 0);
}

TEST_CASE("Can add beeps indefinitely")
{
    constexpr size_t MAX(10);
    Beep uut(testCallback, MAX);

    uut.rapidBeeps();

    uut.poll();
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testValue == false);

    // Add another before last one has fully expired
    uut.rapidBeeps();

    testClear();
    uut.poll();
    REQUIRE(testCalls == 0);

    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 2);
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testCalls == 3);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 4);
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testCalls == 5);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 6);
    REQUIRE(testValue == false);

    // time passes
    for (size_t i = 0; i < 10; ++i)
    {
        uut.poll();
    }

    // and another
    uut.rapidBeeps();

    testClear();
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 2);
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testCalls == 3);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 4);
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testCalls == 5);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 6);
    REQUIRE(testValue == false);
    uut.poll();

    // and another
    uut.rapidBeeps();

    testClear();
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 2);
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testCalls == 3);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 4);
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testCalls == 5);
    REQUIRE(testValue == true);
    uut.poll();
    REQUIRE(testCalls == 6);
    REQUIRE(testValue == false);
}

TEST_CASE("With queue full, another beep is ignored")
{
    constexpr size_t MAX(5);
    Beep uut(testCallback, MAX);

    uut.shortBeep();
    uut.shortBeep();
    uut.longBeep();

    // first short beep
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);
    uut.poll();
    uut.poll();
    REQUIRE(testCalls == 1);
    uut.poll();
    REQUIRE(testCalls == 2);
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testCalls == 2);

    // second short beep
    testClearTo(false);
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == true);
    uut.poll();
    uut.poll();
    REQUIRE(testCalls == 1);
    uut.poll();
    REQUIRE(testCalls == 2);
    REQUIRE(testValue == false);
    uut.poll();
    REQUIRE(testCalls == 2);

    // silent
    testClear();
    uut.poll();
    uut.poll();
    uut.poll();
    uut.poll();
    uut.poll();
    uut.poll();
    REQUIRE(testCalls == 0);
}
