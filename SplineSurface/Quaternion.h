#ifndef QUATERNION_H
#define QUATERNION_H

#include "Mat4x4.h"
#include "Point.h"
#include <cmath>

class Quaternion
{
protected:

public:
	float a_;
	Point u_;

	Quaternion(){}
	Quaternion(float angle, float x, float y, float z) :a_(std::cosf(angle / 2)), u_(x * std::sinf(angle / 2), y * std::sinf(angle / 2), z * std::sinf(angle / 2)){}
	Quaternion(float angle, Point direction) : a_(std::cosf(angle / 2)), u_(direction * std::sinf(angle / 2)){}

	Quaternion& operator+=(const Quaternion& q)
	{
		a_ += q.a_;
		u_ += q.u_;

		return *this;
	}
	Quaternion& operator-=(const Quaternion& q)
	{
		a_ -= q.a_;
		u_ -= q.u_;

		return *this;
	}
	Quaternion& operator*=(const Quaternion& r)
	{
		Quaternion const p(*this);
		Quaternion const q(r);

		this->a_   = p.a_ * q.a_ - p.u_.x * q.u_.x - p.u_.y * q.u_.y - p.u_.z * q.u_.z;
		this->u_.x = p.a_ * q.u_.x + p.u_.x * q.a_ + p.u_.y * q.u_.z - p.u_.z * q.u_.y;
		this->u_.y = p.a_ * q.u_.y + p.u_.y * q.a_ + p.u_.z * q.u_.x - p.u_.x * q.u_.z;
		this->u_.z = p.a_ * q.u_.z + p.u_.z * q.a_ + p.u_.x * q.u_.y - p.u_.y * q.u_.x;
		return *this;

		return *this;
	}
	Quaternion& operator*=(float f)
	{
		a_ *= f;
		u_ *= f;
		return *this;
	}
	Quaternion& operator/=(float f)
	{
		//throw exeption
		//if (f == 0) throw std:: 
		a_ /= f;
		u_ /= f;
		return *this;
	}

	Quaternion operator+(const Quaternion& q)
	{
		return Quaternion(*this) += q;
	}
	Quaternion operator-(const Quaternion& q)
	{
		return Quaternion(*this) -= q;
	}
	Quaternion operator*(const Quaternion& q)
	{
		return Quaternion(*this) *= q;
	}
	Point operator*(const Point& p)
	{
		return Point(u_) *= p;
	}
	Quaternion operator*(float f)
	{
		return Quaternion(*this) *= f;
	}
	/*Quaternion operator/(const Quaternion& q)
	{
		return Quaternion(*this) /= q;
	}*/
	Quaternion operator/(float f)
	{
		return Quaternion(*this) /= f;
	}

	Quaternion operator-()
	{
		Quaternion q(*this);

		q.a_ = -q.a_;
		q.u_ = -q.u_;

		return q;
	}

	Quaternion conjugate()
	{
		Quaternion q(*this);
		q.u_ = -q.u_;

		return q;
	}
	float magnitude()
	{
		return std::sqrt(a_*a_ + u_.magnitude()*u_.magnitude());
	}
	Quaternion normalized()
	{
		return Quaternion(*this)/ this->magnitude();
	}
	Quaternion reciprocal()
	{
		return conjugate() / (magnitude() * magnitude());
	}

	Mat4x4 to_mat4()
	{
		Mat4x4 Result(0);
		Result.identity();

		float qxx(u_.Getx() * u_.Getx());
		float qyy(u_.Gety() * u_.Gety());
		float qzz(u_.Getz() * u_.Getz());
		float qxz(u_.Getx() * u_.Getz());
		float qxy(u_.Getx() * u_.Gety());
		float qyz(u_.Gety() * u_.Getz());
		float qwx(a_ * u_.Getx());
		float qwy(a_ * u_.Gety());
		float qwz(a_ * u_.Getz());



		Result[0] = 1 - 2 * (qyy +  qzz);
		Result[1] = 2 * (qxy + qwz);
		Result[2] = 2 * (qxz - qwy);

		Result[4] = 2 * (qxy - qwz);
		Result[5] = 1 - 2 * (qxx +  qzz);
		Result[6] = 2 * (qyz + qwx);

		Result[8] = 2 * (qxz + qwy);
		Result[9] = 2 * (qyz - qwx);
		Result[10] = 1 - 2 * (qxx +  qyy);

		return Result;
	}

	float getRealPart(){return a_;}
	Point getImgPart() {return u_;}

	void setRealPart(float a){ a_ = a;}
	void setImgPart (Point u){ u_ = u;}

	static Quaternion CreateFromAxisAngle(Point axis, float angle)
	{
		float halfAngle = angle * .5f;
		float s = std::sinf(halfAngle);
		Quaternion q;
		q.u_.Setx(axis.Getx() * s);
		q.u_.Sety(axis.Gety() * s);
		q.u_.Setz( axis.Getz() * s);
		q.a_ = std::cosf(halfAngle);
		return q;
	}

	static Quaternion eulerAngle(Point eulerAngle)
	{
		Quaternion q;
		Point c(std::cosf(eulerAngle.x * 0.5), std::cosf(eulerAngle.y * 0.5), std::cosf(eulerAngle.z * 0.5));
		Point s(std::sinf(eulerAngle.x * 0.5), std::sinf(eulerAngle.y * 0.5), std::sinf(eulerAngle.z * 0.5));

		q.a_ = c.x * c.y * c.z + s.x * s.y * s.z;
		q.u_.x = s.x * c.y * c.z - c.x * s.y * s.z;
		q.u_.y = c.x * s.y * c.z + s.x * c.y * s.z;
		q.u_.z = c.x * c.y * s.z - s.x * s.y * c.z;

		return q;
	}

	Point toEulerAngle()
	{
		float pitch = std::atan2(2 * u_.y* a_ - 2 * u_.x * u_.z, 1 - 2 * u_.y * u_.y - 2 * u_.z * u_.z);
		float roll = std::asin(2 * u_.x * u_.y + 2 * u_.z * a_);
		float yaw = std::atan2(2 * u_.x * a_ - 2 * u_.y * u_.z, 1 - 2 * u_.x * u_.x - 2 * u_.z * u_.z);

		if (u_.x * u_.y + u_.z * a_ == 0.5f)// (north pole)
		{
			pitch = 2 * std::atan2(u_.x, a_);
			roll = M_PI / 2;
			yaw = 0;
		}
		if( u_.x * u_.y + u_.z * a_ == -0.5f)// (south pole)
		{
			pitch = -2 * std::atan2(u_.x, a_);
			roll = -M_PI / 2;
			yaw = 0;
		}
		return Point(yaw, pitch, roll);
	}

	static Quaternion RotationBetweenVectors(Point start, Point dest)
	{
		start.normalize();
		dest.normalize();
		
		float cosTheta = Point::scalar(start, dest);
		Point rotationAxis;
		
		if (cosTheta < -1 + 0.001f)
		{
			// special case when vectors in opposite directions:
			// there is no "ideal" rotation axis
			// So guess one; any will do as long as it's perpendicular to start
			rotationAxis = Point::crossProduct(Point(0.0f, 0.0f, 1.0f), start);
			if ((rotationAxis.magnitude() * rotationAxis.magnitude()) < 0.01) // bad luck, they were parallel, try again!
				rotationAxis = Point::crossProduct(Point(1.0f, 0.0f, 0.0f), start);

			rotationAxis.normalize();
			return Quaternion::CreateFromAxisAngle(rotationAxis, M_PI);
		}
		
		
		rotationAxis = Point::crossProduct(start, dest);
		
		float s = sqrt((1 + cosTheta) * 2);
		float invs = 1 / s;
		
		Quaternion ret;
		ret.setImgPart(rotationAxis * invs);
		ret.a_ = s * 0.5f;

		return ret;
		
		
	}
	
};

#endif