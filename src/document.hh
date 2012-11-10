#ifndef	DOCUMENT_HH
#define	DOCUMENT_HH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>
#include <map>

#include <unistd.h>
#include <sys/types.h>

#include "config.hh"
#include "erro.hh"
#include "inet.hh"


/*************************************************************************/

class document
{
public:
  document (const string &s=" ");
  virtual ~document ();
  
  virtual void parse_request (const string &, inet_socket &) const {}
  
  inline string get_id () const { return doc_id; }

  inline string get_driver () const { return driver; }

  inline const char *content_type () const { return mime_type; }
  
  inline void handle_request (inet_socket &i, const string &url) const
  { parse_request(url, i); }

  inline bool cgi_document () const
  { return is_cgi; }

protected:
  bool is_cgi;
  const char *mime_type;
  string driver;

private:
  string doc_id;
};

#endif

