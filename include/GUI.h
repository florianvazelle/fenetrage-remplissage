#ifndef GUI_H
#define GUI_H

#include <nanogui/nanogui.h>
#include "env.h"

class GUI : public nanogui::Screen {
  public:
    GUI() : nanogui::Screen() {};
    void init(GLFWwindow *window);
    void draw();
};

#endif