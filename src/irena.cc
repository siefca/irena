#include "irena.hh"

int main (int argc, char *argv[])
{
  {
    erro e;
    e.log(e.application_name(0) + " web server (" VERSION ") initializing..");
  }
  
  try {

    ht_server Server;

    pipedocument _doc_pipe("/pipe", "./test.sh");
//    filesdocument _doc_files();
    filedocument _doc_file("/file", "./file.html");
    doc_helo _doc_helo("/helo");
    
    Server.register_document(_doc_file);
    Server.register_document(_doc_helo);
    Server.register_document(_doc_pipe);

    Server.work();
    
  } catch (erro &e) {
    
    e.show_err();
    exit(e.get_errno());
    
  }

  exit (0);
}

