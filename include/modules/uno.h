#ifndef UNO_H_INCLUDED
#define UNO_H_INCLUDED

#include <string>
#include <vector>
#include <fstream>
#include <algorithm> // for random_shuffle
#include <iterator>
#include <sstream>
#include <boost/timer.hpp> // for turn timer
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "BaseModule.h"
#include "brobot.h"

class Uno : public BaseModule {
	enum Cardtype { none_, red, blue, green, yellow };
	enum Cardattr { none, skip, reverse, drawtwo, drawfour, wild }; // drawfour is w+4, wild is just wild
	Cardtype strtocol(const std::string& str);
	Cardattr strtoattr(const std::string& str);
	struct Card {
		short int number; // -1 = special card
		Cardtype type;
		Cardattr attr;
		std::string ascii[14];
		Card(Brobot* const bro, short int num, Cardtype col, Cardattr spec, const std::string& fname);
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
	std::vector<Player> players; // players
	std::vector<std::string> dropped_players; // people that .drop out
	std::vector<Card> deck; // deck of cards
	std::vector<Card> discard; // discard pile
	std::string channel; // channel the game is on
	std::string uno_creator; // who created the game
	std::vector<Player>::iterator current_player; // playing player
	std::vector<Score> scores; // players' score
	boost::timer turntimer; // timer before .skip is available
	unsigned short int started; // 0 = no game running, 1 = game started, players can join, 2 = game running
	unsigned short int has_to_draw_cards; // number of cards to draw in case of +2/+4
	bool locked;
	void reversePlayers();
	void printCard(Brobot* const bro, const std::string& target, bool notice, const Card& card);
	void printCard(Brobot* const bro, const std::string& target, bool notice, std::vector<Card> cards);
	void nextTurn(Brobot* const bro);
	void endGame(Brobot* const bro, bool updatescore);
	void updateScore(Brobot* const bro, const std::string& nick, unsigned int score);
	// inlines
	inline void nextPlayer() {
		if (++current_player == players.end())
			current_player = players.begin();
	};
	inline bool swapDecks(Brobot* const bro) {
		if (deck.empty() && (discard.size() == 1 || started == 1)) {
			bro->irc->privmsg(channel, "I ran out of cards! Stop hacking you fucking faggot!");
			endGame(bro, false);
			return false;
		}
		Card currdiscard = discard.back();
		discard.pop_back();
		deck.swap(discard);
		random_shuffle(deck.begin(), deck.end());
		discard.push_back(currdiscard);
		return true;
	};
	inline void pl_card(Brobot* const bro, const std::vector<Card>::iterator& it) {
		bro->irc->privmsg(channel, ""+current_player->nick+" plays:");
		printCard(bro, channel, false, *it);
		discard.push_back(*it);
		current_player->hand.erase(it);
		current_player->has_drawn = false;
		current_player->has_challenged = false;
	};
	public:
	Uno(Brobot* const bro);
	void onLoad(Brobot* const bro);
	void onUnload(Brobot* const bro);
	// Hooks
	void gameStart(Brobot* const bro, const Args& args);
	void help(Brobot* const bro, const Args& args);
	void showScores(Brobot* const bro, const Args& args);
	void joinHook(Brobot* const bro, const Args& args);
	void listPlayers(Brobot* const bro, const Args& args);
	void gameEnd(Brobot* const bro, const Args& args);
	void startGame(Brobot* const bro, const Args& args);
	void showDiscard(Brobot* const bro, const Args& args);
	void passTurn(Brobot* const bro, const Args& args);
	void drawCard(Brobot* const bro, const Args& args);
	void showHand(Brobot* const bro, const Args& args);
	void playCard(Brobot* const bro, const Args& args);
	void lockGame(Brobot* const bro, const Args& args);
	void challenge(Brobot* const bro, const Args& args);
	void skipTurn(Brobot* const bro, const Args& args);
	void nickHook(Brobot* const, const Args& args);
	void partHook(Brobot* const bro, const Args& args);
	void quitHook(Brobot* const bro, const Args& args);
	void dropPlayer(Brobot* const bro, const Args& args);
};

#endif // UNO_H_INCLUDED