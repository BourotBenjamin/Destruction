#include "typedefs.h"
#include "Objet.h"
#include <iostream>
#include <stdio.h>

class Destructor
{
public:
	Destructor();
	DT3 triangulation;
	void generatePoints(float x, float y, float z, int maxDist, int nbPoints);
	std::vector<std::shared_ptr<Objet>> generateTriangulation3D();
	std::vector<Point_3> pts3D;
	~Destructor();
};

