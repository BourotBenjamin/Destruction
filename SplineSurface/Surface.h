#ifndef SURFACE_H
#define SURFACE_H

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
#include "Courbe.h"
#include "Mat4x4.h"

typedef std::vector<Point> ListPts;

class Surface
{
protected:
	std::vector<ListPts> m_listCourbes;
	ListPts list;							//list of controle point
	double step;							//number of lines in courbe
	double ratio;
	ListPts m_pointInCourbe;				//list of courbe point
	Shader m_shader;						//Shader who will draw

	std::vector<float> m_toVBO;				//formated data to give to VBO for controle points
	std::vector<float> m_toVBOCourbe;		//formated data to give to VBO for courbe points

	int m_width;							//width of window				
	int m_height;							//height of window

	GLuint m_vboID;							//ID of the VBO passed to GPU
	GLuint m_vaoID;							//ID of the VAO passed to GPU
	GLuint m_triangleEBO;
	std::vector<GLushort> m_indicesTriangle;
	int m_sizeOfList;						//size of data in Byte of m_toVBO
	int m_sizeOfCourbe;						//size of data in Byte of m_toVBOCourbe
	int m_sizeIndiceTriangle;

	void genericSetVBO(std::vector<float>& coord, std::vector<ListPts> listOfPts, int& size);

public:
	Surface();
	Surface(std::string vertexShader, std::string fragmentShader);
	~Surface();

	void setWidthHeight(int width, int height)															//rescale point to be on window
	{
		m_width = width; m_height = height; computePointInCourbe();
	}

	virtual void setVBO();																						//fill m_toVBO correctly
	virtual void setVBOCourbe();																				//fill m_toVBOCourbe correctly
	virtual void setEBO();
	void setForm(ListPts l){ if (!m_listCourbes.empty())m_listCourbes.clear(); m_listCourbes.push_back(l); }
	virtual void computePointInCourbe();																//compute points of courbe
	virtual void load();																			//load data into GPU
	virtual void drawCourbe(Mat4x4 projection, Mat4x4 modelView);																		//display courbe

	void pushPoint(Point p){ list.push_back(p); };														//push a point in list
	void popPoint(){ list.pop_back(); };																//remove the last Point from list
	Point& getBack(){ return list.back(); }																//get the last Point in list
	Point& getPointAt(unsigned int i){ if (list.size() > 0 && list.size() > i) return list[i]; };		//return a point at the given index
	ListPts::iterator getBegin(){ return list.begin(); };												//get begin iterator from list
	ListPts::iterator getEnd(){ return list.end(); };													//get end iterator from list
	int  getLength(){ return list.size(); };															//get list size
	void clear(){ list.clear(); };																		//clear list


	//GETTERS
	double getStep(){ return step; };
	double getRatio(){ return ratio; };

	//SETTERS
	void setStep(double s){ if (s>0 && s< 50)step = s; };
	void setRatio(double r){ if (r>0 && r< 1)ratio = r; };
};

#endif