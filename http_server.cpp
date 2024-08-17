#include "http_server.hh"

#include <vector>

#include <sys/stat.h>

#include <fstream>
#include <sstream>

vector<string> split(const string &s, char delim) {
  vector<string> elems;

  stringstream ss(s);
  string item;

  while (getline(ss, item, delim)) {
    if (!item.empty())
      elems.push_back(item);
  }

  return elems;
}
string getstring(string& url) {
  
    ifstream input_file(url);
    auto ss = ostringstream{};
    if (input_file.is_open()) {
        ss << input_file.rdbuf();
    return ss.str();
    }
    cout<< "Failure in opening file";
             
        exit(1);
    
}
HTTP_Request::HTTP_Request(string request) {
  vector<string> lines = split(request, '\n');
  vector<string> first_line = split(lines[0], ' ');

  this->HTTP_version = "1.1"; // We'll be using 1.0 irrespective of the request
  this->method=first_line[0];
  /*
   TODO : extract the request method and URL from first_line here
  */

  if (this->method != "GET") {
    cerr << "Method '" << this->method << "' not supported" << endl;
    exit(1);
  }
  this->url=first_line[1];
}

HTTP_Response *handle_request(string req) {
  HTTP_Request *request = new HTTP_Request(req);

  HTTP_Response *response =  new HTTP_Response();

  string url = string("html_files") + request->url;

  response->HTTP_version = "1.0";

  struct stat sb;int qpos=-1;
  for(int i=0;i<url.length();i++) {
    if(url[i]=='?') {
      qpos=i;
      break;
    }
  }
  if (stat(url.c_str(), &sb) == 0) // requested path exists
  {
    response->status_code = "200";
    response->status_text = "OK";
    response->content_type = "text/html";

    string body;

    if (S_ISDIR(sb.st_mode)) {
      /*
      In this case, requested path is a directory.
      TODO : find the index.html file in that directory (modify the url
      accordingly)
      */
      if(url.back()!='/') {
        url+="/index.html";
      }
      else {
        url+="index.html";
      }
    }

    /*
    TODO : open the file and read its contents
    */
    response->body=getstring(url);
    response->content_length=to_string(response->body.length());
    
    /*
    TODO : set the remaining fields of response appropriately
    */
  }

  

  else {
    

    /*
    TODO : set the remaining fields of response appropriately
    */
    response->status_code = "404";
    response->status_text="Not Found!";
  }

  delete request;

  return response;
}

string HTTP_Response::get_string() {
  /*
  TODO : implement this function
  */
  string ans;
  ans+="HTTP/" + this->HTTP_version + " " + this->status_code + " " + this->status_text+"\n";
  ans+="Length: "+this->content_length+"\n";
  ans+="Type: "+this->content_type+"\n";
  ans+="\n\n"+this->body;
  

 return ans;
}
