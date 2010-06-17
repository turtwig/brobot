#ifndef BASEMODULE_H_INCLUDED
#define BASEMODULE_H_INCLUDED

#include <boost/noncopyable.hpp>

class Brobot;

class BaseModule : private boost::noncopyable {
    public:
    virtual void onLoad(Brobot* bro) = 0; // receives a pointer to Brobot
    virtual void onUnload(Brobot* bro) = 0; // used for unloading modules upon unload. the destructor may or may not be called after.
};

#endif // BASEMODULE_H_INCLUDED
