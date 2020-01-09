#pragma once
#ifndef FenetreSH_H
#define FenetreSH_H

#include <vector>
#include <nanogui/nanogui.h>

bool coupe(Eigen::Vector2f S, std::vector<Eigen::Vector2f> P,
	std::vector<Eigen::Vector2f> F, std::vector<Eigen::Vector2f> F1);

Eigen::Vector2f intersection(Eigen::Vector2f S, std::vector<Eigen::Vector2f> P,
	std::vector<Eigen::Vector2f> F, std::vector<Eigen::Vector2f> F1);

bool visible(Eigen::Vector2f S,std::vector<Eigen::Vector2f> F, std::vector<Eigen::Vector2f> F1);

#endif