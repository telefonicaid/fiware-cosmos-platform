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

# First database schema

# --- !Ups

CREATE TABLE user (
  cosmos_id INT NOT NULL AUTO_INCREMENT,
  user_id VARCHAR(45) NULL,
  handle VARCHAR(45) NOT NULL,
  api_key CHAR(20) NOT NULL,
  api_secret CHAR(40) NOT NULL,
  PRIMARY KEY (cosmos_id),
  UNIQUE INDEX tu_id_UNIQUE (user_id ASC),
  UNIQUE INDEX api_key_UNIQUE (api_key ASC)
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

CREATE TABLE cluster (
  cluster_id VARCHAR(45) NOT NULL,
  owner INT NOT NULL,
  PRIMARY KEY (cluster_id),
  INDEX owner_INDEX (owner ASC),
  CONSTRAINT fk_public_key_owner
    FOREIGN KEY (owner)
    REFERENCES user (cosmos_id)
);

# --- !Downs

DROP TABLE IF EXISTS public_key;
DROP TABLE IF EXISTS user;
