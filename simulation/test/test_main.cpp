#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch_all.hpp>
#include "reservoir.hpp"
#include "scaled_clock.hpp"
#include <numeric>

void require_close(float target, float value, float acceptedError = std::numeric_limits<float>::epsilon())
{
    REQUIRE(std::abs(target - value) < acceptedError);
}


TEST_CASE("Liquid", "[liquid]") {

    Liquid l0 {.amount = 1.0f, .ph = 14.0f, .ec = 0.0f};
    Liquid l2 {.amount = 1.0f, .ph = 0.0f, .ec = 2.0f};

    Liquid r0 = l0 + l2;

    require_close(r0.amount, 2.0f);
    require_close(r0.ph, 7.0f);
    require_close(r0.ec, 1.0f);
}


TEST_CASE("scaled_clock", "[scaled_clock]")
{
    using std::chrono::steady_clock;

    constexpr double k = 2.0f;
    constexpr double epsilon = 0.00001f;
    constexpr double lok = k - epsilon;
    constexpr double upk = k + epsilon;

    ScaledClock<double> scaled(k);
    steady_clock steady;

    const auto scaledT0 = scaled.now();
    const auto steadyT0 = steady.now();

    while (steady.now() - steadyT0 < std::chrono::milliseconds(100)) {}

    const auto scaledT1 = scaled.now();
    const auto steadyT1 = steady.now();

    REQUIRE(scaledT1.time_since_epoch().count() >= (lok * steadyT1.time_since_epoch().count()));
    REQUIRE(scaledT1.time_since_epoch().count() <= (upk * steadyT1.time_since_epoch().count()));

    const auto scaledDur = scaledT1 - scaledT0;
    const auto steadyDur = steadyT1 - steadyT0;

    REQUIRE(scaledDur >= lok * steadyDur);
    REQUIRE(scaledDur <= upk * steadyDur);

}