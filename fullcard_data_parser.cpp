#include "fullcard_data_parser.h"

#include <stdlib.h>
#include <iostream>  /**< std::cerr std::endl */
#include <algorithm> /**< std::copy */
#include <iterator>  /**< std::inserter */
#include <math.h>    /** sin */
#include <iomanip>   /** setprecision */
#include <string.h>  /** strsep */
//#include <stddef.h>/** strsep */

#define DIS_MAX 1000000 //1000m
#define DIS_MIN 0
#define ANCHOR_NO1 0
#define ANCHOR_NO2 1
#define ANCHOR_NO3 2
#define TAG_NO 8

#define ignoreSep(str, ch, count) \
    {                             \
        int i = 0, j = 0;         \
        for (; i < count; j++)    \
        {                         \
            if (str[j] == ch)     \
            {                     \
                i++;              \
            }                     \
        }                         \
        str = str + j;            \
    }

/*
char* strsep(char** stringp, const char* delim)
{
    char* start = *stringp;
    char* p;

    p = (start != NULL) ? strpbrk(start, delim) : NULL;

    if (p == NULL)
    {
    *stringp = NULL;
    }
    else
    {
    *p = '\0';
    *stringp = p + 1;
    }

    return start;
}
*/

//xyz coordinates
Point_XY FullCardDataParser::get2dXY(int fildis, int tag, int anchor)
{
    class Trilateration Trilate;
    switch (anchor)
    {
    case ANCHOR_NO1:
    {
        if (m_r1 != 0)
        {
            m_lostcount++;
        }
        else
        {
            m_totalcount++;
        }

        m_r1 = fildis;
        break;
    }
    case ANCHOR_NO2:
    {
        if (m_r2 != 0)
        {
            m_lostcount++;
        }
        else
        {
            m_totalcount++;
        }
        m_r2 = fildis;
        break;
    }
    case ANCHOR_NO3:
    {
        if (m_r3 != 0)
        {
            m_lostcount++;
        }
        else
        {
            m_totalcount++;
        }
        m_r3 = fildis;
        break;
    }
    }

    //All points updated, refresh location.
    if (m_r1 != 0 && m_r2 != 0 && m_r3 != 0)
    {
        m_finalPose = Trilate.triangulateXY(Cartesian_p1, Cartesian_p2, Cartesian_p3, m_r1, m_r2, m_r3);
        m_r1 = 0;
        m_r2 = 0;
        m_r3 = 0;
        printf("@@@@@@@@@@x  %f, y  %f\n", m_finalPose.x, m_finalPose.y);
        m_resultLog << m_finalPose.x << ',' << m_finalPose.y << std::endl;
    }
    //printf("all  %d, lost  %d\n", m_totalcount, m_lostcount);
    return m_finalPose;
}

//lon, lat coordinates
Point_LonLat FullCardDataParser::get2dLonLat(int fildis, int tag, int anchor)
{
    class Trilateration Trilate;
    switch (anchor)
    {
    case ANCHOR_NO1:
    {
        if (m_r1 != 0)
        {
            m_lostcount++;
        }
        else
        {
            m_totalcount++;
        }

        m_r1 = fildis;
        break;
    }
    case ANCHOR_NO2:
    {
        if (m_r2 != 0)
        {
            m_lostcount++;
        }
        else
        {
            m_totalcount++;
        }
        m_r2 = fildis;
        break;
    }
    case ANCHOR_NO3:
    {
        if (m_r3 != 0)
        {
            m_lostcount++;
        }
        else
        {
            m_totalcount++;
        }
        m_r3 = fildis;
        break;
    }
    }

    //All points updated, refresh location.
    if (m_r1 != 0 && m_r2 != 0 && m_r3 != 0)
    {
        class Coordinate a(m_r1_lat, m_r1_lon, m_r1 / 1000);
        class Coordinate b(m_r2_lat, m_r2_lon, m_r2 / 1000);
        class Coordinate c(m_r3_lat, m_r3_lon, m_r3 / 1000);
        m_finalonlat = Trilate.triangulateLonLat(a, b, c);

        //reset
        m_r1 = 0;
        m_r2 = 0;
        m_r3 = 0;

        //print and save
        printf("@@@@@@@@@@lon  %f, lat  %f\n", m_finalonlat.lon, m_finalonlat.lat);
        m_resultLog << std::fixed << std::setprecision(10) << m_finalonlat.lon << ',' << m_finalonlat.lat << std::endl;
    }
    printf("all  %d, lost  %d\n", m_totalcount, m_lostcount);
    return m_finalonlat;
}

void FullCardDataParser::parse(const uint8_t *data, size_t length)
{
    const uint8_t *p = data;
    size_t len = length;

    uint8_t c;
    char *buf = (char *)m_buffer;
    char frame[1024];

    if (p == NULL)
    {
#if defined(DEBUG)
        std::cerr << "Data is invalid!" << std::endl;
#endif /* DEBUG */
        return;
    }

    while (len > 0)
    {
        c = *p++;
        len--;

        if (c != '\n')
        {
            m_buffer[m_Offset] = c;
            m_Offset++;

            if (m_Offset > 1024)
                m_Offset = 0;
            continue;
        }

        //save log
        memset(frame, 0, sizeof(frame));
        memcpy(frame, &m_buffer, m_Offset - 1);
        m_originalLog << frame << std::endl;

        m_buffer[m_Offset] = '\0';
        m_Offset = 0;
        buf = (char *)m_buffer;

        if (strstr(buf, "mc") != NULL)
        {
            ignoreSep(buf, ' ', 2);
            int fildis = strtol(buf, &buf, 16);
            ignoreSep(buf, ' ', 1);
            int oridis = strtol(buf, &buf, 16);
            ignoreSep(buf, ' ', 3);
            int count = atol(buf);
            ignoreSep(buf, ' ', 3);
            buf++;
            char *ptag = NULL;
            char *panchor = NULL;
            ptag = strsep(&buf, ":");
            if (ptag != NULL)
            {
                panchor = strsep(&buf, ":");
            }
            int tag = atol(ptag);
            int anchor = atol(panchor);

            //printf("@@@@@@@@@@filter_dis  %d, original_dis  %d, count  %d, tag  %d, anchor  %d\n",
            //       fildis, oridis, count, tag, anchor);
            if (fildis < DIS_MAX && fildis > DIS_MIN && tag == TAG_NO && ((anchor == ANCHOR_NO1) | (anchor == ANCHOR_NO2) | (anchor == ANCHOR_NO3)))
            {
                get2dXY(fildis, tag, anchor);
                //get2dLonLat(fildis, tag, anchor);
            }
            else
            {
                printf("!!!!!!filter_dis  %d, original_dis  %d, count  %d, tag  %d, anchor  %d\n",
                       fildis, oridis, count, tag, anchor);
            }
        }
    }
}

FullCardDataParser::~FullCardDataParser()
{
    m_resultLog.close();
    m_originalLog.close();
}
