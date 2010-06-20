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
}

void Uno::printCard(Brobot* bro, const std::string& target, bool notice, Card card) { // single card
	for (int i = 0; i < 14; ++i) {
		if (notice) {
			bro->irc->notice(target, card.ascii[i]);
		} else {
			bro->irc->privmsg(target, card.ascii[i]);
		}
	}
}

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
}

void Uno::onLoad(Brobot* bro) {
	bro->hook("[uno] gameStart", "OnPRIVMSG", boost::bind(&Uno::gameStart, this, bro, _1));
	bro->hook("[uno] joinHook", "OnPRIVMSG", boost::bind(&Uno::joinHook, this, bro, _1));
	bro->hook("[uno] listPlayers", "OnPRIVMSG", boost::bind(&Uno::listPlayers, this, bro, _1));
	bro->hook("[uno] nickHook", "OnNICK", boost::bind(&Uno::nickHook, this, bro, _1));
}

void Uno::onUnload(Brobot* bro) {
	bro->unhook("[uno] gameStart", "OnPRIVMSG");
	bro->unhook("[uno] joinHook", "OnPRIVMSG");
	bro->unhook("[uno] listPlayers", "OnPRIVMSG");
	bro->unhook("[uno] nickHook", "OnNICK");
}

void Uno::gameStart(Brobot* bro, Args& args) {
	if (args[5] != ".uno" || args[4][0] != '#')
		return;
	if (started != 0) {
		bro->irc->privmsg(args[4], "Game in "+channel+" already in progress!");
		return;
	}
	started = 1;
	deck.clear();
	for(int i = 0; i < 108; ++i) {
		deck.push_back(__cards[rand()%54]);
	}
	channel = args[4];
	bro->irc->privmsg(args[4], "Starting 4U8N3O12! game in "+args[4]+"!");
	bro->irc->privmsg(args[4], "Say .join to join in and .start to start the game!");
	Player p(args[1]);
	for (int i = 0; i < 7; ++i) {
		p.hand.push_back(deck.back());
		deck.pop_back();
	}
	players.push_back(p);
	printCard(bro, args[1], true, p.hand);
}

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
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end()) {
		bro->irc->privmsg(args[4], "You are already in the game!");
		bro->irc->notice(args[1], "Your cards are:");
		printCard(bro, args[1], true, it->hand);
		return;
	}
	Player p(args[1]);
	for (int i = 0; i < 7; ++i) {
		p.hand.push_back(deck.back());
		deck.pop_back();
	}
	players.push_back(p);
	bro->irc->privmsg(args[4], args[1]+" has joined this game of 4U8N3O12!");
	bro->irc->notice(args[1], "Your cards are:");
	printCard(bro, args[1], true, p.hand);
}

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

void Uno::nickHook(Brobot* bro, Args& args) {
	if (started == 0)
		return; // game hasn't started
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end())
		it->nick = args[2];
}