#pragma once

#include "Trilateration.h"

#include <stdlib.h>
#include <stddef.h>
#include <cstring>  /**< memset */
#include <vector>   /**< std::vector */
#include <iostream> /**< std::cout std::cerr std::endl */
#include <fstream>  /**< std::ifstream */
#include <assert.h> /** assert */
#include "rapidjson/document.h"         /**< rapidjson::Document */
#include "rapidjson/istreamwrapper.h"   /**< rapidjson::IStreamWrapper */

class FullCardDataParser
{
public:
    FullCardDataParser(rapidjson::Document &config)
        : m_Offset(0)
    {
        //init
        memset(m_buffer, 0, sizeof(m_buffer));
        m_r1 = 0;
        m_r2 = 0;
        m_r3 = 0;
        m_totalcount = 0;
        m_lostcount = 0;
        memset(&m_finalPose, 0, sizeof(m_finalPose));
        memset(&m_finalonlat, 0, sizeof(m_finalonlat));

        double x0 = config["x0"].GetDouble();
        double y0 = config["y0"].GetDouble();
        double x1 = config["x1"].GetDouble();
        double y1 = config["y1"].GetDouble();
        double x2 = config["x2"].GetDouble();
        double y2 = config["y2"].GetDouble();
        Cartesian_p1 = {x0, y0};
        Cartesian_p2 = {x1, y1};
        Cartesian_p3 = {x2, y2};
         m_r1_lon = config["lon0"].GetDouble();
         m_r1_lat = config["lat0"].GetDouble();
         m_r2_lon = config["lon1"].GetDouble();
         m_r2_lat = config["lat1"].GetDouble();
         m_r3_lon = config["lon2"].GetDouble();
         m_r3_lat = config["lat2"].GetDouble();

        //open log file
        time_t t = time(NULL);
        char originalFileName[64] = {0};
        char resultFileName[64] = {0};
        strftime(originalFileName, sizeof(originalFileName) - 1, "%Y-%m-%d_%H-%M-%S_originalFC.log", localtime(&t));
        strftime(resultFileName, sizeof(resultFileName) - 1, "%Y-%m-%d_%H-%M-%S_resultFC.log", localtime(&t));

        m_resultLog.open(resultFileName, std::ios::binary);
        if (m_resultLog.fail())
        {
            printf("open fullcard record log error!");
            exit(0);
        }

        m_originalLog.open(originalFileName, std::ios::binary);
        if (m_originalLog.fail())
        {
            printf("open fullcard all data record log error!");
            exit(0);
        }
    }

    virtual ~FullCardDataParser();

    // This API will regard the input as a stream data.
    void parse(const uint8_t *data, size_t length);
    Point_XY get2dXY(int fildis, int tag, int anchor);
    Point_LonLat get2dLonLat(int fildis, int tag, int anchor);

private:
    uint8_t m_buffer[1024];
    int m_Offset;
    int m_totalcount;
    int m_lostcount;
    //point
    Point_XY Cartesian_p1;
    Point_XY Cartesian_p2;
    Point_XY Cartesian_p3;
    double m_r1_lon;
    double m_r1_lat;
    double m_r2_lon;
    double m_r2_lat;
    double m_r3_lon;
    double m_r3_lat;
    //distance to anchor
    double m_r1;
    double m_r2;
    double m_r3;
    //result
    Point_XY m_finalPose;
    Point_LonLat m_finalonlat;
    //save log
    std::ofstream m_resultLog;
    std::ofstream m_originalLog;

    //lon,lat
    //20190715
    //double m_r1_lon = 116.263654;
    //double m_r1_lat = 40.065778;
    //double m_r2_lon = 116.263428;
    //double m_r2_lat = 40.065321;
    //double m_r3_lon = 116.263832;
    //double m_r3_lat = 40.065444;
    //double m_r1_lon = 116.26370117;
    //double m_r1_lat = 40.06577167;
    //double m_r2_lon = 116.26339674;
    //double m_r2_lat = 40.06529852;
    //double m_r3_lon = 116.26379371;
    //double m_r3_lat = 40.06541142;

    //20190717 gongsi test
    //double m_r1_lon = 116.2396329638889;
    //double m_r1_lat = 40.0716846;
    //double m_r2_lon = 116.23977256666667;
    //double m_r2_lat = 40.07140895;
    //double m_r3_lon = 116.23980405277777;
    //double m_r3_lat = 40.071637808333335;
    //20190717 gongsi anchor
    //double m_r1_lon = 116.2395025728;
    //double m_r1_lat = 40.0717291061;
    //double m_r2_lon = 116.2397069754;
    //double m_r2_lat = 40.0713133828;
    //double m_r3_lon = 116.2399420473;
    //double m_r3_lat = 40.0717547668;

    //20190717 qiaoshang
    //double m_r1_lon = 116.26413570833333;
    //double m_r1_lat = 40.065572458333335;
    //double m_r2_lon = 116.26276603888888;
    //double m_r2_lat = 40.06545097777778;
    //double m_r3_lon = 116.26366968055555;
    //double m_r3_lat = 40.06570475;

    //20190717 gongsi anchor
    //double m_r1_lon = 116.2395591;
    //double m_r1_lat = 40.07030046;
    //double m_r2_lon = 116.2396496500;
    //double m_r2_lat = 40.070114700;
    //double m_r3_lon = 116.239933900;
    //double m_r3_lat = 40.070313500;

};
