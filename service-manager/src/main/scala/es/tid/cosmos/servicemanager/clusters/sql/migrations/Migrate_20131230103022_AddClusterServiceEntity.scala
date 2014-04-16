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

class Migrate_20131230103022_AddClusterServiceEntity extends Migration {

  val tableName = "cluster_service"

  override def up() {
    execute("""CREATE TABLE cluster_service (
              |  id INT NOT NULL AUTO_INCREMENT,
              |  cluster_id VARCHAR(45) NOT NULL,
              |  name VARCHAR(45) NOT NULL,
              |  PRIMARY KEY (id),
              |  KEY fk_cluster_service (cluster_id),
              |  CONSTRAINT fk_cluster_service
              |     FOREIGN KEY (cluster_id) REFERENCES cluster_state (id) ON DELETE CASCADE
              |);""".stripMargin)
  }

  override def down() {
    execute("DROP TABLE IF EXISTS cluster_service;")
  }
}
