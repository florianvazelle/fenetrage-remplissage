#include <nanogui/nanogui.h>
#include "GUI.h"

void GUI::init(GLFWwindow *window) {
    using namespace nanogui;
    initialize(window, true);

    /* */
    FormHelper *gui = new FormHelper(this);
    ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Actions");
    gui->addGroup("Tracer");

    Button* btn_noop = gui->addButton("Aucune operation", [&]() { changeMode(Mode::no_Operation_mode); });
    btn_noop->setFlags(Button::RadioButton);
    btn_noop->setTooltip("Rien ne se passe");
    btn_noop->setPushed(true);

    gui->addButton("Polygone", [&]() { changeMode(Mode::edit_Polygon_mode); })->setFlags(Button::RadioButton);
    gui->addButton("Fenetre", [&]() { changeMode(Mode::edit_Window_mode); })->setFlags(Button::RadioButton);
    
    auto cp = new ColorPicker(nanoguiWindow, { 255, 255, 0, 255 });
    cp->setFinalCallback([&](const Color& c) { currentColor = c; });
    gui->addWidget("", cp);

    gui->addGroup("Effectuer");
    gui->addButton("Fenetrage", []() { std::cout << "Fenetrage" << std::endl; });
    gui->addButton("Remplissage", []() { std::cout << "Remplissage" << std::endl; });

    setVisible(true);
    performLayout();

    defineCallbacks(window);
}

void GUI::draw(uint32_t shader) {
    if (cutWindow.size() == 1) cutWindow.setColor(currentColor);
    if (polygon.size() == 1) polygon.setColor(currentColor);
    cutWindow.draw(width, height, shader, (mode == Mode::edit_Window_mode), mouse);
    polygon.draw(width, height, shader, (mode == Mode::edit_Polygon_mode), mouse);

    // 
    drawContents();
    drawWidgets();
}

void GUI::destroy() {
    cutWindow.destroy();
    polygon.destroy();
}

void GUI::changeMode(Mode m) {
    static const char* ModeStrings[] = { "no_Operation_mode", "edit_Polygon_mode", "edit_Window_mode" };
    std::cout << "Changement de mode: " << ModeStrings[(int)m] << std::endl;
    mode = m;
}

void GUI::defineCallbacks(GLFWwindow* window) {
    // https://stackoverflow.com/questions/28283724/use-a-member-function-as-callback
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));
        // Appeler a chaque fois que la souris change de position
        gui->cursorPosCallbackEvent(x, y);
        // Sauvegarde de la position de la souris
        gui->mouse[0] = (float)x;
        gui->mouse[1] = (float)y;
        });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int modifiers) {
        GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));
        bool inWidget = gui->mouseButtonCallbackEvent(button, action, modifiers);
        if (!inWidget && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            std::cout << "Mouse pos: (" << gui->mouse[0] << ", " << gui->mouse[1] << ")" << std::endl;
            std::cout << "Click Left!" << std::endl;
            // Convertion Local-to-World [0,width] -> [-1,1] et [0,height] -> [-1,1]
            float xGL = ((gui->mouse[0] + 0.5f) / gui->width) * 2.0f - 1.0f;
            float yGL = 1.0f - ((gui->mouse[1] + 0.5f) / gui->height) * 2.0f;
            if (gui->mode == Mode::edit_Polygon_mode) {
                gui->polygon.addVertex({ xGL, yGL });
            }
            else if (gui->mode == Mode::edit_Window_mode) {
                gui->cutWindow.addVertex({ xGL, yGL });
            }
        }
        });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->keyCallbackEvent(key, scancode, action, mods);
        });

    glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int codepoint) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->charCallbackEvent(codepoint);
        });

    glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** filenames) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->dropCallbackEvent(count, filenames);
        });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->scrollCallbackEvent(x, y);
        });

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        static_cast<GUI*>(glfwGetWindowUserPointer(window))->resizeCallbackEvent(width, height);
        });
}