#ifndef OBJET_H
#define OBJET_H

#define _USE_MATH_DEFINES
#include <iostream>
#include <stdio.h>
#include <list>
#include <vector>
#include "Point.h"
#include <math.h>
#include <cmath>
#include <memory>
#define GLEW_STATIC 1
#include <GL\glew.h>
#include "Shader.h"
#include "EsgiShader.h"
#include "Mat4x4.h"
#include <tiny_obj_loader.h>
#include <time.h>
#include "typedefs.h"


struct Particle
{
	//float Type;
	Point Pos;
	Point Vel;
	float LifetimeMillis;
};
struct MyEdge;
struct Face;
struct Vertex;

struct Triangle
{
	int indice[3];
};

struct Vertex
{
	int indice;
	Point position;
	std::vector<Face*> face;
	std::vector<MyEdge*> edge;
	Point vertexPoint;
	bool operator==(const Vertex& e)
	{
		if ((e.position == position ))
			return true;
		else
			return false;
	}
	bool operator==(const Point& p)
	{
		if ((p == position))
			return true;
		else
			return false;
	}
	Vertex operator+(const Vertex& e)
	{
		
	}

	
};

struct MyEdge
{
	int indice[2];
	Vertex* vertice[2];
	Vertex* verticeVoisin[2];
	Face* voisin[2];
	Point edgePoint;
	bool operator==(const MyEdge& e)
	{
		if ((e.vertice[0] == vertice[0] && e.vertice[1] == vertice[1]) || (e.vertice[1] == vertice[0] && e.vertice[0] == vertice[1]))
			return true;
		else 
			return false;
	}

	bool operator==(const MyEdge* e)
	{
		if ((e->vertice[0] == vertice[0] && e->vertice[1] == vertice[1]) || (e->vertice[1] == vertice[0] && e->vertice[0] == vertice[1]))
			return true;
		else
			return false;
	}
};

struct Face
{
	int indice[3];
	Vertex* vertice[3];
	MyEdge* edge[3];
	Face* voisin[3];
	Point facePoint;
	Vertex FP;
};

class Objet
{
protected:


	Mat4x4 worldMatrix;
	// mesh
	GLuint VBO = 0;
	GLuint EBO = 0;
	GLuint ElementCount;
	GLenum PrimitiveType;
	GLuint VAO = 0;
	// material
	GLuint textureObj;
	GLuint specularObj;
	GLuint normalObj;

	//Shader * m_shader;
	std::vector<Point> vertice;
	std::vector<Triangle> triangle;
	std::vector<Face> face;
	std::vector<MyEdge> edge;
	std::vector<Vertex> vertex;




	std::vector<uint32_t> eboIndices;
	std::vector<float> vboPos;

	std::vector<float> texCoords;

	bool isLookat = false;

	Point ambiant;
	Point diffuse;
	Point specular;
	float shininess;

	bool isDiffuseMap = false;
	bool isNormalMap = false;
	bool isSpecularMap = false;

	bool isParticul = false;

	float time;

	//particule system related
	bool isFirst;
	GLuint particuleUpdateVAO[2];
	GLuint particuleRenderVAO[2];
	GLuint particleBuffer[2];
	GLuint transformFeedback[2];
	uint32_t nbParticules;

	GLuint randomTex;

public:
	// transform
	Point position;
	Point rotation;
	Point scale;
	Point lookat;

	uint32_t currVB;
	uint32_t currTFB;
	uint32_t currSource;

	std::vector<Face> nface;
	std::vector<MyEdge> nedge;
	std::vector<Vertex> nvertex;

	std::vector<Vertex> facePoint;
	std::vector<Vertex> edgePoint;
	std::vector<Vertex> vertexPoint;

	Objet::Objet();
	Objet(std::string objfile, bool noNorma = false);
	Objet(std::vector<std::string>& v);
	Objet(std::string objfile, std::string normalMap);
	Objet(std::string objfile, uint32_t nbParticules, EsgiShader& programUpdate, std::string vertexShader, std::string fragmentShader, std::string geometryShader);
	~Objet();

	void reload();
	void reloadEBO();
	void Objet::loadVerticesAndIndices(const std::vector<unsigned int>& indices, const std::vector<float>& positions);
	void Objet::LoadByDatas(const std::vector<unsigned int>& indices, const std::vector<float>& positions, const std::vector<float>& normals, const std::vector<float>& texcoords, std::string& file, std::vector<tinyobj::material_t>& materials, bool noNormal);

	void render2(GLuint& program, GLuint shadowText, bool wireframe);
	void render3(GLuint& program, GLuint shadowText, bool wireframe);

	void setWorldMatrix(float* col0, float* col1, float* col2, float* col3)
	{
		worldMatrix = Mat4x4(col0, col1, col2, col3);
	}

	//void setupFeedback(GLuint program);
	void setupStruct();
	void setFacePoint();
	void setEdgePoint();
	void setVertexPoint();
	void subdivisionCatmullClark();
	void subdivisionLoop();
	void CalculKobbelt();
	void setPost(Point p){ position = p; }
	void setRot(Point p){ rotation = p; }
	void setScale(Point p){ scale = p; }
	void setLookAtPoint(Point p);
	void unDoneLookAt(){ isLookat = false; }
	void LoadFromObj(std::string objfile, bool noNorma = false);
	void render(GLuint& program, GLuint shadowText = 0);
	void renderCubeMap(GLuint& program);
	void renderNormalMap(GLuint& program);
	void renderBillBoardParticules(GLuint& program);

	void UpdateMaterial(GLuint& program);

	void UpdateTransform();
	void UpdateParticules(GLuint& program, float deltaTime);
	
	float RandomFloat()
	{
		float Max = RAND_MAX;
		return ((float)rand() / Max);
	}
	bool alive = true;

	std::shared_ptr<Polyhedron_3> generatePolyhedron();


};

#endif