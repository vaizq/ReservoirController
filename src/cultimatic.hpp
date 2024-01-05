#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <chrono>


using namespace std::chrono_literals;


namespace Cultimatic 
{
    template <typename Rep, typename Period>
    void delay(std::chrono::duration<Rep, Period> duration)
    {
        vTaskDelay(pdMS_TO_TICKS(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()));
    }
}