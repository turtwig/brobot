#include "..\include\uno.h"

Uno::Card::Card(Brobot* bro, short int num, Cardtype col, Cardattr spec, const std::string& fname) : number(num), type(col), attr(spec) {
	if (fname == "")
		return;
	std::ifstream file;
	file.open((bro->stor->get("module.uno.dir")+fname).c_str(), std::ios::in);
	if (file) {
		for (int i = 0; i < 14; ++i) {
			std::getline(file, ascii[i]);
		}
	}
	file.close();
};

void Uno::printCard(Brobot* bro, const std::string& target, bool notice, const Card& card) { // single card
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

Uno::Uno(Brobot* bro) : started(0), has_to_draw_cards(0),
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
						_wild(bro, -1, red, wild, "wild.txt"), _p4wild(bro, -1, red, drawfour, "wild+4.txt") {
	std::string scorefile = bro->stor->get("module.uno.scorefile");
	if (scorefile == "")
		return;
	std::ifstream file;
	file.open(scorefile.c_str(), std::ios::in);
	if (file) {
		while (!file.eof()) {
			std::string line;
			std::getline(file, line);
			if (line[0] != '#') continue;
			size_t space = line.find_first_of(" ");
			std::string ch = line.substr(0, space);
			line.erase(0, space);
			std::vector<std::string> tokens;
			std::istringstream iss(line);
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter<std::vector<std::string> >(tokens));
			for (unsigned short int i = 0; i < tokens.size(); i += 3) {  // i+0 -> nick, i+1-> win number, i+2-> total score
				scores.push_back(Score(tokens[i], ch, atoi(tokens[i+2].c_str()), atoi(tokens[i+1].c_str())));
			}
		}
		std::sort(scores.begin(), scores.end());
	}
	file.close();
};

void Uno::showScores(Brobot* bro, Args& args) {
	if (args[5] != ".uno leaderboard" || args[4][0] != '#')
		return;
	bro->irc->privmsg(args[4], "4U8N3O12! leaderboard for "+args[4]+":");
	unsigned short int counter = 1;
	BOOST_FOREACH(Score score, scores) {
		if (score.channel != args[4]) continue;
		char wincount[20];
		_itoa(score.win_count, wincount, 10);
		char totalscore[20];
		_itoa(score.total_score, totalscore, 10);
		char position[10];
		_itoa(counter, position, 10);
		std::string victories(wincount);
		if (score.win_count == 1) {
			victories += " victory!";
		} else {
			victories += " victories!";
		}
		bro->irc->privmsg(args[4], std::string(position)+") "+score.nick+" with "+std::string(totalscore)+" points and "+victories);
		++counter;
	}
	std::vector<Score> global_scores;
	BOOST_FOREACH(Score score, scores) {
		std::vector<Score>::iterator it = std::find(global_scores.begin(), global_scores.end(), score);
		if (it == global_scores.end()) {
			global_scores.push_back(score);
		} else {
			it->win_count += score.win_count;
			it->total_score += score.total_score;
		}
	}
	bro->irc->privmsg(args[4], "Global 4U8N3O12! leaderboard:");
	counter = 1;
	BOOST_FOREACH(Score score, global_scores) {
		char wincount[20];
		_itoa(score.win_count, wincount, 10);
		char totalscore[20];
		_itoa(score.total_score, totalscore, 10);
		char position[10];
		_itoa(counter, position, 10);
		std::string victories(wincount);
		if (score.win_count == 1) {
			victories += " victory!";
		} else {
			victories += " victories!";
		}
		bro->irc->privmsg(args[4], std::string(position)+") "+score.nick+" with "+std::string(totalscore)+" points and "+victories);
		++counter;
	}
};

void Uno::updateScore(Brobot* bro, const std::string& nick, unsigned int score) {
	bool found = false;
	for (std::vector<Score>::iterator it = scores.begin(); it != scores.end(); ++it) {
		if (it->channel != channel) continue;
		if (it->nick != nick) continue;
		it->win_count++;
		it->total_score += score;
		found = true;
		break;
	}
	if (!found) {
		scores.push_back(Score(nick, channel, score, 1));
	}
	std::sort(scores.begin(), scores.end());
	std::ofstream file;
	file.open(bro->stor->get("module.uno.scorefile").c_str(), std::ios::out);
	if (!file) {
		return; // OH WELL
	}
	std::string ch = "";
	BOOST_FOREACH(Score sc, scores) {
		if (ch != sc.channel) {
			ch = sc.channel;
			file << std::endl << ch;
		}
		file << " " << sc.nick << " " << sc.win_count << " " << sc.total_score;
	}
	file.close();
};

void Uno::onLoad(Brobot* bro) {
	bro->hook("[uno] gameStart", "OnPRIVMSG", boost::bind(&Uno::gameStart, this, bro, _1));
	bro->hook("[uno] help", "OnPRIVMSG", boost::bind(&Uno::help, this, bro, _1));
	bro->hook("[uno] showScores", "OnPRIVMSG", boost::bind(&Uno::showScores, this, bro, _1));
};

void Uno::onUnload(Brobot* bro) {
	endGame(bro, false); // make sure the game ends properly (if any)
	bro->unhook("[uno] gameStart", "OnPRIVMSG");
	bro->unhook("[uno] help", "OnPRIVMSG");
	bro->unhook("[uno] showScores", "OnPRIVMSG");
};

void Uno::help(Brobot* bro, Args& args) {
	if (args[5] != ".uno help" || args[4][0] != '#')
		return;
	bro->irc->privmsg(args[4], "           4U8N3O12! HELP");
	bro->irc->privmsg(args[4], ".uno help           this help");
	bro->irc->privmsg(args[4], ".uno leaderboard    display leaderboard");
	bro->irc->privmsg(args[4], ".uno                creates a game");
	bro->irc->privmsg(args[4], ".join               joins the game");
	bro->irc->privmsg(args[4], ".start              starts a created game");
	bro->irc->privmsg(args[4], ".endgame            ends a game");
	bro->irc->privmsg(args[4], ".drop               drops you from the game");
	bro->irc->privmsg(args[4], ".hand               shows your hand");
	bro->irc->privmsg(args[4], ".ha                 alias for .hand");
	bro->irc->privmsg(args[4], ".discard            shows current discard");
	bro->irc->privmsg(args[4], ".dc                 alias for .discard");
	bro->irc->privmsg(args[4], ".players            shows playing order, current player and number of cards of each player");
	bro->irc->privmsg(args[4], ".challenge          challenges a player after he played a wild +4");
	bro->irc->privmsg(args[4], ".draw               draws a card");
	bro->irc->privmsg(args[4], ".dr                 alias for .draw");
	bro->irc->privmsg(args[4], ".pass               passes turn");
	bro->irc->privmsg(args[4], ".pa                 alias for .pass");
	bro->irc->privmsg(args[4], ".play               plays a card");
	bro->irc->privmsg(args[4], ".pl                 alias for .play");
	bro->irc->privmsg(args[4], ".skip               makes the current player draw a card and pass his turn");
	bro->irc->privmsg(args[4], "                    this command only works if a minute has passed since the player's turn started");
	bro->irc->privmsg(args[4], " ");
	bro->irc->privmsg(args[4], "Syntax for .play is: .play [rgby][0-9] (i.e. r1, b5, g6)");
	bro->irc->privmsg(args[4], "                           Special cards: r+2, rs, rr");
	bro->irc->privmsg(args[4], "                           Wild and Wild+4s: w [rgby], w+4 [rgby] to specify a color");
	bro->irc->privmsg(args[4], "If you drop from a game you cannot re-join it.");
};

void Uno::gameEnd(Brobot* bro, Args& args) {
	if (args[5] != ".endgame" || args[4] != channel)
		return;
	if (uno_creator != args[1]) {
		bro->irc->privmsg(channel, "Only "+uno_creator+" can end the game!");
		return;
	}
	endGame(bro, false);
};

void Uno::playCard(Brobot* bro, Args& args) {
	if (started != 2 || args[5].size() < 6 || (args[5].substr(0,3) != ".pl" || args[5] == ".players") || args[4] != channel)
		return;
	if (args[1] != current_player->nick) {
		bro->irc->privmsg(channel, "It's not your turn!");
		return;
	}
	unsigned short int h = 0;
	if (args[5].substr(0,6) == ".play ") {
		h = 2;
	}
	Cardtype color;
	short int number;
	switch(args[5][4+h]) {
		case 'r':
			color = red;
			number = atoi(&(args[5][5+h]));
			break;
		case 'b':
			color = blue;
			number = atoi(&(args[5][5+h]));
			break;
		case 'g':
			color = green;
			number = atoi(&(args[5][5+h]));
			break;
		case 'y':
			color = yellow;
			number = atoi(&(args[5][5+h]));
			break;
		case 'w':
			color = none_;
			number = -1;
			break;
		default:
			bro->irc->privmsg(channel, "What card was that again?");
			return;
	}
	if (args[5][5+h] == 'r' || args[5][5+h] == 's' || args[5][5+h] == '+' || number == -1) {
		if (has_to_draw_cards == 0 && args[5][5+h] == 'r' && (discard.back().type == color || discard.back().attr == reverse)) {
			Card c(NULL, -1, color, reverse, "");
			std::vector<Card>::iterator it = std::find(current_player->hand.begin(), current_player->hand.end(), c);
			if (it == current_player->hand.end()) {
				bro->irc->privmsg(channel, "You don't have that card!");
				return;
			}
			bro->irc->privmsg(channel, ""+current_player->nick+" plays:");
			printCard(bro, channel, false, *it);
			discard.push_back(*it);
			current_player->hand.erase(it);
			current_player->has_drawn = false;
			current_player->has_challenged = false;
			if (current_player->hand.empty()) {
				endGame(bro, true);
				return;
			} else if (current_player->hand.size() == 1) {
				bro->irc->privmsg(channel, ""+current_player->nick+" has 4U8N3O12!");
			}
			if (players.size() > 2) {
				reversePlayers();
				bro->irc->privmsg(channel, "Playing order has been reversed!");
				nextPlayer();
				nextTurn(bro);
			} else {
				nextPlayer();
				bro->irc->privmsg(channel, ""+current_player->nick+" skips his turn!");
				nextPlayer();
				nextTurn(bro);
			}
		} else if (has_to_draw_cards == 0 && args[5][5+h] == 's' && (discard.back().type == color || discard.back().attr == skip)) {
			Card c(NULL, -1, color, skip, "");
			std::vector<Card>::iterator it = std::find(current_player->hand.begin(), current_player->hand.end(), c);
			if (it == current_player->hand.end()) {
				bro->irc->privmsg(channel, "You don't have that card!");
				return;
			}
			bro->irc->privmsg(channel, ""+current_player->nick+" plays:");
			printCard(bro, channel, false, *it);
			discard.push_back(*it);
			current_player->hand.erase(it);
			current_player->has_drawn = false;
			current_player->has_challenged = false;
			has_to_draw_cards = 0;
			if (current_player->hand.empty()) {
				endGame(bro, true);
				return;
			} else if (current_player->hand.size() == 1) {
				bro->irc->privmsg(channel, ""+current_player->nick+" has 4U8N3O12!");
			}
			nextPlayer();
			bro->irc->privmsg(channel, ""+current_player->nick+" skips his turn!");
			nextPlayer();
			nextTurn(bro);
		} else if (args[5][4+h] != 'w' && args[5].substr(5+h,2) == "+2" && (discard.back().type == color || discard.back().attr == drawtwo || (discard.back().attr == drawfour && discard.back().type == color))) {
			Card c(NULL, -1, color, drawtwo, "");
			std::vector<Card>::iterator it = std::find(current_player->hand.begin(), current_player->hand.end(), c);
			if (it == current_player->hand.end()) {
				bro->irc->privmsg(channel, "You don't have that card!");
				return;
			}
			bro->irc->privmsg(channel, ""+current_player->nick+" plays:");
			printCard(bro, channel, false, *it);
			discard.push_back(*it);
			current_player->hand.erase(it);
			current_player->has_drawn = false;
			current_player->has_challenged = false;
			has_to_draw_cards += 2;
			if (current_player->hand.empty()) {
				endGame(bro, true);
				return;
			} else if (current_player->hand.size() == 1) {
				bro->irc->privmsg(channel, ""+current_player->nick+" has 4U8N3O12!");
			}
			nextPlayer();
			nextTurn(bro);
		} else if (args[5][4+h] == 'w' && args[5][5+h] != '+' && has_to_draw_cards == 0) {
			char ch;
			if (args[5][5+h] == ' ') {
				ch = args[5][6+h];
			} else {
				ch = args[5][5+h];
			}
			Cardtype newcol;
			switch (ch) {
				case 'b':
					newcol = blue;
					break;
				case 'r':
					newcol = red;
					break;
				case 'g':
					newcol = green;
					break;
				case 'y':
					newcol = yellow;
					break;
				default:
					bro->irc->privmsg(channel, "You need to specify a color!");
					return;
			}
			Card c(NULL, -1, none_, wild, "");
			std::vector<Card>::iterator it = std::find(current_player->hand.begin(), current_player->hand.end(), c);
			if (it == current_player->hand.end()) {
				bro->irc->privmsg(channel, "You don't have that card!");
				return;
			}
			it->type = newcol;
			bro->irc->privmsg(channel, ""+current_player->nick+" plays:");
			printCard(bro, channel, false, *it);
			discard.push_back(*it);
			current_player->hand.erase(it);
			current_player->has_drawn = false;
			current_player->has_challenged = false;
			has_to_draw_cards = 0;
			if (current_player->hand.empty()) {
				endGame(bro, true);
				return;
			} else if (current_player->hand.size() == 1) {
				bro->irc->privmsg(channel, ""+current_player->nick+" has 4U8N3O12!");
			}
			nextPlayer();
			nextTurn(bro);
		} else if (args[5].substr(4+h,3) == "w+4" && args[5].size() >= 8) {
			char ch;
			if (args[5][7+h] == ' ') {
				ch = args[5][8+h];
			} else {
				ch = args[5][7+h];
			}
			Cardtype newcol;
			switch (ch) {
				case 'b':
					newcol = blue;
					break;
				case 'r':
					newcol = red;
					break;
				case 'g':
					newcol = green;
					break;
				case 'y':
					newcol = yellow;
					break;
				default:
					bro->irc->privmsg(channel, "You need to specify a color!");
					return;
			}
			Card c(NULL, -1, none_, drawfour, "");
			std::vector<Card>::iterator it = std::find(current_player->hand.begin(), current_player->hand.end(), c);
			if (it == current_player->hand.end()) {
				bro->irc->privmsg(channel, "You don't have that card!");
				return;
			}
			it->type = newcol;
			bro->irc->privmsg(channel, ""+current_player->nick+" plays:");
			printCard(bro, channel, false, *it);
			discard.push_back(*it);
			current_player->hand.erase(it);
			current_player->has_drawn = false;
			current_player->has_challenged = false;
			has_to_draw_cards += 4;
			if (current_player->hand.empty()) {
				endGame(bro, true);
				return;
			} else if (current_player->hand.size() == 1) {
				bro->irc->privmsg(channel, ""+current_player->nick+" has 4U8N3O12!");
			}
			nextPlayer();
			nextTurn(bro);
		} else {
			bro->irc->privmsg(channel, "You can't play that card!");
		}
	} else if (has_to_draw_cards == 0 && discard.back().type == color || discard.back().number == number) {
		Card c(NULL, number, color, none, "");
		std::vector<Card>::iterator it = std::find(current_player->hand.begin(), current_player->hand.end(), c);
		if (it == current_player->hand.end()) {
			bro->irc->privmsg(channel, "You don't have that card!");
			return;
		}
		bro->irc->privmsg(channel, ""+current_player->nick+" plays:");
		printCard(bro, channel, false, *it);
		discard.push_back(*it);
		current_player->hand.erase(it);
		current_player->has_drawn = false;
		current_player->has_challenged = false;
		has_to_draw_cards = 0;
		if (current_player->hand.empty()) {
			endGame(bro, true);
			return;
		} else if (current_player->hand.size() == 1) {
			bro->irc->privmsg(channel, ""+current_player->nick+" has 4U8N3O12!");
		}
		nextPlayer();
		nextTurn(bro);
	} else {
		bro->irc->privmsg(channel, "You can't play that card!");
	}
};

void Uno::challenge(Brobot* bro, Args& args) {
	if (started != 2 || args[5] != ".challenge" || args[4] != channel)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it == players.end())
		return;
	if (it != current_player) {
		bro->irc->privmsg(channel, "It's not your turn!");
		return;
	}
	if (current_player->has_challenged || current_player->has_drawn || (discard.back().attr != drawfour && has_to_draw_cards < 4)) {
		bro->irc->privmsg(channel, "You can't do that!");
		return;
	}
	current_player->has_challenged = true;
	if (it == players.begin()) {
		it = --players.end();
	} else {
		--it;
	}
	bool invalid = false;
	BOOST_FOREACH(Card c, it->hand) {
		if (c.type == (discard.end()-2)->type) {
			invalid = true;
			break;
		}
	}
	if (!invalid) {
		has_to_draw_cards += 2;
		char tmpbuf[10];
		_itoa(has_to_draw_cards, tmpbuf, 10);
		bro->irc->privmsg(channel, ""+it->nick+"'s move was legal and "+current_player->nick+" must draw "+std::string(tmpbuf)+" cards!");
		return;
	} else {
		char tmpbuf[10];
		_itoa(has_to_draw_cards, tmpbuf, 10);
		bro->irc->privmsg(channel, ""+it->nick+"'s move was illegal and has to draw "+std::string(tmpbuf)+" cards!");
		current_player->has_challenged = false; // challenge was successful
		current_player = it;
		current_player->has_challenged = true; // the previous player can't challenge himself
		nextTurn(bro);
	}
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
	uno_creator = args[1];
	bro->hook("[uno] joinHook", "OnPRIVMSG", boost::bind(&Uno::joinHook, this, bro, _1));
	bro->hook("[uno] listPlayers", "OnPRIVMSG", boost::bind(&Uno::listPlayers, this, bro, _1));
	bro->hook("[uno] nickHook", "OnNICK", boost::bind(&Uno::nickHook, this, bro, _1));
	bro->hook("[uno] partHook", "OnPART", boost::bind(&Uno::partHook, this, bro, _1));
	bro->hook("[uno] quitHook", "OnQUIT", boost::bind(&Uno::quitHook, this, bro, _1));
	bro->hook("[uno] dropPlayer", "OnPRIVMSG", boost::bind(&Uno::dropPlayer, this, bro, _1));
	bro->hook("[uno] startGame", "OnPRIVMSG", boost::bind(&Uno::startGame, this, bro, _1));
	bro->hook("[uno] showDiscard", "OnPRIVMSG", boost::bind(&Uno::showDiscard, this, bro, _1));
	bro->hook("[uno] passTurn", "OnPRIVMSG", boost::bind(&Uno::passTurn, this, bro, _1));
	bro->hook("[uno] drawCard", "OnPRIVMSG", boost::bind(&Uno::drawCard, this, bro, _1));
	bro->hook("[uno] showHand", "OnPRIVMSG", boost::bind(&Uno::showHand, this, bro, _1));
	bro->hook("[uno] gameEnd", "OnPRIVMSG", boost::bind(&Uno::gameEnd, this, bro, _1));
	bro->hook("[uno] playCard", "OnPRIVMSG", boost::bind(&Uno::playCard, this, bro, _1));
	bro->hook("[uno] challenge", "OnPRIVMSG", boost::bind(&Uno::challenge, this, bro, _1));
	bro->hook("[uno] skipTurn", "OnPRIVMSG", boost::bind(&Uno::skipTurn, this, bro, _1));
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
	current_player = players.begin();
	bro->irc->notice(args[1], "Your cards are:");
	printCard(bro, args[1], true, p.hand);
};

void Uno::endGame(Brobot* bro, bool updatescore) {
	if (started == 0)
		return;
	bro->irc->privmsg(channel, "4U8N3O12! game in "+channel+" ended!");
	if (players.size() > 1) {
		std::string winner = current_player->nick;
		if (has_to_draw_cards != 0) {
			nextPlayer();
			char tmpbuf[10];
			_itoa(has_to_draw_cards, tmpbuf, 10);
			bro->irc->privmsg(channel, ""+current_player->nick+" draws "+std::string(tmpbuf)+" cards!");
			for (int i = 0; i < has_to_draw_cards; i++) {
				current_player->hand.push_back(deck.back());
				deck.pop_back();
				if (deck.empty())
					swapDecks();
			}
			current_player = std::find(players.begin(), players.end(), winner);
		}
		players.erase(current_player);
		unsigned int score = 0;
		BOOST_FOREACH(Player p, players) {
			bro->irc->privmsg(channel, ""+p.nick+" still had:");
			printCard(bro, channel, false, p.hand);
			BOOST_FOREACH(Card c, p.hand) {
				if (c.attr == none) {
					score += c.number;
				} else if (c.attr == wild || c.attr == drawfour) {
					score += 50;
				} else if (c.attr == skip || c.attr == reverse || c.attr == drawtwo) {
					score += 20;
				}
			}
		}
		char tmpbuf[10]; // a really small one (but no one will have a score with more than 9 digits (right))
		_itoa(score, tmpbuf, 10);
		bro->irc->privmsg(channel, "Winner is "+winner+" with "+std::string(tmpbuf)+" points!");
		if (updatescore) {
			updateScore(bro, winner, score); // update in-memory scores and global_scores and write to file
		} else {
			bro->irc->privmsg(channel, "The stats of this match were not saved!");
		}
	}
	started = 0;
	channel.clear();
	uno_creator.clear();
	players.clear();
	dropped_players.clear();
	deck.clear();
	discard.clear();
	current_player = players.end();
	has_to_draw_cards = 0;
	bro->unhook("[uno] joinHook", "OnPRIVMSG");
	bro->unhook("[uno] listPlayers", "OnPRIVMSG");
	bro->unhook("[uno] nickHook", "OnNICK");
	bro->unhook("[uno] partHook", "OnPART");
	bro->unhook("[uno] quitHook", "OnQUIT");
	bro->unhook("[uno] dropPlayer", "OnPRIVMSG");
	bro->unhook("[uno] startGame", "OnPRIVMSG");
	bro->unhook("[uno] showDiscard", "OnPRIVMSG");
	bro->unhook("[uno] passTurn", "OnPRIVMSG");
	bro->unhook("[uno] drawCard", "OnPRIVMSG");
	bro->unhook("[uno] showHand", "OnPRIVMSG");
	bro->unhook("[uno] gameEnd", "OnPRIVMSG");
	bro->unhook("[uno] playCard", "OnPRIVMSG");
	bro->unhook("[uno] challenge", "OnPRIVMSG");
	bro->unhook("[uno] skipTurn", "OnPRIVMSG");
};

void Uno::skipTurn(Brobot* bro, Args& args) {
	if (started != 2 || args[4] != channel || args[5] != ".skip")
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it == players.end())
		return;
	if (turntimer.elapsed() < 60) {
		char tmpbuf[10];
		_itoa(60-turntimer.elapsed(), tmpbuf, 10);
		bro->irc->privmsg(channel, "You need to wait "+std::string(tmpbuf)+" more seconds!");
		return;
	}
	bro->irc->privmsg(channel, ""+current_player->nick+" is taking too long to play and has been skipped!");
	if (current_player->has_drawn) {
		current_player->has_drawn = false;
		current_player->has_challenged = false;
		nextPlayer();
		bro->irc->privmsg(channel, "Current discard:");
		printCard(bro, channel, false, discard.back());
		nextTurn(bro);
	} else {
		if (has_to_draw_cards != 0) {
			char tmpbuf[10];
			_itoa(has_to_draw_cards, tmpbuf, 10);
			bro->irc->privmsg(channel, ""+current_player->nick+" draws "+std::string(tmpbuf)+" cards!");
			std::vector<Card> drawncards;
			for (int i = 0; i < has_to_draw_cards; i++) {
				drawncards.push_back(deck.back());
				deck.pop_back();
				if (deck.empty())
					swapDecks();
			}
			current_player->hand.insert(current_player->hand.end(), drawncards.begin(), drawncards.end());
			has_to_draw_cards = 0;
			current_player->has_drawn = false;
			current_player->has_challenged = false;
			nextPlayer();
			bro->irc->privmsg(channel, "Current discard:");
			printCard(bro, channel, false, discard.back());
			nextTurn(bro);
		} else {
			current_player->hand.push_back(deck.back());
			bro->irc->privmsg(channel, ""+current_player->nick+" draws a card!");
			deck.pop_back();
			if (deck.empty())
				swapDecks();
			current_player->has_drawn = false;
			current_player->has_challenged = false;
			nextPlayer();
			bro->irc->privmsg(channel, "Current discard:");
			printCard(bro, channel, false, discard.back());
			nextTurn(bro);
		}
	}
};

void Uno::startGame(Brobot* bro, Args& args) {
	if (started != 1 || args[5] != ".start" || args[4] != channel)
		return;
	if (started == 2) {
		bro->irc->privmsg(channel, "Game in "+channel+" is already in progress!");
		return;
	}
	if (uno_creator != args[1]) {
		bro->irc->privmsg(channel, "Only "+uno_creator+" can start the game!");
		return;
	}
	if (players.size() < 2 || players.size() > 10) {
		bro->irc->privmsg(channel, "Not enough players!");
		return;
	}
	bro->irc->privmsg(channel, "Starting 4U8N3O12! game in "+channel+"!");
	turntimer.restart();
	current_player = players.begin();
	std::string pstring;
	BOOST_FOREACH( Player p, players)
		pstring += p.nick+" ";
	bro->irc->privmsg(args[4], "Playing order: "+pstring);
	discard.push_back(deck.back());
	deck.pop_back();
	bro->irc->privmsg(channel, "Current discard:");
	printCard(bro, channel, false, discard.back());
	started = 2;
	if (discard.back().attr == none) {
		nextTurn(bro);
	} else if (discard.back().attr == wild || discard.back().attr == drawfour) {
		switch (rand() % 4) {
			case 0:
				discard.back().type = red;
				break;
			case 1:
				discard.back().type = blue;
				break;
			case 2:
				discard.back().type = green;
				break;
			case 3:
				discard.back().type = yellow;
				break;
		}
		if (discard.back().attr == drawfour) {
			has_to_draw_cards += 4;
			current_player->has_challenged = true; // don't let the player challenge on first turn
		}
		nextTurn(bro);
	} else if (discard.back().attr == skip) {
		bro->irc->privmsg(channel, ""+current_player->nick+" skips his turn!");
		nextPlayer();
		nextTurn(bro);
	} else if (discard.back().attr == reverse) {
		if (players.size() > 2) {
			nextTurn(bro);
			reversePlayers();
			bro->irc->privmsg(channel, "Playing order has been reversed!");
			std::string pstring;
			std::vector<Player>::iterator it = current_player;
			while (it != players.end()) {
				pstring += it++->nick+" ";
			}
			it = players.begin();
			while (it != current_player) {
				pstring += it++->nick+" ";
			}
			bro->irc->privmsg(channel, "Playing order is now: "+pstring);
		} else { // act like a skip
			bro->irc->privmsg(channel, ""+current_player->nick+" skips his turn!");
			nextPlayer();
			nextTurn(bro);
		}
	} else if (discard.back().attr == drawtwo) {
		has_to_draw_cards += 2;
		nextTurn(bro);
	}
};

void Uno::reversePlayers() {
	std::vector<Player> tmp;
	std::vector<Player>::iterator it = current_player;
	while (it != players.begin()) {
		tmp.push_back(*(it--));
	}
	tmp.push_back(*it);
	it = --players.end();
	while (it != current_player) {
		tmp.push_back(*(it--));
	}
	players = tmp;
	current_player = players.begin();
};

void Uno::drawCard(Brobot* bro, Args& args) {
	if (started != 2 || (args[5] != ".draw" && args[5] != ".dr") || args[4] != channel)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it == players.end())
		return;
	if (current_player != it) {
		bro->irc->privmsg(channel, "It's not your turn!");
		return;
	}
	if (it->has_drawn) {
		bro->irc->privmsg(channel, "You have already drawn a card!");
		return;
	}
	if (has_to_draw_cards != 0) {
		char tmpbuf[10];
		_itoa(has_to_draw_cards, tmpbuf, 10);
		bro->irc->privmsg(channel, ""+args[1]+" draws "+std::string(tmpbuf)+" cards!");
		std::vector<Card> drawncards;
		for (int i = 0; i < has_to_draw_cards; i++) {
			drawncards.push_back(deck.back());
			deck.pop_back();
			if (deck.empty())
				swapDecks();
		}
		bro->irc->notice(args[1], "You have drawn:");
		printCard(bro, args[1], true, drawncards);
		it->hand.insert(it->hand.end(), drawncards.begin(), drawncards.end());
		has_to_draw_cards = 0;
		bro->irc->privmsg(channel, ""+args[1]+" loses his turn!");
		it->has_challenged = false;
		nextPlayer();
		bro->irc->privmsg(args[4], "Current discard:");
		printCard(bro, channel, false, discard.back());
		nextTurn(bro);
	} else {
		it->hand.push_back(deck.back());
		bro->irc->privmsg(channel, ""+args[1]+" draws a card!");
		bro->irc->notice(args[1], "You have drawn:");
		printCard(bro, args[1], true, deck.back());
		deck.pop_back();
		if (deck.empty())
			swapDecks();
		it->has_drawn = true;
	}
};

void Uno::passTurn(Brobot* bro, Args& args) {
	if (started != 2 || (args[5] != ".pass" && args[5] != ".pa") || args[4] != channel)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it == players.end())
		return;
	if (current_player != it) {
		bro->irc->privmsg(channel, "It's not your turn!");
		return;
	}
	if (!it->has_drawn || has_to_draw_cards != 0) {
		bro->irc->privmsg(channel, "You need to draw a card first!");
		return;
	}
	bro->irc->privmsg(channel, ""+args[1]+" passes his turn!");
	it->has_drawn = false;
	it->has_challenged = false;
	nextPlayer();
	bro->irc->privmsg(args[4], "Current discard:");
	printCard(bro, channel, false, discard.back());
	nextTurn(bro);
};

void Uno::showHand(Brobot* bro, Args& args) {
	if (started == 0 || (args[5] != ".hand" && args[5] != ".ha") || args[4] != channel)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it == players.end())
		return;
	bro->irc->notice(args[1], "Your cards are:");
	printCard(bro, args[1], true, it->hand);
};

void Uno::nextTurn(Brobot* bro) {
	turntimer.restart();
	bro->irc->privmsg(channel, "It is now "+current_player->nick+"'s turn!");
	if (discard.back().attr == none) {
	} else if (has_to_draw_cards != 0) {
		char tmpbuf[10];
		_itoa(has_to_draw_cards, tmpbuf, 10);
		bro->irc->privmsg(channel, ""+current_player->nick+" must draw "+std::string(tmpbuf)+" cards!");
	}
	if (discard.back().attr == wild || discard.back().attr == drawfour) {
		switch(discard.back().type) {
			case red:
				bro->irc->privmsg(channel, "Color is 4red!");
				break;
			case blue:
				bro->irc->privmsg(channel, "Color is 12blue!");
				break;
			case green:
				bro->irc->privmsg(channel, "Color is 9green!");
				break;
			case yellow:
				bro->irc->privmsg(channel, "Color is 8yellow!");
				break;
		}
	}
	bro->irc->notice(current_player->nick, "Your cards are:");
	printCard(bro, current_player->nick, true, current_player->hand);
};


void Uno::joinHook(Brobot* bro, Args& args) {
	if (started == 0 || args[5] != ".join" || args[4] != channel)
		return;
	std::vector<std::string>::iterator dropit = std::find(dropped_players.begin(), dropped_players.end(), args[1]);
	if (dropit != dropped_players.end()) {
		bro->irc->privmsg(args[4], "You have already dropped from the game!");
		return;
	}
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end()) {
		bro->irc->privmsg(args[4], "You are already in the game!");
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
		if (deck.empty())
			swapDecks();
	}
	std::string current_nick = current_player->nick;
	players.push_back(p);
	current_player = std::find(players.begin(), players.end(), current_nick);
	bro->irc->privmsg(args[4], ""+args[1]+" has joined this game of 4U8N3O12!");
	bro->irc->notice(args[1], "Your cards are:");
	printCard(bro, args[1], true, p.hand);
};

void Uno::listPlayers(Brobot* bro, Args& args) {
	if (started == 0 || args[5] != ".players" || args[4] != channel)
		return;
	std::string pstring;
	std::vector<Player>::iterator it = current_player;
	while (it != players.end()) {
		pstring += it++->nick+" ";
	}
	it = players.begin();
	while (it != current_player) {
		pstring += it++->nick+" ";
	}
	bro->irc->privmsg(args[4], "Playing order: "+pstring);
	if (started == 2) {
		bro->irc->privmsg(args[4], "It is "+current_player->nick+"'s turn!");
		BOOST_FOREACH(Player p, players) {
			char tmpbuf[10];
			_itoa(p.hand.size(), tmpbuf, 10);
			bro->irc->privmsg(args[4], ""+p.nick+" has "+std::string(tmpbuf)+" cards!");
		}
	}
};

void Uno::showDiscard(Brobot* bro, Args& args) {
	if (started != 2 || (args[5] != ".discard" && args[5] != ".dc") || args[4] != channel)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it == players.end())
		return;
	bro->irc->privmsg(args[4], "Current discard:");
	printCard(bro, channel, false, discard.back());
	if (discard.back().attr == none) {
	} else if (discard.back().attr == wild || discard.back().attr == drawfour) {
		switch(discard.back().type) {
			case red:
				bro->irc->privmsg(channel, "Color is 4red!");
				break;
			case blue:
				bro->irc->privmsg(channel, "Color is 12blue!");
				break;
			case green:
				bro->irc->privmsg(channel, "Color is 9green!");
				break;
			case yellow:
				bro->irc->privmsg(channel, "Color is 8yellow!!");
				break;
		}
	}
	if (has_to_draw_cards != 0) {
		char tmpbuf[10];
		_itoa(has_to_draw_cards, tmpbuf, 10);
		bro->irc->privmsg(channel, ""+current_player->nick+" must draw "+std::string(tmpbuf)+" cards!");
	}
};

void Uno::nickHook(Brobot* bro, Args& args) {
	if (started == 0)
		return; // game hasn't started
	if (uno_creator == args[1])
		uno_creator = args[2];
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
		bro->irc->privmsg(channel, ""+args[1]+" has left the game!");
		if (players.size() == 1 || (players.size() <= 2 && started == 2)) {
			if (started == 2 && current_player == it)
				nextPlayer();
			endGame(bro, false);
			return;
		} else if (current_player == it && started == 2) {
			nextPlayer(); // skip to the next player
			bro->irc->privmsg(channel, "It is now "+current_player->nick+"'s turn!");
			discard.insert(discard.begin(), it->hand.begin(), it->hand.end());
		}
		dropped_players.push_back(args[1]);
		std::string current_nick = current_player->nick;
		players.erase(it);
		current_player = std::find(players.begin(), players.end(), current_nick);
	}
};

void Uno::quitHook(Brobot* bro, Args& args) {
	if (started == 0)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end()) {
		bro->irc->privmsg(channel, ""+args[1]+" has left the game!");
		if (players.size() == 1 || (players.size() <= 2 && started == 2)) {
			if (started == 2 && current_player == it)
				nextPlayer();
			endGame(bro, false);
			return;
		} else if (current_player == it && started == 2) {
			nextPlayer(); // skip to the next player
			bro->irc->privmsg(channel, "It is now "+current_player->nick+"'s turn!");
			discard.insert(discard.begin(), it->hand.begin(), it->hand.end());
		}
		dropped_players.push_back(args[1]);
		std::string current_nick = current_player->nick;
		players.erase(it);
		current_player = std::find(players.begin(), players.end(), current_nick);
	}
};

void Uno::dropPlayer(Brobot* bro, Args& args) {
	if (started == 0 || args[5] != ".drop" || args[4] != channel)
		return;
	std::vector<Player>::iterator it = std::find(players.begin(), players.end(), args[1]);
	if (it != players.end()) {
		bro->irc->privmsg(channel, ""+args[1]+" has left the game!");
		if (players.size() == 1 || (players.size() <= 2 && started == 2)) {
			if (started == 2 && current_player == it)
				nextPlayer();
			endGame(bro, false);
			return;
		} else if (current_player == it && started == 2) {
			nextPlayer(); // skip to the next player
			bro->irc->privmsg(channel, "It is now "+current_player->nick+"'s turn!");
			discard.insert(discard.begin(), it->hand.begin(), it->hand.end());
		}
		dropped_players.push_back(args[1]);
		std::string current_nick = current_player->nick;
		players.erase(it);
		current_player = std::find(players.begin(), players.end(), current_nick);
	}
};