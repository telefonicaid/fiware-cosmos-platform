package es.tid.cosmos.api.profile.sql

import java.sql.Connection

import play.api.Play.current
import play.api.db.DB

import es.tid.cosmos.api.profile.DataStore

object PlayDbDataStore extends DataStore[Connection] {
  override def withConnection[A](block: Connection => A): A = DB.withConnection[A](block)
  override def withTransaction[A](block: Connection => A): A = DB.withTransaction[A](block)
}
