#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

#include <fstream>
#include <string>
#include <map>
#include <boost/noncopyable.hpp>

class Storage : private boost::noncopyable {
	std::fstream file;
	std::map<std::string, std::string> keys;
	public:
	Storage(const std::string& fname);
	const char* getcstr(const std::string& key) { if (keys.find(key) != keys.end()) { return keys.find(key)->second.c_str(); } else { return ""; } };
	const std::string& get(const std::string& key) { if (keys.find(key) != keys.end()) { return keys.find(key)->second; } else { return ""; } };
};

#endif // STORAGE_H_INCLUDED
