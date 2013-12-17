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
