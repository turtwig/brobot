#ifndef UNO_H_INCLUDED
#define UNO_H_INCLUDED
#include <cstdlib> // for itoa/atoi
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // for random_shuffle
#include "../include/BaseModule.h"
#include "../include/Args.h"
#include "../include/brobot.h"

class Uno : public BaseModule {
	enum Cardtype { none_, red, blue, green, yellow };
	enum Cardattr { none, skip, reverse, drawtwo, drawfour, wild }; // drawfour is w+4, wild is just wild
	struct Card {
		short int number; // -1 = special card
		Cardtype type;
		Cardattr attr;
		std::string ascii[14];
		Card(Brobot* bro, short int num, Cardtype col, Cardattr spec, const std::string& fname);
		bool operator==(const Card& c) { return (number == c.number && type == c.type && attr == c.attr) || ((number == c.number) && (number == -1) && (attr == c.attr) && (attr == wild)) ||
												((number == c.number) && (number == -1) && (attr == c.attr) && (attr == drawfour)); };
	};
	struct Player {
		std::vector<Card> hand;
		bool has_drawn;
		std::string nick;
		Player(const std::string& n) : nick(n), has_drawn(false) {};
		bool operator==(const std::string& n) { return n == nick; };
	};
	// Cards
	const Card _0red, _1red, _2red, _3red, _4red, _5red, _6red, _7red, _8red, _9red, _p2red, _skipred, _reversered,
		_0blue, _1blue, _2blue, _3blue, _4blue, _5blue, _6blue, _7blue, _8blue, _9blue, _p2blue, _skipblue, _reverseblue,
		_0green, _1green, _2green, _3green, _4green, _5green, _6green, _7green, _8green, _9green, _p2green, _skipgreen, _reversegreen,
		_0yellow, _1yellow, _2yellow, _3yellow, _4yellow, _5yellow, _6yellow, _7yellow, _8yellow, _9yellow, _p2yellow, _skipyellow, _reverseyellow,
		_wild, _p4wild;
	std::vector<Player> players;
	std::vector<std::string> dropped_players;
	std::vector<Card> deck;
	std::vector<Card> discard;
	std::string channel;
	std::string uno_creator;
	std::vector<Player>::iterator current_player;
	unsigned short int started; // 0 = no game running, 1 = game started, players can join, 2 = game running, no one can join
	unsigned short int has_to_draw_cards; // number of cards to draw
	void reversePlayers();
	void nextPlayer() { if (++current_player == players.end()) current_player = players.begin(); };
	void swapDecks() { Card currdiscard = discard.back(); discard.pop_back(); deck.swap(discard); random_shuffle(deck.begin(), deck.end()); discard.push_back(currdiscard); };
	void printCard(Brobot* bro, const std::string& target, bool notice, const Card& card);
	void printCard(Brobot* bro, const std::string& target, bool notice, std::vector<Card> cards);
	void nextTurn(Brobot* bro);
	void endGame(Brobot* bro);
	public:
	Uno(Brobot* bro) : started(0), has_to_draw_cards(0),
						_0red(bro, 0, red, none, "0red.txt"), _1red(bro, 1, red, none, "1red.txt"), _2red(bro, 2, red, none, "2red.txt"), _3red(bro, 3, red, none, "3red.txt"),
						_4red(bro, 4, red, none, "4red.txt"), _5red(bro, 5, red, none, "5red.txt"), _6red(bro, 6, red, none, "6red.txt"), _7red(bro, 7, red, none, "7red.txt"),
						_8red(bro, 8, red, none, "8red.txt"), _9red(bro, 9, red, none, "9red.txt"), _p2red(bro, -1, red, drawtwo, "+2red.txt"), _skipred(bro, -1, red, skip, "skipred.txt"),
						_reversered(bro, -1, red, reverse, "reversered.txt"),
						_0blue(bro, 0, blue, none, "0blue.txt"), _1blue(bro, 1, blue, none, "1blue.txt"), _2blue(bro, 2, blue, none, "2blue.txt"), _3blue(bro, 3, blue, none, "3blue.txt"),
						_4blue(bro, 4, blue, none, "4blue.txt"), _5blue(bro, 5, blue, none, "5blue.txt"), _6blue(bro, 6, blue, none, "6blue.txt"), _7blue(bro, 7, blue, none, "7blue.txt"),
						_8blue(bro, 8, blue, none, "8blue.txt"), _9blue(bro, 9, blue, none, "9blue.txt"), _p2blue(bro, -1, blue, drawtwo, "+2blue.txt"), _skipblue(bro, -1, blue, skip, "skipblue.txt"),
						_reverseblue(bro, -1, blue, reverse, "reverseblue.txt"),
						_0green(bro, 0, green, none, "0green.txt"), _1green(bro, 1, green, none, "1green.txt"), _2green(bro, 2, green, none, "2green.txt"), _3green(bro, 3, green, none, "3green.txt"),
						_4green(bro, 4, green, none, "4green.txt"), _5green(bro, 5, green, none, "5green.txt"), _6green(bro, 6, green, none, "6green.txt"), _7green(bro, 7, green, none, "7green.txt"),
						_8green(bro, 8, green, none, "8green.txt"), _9green(bro, 9, green, none, "9green.txt"), _p2green(bro, -1, green, drawtwo, "+2green.txt"), _skipgreen(bro, -1, green, skip, "skipgreen.txt"),
						_reversegreen(bro, -1, green, reverse, "reversegreen.txt"),
						_0yellow(bro, 0, yellow, none, "0yellow.txt"), _1yellow(bro, 1, yellow, none, "1yellow.txt"), _2yellow(bro, 2, yellow, none, "2yellow.txt"), _3yellow(bro, 3, yellow, none, "3yellow.txt"),
						_4yellow(bro, 4, yellow, none, "4yellow.txt"), _5yellow(bro, 5, yellow, none, "5yellow.txt"), _6yellow(bro, 6, yellow, none, "6yellow.txt"), _7yellow(bro, 7, yellow, none, "7yellow.txt"),
						_8yellow(bro, 8, yellow, none, "8yellow.txt"), _9yellow(bro, 9, yellow, none, "9yellow.txt"), _p2yellow(bro, -1, yellow, drawtwo, "+2yellow.txt"), _skipyellow(bro, -1, yellow, skip, "skipyellow.txt"),
						_reverseyellow(bro, -1, yellow, reverse, "reverseyellow.txt"),
						_wild(bro, -1, red, wild, "wild.txt"), _p4wild(bro, -1, red, drawfour, "wild+4.txt") {};
	void onLoad(Brobot* bro);
	void onUnload(Brobot* bro);
	// Hooks
	void gameStart(Brobot* bro, Args& args);
	void help(Brobot* bro, Args& args);
	void joinHook(Brobot* bro, Args& args);
	void listPlayers(Brobot* bro, Args& args);
	void nickHook(Brobot* bro, Args& args);
	void partHook(Brobot* bro, Args& args);
	void quitHook(Brobot* bro, Args& args);
	void dropPlayer(Brobot* bro, Args& args);
	void gameEnd(Brobot* bro, Args& args);
	void startGame(Brobot* bro, Args& args);
	void showDiscard(Brobot* bro, Args& args);
	void passTurn(Brobot* bro, Args& args);
	void drawCard(Brobot* bro, Args& args);
	void showHand(Brobot* bro, Args& args);
	void playCard(Brobot* bro, Args& args);
	void challenge(Brobot* bro, Args& args);
};

#endif // UNO_H_INCLUDED