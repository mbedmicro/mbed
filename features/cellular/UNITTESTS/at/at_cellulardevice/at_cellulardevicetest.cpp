/*
 * Copyright (c) 2018 ARM. All rights reserved.
 */
#include "CppUTest/TestHarness.h"
#include "test_at_cellulardevice.h"

TEST_GROUP(AT_CellularDevice)
{
    Test_AT_CellularDevice* unit;

    void setup()
    {
        unit = new Test_AT_CellularDevice();
    }

    void teardown()
    {
        delete unit;
    }
};

TEST(AT_CellularDevice, Create)
{
    CHECK(unit != NULL);
}

TEST(AT_CellularDevice, test_AT_CellularDevice_constructor)
{
    unit->test_AT_CellularDevice_constructor();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_get_at_handler)
{
    unit->test_AT_CellularDevice_get_at_handler();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_open_network)
{
    unit->test_AT_CellularDevice_open_network();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_open_sms)
{
    unit->test_AT_CellularDevice_open_sms();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_open_power)
{
    unit->test_AT_CellularDevice_open_power();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_open_sim)
{
    unit->test_AT_CellularDevice_open_sim();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_open_multiplexer)
{
    unit->test_AT_CellularDevice_open_multiplexer();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_open_information)
{
    unit->test_AT_CellularDevice_open_information();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_close_network)
{
    unit->test_AT_CellularDevice_close_network();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_close_sms)
{
    unit->test_AT_CellularDevice_close_sms();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_close_power)
{
    unit->test_AT_CellularDevice_close_power();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_close_sim)
{
    unit->test_AT_CellularDevice_close_sim();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_close_multiplexer)
{
    unit->test_AT_CellularDevice_close_multiplexer();
}

TEST(AT_CellularDevice, test_AT_CellularDevice_close_information)
{
    unit->test_AT_CellularDevice_close_information();
}

