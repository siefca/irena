#include "server.hh"

/*************************************************************************/

ht_server::ht_server ()
  : inet_server (getopt(OPT_BIND_TO))
{
  reuse_addr(true);
  bind();
  timeout(getopt_int(OPT_TIMEOUT));
  uses_headers = true;

  log("server object initialized");
}

ht_server::~ht_server()
{
  ;
}

/*************************************************************************/

void ht_server::check_handlers ()
{
  log("server object handlers check");
  if (!is_any_document()) throw erro("No handlers found!");
  log("=> found handlers:");
  const document *d;
  string tmps;
  reset_iterator();
  while ((d = get_next()))
  {
    tmps = "==> (";
    tmps += d->get_driver();
    tmps += ") ";
    tmps += d->get_id();
    tmps += " [";
    tmps += d->content_type();
    tmps += "]";
    log(tmps);
  }
  
  if (tmps.empty())
    throw erro("Empty aftercheck string. Aborting.");

  log("handlers checked");
}

/*************************************************************************/

void ht_server::get_request ()
{
  method = UNKNOWN;
  string s;
  s = connection.readline(256);
  if (s.empty()) return;
  unsigned int pos = s.find_first_of(" \t");
  if (pos == string::npos) return;
  
  /* method */
  const string &p = s.substr(0, pos).c_str();
  if (p.empty()) return;

  if (p == "GET")
    method = GET;
  else
    {
      if (p == "POST")
	method = POST;
      else
	{
	  if (p == "HEAD")
	    method = HEAD;
	  else
	    return;
	}
    }

  method_s = p;
  
  /* url */
  for (string::iterator pp = s.substr(pos).begin();
       pp != s.end();
		     pp++, pos++) if (*pp != ' ' && *pp != '\t') break;
  
  unsigned int epos = s.substr(pos).find_first_of(" \t");
  uri_path = s.substr(pos, epos);
  
  /* protocol - optional */
  if (epos != string::npos)
    {
      epos += pos;
      for (string::iterator pp = s.substr(epos).begin();
	   pp != s.end();
	   pp++, epos++) if (*pp != ' ' && *pp != '\t') break;
      
      pos = s.substr(epos).find_first_of(" \t");
      protocol_s = s.substr(epos, pos);
    }
}

/*************************************************************************/

void ht_server::parse_header (const string &s)
{
  unsigned int sep = s.find(":");
  if (sep == string::npos) return;
  const char *p = s.substr(0, sep).c_str();
  if (!p || !*p) return;
  const char *a = s.substr(sep).c_str();
  if (!a || !*a) return;
  while (*a == ' ' || *a == '\t' || *a == ':')
    { a++; sep++; }
  if (!*a) return;
  
  if (!strcasecmp(p, "user-agent")) 
    { h_user_agent = s.substr(sep); return; }
  if (!strcasecmp(p, "content-length"))
    { h_content_length = s.substr(sep); return; }
  if (!strcasecmp(p, "authorization"))
    { h_authorization = s.substr(sep); return; }
  if (!strcasecmp(p, "content-type"))
    { h_content_type = s.substr(sep); return; }
  if (!strcasecmp(p, "host"))
    { h_host = s.substr(sep); return; }
  if (!strcasecmp(p, "cookie"))
    { h_cookie = s.substr(sep); return; }
  if (!strcasecmp(p, "referer"))
    { h_referer = s.substr(sep); return; }
}

/*************************************************************************/

void ht_server::get_headers ()
{
  if (!uses_headers) return;
  
  string s;
  while (1)
    {
      s = connection.readline(256);
      if (s.empty()) break;
      parse_header (s);
      //cerr << s << endl;
    }
}

/*************************************************************************/

void ht_server::send_error(unsigned number, const string &i)
{
  static string ergen;
  static char buf[8];
  
  serve_headers(NULL, number, i);

  snprintf(buf, 4, "%d", number);
  buf[3] = '\0';

  ergen = "<HTML><HEAD><TITLE>";
  ergen += buf;
  ergen += " ";
  ergen += i;
  ergen += "</TITLE></HEAD><BODY><HR><B>";
  ergen += buf;
  ergen += " ";
  ergen += i;
  ergen += "<HR></B></BODY></HTML>\n";
  connection.write(ergen);

  ergen = "> ERROR ";
  ergen += buf;
  log(ergen);
}

/*************************************************************************/
			   
bool ht_server::validate_request()
{
  if (uri_path.empty() || method == UNKNOWN)
    throw erro("malformed request", false);
  
  if (protocol_s.empty())
    {
      if (method == GET) {
	protocol_s = "HTTP/0.9";
	uses_headers = 0;
      } else {
	throw erro("malformed protocol string", false);
      }
    }
  return 1;
}

/*************************************************************************/

bool ht_server::validate_headers()
{
  if (!uses_headers) return 1;
  return 1;
}

/*************************************************************************/

void ht_server::serve_headers(const document *d, unsigned number=200,
			      const string &i="OK")
{
  static string hdr_buf;
  static char buf[64];
  static const char *tmft = "Date: %a, %d %b %Y %H:%M:%S GMT";
  if (!uses_headers) return;

  snprintf(buf, 4, "%d", number);
  buf[3] = '\0';
  hdr_buf = "HTTP/1.0 ";
  hdr_buf += buf;
  hdr_buf += " ";
  hdr_buf += i;
  hdr_buf += "\r\n";

  time_t cur_time = time(NULL);
  strftime(buf, sizeof(buf), tmft, gmtime(&cur_time));
  buf[63] = '\0';

  hdr_buf += buf;
  hdr_buf += "\r\nServer: "
    PACKAGE
    "/"
    VERSION
    "\r\nConnection: close\r\n";

  if (!d || !(d->cgi_document()))
    {
      hdr_buf += "Content-Type: ";
      hdr_buf += (d && d->content_type()) ? (d->content_type()) : "text/html";
      hdr_buf += "\r\n\r\n";
    }
  connection.write(hdr_buf);
}

/*************************************************************************/

void ht_server::serve_document()
{
  log("< " + method_s + " " + uri_path + " (" + protocol_s + ")");
  const document *d = seek_document(uri_path);
  if (!d)
    {
      send_error(404, "Document NOT FOUND error");
      return;
    }

  serve_headers(d);
  d->handle_request(connection, uri_path);
  log("> OK");
}

/*************************************************************************/

void ht_server::work ()
{
  string s;
  
  check_handlers();
  log("server is listening on " + local_address_port());
  
  connection = accept();
  set_prefix("[" + connection.remote_address_port() + "] ");
  log("CONNECT");
  
  try {

    get_request(); validate_request();
    get_headers(); validate_headers();
    serve_document();
    
  } catch (erro &e) {
    
    e.set_prefix(get_prefix());
    if (connection.read_timed_out()) e.log("read timeout");
    else 
      if (connection.write_timed_out()) e.log("write timeout");
      else 
	e.show_err();
    e.log("disconnecting");
    //send_error_doc(); //a wczesniejsze niech ustawiajom pointer abo co
    connection.close();
  }
  
  //cerr << "uri: |" << uri_path << "|" << endl
  //     << "protocol: |" << protocol_s << "|" << endl
  //     << "user-agent: |" << h_user_agent << "|" << endl;
  connection.close();
}

/*************************************************************************/

