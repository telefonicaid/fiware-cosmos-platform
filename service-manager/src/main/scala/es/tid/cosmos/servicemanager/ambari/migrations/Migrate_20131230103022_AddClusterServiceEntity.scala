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

package es.tid.cosmos.servicemanager.ambari.migrations

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
