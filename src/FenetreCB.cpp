#include <iostream>

#include "FenetreCB.h"

bool cyrusBeck(float x1,float y1,float x2,float y2, std::vector<Eigen::Vector2f> poly, std::vector<Eigen::Vector2f> normale,int nbSom) {

	float t, tinf, tsup;
	int dx, dy, wn, dn;
	Eigen::Vector2f C;
	int i, nbSeg;

	dx = int(x2 - x1);
	dy = int(y2 - x1);

	nbSeg = nbSom - 1;

	for (int i = 1; i <= nbSeg; i++) {

		C = poly[i];
		dn = dx * normale[i].x + dy * normale[i].y;
		wn = (x1 - C.x)* normale[i].x + (y1 - C.y)*normale[i].y;
		
			if (dn == 0) {
				return (wn>=0);
			}
			
			else {
				t = -(wn) / (dn);
					if (dn > 0) {
						if (t > tinf) {
							tinf = t;
						}

					}
					else {
						if (t < tsup) {
							tsup = t;
						}
					}
			}
	}

	if (tinf < tsup) {
		if ((tinf < 0) && (tsup > 1)) {
			return true;
		}
		else {
			if ((tinf > 1) || (tsup < 0)) {
				return false;
			}
			else {
				if (tinf < 0) {
					tinf = 0;
				}
				else {
					if (tsup > 1) {
						tsup = 1;
					}
				}

				x2 = x1 + dx * tsup;
				y2 = y1 + dy * tsup;
				x1 = x1 + dx * tinf;
				y1 = y1 + dy * tinf;
				return true;
			}
		}
	}



}