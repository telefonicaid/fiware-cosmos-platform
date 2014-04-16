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

class Migrate_20131203162457_AddClusterUserEntity extends Migration {

  val tableName = "cluster_state"

  override def up() {
    execute("""CREATE TABLE cluster_user (
              |  id INT NOT NULL AUTO_INCREMENT,
              |  cluster_id VARCHAR(45) NOT NULL,
              |  user_name VARCHAR(45) NOT NULL,
              |  public_key text NOT NULL,
              |  hdfs_enabled TINYINT(1) NOT NULL,
              |  is_sudoer TINYINT(1) NOT NULL,
              |  ssh_enabled TINYINT(1) NOT NULL,
              |  PRIMARY KEY (id),
              |  KEY fk_cluster_user (cluster_id),
              |  CONSTRAINT fk_cluster_user
              |     FOREIGN KEY (cluster_id) REFERENCES cluster_state (id) ON DELETE CASCADE
              |);""".stripMargin)
    execute("RENAME TABLE master TO cluster_master;")
    execute("RENAME TABLE slave TO cluster_slave;")
  }

  override def down() {
    execute("DROP TABLE IF EXISTS cluster_user;")
    execute("RENAME TABLE cluster_master TO master;")
    execute("RENAME TABLE cluster_slave TO slave;")
  }

}
