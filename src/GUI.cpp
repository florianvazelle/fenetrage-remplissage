#include <nanogui/nanogui.h>
#include "GUI.h"

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

	for (Mesh i : polygonHitboxes)
		i.draw(width, height, shader, false, mouse);
    // draw de nanoGUI
    drawContents();
    drawWidgets();
}

void GUI::destroy() {
    cutWindow.destroy();
    polygon.destroy();

	for (Mesh i : polygonHitboxes)
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

        // Sauvegarde de la position de la souris
        gui->mouse[0] = (float)x;
        gui->mouse[1] = (float)y;

		// Convertion Local-to-World [0,width] -> [-1,1] et [0,height] -> [-1,1]
		float xGL = ((gui->mouse[0] + 0.5f) / gui->width) * 2.0f - 1.0f;
		float yGL = 1.0f - ((gui->mouse[1] + 0.5f) / gui->height) * 2.0f;

		if (gui->wantToEditPolygon == true) {
			if (gui->indicePolygonToModify == 0 || gui->indicePolygonToModify == gui->polygon.size() - 1) {
				gui->polygon.setVertex(0, xGL, yGL);
				gui->polygon.setVertex(gui->polygon.size() - 1, xGL, yGL);
			} else {
				gui->polygon.setVertex(gui->indicePolygonToModify, xGL, yGL);
			}
			gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(0, xGL - 0.03, yGL - 0.03);
			gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(1, xGL - 0.03, yGL + 0.03);
			gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(2, xGL + 0.03, yGL + 0.03);
			gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(3, xGL + 0.03, yGL - 0.03);
			gui->polygonHitboxes[gui->indicePolygonToModify].setVertex(4, xGL - 0.03, yGL - 0.03);
		}
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int modifiers) {
        GUI* gui = static_cast<GUI*>(glfwGetWindowUserPointer(window));

        // On recupere si le click a ete realisee dans une widget ou non
        bool inWidget = gui->mouseButtonCallbackEvent(button, action, modifiers);
        if (!inWidget && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

            // Convertion Local-to-World [0,width] -> [-1,1] et [0,height] -> [-1,1]
            float xGL = ((gui->mouse[0] + 0.5f) / gui->width) * 2.0f - 1.0f;
            float yGL = 1.0f - ((gui->mouse[1] + 0.5f) / gui->height) * 2.0f;

            // En fonction du mode, on va ajoute le sommets dans le mesh correspondant
            if (gui->mode == Mode::edit_Polygon_mode) {
				bool insideHitBox = false;
				int indiceHitBox = 0;
				for (Mesh i : gui->polygonHitboxes) {
					if (i.contain(xGL, yGL)) {
						insideHitBox = true;
					}
					indiceHitBox += 1;
				}
				//std::cout << insideHitBox << indiceHitBox << std::endl;
				
				if (insideHitBox) {
					gui->polygon.addVertex( gui->polygon.getVertex(0) );
					gui->mode = Mode::no_Operation_mode;
				}
				else {
					gui->polygon.addVertex({ xGL, yGL });

					Mesh hitBox;
					hitBox.init();
					hitBox.setColor(nanogui::Color(Eigen::Vector4f(0.0f, 1.0f, 0.0f, 1.0f)));
					hitBox.addVertex({ xGL - 0.03, yGL - 0.03 });
					hitBox.addVertex({ xGL - 0.03, yGL + 0.03 });
					hitBox.addVertex({ xGL + 0.03, yGL + 0.03 });
					hitBox.addVertex({ xGL + 0.03, yGL - 0.03 });
					hitBox.addVertex({ xGL - 0.03, yGL - 0.03 });
					gui->polygonHitboxes.push_back(hitBox);
				}
            } else if (gui->mode == Mode::edit_Window_mode) {
                gui->cutWindow.addVertex({ xGL, yGL });
			} else if (gui->mode == Mode::no_Operation_mode) {
				bool insideHitBox = false;
				int indiceHitBox = 0;
				for (Mesh i : gui->polygonHitboxes) {
					if (i.contain(xGL, yGL)) {
						insideHitBox = true;
						break;
					}
					indiceHitBox += 1;
				}
				if (insideHitBox)
				{
					if (gui->wantToEditPolygon == false) {
						std::cout << indiceHitBox << std::endl;
						gui->wantToEditPolygon = true;
						gui->indicePolygonToModify = indiceHitBox;
					}
					else {
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