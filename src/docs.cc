#include "docs.hh"

/*************************************************************************/

void pipedocument::parse_request(const string &s, inet_socket &i) const
{
  int pip_r[2];
  int pip_w[2];

  if ((pipe(pip_r))+(pipe(pip_w)) < 0)
    throw erro("pipedocument::parse_request pipe");

  pid_t x = fork();
  if (x == -1) throw erro("pipedocument::parse_request fork");
  if (!x)
    {
      close(pip_r[0]);
      close(pip_w[1]);
      erro e;
      if ((dup2(pip_r[1], 0)) == -1 || (dup2(pip_w[0], 1)) == -1)
	{ e.log("(child) error - dup2"); exit (128); }
      string proctitle = PACKAGE "-CGI: " + prog_name;
      //tu jeszcze setenv
     
      //if ((execlp(prog_name.c_str(), proctitle.c_str())) == -1)\
        if ((execlp("./test.sh", "TEST")) == -1)
	{ e.log("(child) error - execlp - " + string(strerror(errno))); exit (128); }
    
      exit (0);
    }
  else
    {
      close(pip_r[1]);
      close(pip_w[0]);
      sleep (10);
    }
}

/*************************************************************************/

void filedocument::parse_request(const string &s, inet_socket &i) const
{
  ifstream f(file_name.c_str());
  if (!f) throw erro("filedocument::parse_request",
		     "ID: " + get_id() + "File: " + file_name);
  char buf[256];
  for(;;)
    {
      f.read(buf, 255);
      if (f.bad()) break;
      i.write(buf, f.gcount());
      if (f.eof() || f.bad()) break;
    }
  if (f.bad()) throw erro("filedocument::parse_request",
			  "ID: " + get_id() + "File: " + file_name);
}

