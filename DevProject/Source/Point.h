#ifndef __POINT_H__
#define __POINT_H__

#include "Defs.h"
#include <math.h>

template<class TYPE>
class Point
{
public:

	TYPE x, y;

	Point()
	{}

	Point(const Point<TYPE>& v)
	{
		this->x = v.x;
		this->y = v.y;
	}

	Point(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;
	}

	Point& create(const TYPE& x, const TYPE& y)
	{
		this->x = x;
		this->y = y;

		return(*this);
	}

	Point operator -(const Point &v) const
	{
		p2Vector2 r;

		r.x = x - v.x;
		r.y = y - v.y;

		return(r);
	}

	Point operator + (const Point &v) const
	{
		p2Vector2 r;

		r.x = x + v.x;
		r.y = y + v.y;

		return(r);
	}

	const Point& operator -=(const Point &v)
	{
		x -= v.x;
		y -= v.y;

		return(*this);
	}

	const Point& operator +=(const Point &v)
	{
		x += v.x;
		y += v.y;

		return(*this);
	}

	const Point& operator *= (const Point &v) {

		x *= v.x;
		y *= v.y;

		return(*this);
	}

	const Point& operator *= (const float &v) {

		x *= v;
		y *= v;

		return(*this);
	}

	bool operator ==(const Point& v) const
	{
		return (x == v.x && y == v.y);
	}

	bool operator !=(const Point& v) const
	{
		return (x != v.x || y != v.y);
	}

	bool IsZero() const
	{
		return (x == 0 && y == 0);
	}

	Point& SetToZero()
	{
		x = y = 0;
		return(*this);
	}

	Point& Negate()
	{
		x = -x;
		y = -y;

		return(*this);
	}

	TYPE DistanceTo(const Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return sqrtf((fx*fx) + (fy*fy));
	}

	TYPE DistanceNoSqrt(const Point& v) const
	{
		TYPE fx = x - v.x;
		TYPE fy = y - v.y;

		return (fx*fx) + (fy*fy);
	}

	TYPE DistanceManhattan(const Point& v) const
	{
		return abs(v.x - x) + abs(v.y - y);
	}
};

typedef Point<int> iPoint;
typedef Point<float> fPoint;

#endif // __POINT_H__