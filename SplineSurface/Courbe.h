#ifndef COURBE_H
#define COURBE_H

#define HEIGHT 600
#define WIDTH 800
#define POINT_SIZE_FOR_BEZIER 6
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
#include "Mat4x4.h"

typedef std::vector<Point> ListPts;

class Courbe
{
protected:
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
	int m_sizeOfList;						//size of data in Byte of m_toVBO
	int m_sizeOfCourbe;						//size of data in Byte of m_toVBOCourbe


	void genericSetVBO(std::vector<float>& coord, ListPts listOfPts, int& size);
	
public:
	Courbe(std::string vertexShader, std::string fragmentShader);
	Courbe(float timeMs);
	~Courbe();
	float timeToProceed;
	float deltaTime;
	void setWidthHeight(int width, int height)															//rescale point to be on window
	{ m_width = width; m_height = height; computePointInCourbe(); }

	void invert()
	{
		for (auto it = list.begin()+1; it != list.end(); ++it)
		{
			it->z = list.front().x;
			it->x = it->z;
			
		}
	}

	void setVBO();																						//fill m_toVBO correctly
	void setVBOCourbe();																				//fill m_toVBOCourbe correctly
	virtual void computePointInCourbe()=0;																//compute points of courbe
	virtual void load() = 0;																			//load data into GPU
	virtual void drawCourbe(Mat4x4 projection, Mat4x4 modelView) = 0;																		//display courbe
	virtual void drawCourbe2(Mat4x4 projection, Mat4x4 modelView, GLuint program){};
	void pushPoint(Point p){ list.push_back(p); };														//push a point in list
	void popPoint(){ list.pop_back(); };																//remove the last Point from list
	Point& getBack(){ return list.back(); }																//get the last Point in list
	Point& getPointAt(unsigned int i){ if (list.size() > 0 && list.size() > i) return list[i]; };		//return a point at the given index
	ListPts::iterator getBegin(){ return list.begin(); };												//get begin iterator from list
	ListPts::iterator getEnd(){ return list.end(); };													//get end iterator from list
	void insertFirst(const Point& p){ list.insert(list.begin(), p); }
	void insertLast(const Point& p){ list.insert(list.end(), p); }
	int  getLength(){ return list.size(); };															//get list size
	void clear(){ list.clear(); };																		//clear list


	ListPts getPointInCourbe(){ return m_pointInCourbe; }


	//GETTERS
	double getStep(){ return step; };
	double getRatio(){ return ratio; };

	//SETTERS
	void setStep(double s){ if (s>0 && s< 50)step = s; };
	void setRatio(double r){ if (r>0 && r< 1)ratio = r; };
};

#endif