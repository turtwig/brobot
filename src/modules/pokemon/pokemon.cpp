#include "modules/pokemon/pokemon.h"

PokemonModule::PokemonModule(Brobot* const bro) {
	channel = bro->stor->get("module.pokemon.channel");
	asciidir = bro->stor->get("module.pokemon.asciidir");
	stordir = bro->stor->get("module.pokemon.basedir");
};

void PokemonModule::onLoad(Brobot* const bro) {
	bro->hook("[poke] Test", "OnPRIVMSG", boost::bind(&PokemonModule::testF, this, bro, _1));
};

void PokemonModule::onUnload(Brobot* const bro) {
	bro->unhook("[poke] Test", "OnPRIVMSG");
};

void PokemonModule::testF(Brobot* const bro, const Args& args) {
	if (args[4] != channel || args[5].substr(0, 6) != ".poke ")
		return;
	Pokemon front(args[5].substr(6, std::string::npos), "cocksucker ", 30, 5);
	Pokemon back(args[5].substr(6, std::string::npos), "faggot<3   ", 555, 55);
	front.hp = 10;
	back.hp = 55;
	BOOST_FOREACH(std::string s, renderBattle(front, back))
		bro->irc->privmsg(channel, s);
	BOOST_FOREACH(std::string s, renderMessage("I AM SO FUCKING GAY", "THOUGH SERIOUSLY WHAT WILL COCKSUCKER DO", true))
		bro->irc->privmsg(channel, s);
};

std::vector<std::string> PokemonModule::renderHP(const Pokemon& p) {
	std::vector<std::string> hpbar;
	if (p.nickname.size() != 11 || p.hp > p.maxhp || p.level > 999 || p.hp > 99999999 || p.maxhp > 99999999) {
		return hpbar;
	}	
	std::string levelstr;
	if (p.level < 10) {
		levelstr = "0"+boost::lexical_cast<std::string>(p.level)+"0,0X";
	} else if (p.level > 99) {
		levelstr = boost::lexical_cast<std::string>(p.level)+"0,0";
	} else {
		levelstr = boost::lexical_cast<std::string>(p.level)+"0,0X";
	}
	hpbar.push_back("1,0"+p.nickname+"0,0XXXXX1,0L.0,0X1,00"+levelstr);
	unsigned short int barcolor;
	if ((double)p.hp/(double)p.maxhp > 0.6) {
		barcolor = 9;
	} else if ((double)p.hp/(double)p.maxhp > 0.3) {
		barcolor = 8;
	} else {
		barcolor = 4;
	}
	std::string l = "1,1X"+boost::lexical_cast<std::string>(barcolor)+","+boost::lexical_cast<std::string>(barcolor);
	unsigned short int i;
	for (i = 0; i < (int)(((double)p.hp/(double)p.maxhp)*20.0); ++i) {
		l += "X";
	}
	l += "0,0";
	for (; i < 20; ++i) {
		l += "X";
	}
	l += "1,1X0,0";
	hpbar.push_back(l);
	std::string hpline = "1,1XX0,01"+boost::lexical_cast<std::string>(p.hp)+"1,1";
	for (unsigned short int j = 8; j > boost::lexical_cast<std::string>(p.hp).size(); --j) {
		hpline += "X";
	}
	hpline += "0,1/1,1";
	for (unsigned short int j = 8; j > boost::lexical_cast<std::string>(p.maxhp).size(); --j) {
		hpline += "X";
	}
	hpline += "0,01"+boost::lexical_cast<std::string>(p.maxhp)+"1,1XXX0,0";
	hpbar.push_back(hpline);
	return hpbar;
};

std::vector<std::string> PokemonModule::renderBattle(const Pokemon& front, const Pokemon& back) {
	std::vector<std::string> screen;
	std::ifstream fsprite, bsprite;
	fsprite.open((asciidir+front.frontfname).c_str(), std::ios::in);
	bsprite.open((asciidir+back.backfname).c_str(), std::ios::in);
	if (!(fsprite && bsprite))
		return screen;
	std::string btoppadding = "0,0XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	std::string fbotpadding = "0,0XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
	unsigned short int i;
	for (i = 0; i < 3; ++i) {
		screen.push_back(btoppadding);
	}
	BOOST_FOREACH(std::string hpline, renderHP(front)) {
		hpline = "0,0XXXXXXXXXXXXXXXXXXXXXXX"+hpline+"XXXXXXXXXXXXXXX";
		screen.push_back(hpline);
	}
	for (; i < 14; ++i) {
		screen.push_back(btoppadding);
	}
	std::string line;
	while (!bsprite.eof()) {
		std::getline(bsprite, line);
		screen.push_back(line);
	}
	bsprite.close();
	i = 0;
	while (!fsprite.eof()) {
		std::getline(fsprite, line);
		screen[i] += line;
		++i;
	}
	for (; i < screen.size()-6; ++i) {
		screen[i] += fbotpadding;
	}
	BOOST_FOREACH(std::string hpline, renderHP(back)) {
		hpline = "0,0XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"+hpline+"XXXXXXXXXXXXXXXXXXXXXXX";
		screen[i++] += hpline;
	}
	for (; i < screen.size(); ++i) {
		screen[i] += fbotpadding;
	}
	return screen;
};

std::vector<std::string> PokemonModule::renderMessage(std::string m1, std::string m2, bool actionBox) {
	std::vector<std::string> msg;
	if (!actionBox) {
		if (m1.size() > 90 || m2.size() > 90)
			return msg;
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XX1,1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX14,14XX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		m1 += "0,0";
		for (unsigned short int i = 90; 90 > m1.size(); ++i)
			m1 += 'X';
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XX1,1XX0,0XX1,0"+m1+"XX1,1XX14,14XX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		m2 += "0,0";
		for (unsigned short int i = 90; 90 > m2.size(); ++i)
			m2 += 'X';
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XX1,1XX0,0XX1,0"+m2+"XX1,1XX14,14XX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XX1,1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX14,14XX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	} else {
		if (m1.size() > 70 || m2.size() > 70)
			return msg;
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0,0XX1,1XXXXXXXXXXXXXXXXXX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX1,1XX0,0XX1,1XX0,0XXXXXXXXXXXXXX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XX1,1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX14,14XX1,1XX0,0XX1,1XX0,0X1,0FIGHT0,0XX1,0PKMN0,0XX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		m1 += "0,0";
		for (unsigned short int i = 70; 70 > m1.size(); ++i)
			m1 += 'X';
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XX1,1XX0,0XX1,0"+m1+"XX1,1XX14,14XX1,1XX0,0XX1,1XX0,0XXXXXXXXXXXXXX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		m2 += "0,0";
		for (unsigned short int i = 70; 70 > m2.size(); ++i)
			m2 += 'X';
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XX1,1XX0,0XX1,0"+m2+"XX1,1XX14,14XX1,1XX0,0XX1,1XX0,0XX1,0ITEM0,0XXX1,0RUN0,0XX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XX1,1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX14,14XX1,1XX0,0XX1,1XX0,0XXXXXXXXXXXXXX1,1XX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XX14,14XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX1,1XX0,0XX1,1XXXXXXXXXXXXXXXXXX0,0XXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXX1,1XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0,0XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
		msg.push_back("0,0XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	}
	return msg;
};