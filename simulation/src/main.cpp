#include "app.hpp"
#include "core/dt_timer.hpp"


int main()
{
    App app;

    bool run = true;
    while(run)
    {
        app.loop();
    }
}