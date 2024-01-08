#include "app.hpp"
#include "core/dt_timer.hpp"


int main()
{
    App app;

    DtTimer timer;

    bool run = true;
    while(run)
    {
        const auto dt = timer.tick();
        run = app.loop(dt);
    }
}