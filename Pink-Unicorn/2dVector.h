#ifndef _2D_VECTOR_H
#define _2D_VECTOR_H

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <math.h>

struct Angle
{
	double a;
};

class Vector2D
{
    public:
    struct Default{};
    struct ByOx{};
    struct ByOy{};

    Vector2D(Default){x = 0.; y = 0.;}
    Vector2D(ByOx){x = 1.; y = 0.;}
    Vector2D(ByOy){x = 0.; y = 1.;}
		Vector2D(double angle){ x = cos(angle); y = sin(angle); }
    Vector2D(double x, double y) :x(x) , y(y) {}
	Vector2D(const BWAPI::TilePosition& pos) : x(double(pos.x)), y(double(pos.y)){}

    Vector2D operator+(const Vector2D &o) const {return Vector2D(x + o.x, y + o.y);}
    Vector2D operator-(const Vector2D &o) const {return Vector2D(x - o.x, y - o.y);}
    Vector2D operator*(double d) const { return Vector2D(x * d, y * d);}
    Vector2D operator/(double d) const { return Vector2D(x / d, y / d);}


    bool intersect(const Vector2D &o) const
    {
        double a = x / o.x;
        return   (o.y  > (a*y + 0.001)) || (o.y   < (a*y + 0.001));
    }
    double Dist(const Vector2D &o) const
    {
        double x1 = o.x - x;
        double y1 = o.y - y;
        return sqrt( x1*x1 + y1*y1);
    }

    double Dist()
    {
        return sqrt( x*x + y*y);
    }


    void Normalize() { Vector2D v{0.,0.};double d = Dist(v);x /= d ? d : 1.; y /= d ? d : 1.;}
    friend std::ostream & operator<<(std::ostream &o, Vector2D &v);

    double x,y;
};

inline std::ostream & operator<<(std::ostream &o, Vector2D &v) { o << v.x  << " "<< v.y; return o;}

inline double RandDouble(double a = 0., double b = 1.)
{
	double f = (double)rand() / RAND_MAX;
	return a + f * (b - a);
}

#endif

