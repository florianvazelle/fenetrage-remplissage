#include <nanogui/nanogui.h>
#include "GUI.h"

float margin = 0.03f;

GUI::GUI() : nanogui::Screen() {
    mode = Mode::no_Operation_mode;
    width = 640;
    height = 480;
    mouse = Eigen::Vector2f(0.0f, 0.0f);
    currentColor = nanogui::Color(Eigen::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	polygonHitboxes.clear();
};

void GUI::init(GLFWwindow *window) {
    using namespace nanogui;
    initialize(window, true);

    FormHelper *gui = new FormHelper(this);
    ref<Window> nanoguiWindow = gui->addWindow(Eigen::Vector2i(10, 10), "Actions");
    gui->addGroup("Tracer");

    Button* btn_noop = gui->addButton("Aucune operation", [&]() { changeMode(Mode::no_Operation_mode); });
    btn_noop->setFlags(Button::RadioButton);
    btn_noop->setTooltip("Rien ne se passe");
    btn_noop->setPushed(true);

    gui->addButton("Polygone", [&]() { changeMode(Mode::edit_Polygon_mode); })->setFlags(Button::RadioButton);
    gui->addButton("Fenetre", [&]() { changeMode(Mode::edit_Window_mode); })->setFlags(Button::RadioButton);
    
    auto cp = new ColorPicker(nanoguiWindow, { 255, 0, 0, 255 });
    cp->setFinalCallback([&](const Color& c) { currentColor = c; });
    gui->addWidget("", cp);

    gui->addGroup("Effectuer");
    gui->addButton("Fenetrage", []() { std::cout << "Fenetrage" << std::endl; });
    gui->addButton("Remplissage", []() { std::cout << "Remplissage" << std::endl; });

    setVisible(true);
    performLayout();

    defineCallbacks(window);

    cutWindow.init();
    polygon.init();
}

void GUI::draw(uint32_t shader) {
    if (cutWindow.size() == 1) cutWindow.setColor(currentColor);
    if (polygon.size() == 1) polygon.setColor(currentColor);

    cutWindow.draw(width, height, shader, (mode == Mode::edit_Window_mode), mouse);
    polygon.draw(width, height, shader, (mode == Mode::edit_Polygon_mode), mouse);

    // Draw Hitbox
	for (Hitbox i : polygonHitboxes)
		i.draw(shader, mouse);

    // draw de nanoGUI
    drawContents();
    drawWidgets();
}

void GUI::destroy() {
    cutWindow.destroy();
    polygon.destroy();

	for (Hitbox i : polygonHitboxes)
		i.destroy();
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

		if (gui->wantToEditPolygon == true) {
			if (gui->indicePolygonToModify == 0 || gui->indicePolygonToModify == gui->polygon.size() - 1) {
				gui->polygon.setVertex(0, coordGL);
				gui->polygon.setVertex(gui->polygon.size() - 1, coordGL);
			} else {
				gui->polygon.setVertex(gui->indicePolygonToModify, coordGL);
			}

            gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(0, { coordGL[0] - margin, coordGL[1] - margin });
            gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(1, { coordGL[0] - margin, coordGL[1] + margin });
            gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(2, { coordGL[0] + margin, coordGL[1] + margin });
            gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(3, { coordGL[0] + margin, coordGL[1] - margin });
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
				bool insideHitBox = false;
				int indiceHitBox = 0;

                // Regarde si le click s'effectue dans une hitbox
                for (indiceHitBox = 0; indiceHitBox < gui->polygonHitboxes.size(); indiceHitBox++) {
                    Hitbox currentHitbox = gui->polygonHitboxes[indiceHitBox];
                    if (currentHitbox.contain(coordGL[0], coordGL[1])) {
                        insideHitBox = true;
                        break;
                    }
                }

				if (insideHitBox) {
                    // Si oui, on ferme le mesh
					gui->polygon.addVertex(gui->polygon.getVertex(0));
					gui->mode = Mode::no_Operation_mode;
				} else {
                    // Sinon, comportement normal
                    // On ajoute le point au mesh et sa hitbox correspondante
					gui->polygon.addVertex(coordGL);

					Hitbox hitBox;
					hitBox.init();
                    hitBox.addVertex({ coordGL[0] - margin, coordGL[1] - margin });
                    hitBox.addVertex({ coordGL[0] - margin, coordGL[1] + margin });
                    hitBox.addVertex({ coordGL[0] + margin, coordGL[1] + margin });
                    hitBox.addVertex({ coordGL[0] + margin, coordGL[1] - margin });
					gui->polygonHitboxes.push_back(hitBox);
				}
            } else if (gui->mode == Mode::edit_Window_mode) {
                gui->cutWindow.addVertex(coordGL);
			} else if (gui->mode == Mode::no_Operation_mode) {
				bool insideHitBox = false;
				int indiceHitBox = 0;

                // Regarde si le click s'effectue dans une hitbox
                for (indiceHitBox = 0; indiceHitBox < gui->polygonHitboxes.size(); indiceHitBox++) {
                    Hitbox currentHitbox = gui->polygonHitboxes[indiceHitBox];
                    if (currentHitbox.contain(coordGL[0], coordGL[1])) {
                        insideHitBox = true;
                        break;
                    }
                }

				if (insideHitBox) {
                    // Si le click est dans une hitbox
					if (gui->wantToEditPolygon == false) {
                        // On indique que l'on veut modifier un sommet du polygon 
                        // et on indique l'indice de ce sommet
						gui->wantToEditPolygon = true;
						gui->indicePolygonToModify = indiceHitBox;
					} else {
						gui->wantToEditPolygon = false;
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