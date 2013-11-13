#pragma once

class Vector3;
class Vector4;

class Vector2
{
public:
	float x, y;
	Vector2();
	Vector2(float x, float y);
	Vector2(const Vector2 &o);

	Vector2 operator+(const Vector2 &o) const;
	Vector2 operator+(float f) const;

	Vector2 operator-(const Vector2 &o) const;
	Vector2 operator-(float f) const;

	Vector2 operator*(const Vector2 &o) const;
	Vector2 operator*(float f) const;

	Vector2 operator/(const Vector2 &o) const;
	Vector2 operator/(float f) const;

	void operator+=(const Vector2 &o);
	void operator+=(float f);

	void operator-=(const Vector2 &o);
	void operator-=(float f);

	void operator/=(const Vector2 &o);
	void operator/=(float f);

	void operator*=(const Vector2 &o);
	void operator*=(float f);

	Vector2 operator-() const;

	bool operator==(const Vector2 &o) const;
	bool operator!=(const Vector2 &o) const;

	bool operator<(const Vector2 &o) const;
	bool operator>(const Vector2 &o) const;
	bool operator<=(const Vector2 &o) const;
	bool operator>=(const Vector2 &o) const;

	float Magnitude() const;
	float MagnitudeSquare() const;
	Vector2 Normalized() const;
	void Normalize();
	bool IsNormalized() const;
	float Distance(const Vector2 &o) const;
	float DistanceSquare(const Vector2 &o) const;
	float Dot(const Vector2 &o) const;

	//Returns a floorf/ceilf vector
	Vector2 Floor() const;
	//Returns a floorf/ceilf vector
	Vector2 Ceil() const;

	static Vector2 LinearInterpolate(const Vector2 &From, const Vector2 &To, const float &t);
	/*
		Special case so we can rotate something on the virtual "Z" axis
		without converting to Vector4 and transforming with a Matrix4x4

		Angle is in radians
	*/
	static Vector2 Rotate(const Vector2 &In, float Angle);
};

class Vector3
{
public:
	float x, y, z;
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(const Vector3 &o);
	Vector3(const Vector2 &v);
	Vector3(const Vector2 &xy, float z);
	Vector3(float x, const Vector2 &yz);

	Vector3 operator+(const Vector3 &o) const;
	Vector3 operator+(float f) const;

	Vector3 operator-(const Vector3 &o) const;
	Vector3 operator-(float f) const;

	Vector3 operator*(const Vector3 &o) const;
	Vector3 operator*(float f) const;

	Vector3 operator/(const Vector3 &o) const;
	Vector3 operator/(float f) const;

	void operator+=(const Vector3 &o);
	void operator+=(float f);

	void operator-=(const Vector3 &o);
	void operator-=(float f);

	void operator/=(const Vector3 &o);
	void operator/=(float f);

	void operator*=(const Vector3 &o);
	void operator*=(float f);

	bool operator==(const Vector3 &o) const;
	bool operator!=(const Vector3 &o) const;

	bool operator<(const Vector3 &o) const;
	bool operator>(const Vector3 &o) const;
	bool operator<=(const Vector3 &o) const;
	bool operator>=(const Vector3 &o) const;

	Vector3 operator-() const;

	float Magnitude() const;
	float MagnitudeSquare() const;
	Vector3 Normalized() const;
	void Normalize();
	bool IsNormalized() const;
	float Distance(const Vector3 &o) const;
	float DistanceSquare(const Vector3 &o) const;
	float Dot(const Vector3 &o) const;
	Vector3 Cross(const Vector3 &o) const;
	Vector2 ToVector2() const;

	//Returns a floorf/ceilf vector
	Vector3 Floor() const;
	//Returns a floorf/ceilf vector
	Vector3 Ceil() const;

	static Vector3 NormalTri(const Vector3 &v1, const Vector3 &v2,
		const Vector3 &v3);
	static Vector3 VectorAngles(const Vector3 &Vector);
	static Vector3 AnglesVector(const Vector3 &Angles);
	static Vector3 LinearInterpolate(const Vector3 &From, const Vector3 &To, const float &t);
};

class Vector4
{
public:
	float x, y, z, w;
	Vector4();
	Vector4(float x, float y, float z);
	Vector4(float x, float y, float z, float w);
	Vector4(const Vector4 &o);
	Vector4(const Vector2 &v);
	Vector4(const Vector2 &xy, float z, float w);
	Vector4(float x, const Vector2 &yz);
	Vector4(float x, const Vector2 &yz, float w);
	Vector4(float x, float y, const Vector2 &zw);
	Vector4(const Vector2 &xy, const Vector2 &zw);
	Vector4(const Vector3 &v);
	Vector4(const Vector3 &xyz, float w);
	Vector4(float x, const Vector3 &yzw);

	Vector4 operator+(const Vector4 &o) const;
	Vector4 operator+(float f) const;

	Vector4 operator-(const Vector4 &o) const;
	Vector4 operator-(float f) const;

	Vector4 operator*(const Vector4 &o) const;
	Vector4 operator*(float f) const;

	Vector4 operator/(const Vector4 &o) const;
	Vector4 operator/(float f) const;

	void operator+=(const Vector4 &o);
	void operator+=(float f);

	void operator-=(const Vector4 &o);
	void operator-=(float f);

	void operator/=(const Vector4 &o);
	void operator/=(float f);

	void operator*=(const Vector4 &o);
	void operator*=(float f);

	bool operator==(const Vector4 &o) const;
	bool operator!=(const Vector4 &o) const;

	bool operator<(const Vector4 &o) const;
	bool operator>(const Vector4 &o) const;
	bool operator<=(const Vector4 &o) const;
	bool operator>=(const Vector4 &o) const;

	Vector4 operator-() const;

	float Magnitude() const;
	float MagnitudeSquare() const;
	Vector4 Normalized() const;
	void Normalize();
	bool IsNormalized() const;
	float Distance(const Vector4 &o) const;
	float DistanceSquare(const Vector4 &o) const;
	float Dot(const Vector4 &o) const;
	Vector4 Cross(const Vector4 &o) const;
	Vector2 ToVector2() const;
	Vector3 ToVector3() const;

	//Returns a floorf/ceilf vector
	Vector4 Floor() const;
	//Returns a floorf/ceilf vector
	Vector4 Ceil() const;

	static Vector4 LinearInterpolate(const Vector4 &From, const Vector4 &To, const float &t);
};

class Rect
{
public:
	float Left, Right, Top, Bottom;

	Rect();
	Rect(const Rect &Other);
	Rect(float Width, float Height);
	Rect(float Left, float Right, float Top, float Bottom);

	bool IsInside(const Vector2 &Position) const;
	bool IsOutside(const Vector2 &Position) const;

	Rect operator+(const Rect &Other) const
	{
		return Rect(Left + Other.Left, Right + Other.Right, Top + Other.Top, Bottom + Other.Bottom);
	};

	Rect operator-(const Rect &Other) const
	{
		return Rect(Left - Other.Left, Right - Other.Right, Top - Other.Top, Bottom - Other.Bottom);
	};

	Vector2 Size() const
	{
		return Vector2(Right - Left, Bottom - Top);
	};

	Vector2 Position() const
	{
		return Vector2(Left, Top);
	};
	
	Vector2 ToFullSize() const
	{
		return Size() + Position();
	};
};

class AxisAlignedBoundingBox
{
public:
	Vector3 min, max;
	AxisAlignedBoundingBox();
	AxisAlignedBoundingBox(const AxisAlignedBoundingBox &Other);
	AxisAlignedBoundingBox(const Vector3 &min, const Vector3 &max);

	void Clear();
	void Calculate(const std::vector<Vector3> &Points);
	void Calculate(const Vector3 &Point);
	bool Intersects(const AxisAlignedBoundingBox &o) const;
	bool IsInside(const Vector3 &Position) const;
	bool IsOutside(const Vector3 &Position) const;
	//Requires 8 vertices
	void Corners(Vector3 *Destination) const;
};

class MathUtils
{
public:
	static const double Pi;
	static const float Epsilon;

	/*!
		Clamps a value between two min/max values
		\param Value the value we want to Clamp
		\param Min the minimum value
		\param Max the maximum value
	*/
	static inline float Clamp(float Value, float Min = 0.0f, float Max = 1.0f)
	{
		return Value < Min ? Min : Value > Max ? Max : Value;
	};

	/*!
		Rounds a value so it reaches the next positive integer based on the decimal value of the Value
		E.g., for a value of 0.4, you'll get 0, for a value of 0.5 you'll get 1
		\param Value the value to round
		\return The rounded value
	*/
	static inline float Round(float Value)
	{
		return floorf(Value + 0.5f);
	};

	static inline float Min(float A, float B)
	{
		return A > B ? B : A;
	};

	static inline float Max(float A, float B)
	{
		return A > B ? A : B;
	};

	/*!
		Converts a degree angle to a radian angle
		\param Value the angle to convert
		\return the converted angle
	*/
	static inline float DegToRad(float Value)
	{
		return Value * 0.0174444444444444f;
	};

	/*!
		Converts a radian angle to a degree angle
		\param Value the angle to convert
		\return the converted angle
	*/
	static inline float RadToDeg(float Value)
	{
		return Value * 57.32484076433121f;
	};
};
