#include "document.hh"


/*************************************************************************/
/* Class: document							 */

document::document (const string &id)
  : driver("VIRTUAL")
{
  if (id.empty()) 
    {
      cerr << "ah1" << endl;
      errno = 0;
      throw erro("document::document",
		 "empty document identifier");
    }

  doc_id = id;
  is_cgi = false;
  mime_type = "text/html";
}

/*************************************************************************/

document::~document ()
{
  ;
}

/*************************************************************************/


//niech inlajnowa fn content_type tez ustawia

/*************************************************************************/



