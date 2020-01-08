#ifndef ENV_H
#define ENV_H

#include <iostream>
#include <nanogui/nanogui.h>
#include "Mesh.h"

enum Mode : int { noop, polygone, fenetre };
Mode mode = Mode::noop;

Eigen::Vector2f mouse(0.0f, 0.0f);
Mesh p;
Mesh f;

void changeMode(Mode m) {
	static const char* ModeStrings[] = { "noop", "polygone", "fenetre" };
	std::cout << "Changement de mode: " << ModeStrings[m] << std::endl;
	mode = m;
}

#endif
