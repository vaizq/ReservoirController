#include "simulation.hpp"
#include "core/dt_timer.hpp"


int main()
{
    AppBase& sim = Simulation::instance();
    bool run = true;
    while(run)
    {
        run = sim.loop();
    }
}