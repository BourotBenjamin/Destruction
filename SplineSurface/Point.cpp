
#include "Point.h"
Point::Point()
{
	x = 0.;
	y = 0.;
	z = 0.;
}

Point::Point(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

bool Point::operator==(const Point &p) const
{
	if (this->x != p.x) return false;
	if (this->y != p.y) return false;
	if (this->z != p.z) return false;
	return true;
}

bool Point::operator!=(const Point &p) const
{
	if ((this->x == p.x) && (this->y == p.y) && (this->z == p.z))return false;
	return true;
}

bool Point::operator<(const Point &p) const
{
	if (this->x == p.x && this->y < p.y) return true;
	if (this->x < p.x) return true;
	return false;
}

Point& Point::operator+=(const Point& p)
{
	x += p.x;
	y += p.y;
	z += p.z;
	return *this;
}

Point operator+(const Point& p, const Point& p2)
{
	Point cpy(p);
	cpy += p2;
	return cpy;
}

Point& Point::operator*=(const Point& p)
{
	x *= p.x;
	y *= p.y;
	z *= p.z;
	return *this;
}

Point& Point::operator*=(const float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

Point operator*(const Point& p, const Point& p2)
{
	Point cpy(p);
	cpy *= p2;
	return cpy;
}

Point operator*(const Point& p, const float f)
{
	Point cpy(p);
	cpy *= f;
	return cpy;
}

Point& Point::operator-=(const Point& p)
{
	x -= p.x;
	y -= p.y;
	z -= p.z;
	return *this;
}

Point operator-(const Point& p, const Point& p2)
{
	Point cpy(p);
	cpy -= p2;
	return cpy;
}

Point& Point::operator/=(const Point& p)
{
	x /= p.x;
	y /= p.y;
	z /= p.z;
	return *this;
}

Point& Point::operator/=(const float f)
{
	x /= f;
	y /= f;
	z /= f;
	return *this;
}

Point operator/(const Point& p, const float f)
{
	Point cpy(p);
	cpy /= f;
	return cpy;
}

Point Point::getVect(const Point &P1, const Point &P2)
{
	float x = (P2.x - P1.x);
	float y = (P2.y - P1.y);
	float z = (P2.z - P1.z);
	Point v(x, y, z);
	return v;
}

float Point::scalar(const Point& vect1, const Point& vect2)
{
	return (vect1.x * vect2.x) + (vect1.y*vect2.y)+ (vect1.z * vect2.z);
}

Point Point::crossProduct(const Point& p1, const Point& p2)
{
	Point p = Point();
	p.x = p1.y*p2.z - p2.y*p1.z;
	p.y = p1.z*p2.x - p2.z*p1.x;
	p.z = p1.x*p2.y - p2.x*p1.y;
	return p;
}

void Point::normalize()
{
	(*this) /= std::sqrtf((x*x)+(y*y)+(z*z));
}


