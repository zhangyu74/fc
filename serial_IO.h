#ifndef SERIAL_IO_H_INCLUDED
#define SERIAL_IO_H_INCLUDED

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <iostream> /**< std::string > */
#include <termios.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "rapidjson/document.h"         /**< rapidjson::Document */
#include "rapidjson/istreamwrapper.h"   /**< rapidjson::IStreamWrapper */

typedef unsigned int	speed_t;

class SerialIO
{
public:

    SerialIO(rapidjson::Document &config);
    virtual ~SerialIO();

    bool open();
    void close();
    size_t read(uint8_t *dataBuffer, size_t dataBufferSize);

private:
    ssize_t write(void* buf, size_t len);

private:
    int m_devfd;
    std::string m_devPath;
    speed_t m_speed;

};

#endif // SERIAL_IO_H_INCLUDED
