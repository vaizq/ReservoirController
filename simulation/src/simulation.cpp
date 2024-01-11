#include "simulation.hpp"
#include "dt_timer.hpp"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cmath>
#include <iostream>


constexpr Core::Controller::Config phControllerConfig()
{
    return Core::Controller::Config{{5.8, 6.2}, 10.00f, std::chrono::minutes(10)};
}


Simulation::Simulation()
: 
    mClock(60.0f), 
    mReservoir{250.f, finnishTapWater(mConfig.targetLevel)},
    mLLController(Driver::LiquidLevelSensor{mReservoir, mConfig.targetLevel}, Driver::Valve{mReservoir, mConfig.waterFlowRate_L, finnishTapWater}),
    mPhController{Driver::PhSensor{mReservoir}, Doser{Driver::Valve{mReservoir, mConfig.phFlowRate_mL, phDownSolution}, mConfig.phFlowRate_mL}, phControllerConfig()}
{}

Simulation &Simulation::instance()
{
    static Simulation app;
    return app;
}


bool Simulation::update()
{
    static DtTimer<Clock> timer(mClock);
    const auto dt = timer.tick();

    updateGui(dt);
    updateControllers(dt);

    return true;
}

void Simulation::updateGui(const Duration)
{
    ImGui::Begin("Reservoir status");
    {
        ImGui::Text("Amount: %f", mReservoir.content().amount);
        ImGui::Text("pH: %f", mReservoir.content().ph);
        ImGui::Text("EC: %f", mReservoir.content().ec);
    }
    ImGui::End();

    ImGui::Begin("Environment");
    {
        static float timeScale = mClock.getScale();
        if (ImGui::SliderFloat("Time scale", &timeScale, 1.0f, 120.0f))
        {
            mClock.setScale(timeScale);
        }

        static float waterAmount = 0.0f;
        ImGui::SliderFloat("WaterAmount", &waterAmount, 0.0f, 100.0f);
        if (ImGui::Button("Add water"))
        {
            Liquid tapWater(waterAmount, 8.0f, 0.1f);
            mReservoir.add(tapWater);
        }

        if (ImGui::Button("Remove liquid"))
        {
            mReservoir.remove(waterAmount);
        }
    }
    ImGui::End();
}


void Simulation::updateControllers(const Duration dt)
{
    mLLController.update(dt);
    mLLController.getValve().update(dt);

    mPhController.update(dt);
    mPhController.getValve().update(dt);
}