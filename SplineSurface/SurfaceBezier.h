#ifndef SURFACEBEZIER_H
#define SURFACEBEZIER_H

#include "Surface.h"
#include "Bezier.h"

class SurfaceBezier :
	public Surface
{
protected:
	std::vector<ListPts> listOfControl;
	std::vector<Bezier> listOfBezier;
	std::vector<Bezier> listOfVertical;
	

public:
	SurfaceBezier(std::string vertexShader, std::string fragmentShader);
	~SurfaceBezier();

	Point CastelJau(ListPts l, double step);
	Bezier computeBezier(ListPts l);

	void SurfaceCastelJau();

	void computePointInCourbe();															//compute points of courbe
	void load();																			//load data into GPU
	void drawCourbe(Mat4x4 projection, Mat4x4 modelView);
};

#endif