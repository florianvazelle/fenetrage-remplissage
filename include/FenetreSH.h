#ifndef FenetreSH_H
#define FenetreSH_H

#include <vector>
#include <nanogui/nanogui.h>
#include "Mesh.h"

bool coupe(const Eigen::Vector2f& S, const Eigen::Vector2f& P, const Eigen::Vector2f& F, const Eigen::Vector2f& F1);
Eigen::Vector2f intersection(const Eigen::Vector2f& S, const Eigen::Vector2f& P, const Eigen::Vector2f& F, const Eigen::Vector2f& F1);

void Decoupage(std::vector<Eigen::Vector2f>&, const Mesh&, const Mesh&);
void Decoupage(std::vector<Eigen::Vector2f>&, const std::vector<Eigen::Vector2f>&, const std::vector<Eigen::Vector2f>&);

#endif