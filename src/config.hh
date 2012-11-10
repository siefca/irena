#ifndef	_CONFIG_HH
#define	_CONFIG_HH

#include <stdlib.h>
#include <fstream.h>
#include <string>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "erro.hh"
#include "confcache.hh"

#ifndef	CONFIG_DIR
#define	CONFIG_DIR	"/etc/ht-serv"
#endif

class config
{
	public:
			config();
			config(const string &);
			string getopt(const string &);
			int getopt_int(const string &);
			void config_init (const string &cf_dir=CONFIG_DIR);
			inline string operator[] (const string &);
			
			bool first_line_only;
	private:
			string config_dir;
			confcache optcache;
};

#endif
