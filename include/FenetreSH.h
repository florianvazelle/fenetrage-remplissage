#ifndef FenetreSH_H
#define FenetreSH_H

#include <vector>
#include <nanogui/nanogui.h>
#include "Mesh.h"

std::vector<Eigen::Vector2f> Decoupage(Mesh PL, Mesh PW);
std::vector<Eigen::Vector2f> Decoupage(std::vector<Eigen::Vector2f> PL, std::vector<Eigen::Vector2f> PW);

#endif