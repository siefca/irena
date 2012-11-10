#include "config.hh"

/*************************************************************************/

config::config (const string &cf_dir)
{
	config_init(cf_dir);
}

/*************************************************************************/

config::config ()
{
	config_init();
}

/*************************************************************************/

void config::config_init (const string &cf_dir=CONFIG_DIR)
{
	optcache.clear();
	config_dir = cf_dir;
	if (config_dir.substr(config_dir.length()) != "/")
			config_dir += "/";
	first_line_only = true;
}

/*************************************************************************/

string config::getopt(const string &optname)
{
	string path = config_dir + optname;
	string r = optcache[optname];
	if (!r.empty())
	{
		if (!first_line_only) return r;
		else return r.substr(0, r.find("\n"));
	}
	ifstream f(path.c_str());
	if (!f) throw erro("config::getopt ifstream",
			   "cannot open file: " + path);
	
	if (first_line_only)
	{
		getline(f, r);
	}
	else
	{
		string tmpb;
		while (getline(f, tmpb))
		{
			r += tmpb;
			r += "\n";
		}
	}
	
	if (r.empty()) throw erro("config::getopt",
				  "empty value while reading " + optname);
	optcache[optname] = r;
	return r;
}

/*************************************************************************/

int config::getopt_int(const string &optname)
{
	string s = getopt(optname);
	if (s.empty()) { return 0; }
	return atoi(s.c_str());
}

/*************************************************************************/

inline string config::operator[] (const string &optname)
{
	return getopt(optname);
}

/*************************************************************************

int main (int argc, char *argv[])
{
	config c("./deva");
		
	try {
		string s = c["test"];
		cerr << "got variable: " << s << endl;
	} catch (erro e) {
		e.show_err();
	}
	exit(0);
}

*************************************************************************/
