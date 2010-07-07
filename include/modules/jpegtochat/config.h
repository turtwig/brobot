#ifndef JPEGTOCHAT_CONFIG_H_INCLUDED
#define JPEGTOCHAT_CONFIG_H_INCLUDED

#ifdef _WIN32
#define strcasecmp _stricmp
#define snprintf _snprintf
#define sleep(a) Sleep(a*100)
#include <windows.h>
#include <getopt.h>
#else
#include <unistd.h>
#endif

#endif // JPEGTOCHAT_CONFIG_H_INCLUDED