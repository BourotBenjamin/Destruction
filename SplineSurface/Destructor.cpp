#include "Destructor.h"



std::vector<std::shared_ptr<Objet>> Destructor::generateTriangulation3D()
{
	std::vector<std::shared_ptr<Objet>> objets;
	triangulation.insert(pts3D.begin(), pts3D.end());
	if (triangulation.is_valid())
	{
		int i = 0;

		auto tetra = triangulation.finite_cells_begin();
		while (tetra != triangulation.finite_cells_end())
		{
			Objet* o = new Objet();
			std::vector<float> vboPos;
			std::vector<unsigned int> eboIndices;

			vboPos.push_back(tetra->vertex(0)->point().x());
			vboPos.push_back(tetra->vertex(0)->point().y());
			vboPos.push_back(tetra->vertex(0)->point().z());
			vboPos.push_back(tetra->vertex(1)->point().x());
			vboPos.push_back(tetra->vertex(1)->point().y());
			vboPos.push_back(tetra->vertex(1)->point().z());
			vboPos.push_back(tetra->vertex(2)->point().x());
			vboPos.push_back(tetra->vertex(2)->point().y());
			vboPos.push_back(tetra->vertex(2)->point().z());
			vboPos.push_back(tetra->vertex(3)->point().x());
			vboPos.push_back(tetra->vertex(3)->point().y());
			vboPos.push_back(tetra->vertex(3)->point().z());

			eboIndices.push_back(0);
			eboIndices.push_back(1);
			eboIndices.push_back(2);

			eboIndices.push_back(0);
			eboIndices.push_back(1);
			eboIndices.push_back(3);

			eboIndices.push_back(0);
			eboIndices.push_back(2);
			eboIndices.push_back(3);

			eboIndices.push_back(1);
			eboIndices.push_back(2);
			eboIndices.push_back(3);

			++tetra;
			o->loadVerticesAndIndices(eboIndices, vboPos);
			o->reload();
			const std::vector<float> normals, texcoords;
			std::vector<tinyobj::material_t> materials;
			o->LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);
			objets.push_back(std::shared_ptr<Objet>(o));
		}
	}
	else
		std::cout << "Invalid" << std::endl;
	pts3D.clear();
	triangulation.clear();
	return objets;
}

float getRandomCoords(int maxDist, float middle)
{
	int dist = maxDist / 10;
	while (dist <= maxDist)
	{
		if (rand() % 3 > 1)
			return rand() % (dist * 2) + middle - dist;
		dist *= 1.5;
	}
	return rand() % (dist * 2) + middle - dist;
}

void Destructor::generatePoints(float x, float y, float z, int maxDist, int nbPoints)
{
	/*
	pts3D.push_back(Point_3(-1.0f, -1.0f, -1.0f));
	pts3D.push_back(Point_3(-1.0f,  1.0f, -1.0f));
	pts3D.push_back(Point_3(1.0f,  -1.0f, -1.0f));
	pts3D.push_back(Point_3(1.0f,   1.0f, -1.0f));
	pts3D.push_back(Point_3(-1.0f, -1.0f,  1.0f));
	pts3D.push_back(Point_3(-1.0f,  1.0f,  1.0f));
	pts3D.push_back(Point_3(1.0f,  -1.0f,  1.0f));
	pts3D.push_back(Point_3(1.0f,   1.0f,  1.0f));
	*/

	int i = 0;
	float x2, y2, z2;
	while (i < nbPoints)
	{
		++i;
		x2 = getRandomCoords(maxDist * 1000, x * 1000);
		y2 = getRandomCoords(maxDist * 1000, y * 1000);
		z2 = getRandomCoords(maxDist * 1000, z * 1000);
		pts3D.push_back(Point_3(x2 / 1000.0f, y2 / 1000.0f, z2 / 1000.0f));
	}
}

Destructor::Destructor()
{
}


Destructor::~Destructor()
{
}
