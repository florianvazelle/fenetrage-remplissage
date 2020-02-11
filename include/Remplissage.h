#ifndef Remplissage_H
#define Remplissage_H

#include <vector>
#include <nanogui/nanogui.h>

void RemplissageLCA(const std::vector<Eigen::Vector2f>& Poly, const std::vector<Eigen::Vector2f>& CR);
void RemplissageNaif(const std::vector<Eigen::Vector2f>& Poly, int width, int height);

#endif