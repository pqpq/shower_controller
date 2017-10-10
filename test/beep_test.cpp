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

TEST_CASE("longBeep turns on for 6 polls")
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
    uut.poll();
    REQUIRE(testCalls == 0);
    uut.poll();
    REQUIRE(testCalls == 0);
    uut.poll();
    REQUIRE(testCalls == 0);
    uut.poll();
    REQUIRE(testCalls == 0);
    uut.poll();
    REQUIRE(testCalls == 0);

    // Off on 7th poll
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
    REQUIRE(testCalls == 0);
    uut.poll();
    REQUIRE(testCalls == 0);

    // Off on 4th poll
    testClear();
    uut.poll();
    REQUIRE(testCalls == 1);
    REQUIRE(testValue == false);

    // silent for another
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
    REQUIRE(testCalls == 0);
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

