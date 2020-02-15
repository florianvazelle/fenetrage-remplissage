#include "SI.h"

SI::SI(int height) {
    List<Node> emptyList;

	emptyList.push_front(Node());
	for (int i = 0; i < height; i++) {
		_list.push_back(emptyList);
	}
}

void SI::build(const std::vector<Eigen::Vector2f>& Poly) {
	Eigen::Vector2f v1, v2;
	int i, yPrev;

	yPrev = Poly[Poly.size() - 2][1];
	v1 = Poly.back();

	for (i = 0; i < Poly.size(); i++) {
		v2 = Poly[i];
		if (v1[1] != v2[1]) {
			if (v1[1] < v2[1])
				recordOfSide(v1, v2, yNext(i, Poly)); 
			else
				recordOfSide(v2, v1, yPrev); 
		}
		yPrev = v1[1];
		v1 = v2;
	}
}

int SI::yNext(int k, const std::vector<Eigen::Vector2f>& p) {
	int j; 

	if ((k + 1) > (p.size() - 1))
		j = 0;
	else
		j = k + 1;
	while (p[k][1] == p[j][1])
		if ((j + 1) > (p.size() - 1))
			j = 0;
		else
			j++;
	return (p[j][1]);
}

void SI::recordOfSide(const Eigen::Vector2f& minPoint, const Eigen::Vector2f& maxPoint, int yComp) {
	Node n;
	n.invertCoef = (float)(maxPoint[0] - minPoint[0]) / (maxPoint[1] - minPoint[1]);
	n.xmin = minPoint[0];

	if (maxPoint[1] < yComp)
		n.ymax = maxPoint[1] - 1;
	else
		n.ymax = maxPoint[1];
	_list[minPoint[1]].insertInTheList(n);
}