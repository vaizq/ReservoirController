#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch_all.hpp>
#include "do_timer.hpp"
#include "reservoir.hpp"
#include "relative_clock.hpp"
#include "driver/valve.hpp"
#include "driver/liquid_level_sensor.hpp"
#include <numeric>
#include <vector>
#include <thread>
#include <iostream>


void require_close(float target, float value, float acceptedError = std::numeric_limits<float>::epsilon())
{
    REQUIRE(std::abs(target - value) < acceptedError);
}

#define REQUIRE_CLOSE(target, value, acc) REQUIRE(std::abs(target - value) > std::abs(acc))


TEST_CASE("Liquid", "[liquid]") {

    Liquid l0 {.amount = 1.0f, .ph = 14.0f, .ec = 0.0f};
    Liquid l2 {.amount = 1.0f, .ph = 0.0f, .ec = 2.0f};

    Liquid r0 = l0 + l2;

    require_close(r0.amount, 2.0f);
    require_close(r0.ph, 7.0f);
    require_close(r0.ec, 1.0f);
}

constexpr std::chrono::milliseconds toMs(auto duration)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration);
}

TEST_CASE("variation", "[relative_clock]")
{
    using std::chrono::steady_clock;

    auto abs_diff = [](auto a, auto b) {
        if (a > b) 
            return a - b;
        else 
            return b - a;
    };

    std::vector<double> scalars = {2.0, 1.0, 60.0, 3.0, 100.0, 4.0, 99.0, 0.1};
    constexpr std::chrono::milliseconds dt(10);
    constexpr auto ace = std::chrono::milliseconds(1);

    RelativeClock<steady_clock, double> scaled(1.0f);
    steady_clock steady;


    const auto scaledT0 = scaled.now();
    const auto steadyT0 = steady.now();

    for (double k : scalars)
    {
        scaled.setScale(k);
        const auto t0 = steady.now();
        while (steady.now() - t0 < dt) {}
    }

    const auto scaledT1 = scaled.now();
    const auto steadyT1 = steady.now();

    const auto duration = steadyT1 - steadyT0;
    const auto scaledDuration = scaledT1 - scaledT0;

    using Duration = decltype(scaled)::duration;
    const auto expected = std::accumulate(scalars.begin(), scalars.end(), Duration(0),
        [dt](Duration dur, double k) {
            return dur + std::chrono::duration_cast<Duration>(k * dt);
        });
    
    const double avgK = std::accumulate(scalars.begin(), scalars.end(), 0.) / scalars.size();

    std::cout << "real: " << toMs(duration).count() << "ms" << std::endl;
    std::cout << "scaled: " << toMs(scaledDuration).count() << "ms" << std::endl;
    std::cout << "expected: " << toMs(expected).count() << "ms" << std::endl;


    REQUIRE(abs_diff(scaledDuration, expected) < ace);
    REQUIRE(abs_diff(scaledDuration, avgK * duration) < ace);
}

TEST_CASE("do_timer", "[do_timer]")
{
    using std::chrono::steady_clock;
    constexpr double lower = 0.9;
    constexpr double upper = 1.1;
    steady_clock clock;
    constexpr int n = 10;
    constexpr auto interval = std::chrono::milliseconds(10);
    DoTimer<steady_clock> timer(clock, interval);

    int i = 0;
    const auto startTime = clock.now();
    std::vector<steady_clock::time_point> times;

    while (i < n)
    {
        if (timer.isTime())
        {
            ++i;
            times.push_back(clock.now());
        }
    }

    auto a = times.begin();
    auto b = a + 1;

    for (; b != times.end(); a = b, ++b)
    {
        const auto duration = *b - *a;
        REQUIRE(duration > lower * interval);
        REQUIRE(duration < upper * interval);
    }
}

TEST_CASE("valve", "[valve]")
{
    constexpr float flowRate = 1.0f;
    constexpr std::chrono::duration<float> openDuration = std::chrono::milliseconds(100);
    constexpr float lower = 0.9f * flowRate * openDuration.count();
    constexpr float upper = 1.1f * flowRate * openDuration.count();
    Reservoir reservoir(100, Liquid{});
    Driver::Valve valve(reservoir, flowRate);

    valve.open();
    std::this_thread::sleep_for(openDuration);
    valve.close();

    REQUIRE(reservoir.content().amount > lower);
    REQUIRE(reservoir.content().amount < upper);

    std::this_thread::sleep_for(openDuration);

    valve.open();
    std::this_thread::sleep_for(openDuration);
    valve.close();

    REQUIRE(reservoir.content().amount > 2 * lower);
    REQUIRE(reservoir.content().amount < 2 * upper);
}
