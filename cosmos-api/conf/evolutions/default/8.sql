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
