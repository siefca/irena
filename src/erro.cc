/*********************************************************************************/
/** Error Handling Classes. Copyright (c) 2002 by Pawel Wilk <siefca@kernel.pl> **/
/**                          	                                        	**/
/** Please contact author to ask for license terms.				**/
/** Using and/or redistribution in any form without permission is prohibited. 	**/
/*********************************************************************************/

#include "erro.hh"

#ifndef	PACKAGE
#define	PACKAGE	""
#endif

string erro::app_name = PACKAGE;

/*************************************************************************/

erro::erro ()
{
	char buf[16];
	erro_str = "std call error ";
	snprintf (buf, 15, "%d", errno);
	errno_code = errno;
	erro_str += buf;
	erro_str += ": ";
	erro_str + strerror(errno);
}

/*************************************************************************/

erro::erro (const string &s, bool syserr=true)
{
	if (syserr) errno_code = errno;
	add_err(s, syserr);
}

/*************************************************************************/

erro::erro (const string &s, const string &m)
{
	errno_code = errno;
	add_err(s, true);
	add_msg(m);
}
	
/*************************************************************************/

void erro::add_err (const string &s, bool syse)
{
	if (!syse) { erro_str = s; return; }
	erro_str = "error ";
	if (errno) {
		char buf[16];
		snprintf (buf, 15, "%d", errno);
		erro_str += buf;
		erro_str += " ";
		}
	erro_str += "at ";
	erro_str += s;
	if (errno) {
		erro_str += " (";
		erro_str += strerror(errno);
		erro_str += ")";
	} else erro_msg = s;	
}

/*************************************************************************/

void erro::add_msg (const string &m)
{
	erro_msg = m;
}

/*************************************************************************/

void erro::show_err ()
{
	openlog(app_name.c_str(), LOG_PID|LOG_CONS, LOG_DAEMON);

	if (!erro_str.empty())
	    {
		syslog(LOG_ERR, "%s%s", prefix_msg.c_str(), erro_str.c_str());
		//cerr << application_name() << prefix_msg << erro_str << endl;
	    }
	if (!erro_msg.empty())
	    {
		syslog(LOG_ERR, "%smore info: %s", prefix_msg.c_str(),
						   erro_msg.c_str());
		//cerr << application_name()
		//     << prefix_msg << "more info: " << erro_msg << endl;
	    }
}

/*************************************************************************/

string erro::application_name (bool braces=1) const
{
	if (app_name.empty()) return app_name;
	string s;
	if (braces) s = "[";
	s += app_name;
	if (braces) s += "] ";
	return s;
}

/*************************************************************************/

void erro::set_application_name (const string &s)
{
	app_name = s;
}

/*************************************************************************/

void erro::set_prefix (const string &s)
{
	prefix_msg = s;
}

/*************************************************************************/


int erro::get_errno ()
{
	return errno_code;
}

/*************************************************************************/

void erro::log (const string &s, int priority = LOG_NOTICE) const
{
	openlog(app_name.c_str(), LOG_PID, LOG_DAEMON);
	syslog(priority, "%s%s", prefix_msg.c_str(), s.c_str());
	//cerr << application_name() << prefix_msg << s << endl;
}

/*************************************************************************/

const string &erro::get_prefix ()
{
	return prefix_msg;
}
