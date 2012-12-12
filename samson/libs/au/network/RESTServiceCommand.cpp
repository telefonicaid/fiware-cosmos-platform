/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */
#include "au/network/RESTServiceCommand.h"  // Own interface

#include "au/au.pb.h"
#include "au/gpb.h"
#include "au/string/xml.h"

namespace au {
namespace network {
void FindAndReplaceInString(std::string& source, const std::string& find, const std::string& replace) {
  size_t j;

  for (; (j = source.find(find)) != std::string::npos; ) {
    source.replace(j, find.length(), replace);
  }
}

RESTServiceCommand::RESTServiceCommand() :
  token_("RESTServiceCommandBase") {
  http_state_ = 200;  // By default 200 response

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
  LOG_V(logs.rest,
        ("Start reading a REST request from socket %s",
         socket_connection->host_and_port().c_str()));

  // Read a line from socket
  char request_line[1024];
  au::Status s = socket_connection->ReadLine(request_line, sizeof(request_line), 10);

  if (s == au::OK) {
    LOG_V(logs.rest, ("REST FIRST Head line: %s", request_line));

    // Remove last "\n" "\r" characters.
    request_line_ = au::StripString(request_line);

    // Process incoming line with cmdLine
    au::CommandLine cmdLine;
    cmdLine.Parse(request_line_);

    if (cmdLine.get_num_arguments() < 2) {
      error.AddError(au::str("Unexpected format. Incomming line %s", request_line_.c_str()));
      return au::Error;
    }

    // Get request parts...
    command_ = cmdLine.get_argument(0);
    resource_ = cmdLine.get_argument(1);

    // Replace url-chars for the real ones....
    FindAndReplaceInString(resource_, "%7B", "{");
    FindAndReplaceInString(resource_, "%7D", "}");
    FindAndReplaceInString(resource_, "%22", "\"");
    FindAndReplaceInString(resource_, "%5B", "[");
    FindAndReplaceInString(resource_, "%5D", "]");

    // Get path componenets and format
    path_components_ = StringVector::ParseFromString(resource_, '/');

    // Extract extension from the last one
    format_ = "";  // Default values
    if (path_components_.size() > 0) {
      size_t pos = path_components_[path_components_.size() - 1].rfind(".");
      if (pos != std::string::npos) {
        format_ = path_components_[path_components_.size() - 1].substr(pos + 1);

        if ((format_ == "json") || (format_ == "xml") || (format_ == "txt") || (format_ == "html") || (format_
                                                                                                       == "thtml"))
        {
          path_components_[path_components_.size() - 1] = path_components_[path_components_.size() - 1].substr(0, pos);
        }
      }
    }

    // Build path from components
    path_ = "/";
    for (size_t i = 0; i < path_components_.size(); ++i) {
      path_ += path_components_[i];
      if (i != (path_components_.size() - 1)) {
        path_ += "/";
      }
    }

    // Read the rest of the REST Request
    char buffer_line[1024];
    while (s == au::OK) {
      s = socket_connection->ReadLine(buffer_line, sizeof(buffer_line), 10);
      std::string line = au::StripString(buffer_line);

      if (line.length() == 0) {
        LOG_V(logs.rest, ("REST End of header"));
        break;
      }

      if (s == au::OK) {
        std::string header_line = line;
        size_t pos = header_line.find(":");

        if (pos == std::string::npos) {
          error.AddError(au::str("No valid HTTP header field: %s", line.c_str()));
          return au::Error;
        }

        std::string concept = header_line.substr(0, pos);
        std::string value = header_line.substr(pos + 2);
        header_.Set(concept, value);

        LOG_V(logs.rest, ("REST Head line: '%s' [%s=%s]", line.c_str(), concept.c_str(), value.c_str()));
      } else {
        error.AddError("No valid HTTP header");
        return au::Error;
      }
    }

    // Read data if any....
    if (header_.IsSet("Content-Length")) {
      size_t size = header_.Get("Content-Length", 0);
      if (size > 0) {
        LOG_V(logs.rest, ("REST Reading body of %lu bytes", size));

        if (data_) {
          free(data_);
        }
        data_ = reinterpret_cast<char *>(malloc(size));
        data_size_ = size;

        s = socket_connection->readBuffer(data_, size, 10);

        if (s != au::OK) {
          error.AddError(au::str("Error reading REST body (%lu bytes)", size));
          return au::Error;
        }
      }
    }

    return au::OK;
  } else {
    error.AddError("Error reading incoming command");
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
    header << "Content-Type: text/html\n";
  } else if (format_ == "thtml") {
    header << "Content-Type: text/thtml\n";
  } else {
    LOG_SW(("no format (does this mean its XML?"));
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

void RESTServiceCommand::AppendFormatedLiteral(const std::string& name, const std::string& value) {
  std::ostringstream output;

  if (format_ == "xml") {
    au::xml_simple_literal(output, name, value);
  } else if (format_ == "json") {
    au::json_simple_literal(output, name, value);
  } else if (format_ == "html") {
    output << "<h1>" << name << "</h1>" << value;
  } else {
    output << name << ":\n" << value;
  }
  Append(output.str());
}

void RESTServiceCommand::AppendFormatedError(const std::string& message) {
  AppendFormatedLiteral("error", message);
}

void RESTServiceCommand::AppendFormatedError(int _http_state, const std::string& message) {
  set_http_state(_http_state);
  AppendFormatedLiteral("error", message);
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
}
}
