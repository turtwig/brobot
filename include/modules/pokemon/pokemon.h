#ifndef POKEMON_H_INCLUDED
#define POKEMON_H_INCLUDED

#include "BaseModule.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include "brobot.h"

struct Pokemon {
	std::string frontfname;
	std::string backfname;
	std::string nickname;
	unsigned int hp;
	unsigned int maxhp;
	unsigned int level;
	Pokemon(const std::string& fname, const std::string& nick, const unsigned int& mhp, const unsigned int& level) : nickname(nick), hp(mhp), maxhp(mhp), level(level) {
		frontfname = fname+"front.txt"; backfname = fname+"back.txt";
	};
};

class PokemonModule : public BaseModule {
	std::string channel, stordir, asciidir;
	std::vector<std::string> renderHP(const Pokemon& p);
	std::vector<std::string> renderBattle(const Pokemon& front, const Pokemon& back);
	public:
	PokemonModule(Brobot* const bro);
	void onLoad(Brobot* const bro);
	void onUnload(Brobot* const bro);
	// Hooks
	void testF(Brobot* const bro, const Args& args);
};

#endif // POKEMON_H_INCLUDED