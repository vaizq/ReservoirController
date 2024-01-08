#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch_all.hpp>
#include "reservoir.hpp"
#include <numeric>

void require_close(float target, float value)
{
    REQUIRE(std::abs(target - value) < std::numeric_limits<float>::epsilon());
}

TEST_CASE("Liquid", "[liquid]") {

    Liquid l0 {.amount = 1.0f, .ph = 14.0f, .ec = 0.0f};
    Liquid l2 {.amount = 1.0f, .ph = 0.0f, .ec = 2.0f};

    Liquid r0 = l0 + l2;

    require_close(r0.amount, 2.0f);
    require_close(r0.ph, 7.0f);
    require_close(r0.ec, 1.0f);
}