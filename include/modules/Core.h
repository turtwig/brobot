#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include "BaseModule.h"
#include "brobot.h"
#include <map>
#include <vector>
#include <string>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

class CoreModule : public BaseModule {
	std::map<std::string, BaseModule*> all_mods; // hold a BaseModule* pointer for every module that gets registered so we can load/unload them
	public:
	void module(const std::string& name, BaseModule* const mod);
	void loadMods(Brobot* const bro);
	// Brobot stuff:
	void onLoad(Brobot* const bro);
	void onUnload(Brobot* const bro);
	// Parsers
	void numerics(Brobot* const bro, const std::string& str);
	void commands(Brobot* const bro, const std::string& str);
	void nick(Brobot* const bro, const std::string& str);
	void joinpart(Brobot* const bro, const std::string& str);
	void quit(Brobot* const bro, const std::string& str);
	void ping(Brobot* const bro, const std::string& str);
	// Hooks
	void onconnect(Brobot* const bro, const Args& arg);
	void pingHook(Brobot* const bro, const Args& arg);
	void modulelist(Brobot* const bro, const Args& args);
	void hooklist(Brobot* const bro, const Args& args);
	void moduleunload(Brobot* const bro, const Args& args);
	void moduleload(Brobot* const bro, const Args& args);
	void hookunload(Brobot* const bro, const Args& args);
};

#endif // CORE_H_INCLUDED
