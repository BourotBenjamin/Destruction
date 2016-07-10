#include "Destructor.h"




bool pointInside(Polyhedron_3 &polyhedron, K::Point_3 &query) {
	// Construct AABB tree with a KdTree
	Tree tree(faces(polyhedron).first, faces(polyhedron).second, polyhedron);
	tree.accelerate_distance_queries();
	// Initialize the point-in-polyhedron tester
	Point_inside inside_tester(tree);

	// Determine the side and return true if inside!
	return inside_tester(query) == CGAL::ON_BOUNDED_SIDE;
}

void Destructor::generateTriangulation3D(std::vector<std::shared_ptr<Objet>>& objets, std::shared_ptr<Objet>& baseObject, Polyhedron_3& baseObjectPoly)
{
	//std::vector<std::shared_ptr<Objet>> objets;
	triangulation.insert(pts3D.begin(), pts3D.end());
	if (triangulation.is_valid())
	{
		int i = 0;

		std::vector<float> vboPos;
		std::vector<unsigned int> eboIndices;
		std::vector<float> texcoords;
		std::vector<float> normals;
		std::vector<tinyobj::material_t> materials;

		Polyhedron_3 triangulationPoly;
		CGAL::convex_hull_3_to_polyhedron_3(triangulation, triangulationPoly);
		Nef_polyhedron triangulationNef(triangulationPoly);
		Nef_polyhedron baseObjectNef(baseObjectPoly);
		triangulationNef = triangulationNef.intersection(baseObjectNef);
		triangulationNef.convert_to_Polyhedron(triangulationPoly);
		triangulation.clear();
		auto vertice = triangulationNef.vertices_begin();
		while (vertice != triangulationNef.vertices_end())
		{
			triangulation.insert(vertice->point());
			++vertice;
		}
		auto pt3D = pts3D.begin();
		while (pt3D != pts3D.end())
		{
			if (pointInside(triangulationPoly, *pt3D))
				triangulation.insert(*pt3D);
			++pt3D;
		}
		if (triangulation.is_valid())
		{
			auto tetra = triangulation.finite_cells_begin();
			while (tetra != triangulation.finite_cells_end())
			{
				triangulationPoly.clear();
				triangulationPoly.make_tetrahedron(tetra->vertex(0)->point(), tetra->vertex(1)->point(), tetra->vertex(2)->point(), tetra->vertex(3)->point());
				float posX = 0, posY = 0, posZ = 0, nbVertices = 0;
				auto verticeTr = triangulationPoly.vertices_begin();
				while (verticeTr != triangulationPoly.vertices_end())
				{
					posX += CGAL::to_double(vertice->point().x());
					posY += CGAL::to_double(vertice->point().y());
					posZ += CGAL::to_double(vertice->point().z());
					nbVertices += 1.0f;
					++verticeTr;
				}
				posX /= nbVertices;
				posY /= nbVertices;
				posZ /= nbVertices;
				std::vector<float> normals;
				Objet* o = new Objet();
				o->alive = true;
				int indice = 0;
				auto facet = triangulationPoly.facets_begin();
				while (facet != triangulationPoly.facets_end())
				{
					K::Vector_3 normal = CGAL::Polygon_mesh_processing::compute_face_normal(facet, triangulationPoly);
					o->alive = true;
					auto vertice = facet->facet_begin();
					for (int i = 0; i < 3; i++, vertice++)
					{
						//K::Vector_3 normal = CGAL::Polygon_mesh_processing::compute_vertex_normal(vertice->vertex(), triangulationPoly);
						vboPos.push_back(CGAL::to_double(vertice->vertex()->point().x()) - posX);
						vboPos.push_back(CGAL::to_double(vertice->vertex()->point().y()) - posY);
						vboPos.push_back(CGAL::to_double(vertice->vertex()->point().z()) - posZ);
						eboIndices.push_back(indice);
						normals.push_back(CGAL::to_double(normal.x()));
						normals.push_back(CGAL::to_double(normal.y()));
						normals.push_back(CGAL::to_double(normal.z()));
						++indice;
					}
					++facet;
				}
				o->loadVerticesAndIndices(eboIndices, vboPos);
				o->reload();
				o->LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);
				o->position.x = 0;// posX;
				o->position.y = 0;//posY;
				o->position.z = 0;//posZ;
				objets.push_back(std::shared_ptr<Objet>(o));

				vboPos.clear();
				eboIndices.clear();
				normals.clear();
				++tetra;
			}
		}
		Polyhedron_3 polyTetra;
		baseObjectNef -= triangulationNef;
		int indice = 0;
		baseObject->alive = false;
		baseObjectNef.convert_to_Polyhedron(baseObjectPoly);
		float posX = 0, posY = 0, posZ = 0, nbVertices = 0;
		auto verticeP = baseObjectPoly.vertices_begin();
		while (verticeP != baseObjectPoly.vertices_end())
		{
			posX += CGAL::to_double(vertice->point().x());
			posY += CGAL::to_double(vertice->point().y());
			posZ += CGAL::to_double(vertice->point().z());
			nbVertices += 1.0f;
			++verticeP;
		}
		posX /= nbVertices;
		posY /= nbVertices;
		posZ /= nbVertices;
		auto facet = baseObjectPoly.facets_begin();
		while (facet != baseObjectPoly.facets_end())
		{
			K::Vector_3 normal = CGAL::Polygon_mesh_processing::compute_face_normal(facet, triangulationPoly);
			baseObject->alive = true;
			auto vertice = facet->facet_begin();
			for (int i = 0; i < 3; i++, vertice++)
			{
				//K::Vector_3 normal = CGAL::Polygon_mesh_processing::compute_vertex_normal(vertice->vertex(), triangulationPoly);
				vboPos.push_back(CGAL::to_double(vertice->vertex()->point().x()) - posX);
				vboPos.push_back(CGAL::to_double(vertice->vertex()->point().y()) - posY);
				vboPos.push_back(CGAL::to_double(vertice->vertex()->point().z()) - posZ);
				eboIndices.push_back(indice);
				normals.push_back(CGAL::to_double(normal.x()));
				normals.push_back(CGAL::to_double(normal.y()));
				normals.push_back(CGAL::to_double(normal.z()));
				++indice;
			}
			++facet;
		}


		if (baseObject->alive)
		{
			baseObject->position.x = 0;//posX;
			baseObject->position.y = 0;//posY;
			baseObject->position.z = 0;//posZ;
			baseObject->loadVerticesAndIndices(eboIndices, vboPos);
			baseObject->reload();
			baseObject->LoadByDatas(eboIndices, vboPos, normals, texcoords, std::string(""), materials, true);
			objets.push_back(baseObject);
		}
		vboPos.clear();
		eboIndices.clear();
		normals.clear();
	}
	else
		std::cout << "Invalid" << std::endl;



	pts3D.clear();
	triangulation.clear();
	//return objets;
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
