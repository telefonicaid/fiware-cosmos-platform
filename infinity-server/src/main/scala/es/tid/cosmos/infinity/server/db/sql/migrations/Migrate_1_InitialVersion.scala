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

package es.tid.cosmos.infinity.server.db.sql.migrations

import com.imageworks.migration.Migration

import es.tid.cosmos.infinity.server.fs.RootInode

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

    execute(s"""INSERT INTO `inode` (`id`, `name`, `directory`, `owner`,
              |  `group`, `permissions`, `parent_id`)
              |  VALUES ('${RootInode.Id}', '${RootInode.Name}', true, 'root', 'root', '777', '0')
            """.stripMargin)

  }

  def down(): Unit = {

    execute("""DROP TABLE `inode`""")

  }

}
