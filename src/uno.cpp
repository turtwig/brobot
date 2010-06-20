#include "..\include\uno.h"
#include <fstream>

Card::Card(short int num, Cardtype col, Cardattr spec, const std::string& fname) : number(num), type(col), attr(spec) {
	std::fstream file;
	file.open((ascii_dir+fname).c_str(), std::ios::in);
    if (file) {
		for (int i = 0; i < 14; ++i) {
            std::getline(file, ascii[i]);
        }
	}
	file.close();
};

void Uno::printCard(Brobot* bro, const std::string& target, bool notice, Card card) { // single card
	for (int i = 0; i < 14; ++i) {
		if (notice) {
			bro->irc->notice(target, card.ascii[i]);
		} else {
			bro->irc->privmsg(target, card.ascii[i]);
		}
	}
};

void Uno::printCard(Brobot* bro, const std::string& target, bool notice, std::vector<Card> cards) { // multiple cards
	while(!cards.empty()) {
		std::vector<Card>::iterator it;
		if (cards.size() > 5) {
			it = cards.begin()+5;
		} else {
			it = cards.end();
		}
		std::vector<Card> tmpcards(cards.begin(), it);
		cards.erase(cards.begin(), it);
		for (int i = 0; i < 14; ++i) {
			std::string line;
			BOOST_FOREACH( Card c, tmpcards ) {
				line += c.ascii[i];
				line += " ";
			}
			line.erase(line.size()-2, std::string::npos);
			if (notice) {
				bro->irc->notice(target, line);
			} else {
				bro->irc->privmsg(target, line);
			}
		}
		if (!cards.empty()) {
			if (notice) {
				bro->irc->notice(target, " ");
			} else {
				bro->irc->privmsg(target, " ");
			}
		}
	}
};

void Uno::onLoad(Brobot* bro) {
	bro->hook("[uno] gameStart", "OnPRIVMSG", boost::bind(&Uno::gameStart, this, bro, _1));
	bro->hook("[uno] joinHook", "OnPRIVMSG", boost::bind(&Uno::joinHook, this, bro, _1));
	bro->hook("[uno] listPlayers", "OnPRIVMSG", boost::bind(&Uno::listPlayers, this, bro, _1));
	bro->hook("[uno] nickHook", "OnNICK", boost::bind(&Uno::nickHook, this, bro, _1));
	bro->hook("[uno] partHook", "OnPART", boost::bind(&Uno::partHook, this, bro, _1));
	bro->hook("[uno] quitHook", "OnQUIT", boost::bind(&Uno::quitHook, this, bro, _1));
	bro->hook("[uno] dropPlayer", "OnPRIVMSG", boost::bind(&Uno::dropPlayer, this, bro, _1));
	bro->hook("[uno] startGame", "OnPRIVMSG", boost::bind(&Uno::startGame, this, bro, _1));
	bro->hook("[uno] showDiscard", "OnPRIVMSG", boost::bind(&Uno::showDiscard, this, bro, _1));
};

void Uno::onUnload(Brobot* bro) {
	bro->unhook("[uno] gameStart", "OnPRIVMSG");
	bro->unhook("[uno] joinHook", "OnPRIVMSG");
	bro->unhook("[uno] listPlayers", "OnPRIVMSG");
	bro->unhook("[uno] nickHook", "OnNICK");
	bro->unhook("[uno] partHook", "OnPART");
	bro->unhook("[uno] quitHook", "OnQUIT");
	bro->unhook("[uno] dropPlayer", "OnPRIVMSG");
	bro->unhook("[uno] startGame", "OnPRIVMSG");
	bro->unhook("[uno] showDiscard", "OnPRIVMSG");
};

void Uno::gameStart(Brobot* bro, Args& args) {
	if (args[5] != ".uno" || args[4][0] != '#')
		return;
	if (started != 0) {
		bro->irc->privmsg(args[4], "Game in "+channel+" already in progress!");
		return;
	}
	started = 1;
	channel = args[4];
	bro->irc->privmsg(args[4], "Starting 4U8N3O12! game in "+args[4]+"!");
	bro->irc->privmsg(args[4], "Say .join to join in and .start to start the game!");
	// Deck cards
	// 19 red cards
	deck.push_back(_0red);
	deck.push_back(_1red);
	deck.push_back(_2red);
	deck.push_back(_3red);
	deck.push_back(_4red);
	deck.push_back(_5red);
	deck.push_back(_6red);
	deck.push_back(_7red);
	deck.push_back(_8red);
	deck.push_back(_9red);
	deck.push_back(_1red);
	deck.push_back(_2red);
	deck.push_back(_3red);
	deck.push_back(_4red);
	deck.push_back(_5red);
	deck.push_back(_6red);
	deck.push_back(_7red);
	deck.push_back(_8red);
	deck.push_back(_9red);
	// 19 blue cards
	deck.push_back(_0blue);
	deck.push_back(_1blue);
	deck.push_back(_2blue);
	deck.push_back(_3blue);
	deck.push_back(_4blue);
	deck.push_back(_5blue);
	deck.push_back(_6blue);
	deck.push_back(_7blue);
	deck.push_back(_8blue);
	deck.push_back(_9blue);
	deck.push_back(_1blue);
	deck.push_back(_2blue);
	deck.push_back(_3blue);
	deck.push_back(_4blue);
	deck.push_back(_5blue);
	deck.push_back(_6blue);
	deck.push_back(_7blue);
	deck.push_back(_8blue);
	deck.push_back(_9blue);
	// 19 green cards
	deck.push_back(_0green);
	deck.push_back(_1green);
	deck.push_back(_2green);
	deck.push_back(_3green);
	deck.push_back(_4green);
	deck.push_back(_5green);
	deck.push_back(_6green);
	deck.push_back(_7green);
	deck.push_back(_8green);
	deck.push_back(_9green);
	deck.push_back(_1green);
	deck.push_back(_2green);
	deck.push_back(_3green);
	deck.push_back(_4green);
	deck.push_back(_5green);
	deck.push_back(_6green);
	deck.push_back(_7green);
	deck.push_back(_8green);
	deck.push_back(_9green);
	// 19 yellow cards
	deck.push_back(_0yellow);
	deck.push_back(_1yellow);
	deck.push_back(_2yellow);
	deck.push_back(_3yellow);
	deck.push_back(_4yellow);
	deck.push_back(_5yellow);
	deck.push_back(_6yellow);
	deck.push_back(_7yellow);
	deck.push_back(_8yellow);
	deck.push_back(_9yellow);
	deck.push_back(_1yellow);
	deck.push_back(_2yellow);
	deck.push_back(_3yellow);
	deck.push_back(_4yellow);
	deck.push_back(_5yellow);
	deck.push_back(_6yellow);
	deck.push_back(_7yellow);
	deck.push_back(_8yellow);
	deck.push_back(_9yellow);
	// Draw two cards
	deck.push_back(_p2red);
	deck.push_back(_p2red);
	deck.push_back(_p2blue);
	deck.push_back(_p2blue);
	deck.push_back(_p2green);
	deck.push_back(_p2green);
	deck.push_back(_p2yellow);
	deck.push_back(_p2yellow);
	// Reverse cards
	deck.push_back(_skipred);
	deck.push_back(_skipred);
	deck.push_back(_skipblue);
	deck.push_back(_skipblue);
	deck.push_back(_skipgreen);
	deck.push_back(_skipgreen);
	deck.push_back(_skipyellow);
	deck.push_back(_skipyellow);
	// Skip cards
	deck.push_back(_reversered);
	deck.push_back(_reversered);
	deck.push_back(_reverseblue);
	deck.push_back(_reverseblue);
	deck.push_back(_reversegreen);
	deck.push_back(_reversegreen);
	deck.push_back(_reverseyellow);
	deck.push_back(_reverseyellow);
	// Wild cards
	deck.push_back(_wild);
	deck.push_back(_wild);
	deck.push_back(_wild);
	deck.push_back(_wild);
	// Wild+4 cards
	deck.push_back(_p4wild);
	deck.push_back(_p4wild);
	deck.push_back(_p4wild);
	deck.push_back(_p4wild);
	// shuffle the deck
	random_shuffle(deck.begin(), deck.end());
	Player p(args[1]);
	for (int i = 0; i < 7; ++i) {
		p.hand.push_back(deck.back());
		deck.pop_back();
	}
	players.push_back(p);
	printCard(bro, args[1], true, p.hand);
};

void Uno::endGame(Brobot* bro) {
	if (started == 0)
		return;
	bro->irc->privmsg(channel, "4U8N3O12! game in "+channel+" ended!");
	if (players.size() > 1) {
		bro->irc->privmsg(channel, "Winner is "+current_player->nick+"!");
		players.erase(current_player);
		BOOST_FOREACH(Player p, players) {
			bro->irc->privmsg(channel, ""+p.nick+" still had:");
			printCard(bro, channel, false, p.hand);
		}
	}
	started = 0;
	channel.clear();
	players.clear();
	dropped_players.clear();
	deck.clear();
	discard.clear();
	current_player = players.end();
};

void Uno::startGame(Brobot* bro, Args& args) {
	if (args[5] != ".start" || args[4] != channel)
		return;
	if (started == 2) {
		bro->irc->privmsg(channel, "Game in "+channel+" is already in progress!");
		return;
	}
	if (players.size() < 2 || players.size() > 10) {
		bro->irc->privmsg(channel, "Not enough players!");
		return;
	}
	bro->irc->privmsg(channel, "Starting 4U8N3O12! game in "+channel+"!");
	current_player = players.begin();
	std::string pstring;
	BOOST_FOREACH( Player p, players)
		pstring += p.nick+" ";
	bro->irc->privmsg(args[4], "Playing order: "+pstring);
	discard.push_back(deck.back());
	deck.pop_back();
	started = 2;
	nextTurn(bro);
};

void Uno::nextTurn(Brobot* bro) {
	bro->irc->privmsg(channel, "It is now "+current_player->nick+"'s turn!");
	bro->irc->privmsg(channel, "Current discard:");
	printCard(bro, channel, false, discard.back());
};


void Uno::joinHook(Brobot* bro, Args& args) {
	if (args[5] != ".join" || args[4] != channel)
		return;
	if (started == 0) {
		bro->irc->privmsg(args[4], "No game in "+args[4]+" right now! Type .uno to start one!");
		return;
	}
	if (started == 2) {
		bro->irc->privmsg(args[4], "Game in "+args[4]+" already in progress!");
		return;
	}
	std::vector<std::string>::iterator dropit = std::find(dropped_players.begin(), dropped_players.end(), args[1]);
	if (dropit != dropped_players.end()) {
		bro->irc->privmsg(args[4], "You have already dropped from the game!");
		return;
	}
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end()) {
		bro->irc->privmsg(args[4], "You are already in the game!");
		bro->irc->notice(args[1], "Your cards are:");
		printCard(bro, args[1], true, it->hand);
		return;
	}
	if (players.size() == 10) {
		bro->irc->privmsg(args[4], "There are 10 players in "+args[4]+" already!");
		bro->irc->privmsg(args[4], "Type .start to start the game!");
		return;
	}
	Player p(args[1]);
	for (int i = 0; i < 7; ++i) {
		p.hand.push_back(deck.back());
		deck.pop_back();
	}
	players.push_back(p);
	current_player = players.begin();
	bro->irc->privmsg(args[4], args[1]+" has joined this game of 4U8N3O12!");
	bro->irc->notice(args[1], "Your cards are:");
	printCard(bro, args[1], true, p.hand);
};

void Uno::listPlayers(Brobot* bro, Args& args) {
	if (args[5] != ".players" || args[4] != channel)
		return;
	if (started == 0) {
		bro->irc->privmsg(args[4], "No game in "+args[4]+" right now! Type .uno to start one!");
		return;
	}
	std::string pstring;
	BOOST_FOREACH( Player p, players)
		pstring += p.nick+" ";
	bro->irc->privmsg(args[4], "Playing order: "+pstring);
};

void Uno::showDiscard(Brobot* bro, Args& args) {
	if (started != 2 || args[5] != ".discard" || args[4] != channel)
		return;
	bro->irc->privmsg(args[4], "Current discard:");
	printCard(bro, channel, false, discard.back());
};

void Uno::nickHook(Brobot* bro, Args& args) {
	if (started == 0)
		return; // game hasn't started
	std::vector<std::string>::iterator dropit = std::find(dropped_players.begin(), dropped_players.end(), args[1]);
	if (dropit != dropped_players.end())
		*dropit = args[2];
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end())
		it->nick = args[2];
};

void Uno::partHook(Brobot* bro, Args& args) {
	if (started == 0 || args[4] != channel)
		return; // game hasn't started/wrong channel
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end()) {
		bro->irc->privmsg(channel, args[1]+" has left the game!");
		if (players.size() == 1 || (players.size() <= 2 && started == 2)) {
			if (current_player == it)
				skip();
			endGame(bro);
			return;
		} else if (current_player == it && started == 2) {
			skip(); // skip to the next player
			bro->irc->privmsg(channel, "It is now "+current_player->nick+"'s turn!");
			BOOST_FOREACH(Card c, it->hand)
				discard.push_back(c);
		}
		dropped_players.push_back(args[1]);
		players.erase(it);
	}
};

void Uno::quitHook(Brobot* bro, Args& args) {
	if (started == 0)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end()) {
		bro->irc->privmsg(channel, args[1]+" has left the game!");
		if (players.size() == 1 || (players.size() <= 2 && started == 2)) {
			if (current_player == it)
				skip();
			endGame(bro);
			return;
		} else if (current_player == it && started == 2) {
			skip(); // skip to the next player
			bro->irc->privmsg(channel, "It is now "+current_player->nick+"'s turn!");
			BOOST_FOREACH(Card c, it->hand)
				discard.push_back(c);
		}
		dropped_players.push_back(args[1]);
		players.erase(it);
	}
};

void Uno::dropPlayer(Brobot* bro, Args& args) {
	if (started == 0 || args[5] != ".drop" || args[4] != channel)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end()) {
		bro->irc->privmsg(channel, args[1]+" has left the game!");
		if (players.size() == 1 || (players.size() <= 2 && started == 2)) {
			if (current_player == it)
				skip();
			endGame(bro);
			return;
		} else if (current_player == it && started == 2) {
			skip(); // skip to the next player
			bro->irc->privmsg(channel, "It is now "+current_player->nick+"'s turn!");
			BOOST_FOREACH(Card c, it->hand)
				discard.push_back(c);
		}
		dropped_players.push_back(args[1]);
		players.erase(it);
	}
};