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

class Migrate_20140220123200_LongerClusterNames extends Migration {

  val tableName = "cluster_state"

  override def up() {
    execute("ALTER TABLE cluster_state CHANGE COLUMN name name VARCHAR(120) NOT NULL;")
  }

  override def down() {
    execute("UPDATE cluster_state SET name = SUBSTRING(name, 1, 45);")
    execute("ALTER TABLE cluster_state CHANGE COLUMN name name VARCHAR(80) NOT NULL;")
  }
}
