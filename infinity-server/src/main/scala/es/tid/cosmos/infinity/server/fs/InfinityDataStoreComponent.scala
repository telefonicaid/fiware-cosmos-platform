package es.tid.cosmos.infinity.server.fs

import es.tid.cosmos.infinity.server.db.InfinityDataStore

trait InfinityDataStoreComponent {
  def infinityDataStore: InfinityDataStore
}
