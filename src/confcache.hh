#ifndef	CONFCACHE_HH
#define	CONFCACHE_HH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <errno.h>

#include <string>
#include <map>

typedef map<string, string> cfcache;

class confcache : public cfcache
{
	public:
			int getopt_int(const string &);
			inline bool conversion_error() { return err; }
	private:
			inline void set_err() { err = true; }
			inline void clear_err() { err = false; }
			bool err;
};


#endif

