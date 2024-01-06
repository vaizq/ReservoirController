#include "unity.h"
#include "test_dt_timer.hpp"


int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_dt_timer);

    UNITY_END();

    return 0;
}