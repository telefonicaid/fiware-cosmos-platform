#include "au/network/RESTServiceCommand.h"  // Own interface

#include "au/string/xml.h"

namespace au {
namespace network {
void find_and_replace(std::string& source, const std::string& find, const std::string& replace) {
  size_t j;

  for (; (j = source.find(find)) != std::string::npos;) {
    source.replace(j, find.length(), replace);
  }
}

RESTServiceCommand::RESTServiceCommand() :
  token_("RESTServiceCommandBase") {
  http_state_ = 200; // By default 200 response

  // No data by default
  data_ = NULL;
  data_size_ = 0;

  finished_ = false;
}

RESTServiceCommand::~RESTServiceCommand() {
  if (data_) {
    free(data_);
    data_ = NULL;
  }
}

// Read command from a socket
au::Status RESTServiceCommand::Read(SocketConnection *socket_connection, au::ErrorManager& error) {
  LM_T(LmtRest,
      ("Start reading a REST request from socket %s",
          socket_connection->host_and_port().c_str()));

  // Read a line from socket
  au::Status s = socket_connection->ReadLine(request_line_, sizeof(request_line_), 10);

  if (s == au::OK) {
    LM_T(LmtRest, ("REST FIRST Head line: %s", request_line_));

    // Remove last "\n" "\r" characters.
    au::remove_return_chars(request_line_);

    // Process incomming line with cmdLine
    au::CommandLine cmdLine;
    cmdLine.Parse(request_line_);

    if (cmdLine.get_num_arguments() < 2) {
      error.set(au::str("Unexpected format. Incomming line %s", request_line_));
      return au::Error;
    }

    // Get request parts...
    command_ = cmdLine.get_argument(0);
    resource_ = cmdLine.get_argument(1);

    // Replace url-chars for the real ones....
    find_and_replace(resource_, "%7B", "{");
    find_and_replace(resource_, "%7D", "}");
    find_and_replace(resource_, "%22", "\"");
    find_and_replace(resource_, "%5B", "[");
    find_and_replace(resource_, "%5D", "]");

    // Get path componenets and format
    path_components_ = StringVector::ParseFromString(resource_, '/');

    // Extract extension from the last one
    format_ = ""; // Default values
    if (path_components_.size() > 0) {
      size_t pos = path_components_[path_components_.size() - 1].rfind(".");
      if (pos != std::string::npos) {
        format_ = path_components_[path_components_.size() - 1].substr(pos + 1);

        if ((format_ == "json") || (format_ == "xml") || (format_ == "txt") || (format_ == "html") || (format_
            == "thtml")) {
          path_components_[path_components_.size() - 1] = path_components_[path_components_.size() - 1].substr(0, pos);
        }
      }
    }

    // Read the rest of the REST Request
    char line[1024];
    while (s == au::OK) {
      s = socket_connection->ReadLine(line, sizeof(request_line_), 10);
      au::remove_return_chars(line);

      if (strlen(line) == 0) {
        LM_T(LmtRest, ("REST End of header"));
        break;
      }

      if (s == au::OK) {
        std::string header_line = line;
        size_t pos = header_line.find(":");

        if (pos == std::string::npos) {
          error.set(au::str("No valid HTTP header field: %s", line));
          return au::Error;
        }

        std::string concept = header_line.substr(0, pos);
        std::string value = header_line.substr(pos + 2);
        header_.Set(concept, value);

        LM_T(LmtRest,
            ("REST Head line: '%s' [%s=%s]", line,
                concept.c_str(), value.c_str()));
      } else {
        error.set("No valid HTTP header");
        return au::Error;
      }
    }

    // Read data if any....
    if (header_.IsSet("Content-Length")) {
      size_t size = header_.Get("Content-Length", 0);
      if (size > 0) {
        LM_T(LmtRest, ("REST Reading body of %lu bytes", size));

        if (data_) {
          free(data_);
        }
        data_ = reinterpret_cast<char *>(malloc(size));
        data_size_ = size;

        s = socket_connection->readBuffer(data_, size, 10);

        if (s != au::OK) {
          error.set(au::str("Error reading REST body (%lu bytes)", size));
          return au::Error;
        }
      }
    }

    return au::OK;
  } else {
    error.set("Error reading incomming command");
    return au::Error;
  }
}

// Write answer to the socket
au::Status RESTServiceCommand::Write(SocketConnection *socket_connection) {
  // String with a complete answer
  std::string data = output_.str();

  // Prepare header of the HTTP message
  std::ostringstream header;

  // Redirect message
  if (redirect_.length() > 0) {
    // Prepare redirect header
    header << "HTTP/1.1 302 Found\n";
    header << "Location:   " << redirect_ << "\n";
    header << "Content-Type:   application/txt; charset=utf-8\n";
    header << "Content-Length: 0\n";
    header << "\n";
    header << "\n";

    std::string full_output = header.str();
    socket_connection->WriteLine(full_output.c_str(), 1, 0, 100);
    return au::OK;
  }

  switch (http_state_) {
    case 200:
      header << "HTTP/1.1 200 OK\n";
      break;

    case 400:
      header << "HTTP/1.1 400 Bad Request\n";
      break;

    case 404:
      header << "HTTP/1.1 404 Not Found\n";
      break;

    default:
      header << "HTTP/1.1 Bad Request \n";
      break;
  }

  if (format_ == "json") {
    header << "Content-Type: application/json\n";
  } else if (format_ == "xml") {
    header << "Content-Type: application/xml\n";
  } else if (format_ == "html") {
    header << "Content-Type: application/html\n";
  } else if (format_ == "thtml") {
    header << "Content-Type: application/thtml\n";
  } else {
    LM_W(("no format (does this mean its XML?"));
  }
  header << "Content-Length: " << data.length() << "\n";
  header << "Connection: close\n";
  header << "\n";

  // Complete output stream of data
  std::ostringstream full_output;
  full_output << header.str();
  full_output << data;

  // Write the complete output
  // Try just once, timeout 0.0001 seconds
  socket_connection->WriteLine(full_output.str().c_str(), 1, 0, 100);
  return au::OK;
}

// Command to append something at the output
void RESTServiceCommand::Append(const std::string& txt) {
  output_ << txt;
}

void RESTServiceCommand::set_http_state(int s) {
  http_state_ = s;
}

int RESTServiceCommand::http_state() {
  return http_state_;
}

std::string RESTServiceCommand::format() {
  return format_;
}

void RESTServiceCommand::set_format(const std::string format) {
  format_ = format;
}

void RESTServiceCommand::AppendFormatedElement(const std::string& name, const std::string& value) {
  std::ostringstream output;

  if (format_ == "xml") {
    au::xml_simple(output, name, value);
  } else if (format_ == "json") {
    au::json_simple(output, name, value);
  } else if (format_ == "html") {
    output << "<h1>" << name << "</h1>" << value;
  } else {
    output << name << ":\n" << value;
  }
  Append(output.str());
}

void RESTServiceCommand::AppendFormatedError(const std::string& message) {
  AppendFormatedElement("error", message);
}

void RESTServiceCommand::AppendFormatedError(int _http_state, const std::string& message) {
  set_http_state(_http_state);
  AppendFormatedElement("error", message);
}

void RESTServiceCommand::SetRedirect(const std::string& redirect_resource) {
  redirect_ = redirect_resource;
}

void RESTServiceCommand::WaitUntilFinished() {
  while (true) {
    au::TokenTaker tt(&token_);
    if (finished_) {
      return;
    } else {
      tt.Stop();
    }
  }
}

void RESTServiceCommand::NotifyFinish() {
  au::TokenTaker tt(&token_);

  finished_ = true;
  tt.WakeUp();
}

const StringVector& RESTServiceCommand::path_components() {
  return path_components_;
}

std::string RESTServiceCommand::command() {
  return command_;
}
}
}
