#include <boost/foreach.hpp>
#include <iostream>
#include "../include/brobot.h"
#include "../include/perl.h"

EXTERN_C void xs_init (pTHX); // From perlxsi.c

void PerlTest(Brobot* bro, Args& args) {
	char *embedding[] = { "", "-e", "0" };
	if (args[5].substr(0,6) == ".init ") {
		my_perl = perl_alloc();
		perl_construct(my_perl);
		PL_origalen = 1;
		PL_exit_flags |= PERL_EXIT_DESTRUCT_END;
		perl_parse(my_perl, xs_init, 3, embedding, NULL);
		perl_run(my_perl);
	}
	if (args[5].substr(0,6) == ".quit ") {
		perl_destruct(my_perl);
		perl_free(my_perl);
		PERL_SYS_TERM();
	}
	if (args[5].substr(0,6) != ".perl ")
		return;

	std::string perl_line = "eval {";
	perl_line += args[5].substr(6, std::string::npos);
	perl_line += "}; $@;";
	SV* h = eval_pv(perl_line.c_str(), TRUE);
	bro->irc->privmsg("#goodjob", SvPV_nolen(h));
}