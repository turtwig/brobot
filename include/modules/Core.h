#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include "BaseModule.h"
#include "brobot.h"
#include <map>
#include <vector>
#include <cstdlib> // for itoa
#include <string>
#include <boost/regex.hpp>

class CoreModule : public BaseModule {
	std::map<std::string, BaseModule*> all_mods; // hold a BaseModule* pointer for every module that gets registered so we can load/unload them
	public:
	void module(const std::string& name, BaseModule* mod);
	// Brobot stuff:
	void onLoad(Brobot* bro);
	void onUnload(Brobot* bro);
	// Parsers
	void numerics(Brobot* bro, const std::string& str);
	void commands(Brobot* bro, const std::string& str);
	void nick(Brobot* bro, const std::string& str);
	void joinpart(Brobot* bro, const std::string& str);
	void quit(Brobot* bro, const std::string& str);
	void ping(Brobot* bro, const std::string& str);
	// Hooks
	void onconnect(Brobot* bro, Args& arg);
	void pingHook(Brobot* bro, Args& arg);
	void modulelist(Brobot* bro, Args& args);
	void hooklist(Brobot* bro, Args& args);
	void moduleunload(Brobot* bro, Args& args);
	void moduleload(Brobot* bro, Args& args);
};

#endif // CORE_H_INCLUDED
