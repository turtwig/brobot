#ifndef UNO_H_INCLUDED
#define UNO_H_INCLUDED
#include <cstdlib> // for itoa/atoi
#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // for random_shuffle
#include <iterator>
#include <sstream>
#include <boost/timer.hpp> // for turn timer
#include "BaseModule.h"
#include "Args.h"
#include "brobot.h"

extern void ascii(Brobot* bot, Args& args);

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
		bool has_drawn, has_challenged;
		std::string nick;
		Player(const std::string& n) : nick(n), has_drawn(false), has_challenged(false) {};
		bool operator==(const std::string& n) { return n == nick; };
	};
	struct Score {
		std::string nick;
		std::string channel;
		unsigned int total_score;
		unsigned int win_count;
		Score(const std::string& n, const std::string& ch, unsigned int sc, unsigned int wn) : nick(n), channel(ch), total_score(sc), win_count(wn) {};
		bool operator<(const Score& score) { if (score.total_score == total_score) { return score.win_count < win_count; } else { return score.total_score < total_score; } };
		bool operator==(const Score& score) { return nick == score.nick; };
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
	std::vector<Score> scores;
	boost::timer turntimer;
	unsigned short int started; // 0 = no game running, 1 = game started, players can join, 2 = game running
	unsigned short int has_to_draw_cards; // number of cards to draw
	void reversePlayers();
	void nextPlayer() { if (++current_player == players.end()) current_player = players.begin(); };
	void swapDecks() { Card currdiscard = discard.back(); discard.pop_back(); deck.swap(discard); random_shuffle(deck.begin(), deck.end()); discard.push_back(currdiscard); };
	void printCard(Brobot* bro, const std::string& target, bool notice, const Card& card);
	void printCard(Brobot* bro, const std::string& target, bool notice, std::vector<Card> cards);
	void nextTurn(Brobot* bro);
	void endGame(Brobot* bro, bool updatescore);
	void updateScore(Brobot* bro, const std::string& nick, unsigned int score);
	public:
	Uno(Brobot* bro);
	void onLoad(Brobot* bro);
	void onUnload(Brobot* bro);
	// Hooks
	void gameStart(Brobot* bro, Args& args);
	void help(Brobot* bro, Args& args);
	void showScores(Brobot* bro, Args& args);
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
	void skipTurn(Brobot* bro, Args& args);
};

#endif // UNO_H_INCLUDED