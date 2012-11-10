#include "doccontainer.hh"


doccontainer::doccontainer ()
  {
    reset_iterator();
  }

/*************************************************************************/

void doccontainer::register_document (document &p_doc)
  {
    if (documents[p_doc.get_id()])
      {
	throw erro("document::document",
		   "document with id=\"" + p_doc.get_id() +
		   "\" has been already registered");
      }
    else
      {
	documents[p_doc.get_id()] = &p_doc;
      }
  }

/*************************************************************************/

void doccontainer::unregister_document (document &p_doc)
  {
    if (documents[p_doc.get_id()])
      {
	throw erro("doccontainer::unregister_document",
		   "document with id=\"" + p_doc.get_id() +
		   "\" was not found");
      }
    else
      {
	documents.erase(p_doc.get_id());
      }
  }

/*************************************************************************/
