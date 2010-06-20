#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../include/brobot.h"
#include "../include/Core.h"
#include "../include/uno.h"
#include "../include/perl.h"

Brobot* g_bro;

extern void ascii(Brobot* bot, Args& args);
extern void PerlTest(Brobot* bro, Args& args);

int main(int argc, char** argv, char** env) {
	PERL_SYS_INIT3(&argc, &argv, &env);
	std::srand(std::time(NULL));
    try {
        boost::asio::io_service io_service;
        boost::asio::ssl::context context(io_service, boost::asio::ssl::context::sslv23);
		std::string config;
		if (argc > 1)
			config = argv[1];
		else
			config = "..\\brobot.conf";
        g_bro = new Brobot(io_service, context, config);
        g_bro->hook("Ascii", "OnPRIVMSG", boost::bind(&ascii, g_bro, _1));
		g_bro->hook("PerlTest", "OnPRIVMSG", boost::bind(&PerlTest, g_bro, _1));
        CoreModule core;
        g_bro->loadMod("Core", &core);
		Uno unomod;
		g_bro->loadMod("Uno", &unomod);
        g_bro->start();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    return 0;
};

//Brobot* current_bro() { return &g_bro; }