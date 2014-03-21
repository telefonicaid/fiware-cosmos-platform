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

package es.tid.cosmos.infinity.server.db

import java.sql.Connection
import scala.util.control.NonFatal

object DB {

  def withConnection[A](conn: Connection)(block: Connection => A): A = {
    try {
      block(conn)
    } finally {
      conn.close()
    }
  }

  def withTransaction[A](conn: Connection)(block: Connection => A): A =
    withConnection(conn) { conn =>
      try {
        conn.setAutoCommit(false)
        val v = block(conn)
        conn.commit()
        v
      } catch {
        case NonFatal(e) => conn.rollback(); throw e
      }
    }

}
