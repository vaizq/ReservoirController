//
// Created by vaige on 5.2.2024.
//

#ifndef RESERVOIRCONTROLLER_DIGITALSENSOR_H
#define RESERVOIRCONTROLLER_DIGITALSENSOR_H

#include <stdint.h>

namespace Driver
{
    class DigitalSensor
    {
    public:
        explicit DigitalSensor(uint8_t pin, int highState = 0x01);
        [[nodiscard]] bool read() const;

    private:
        uint8_t mPin;
        const int mHighState;
    };
}

#endif //RESERVOIRCONTROLLER_DIGITALSENSOR_H
