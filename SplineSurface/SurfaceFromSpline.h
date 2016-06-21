#ifndef SURFACEFROMSPLINE_H
#define SURFACEFROMSPLINE_H

#include "Surface.h"
class SurfaceFromSpline :
	public Surface
{
protected:
	Point p;
	

public:
	SurfaceFromSpline(std::string vertexShader, std::string fragmentShader);
	~SurfaceFromSpline();

	enum geometry { PLAN, SPHERE, CYLINDER, CONE, TORUS};

	void SimpleExtrude();
	void ExtrudeFromAxis();
	void ExtrudeFromSoul(std::shared_ptr<Courbe> soul);

	void computePointInCourbe();															//compute points of courbe
	void load();																			//load data into GPU
	void drawCourbe(Mat4x4 projection, Mat4x4 modelView);
};

#endif