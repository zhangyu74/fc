#ifndef TRILATERATION_H_INCLUDED
#define TRILATERATION_H_INCLUDED

#include    <vector>
#include    <math.h>

#define PI 3.14159265358979323846
#define DEG2RADs(x) ((PI / 180.000) * x)
#define RAD2DEGs(x) ((180.000 / PI) * x)
#define SQR(x) (x * x)

struct Point_XY
{
    double x, y;
};

struct Point_LonLat
{
    double lon, lat;
};

class Coordinate
{
public:
    double lat;

public:
    double lon;

public:
    double rad;

public:
    Coordinate(double lat, double lon, double rad)
    {
        this->lat = lat;
        this->lon = lon;
        this->rad = rad;
    }
};

class Trilateration
{
public:
    Point_LonLat triangulateLonLat(Coordinate a, Coordinate b, Coordinate c);
    Point_XY triangulateXY(Point_XY point1, Point_XY point2, Point_XY point3, double r1, double r2, double r3);
};


#endif // TRILATERATION_H_INCLUDED
