#include <cstdlib>
#include <ctime>
#include "brobot.h"
#include "modules/Core.h"
#include "modules/uno.h"
#include "modules/pokemon/pokemon.h"
#include "broperl.h"

Brobot* g_bro;

extern void ascii(Brobot* bot, const Args& args);
extern void jpegchat(Brobot* const bro, const Args& args);
extern void PerlTest(Brobot* const bro, const Args& args);

int main(int argc, char** argv, char** env) {
	std::srand(std::time(NULL));
	PERL_SYS_INIT3(&argc, &argv, &env);
	boost::asio::io_service io_service;
	boost::asio::ssl::context context(io_service, boost::asio::ssl::context::sslv23);
	std::string config;
	if (argc > 1)
		config = argv[1];
	else
		config = "..\\brobot.conf";
	try {
		g_bro = new Brobot(io_service, context, config);
		g_bro->hook("Ascii", "OnPRIVMSG", boost::bind(&ascii, g_bro, _1));
		g_bro->hook("JPEGTOCHAT", "OnPRIVMSG", boost::bind(&jpegchat, g_bro, _1));
		g_bro->hook("PerlTest", "OnPRIVMSG", boost::bind(&PerlTest, g_bro, _1));
		CoreModule core;
		Uno uno(g_bro);
		PokemonModule poke(g_bro);
		core.module("Core", &core);
		core.module("Uno", &uno);
		core.module("Pokémon", &poke);
		core.loadMods(g_bro);
		g_bro->start();
	} catch (std::exception& e) {
		//std::cerr << "Exception: " << e.what() << '\n';
	}
	return 0;
};