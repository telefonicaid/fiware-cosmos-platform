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

package es.tid.cosmos.infinity.server.db.migrations

import com.imageworks.migration.Migration

class Migrate_1_InitialVersion extends Migration {

  def up(): Unit = {

    execute("""CREATE TABLE `inode` (
              |  `id`          VARCHAR(45) NOT NULL,
              |  `name`        VARCHAR(255)NOT NULL,
              |  `directory`   BOOLEAN NOT NULL,
              |  `owner`       VARCHAR(255) NOT NULL,
              |  `group`       VARCHAR(255) NOT NULL,
              |  `permissions` CHAR(3) NOT NULL,
              |  `parent_id`   VARCHAR(45) NOT NULL,
              |  PRIMARY KEY (`id`),
              |  CONSTRAINT `fk_inode_parent`
              |      FOREIGN KEY (`parent_id`)
              |      REFERENCES `inode` (`id`)
              |)""".stripMargin)

    execute("""CREATE UNIQUE INDEX `idx_inode_findByParentAndName`
              |  ON `inode` (`parent_id`, `name`)""".stripMargin)

    execute("""INSERT INTO `inode` (`id`, `name`, `directory`, `owner`,
              |  `group`, `permissions`, `parent_id`)
              |  VALUES ('0', '/', true, 'root', 'root', '777', '0')""".stripMargin)

  }

  def down(): Unit = {

    execute("""DROP TABLE `inode`""")

  }

}
