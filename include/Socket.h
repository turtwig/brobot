#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include "BaseSocket.h"

using boost::asio::ip::tcp;

class Socket : public BaseSocket {
	tcp::socket sock;
	tcp::resolver::iterator iterator;
	boost::asio::streambuf response;
	public:
	Socket (const char* host, const char* port, boost::asio::io_service& io) : sock(io) {
		tcp::resolver resolver(io);
		tcp::resolver::query query(tcp::v4(), host, port);
		iterator = resolver.resolve(query);
	};

	void connect() {
		sock.connect(*iterator);
	};

	void close() {
		sock.close();
	};

	void write(const std::string& msg) { // writes msg
		boost::asio::write(sock, boost::asio::buffer(msg, msg.length()));
	};

	void write(const char* msg) { // writes msg
		boost::asio::write(sock, boost::asio::buffer(msg, std::strlen(msg)));
	};

	std::string read() { // returns a string up to the next '\r\n'. this function only blocks if there is no '\r\n' in the streambuf, otherwise it returns immediately
		boost::asio::read_until(sock, response, "\r\n");
		std::istream tmpis(&response);
		std::string tmp;
		std::getline(tmpis, tmp);
		if (tmp[tmp.size()-1] == '\r') tmp.erase(tmp.size()-1);
		return tmp;
	};
};

#endif // SOCKET_H_INCLUDED
