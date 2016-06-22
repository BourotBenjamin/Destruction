#include "Destructor.h"



std::vector<std::shared_ptr<Objet>> Destructor::generateTriangulation3D(std::shared_ptr<Objet> baseObject, Polyhedron_3& baseObjectPoly)
{
	std::vector<std::shared_ptr<Objet>> objets;
	triangulation.insert(pts3D.begin(), pts3D.end());
	if (triangulation.is_valid())
	{
		int i = 0;

		std::vector<float> vboPos;
		std::vector<unsigned int> eboIndices;

		Polyhedron_3 triangulationPoly;
		CGAL::convex_hull_3_to_polyhedron_3(triangulation, triangulationPoly);
		Nef_polyhedron triangulationNef(triangulationPoly);
		Nef_polyhedron baseObjectNef(baseObjectPoly);
		Nef_polyhedron singlePieceNef;
		
		Polyhedron_3 polyTetra;
		auto tetra = triangulation.finite_cells_begin();
		while (tetra != triangulation.finite_cells_end())
		{
			singlePieceNef.clear();
			polyTetra.clear();
			polyTetra.make_tetrahedron(tetra->vertex(0)->point(), tetra->vertex(1)->point(), tetra->vertex(2)->point(), tetra->vertex(3)->point());
			singlePieceNef = Nef_polyhedron(polyTetra);
			singlePieceNef = singlePieceNef - (singlePieceNef - baseObjectNef);
			if (singlePieceNef.is_simple())
				singlePieceNef.convert_to_Polyhedron(polyTetra);
			else
				std::cout << "Not Simple" << std::endl;

			int indice = 0;
			Objet* o = new Objet();
			o->alive = false;

			auto facet = polyTetra.facets_begin();
			while (facet != polyTetra.facets_end())
			{
				o->alive = true;
				auto vertice = facet->facet_begin();
				for (int i = 0; i < 3; i++, vertice++)
				{
					vboPos.push_back(CGAL::to_double(vertice->vertex()->point().x()));
					vboPos.push_back(CGAL::to_double(vertice->vertex()->point().y()));
					vboPos.push_back(CGAL::to_double(vertice->vertex()->point().z()));
					eboIndices.push_back(indice);
					++indice;
				}
				++facet;
			}

			++tetra;
			if (o->alive)
			{
				o->loadVerticesAndIndices(eboIndices, vboPos);
				o->reload();
				const std::vector<float> normals, texcoords;
				std::vector<tinyobj::material_t> materials;
				o->LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);
				objets.push_back(std::shared_ptr<Objet>(o));
			}
			vboPos.clear();
			eboIndices.clear();
		}
		baseObjectNef -= triangulationNef;

		int indice = 0;
		baseObject->alive = false;
		baseObjectNef.convert_to_Polyhedron(baseObjectPoly);
		auto facet = baseObjectPoly.facets_begin();
		while (facet != baseObjectPoly.facets_end())
		{
			baseObject->alive = true;
			auto vertice = facet->facet_begin();
			for (int i = 0; i < 3; i++, vertice++)
			{
				vboPos.push_back(CGAL::to_double(vertice->vertex()->point().x()));
				vboPos.push_back(CGAL::to_double(vertice->vertex()->point().y()));
				vboPos.push_back(CGAL::to_double(vertice->vertex()->point().z()));
				eboIndices.push_back(indice);
				++indice;
			}
			++facet;
		}
		if (baseObject->alive)
		{
			baseObject->loadVerticesAndIndices(eboIndices, vboPos);
			baseObject->reload();
			const std::vector<float> normals, texcoords;
			std::vector<tinyobj::material_t> materials;
			baseObject->LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);
			objets.push_back(baseObject);
		}
		vboPos.clear();
		eboIndices.clear();
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
		pts3D.push_back(K::Point_3(x2 / 1000.0f, y2 / 1000.0f, z2 / 1000.0f));
	}
}

Destructor::Destructor()
{
}


Destructor::~Destructor()
{
}
