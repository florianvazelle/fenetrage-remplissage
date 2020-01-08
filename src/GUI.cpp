#include <nanogui/nanogui.h>
#include "GUI.h"

Env myenv;

void GUI::init(GLFWwindow *window) {
    using namespace nanogui;
    initialize(window, true);

    FormHelper *gui = new FormHelper(this);
    ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Actions");
    gui->addGroup("Tracer");

    Button* btn_noop = gui->addButton("Noop", []() { std::cout << "Button pressed." << std::endl; myenv.changeMode(myenv.Mode::noop); });
    btn_noop->setFlags(Button::RadioButton);
    btn_noop->setTooltip("Rien ne se passe");
    btn_noop->setPushed(true);

    gui->addButton("Polygone", []() { std::cout << "Button pressed." << std::endl; myenv.changeMode(myenv.Mode::polygone); })->setFlags(Button::RadioButton);
    gui->addButton("Fenetre", []() { std::cout << "Button pressed." << std::endl; myenv.changeMode(myenv.Mode::fenetre); })->setFlags(Button::RadioButton);
    
    auto cp = new ColorPicker(nanoguiWindow, { 255, 255, 0, 255 });
    cp->setFinalCallback([&](const Color& c) { myenv.currentColor = c; });
    gui->addWidget("", cp);

    gui->addGroup("Effectuer");
    gui->addButton("Fenetrage", []() { std::cout << "Fenetrage" << std::endl; });
    gui->addButton("Remplissage", []() { std::cout << "Remplissage" << std::endl; });

    setVisible(true);
    performLayout();

    // https://stackoverflow.com/questions/28283724/use-a-member-function-as-callback
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, [](GLFWwindow * window, double x, double y) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->cursorPosCallbackEvent(x, y);
        myenv.mouse[0] = (float)x;
        myenv.mouse[1] = (float)y;
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow * window, int button, int action, int modifiers) {
        bool inWidget = static_cast<GUI*>(glfwGetWindowUserPointer(window))->mouseButtonCallbackEvent(button, action, modifiers);
        if (!inWidget && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            std::cout << "Mouse pos: (" << myenv.mouse[0] << ", " << myenv.mouse[1] << ")" << std::endl;
            std::cout << "Click Left!" << std::endl;
            float xClip = ((myenv.mouse[0] + 0.5f) / myenv.width) * 2.0f - 1.0f;
            float yClip = 1.0f - ((myenv.mouse[1] + 0.5f) / myenv.height) * 2.0f;
            if (myenv.mode == myenv.Mode::polygone) {
                myenv.p.addVertex({ xClip, yClip });
            } else if (myenv.mode == myenv.Mode::fenetre) {
                myenv.f.addVertex({ xClip, yClip });
            }
        }
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