#pragma once

#include <GLFW/glfw3.h>


class AppBase
{
public:
    AppBase();
    virtual ~AppBase();
    bool loop();
private:
    virtual void initFrame();
    virtual bool update() = 0;
    virtual void renderFrame();
private:
    GLFWwindow* mWindow;
};