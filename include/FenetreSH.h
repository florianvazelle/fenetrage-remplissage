#ifndef FenetreSH_H
#define FenetreSH_H

#include <vector>
#include <nanogui/nanogui.h>
#include "Mesh.h"

void Decoupage(std::vector<Eigen::Vector2f>&, const Mesh&, const Mesh&);
void Decoupage(std::vector<Eigen::Vector2f>&, const std::vector<Eigen::Vector2f>&, const std::vector<Eigen::Vector2f>&);

#endif