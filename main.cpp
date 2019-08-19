#include "fullcard_data_parser.h"
#include "Trilateration.h"
#include "serial_IO.h"

#include <stdlib.h>
#include <stdio.h>
#include "rapidjson/document.h"         /**< rapidjson::Document */
#include "rapidjson/istreamwrapper.h"   /**< rapidjson::IStreamWrapper */

#define BUFFER_SIZE 1024

static bool onRun(rapidjson::Document &config)
{

    FullCardDataParser *fullcard = new FullCardDataParser(config);
    SerialIO *com = new SerialIO(config);

    uint8_t buffer[BUFFER_SIZE] = { 0 };
    bool succ = com->open();

    if (!succ)
    {
        std::cout << "Failed to open!" << std::endl;
        com->close();
        return false;
    }


    while (true)
    {
        //m_counter++;
        size_t readSize = com->read(buffer, BUFFER_SIZE);

        if (readSize > 0)
        {
            fullcard->parse(buffer, readSize);
        }
    }

    com->close();
    return true;

}

static void onTest(rapidjson::Document &config)
{
    FullCardDataParser *dp_fullcard = new FullCardDataParser(config);
    uint8_t buffer[1024] = {0};
    uint8_t read_buffer[10] = {0};

    std::ifstream fin("fullcard_original_20190723_zhi1.txt", std::ios::binary);
    if (!fin.good())
    {
        std::cout << "Failed to laod the config file: " << "config.txt" << "!" << std::endl;
        assert(fin);
    }

    while (!fin.eof())
    {
        fin.read(reinterpret_cast<char *>(&read_buffer), sizeof(read_buffer));
        memmove(buffer, &read_buffer, sizeof(read_buffer));
        dp_fullcard->parse(buffer, sizeof(read_buffer));
    }

    //lon lat test
    class Trilateration test_a;
    //class Coordinate a(37.418436, -121.963477, 0.265710701754);
    //class Coordinate b(37.417243, -121.961889, 0.234592423446);
    //class Coordinate c(37.418692, -121.960194, 0.0548954278262);

    class Coordinate a(40.065572458333335, 116.26413570833333, 81.887);
    class Coordinate b(40.06545097777778, 116.26276603888888, 44.673);
    class Coordinate c(40.06570475, 116.26366968055555, 61.696);
    test_a.triangulateLonLat(a, b, c);

    fin.close();
}

int main(void)
{
    //get config
    std::string configFilePath = "config.txt";
    std::ifstream ifs(configFilePath);
    if (!ifs.good())
    {
        std::cout << "Failed to laod the config file: " << configFilePath << "!" << std::endl;
        assert(ifs);
    }
    rapidjson::IStreamWrapper isw(ifs);
    rapidjson::Document doc;
    doc.ParseStream(isw);

    //onTest(doc);
    onRun(doc);

}

