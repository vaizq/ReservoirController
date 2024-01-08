#include "app.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cmath>

App::App()
{
  // Initialize GLFW
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // Create a windowed mode window and its OpenGL context
    mWindow = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

    // Make the window's context current
    glfwMakeContextCurrent(mWindow);

    // Setup ImGui context
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); 
    io.IniFilename = "imgui.ini";
    (void)io;

    // Setup ImGui Platform and Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

App::~App()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

bool App::loop(Duration dt)
{
    // Poll for and process events
    glfwPollEvents();

    // Start new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    update(dt);
    render(dt);

    return !glfwWindowShouldClose(mWindow);
}

void App::update(Duration dt)
{
    ImGui::Begin("Reservoir status");
    {
        ImGui::Text("Amount: %f", mReservoir.content().amount);
        ImGui::Text("pH: %f", mReservoir.content().ph);
        ImGui::Text("EC: %f", mReservoir.content().ec);
    }
    ImGui::End();

    ImGui::Begin("PID Controller");
    {
        static float sp = 6.0f;
        static float kp = 0.01f;
        static float ki = 0.01f;
        static float kd = 0.01f;


        bool update = (ImGui::SliderFloat("Setpoint", &sp, 0.0f, 14.0f)) ||
            (ImGui::SliderFloat("Kp", &kp, 0.0f, 1.0f)) ||
            (ImGui::SliderFloat("Ki", &ki, 0.0f, 1.0f)) ||
            (ImGui::SliderFloat("Kd", &kd, 0.0f, 1.0f));

        if (update)
            mPid.setParams(kp, kd, ki);

        if (ImGui::Button("Reset integral"))
            mPid.resetIntegral();

    }
    ImGui::End();

    ImGui::Begin("Environment");
    {
        ImGui::SliderFloat("Time scale", &mTimeScale, 1.0f, 60.0f * 24.0f);

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

    static Clock::time_point lastUpdate = Clock::now();

    auto now = Clock::now();
    if (mTimeScale * (now - lastUpdate) > mDosingInterval)
    {
        const float ph = mReservoir.content().ph;
        const float pidOutput = mPid.calculate(mTargetPh, ph, dt * mTimeScale);
        const float amount = std::abs(pidOutput);
        if (pidOutput > 0.0f)
        {
            mReservoir.add(Liquid(amount, 14.0f, 0.0f)); // add ph up
        }
        else
        {
            mReservoir.add(Liquid(amount, 0.0f, 0.0f)); // add ph down
        }
        lastUpdate = now;
    }

}

void App::render(Duration dt)
{
    // Render ImGui
    ImGui::Render();
    int displayW, displayH;
    glfwGetFramebufferSize(mWindow, &displayW, &displayH);
    glViewport(0, 0, displayW, displayH);
    glClearColor(0, 0, 0, 255);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap front and back buffers
    glfwSwapBuffers(mWindow);
}
