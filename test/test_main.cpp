#include "unity.h"
#include "test_dt_timer.hpp"


extern "C" void app_main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_dt_timer);

    UNITY_END();
}