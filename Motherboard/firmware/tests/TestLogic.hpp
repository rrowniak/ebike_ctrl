#include <boost/test/included/unit_test.hpp>

#include <chrono>
#include <future>
#include <thread>

#include <stm32_puppet.hpp>
#include "logic.h"
#include <bike_can_protocol.h>

namespace utf = boost::unit_test;

#define ADC_RESOLUTION 4096
#define MAX_ADC_VALUE (ADC_RESOLUTION - 1)
#define VREF 3.3

#define CONV_KICK_OFF_DELAY 100

uint16_t ConvVolt2Bits(double v)
{
    v = std::min(v, VREF);
    v = std::max(v, 0.0);

    return ADC_RESOLUTION * v / VREF;
}

uint16_t CalcBattV(double v)
{
    constexpr double R1 = 100000;
    constexpr double R2 = 3300;
    return ConvVolt2Bits(v * R2 / (R1 + R2));
}

template <class T, int F>
bool GetLatestMsg(T& el)
{
    auto& v = GetCanBusBuffer();

    for (auto it = v.rbegin(); it != v.rend(); ++it) {
        if (it->data[0] == F) {
            std::memcpy(&el, &it->data[1], 
                sizeof(T));
            return true;
        }        
    }

    return false;
}

bool GetLatestEl(bcp_msg_electric& el) {
    return GetLatestMsg<bcp_msg_electric, BCP_MSG_ELECTRIC>(el);
}

bool GetLatestMotion(bcp_msg_motion& el) {
    return GetLatestMsg<bcp_msg_motion, BCP_MSG_MOTION>(el);
}

bool GetLatestBlk1(bcp_msg_sens_blk1& el) {
    return GetLatestMsg<bcp_msg_sens_blk1, BCP_MSG_SENS_BLK1>(el);
}

void ValidateAgainstUnknownMsg()
{
    for (auto& msg : GetCanBusBuffer()) {
        switch (msg.data[0]) {
        case BCP_MSG_ELECTRIC:   
            break;
        case BCP_MSG_MOTION:
            break;
        case BCP_MSG_SENS_BLK1:
            break;
        default:
            std::cout << "Received: " << (int)msg.data[0] << std::endl;
            BOOST_ERROR("Unknown message type");
            break;
        }
    }
}

void FillAdcWithDefaultVals()
{
    // current (2.5 means 0A)
    adcRawValues[0] = ConvVolt2Bits(2.5);
    
    // battery temperature, about 25C
    adcRawValues[1] = ConvVolt2Bits(1.57);
    
    // driver temperature, about 25C
    adcRawValues[2] = ConvVolt2Bits(1.57);
    
    // battery voltage, about 80V
    adcRawValues[3] = CalcBattV(80);

    // motor temperature KTY83
    adcRawValues[4] = 13; // sensor disconnected

    // motor temperature NTC, about 25C
    adcRawValues[5] = ConvVolt2Bits(2.5);
}

BOOST_AUTO_TEST_CASE(logic_basic_test, * utf::tolerance(0.01))
{
    HAL_Tick = 0;
    hdma_adc1.State = 0;
    logic_init();
    
    // prepare ADC sample
    FillAdcWithDefaultVals();

    // logic update will be waiting for conversion
    auto job = std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::milliseconds(CONV_KICK_OFF_DELAY));
        // a potential for a race condition :/
        hdma_adc1.State = HAL_DMA_STATE_READY;
        int_conv_dma(&hdma_adc1);
    });
    logic_update();
    job.get();

    BOOST_TEST(GetCanBusBuffer().size() > 1);

    // decode values (like UI module does)

    // Measure block 1
    bcp_msg_sens_blk1 blk;
    BOOST_REQUIRE(GetLatestBlk1(blk));

    auto moto_temp = convert_from_9bit(blk.moto_t);
    auto driver_temp = convert_from_9bit(blk.drv_t);
    auto batt_temp = convert_from_9bit(blk.batt_t);

    // don't test moto temp as there is no calibration done yet
    // BOOST_TEST(moto_temp == 25);
    BOOST_TEST(driver_temp == 25);
    BOOST_TEST(batt_temp == 25);

    // Electric parameters
    bcp_msg_electric el; 
    BOOST_REQUIRE(GetLatestEl(el));

    float b_v = el.voltage / 10.0;
    std::cout << "Voltage: " << b_v;
    BOOST_TEST(b_v == 80.0);

    float a =  convert_from_11bit(el.current) / 10;
    BOOST_TEST(a == 0.0);

    ValidateAgainstUnknownMsg();
}

BOOST_AUTO_TEST_CASE(logic_moto_temp_cal, * utf::tolerance(0.01))
{
    HAL_Tick = 0;
    hdma_adc1.State = 0;
    logic_init();
    
    // prepare ADC sample
    FillAdcWithDefaultVals();

    for (int i = 0; i < 11; ++i) {
        // logic update will be waiting for conversion
        auto job = std::async(std::launch::async, []() {
            std::this_thread::sleep_for(std::chrono::milliseconds(CONV_KICK_OFF_DELAY));
            // a potential forBETA race condition :/
            hdma_adc1.State = HAL_DMA_STATE_READY;
            int_conv_dma(&hdma_adc1);
        });
        logic_update();
        job.get();

        // move ahead by 500ms
        HAL_Tick += 500;
    }

    // Measure block 1
    bcp_msg_sens_blk1 blk;
    BOOST_REQUIRE(GetLatestBlk1(blk));

    auto moto_temp = convert_from_9bit(blk.moto_t);
    BOOST_TEST(moto_temp == 25);
}