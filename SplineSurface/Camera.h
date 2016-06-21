#ifndef DEF_CAMERA
#define DEF_CAMERA


#include "Point.h"
#include "Mat4x4.h"
#include "Quaternion.h"

// Classe

class Camera
{ 
private:

    float m_phi;
    float m_theta;
	Point m_orientation;

	Point m_axeVertical;
	Point m_deplacementLateral;

	Point m_position;
	Point m_pointCible;

	float FOV;

public:

    Camera();
    Camera(Point position, Point pointCible, Point axeVertical);
    ~Camera();

    void orienter(float xRel, float yRel);
    void deplacer(int i, float xrel, float yrel);
	void deplacer(int i[50], float xrel, float yrel, float deltaTime);
    void lookAt(Mat4x4 &modelview);
	Point getPos();
	void setPointcible(Point pointCible);
	void setPosition(Point position);
	Point getAxis();
};

#endif
