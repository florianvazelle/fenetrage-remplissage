#ifndef SI_H
#define SI_H

#include <vector>
#include <nanogui/nanogui.h>
#include "Node.h"
#include "CustomList.h"

class SI {
public:
    SI(int height);

	void build(const std::vector<Eigen::Vector2f>&);
	int yNext(int, const std::vector<Eigen::Vector2f>&);
	void recordOfSide(const Eigen::Vector2f&, const Eigen::Vector2f&, int);

    const List<Node>& operator[](const int idx) const { return _list[idx]; }
    List<Node>& operator[](const int idx) { return _list[idx]; }

private:
	std::vector<List<Node>> _list;
};

#endif