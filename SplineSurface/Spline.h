#pragma once
#include "Courbe.h"
#include <QtOpenGL\qglWidget>
class Spline :
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

	std::vector<int> m_T;		//Knot vector

	int p;						//degree of the spline
	int n;

	inline float alpha1(float t, int i, int p){ if ((m_T[i + p] - m_T[i]) == 0)return 0.f;			else return((t - m_T[i]) / ((m_T[i + p] - m_T[i]))*1.f); }
	inline float alpha2(float t, int i, int p){ if ((m_T[i + 1 + p] - m_T[i + 1]) == 0)return 0.f;	else return(((m_T[i + 1 + p] - t)) / ((m_T[i + 1 + p] - m_T[i + 1])*1.f)); }
	
	
public:
	Spline(std::string vertexShader, std::string fragmentShader);
	Spline(float timeMs);
	~Spline();

	void LoadSpline();
	void render(GLuint& program, GLuint shadowTex);
	void calculateT();						//compute Knot Vector
	void computePointInCourbe();			
	float DeBoor(int p, int i, float t);	//DeBoor algorithm to get controle point factor
	void load();							
	void drawCourbe2(Mat4x4 projection, Mat4x4 modelView, GLuint program);
	void drawCourbe(Mat4x4 projection, Mat4x4 modelView) {};

};

