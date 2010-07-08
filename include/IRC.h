#ifndef IRC_H_INCLUDED
#define IRC_H_INCLUDED

#include <string>
#include <boost/noncopyable.hpp>

class Brobot;

// TODO : add oper commands and whatnot (possibly through an interface)
// TODO : make sure that NAMES events get parsed correctly (as well as WHOIS, WHOWAS, WHO and any other commands that send back to the bot)

class IRC : private boost::noncopyable {
	Brobot* bro; // Brobot instance
	typedef const std::string& arg_; // shorthand for argument types
	public:
	IRC(Brobot* bot) : bro(bot) {};
	// RAW(string) returns nothing
	inline void raw(arg_ message) {
		std::string str = message + "\r\n";
		bro->sock->write(str);
	};
	// PRIVMSG(target, message)
	inline void privmsg(arg_ target, arg_ message) {
		std::string str = "PRIVMSG " + target + " :" + message + "\r\n";
		bro->sock->write(str);
	};
	//JOIN(channel)
	inline void join(arg_ channel) {
		std::string str = "JOIN " + channel + "\r\n";
		bro->sock->write(str);
	};
	//QUIT(message)
	inline void quit(arg_ message) {
		std::string str = "QUIT :" + message + "\r\n";
		bro->sock->write(str);
	};
	//NAMES(channel)
	inline void names(arg_ channel) {
		std::string str = "NAMES " + channel + "\r\n";
		bro->sock->write(str);
	};
	//AWAY(message)
	inline void away(arg_ message) {
		std::string str = "AWAY :" + message + "\r\n";
		bro->sock->write(str);
	};
	//INVITE(user,channel)
	inline void invite(arg_ user, arg_ channel) {
		std::string str = "INVITE " + user + " " + channel + "\r\n";
		bro->sock->write(str);
	};
	//KICK(user,channel,reason)
	inline void kick(arg_ user, arg_ channel, arg_ reason) {
		std::string str = "KICK " + channel + " " + user + " :" + reason + "\r\n";
		bro->sock->write(str);
	};
	//MODE(target,string)
	inline void mode(arg_ target, arg_ strng) {
		std::string str = "MODE " + target + " " + strng + "\r\n";
		bro->sock->write(str);
	};
	//NICK(newnick)
	inline void nick(arg_ newnick) {
		std::string str = "NICK " + newnick + "\r\n";
		bro->sock->write(str);
		// TODO : update nick status (somewhere)
	};
	//NOTICE(target,message)
	inline void notice(arg_ target, arg_ message) {
		std::string str = "NOTICE " + target + " :" + message + "\r\n";
		bro->sock->write(str);
	};
	//PART(channel)
	inline void part(arg_ channel) {
		std::string str = "PART " + channel + "\r\n";
		bro->sock->write(str);
	};
	//TOPIC(channel,message)
	inline void topic(arg_ channel, arg_ message) {
		std::string str = "TOPIC " + channel + " :" + message + "\r\n";
		bro->sock->write(str);
	};
	//WHOIS(user)
	inline void whois(arg_ user) {
		std::string str = "WHOIS " + user + "\r\n";
		bro->sock->write(str);
	};
	//USER(ident,realname)
	inline void user(arg_ ident, arg_ realname) {
		std::string str = "USER " + ident + " * * :" + realname + "\r\n";
		bro->sock->write(str);
	};
	//OPER(login,pass)
	inline void oper(arg_ login, arg_ pass) {
		std::string str = "OPER " + login + " " + pass + "\r\n";
		bro->sock->write(str);
	};
};

#endif // IRC_H_INCLUDED
