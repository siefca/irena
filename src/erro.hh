/*********************************************************************************/
/** Error Handling Classes. Copyright (c) 2002 by Pawel Wilk <siefca@kernel.pl> **/
/**                          	                                        	**/
/** Please contact author to ask for license terms.				**/
/** Using and/or redistribution in any form without permission is prohibited. 	**/
/*********************************************************************************/

#ifndef _ERRO_HH
#define _ERRO_HH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef HAVE_SNPRINTF_H
#include "snprintf.h"
#endif

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#include <string>

class erro
{
	public:
		erro();
		erro(const string &, bool syserr=true);
		erro(const string &, const string &);
		~erro() { closelog(); }
		string application_name(bool braces=1) const;
		void add_err(const string &, bool);
		void add_msg(const string &);
		void show_err();
		int get_errno();
		void set_application_name(const string &);
		void set_prefix(const string &s);
		const string &get_prefix();
		void log(const string &s, int priority = LOG_NOTICE) const;
	
	private:
		int errno_code;
		static string app_name;
		string erro_str;
		string erro_msg;
		string prefix_msg;
};

#endif
