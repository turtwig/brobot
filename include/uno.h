#ifndef UNO_H_INCLUDED
#define UNO_H_INCLUDED
#include <cstdlib> // for itoa
#include <string>
#include <vector>
#include <algorithm>
#include "../include/BaseModule.h"
#include "../include/Args.h"
#include "../include/brobot.h"

const std::string ascii_dir = "C:\\Users\\Admin\\Desktop\\ascii\\uno\\";

enum Cardtype { none_, red, blue, green, yellow };
enum Cardattr { none, skip, reverse, drawtwo, drawfour, wild }; // drawfour is w+4, wild is just wild

struct Card {
	short int number; // -1 = special card
	Cardtype type;
	Cardattr attr;
	std::string ascii[14];
	Card(short int num, Cardtype col, Cardattr spec, const std::string& fname);
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

class Uno : public BaseModule {
	std::vector<Player> players;
	std::vector<std::string> dropped_players;
	std::vector<Card> deck;
	std::vector<Card> discard;
	std::string channel;
	std::string uno_creator;
	std::vector<Player>::iterator current_player;
	unsigned short int started; // 0 = no game running, 1 = game started, players can join, 2 = game running, no one can join
	unsigned short int has_to_draw_cards; // number of cards to draw
	public:
	Uno() : started(0), has_to_draw_cards(0) {};
	void onLoad(Brobot* bro);
	void onUnload(Brobot* bro);
	void reversePlayers();
	void nextPlayer() { if (++current_player == players.end()) current_player = players.begin(); };
	void swapDecks() { Card currdiscard = discard.back(); discard.pop_back(); deck.swap(discard); random_shuffle(deck.begin(), deck.end()); discard.push_back(currdiscard); };
	void printCard(Brobot* bro, const std::string& target, bool notice, const Card& card);
	void printCard(Brobot* bro, const std::string& target, bool notice, std::vector<Card> cards);
	void gameStart(Brobot* bro, Args& args);
	void help(Brobot* bro, Args& args);
	void joinHook(Brobot* bro, Args& args);
	void listPlayers(Brobot* bro, Args& args);
	void nickHook(Brobot* bro, Args& args);
	void partHook(Brobot* bro, Args& args);
	void quitHook(Brobot* bro, Args& args);
	void dropPlayer(Brobot* bro, Args& args);
	void endGame(Brobot* bro);
	void gameEnd(Brobot* bro, Args& args); // handles .endgame
	void startGame(Brobot* bro, Args& args); // handles .start rather than .uno
	void showDiscard(Brobot* bro, Args& args);
	void nextTurn(Brobot* bro);
	void passTurn(Brobot* bro, Args& args);
	void drawCard(Brobot* bro, Args& args);
	void showHand(Brobot* bro, Args& args);
	void playCard(Brobot* bro, Args& args);
};

// Cards definition
// Red
const Card _0red(0, red, none, "0red.txt");
const Card _1red(1, red, none, "1red.txt");
const Card _2red(2, red, none, "2red.txt");
const Card _3red(3, red, none, "3red.txt");
const Card _4red(4, red, none, "4red.txt");
const Card _5red(5, red, none, "5red.txt");
const Card _6red(6, red, none, "6red.txt");
const Card _7red(7, red, none, "7red.txt");
const Card _8red(8, red, none, "8red.txt");
const Card _9red(9, red, none, "9red.txt");
const Card _p2red(-1, red, drawtwo, "+2red.txt");
const Card _skipred(-1, red, skip, "skipred.txt");
const Card _reversered(-1, red, reverse, "reversered.txt");
// Blue
const Card _0blue(0, blue, none, "0blue.txt");
const Card _1blue(1, blue, none, "1blue.txt");
const Card _2blue(2, blue, none, "2blue.txt");
const Card _3blue(3, blue, none, "3blue.txt");
const Card _4blue(4, blue, none, "4blue.txt");
const Card _5blue(5, blue, none, "5blue.txt");
const Card _6blue(6, blue, none, "6blue.txt");
const Card _7blue(7, blue, none, "7blue.txt");
const Card _8blue(8, blue, none, "8blue.txt");
const Card _9blue(9, blue, none, "9blue.txt");
const Card _p2blue(-1, blue, drawtwo, "+2blue.txt");
const Card _skipblue(-1, blue, skip, "skipblue.txt");
const Card _reverseblue(-1, blue, reverse, "reverseblue.txt");
// Green
const Card _0green(0, green, none, "0green.txt");
const Card _1green(1, green, none, "1green.txt");
const Card _2green(2, green, none, "2green.txt");
const Card _3green(3, green, none, "3green.txt");
const Card _4green(4, green, none, "4green.txt");
const Card _5green(5, green, none, "5green.txt");
const Card _6green(6, green, none, "6green.txt");
const Card _7green(7, green, none, "7green.txt");
const Card _8green(8, green, none, "8green.txt");
const Card _9green(9, green, none, "9green.txt");
const Card _p2green(-1, green, drawtwo, "+2green.txt");
const Card _skipgreen(-1, green, skip, "skipgreen.txt");
const Card _reversegreen(-1, green, reverse, "reversegreen.txt");
// Yellow
const Card _0yellow(0, yellow, none, "0yellow.txt");
const Card _1yellow(1, yellow, none, "1yellow.txt");
const Card _2yellow(2, yellow, none, "2yellow.txt");
const Card _3yellow(3, yellow, none, "3yellow.txt");
const Card _4yellow(4, yellow, none, "4yellow.txt");
const Card _5yellow(5, yellow, none, "5yellow.txt");
const Card _6yellow(6, yellow, none, "6yellow.txt");
const Card _7yellow(7, yellow, none, "7yellow.txt");
const Card _8yellow(8, yellow, none, "8yellow.txt");
const Card _9yellow(9, yellow, none, "9yellow.txt");
const Card _p2yellow(-1, yellow, drawtwo, "+2yellow.txt");
const Card _skipyellow(-1, yellow, skip, "skipyellow.txt");
const Card _reverseyellow(-1, yellow, reverse, "reverseyellow.txt");
// Wilds (colors aren't checked in wilds, they are set)
const Card _wild(-1, red, wild, "wild.txt");
const Card _p4wild(-1, red, drawfour, "wild+4.txt");

#endif // UNO_H_INCLUDED