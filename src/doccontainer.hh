#ifndef	DOCCONTAINER_HH
#define	DOCCONTAINER_HH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "document.hh"
#include "erro.hh"


class doccontainer
{
public:
  doccontainer ();
  void register_document (document &);
  void unregister_document (document &);

  inline bool parse_request (inet_socket &i, const string &url = "/")
  {
    const document *d = documents[url];
    if (d) d->parse_request(url, i);
    else return false;
    return true;
  }

  inline const document * seek_document (const string &url = "/")
  { return documents[url]; }

  inline void reset_iterator ()
  { doc_pos = documents.begin(); }

  inline const document *get_next ()
  {
    Doctype::const_iterator dp = doc_pos;
    if (dp == documents.end()) { return NULL; }
    doc_pos++;
    return dp->second;
  }
  
  inline bool is_any_document ()
  {
    if (documents.begin()->second) return true;
    else return false;
  }
  
private:
  typedef map<const string, const document *> Doctype;
  Doctype::const_iterator doc_pos;
  Doctype documents;  
};


#endif
