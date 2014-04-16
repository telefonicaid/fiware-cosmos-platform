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

-- * Add groups table and association to users

# --- !Ups

CREATE TABLE user_group (
  name VARCHAR(45) NOT NULL,
  min_quota INT NOT NULL,
  PRIMARY KEY (name)
);

ALTER TABLE user
  ADD group_name VARCHAR(45) NULL,
  ADD INDEX group_name_INDEX (group_name ASC),
  ADD CONSTRAINT fk_user_user_group
    FOREIGN KEY (group_name)
    REFERENCES user_group (name)
    ON DELETE SET NULL;

# --- !Downs

ALTER TABLE user DROP FOREIGN KEY fk_user_user_group;

ALTER TABLE user DROP COLUMN group_name;

DROP TABLE IF EXISTS user_group;
