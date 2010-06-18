#ifndef BASESOCKET_H_INCLUDED
#define BASESOCKET_H_INCLUDED

#include <string>
#include <cstring> // for strlen
#include <boost/asio.hpp>

class BaseSocket : private boost::noncopyable{
    public:
    virtual void connect() = 0;
    virtual void close() = 0;
    virtual void write(const std::string& msg) = 0;
    virtual void write(const char* msg) = 0;
    virtual std::string read() = 0;
};

#endif // BASESOCKET_H_INCLUDED
