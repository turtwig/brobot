#include "..\include\uno.h"
#include <fstream>
#include <cstdlib>
#include "..\include\brobot.h"

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

void printCard(Brobot* bro, const std::string& target, bool notice, Card card) { // single card
	for (int i = 0; i < 14; ++i) {
		if (notice) {
			bro->irc->notice(target, card.ascii[i]);
		} else {
			bro->irc->privmsg(target, card.ascii[i]);
		}
	}
}

void printCard(Brobot* bro, const std::string& target, bool notice, std::vector<Card> cards) { // multiple cards
	while(!cards.empty()) {
		std::vector<Card>::iterator it;
		if (cards.size() > 8) {
			it = cards.begin()+8;
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


std::vector<Player> players;

void Unotest(Brobot* bro, Args& args) {
	if (args[5].substr(0,5) == ".draw") {
		init();
		std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
		if (it == players.end()) {
			bro->irc->privmsg(args[4], "You are not in the game.");
			return;
		} else {
			int i = rand() % 54; // 0 -> 53
			bro->irc->privmsg(args[4], "You drew:");
			printCard(bro, args[4], false, __cards[i]);
			it->hand.push_back(__cards[i]);
		}
	}
	if (args[5].substr(0,4) != ".uno")
		return; // the privmsg was not aimed at us

	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it == players.end()) {
		players.push_back(Player(args[1]));
		bro->irc->privmsg(args[4], "Player added.");
	} else {
		bro->irc->notice(args[1], "Your hand:");
		printCard(bro, args[1], true, it->hand);
	}
}