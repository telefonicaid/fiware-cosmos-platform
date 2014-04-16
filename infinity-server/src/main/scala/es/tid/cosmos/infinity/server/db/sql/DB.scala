/*
 * Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package es.tid.cosmos.infinity.server.db.sql

import java.sql.Connection
import scala.util.control.NonFatal

private[sql] object DB {

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
