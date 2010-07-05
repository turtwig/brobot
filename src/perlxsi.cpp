#include "brobot.h"

#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>

extern Brobot* g_bro;

EXTERN_C void xs_init (pTHX);

EXTERN_C void boot_DynaLoader (pTHX_ CV* cv);
EXTERN_C void boot_Win32CORE (pTHX_ CV* cv);

XS (XS_brobot_say)
{
	char *text = NULL;
	char *chan = NULL;

	dXSARGS;
	if (items != 2) {
		g_bro->irc->privmsg("#goodjob", "LOL");
	} else {
		chan = SvPV_nolen (ST (0));
		text = SvPV_nolen (ST (1));
		g_bro->irc->privmsg(chan, text);
	}
	XSRETURN_EMPTY;
};

EXTERN_C void
xs_init(pTHX)
{
	char *file = __FILE__;
	dXSUB_SYS;

	/* DynaLoader is a special case */
	newXS("DynaLoader::boot_DynaLoader", boot_DynaLoader, file);
	newXS("Win32CORE::bootstrap", boot_Win32CORE, file);
	//
	newXS("Brobot::say", XS_brobot_say, file);
};