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

package es.tid.cosmos.servicemanager.clusters.sql.migrations

import com.imageworks.migration.Migration

class Migrate_20131126083802_InitialTableCreation extends Migration {
  val tableName = "cluster_state"

  override def up() {
    execute("""CREATE TABLE cluster_state (
              |  id VARCHAR(45) NOT NULL,
              |  name VARCHAR(45) NOT NULL,
              |  size INT NOT NULL,
              |  name_node VARCHAR(45),
              |  state VARCHAR(20) NOT NULL,
              |  reason TEXT,
              |  PRIMARY KEY (id)
              |);""".stripMargin)

    execute("""CREATE TABLE master (
              |  name VARCHAR(45) NOT NULL,
              |  cluster_id VARCHAR(45) NOT NULL,
              |  ip VARCHAR(45) NOT NULL,
              |  PRIMARY KEY (cluster_id),
              |  CONSTRAINT fk_cluster_master
              |    FOREIGN KEY (cluster_id)
              |    REFERENCES cluster_state (id)
              |);""".stripMargin)

    execute("""CREATE TABLE slave (
              |  name VARCHAR(45) NOT NULL,
              |  cluster_id VARCHAR(45) NOT NULL,
              |  ip VARCHAR(45) NOT NULL,
              |  PRIMARY KEY (name, cluster_id, ip),
              |  CONSTRAINT fk_cluster_slave
              |    FOREIGN KEY (cluster_Id)
              |    REFERENCES cluster_state (id)
              |);""".stripMargin)

  }

  override def down() {
    execute("DROP TABLE IF EXISTS slave;")
    execute("DROP TABLE IF EXISTS master;")
    execute("DROP TABLE IF EXISTS cluster_state;")
  }
}
