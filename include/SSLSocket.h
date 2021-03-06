#ifndef SSLSOCKET_H_INCLUDED
#define SSLSOCKET_H_INCLUDED

#include "Socket.h"
#include <boost/asio/ssl.hpp>

class SSLSocket : public Socket {
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> sock;
	public:
	SSLSocket (const char* host, const char* port, boost::asio::io_service& io, boost::asio::ssl::context& context) : sock(io, context) {
		boost::asio::ip::tcp::resolver resolver(io);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), host, port);
		iterator = resolver.resolve(query);
	};

	void connect() {
		sock.lowest_layer().connect(*iterator);
		sock.handshake(boost::asio::ssl::stream_base::client);
	};

	void close() {
		sock.lowest_layer().close();
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

#endif // SSLSOCKET_H_INCLUDED
