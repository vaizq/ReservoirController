#include "app.hpp"
#include "core/dt_timer.hpp"


int main()
{
    auto& app = App::instance();
    bool run = true;
    while(run)
    {
        run = app.loop();
    }
}