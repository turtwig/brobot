#ifndef ARGS_H_INCLUDED
#define ARGS_H_INCLUDED

#include <vector>
#include <string>
#include <boost/noncopyable.hpp>

//Defines the class that is used for passing arguments to callbacks.
//Basically a wrapper around std::vector<std::string>

class Args : private boost::noncopyable{
	std::vector<std::string> args;
	public:
	Args& operator%(const char* string) { args.push_back(string); return *this; };
	Args& operator%(const std::string& string) { args.push_back(string); return *this; };
	std::string operator[](const unsigned int& i) { return args[i]; };
	const std::vector<std::string>& vect() { return args; };
};

#endif // ARGS_H_INCLUDED
