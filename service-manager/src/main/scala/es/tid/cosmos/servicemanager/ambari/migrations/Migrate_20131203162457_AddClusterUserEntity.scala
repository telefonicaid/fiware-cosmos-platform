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

class Migrate_20131203162457_AddClusterUserEntity extends Migration {

  val tableName = "cluster_state"

  override def up() {
    execute("""CREATE TABLE `cluster_user` (
              |  `id` bigint(20) NOT NULL AUTO_INCREMENT,
              |  `cluster_id` varchar(128) NOT NULL,
              |  `user_name` varchar(128) NOT NULL,
              |  `public_key` text NOT NULL,
              |  `hdfs_enabled` tinyint(1) NOT NULL,
              |  `is_sudoer` tinyint(1) NOT NULL,
              |  `ssh_enabled` tinyint(1) NOT NULL,
              |  PRIMARY KEY (`id`),
              |  KEY `cluster_userFK1` (`cluster_id`),
              |  CONSTRAINT `cluster_userFK1`
              |     FOREIGN KEY (`cluster_id`) REFERENCES `cluster_state` (`id`) ON DELETE CASCADE
              |);""".stripMargin)
    execute("""RENAME TABLE `master` TO `cluster_master`;""")
    execute("""RENAME TABLE `slave` TO `cluster_slave`;""")
  }

  override def down() {
    execute("DROP TABLE IF EXISTS cluster_user;")
    execute("""RENAME TABLE `cluster_master` TO `master`;""")
    execute("""RENAME TABLE `cluster_slave` TO `slave`;""")
  }

}
