#ifndef BROBOT_H_INCLUDED
#define BROBOT_H_INCLUDED

#include "Socket.h"
#include "SSLSocket.h"

#include "Storage.h" // manages the flatfile backend (basically a wrapper around std::map<std::string, std::string>, used for config file
#include "BaseModule.h"

#include <map> // Used for hooks
#include <vector> // Used for hooks

#include <string>

#include <boost/bind.hpp> // Used in callbacks
#include <boost/function.hpp> // Used in callbacks
#include <boost/foreach.hpp> // Plain useful
#include <boost/noncopyable.hpp>

/* Hooks
 * They need to take a Brobot* const argument and a
 * const Args& argument.
 * Args is a typedef to std::vector<std::string>
 * access is done by args[0]...args[n]
 * which differ for each hook
 *
 * Please notice that you should bind the
 * Brobot* argument, thus only
 * the Args have to be passed.
 */

 /* Modules
  * Just make a class that inherits from
  * BaseModule, and override the virtual functions
  * onLoad and onUnload (both take a Brobot*).
  * These are basically a way of enabling
  * a more formal module-like behavior for brobot.
  * A module would use onLoad and onUnload functions
  * to register/unregister hooks, parser events and
  * whatever it might need.
  * You can also provide a public interface for
  * other modules to access (using a dynamic_cast
  * on the pointer contained in the modules map).
  */

class IRC; // Declaration for use of friend in Brobot

typedef std::vector<std::string> Args;

class Brobot : private boost::noncopyable {
	Socket* sock; // is a pointer so we can use either a normal Socket or a SSLSocket
	boost::asio::ssl::context& context; // Used for SSL (even if SSL is disabled)
	boost::asio::io_service& io_service;

	typedef std::map<std::string, boost::function<void (const Args&)> > callback_map_t; // map holding various elements, each having a name and a function
	std::map<std::string, callback_map_t> callbacks; // the actual map of callbacks. each element points to an ``event''. each element points to a callback_map_t

	std::map<std::string, BaseModule*> modules; // map of modules (name, module*) is a pointer for polymorphism

	std::map<std::string, boost::function<void (const std::string&)> > parserEvents; // map of functions to call on each run of parse(), (name, function)

	std::vector<std::pair<std::string, std::string> > hooks_to_unhook; // list of hooks to be unhooked on the next runHooks()
	std::vector<std::string> parsers_to_delete; // list of parsers to delete on next parse();

	public:
	Brobot(boost::asio::io_service& io, boost::asio::ssl::context& ctx, const std::string& fname = "brobot.conf");
	~Brobot();

	void start(); // main bot loop, connects and parses shit.

	void hook(const std::string& name, const std::string& hook, boost::function<void (const Args&)> func);
	void unhook(const std::string& name, const std::string& hook);
	void runHooks(const std::string& hook, const Args& arg);
	std::map<std::string, std::vector<std::string> > listHooks();

	bool loadMod(const std::string& name, BaseModule* module);
	bool unloadMod(const std::string& name);
	BaseModule* findMod(const std::string& name); // returns NULL if module not found
	std::vector<std::string> listMods();

	void addParser(const std::string& name, boost::function<void (const std::string&)> func);
	void delParser(const std::string& name);

	// public members
	// The only reason they are public is for easy access to their own interfaces
	Storage* stor; // config file (brobot.conf)
	friend class IRC; // used for IRC control
	IRC* irc;


	private:
	void parse(const std::string& s); // the actual parser, runs every time something is received from the server.
};

#include "IRC.h" // member functions for easy IRC control, must be included after Brobot due to proper type qualifications

inline Args& operator%(Args& arg, const char* string) { arg.push_back(string); return arg; };
inline Args& operator%(Args& arg, const std::string& string) { arg.push_back(string); return arg; };

#endif // BROBOT_H_INCLUDED
