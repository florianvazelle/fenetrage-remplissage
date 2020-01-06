#include "env.h"
#include <GUI.h>
#include <nanogui/nanogui.h>

void GUI::init(GLFWwindow *window) {
    using namespace nanogui;
    initialize(window, true);

    FormHelper *gui = new FormHelper(this);
    ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Actions");
    gui->addGroup("Tracer");
    gui->addButton("Polygone", []() { std::cout << "Button pressed." << std::endl; changeMode(Mode::polygone); });
    gui->addButton("Fenetre", []() { std::cout << "Button pressed." << std::endl; changeMode(Mode::fenetre); });
    
    gui->addGroup("Effectuer");
    gui->addButton("Fenetrage", []() { std::cout << "Fenetrage" << std::endl; });
    gui->addButton("Remplissage", []() { std::cout << "Remplissage" << std::endl; });

    setVisible(true);
    performLayout();

    // https://stackoverflow.com/questions/28283724/use-a-member-function-as-callback
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, [](GLFWwindow * window, double x, double y) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->cursorPosCallbackEvent(x, y);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow * window, int button, int action, int modifiers) {
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            std::cout << "Click Left!" << std::endl;
            if (mode == Mode::polygone) {

            } else if (mode == Mode::fenetre) {

            }
        }
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->mouseButtonCallbackEvent(button, action, modifiers);
    });

    glfwSetKeyCallback(window, [](GLFWwindow * window, int key, int scancode, int action, int mods) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->keyCallbackEvent(key, scancode, action, mods);
    });

    glfwSetCharCallback(window, [](GLFWwindow * window, unsigned int codepoint) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->charCallbackEvent(codepoint);
    });

    glfwSetDropCallback(window, [](GLFWwindow * window, int count, const char **filenames) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->dropCallbackEvent(count, filenames);
    });

    glfwSetScrollCallback(window, [](GLFWwindow * window, double x, double y) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->scrollCallbackEvent(x, y);
    });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow * window, int width, int height) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->resizeCallbackEvent(width, height);
    });
}

void GUI::draw() {
    drawContents();
    drawWidgets();
}