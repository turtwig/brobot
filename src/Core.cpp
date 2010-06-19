#include "../include/Core.h"
#include <iostream>

void CoreModule::onLoad(Brobot* bro) {
    // Parser functions
    bro->addParser("ping", boost::bind(&CoreModule::ping, this, bro, _1));
    bro->addParser("numerics", boost::bind(&CoreModule::numerics, this, bro, _1));
    bro->addParser("commands", boost::bind(&CoreModule::commands, this, bro, _1));
    // Hooks
    bro->hook("[core] Ping", "OnPING", boost::bind(&CoreModule::pingHook, this, bro, _1));
};

void CoreModule::onUnload(Brobot* bro) {
    // Parser functions
    bro->delParser("commands");
    bro->delParser("numerics");
    bro->delParser("ping");
    // Hooks
    bro->unhook("[core] Ping", "OnPING");
};

/* Calls numeric hooks.
 * The hook name syntax is (for numeric 001):
 *  Numeric001
 * It forwards the text (after the nick argument) the numeric sends
 * including (or not) the : before it.
 */
void CoreModule::numerics(Brobot* bro, const std::string& str) {
    size_t delim1 = str.find(' ');
    size_t delim2 = str.find(' ', ++delim1);
    if (str.substr(delim1, delim2-delim1).find_first_of("0123456789") == std::string::npos) // no numbers
        return; // nothing else to do
    size_t argsDelim = str.find(' ', delim2+1)+1;
    Args arg;
    bro->runHooks("Numeric" + str.substr(delim1, delim2-delim1), arg % str.substr(argsDelim, std::string::npos));
};

/* Handles most user commands
 * including, for example, PRIVMSGs and NOTICEs.
 * the hook name(s) follows the pattern of:
 * OnCOMMAND, OnPRIVMSG being the one for PRIVMSGs.
 * It forwards the matches as the arguments for the hook
 * a[0] being the full raw string, a[1] the nick, a[2] the ident,
 * a[3] the host ,a[4] the target and
 * a[5] the actual message.
 */
void CoreModule::commands(Brobot* bro, const std::string& str) {
    static const boost::regex expr("^:(.+?)!(.+?)@(.+?) (.+?) (.+?) :(.+?)$"); // matches :NICK!IDENT@HOST COMMAND TARGET :MESSAGE
    boost::cmatch match;
    if (!boost::regex_match(str.c_str(), match, expr)) // we did not get a match
        return; // nothing else to do
    Args arg;
    bro->runHooks("On" + match[4], arg % str % match[1] % match[2] % match[3] % match[5] % match[6]);
};

/* Handles PING
 * fires hook OnPING.
 * single argument is the text to PONG back
 */
void CoreModule::ping(Brobot* bro, const std::string& str) {
    if (str.substr(0,4) != "PING")
        return; //nothing else to do
    Args arg;
    bro->runHooks("OnPING", arg % str.substr(5, std::string::npos));
};

// Responds to PING
void CoreModule::pingHook(Brobot* bro, Args& arg) {
    bro->irc->raw("PONG " + arg[0]); // simply respond to the PING
};
