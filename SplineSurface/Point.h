#ifndef POINT_H
#define POINT_H

#include <list>

class Point
{
public:
	Point();
	Point(float x, float y, float z);
	bool operator==(const Point &p) const;
	bool operator!=(const Point &p) const;
	bool operator<(const Point &p) const;

	Point& operator+=(const Point& p);
	Point& operator*=(const Point& p);
	Point& operator*=(const float f);
	Point& operator-=(const Point& p);
	Point& operator/=(const Point& p);
	Point& operator/=(const float f);

	Point operator-()
	{
		Point p(*this);

		p.x *= -1;
		p.y *= -1;
		p.z *= -1;

		return p;
	}

	static Point crossProduct(const Point& p1, const Point& p2);
	void normalize();
	float magnitude()
	{
		return std::sqrtf((x*x) + (y*y) + (z*z));
	}

	static Point getVect(const Point &P1,const Point &P2);
	static float scalar(const Point& vect1, const Point& vect2);

	float Getx() const{ return x; }
	void Setx(float val) { x = val; }
	float Gety() const{ return y; }
	void Sety(float val) { y = val; }
	float Getz() const{ return z; }
	void Setz(float val) { z = val; }

	float x;
	float y;
	float z;

private:
	
};

Point operator+(const Point& p, const Point& p2);
Point operator*(const Point& p, const Point& p2);
Point operator*(const Point& p, const float f);
Point operator-(const Point& p, const Point& p2);
Point operator/(const Point& p, const Point& p2);
Point operator/(const Point& p, const float f);


#endif // POINT_H
