--
-- Telefónica Digital - Product Development and Innovation
--
-- THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
-- EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
-- WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
--
-- Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
-- All rights reserved.
--

# Support multiple authentication backends

# --- !Ups
DROP INDEX `tu_id_UNIQUE` ON `user`;
ALTER TABLE `user` ADD `auth_realm` VARCHAR(45) NOT NULL DEFAULT 'default';
ALTER TABLE `user` CHANGE `user_id` `auth_id` VARCHAR(45) NOT NULL;
CREATE INDEX `auth_UNIQUE` ON `user` (`auth_realm` ASC, `auth_id` ASC);

# --- !Downs
DROP INDEX `auth_UNIQUE` ON `user`;
ALTER TABLE `user` DROP COLUMN `auth_realm`;
ALTER TABLE `user` CHANGE `auth_id` `user_id` VARCHAR(45) NULL;
CREATE INDEX `tu_id_UNIQUE` on `user` (`user_id` ASC);
