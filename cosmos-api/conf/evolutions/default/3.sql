--
-- Copyright (c) 2013-2014 Telefónica Investigación y Desarrollo S.A.U.
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--

# Support multiple authentication backends

# --- !Ups
DROP INDEX `tu_id_UNIQUE` ON `user`;
ALTER TABLE `user` ADD `auth_realm` VARCHAR(45) NOT NULL DEFAULT 'tuid';
ALTER TABLE `user` CHANGE `user_id` `auth_id` VARCHAR(45) NOT NULL;
ALTER TABLE `user` ADD UNIQUE `auth_UNIQUE` (`auth_realm` ASC, `auth_id` ASC);
ALTER TABLE `user` ADD UNIQUE `handle_UNIQUE` (`handle` ASC);

# --- !Downs
DROP INDEX `auth_UNIQUE` ON `user`;
DROP INDEX `handle_UNIQUE` ON `user`;
ALTER TABLE `user` DROP COLUMN `auth_realm`;
ALTER TABLE `user` CHANGE `auth_id` `user_id` VARCHAR(45) NULL;
CREATE INDEX `tu_id_UNIQUE` on `user` (`user_id` ASC);
