#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include "Math.hpp"

Vector2::Vector2() : x(0), y(0) {};
Vector2::Vector2(float X, float Y) : x(X), y(Y) {};
Vector2::Vector2(const Vector2 &o) : x(o.x), y(o.y) {};

Vector2 Vector2::operator+(const Vector2 &o) const
{
	return Vector2(x + o.x, y + o.y);
};

Vector2 Vector2::operator+(float f) const
{
	return Vector2(x + f, y + f);
};

Vector2 Vector2::operator-(const Vector2 &o) const
{
	return Vector2(x - o.x, y - o.y);
};

Vector2 Vector2::operator-(float f) const
{
	return Vector2(x - f, y - f);
};

Vector2 Vector2::operator*(const Vector2 &o) const
{
	return Vector2(x * o.x, y * o.y);
};

Vector2 Vector2::operator*(float f) const
{
	return Vector2(x * f, y * f);
};

Vector2 Vector2::operator/(const Vector2 &o) const
{
	return Vector2(x / ((o.x > 0) ? (o.x) : (1)), y / ((o.y > 0) ? (o.y) : (1)));
};

Vector2 Vector2::operator/(float f) const
{
	float Final = f != 0 ? f : 1;

	return Vector2(x / Final, y / Final);
};

void Vector2::operator+=(const Vector2 &o)
{
	*this = *this + o;
};

void Vector2::operator+=(float f)
{
	*this = *this + f;
};

void Vector2::operator-=(const Vector2 &o)
{
	*this = *this - o;
};

void Vector2::operator-=(float f)
{
	*this = *this - f;
};

void Vector2::operator/=(const Vector2 &o)
{
	*this = *this / o;
};

void Vector2::operator/=(float f)
{
	*this = *this / f;
};

void Vector2::operator*=(const Vector2 &o)
{
	*this = *this * o;
};

void Vector2::operator*=(float f)
{
	*this = *this * f;
};

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
};

bool Vector2::operator==(const Vector2 &o) const
{
	return x == o.x && y == o.y;
};

bool Vector2::operator!=(const Vector2 &o) const
{
	return !(*this == o);
};

bool Vector2::operator<(const Vector2 &o) const
{
	if(x < o.x)
		return true;

	if(y < o.y)
		return true;

	return false;
};

bool Vector2::operator>(const Vector2 &o) const
{
	if(x > o.x)
		return true;

	if(y > o.y)
		return true;

	return false;
};

bool Vector2::operator<=(const Vector2 &o) const
{
	return operator<(o) || operator==(o);
};

bool Vector2::operator>=(const Vector2 &o) const
{
	return operator>(o) || operator==(o);
};

float Vector2::Magnitude() const
{
	return sqrtf(x * x + y * y);
};

float Vector2::MagnitudeSquare() const
{
	return x * x + y * y;
};

Vector2 Vector2::Normalized() const
{
	Vector2 v = *this;
	float Mag = Magnitude();

	if(Mag <= 0) Mag = 1;

	v.x /= Mag;
	v.y /= Mag;

	return v;
};

void Vector2::Normalize()
{
	float Mag = Magnitude();

	if(Mag <= 0) Mag = 1;

	x /= Mag;
	y /= Mag;
};

bool Vector2::IsNormalized() const
{
	return (x == 0 && y == 0) || MagnitudeSquare() == 1;
};

float Vector2::Distance(const Vector2 &o) const
{
	return Vector2(o.x - x, o.y - y).Magnitude();
};

float Vector2::DistanceSquare(const Vector2 &o) const
{
	return Vector2(o.x - x, o.y - y).MagnitudeSquare();
};

float Vector2::Dot(const Vector2 &o) const
{
	return x * o.x + y * o.y;
};

Vector2 Vector2::Floor() const
{
	return Vector2(floorf(x), floorf(y));
};

Vector2 Vector2::Ceil() const
{
	return Vector2(ceilf(x), ceilf(y));
};

Vector2 Vector2::LinearInterpolate(const Vector2 &From, const Vector2 &To, const float &t)
{
	return From + (To - From) * t;
};

Vector2 Vector2::Rotate(const Vector2 &In, float Angle)
{
	float cosA(cosf(Angle)), sinA(sinf(Angle));

	return Vector2(In.x * cosA - In.y * sinA, In.y * cosA + In.x * sinA);
};

Vector3::Vector3() : x(0), y(0), z(0) {};

Vector3::Vector3(float X, float Y, float Z) : x(X), y(Y), z(Z) {};

Vector3::Vector3(const Vector3 &o) : x(o.x), y(o.y), z(o.z) {};

Vector3::Vector3(const Vector2 &v) : x(v.x), y(v.y), z(0) {};

Vector3::Vector3(const Vector2 &xy, float Z) : x(xy.x), y(xy.y), z(Z) {};

Vector3::Vector3(float X, const Vector2 &yz) : x(X), y(yz.x), z(yz.y) {};

Vector3 Vector3::operator+(const Vector3 &o) const
{
	return Vector3(x + o.x, y + o.y, z + o.z);
};

Vector3 Vector3::operator+(float f) const
{
	return Vector3(x + f, y + f, z + f);
};

Vector3 Vector3::operator-(const Vector3 &o) const
{
	return Vector3(x - o.x, y - o.y, z -  o.z);
};

Vector3 Vector3::operator-(float f) const
{
	return Vector3(x - f, y - f, z - f);
};

Vector3 Vector3::operator*(const Vector3 &o) const
{
	return Vector3(x * o.x, y * o.y, z * o.z);
};

Vector3 Vector3::operator*(float f) const
{
	return Vector3(x * f, y * f, z * f);
};

Vector3 Vector3::operator/(const Vector3 &o) const
{
	return Vector3(x / ((o.x > 0) ? (o.x) : (1)), y / ((o.y > 0) ? (o.y) : (1)), z / ((o.z > 0) ? (o.z) : (1)));
};

Vector3 Vector3::operator/(float f) const
{
	float Final = f != 0 ? f : 1;

	return Vector3(x / Final, y / Final, z / Final);
};

void Vector3::operator+=(const Vector3 &o)
{
	*this = *this + o;
};

void Vector3::operator+=(float f)
{
	*this = *this + f;
};

void Vector3::operator-=(const Vector3 &o)
{
	*this = *this - o;
};

void Vector3::operator-=(float f)
{
	*this = *this - f;
};

void Vector3::operator/=(const Vector3 &o)
{
	*this = *this / o;
};

void Vector3::operator/=(float f)
{
	*this = *this / f;
};

void Vector3::operator*=(const Vector3 &o)
{
	*this = *this * o;
};

void Vector3::operator*=(float f)
{
	*this = *this * f;
};

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
};

bool Vector3::operator==(const Vector3 &o) const
{
	return x == o.x && y == o.y && z == o.z;
};

bool Vector3::operator!=(const Vector3 &o) const
{
	return !(*this == o);
};

bool Vector3::operator<(const Vector3 &o) const
{
	if(x < o.x)
		return true;

	if(y < o.y)
		return true;

	if(z < o.z)
		return true;

	return false;
};

bool Vector3::operator>(const Vector3 &o) const
{
	if(x > o.x)
		return true;

	if(y > o.y)
		return true;

	if(z > o.z)
		return true;

	return false;
};

bool Vector3::operator<=(const Vector3 &o) const
{
	return operator<(o) || operator==(o);
};

bool Vector3::operator>=(const Vector3 &o) const
{
	return operator>(o) || operator==(o);
};

float Vector3::Magnitude() const
{
	return sqrtf(x * x + y * y + z * z);
};

float Vector3::MagnitudeSquare() const
{
	return x * x + y * y + z * z;
};

Vector3 Vector3::Normalized() const
{
	Vector3 v = *this;
	float Mag = Magnitude();

	if(Mag <= 0) Mag = 1;

	v.x /= Mag;
	v.y /= Mag;
	v.z /= Mag;

	return v;
};

void Vector3::Normalize()
{
	float Mag = Magnitude();

	if(Mag <= 0) Mag = 1;

	x /= Mag;
	y /= Mag;
	z /= Mag;
};

bool Vector3::IsNormalized() const
{
	return (x == 0 && y == 0 && z == 0) || MagnitudeSquare() == 1;
};

float Vector3::Distance(const Vector3 &o) const
{
	return Vector3(o.x - x, o.y - y, o.z - z).Magnitude();
};

float Vector3::DistanceSquare(const Vector3 &o) const
{
	return Vector3(o.x - x, o.y - y, o.z - z).MagnitudeSquare();
};

float Vector3::Dot(const Vector3 &o) const
{
	return x * o.x + y * o.y + z * o.z;
};

Vector3 Vector3::Cross(const Vector3 &o) const
{
	return Vector3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
};

Vector2 Vector3::ToVector2() const
{
	return Vector2(x, y);
};

Vector3 Vector3::Floor() const
{
	return Vector3(floorf(x), floorf(y), floorf(z));
};

Vector3 Vector3::Ceil() const
{
	return Vector3(ceilf(x), ceilf(y), ceilf(z));
};

Vector3 Vector3::NormalTri(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
	return (v3 - v1).Cross(v2 - v1).Normalized();
};

Vector3 Vector3::VectorAngles(const Vector3 &Vector)
{
	float Magnitude = Vector.Magnitude();

	return Vector3(Magnitude, acosf(Vector.z / Magnitude), atan2f(Vector.y, Vector.x));
};

Vector3 Vector3::AnglesVector(const Vector3 &Angles)
{
	float siny(sinf(Angles.y));

	return Vector3(Angles.x * siny * cosf(Angles.z), Angles.x * siny * sinf(Angles.z),
		Angles.x * cosf(Angles.y));
};

Vector3 Vector3::LinearInterpolate(const Vector3 &From, const Vector3 &To, const float &t)
{
	return From + (To - From) * t;
};

Vector4::Vector4() : x(0), y(0), z(0), w(0) {};
Vector4::Vector4(float X, float Y, float Z) : x(X), y(Y), z(Z), w(1) {};
Vector4::Vector4(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {};
Vector4::Vector4(const Vector4 &o) : x(o.x), y(o.y), z(o.z), w(o.w) {};
Vector4::Vector4(const Vector2 &xy, float Z, float W) : x(xy.x), y(xy.y), z(Z), w(W) {};
Vector4::Vector4(float X, const Vector2 &yz) : x(X), y(yz.x), z(yz.y), w(0) {};
Vector4::Vector4(float X, const Vector2 &yz, float W) : x(X), y(yz.x), z(yz.y), w(W) {};
Vector4::Vector4(float X, float Y, const Vector2 &zw) : x(X), y(Y), z(zw.x), w(zw.y) {};
Vector4::Vector4(const Vector2 &xy, const Vector2 &zw) : x(xy.x), y(xy.y), z(zw.x),
	w(zw.y) {};
Vector4::Vector4(const Vector3 &xyz, float W) : x(xyz.x), y(xyz.y), z(xyz.z), w(W) {};
Vector4::Vector4(float X, const Vector3 &yzw) : x(X), y(yzw.x), z(yzw.y), w(yzw.z) {};
Vector4::Vector4(const Vector2 &v) : x(v.x), y(v.y), z(0), w(0) {};
Vector4::Vector4(const Vector3 &v) : x(v.x), y(v.y), z(v.z), w(0) {};

Vector4 Vector4::operator+(const Vector4 &o) const
{
	return Vector4(x + o.x, y + o.y, z + o.z, w + o.w);
};

Vector4 Vector4::operator+(float f) const
{
	return Vector4(x + f, y + f, z + f, w + f);
};

Vector4 Vector4::operator-(const Vector4 &o) const
{
	return Vector4(x - o.x, y - o.y, z - o.z, w - o.w);
};

Vector4 Vector4::operator-(float f) const
{
	return Vector4(x - f, y - f, z - f, w - f);
};

Vector4 Vector4::operator*(const Vector4 &o) const
{
	return Vector4(x * o.x, y * o.y, z * o.z, w * o.w);
};

Vector4 Vector4::operator*(float f) const
{
	return Vector4(x * f, y * f, z * f, w * f);
};

Vector4 Vector4::operator/(const Vector4 &o) const
{
	return Vector4(x / ((o.x > 0) ? (o.x) : (1)), y / ((o.y > 0) ? (o.y) : (1)),
		z / ((o.z > 0) ? (o.z) : (1)), w / ((o.w > 0) ? (o.w) : (1)));
};

Vector4 Vector4::operator/(float f) const
{
	float Final = f != 0 ? f : 1;

	return Vector4(x / Final, y / Final, z / Final, w / Final);
};

void Vector4::operator+=(const Vector4 &o)
{
	*this = *this + o;
};

void Vector4::operator+=(float f)
{
	*this = *this + f;
};

void Vector4::operator-=(const Vector4 &o)
{
	*this = *this - o;
};

void Vector4::operator-=(float f)
{
	*this = *this - f;
};

void Vector4::operator/=(const Vector4 &o)
{
	*this = *this / o;
};

void Vector4::operator/=(float f)
{
	*this = *this / f;
};

void Vector4::operator*=(const Vector4 &o)
{
	*this = *this * o;
};

void Vector4::operator*=(float f)
{
	*this = *this * f;
};

Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
};

bool Vector4::operator==(const Vector4 &o) const
{
	return x == o.x && y == o.y && z == o.z && w == o.w;
};

bool Vector4::operator!=(const Vector4 &o) const
{
	return !(*this == o);
};

bool Vector4::operator<(const Vector4 &o) const
{
	if(x < o.x)
		return true;

	if(y < o.y)
		return true;

	if(z < o.z)
		return true;

	if(w < o.w)
		return true;

	return false;
};

bool Vector4::operator>(const Vector4 &o) const
{
	if(x > o.x)
		return true;

	if(y > o.y)
		return true;

	if(z > o.z)
		return true;

	if(w > o.w)
		return true;

	return false;
};

bool Vector4::operator<=(const Vector4 &o) const
{
	return operator<(o) || operator==(o);
};

bool Vector4::operator>=(const Vector4 &o) const
{
	return operator>(o) || operator==(o);
};

float Vector4::Magnitude() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
};

float Vector4::MagnitudeSquare() const
{
	return x * x + y * y + z * z + w * w;
};

Vector4 Vector4::Normalized() const
{
	Vector4 v(*this);
	float Mag = Magnitude();

	if(Mag <= 0) Mag = 1;

	v.x /= Mag;
	v.y /= Mag;
	v.z /= Mag;
	v.w /= Mag;

	return v;
};

void Vector4::Normalize()
{
	float Mag = Magnitude();

	if(Mag <= 0) Mag = 1;

	x /= Mag;
	y /= Mag;
	z /= Mag;
	w /= Mag;
};

bool Vector4::IsNormalized() const
{
	return (x == 0 && y == 0 && z == 0 && w == 0) || MagnitudeSquare() == 1;
};

float Vector4::Distance(const Vector4 &o) const
{
	return Vector4(o.x - x, o.y - y, o.z - z, o.w - w).Magnitude();
};

float Vector4::DistanceSquare(const Vector4 &o) const
{
	return Vector4(o.x - x, o.y - y, o.z - z, o.w - w).MagnitudeSquare();
};

float Vector4::Dot(const Vector4 &o) const
{
	return x * o.x + y * o.y + z * o.z + w * o.w;
};

Vector4 Vector4::Floor() const
{
	return Vector4(floorf(x), floorf(y), floorf(z), floorf(w));
};

Vector4 Vector4::Ceil() const
{
	return Vector4(ceilf(x), ceilf(y), ceilf(z), ceilf(w));
};

Vector4 Vector4::Cross(const Vector4 &o) const
{
	//TODO: W component
	return Vector4(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x, 0);
};

Vector4 Vector4::LinearInterpolate(const Vector4 &From, const Vector4 &To, const float &t)
{
	return From + (To - From) * t;
};

Vector2 Vector4::ToVector2() const
{
	return Vector2(x, y);
};

Vector3 Vector4::ToVector3() const
{
	return Vector3(x, y, z);
};

Rect::Rect()
{
	Left = Right = Top = Bottom = 0.0f;
};

Rect::Rect(const Rect &Other)
{
	this->Left = Other.Left;
	this->Right = Other.Right;
	this->Top = Other.Top;
	this->Bottom = Other.Bottom;
};

Rect::Rect(float Width, float Height)
{
	Left = -Width;
	Right = -Left;
	Top = -Height;
	Bottom = -Top;
};

Rect::Rect(float Left, float Right, float Top, float Bottom)
{
	this->Left = Left;
	this->Right = Right;
	this->Top = Top;
	this->Bottom = Bottom;
};

bool Rect::IsInside(const Vector2 &Position) const
{
	return(Position.x >= Left && Position.x <= Right &&
		Position.y >= Bottom && Position.y <= Top);
};

bool Rect::IsOutside(const Vector2 &Position) const
{
	return !IsInside(Position);
};

AxisAlignedBoundingBox::AxisAlignedBoundingBox()
{
	Clear();
};

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const AxisAlignedBoundingBox &Other)
{
	this->min = Other.min;
	this->max = Other.max;
};

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Vector3 &min, const Vector3 &max)
{
	this->min = min;
	this->max = max;
};

void AxisAlignedBoundingBox::Clear()
{
	min = Vector3(9999999.0f, 9999999.0f, 9999999.0f);
	max = -min;
};

void AxisAlignedBoundingBox::Calculate(const std::vector<Vector3> &Points)
{
	for(size_t i = 0; i < Points.size(); i++)
	{
		const Vector3& v = Points[i];

		if(v.x < min.x)
		{
			min.x = v.x;
		};

		if(v.y < min.y)
		{
			min.y = v.y;
		};

		if(v.z < min.z)
		{
			min.z = v.z;
		};

		if(v.x > max.x)
		{
			max.x = v.x;
		};

		if(v.y > max.y)
		{
			max.y = v.y;
		};

		if(v.z > max.z)
		{
			max.z = v.z;
		};
	};
};

void AxisAlignedBoundingBox::Calculate(const Vector3 &Point)
{
	if(Point.x < min.x)
	{
		min.x = Point.x;
	};

	if(Point.y < min.y)
	{
		min.y = Point.y;
	};

	if(Point.z < min.z)
	{
		min.z = Point.z;
	};

	if(Point.x > max.x)
	{
		max.x = Point.x;
	};

	if(Point.y > max.y)
	{
		max.y = Point.y;
	};

	if(Point.z > max.z)
	{
		max.z = Point.z;
	};
};

bool AxisAlignedBoundingBox::Intersects(const AxisAlignedBoundingBox &o) const
{
	return min.x <= o.max.x && min.y <= o.max.y && min.z <= o.max.z &&
		max.x >= o.min.x && max.y >= o.min.y && max.z >= o.min.z;
};

bool AxisAlignedBoundingBox::IsInside(const Vector3 &Position) const
{
	return Position.x >= min.x && Position.x <= max.x &&
		Position.y >= min.y && Position.y <= max.y &&
		Position.z >= min.z && Position.z <= max.z;
};

bool AxisAlignedBoundingBox::IsOutside(const Vector3 &Position) const
{
	return !IsInside(Position);
};

void AxisAlignedBoundingBox::Corners(Vector3 *Destination) const
{
	Destination[0] = min;
	Destination[1] = Vector3(max.x, min.y, min.z);
	Destination[2] = Vector3(max.x, max.y, min.z);	
	Destination[3] = Vector3(min.x, max.y, min.z);
	Destination[4] = Vector3(min.x, min.y, max.z);
	Destination[5] = Vector3(max.x, min.y, max.z);
	Destination[6] = max;
	Destination[7] = Vector3(min.x, max.y, max.z);
};

const double MathUtils::Pi = 3.14159265358979323846264338327950288419716939937510;
const float MathUtils::Epsilon = 0.000001f;
