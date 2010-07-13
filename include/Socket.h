#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <string>
#include <cstring> // for strlen
#include <boost/asio.hpp>

class Socket : private boost::noncopyable {
	boost::asio::ip::tcp::socket* sock;
	protected:
	boost::asio::ip::tcp::resolver::iterator iterator;
	boost::asio::streambuf response;
	public:
	Socket (const char* host, const char* port, boost::asio::io_service& io) {
		sock = new boost::asio::ip::tcp::socket(io);
		boost::asio::ip::tcp::resolver resolver(io);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), host, port);
		iterator = resolver.resolve(query);
	};

	Socket () : sock (NULL) {};

	~Socket () {
		if (sock != NULL)
			delete sock;
	};

	virtual void connect() {
		sock->connect(*iterator);
	};

	virtual void close() {
		sock->close();
	};

	virtual void write(const std::string& msg) { // writes msg
		boost::asio::write(*sock, boost::asio::buffer(msg, msg.length()));
	};

	virtual void write(const char* msg) { // writes msg
		boost::asio::write(*sock, boost::asio::buffer(msg, std::strlen(msg)));
	};

	virtual std::string read() { // returns a string up to the next '\r\n'. this function only blocks if there is no '\r\n' in the streambuf, otherwise it returns immediately
		boost::asio::read_until(*sock, response, "\r\n");
		std::istream tmpis(&response);
		std::string tmp;
		std::getline(tmpis, tmp);
		if (tmp[tmp.size()-1] == '\r') tmp.erase(tmp.size()-1);
		return tmp;
	};
};

#endif // SOCKET_H_INCLUDED
