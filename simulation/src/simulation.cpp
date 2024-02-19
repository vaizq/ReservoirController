#include "simulation.hpp"
#include "dt_timer.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cmath>
#include <iostream>
#include <chrono>


Doser makeDoser(Reservoir& reservoir, float flowRate, Driver::Valve::LiquidSupply supply)
{
    return Doser{Driver::Valve{reservoir, flowRate, supply}, flowRate};
}

std::array<Doser, nutrientPumpCount> makeNutrientDosers(Reservoir& reservoir, float flowRate, Driver::Valve::LiquidSupply supply)
{
    Doser doser{makeDoser(reservoir, flowRate, supply)};
    return {doser, doser, doser};
}

Simulation::Simulation()
: 
    mClock(60.0f), 
    mReservoir{250.f, finnishTapWater(mConfig.targetLevel)},
    mLLController(Driver::LiquidLevelSensor{mReservoir, mConfig.targetLevel}, Driver::Valve{mReservoir, mConfig.waterFlowRate, finnishTapWater}),
    mPhController{Driver::PhSensor{mReservoir}, makeDoser(mReservoir, mConfig.doserFlowRate, phDownSolution), phControllerConfig},
    mEcController{Driver::EcSensor{mReservoir}, makeNutrientDosers(mReservoir, mConfig.doserFlowRate, nutrientSolution), ecControllerConfig}
{
    mEcController.setSchedule(feedingSchedule);
}

Simulation &Simulation::instance()
{
    static Simulation app;
    return app;
}


bool Simulation::update()
{
    static DtTimer<Clock> timer(mClock);
    const auto dt = timer.tick();

    onGUI(dt);
    updateControllers(dt);

    return true;
}

void Simulation::onGUI(const Duration)
{
    ImGui::Begin("Reservoir status");
    {
        ImGui::Text("Amount (L): %f", mReservoir.content().amount);
        ImGui::Text("pH: %f", mReservoir.content().ph);
        ImGui::Text("EC: %f", mReservoir.content().ec);
    }
    ImGui::End();


    ImGui::Begin("Environment");
    {
        const std::chrono::hh_mm_ss t{mClock.now().time_since_epoch()};
        ImGui::Text("Time: %ld:%ld:%ld", t.hours().count(), t.minutes().count(), t.seconds().count());

        static float timeScale = mClock.getScale();
        if (ImGui::SliderFloat("Time scale", &timeScale, 1.0f, 120.0f))
        {
            mClock.setScale(timeScale);
        }

        auto separate = []() {
            for (int i = 0; i < 4; i++)
                ImGui::Spacing();
            ImGui::Separator();
            for (int i = 0; i < 4; i++)
                ImGui::Spacing();
        };
        separate();

        static float waterAmount = 0.0f;
        ImGui::SliderFloat("WaterAmount (L)", &waterAmount, 0.0f, 100.0f);
        if (ImGui::Button("Add water"))
        {
            mReservoir.add(finnishTapWater(waterAmount));
        }
        if (ImGui::Button("Remove water"))
        {
            mReservoir.remove(waterAmount);
        }

        separate();

        static float phAdjusterAmount = 0.0f;
        ImGui::SliderFloat("Ph adjuster amount (mL)", &phAdjusterAmount, 0.0f, 10.0f);
        if (ImGui::Button("Add ph down"))
        {
            mReservoir.add(phDownSolution(mL2L(phAdjusterAmount)));
        }
        if (ImGui::Button("Add ph up"))
        {
            mReservoir.add(phUpSolution(mL2L(phAdjusterAmount)));
        }

        separate();

        static float nutrientAmount = 0.0f;
        ImGui::SliderFloat("Nutrient amount (mL)", &nutrientAmount, 0.0f, 100.0f);
        if (ImGui::Button("Add nutrient"))
        {
            mReservoir.add(nutrientSolution(mL2L(nutrientAmount)));
        }
    }
    ImGui::End();
}


void Simulation::updateControllers(const Duration dt)
{
    mLLController.update(dt);
    mLLController.getValve().update(dt);

    mPhController.update(dt);
    mPhController.getDoser().getValve().update(dt);

    mEcController.update(dt);
    auto& dosers = mEcController.getDosers();
    for (auto& doser : dosers)
    {
        doser.getValve().update(dt);
    }
}