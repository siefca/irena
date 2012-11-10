
#include "confcache.hh"

/*************************************************************************/

int confcache::getopt_int(const string &optname)
{
	clear_err();
	string s(find(optname)->second);
	if (s.empty()) { set_err(); return -1; }
	errno = 0;
	int r(strtol(s.c_str(), (char **)NULL, 10));
	if (errno == ERANGE) { set_err(); return -1; }
	return r;
}

/*************************************************************************/

