#include "serial_IO.h"

#include <stdio.h>      /**< sprintf() */
#include <string.h>     /**< strlen(), strerror_r */
#include <assert.h>     /** assert */
#include <errno.h>


SerialIO::SerialIO(rapidjson::Document &config)
{
    int speed = 0;
    //get config
    if (config.HasMember("path")
        && config.HasMember("baudrate"))
    {
        speed = config["baudrate"].GetInt();
        m_devPath = std::string(config["path"].GetString());
    }
    else
    {
        std::cout << "The required members `path` or `baudrate` are missing!" << std::endl;
    }
    // Verify all the required members
    if (speed == 230400)
    {
        m_speed = B230400;
    }
    else if (speed == 115200)
    {
        m_speed = B115200;
    }
    else
    {
        std::cout << "The specific speed is not supported!" << speed << "!" << std::endl;
        assert((speed == 230400) | (speed == 115200) );
    }

    if (m_devPath.empty())
    {
        std::cout << "No device opened!" << std::endl;
        assert(!m_devPath.empty());
    }

}


SerialIO::~SerialIO()
{
    close();
}

static void _setline(int fd, int flags, int speed)
{
    struct termios t;
	tcgetattr(fd, &t);
	t.c_cflag = flags | CREAD | HUPCL | CLOCAL;
	t.c_iflag = IGNBRK | IGNPAR;
	t.c_oflag = 0;
	t.c_lflag = 0;
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;
	cfsetispeed(&t, speed);
	cfsetospeed(&t, speed);
	tcsetattr(fd, TCSANOW, &t);
}

bool SerialIO::open()
{

    std::cout << "Start to open device:" << m_devPath.c_str() << std::endl;
    m_devfd = ::open(m_devPath.c_str(), O_RDWR);

    if (m_devfd == -1)
    {
        char errStr[256];
        errStr[0] = 0;
        std::cout << "Failed to open device:" << m_devPath.c_str() << std::endl;
        std::cout << strerror_r(errno, errStr, 255) << std::endl;
        return false;
    }

    std::cout << "Open device successfully!" << std::endl;
    _setline(m_devfd, CS8, m_speed);
    return true;
}

void SerialIO::close()
{
    if (m_devfd != -1)
    {
        ::close(m_devfd);
        m_devfd = -1;
    }
}

size_t SerialIO::read(uint8_t *dataBuffer, size_t dataBufferSize)
{
    size_t readSize = 0;

    if (m_devfd == -1)
    {
        return readSize;
    }

    readSize = ::read(m_devfd, dataBuffer, dataBufferSize);

#if defined(DEBUG) && defined(VERBOSE)
    LOCKED(m_mutex)
    {
        std::cout << std::dec << "Read data length: " << readSize << std::endl;
        ad::ins::Util::dumpBufferHex(dataBuffer, readSize);
    }
#endif /* DEBUG && VERBOSE */

    return readSize;
}

ssize_t SerialIO::write(void* buf, size_t len)
{
    ssize_t writtenSize = 0;
    uint8_t *dataBuffer = reinterpret_cast<uint8_t *>(buf);
    ssize_t requiredLen = static_cast<ssize_t>(len);

    if (m_devfd == -1)
    {
        return writtenSize;
    }

    do {
        writtenSize += ::write(m_devfd, dataBuffer + writtenSize, requiredLen - writtenSize);
    } while (writtenSize < requiredLen);

#if defined(DEBUG) && defined(VERBOSE)
    LOCKED(m_mutex)
    {
        std::cout << std::dec << "Written data length: " << writtenSize << std::endl;
        ad::ins::Util::dumpBufferHex(buf, len);
    }
#endif /* DEBUG && VERBOSE */
    return writtenSize;
}
