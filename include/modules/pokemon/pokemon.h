#ifndef POKEMON_H_INCLUDED
#define POKEMON_H_INCLUDED

#include "BaseModule.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <vector>
#include "brobot.h"

/*
perhaps use boost::shared_ptrs so i don't have to deal with memory allocation/destruction of pokemon and whatnot
each pokemon has a Stats struct inside that lists the pokemon's literal stats (stats, hp, maxhp, experience, experience for next level)
each pokemon has an array of two Types, at least the first of which has to be set
base pokemon stats/names are stored in basedir/pokemon, and loaded whenever a trainer is assigned a pokemon (at which point stats are randomized and whatnot
each pokemon has an array of four Attacks, each attack is responsible for calculating its damage given access to the pokemon's Stats
on level up, look up the on-disk data for the pokemon and see whether or not the pokemon should learn any moves or evolve
in case of an evolution, proceed as normal as a level up, but update the sprite filenames (IMPORTANT: CHECK BASE STATS OF EVOLUTIONS TO MAKE SURE THAT I SHOULDN'T BE INCREASING STATS SOMEHOW)
also check for type changes and whatnot, perhaps make it so it creates a pokemon of the evolved type, but with the stat calculation making sure the base stats apply and the randomized stats don't go lower than the previous stats
make sure a pokemon's stats are proportional to its level
each Trainer has a vector of Pokemon, and another vector of pokemon in the party
^ make sure both vectors are of shared_ptrs, so changes to pokemon in the party effects the overall pokemon of the trainer
for now provide facilities for assigning a single pokemon to each player, and starting a battle between them
Trainers are only loaded in-memory on creation (at which point they are serialized into the disk and removed from memory) and during battles
Pokemon are local to their Trainers
Trainers are nick-bound, with possible hostmask/password authentication if needed
each Attack for each pokemon is responsible for listing wheter it is enabled or not, returning its name, and calculating damage when it is done
each scene is rendered in a single function, which calls renderBattle and renderMessage, and prints out the result
users are then expected to respond to a command (i.e. .attack ATTACK NUMBER, .run, .pkmn, etc), at which point the battle state is changed, and the screen is re-rendered and shown
a single hook is responsible for handling all the available commands, by checking the battle state and listing available commands and printing the screen and changing it when appropriate
since it will all be within a single channel, commands will not have . in front of them, and are limited to whoever's turn it currently is
no .skip-like functionality will be provided, if players change nick, track them for the duration of the battle, but store the original nick
as an alternative, trainers can be nickgroup-bound, and password authenticated, which might be a better option since people are whining retards
as an option, (e.g. "module.pokemon.channelmodes 1"), set +m and +v accordingly like idlerpg would
asciidir/whateverfront|back.txt - sprites
basedir/pokemon/whateverbase - base stats and anything related to pokemon creation
basedir/pokemon/whateverlevel - table of attacks gained through leveling and possible evolutions
basedir/trainers/whatever - trainer data
everything will be serialized into files before being destroyed as a means for storing on-disk data
nothing uneeded remains on memory (compare this against Uno's alternative of storing cards in-memory even when nothing is going on)
everything is loaded from disk as needed by the appropriate functions
attacks are also responsible for listing its own types and whatnot
the battle progresses through steps, e.g., battle starts, from the two trainers taking place in it, select one as current trainer
the main hook function is responsible for parsing input and advancing the screen/state each time, as soon as he attacks, the next player is responsible for advancing the screen another step and then selecting his own action
go on as necessary
PokemonModule stores a pointer to two Trainers which partake in the battle and a convenience pointer current_trainer. in a non-battle context, essentially both pointers are set to the same value (for safety)
whenever a battle ends, PokemonModule is responsible for serializing the trainers into files and deleting the pointers.
to allow players to see things without things scrolling too fast, screens move in steps.
a BattleState enum is local to the PokemonModule and is responsible for letting the hook function know the current state the battle is in (e.g. attack screen, party screen, item screen, attack message screen, etc)
local variables for storage inside PokemonModule for convenience are allowed (e.g. currentMessage for attacks and whatnot)
a Trainer has a pointer to the pokemon he has out at the moment, which should be used extensively by the hook function to signal the pokemon to do things
essentially all bulk work is in the PokemonModule hook function, which is responsible for updating battle state, transmitting data (damage and actions) between players and rendering and command handling
each Trainer/Pokemon has functions responsible for returning those values used by the hook function, in the sense that a Trainer does not have access to other Trainers and whatnot
(this enables utmost simple serialization of Trainers into files)
a serialized trainer has his pokemon serialized with him, and loaded back onto memory for convenience
the Trainer has his information stored inside him, such as items and party pokemon and whatnot.
since memory is an issue (because this is a lot of data and it is completely meaningless unless a battle is going on), watch out for memory leaks and ensure everything is properly serialized and saved
of course the hook function is responsible for the bulk work, but those can be offloaded into private functions obviously
separate sources so that:
pokemon.h and pokemon.cpp are PokemonModule exclusive
stats.h define the structures used for Stats and Types
attack.h/cpp define the Attack class
trainer.h/cpp define the Player class
poke.h/cpp define the pokemon structure
PokemonModule is responsible for trainer/pokemon creation
other renderWhatever functions may be required for rendering other information (such as pokemon left and whatnot)
also see with faggot if we can get a trainer front/back sprite
*/

struct Pokemon {
	std::string frontfname;
	std::string backfname;
	std::string nickname;
	//Stats stats;
	Pokemon(const std::string& fname, const std::string& nick, const unsigned int& mhp, const unsigned int& level) : nickname(nick), hp(mhp), maxhp(mhp), level(level) {
		frontfname = fname+"front.txt"; backfname = fname+"back.txt";
	};
};

class PokemonModule : public BaseModule {
	std::string channel, stordir, asciidir;
	std::vector<std::string> renderHP(const Pokemon& p); // renders HP bar
	std::vector<std::string> renderBattle(const Pokemon& front, const Pokemon& back); // renders entire battle scene (including hp bars)
	std::vector<std::string> renderMessage(const std::vector<std::string>& messages, bool actionBox); // renders the message box portion of the screen, if actionBox is true, it renders the right-hand size action selector
	public:
	PokemonModule(Brobot* const bro);
	void onLoad(Brobot* const bro);
	void onUnload(Brobot* const bro);
	// Hooks
	void testF(Brobot* const bro, const Args& args);
};

#endif // POKEMON_H_INCLUDED