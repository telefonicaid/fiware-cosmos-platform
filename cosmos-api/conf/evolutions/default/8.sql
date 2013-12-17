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

-- * Add user capabilities table

# --- !Ups

CREATE TABLE user_capability (
  name VARCHAR(45) NOT NULL,
  cosmos_id INT NOT NULL,
  PRIMARY KEY (name, cosmos_id),
  CONSTRAINT fk_user_capacity_user
    FOREIGN KEY (cosmos_id) REFERENCES user (cosmos_id)
);

# --- !Downs

DROP TABLE IF EXISTS user_capability;
