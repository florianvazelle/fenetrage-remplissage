#include <nanogui/nanogui.h>

class GUI : public nanogui::Screen {
  public:
    GUI() : nanogui::Screen() {};
    void init(GLFWwindow *window);
    void draw();
};