#include "app_base.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


AppBase::AppBase()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // Create a windowed mode window and its OpenGL context
    mWindow = glfwCreateWindow(1280, 960, "Hello World", NULL, NULL);

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

AppBase::~AppBase()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(mWindow);
    glfwTerminate();
}

bool AppBase::loop()
{
    bool run{true};

    initFrame();
    run &= update();
    renderFrame();

    run &= !glfwWindowShouldClose(mWindow);
    return run;
}

void AppBase::initFrame()
{
    // Poll for and process events
    glfwPollEvents();

    // Start new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void AppBase::renderFrame()
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