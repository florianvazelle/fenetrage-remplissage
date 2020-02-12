#ifndef Remplissage_H
#define Remplissage_H

#include <vector>
#include <nanogui/nanogui.h>
#include <math.h>
#include "GLShader.h"

void RemplissageLCA(const std::vector<Eigen::Vector2f>& Poly, const std::vector<Eigen::Vector2f>& CR);
void RemplissageNaif(const std::vector<Eigen::Vector2f>& Poly, int width, int height);
void initRemplissage(uint32_t shader, int width, int height);
void displayRemplissage(int width, int height);
void destroyRemplissage();

void Fill(std::vector<Eigen::Vector2f> Poly, int width, int height);

#endif