#ifndef CORE_H_INCLUDED
#define CORE_H_INCLUDED

#include "BaseModule.h"
#include "Args.h"
#include "brobot.h"
#include <string>
#include <boost/regex.hpp>

class CoreModule : public BaseModule {
    public:
    void onLoad(Brobot* bro);
    void onUnload(Brobot* bro);
    void numerics(Brobot* bro, const std::string& str);
    void commands(Brobot* bro, const std::string& str);
    void ping(Brobot* bro, const std::string& str);
    void pingHook(Brobot* bro, Args& arg);
};

#endif // CORE_H_INCLUDED
