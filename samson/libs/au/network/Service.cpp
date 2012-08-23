

#include "au/ThreadManager.h"

#include "Service.h"  // Own interface



namespace au {
namespace network {
void *run_service_item(void *p) {
  // Recover the item from the provided pointer
  ServiceItem *item = (ServiceItem *)p;

  // Main function in Service to define what to do with new connections
  item->service_->run(item->socket_connection_, &item->quit_);

  // Notify finish of this thread
  item->service_->finish(item);

  // Remove item since it is not contained anywhere
  delete item;

  return NULL;
}

ServiceItem::ServiceItem(Service *service,
                         SocketConnection *socket_connection) {
  // Keep a pointer to the service to nofity when finished
  service_ = service;

  // Keep a pointer to the socket connection
  socket_connection_ = socket_connection;

  // By default, do not quit
  quit_ = false;
}

ServiceItem::~ServiceItem() {
  // Make sure this connection is closed
  socket_connection_->Close();
  delete socket_connection_;
}

void ServiceItem::Stop() {
  socket_connection_->Close();     // Close connection, so the thread will come back
  quit_ = true;                    // Also inform with this bool variable ( accessible in connection )
}

void ServiceItem::RunInBackground() {
  // Run a separate thread for this connection
  ThreadManager::shared()->addThread("ServiceItem", &t_, NULL, run_service_item,
                                     this);
}

Service::Service(int port) : token_("Service"), listener_(this) {
  port_ = port;
  init_ = false;
}

Service::~Service() {
  listener_.StopNetworkListener();
}

Status Service::InitService() {
  if (init_) {
    return au::Error;
  }

  Status s = listener_.InitNetworkListener(port_);
  return s;
}

std::string Service::str() const {
  return au::str("Server on port %d", port_);
}

void Service::StopService() {
  // Stop all connections ( close socket and inform about quit )
  {
    au::TokenTaker tt(&token_);
    au::set< ServiceItem >::iterator it_items;
    for (it_items = items_.begin(); it_items != items_.end(); it_items++) {
      (*it_items)->Stop();
    }
  }

  // Wait for all items to finish
  while (true) {
    if (items_.size() == 0) {
      break;
    } else {
      usleep(100000);
    }
  }

  // Stop the main listener
  listener_.StopNetworkListener();

  // Wait until all connections are gone...
  {
    au::Cronometer c;
    while (true) {
      // Check if all connections are gone
      size_t num_items = 0;
      {
        au::TokenTaker tt(&token_);
        num_items = items_.size();
        if (num_items == 0) {
          return;
        }
      }

      if (c.seconds() > 1) {
        c.Reset();
        LM_W((
               "Still %lu ServiceItems do not finish after closing its associated sockets in Server %s",
               num_items, str().c_str()));
      }

      usleep(10000);
    }
  }
}

void Service::newSocketConnection(NetworkListener *listener,
                                  SocketConnection *socket_connetion) {
  au::TokenTaker tt(&token_);

  if (listener != &listener_) {
    LM_E(("Unexpected listner in au::Service"));
    return;
  }

  // Create the item
  ServiceItem *item = new ServiceItem(this, socket_connetion);

  // Insert the item
  items_.insert(item);

  // Run in background
  item->RunInBackground();
}

au::tables::Table *Service::getConnectionsTable() const {
  std::string format =
    "Host|time,f=time|In (B),f=uint64|Out (B),f=uint64|In (B/s),f=uint64|Out (B/s),f=uint64";
  au::tables::Table *table = new au::tables::Table(format);

  au::set< ServiceItem >::iterator it_items;
  for (it_items = items_.begin(); it_items != items_.end(); it_items++) {
    ServiceItem *item = *it_items;

    au::StringVector values;

    values.push_back(item->socket_connection_->host_and_port());
    values.push_back(au::str("%lu", item->socket_connection_->GetConnectionTime()));

    values.push_back(au::str("%lu",
                             (size_t)item->socket_connection_->rate_in().
                             size()));
    values.push_back(au::str("%lu",
                             (size_t)item->socket_connection_->rate_out().
                             size()));
    values.push_back(au::str("%lu",
                             (size_t)item->socket_connection_->rate_in().
                             rate()));
    values.push_back(au::str("%lu",
                             (size_t)item->socket_connection_->rate_out().
                             rate()));

    table->addRow(values);
  }

  return table;
}

int Service::port() const {
  return port_;
}

void Service::finish(ServiceItem *item) {
  // Mutex proctection
  au::TokenTaker tt(&token_);

  // Notify this has finished
  items_.erase(item);
}

std::string Service::GetStringStatus() const {
  if (listener_.IsNetworkListenerRunning()) {
    return au::str("listening on port %d", listener_.port());
  } else {
    return "not listening";
  }
}
}
}