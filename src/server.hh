#ifndef	SERV_HH
#define	SERV_HH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef HAVE_SNPRINTF_H
#include "snprintf.h"
#endif

#include <map>
#include <time.h>

#include "erro.hh"
#include "inet.hh"
#include "confcache.hh"
#include "config.hh"
#include "document.hh"
#include "doccontainer.hh"

//name of a config option which specifies
//to which address and port we should bind

#define	OPT_BIND_TO	"listen"
#define	OPT_TIMEOUT	"timeout"

enum req_methods { GET=0, POST=1, HEAD=2, UNKNOWN };

class ht_server :	public config,
			public inet_server,
			public erro,
			public doccontainer
{
public:
  ht_server();
  ~ht_server();
  void check_handlers();
  void work ();

protected:
  void get_request();
  void get_header();
  void get_headers();
  void parse_header(const string &);
  void serve_headers(const document *d, unsigned number=200,
		     const string &i="OK");
  void serve_document();
  void send_error(unsigned, const string &);
  bool validate_headers();
  bool validate_request();
  req_methods method;
  string uri_path,
    protocol_s,
    method_s,
    h_user_agent,
    h_referer,
    h_host,
    h_content_type,
    h_cookie,
    h_authorization,
    h_content_length;

private:
  bool uses_headers;
  erro er;
  inet_socket connection;
};

#endif

