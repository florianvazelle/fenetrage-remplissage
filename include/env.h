#include <iostream>
#include <nanogui/nanogui.h>

enum Mode : int { noop, polygone, fenetre };
Mode mode = Mode::noop;

Eigen::Vector2d mouse(0.0f, 0.0f);

void changeMode(Mode m) {
	static const char* ModeStrings[] = { "noop", "polygone", "fenetre" };
	std::cout << "Changement de mode: " << ModeStrings[m] << std::endl;
	mode = m;
}