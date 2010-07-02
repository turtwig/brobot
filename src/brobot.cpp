#include "../include/brobot.h"

Brobot::Brobot(boost::asio::io_service& io, boost::asio::ssl::context& ctx, const std::string& fname) : context(ctx), io_service(io) {
	stor = new Storage(fname);
	irc = new IRC(this);
	if (stor->get("core.ssl") == "1") {
		sock = new SSLSocket(stor->getcstr("core.ip"), stor->getcstr("core.port"), io_service, context);
	} else {
		sock = new Socket(stor->getcstr("core.ip"), stor->getcstr("core.port"), io_service);
	}
};

Brobot::~Brobot() {
	delete stor;
	delete irc;
	delete sock;
};

void Brobot::start() {
	sock->connect();
	irc->user(stor->get("core.ident"), stor->get("core.real"));
	irc->nick(stor->get("core.nick"));
	while(true) {
		parse(sock->read());
	}
};

void Brobot::hook(const std::string& name, const std::string& hook, boost::function<void (Args&)> func) {
	 callbacks[hook][name] = func; // overwrites the existing hook (if any).
};

void Brobot::unhook(const std::string& name, const std::string& hook) {
	hooks_to_unhook.push_back(std::make_pair(name, hook)); // add hooks to the list of hooks to be unhooked on next runHooks
};

void Brobot::runHooks(const std::string& hook, Args& arg) {
	typedef std::pair<std::string, boost::function<void (Args&)> > penis_t;
	if (callbacks.find(hook) == callbacks.end())
		return; // no such hook map
	typedef std::pair<std::string, std::string> unhook_t;
	BOOST_FOREACH( unhook_t penis, hooks_to_unhook ) {
		std::map<std::string, callback_map_t>::iterator upper_iter = callbacks.find(penis.second);
		if (upper_iter != callbacks.end()) { // look for hooks
			callback_map_t::iterator lower_iter = upper_iter->second.find(penis.first);
			if (lower_iter != upper_iter->second.end()) { // look for the specific hook
				upper_iter->second.erase(lower_iter); // erase the actual hook
				if (upper_iter->second.empty()) { // the map pointed to by the "top" map (i.e. the one containing the actual funtions) is empty
					callbacks.erase(upper_iter); // so we can erase this element
				}
			}
		}
	}
	hooks_to_unhook.clear();
	BOOST_FOREACH( penis_t h, callbacks[hook] )
		h.second(arg);
};

void Brobot::addParser(const std::string& name, boost::function<void (const std::string&)> func) {
	parserEvents[name] = func; // overwrites any existing functions
};

void Brobot::delParser(const std::string& name) {
	parsers_to_delete.push_back(name);
};

bool Brobot::loadMod(const std::string& name, BaseModule* module) {
	if (modules.find(name) != modules.end()) // module already loaded
		return false;
	module->onLoad(this);
	modules[name] = module;
	return true; // load successful
};

bool Brobot::unloadMod(const std::string& name) {
	std::map<std::string, BaseModule*>::iterator it = modules.find(name);
	if (it == modules.end())
		return false; // could not find module
	it->second->onUnload(this); // found a module
	modules.erase(it);
	return true; // unload successful
};

BaseModule* Brobot::findMod(const std::string& name) {
	std::map<std::string, BaseModule*>::iterator it = modules.find(name);
	if (it == modules.end())// no such module
		return NULL;
	return it->second;
};

std::vector<std::string> Brobot::listMods() {
	std::vector<std::string> vec;
	typedef std::pair<std::string, BaseModule*> pair_t;
	BOOST_FOREACH(pair_t pair, modules) {
		vec.push_back(pair.first);
	}
	return vec;
};

std::map<std::string, std::vector<std::string> > Brobot::listHooks() {
	std::map<std::string, std::vector<std::string> > map;
	typedef std::pair<std::string, callback_map_t> pair_t;
	typedef std::pair<std::string, boost::function<void (Args&)> > penis_t;
	BOOST_FOREACH(pair_t pair1, callbacks) {
		BOOST_FOREACH(penis_t pair2, pair1.second) {
			map[pair1.first].push_back(pair2.first);
		}
	}
	return map;
};

void Brobot::parse(const std::string& s) {
	BOOST_FOREACH( std::string name, parsers_to_delete) {
		std::map<std::string, boost::function<void (const std::string&)> >::iterator it = parserEvents.find(name);
		if (it != parserEvents.end()) {
			parserEvents.erase(it);
		}
	}
	parsers_to_delete.clear();
	typedef std::pair<std::string, boost::function<void (const std::string&)> > pair_t;
	BOOST_FOREACH( pair_t p, parserEvents )
		p.second(s); // each parser function is responsible for firing the appropriate hooks
};
