#include "modules/Core.h"

void CoreModule::onLoad(Brobot* const bro) {
	// Parser functions
	bro->addParser("ping", boost::bind(&CoreModule::ping, this, bro, _1));
	bro->addParser("numerics", boost::bind(&CoreModule::numerics, this, bro, _1));
	bro->addParser("commands", boost::bind(&CoreModule::commands, this, bro, _1));
	bro->addParser("nick", boost::bind(&CoreModule::nick, this, bro, _1));
	bro->addParser("joinpart", boost::bind(&CoreModule::joinpart, this, bro, _1));
	bro->addParser("quit", boost::bind(&CoreModule::quit, this, bro, _1));
	// Hooks
	bro->hook("[core] Ping", "OnPING", boost::bind(&CoreModule::pingHook, this, bro, _1));
	bro->hook("[core] OnConnect", "Numeric001", boost::bind(&CoreModule::onconnect, this, bro, _1));
	bro->hook("[core] ModuleList", "OnPRIVMSG", boost::bind(&CoreModule::modulelist, this, bro, _1));
	bro->hook("[core] HookList", "OnPRIVMSG", boost::bind(&CoreModule::hooklist, this, bro, _1));
	bro->hook("[core] ModuleUnload", "OnPRIVMSG", boost::bind(&CoreModule::moduleunload, this, bro, _1));
	bro->hook("[core] ModuleLoad", "OnPRIVMSG", boost::bind(&CoreModule::moduleload, this, bro, _1));
	bro->hook("[core] HookUnload", "OnPRIVMSG", boost::bind(&CoreModule::hookunload, this, bro, _1));
};

void CoreModule::onUnload(Brobot* const bro) {
	// Parser functions
	bro->delParser("commands");
	bro->delParser("numerics");
	bro->delParser("ping");
	bro->delParser("nick");
	bro->delParser("joinpart");
	bro->delParser("quit");
	// Hooks
	bro->unhook("[core] Ping", "OnPING");
	bro->unhook("[core] ModuleList", "OnPRIVMSG");
	bro->unhook("[core] HookList", "OnPRIVMSG");
	bro->unhook("[core] ModuleUnload", "OnPRIVMSG");
	bro->unhook("[core] ModuleLoad", "OnPRIVMSG");
	bro->unhook("[core] HookUnload", "OnPRIVMSG");
};

/* Calls numeric hooks.
 * The hook name syntax is (for numeric 001):
 *  Numeric001
 * It forwards the text (after the nick argument) the numeric sends
 * including (or not) the : before it.
 */
void CoreModule::numerics(Brobot* const bro, const std::string& str) {
	size_t delim1 = str.find(' ');
	size_t delim2 = str.find(' ', ++delim1);
	if (str.substr(delim1, delim2-delim1).find_first_of("0123456789") == std::string::npos) // no numbers
		return; // nothing else to do
	size_t argsDelim = str.find(' ', delim2+1)+1;
	Args arg;
	arg % str.substr(argsDelim, std::string::npos);
	bro->runHooks("Numeric" + str.substr(delim1, delim2-delim1), arg);
};

/* Handles most user commands
 * including, for example, PRIVMSGs and NOTICEs.
 * the hook name(s) follows the pattern of:
 * OnCOMMAND, OnPRIVMSG being the one for PRIVMSGs.
 * It forwards the matches as the arguments for the hook
 * a[0] being the full raw string, a[1] the nick, a[2] the ident,
 * a[3] the host ,a[4] the target and
 * a[5] the actual message.
 */
void CoreModule::commands(Brobot* const bro, const std::string& str) {
	static const boost::regex expr("^:(\\S+?)!(\\S+?)@(\\S+?) (\\S+?) (\\S+?) :(.+?)$"); // matches :NICK!IDENT@HOST COMMAND TARGET :MESSAGE
	boost::smatch match;
	if (!boost::regex_match(str, match, expr)) // we did not get a match
		return; // nothing else to do
	if (match[4] == "PART")
		return;
	Args arg;
	arg % str % match[1] % match[2] % match[3] % match[5] % match[6];
	bro->runHooks("On" + match[4], arg);
};

/* Handles nick changes
 * Hook is OnNICK
 * a[0] is the raw string, a[1] is the original nick, a[2] is the new nick, a[3] is the ident, a[4] is the host
 */
void CoreModule::nick(Brobot* const bro, const std::string& str) {
	static const boost::regex expr("^:(\\S+?)!(\\S+?)@(\\S+?) NICK :(\\S+?)$"); // matches NICK!IDENT@HOST NICK :NEWNICK
	boost::smatch match;
	if (!boost::regex_match(str, match, expr)) // we did not get a match
		return; // nothing else to do
	Args arg;
	arg % str % match[1] % match[4] % match[2] % match[3];
	bro->runHooks("OnNICK", arg);
};

/* Handles join/parts
 * Hook is OnJOIN/OnPART
 * a[0] is the raw string, a[1] is the nick, a[2] is the ident, a[3] is the host, a[4] is the channel, a[5] is the part message (if any)
 */
void CoreModule::joinpart(Brobot* const bro, const std::string& str) {
	static const boost::regex expr("^:(\\S+?)!(\\S+?)@(\\S+?) (JOIN|PART) :?(#\\S+?)( :(.+?))?$"); // matches NICK!IDENT@HOST JOIN|PART :?#CHANNEL
	boost::smatch match;
	if (!boost::regex_match(str, match, expr)) // we did not get a match
		return; // nothing else to do
	Args arg;
	arg % str % match[1] % match[2] % match[3] % match[5] % match[7];
	bro->runHooks("On"+match[4], arg);
};

/* Handles quits
 * Hook is OnQUIT
 * a[0] is the raw string, a[1] is the nick, a[2] is the ident, a[3] is the hostmask, a[4] is the quit message
 */
void CoreModule::quit(Brobot* const bro, const std::string& str) {
	static const boost::regex expr("^:(\\S+?)!(\\S+?)@(\\S+?) QUIT :(.+?)$"); // matches NICK!IDENT@HOST QUIT :MESSAGE
	boost::smatch match;
	if (!boost::regex_match(str, match, expr)) // we did not get a match
		return; // nothing else to do
	Args arg;
	arg % str % match[1] % match[2] % match[3] % match[4];
	bro->runHooks("OnQUIT", arg);
};

/* Handles PING
 * fires hook OnPING.
 * single argument is the text to PONG back
 */
void CoreModule::ping(Brobot* const bro, const std::string& str) {
	if (str.substr(0,4) != "PING")
		return; //nothing else to do
	Args arg;
	arg % str.substr(5, std::string::npos);
	bro->runHooks("OnPING", arg);
};

// Responds to PING
void CoreModule::pingHook(Brobot* const bro, const Args& arg) {
	bro->irc->raw("PONG " + arg[0]); // simply respond to the PING
};

// Runs the raw commands in brobot.conf starting from core.onconnnect.0 up to however much needed
void CoreModule::onconnect(Brobot* const bro, const Args&) {
	if (bro->stor->get("core.oper.login") != "" && bro->stor->get("core.oper.password") != "") { // we can oper up
		bro->irc->oper(bro->stor->get("core.oper.login"), bro->stor->get("core.oper.password"));
	}
	if (bro->stor->get("core.nickserv") != "") { // we need to identify
		bro->irc->privmsg("NickServ", "IDENTIFY " + bro->stor->get("core.nickserv"));
	}
	unsigned short int h = 0;
	std::string str = "";
	do {
		str = bro->stor->get("core.onconnect."+boost::lexical_cast<std::string>(h));
		bro->irc->raw(str);
		++h;
	} while(str != "");
	bro->unhook("[core] OnConnect", "Numeric001"); // this hook only needs to run once
};

void CoreModule::module(const std::string& name, BaseModule* const mod) {
	all_mods[name] = mod; // add module to list of modules that can be loaded
};

void CoreModule::loadMods(Brobot* const bro) {
	unsigned short int h = 0;
	std::string str = "";
	do {
		str = bro->stor->get("core.modules."+boost::lexical_cast<std::string>(h));
		if (all_mods.find(str) != all_mods.end())
			bro->loadMod(str, all_mods[str]);
		++h;
	} while(str != "");
};

void CoreModule::modulelist(Brobot* const bro, const Args& args) {
	if (args[5] != ".modlist" || args[1] != bro->stor->get("core.owner.nick") || args[2] != bro->stor->get("core.owner.ident") || args[3] != bro->stor->get("core.owner.host"))
		return;
	std::string target;
	if (args[4][0] == '#') {
		target = args[4];
	} else {
		target = args[1];
	}
	bro->irc->privmsg(target, "Modules loaded:");
	BOOST_FOREACH(std::string mod, bro->listMods())
		bro->irc->privmsg(target, " * "+mod);
};

void CoreModule::hooklist(Brobot* const bro, const Args& args) {
	if (args[5] != ".hooklist" || args[1] != bro->stor->get("core.owner.nick") || args[2] != bro->stor->get("core.owner.ident") || args[3] != bro->stor->get("core.owner.host"))
		return;
	std::string target;
	if (args[4][0] == '#') {
		target = args[4];
	} else {
		target = args[1];
	}
	bro->irc->privmsg(target, "Active hooks:");
	typedef std::pair<std::string, std::vector<std::string> > pair_t;
	BOOST_FOREACH(pair_t pair, bro->listHooks()) {
		BOOST_FOREACH(std::string name, pair.second) {
			bro->irc->privmsg(target, " * "+pair.first+" "+name);
		}
	}
};

void CoreModule::moduleunload(Brobot* const bro, const Args& args) {
	if (args[5].substr(0,11) != ".modunload " || args[1] != bro->stor->get("core.owner.nick") || args[2] != bro->stor->get("core.owner.ident") || args[3] != bro->stor->get("core.owner.host"))
		return;
	std::string target;
	if (args[4][0] == '#') {
		target = args[4];
	} else {
		target = args[1];
	}
	bool success = bro->unloadMod(args[5].substr(11,std::string::npos));
	if (success) {
		bro->irc->privmsg(target, "Successfully unloaded "+args[5].substr(11,std::string::npos));
	} else {
		bro->irc->privmsg(target, "Could not unload "+args[5].substr(11,std::string::npos));
	}
};

void CoreModule::moduleload(Brobot* const bro, const Args& args) {
	if (args[5].substr(0,9) != ".modload " || args[1] != bro->stor->get("core.owner.nick") || args[2] != bro->stor->get("core.owner.ident") || args[3] != bro->stor->get("core.owner.host"))
		return;
	std::string target;
	if (args[4][0] == '#') {
		target = args[4];
	} else {
		target = args[1];
	}
	std::map<std::string, BaseModule*>::iterator it = all_mods.find(args[5].substr(9,std::string::npos));
	if (it == all_mods.end()) {
		bro->irc->privmsg(target, "Could not find module "+args[5].substr(9,std::string::npos));
	} else {
		bool success = bro->loadMod(args[5].substr(9,std::string::npos), it->second);
		if (success) {
			bro->irc->privmsg(target, "Successfully loaded "+args[5].substr(9,std::string::npos));
		} else {
			bro->irc->privmsg(target, "Could not load "+args[5].substr(9,std::string::npos));
		}
	}
};

/* Warning:
 * using .unhook on a hook removes the hook
 * however, there is no way of storing the hook function pointer,
 * what this means is that any hooks unhooked by .unhook cannot be re-hooked manually
 * use at your own peril
 * syntax is: .unhook OnWHATEVER|[Module] HookName
 */
void CoreModule::hookunload(Brobot* const bro, const Args& args) {
	if (args[5].substr(0,8) != ".unhook " || args[1] != bro->stor->get("core.owner.nick") || args[2] != bro->stor->get("core.owner.ident") || args[3] != bro->stor->get("core.owner.host"))
		return;
	size_t separator = args[5].find("|");
	if (separator == std::string::npos)
		return;
	std::string target;
	if (args[4][0] == '#') {
		target = args[4];
	} else {
		target = args[1];
	}
	bro->unhook(args[5].substr(separator+1, std::string::npos), args[5].substr(8,separator-8));
	bro->irc->privmsg(target, "Unhooked "+args[5].substr(separator+1, std::string::npos)+".");
}