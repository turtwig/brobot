#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include "../include/BaseModule.h"
#include "../include/Args.h"
#include "../include/brobot.h"
#include <cstdlib> // for itoa
#include <string>
#include <boost/regex.hpp>

class CoreModule : public BaseModule {
    public:
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
};

#endif // CORE_H_INCLUDED
