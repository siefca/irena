#ifndef	DOCS_HH
#define	DOCS_HH

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string>

#include "document.hh"
#include "erro.hh"



class pipedocument : public document
{
public:
  inline pipedocument (const string &s, const string &prog)
    : document(s), prog_name(prog) { driver = "PIPE"; is_cgi = true; }
  virtual void parse_request(const string &, inet_socket &) const;
  
private:
  inline void child_code (int d_in, d_out, d_err)
  {
    while ((z=read(pip_r[0], buf, 255)) > 0)
      {
	buf[z] = 0;
	fprintf(stderr, "was: %s", buf);
      }
    
    if (!z)
      {
	printf("EOF from child!\n");
	exit(0);
      }
    else if (z == -1)
      {
	perror("read");
	exit(10);
      }
    
    fprintf (stderr, "chld has exited\n");
    perror ("dupa");
  }
  string prog_name;
};


class filedocument : public document
{
public:
  inline filedocument (const string &s, const string &filename)
    : document(s), file_name(filename) { driver = "FILE"; }
  virtual void parse_request(const string &, inet_socket &) const;
private:
  string file_name;
};



class filesdocument : public document
{
public:
  filesdocument (const string &s, const string &directory, bool from_file=0)
    : document(s)
  {
    driver = from_file ? "FILELIST" : "DIR";
    
    if (from_file)
      {
	;//fstream("");
      }
    //otworzyc directory (albo plik, jesli from_file=1)
    //dla kazdego pliku:
    //doc_tab[x] = new filedocument(f+nazwapliku);
    //x++
    //potem w destruktorze pousuwac
  }
  
};

//class ssidocument


class doc_helo : public document
{
public:
  inline doc_helo(const string &s) : document(s)
  {
    mime_type = "text/plain";
    driver = "TEST";
  };
  
  inline virtual void parse_request (const string &u, inet_socket &i) const
  {
    i.write("Hello World!");
  }
};



#endif
