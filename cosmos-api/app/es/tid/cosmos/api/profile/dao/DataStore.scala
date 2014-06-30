package es.tid.cosmos.api.profile.dao

/** This trait creates an abstraction on where underlying data is stored, such as SQL databases,
  * in-memory lists, mocks, etc.
  */
trait DataStore {
  /** Type of the data store connection */
  type Conn
  def withConnection[A](block: Conn => A): A
  def withTransaction[A](block: Conn => A): A
}
