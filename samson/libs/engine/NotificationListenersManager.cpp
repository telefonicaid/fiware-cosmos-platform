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


#include "logMsg/logMsg.h"      // LOG_SM
#include "logMsg/traceLevels.h"  // LmtEngineNotification

#include "engine/Logs.h"
#include "engine/Notification.h"       // engine::Notification
#include "engine/NotificationListener.h"             // engine::NotificationListener

#include "NotificationListenersManager.h"     // Own interface

namespace engine {
NotificationListenersManager::NotificationListenersManager() : token_("engine::NotificationListenersManager") {
  engine_id_ = 1;
}

NotificationListenersManager::~NotificationListenersManager() {
  // Remove the vectors with ids for each channel
  channels_.clearMap();
}

void NotificationListenersManager::Add(NotificationListener *o) {
  au::TokenTaker tt(&token_);

  // Give a global id to this object
  o->engine_id_ = engine_id_++;

  // Adding object to the general map by id
  objects_.insertInMap(o->engine_id_, o);

  LOG_D(logs.notifications, ("Add object %p for id %lu", o, o->engine_id_ ));
}

void NotificationListenersManager::Remove(NotificationListener *o) {
  au::TokenTaker tt(&token_);

  LOG_D(logs.notifications, ("Remove completely object %lu", o, o->engine_id_  ));
  objects_.extractFromMap(o->engine_id_);

  au::map< const char *, IdsCollection, au::strCompare >::iterator c;
  for (c = channels_.begin(); c != channels_.end(); c++) {
    c->second->remove(o->engine_id_);
  }
}

void NotificationListenersManager::AddToChannel(NotificationListener *o, const char *name) {
  au::TokenTaker tt(&token_);

  LOG_D(logs.notifications, ("Add object %lu to channel %s", o->engine_id_, name ));
  GetListenersForChannel(name)->add(o->engine_id_);
}

void NotificationListenersManager::RemoveFromChannel(NotificationListener *o, const char *name) {
  au::TokenTaker tt(&token_);

  LOG_D(logs.notifications, ("Remove object %lu to channel %s", o->engine_id_, name ));
  GetListenersForChannel(name)->remove(o->engine_id_);
}

// Get the collections of ids for a particular channel
std::set<size_t> NotificationListenersManager::GetEndgineIdsForChannel(const char *name) {
  au::TokenTaker tt(&token_);

  std::set<size_t> ids;
  IdsCollection *delivery = channels_.findInMap(name);
  if (delivery) {
    delivery->addTo(ids);
  }
  return ids;
}

// Get the collections of ids for a particular channel
IdsCollection *NotificationListenersManager::GetListenersForChannel(const char *name) {
  IdsCollection *delivery = channels_.findInMap(name);

  if (!delivery) {
    delivery = new IdsCollection();
    channels_.insertInMap(name, delivery);
  }
  return delivery;
}

void NotificationListenersManager::Send(Notification *notification) {
  // Get list of objects that should be notified because they are listening the channel
  std::set<size_t> ids = GetEndgineIdsForChannel(notification->name());

  // Add objects that should be notified directly...
  std::set<size_t>::const_iterator t;
  for (t = notification->targets().begin(); t != notification->targets().end(); t++) {
    ids.insert(*t);
  }

  // Notify all the objects
  for (t = ids.begin(); t != ids.end(); t++) {
    Send(notification, *t);
  }
}

// Run a notification in an object
void NotificationListenersManager::Send(Notification *notification, size_t target) {
  NotificationListener *o = NULL;

  {
    au::TokenTaker tt(&token_);
    o = objects_.findInMap(target);
  }

  if (o) {
    o->notify(notification);
  }
}
}
