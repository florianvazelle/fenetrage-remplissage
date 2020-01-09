#include <nanogui/nanogui.h>
#include "GUI.h"

void GUI::init(GLFWwindow *window) {
    using namespace nanogui;
    initialize(window, true);

    /* */
    FormHelper *gui = new FormHelper(this);
    ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Actions");
    gui->addGroup("Tracer");

    Button* btn_noop = gui->addButton("Aucune operation", [&]() { std::cout << "Button pressed." << std::endl; env->changeMode(env->Mode::no_Operation_mode); });
    btn_noop->setFlags(Button::RadioButton);
    btn_noop->setTooltip("Rien ne se passe");
    btn_noop->setPushed(true);

    gui->addButton("Polygone", [&]() { std::cout << "Button pressed." << std::endl; env->changeMode(env->Mode::edit_Polygon_mode); })->setFlags(Button::RadioButton);
    gui->addButton("Fenetre", [&]() { std::cout << "Button pressed." << std::endl; env->changeMode(env->Mode::edit_Window_mode); })->setFlags(Button::RadioButton);
    
    auto cp = new ColorPicker(nanoguiWindow, { 255, 255, 0, 255 });
    cp->setFinalCallback([&](const Color& c) { env->currentColor = c; });
    gui->addWidget("", cp);

    gui->addGroup("Effectuer");
    gui->addButton("Fenetrage", [&]() { std::cout << "Fenetrage" << std::endl; });
    gui->addButton("Remplissage", [&]() { std::cout << "Remplissage" << std::endl; });

    setVisible(true);
    performLayout();
}

void GUI::draw() {
    drawContents();
    drawWidgets();
}