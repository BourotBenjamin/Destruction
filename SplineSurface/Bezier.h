#pragma once

#include "Courbe.h"

class Bezier :
	public Courbe
{
protected:

	Mat4x4 worldMatrix;
	// mesh
	GLuint VBO;
	GLuint EBO;
	GLuint ElementCount;
	GLenum PrimitiveType;
	GLuint VAO;
	// material
	GLuint textureObj;
	GLuint specularObj;


	bool isLookat = false;

	Point ambiant;
	Point diffuse;
	Point specular;
	float shininess;

	bool isDiffuseMap = false;
	bool isNormalMap = false;
	bool isSpecularMap = false;


	// transform
	Point position;
	Point rotation;
	Point scale;
	Point lookat;

	public:
		Bezier(std::string vertexShader, std::string fragmentShader);
		Bezier(float timeMs);
		~Bezier();


		void setControlPts(ListPts l){ list = l; }
		Point getPtsInCourbeAt(int i){ return m_pointInCourbe[i]; }
		void computePointInCourbe();
		void load();
		void drawCourbe(Mat4x4 projection, Mat4x4 modelView);
		void drawCourbe2(Mat4x4 projection, Mat4x4 modelView, GLuint program);
		void drawCourbeSurface(Mat4x4 projection, Mat4x4 modelView, bool points = true, bool poly = true);

		Point CastelJau(double step);			//CastelJau algorithm to compute barycenters
};

