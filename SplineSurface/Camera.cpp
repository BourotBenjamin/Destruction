#include "Camera.h"


Camera::Camera() : m_phi(0.0), m_theta(0.0), m_orientation(), m_axeVertical(0, 0, 1), m_deplacementLateral(), m_position(), m_pointCible()
{
	
}

Camera::Camera(Point position, Point pointCible, Point axeVertical) : m_phi(0.0), m_theta(0.0), m_orientation(), m_axeVertical(axeVertical), m_deplacementLateral(),m_position(position), m_pointCible(pointCible)
{
    // Actualisation du point ciblé

    setPointcible(pointCible);
}

Camera::~Camera()
{

}

void Camera::orienter(float xRel, float yRel)
{
    
	const float ROTATION_SPEED = 0.005f;

	Point Axis = Point::crossProduct(m_pointCible - m_position, m_axeVertical);
	Axis.normalize();

	// Rotate around the y axis
	Quaternion rAroundLateral = Quaternion(-yRel * ROTATION_SPEED, Axis);
	// Rotate around the x axis
	Quaternion rAroundUp = Quaternion(-xRel * ROTATION_SPEED, m_axeVertical);

	auto rotQuat = (rAroundUp *rAroundLateral);

	m_orientation = rotQuat.to_mat4() * m_orientation;
	m_orientation.normalize();
	
    // Calcul de la normale	
    m_deplacementLateral = Point::crossProduct(m_axeVertical, m_orientation);
    m_deplacementLateral.normalize();
	

    // Calcul du point ciblé pour OpenGL

    m_pointCible = m_position + m_orientation;
}

void Camera::deplacer(int i, float xrel, float yrel)
{
    // Gestion de l'orientation

    orienter(xrel, yrel);

	const float sensitivity = 0.9f;
    // Avancée de la caméra

    if(i==0)
    {
		m_position = m_position + m_orientation*sensitivity;//  Point::crossProduct(m_deplacementLateral, m_orientation); //m_orientation;
		m_pointCible = m_position + m_orientation*sensitivity;
    }


    // Recul de la caméra

    if(i==1)
    {
		m_position = m_position - m_orientation*sensitivity;//  Point::crossProduct(m_deplacementLateral, m_orientation); //m_orientation;
		m_pointCible = m_position + m_orientation*sensitivity;
    }


    // Déplacement vers la gauche

    if(i==2)
    {
		m_position = m_position + m_deplacementLateral*sensitivity;
		m_pointCible = m_position + m_orientation*sensitivity;
    }


    // Déplacement vers la droite

    if(i==3)
    {
		m_position = m_position - m_deplacementLateral*sensitivity;
		m_pointCible = m_position + m_orientation*sensitivity;
    }

}

void Camera::deplacer(int i[50], float xrel, float yrel, float deltaTime)
{

	orienter(xrel, yrel);

	const float velocity = 30.f;

	auto sensitivity = velocity * deltaTime;
	// Avancée de la caméra
	Point tmp(0,0,0);
	if (i[0] == 1)
		tmp += m_orientation;

	// Recul de la caméra
	if (i[1] == 1)
		tmp -= m_orientation;

	// Déplacement vers la gauche
	if (i[2] == 1)
		tmp += m_deplacementLateral;
	
	// Déplacement vers la droite
	if (i[3] == 1)
		tmp -= m_deplacementLateral;

	if (i[4] == 1)
		tmp -= Point::crossProduct(m_deplacementLateral, m_orientation);

	if (i[5] == 1)
		tmp += Point::crossProduct(m_deplacementLateral, m_orientation);
	if (tmp.x != 0 || tmp.y != 0 || tmp.z != 0)
	{
		tmp.normalize();
		m_position += tmp* sensitivity;
		m_pointCible = m_position + m_orientation*sensitivity;
	}
	
}

void Camera::lookAt(Mat4x4 &modelview)
{
    modelview.lookAt(m_position, m_pointCible, m_axeVertical);
}

Point Camera::getPos()
{
	return m_position;
}

void Camera::setPointcible(Point pointCible)
{
    // Calcul du vecteur orientation

    m_orientation = pointCible - m_position;
    m_orientation.normalize();


    // Si l'axe vertical est l'axe X

    if(m_axeVertical.Getx() == 1.0)
    {
        // Calcul des angles

		m_phi = std::asin(m_orientation.Getx());
		m_theta = std::acos(m_orientation.Gety() / std::cos(m_phi));

		if (m_orientation.Gety() < 0)
            m_theta *= -1;
    }


    // Si c'est l'axe Y

	else if (m_axeVertical.Gety() == 1.0)
    {
        // Calcul des angles

		m_phi = std::asin(m_orientation.Gety());
		m_theta = std::acos(m_orientation.Getz() / std::cos(m_phi));

		if (m_orientation.Getz() < 0)
            m_theta *= -1;
    }


    // Sinon c'est l'axe Z

    else
    {
        // Calcul des angles

		m_phi = std::asin(m_orientation.Getx());
		m_theta = std::acos(m_orientation.Getz() / std::cos(m_phi));

		if (m_orientation.Getz() < 0)
            m_theta *= -1;
    }


    // Conversion en degrés

    m_phi = m_phi * 180 / M_PI;
    m_theta = m_theta * 180 / M_PI;
}

void Camera::setPosition(Point position)
{
    m_position = position;

    // Actualisation du point ciblé
    m_pointCible = m_position + m_orientation;
}

Point Camera::getAxis()
{
	return Point::crossProduct(m_orientation - m_position, m_axeVertical);
}

