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

# First database schema

# --- !Ups

CREATE TABLE user (
  cosmos_id INT NOT NULL AUTO_INCREMENT,
  user_id VARCHAR(45) NULL,
  handle VARCHAR(45) NOT NULL,
  api_key CHAR(40) NOT NULL,
  api_secret CHAR(40) NOT NULL,
  PRIMARY KEY (cosmos_id),
  UNIQUE INDEX tu_id_UNIQUE (user_id ASC)
);

CREATE TABLE public_key (
  cosmos_id INT NOT NULL,
  name VARCHAR(45) NOT NULL,
  signature TEXT NOT NULL,
  PRIMARY KEY (cosmos_id, name),
  CONSTRAINT fk_public_key_user
    FOREIGN KEY (cosmos_id)
    REFERENCES user (cosmos_id)
);

# --- !Downs

DROP TABLE IF EXISTS public_key;
DROP TABLE IF EXISTS user;
