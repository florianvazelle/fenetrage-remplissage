#include <iostream>

enum class Mode { noop, polygone, fenetre };
enum Mode mode = Mode::noop;

void changeMode(enum Mode m) {
	static const char* ModeStrings[] = { "noop", "polygone", "fenetre" };
	std::cout << "Changement de mode: " << ModeStrings[m] << std::endl;
	mode = m;
}