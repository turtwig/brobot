#include "brobot.h"
#include <fstream>

void ascii(Brobot* bot, const Args& args) {
	// args[0] = raw line
	// args[1] = nick
	// args[2] = ident
	// args[3] = host
	// args[4] = target
	// args[5] = entire message
	// path = taken from config's module.ascii.dir
	if (args[5].substr(0,7) != ".ascii ")
		return; // the privmsg was not aimed at us
	if ((args[5].find("..\\") != std::string::npos) || (args[5].find("../") != std::string::npos))
		return; // someone trying to do something like .ascii ..\dicks
	std::string target;
	if (args[4][0] == '#') { // message in a channel
		target = args[4];
	} else { // private message to ourselves
		target = args[1];
	}
	static const std::string dir = bot->stor->get("module.ascii.dir"); // get the path of the files
	std::string path = dir + args[5].substr(7, std::string::npos) + ".txt";
	std::ifstream file;
	file.open(path.c_str(), std::ios::in);
	if (!file) {
		bot->irc->privmsg(target, "Could not open file.");
		return;
	}
	std::string line;
	while (!file.eof()) {
		std::getline(file, line);
		bot->irc->privmsg(target, line);
	}
	bot->irc->privmsg(target, "(ps: internets is pro)");
};

extern int jpegmain(int argc, char **argv, char **envp);

void jpegchat(Brobot* const bro, const Args& args) {
	if (args[5] != ".jpegtochat")
		return;
	char* dicks[] = { "jpegtochat.exe", "-c", "mirc", "-d", "none", "-x", "80", "-y", "100", "file.jpg" };
	jpegmain(10, dicks, NULL);
};
