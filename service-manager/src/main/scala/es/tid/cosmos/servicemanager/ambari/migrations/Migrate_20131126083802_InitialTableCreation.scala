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
