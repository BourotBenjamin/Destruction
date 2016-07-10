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
	void generateTriangulation3D(std::vector<std::shared_ptr<Objet>>& objets, std::shared_ptr<Objet> baseObject, Polyhedron_3& baseObjectPoly);
	std::vector<K::Point_3> pts3D;
	~Destructor();
};

