#include <nanogui/nanogui.h>
#include "GUI.h"

float margin = 0.03f;

GUI::GUI() : nanogui::Screen() {
    mode = Mode::no_Operation_mode;
    width = 640;
    height = 480;
    mouse = Eigen::Vector2f(0.0f, 0.0f);
    currentColor = nanogui::Color(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

    polyToModify = nullptr;
};

void GUI::init(GLFWwindow *window) {
    using namespace nanogui;
    initialize(window, true);

    Window* w = new Window(this, "Actions");
    w->setPosition(Vector2i(10, 10));
    w->setLayout(new GroupLayout());

    /* Premiere partie */

    new Label(w, "Tracer", "sans-bold", 25);

    GridLayout* layout = new GridLayout(Orientation::Horizontal, 2, Alignment::Fill, 0, 5);

    Widget* tools = new Widget(w);
    tools->setLayout(layout);

    Button* b = new Button(tools, "No operation");
    b->setCallback([&] { changeMode(Mode::no_Operation_mode); });
    b->setFlags(Button::RadioButton);
    b->setPushed(true);

    b = new Button(tools, "");
    b->setCallback([&] { changeMode(Mode::no_Operation_mode); });
    b->setIcon(ENTYPO_ICON_MOUSE_POINTER);
    b->setFlags(Button::RadioButton);

    b = new Button(tools, "Polygone");
    b->setCallback([&] { changeMode(Mode::edit_Polygon_mode); });
    b->setFlags(Button::RadioButton);

    b = new Button(tools, "");
    b->setIcon(ENTYPO_ICON_TRASH);
    b->setCallback([&] { polygons.pop_back(); });

    b = new Button(tools, "Fenetre");
    b->setCallback([&] { changeMode(Mode::edit_Window_mode); });
    b->setFlags(Button::RadioButton);

    b = new Button(tools, "");
    b->setIcon(ENTYPO_ICON_TRASH);
    b->setCallback([&] { cutWindow.clear(); cutWindow.setClose(false); });

    auto cp = new ColorPicker(tools, { 255, 0, 0, 255 });
    cp->setFinalCallback([&](const Color& c) { currentColor = c; });

    /* Seconde partie */

    new Label(w, "Effectuer", "sans-bold", 25);

    b = new Button(w, "Fenetrage");
    b->setCallback([&] { std::cout << "Fenetrage" << std::endl; });

    b = new Button(w, "Remplissage");
    b->setCallback([&] { std::cout << "Remplissage" << std::endl; });

    setVisible(true);
    performLayout();

    defineCallbacks(window);

    cutWindow.init();
}

void GUI::draw(uint32_t shader) {
    if (cutWindow.size() == 1) cutWindow.setColor(currentColor);
    if (polygons.size() > 0 && polygons.back().size() == 1) (&polygons.back())->setColor(currentColor);

    cutWindow.draw(width, height, shader, (mode == Mode::edit_Window_mode), mouse);
    for (Mesh poly : polygons)
        poly.draw(width, height, shader, (mode == Mode::edit_Polygon_mode), mouse);

    // draw de nanoGUI
    drawContents();
    drawWidgets();
}

void GUI::destroy() {
    cutWindow.destroy();
    for (Mesh poly : polygons)
        poly.destroy();
}

void GUI::changeMode(Mode m) {
    static const char* ModeStrings[] = { "no_Operation_mode", "edit_Polygon_mode", "edit_Window_mode" };
    std::cout << "Changement de mode: " << ModeStrings[(int)m] << std::endl;
    mode = m;
}

void GUI::defineCallbacks(GLFWwindow* window) {
    
    // On bind notre instance de GUI, pour l'utiliser dans les callbacks (https://stackoverflow.com/questions/28283724/use-a-member-function-as-callback)
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));

        // Appeler a chaque fois que la souris change de position
        gui->cursorPosCallbackEvent(x, y);

        Eigen::Vector2f coordGL = gui->local2world(x, y);

        // Sauvegarde de la position de la souris
        gui->mouse[0] = coordGL[0];
        gui->mouse[1] = coordGL[1];

		if (gui->wantToEditPolygon && gui->polyToModify != nullptr) {
            gui->indicePointToModify->setPosition(coordGL);
		}
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int modifiers) {
        GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));

        // On recupere si le click a ete realisee dans une widget ou non
        bool inWidget = gui->mouseButtonCallbackEvent(button, action, modifiers);
        if (!inWidget && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

            Eigen::Vector2f coordGL = gui->mouse;

            // En fonction du mode, on va ajoute le sommets dans le mesh correspondant
            if (gui->mode == Mode::edit_Polygon_mode) {
                if (gui->polygons.size() == 0 || gui->polygons.back().isClose()) {
                    Mesh tmp;
                    tmp.init();
                    gui->polygons.push_back(tmp);
                }

                Mesh* currentPoly = &gui->polygons.back();
                iterator_point pointInsideHitBox = currentPoly->contain(coordGL[0], coordGL[1]);

				if (currentPoly->isValid(pointInsideHitBox)) {
                    // Si oui, on ferme le mesh
                    currentPoly->setClose(true);
				} else {
                    // Sinon, comportement normal
                    // On ajoute le point au mesh et sa hitbox correspondante
                    currentPoly->addVertex(coordGL);
				}
            } else if (gui->mode == Mode::edit_Window_mode) {
                iterator_point pointInsideHitBox = gui->cutWindow.contain(coordGL[0], coordGL[1]);

                if (gui->cutWindow.isValid(pointInsideHitBox)) {
                    gui->cutWindow.setClose(true);
                } else {
                    gui->cutWindow.addVertex(coordGL);
                }
			} else if (gui->mode == Mode::no_Operation_mode) {
                if (gui->wantToEditPolygon) {
                    gui->wantToEditPolygon = false;
                    gui->polyToModify = nullptr;
                    return;
                }

                iterator_point pointInsideHitBox;

                // On parcours tout les polygones
                for (int i = 0; i < gui->polygons.size(); i++) {
                    pointInsideHitBox = gui->polygons[i].contain(coordGL[0], coordGL[1]);
                    // Si le click est dans une hitbox
                    if (gui->polygons[i].isValid(pointInsideHitBox)) {
                        if (!gui->wantToEditPolygon) {
                            // On indique que l'on veut modifier un sommet du polygon 
                            // et on indique le sommet
                            gui->wantToEditPolygon = true;
                            gui->polyToModify = &gui->polygons[i];
                            gui->indicePointToModify = pointInsideHitBox;
                        }
                        break;
                    }
                }

                // Pareil pour la fenetre
                pointInsideHitBox = gui->cutWindow.contain(coordGL[0], coordGL[1]);
                if (gui->cutWindow.isValid(pointInsideHitBox)) {
                    if (!gui->wantToEditPolygon) {
                        // On indique que l'on veut modifier un sommet du polygon 
                        // et on indique le sommet
                        gui->wantToEditPolygon = true;
                        gui->polyToModify = &gui->cutWindow;
                        gui->indicePointToModify = pointInsideHitBox;
                    }
                }
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