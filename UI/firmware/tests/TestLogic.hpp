#include <boost/test/included/unit_test.hpp>

#include "logic.h"
#include "ui.h"
#include "Helpers.hpp"

namespace utf = boost::unit_test;

uint32_t electric_seq_id = 0;
uint32_t motion_seq_id = 0;

BOOST_AUTO_TEST_CASE(electric_static_test, * utf::tolerance(0.01))
{
    logic_init();

    // dump_lcd();
    ui_set_display_mode(DM_POWER2);

    // --------------------------------------------------------------
    InsertCanMessage(BuildElectricMsg(840, 0));
    logic_update();
    // dump_lcd();
              //----------------
    BOOST_TEST("    0W    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("84.0V 100%    0A" == hd44780_get_line2());
    // --------------------------------------------------------------
    InsertCanMessage(BuildElectricMsg(840, 10));
    logic_update();
    // dump_lcd();
              //----------------
    BOOST_TEST(" 84.0W    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("84.0V 100%  1.0A" == hd44780_get_line2());
    // --------------------------------------------------------------
    InsertCanMessage(BuildElectricMsg(840, 100));
    logic_update();
              //----------------
    BOOST_TEST("  840W    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("84.0V 100% 10.0A" == hd44780_get_line2());
    // --------------------------------------------------------------
    InsertCanMessage(BuildElectricMsg(840, 1000));
    logic_update();
              //----------------
    BOOST_TEST(" 8.4kW    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("84.0V 100%  100A" == hd44780_get_line2());
    // --------------------------------------------------------------
    InsertCanMessage(BuildElectricMsg(840, 8000));
    logic_update();
              //----------------
    BOOST_TEST("  67kW    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("84.0V 100%  800A" == hd44780_get_line2());
    // --------------------------------------------------------------
    InsertCanMessage(BuildElectricMsg(640, 1200));
    logic_update();
              //----------------
    BOOST_TEST(" 7.7kW    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("64.0V   0%  120A" == hd44780_get_line2());
    // --------------------------------------------------------------
    // battery charging
    InsertCanMessage(BuildElectricMsg(600, -1));
    logic_update();
              //----------------
    BOOST_TEST("+ 6.0W    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("60.0V   0%  0.1A" == hd44780_get_line2());
    // --------------------------------------------------------------
    InsertCanMessage(BuildElectricMsg(645, -9));
    logic_update();
              //----------------
    BOOST_TEST("+58.0W    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("64.5V   2%  0.9A" == hd44780_get_line2());
    // --------------------------------------------------------------
    InsertCanMessage(BuildElectricMsg(725, -109));
    logic_update();
              //----------------
    BOOST_TEST("+ 790W    0Wh/km" == hd44780_get_line1());
              //----------------
    BOOST_TEST("72.5V  42% 10.9A" == hd44780_get_line2());
    // --------------------------------------------------------------
}

BOOST_AUTO_TEST_CASE(electric_Wh_km_c_test, * utf::tolerance(0.01))
{
    logic_init();
    ui_set_display_mode(DM_POWER2);

    // 1.830 m == 16 pulses
    // 84.0V 10A dist=13176m (13.176km), t=1h
    // ==> 840Wh/13.176=

    HAL_Tick = 13;
    int dist = 144;
    
    for (int i = 0; i < 2 * 3600; ++i) {
        InsertCanMessage(BuildElectricMsg(840, 100));
        InsertCanMessage(BuildMotionMsg(dist));
        logic_update();

        HAL_Tick += 500;
        dist += 16;
    }

    HAL_Tick += 10000;
    InsertCanMessage(BuildElectricMsg(840, 0));
    logic_update();

              //----------------
    BOOST_TEST("    0W 63.7Wh/km" == hd44780_get_line1());
}