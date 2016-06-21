#ifndef PATCHCOONS_H
#define PATCHCOONS_H

#include "surface.h"

class PatchCoons : public Surface
{
protected:
	std::vector<ListPts> control;

public:
	PatchCoons(std::string vertexShader, std::string fragmentShader);

	void addControl(ListPts& courbe, const Point& offset)
	{
		for (auto it = courbe.begin()+1; it != courbe.end(); ++it)
			(*it) += offset;
		control.push_back(courbe);
	}
	virtual void computePointInCourbe();																//compute points of courbe
	virtual void load();																			//load data into GPU
	virtual void drawCourbe(Mat4x4 projection, Mat4x4 modelView, GLuint program);
};

#endif