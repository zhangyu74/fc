#include "Trilateration.h"

#include <stdlib.h>
#include <iostream> /**< std::cout std::cerr std::endl */
#include <iterator> /**< std::inserter */
#include <math.h>   /** sin */
#include <iomanip>  /** setprecision */

static std::vector<double> vectorMul(std::vector<double> a, std::vector<double> b)
{
    std::vector<double> result = {0, 0, 0};
    result[0] = a[0] * b[0];
    result[1] = a[1] * b[1];
    result[2] = a[2] * b[2];
    return result;
}

static std::vector<double> vectorMul(double a, std::vector<double> b)
{
    std::vector<double> result = {0, 0, 0};
    result[0] = a * b[0];
    result[1] = a * b[1];
    result[2] = a * b[2];
    return result;
}

static std::vector<double> vectorDiv(std::vector<double> a, std::vector<double> b)
{
    std::vector<double> result = {0, 0, 0};
    result[0] = a[0] / b[0];
    result[1] = a[1] / b[1];
    result[2] = a[2] / b[2];
    return result;
}

static std::vector<double> vectorDiv(std::vector<double> a, double b)
{
    std::vector<double> result = {0, 0, 0};
    result[0] = a[0] / b;
    result[1] = a[1] / b;
    result[2] = a[2] / b;
    return result;
}

static std::vector<double> vectorDiff(std::vector<double> a, std::vector<double> b)
{
    std::vector<double> result = {0, 0, 0};
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
    return result;
}

std::vector<double> vectorAdd(std::vector<double> a, std::vector<double> b)
{
    std::vector<double> result = {0, 0, 0};
    result[0] = a[0] + b[0];
    result[1] = a[1] + b[1];
    result[2] = a[2] + b[2];
    return result;
}

static double vectorDot(std::vector<double> a, std::vector<double> b)
{
    return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
}

std::vector<double> vectorCross(std::vector<double> a, std::vector<double> b)
{
    std::vector<double> result = {0, 0, 0};
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
    return result;
}

static double vectorNorm(std::vector<double> a)
{
    return sqrt(vectorDot(a, a));
}

static std::vector<double> vectorNormalize(std::vector<double> a)
{
    std::vector<double> result = {0, 0, 0};
    double mag = sqrt(vectorDot(a, a));
    result[0] = a[0] / mag;
    result[1] = a[1] / mag;
    result[2] = a[2] / mag;
    return result;
}

static float _norm(Point_XY p) // get the norm of a vector
{
    return pow(pow(p.x, 2) + pow(p.y, 2), .5);
}

Point_XY Trilateration::triangulateXY(Point_XY point1, Point_XY point2, Point_XY point3, double r1, double r2, double r3)
{
    Point_XY resultPose;
    //unit vector in a direction from point1 to point 2
    double p2p1Distance = pow(pow(point2.x - point1.x, 2) + pow(point2.y - point1.y, 2), 0.5);
    Point_XY ex = {(point2.x - point1.x) / p2p1Distance, (point2.y - point1.y) / p2p1Distance};
    Point_XY aux = {point3.x - point1.x, point3.y - point1.y};
    //signed magnitude of the x component
    double i = ex.x * aux.x + ex.y * aux.y;
    //the unit vector in the y direction.
    Point_XY aux2 = {point3.x - point1.x - i * ex.x, point3.y - point1.y - i * ex.y};
    Point_XY ey = {aux2.x / _norm(aux2), aux2.y / _norm(aux2)};
    //the signed magnitude of the y component
    double j = ey.x * aux.x + ey.y * aux.y;
    //coordinates
    double x = (pow(r1, 2) - pow(r2, 2) + pow(p2p1Distance, 2)) / (2 * p2p1Distance);
    double y = (pow(r1, 2) - pow(r3, 2) + pow(i, 2) + pow(j, 2)) / (2 * j) - i * x / j;
    //result coordinates
    double finalX = point1.x + x * ex.x + y * ey.x;
    double finalY = point1.y + x * ex.y + y * ey.y;
    resultPose.x = finalX;
    resultPose.y = finalY;
    return resultPose;
}

Point_LonLat Trilateration::triangulateLonLat(Coordinate a, Coordinate b, Coordinate c)
{

    //Constants
    double earthR = 6378137;
    double LatA = a.lat;
    double LonA = a.lon;
    double DistA = a.rad;
    double LatB = b.lat;
    double LonB = b.lon;
    double DistB = b.rad;
    double LatC = c.lat;
    double LonC = c.lon;
    double DistC = c.rad;

    //using authalic sphere
    //if using an ellipsoid this step is slightly different
    //Convert geodetic Lat/Long to ECEF xyz
    //   1. Convert Lat/Long to radians
    //   2. Convert Lat/Long(radians) to ECEF
    double xA = earthR * (cos(DEG2RADs(LatA)) * cos(DEG2RADs(LonA)));
    double yA = earthR * (cos(DEG2RADs(LatA)) * sin(DEG2RADs(LonA)));
    double zA = earthR * (sin(DEG2RADs(LatA)));

    double xB = earthR * (cos(DEG2RADs(LatB)) * cos(DEG2RADs(LonB)));
    double yB = earthR * (cos(DEG2RADs(LatB)) * sin(DEG2RADs(LonB)));
    double zB = earthR * (sin(DEG2RADs(LatB)));

    double xC = earthR * (cos(DEG2RADs(LatC)) * cos(DEG2RADs(LonC)));
    double yC = earthR * (cos(DEG2RADs(LatC)) * sin(DEG2RADs(LonC)));
    double zC = earthR * (sin(DEG2RADs(LatC)));

    //REQUIRE C++ 11
    std::vector<double> P1 = {xA, yA, zA};
    std::vector<double> P2 = {xB, yB, zB};
    std::vector<double> P3 = {xC, yC, zC};

    //ex = (P2 - P1)/(numpy.linalg.norm(P2 - P1))
    //i = dot(ex, P3 - P1)
    //ey = (P3 - P1 - i*ex)/(numpy.linalg.norm(P3 - P1 - i*ex))
    //ez = numpy.cross(ex,ey)
    //d = numpy.linalg.norm(P2 - P1)
    //j = dot(ey, P3 - P1)
    std::vector<double> ex = vectorDiv(vectorDiff(P2, P1), vectorNorm(vectorDiff(P2, P1)));
    double i = vectorDot(ex, vectorDiff(P3, P1)); //PROB
    std::vector<double> diff = vectorDiff(vectorDiff(P3, P1), vectorMul(i, ex));
    std::vector<double> ey = vectorDiv(diff, vectorNorm(diff));
    std::vector<double> ez = vectorCross(ex, ey);
    double d = vectorNorm(vectorDiff(P2, P1));
    double j = vectorDot(ey, vectorDiff(P3, P1));

    //from wikipedia
    //plug and chug using above values
    double x = (pow(DistA, 2) - pow(DistB, 2) + pow(d, 2)) / (2 * d);
    double y = ((pow(DistA, 2) - pow(DistC, 2) + pow(i, 2) + pow(j, 2)) / (2 * j)) - ((i / j) * x);

    //only one case shown here
    //double z = sqrt(pow(DistA, 2) - pow(x, 2) - pow(y, 2));
    double z = sqrt(fabs(pow(DistA, 2) - pow(x, 2) - pow(y, 2)));
    //triPt is an array with ECEF x,y,z of trilateration point
    //triPt = P1 + x*ex + y*ey + z*ez
    std::vector<double> triPt = vectorAdd(vectorAdd(P1, vectorMul(x, ex)), vectorAdd(vectorMul(y, ey), vectorMul(z, ez)));

    //convert back to lat/long from ECEF
    //convert to degrees
    double lat = RAD2DEGs(asin(triPt[2] / earthR));
    double lon = RAD2DEGs(atan2(triPt[1], triPt[0]));

    std::cout << std::fixed << std::setprecision(10) << lat << std::endl;
    std::cout << std::fixed << std::setprecision(10) << lon << std::endl;

    Point_LonLat result;
    result.lat = lat;
    result.lon = lon;
    return result;
}
